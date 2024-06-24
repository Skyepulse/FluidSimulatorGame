#include "Bounds.h"

Bound::Bound(const glm::vec2& minCorner, const glm::vec2& maxCorner)
    : MinCorner(minCorner), MaxCorner(maxCorner)
{

}

Bound::Bound(const glm::vec2& maxCorner)
    : MinCorner(glm::vec2(0)), MaxCorner(maxCorner)
{

}

bool Bound::InBound(glm::vec2 p) const
{
    return p.x <= MaxCorner.x && p.x >= MinCorner.x && p.y <= MaxCorner.y && p.y >= MinCorner.y;
}

bool Bound::Includes(const Bound& bound) const
{
   /* CORE_DEBUG("This Bound : Max({}, {}), Min({}, {})", MaxCorner.x, MaxCorner.y, MinCorner.x, MinCorner.y);
    CORE_DEBUG("Bound tested : Max({}, {}), Min({}, {})", bound.MaxCorner.x, bound.MaxCorner.y, bound.MinCorner.x, bound.MinCorner.y);
    CORE_DEBUG("Max corner is in bound : {}", InBound(bound.MaxCorner));
    CORE_DEBUG("Min corner is in bound : {}", InBound(bound.MinCorner));*/

    return InBound(bound.MaxCorner) && InBound(bound.MinCorner);
}

void Bound::Translate(const glm::vec2 translation)
{
    MinCorner += translation; 
    MaxCorner += translation;
}

Bound Bound::Intersection(const Bound& b1, const Bound& b2)
{
    glm::vec2 minCorner = glm::vec2(glm::max(b1.MinCorner.x, b2.MinCorner.x), glm::max(b1.MinCorner.y, b2.MinCorner.y));
    glm::vec2 maxCorner = glm::vec2(glm::min(b1.MaxCorner.x, b2.MaxCorner.x), glm::min(b1.MaxCorner.y, b2.MaxCorner.y));

    return Bound(minCorner, maxCorner);
}
