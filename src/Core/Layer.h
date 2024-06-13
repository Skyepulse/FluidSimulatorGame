#pragma once

#include "Log.h"
#include "Event/Event.h"

#include <vector>
#include <string>

class Layer
{
public:
    Layer(const std::string& name) : m_Name(name) {}

    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;

    virtual void Update() = 0;
    virtual bool OnEvent(Event& e) = 0;

    const std::string& GetName() {return m_Name; }
private:
    const std::string m_Name;
};


/// @brief The LayerStack contains two types of layers : basic Layers and Overlay
/// Each type has its own methods for Push/Pop.
/// The overlays will always be stored after every layer in the LayerStack
/// which means that if an overlay is pushed then a layer, the m_Layer vector
/// will contain at index 0 the layer, and at index 1 the overlay.
/// Thus, overlays are intended to be used for UI layers, and will always be rendered after every layer in the LayerStack.
class LayerStack
{
public:
    LayerStack();
    ~LayerStack();

    void PushLayer(Layer* layer);
    void PopLayer(Layer* layer);
    void PushOverlay(Layer* layer);
    void PopOverlay(Layer* layer);

    std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
    std::vector<Layer*>::iterator end() { return m_Layers.end(); }

    void TEST() { CORE_DEBUG("Start index : {}", m_OverlayStartIndex) for(auto& layer : m_Layers) { CORE_DEBUG("{}",layer->GetName()) } }
private:
    std::vector<Layer*> m_Layers;
    uint32_t m_OverlayStartIndex;

};