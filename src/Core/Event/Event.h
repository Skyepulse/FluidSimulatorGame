#pragma once

#include <memory>
#include "../Log.h"

#define CORE_BIND_EVENT_METHOD(className, eventMethodName) std::bind(&className::eventMethodName, this, std::placeholders::_1)

#define CORE_CREATE_EVENT_CATEGORY_METHODS(eventCategory)                                                         \
  virtual EventCategory GetEventCategory() const override { return eventCategory; }                               \
  virtual bool IsEventCategory(EventCategory category) const override { return eventCategory == category; }       \

#define CORE_CREATE_EVENT_TYPE_METHODS(eventType)                                                                 \
  static EventType GetStaticType() { return eventType; }                                                          \
  virtual EventType GetEventType() const override { return GetStaticType(); }                                     \
  virtual bool IsEventType(EventType type) const override { return type == eventType; }                           \
  virtual const char* GetName() const { return #eventType; }                                                      \

/// @brief  TODO : CHANGE CATEGORIES TO FLAGS
enum class EventCategory
{
  None = 0, WindowEventCategory, KeyboardEventCategory, MouseEventCategory, MouseInputEventCategory, MouseMovementEventCategory,
};

enum class EventType
{
  None = 0     ,
  WindowResized, WindowClosed ,
  KeyInputEvent, KeyPressed   , KeyReleased  , KeyTyped  ,
  MousePressed , MouseReleased, MouseTyped, 
  MouseMoved   ,  
};

inline EventType operator|(EventType a, EventType b)
{
  return static_cast<EventType>(static_cast<int>(a) | static_cast<int>(b));
}

class Event
{
public:
  Event() : Handled(false) {}

  virtual EventCategory GetEventCategory() const = 0;                           
  virtual bool IsEventCategory(EventCategory category) const = 0; 
                                                                                                              
  virtual EventType GetEventType() const  = 0;                         
  virtual bool IsEventType(EventType type) const = 0; 

  virtual const char* GetName() const = 0;
  virtual std::string ToString() const = 0;

public:
  bool Handled;
};

class EventDispatcher
{
  template <typename T>
  using EventMethod = std::function<bool(T&)>;
public:
  EventDispatcher(Event& event) : m_Event(event) {}

  template <typename T>
  bool Dispatch(EventMethod<T> eventMethod)
  {
    if(m_Event.IsEventType(T::GetStaticType()))
    {
      m_Event.Handled = eventMethod(dynamic_cast<T&>(m_Event));
      return true;
    }

    return false;
  }

private:
  Event& m_Event;
};