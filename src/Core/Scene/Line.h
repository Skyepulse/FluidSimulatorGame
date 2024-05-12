#pragma once

#include "Shape.h"

#include <glad/glad.h>

class Line : public Shape
{
public:
  Line();
  ~Line();
private:
  // The data is not static because we want to avoid bugs where one line modifies its ShapeInfos and all of them are modified
  ShapeInfos m_ShapeInfos;
};