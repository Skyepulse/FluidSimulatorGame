#pragma once

#include "Shape.h"

#include <glad/glad.h>

class Circle : public Shape
{
public:
  Circle();
  ~Circle();

private:
  ShapeInfos m_ShapeInfos;
};