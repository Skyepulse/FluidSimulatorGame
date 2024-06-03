#include "Layer.h"

#include <algorithm>

#include <iostream>

LayerStack::LayerStack()
{
    m_OverlayStartIndex = 0;
}

LayerStack::~LayerStack()
{
    for(auto& layer : m_Layers)
    {
        layer->OnDetach();
        delete layer;
    }
}

void LayerStack::PushLayer(Layer *layer)
{
    if(std::find(m_Layers.begin(), m_Layers.end(), layer) == m_Layers.end())
    {
        m_Layers.emplace(m_Layers.begin() + m_OverlayStartIndex, layer);
        layer->OnAttach();
        
        m_OverlayStartIndex++;
        return;
    }
    
    CORE_INFO("Layer named {} already in LayerStack", layer->GetName())
}

void LayerStack::PopLayer(Layer *layer)
{
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if(it != m_Layers.end())
    {
        (*it)->OnDetach();
        m_Layers.erase(it);

        m_OverlayStartIndex--;
        return;
    }

    CORE_INFO("No layer named {} in LayerStack", layer->GetName())
}

void LayerStack::PushOverlay(Layer *overlay)
{
    if(std::find(m_Layers.begin(), m_Layers.end(), overlay) == m_Layers.end())
    {
        m_Layers.push_back(overlay);
        overlay->OnAttach();

        return;
    }

    CORE_INFO("Layer named {} already in LayerStack", overlay->GetName())
}

void LayerStack::PopOverlay(Layer *layer)
{
    auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if(it != m_Layers.end())
    {
        (*it)->OnDetach();
        m_Layers.erase(it);
        return;
    }

    CORE_INFO("No layer named {} in LayerStack", layer->GetName())
}
