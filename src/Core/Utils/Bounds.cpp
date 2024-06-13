#include "Bounds.h"

#include "../Log.h"

Bound::Bound(const glm::vec2& minCorner, const glm::vec2& maxCorner)
    : MinCorner(minCorner), MaxCorner(maxCorner)
{

}

bool Bound::InBound(glm::vec2 p) const
{
    return p.x < MaxCorner.x && p.x > MinCorner.x && p.y < MaxCorner.y && p.y > MinCorner.y;
}

bool Bound::Includes(const Bound& bound) const
{
    CORE_DEBUG("This Bound : Max({}, {}), Min({}, {})", MaxCorner.x, MaxCorner.y, MinCorner.x, MinCorner.y);
    CORE_DEBUG("Bound tested : Max({}, {}), Min({}, {})", bound.MaxCorner.x, bound.MaxCorner.y, bound.MinCorner.x, bound.MinCorner.y);
    CORE_DEBUG("Max corner is in bound : {}", InBound(bound.MaxCorner));
    CORE_DEBUG("Min corner is in bound : {}", InBound(bound.MinCorner));

    return InBound(bound.MaxCorner) && InBound(bound.MinCorner);
}
