#pragma once

#include "Walnut/Image.h"
#include <glm/glm.hpp>
#include <memory>
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

struct HitPayload {
	float HitDistance;
	glm::vec3 WorldNormal;
	glm::vec3 WorldPosition;

	int ObjectIndex;
};

class Renderer {
public:
	struct Settings
	{
		bool Accumulate = true;
		bool SlowRandom = false;
		bool UseSkybox = true;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }
private:
	glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen shader
	
	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float HitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
};