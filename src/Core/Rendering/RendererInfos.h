#pragma once

#include <memory>

#include "../Core.h"

#include "../OpenGL/Shader.h"
#include "../OpenGL/VertexArray.h"
#include "../OpenGL/Texture.h"

class RenderProperty
{
public:
  /// @brief Set the propertie to the given shader. Warning : Asserts that the shader is bound !
  /// @param shader 
  virtual void SetProperties(std::shared_ptr<Shader> shader) const = 0;
};

class TextureRenderProperty : public RenderProperty
{
public:
  /// @brief Create a Texture render properties that will be upload to the shader by calling Setproperties.
  /// @attention The data is a pointer, meaning that it must not been freed before using it in SetProperties
  /// @attention Data freeing is not handle by the Render property
  /// @param texture 
  TextureRenderProperty(const Texture* texture) : m_Texture(texture) {}
  virtual void SetProperties(std::shared_ptr<Shader> shader) const override
  {
    // Assert that the data used is not null
    CORE_ASSERT(m_Texture, "The Texture in the RenderProperty is null, you must not free it while having a RenderProperty holding a pointer to it");
    
    shader->SetInt("u_UseTexture", 1);
    // TEMPORARY ------------------ //
    shader->SetInt("u_Texture", 0); // => TODO change for multiple texture binding support
    // ---------------------------- // 
  }
private:
  const Texture* const m_Texture;
};

class ColorRenderProperty : public RenderProperty
{
public:
  /// @brief Create a Color render properties that will be upload to the shader by calling Setproperties.
  /// @attention The data is a pointer, meaning that it must not been freed before using it in SetProperties
  /// @attention Data freeing is not handle by the Render property
  /// @param color 
  ColorRenderProperty(const glm::vec3* color) : m_Color(color) {}
  virtual void SetProperties(std::shared_ptr<Shader> shader) const override
  {
    // Assert that the data used is not null
    CORE_ASSERT(m_Color, "The glm::vec3 pointer in the RenderProperty is null, you must not free it while having a RenderProperty holding a pointer to it");
    
    shader->SetFloat3("u_Color", *m_Color);
  }
private:
  const glm::vec3* const m_Color;
};

struct RendererData
{
  mutable std::shared_ptr<Shader> shader;
  VertexArray vertexArray;
  GLenum renderMode;

  std::vector<std::shared_ptr<RenderProperty>> renderProperties;
};
