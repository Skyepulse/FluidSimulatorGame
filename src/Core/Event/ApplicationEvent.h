#pragma once

#include "Event.h"

#include <glm/glm.hpp>

#include <sstream>
#include <string>

// -------------------------------------------------------------------------
//                                KEYBOARD EVENT
// -------------------------------------------------------------------------

class KeyEvent : public Event
{
public:
  KeyEvent(int keyCode) : Event(), m_KeyCode(keyCode) {}
  int GetKey() { return m_KeyCode; }

  CORE_CREATE_EVENT_CATEGORY_METHODS(EventCategory::KeyboardEventCategory)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << "[KeyEvent] Key " << m_KeyCode;
    return ss.str(); 
  } 
private:
  int m_KeyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
  KeyPressedEvent(int keyCode) : KeyEvent(keyCode) {}

  CORE_CREATE_EVENT_TYPE_METHODS(EventType::KeyPressed)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << KeyEvent::ToString() << " pressed";
    return ss.str(); 
  } 
};

class KeyReleasedEvent : public KeyEvent
{
public:
  KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

  CORE_CREATE_EVENT_TYPE_METHODS(EventType::KeyReleased)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << KeyEvent::ToString() << " released";
    return ss.str(); 
  } 
};

class KeyTypedEvent : public KeyEvent
{
public:
  KeyTypedEvent(int keyCode) : KeyEvent(keyCode) {}

  CORE_CREATE_EVENT_TYPE_METHODS(EventType::KeyTyped)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << KeyEvent::ToString() << " typed";
    return ss.str(); 
  } 
};

// -------------------------------------------------------------------------
//                                MOUSE EVENT
// -------------------------------------------------------------------------

class MouseEvent : public Event
{
public:
  MouseEvent(double mouseX, double mouseY) : Event(), m_MousePosition(glm::vec2(mouseX, mouseY)) {}
  double GetMouseX() { return m_MousePosition.x; }
  double GetMouseY() { return m_MousePosition.y; }
  glm::vec2 GetMousePosition() const { return m_MousePosition; }

  CORE_CREATE_EVENT_CATEGORY_METHODS(EventCategory::MouseEventCategory)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << "[MouseEvent] Position : (" << m_MousePosition.x << ", " << m_MousePosition.y << ")";
    return ss.str(); 
  } 
private:
  glm::vec2 m_MousePosition;
};

class MouseMovedEvent : public MouseEvent
{
public:
  MouseMovedEvent(double mouseX, double mouseY) : MouseEvent(mouseX, mouseY) {}
  
  CORE_CREATE_EVENT_TYPE_METHODS(EventType::MouseMoved)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << MouseEvent::ToString() << " Mouse moved";
    return ss.str(); 
  } 
};

class MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(const double xOffset, const double yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset) { }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "MouseScrolled: " << "(" << m_XOffset << "," << m_YOffset << ")";
        return ss.str();
    }

    double GetXOffset() const { return m_XOffset; }
    double GetYOffset() const { return m_YOffset; }

    CORE_CREATE_EVENT_CATEGORY_METHODS(EventCategory::MouseEventCategory)
    CORE_CREATE_EVENT_TYPE_METHODS(EventType::MouseScrolled)

private:
    const double m_XOffset;
    const double m_YOffset;
};

class MouseInputEvent : public MouseEvent
{
public:
  MouseInputEvent(double mouseX, double mouseY, int mouseButton) : MouseEvent(mouseX, mouseY) {}
  int GetMouseButton() const { return m_MouseButton; }
  
  CORE_CREATE_EVENT_CATEGORY_METHODS(EventCategory::MouseInputEventCategory)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << MouseEvent::ToString() << "Mouse Input : {}" << m_MouseButton;
    return ss.str(); 
  } 
private:
  int m_MouseButton;
};

class MousePressedEvent : public MouseInputEvent
{
public:
  MousePressedEvent(double mouseX, double mouseY, int mouseButton) : MouseInputEvent(mouseX, mouseY, mouseButton) {}
  
  CORE_CREATE_EVENT_TYPE_METHODS(EventType::MousePressed)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << MouseEvent::ToString() << " pressed";
    return ss.str(); 
  } 
};

class MouseReleasedEvent : public MouseInputEvent
{
public:
  MouseReleasedEvent(double mouseX, double mouseY, int mouseButton) : MouseInputEvent(mouseX, mouseY, mouseButton) {}
  
  CORE_CREATE_EVENT_TYPE_METHODS(EventType::MouseReleased)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << MouseEvent::ToString() << " released";
    return ss.str(); 
  } 
};

class MouseTypedEvent : public MouseInputEvent
{
public:
  MouseTypedEvent(double mouseX, double mouseY, int mouseButton) : MouseInputEvent(mouseX, mouseY, mouseButton) {}
  
  CORE_CREATE_EVENT_TYPE_METHODS(EventType::MouseTyped)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << MouseEvent::ToString() << " typed";
    return ss.str(); 
  } 
};

// -------------------------------------------------------------------------
//                                WINDOW EVENTS
// -------------------------------------------------------------------------

class WindowResizedEvent : public Event
{
public:
  WindowResizedEvent(int width, int height) : Event(), m_Size(glm::ivec2(width, height)) {}
  const glm::ivec2& GetSize() { return m_Size; } 

  CORE_CREATE_EVENT_CATEGORY_METHODS(EventCategory::WindowEventCategory)
  CORE_CREATE_EVENT_TYPE_METHODS(EventType::WindowResized)
  virtual std::string ToString() const override 
  {
    std::stringstream ss;
    ss << "[WindowResizedEvent] Size : " << m_Size.x << ", " << m_Size.y;
    return ss.str(); 
  } 

private:
  glm::ivec2 m_Size;
};