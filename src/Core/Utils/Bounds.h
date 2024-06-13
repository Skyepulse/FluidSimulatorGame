#pragma once

#include <glm/glm.hpp>

class Bound
{
public:
    Bound(const glm::vec2& minCorner = glm::vec2(0), const glm::vec2& maxCorner = glm::vec2(0));

    bool InBound(glm::vec2 p) const;
    bool Includes(const Bound& bound) const;

public:
    glm::vec2 MaxCorner;
    glm::vec2 MinCorner;
};