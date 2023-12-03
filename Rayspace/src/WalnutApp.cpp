#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <chrono>
#include <glm/gtc/type_ptr.hpp>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		Material& pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };
		pinkSphere.Roughness = 0.0f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		blueSphere.Roughness = 0.1f;

		Material& orangeSphere = m_Scene.Materials.emplace_back();
		orangeSphere.Albedo = { 1.0f, 1.0f, 0.0f };
		orangeSphere.Roughness = 0.0f;
		orangeSphere.EmissionColor = { 1.0f, 0.692f, 0.235f };
		orangeSphere.EmissionPower = 20.0f;

		{
			Sphere sphere;
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 7.4f, 0.0f, -20.0f };
			sphere.Radius = 11.3f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
	}
	void PushToScene(int size, Sphere spheres[]) {
		for(int i = 0; i < size; i++)
		{
			m_Scene.Spheres.push_back(spheres[i]);
		}
	}
	void PushToScene(int size, Material materials[]) {
		for (int i = 0; i < size; i++)
		{
			m_Scene.Materials.push_back(materials[i]);
		}
	}
	virtual void OnUpdate(float ts) override {
		m_CameraMoving = m_Camera.OnUpdate(ts);
		if (m_CameraMoving)
			m_Renderer.ResetFrameIndex();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Panel");
		if (ImGui::Button("Render")) {
			Render();
		}
		if (ImGui::Button("Reset")) {
			m_Renderer.ResetFrameIndex();
		}
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Slow Random", &m_Renderer.GetSettings().SlowRandom);
		ImGui::Checkbox("Use Skybox", &m_Renderer.GetSettings().UseSkybox);
		if (ImGui::CollapsingHeader("Debug Info")) {
			ImGui::Text("Last render: %.3fms", m_LastRenderTime);
			ImGui::Text("Current FPS: %.0f", (1.0f / ImGui::GetIO().DeltaTime));
		}
		ImGui::End();
		ImGui::Begin("Scene");
		if (ImGui::CollapsingHeader("Spheres")) {
			ImGui::Text("Spheres: %.0f", (float)m_Scene.Spheres.size());
			ImGui::Separator();
			for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
			{
				ImGui::PushID((int)i);

				Sphere& sphere = m_Scene.Spheres[i];
				ImGui::Text("Sphere #%.0f", (float)i + 1);
				ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
				ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
				ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);

				ImGui::Separator();

				ImGui::PopID();
			}
		}
		if (ImGui::CollapsingHeader("Materials")) {
			ImGui::Text("Materials: %.0f", (float)m_Scene.Materials.size());
			ImGui::Separator();
			for (size_t i = 0; i < m_Scene.Materials.size(); i++)
			{
				Material& Mat = m_Scene.Materials[i];
				ImGui::PushID((int)i);

				ImGui::Text("Material #%.0f", (float)i + 1);
				ImGui::ColorEdit3("Albedo", glm::value_ptr(Mat.Albedo));
				ImGui::DragFloat("Roughness", &Mat.Roughness, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("Metallic", &Mat.Metallic, 0.05f, 0.0f, 1.0f);
				ImGui::ColorEdit3("Emission Color", glm::value_ptr(Mat.EmissionColor));
				ImGui::DragFloat("Emission Power", &Mat.EmissionPower, 0.05f, 0.0f, FLT_MAX);

				ImGui::Separator();
				ImGui::PopID();
			}
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;
		auto image = m_Renderer.GetFinalImage();
		if(image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, 
				ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}
	void Render() {
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);

		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
	glm::vec3 getRainbow(float secconds, float saturation, float brightness) {
		auto millisec_since_epoch =
			duration_cast<milliseconds>(system_clock::now().time_since_epoch())
			.count();
		long index = 0;
		float hue = ((millisec_since_epoch + index) % (int)(secconds * 1000)) / (float)(secconds * 1000);
		float r = 0, g = 0, b = 0;
		ImGui::ColorConvertHSVtoRGB(hue, saturation, brightness, r, g, b);
		return glm::vec3(r, g, b);
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	bool m_CameraMoving = false;
	float m_LastRenderTime = 0.0f, m_RainbowSpeed = 4.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Rayspace by Klash";
	spec.Height = 600;
	spec.Width = 1000;

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}