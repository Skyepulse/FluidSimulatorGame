#pragma once

#include "Camera.h"
#include "Transform.h"

#include "../Event/Event.h"
#include "../Event/ApplicationEvent.h"

class CameraController
{
public:
	CameraController(float aspectRatio, float zoomLevel, float cameraSpeed, Bound bound);
	CameraController(float aspectRatio, float zoomLevel, float cameraSpeed);
	~CameraController();

	bool OnEvent(Event& e);
	void SetCameraMovementBound(const Bound bound);
	void SetCameraPosition(const glm::vec2& position);
	void AdaptZoomLevel(const Bound& levelBound);

	float const GetAspectRatio() const { return m_AspectRatio; }
	float const GetZoomLevel() const { return m_ZoomLevel; }
	glm::vec2 GetCenterBound();

	std::shared_ptr<Camera> GetCamera() const { return m_Camera; }
private:
	bool OnWindowResized(WindowResizedEvent& e);
private:
	std::shared_ptr<Camera> m_Camera;
	float m_CameraSpeed;
	float m_AspectRatio;
	float m_ZoomLevel;

	Bound m_LimitBound;
	glm::vec2 m_CameraPosition;
 };