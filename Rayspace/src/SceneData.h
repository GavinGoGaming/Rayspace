#include <rapidjson/document.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace rapidjson;

namespace SceneData {
    class Datafile {
    public:
        // ... (other functions)

        const char* GetStringValue(const char* key) const {
            if (!document) {
                // Handle the case where the document is not initialized.
                return nullptr;
            }

            if (document->HasMember(key) && (*document)[key].IsString()) {
                return (*document)[key].GetString();
            }

            // Handle the case where the key is not found or the value is not a string.
            return nullptr;
        }

        void SetIntValue(const char* key, int tovalue) {
            if (!document) {
                // Handle the case where the document is not initialized.
                return;
            }

            Value::AllocatorType& allocator = document->GetAllocator();

            if (!document->HasMember(key)) {
                // If the key does not exist, add a new key-value pair.
                Value newKey(kStringType);
                newKey.SetString(key, strlen(key), allocator);
                document->AddMember(newKey, Value().SetInt(tovalue), allocator);
            }
            else {
                // If the key exists, update its value.
                Value& existingValue = (*document)[key];
                existingValue.SetInt(tovalue);
            }
        }

        int GetIntValue(const char* key) const {
            if (!document) {
                // Handle the case where the document is not initialized.
                return 0; // Default value, you can choose another default value if needed.
            }

            if (document->HasMember(key) && (*document)[key].IsInt()) {
                return (*document)[key].GetInt();
            }

            // Handle the case where the key is not found or the value is not an int.
            return 0; // Default value, you can choose another default value if needed.
        }
        void SetDocument(Document* doc) {
            document = doc;
        }

    private:
        Document* document = nullptr;
    };

    static char* ReadFileContentAsString(const char* filename) {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return nullptr;
        }

        // Read the file content into a stringstream
        std::ostringstream contentStream;
        contentStream << file.rdbuf();
        std::string content = contentStream.str();

        // Allocate memory for the C-string and copy the content
        char* result = new char[content.size() + 1];
        std::strcpy(result, content.c_str());

        file.close();

        return result;
    }

    static void MakeNewFile(const char* filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return;
        }

        // Sample JSON content (customize as needed)
        const char* jsonContent = R"({
            "name": "Sample Scene",
            "objects": []
        })";

        // Write the JSON content to the file
        file << jsonContent;

        file.close();

        std::cout << "File created successfully: " << filename << std::endl;
    }

    static Datafile ReadJSON(const char* jsonData) {
        Document doc;
        doc.Parse(jsonData);
        Datafile datafile;
        datafile.SetDocument(&doc);
        return datafile;
    }

    static Datafile ReadJSONFromFile(const char* filename) {
        char* fileContent = ReadFileContentAsString(filename);
        Datafile datafile = ReadJSON(fileContent);
        delete[] fileContent; // Remember to free the allocated memory
        return datafile;
    }
}
