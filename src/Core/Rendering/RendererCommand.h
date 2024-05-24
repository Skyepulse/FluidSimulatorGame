#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

class RendererCommand
{
public:
  static void Clear() { glClear(GL_COLOR_BUFFER_BIT); }
  static void ClearColor(float r = 0.3f, float g = 0.4f, float b = 0.4f, float a = 1.0f) { glClearColor(r, g, b, a); }
  static void EnableBlending(bool enabled) { enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND); }
  static void SetViewportSize(const glm::ivec2& size) { glViewport(0, 0, size.x, size.y); }
};