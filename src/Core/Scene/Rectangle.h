#pragma once

#include "Shape.h"

#include <glad/glad.h>

class Rectangle : public Shape
{
public:
  Rectangle();
  ~Rectangle();

private:
  ShapeInfos m_ShapeInfos;
};