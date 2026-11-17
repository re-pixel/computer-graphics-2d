#pragma once
#include <vector>

void preprocessTexture(unsigned& texture, const char* filepath);
unsigned int formVAO(const std::vector<float>& vertices);

std::vector<float> rectVertices(float width, float height, float r, float g, float b, float a);
std::vector<float> circleVertices(float radius, int slices, float r, float g, float b, float a);
std::vector<float> ringVertices(float rInner, float rOuter, int slices, float r, float g, float b, float a);

void setUniforms(unsigned int shader, float x, float y, float s, bool useTexture, float r, float g, float b, float a);
