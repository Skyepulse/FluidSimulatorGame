#pragma once

#include "../Log.h"

#include <glm/glm.hpp>

class Bound
{
public:
    Bound(const glm::vec2& minCorner = glm::vec2(0), const glm::vec2& maxCorner = glm::vec2(0));

    bool InBound(glm::vec2 p) const;
    bool Includes(const Bound& bound) const;
    glm::vec2 GetSize() const { return MaxCorner - MinCorner; }

    void Translate(const glm::vec2 translation);

    static Bound Intersection(const Bound& b1, const Bound& b2);

    inline static void Debug(const Bound& b)
    {
        CORE_DEBUG("Bound :");
        CORE_DEBUG("MinCorner : {}, {}", b.MinCorner.x, b.MinCorner.y);
        CORE_DEBUG("MaxCorner : {}, {}", b.MaxCorner.x, b.MaxCorner.y);
    }

public:
    glm::vec2 MaxCorner;
    glm::vec2 MinCorner;
};