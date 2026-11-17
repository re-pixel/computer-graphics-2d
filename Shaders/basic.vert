#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
layout(location = 2) in vec2 inTex;
out vec4 chCol;
out vec2 chTex;

uniform vec2 uPos;
uniform float uS;
uniform float uAspect;

void main()
{
    vec2 pos = inPos * uS + uPos;
    gl_Position = vec4(pos.x / uAspect, pos.y, 0.0, 1.0);
    chCol = inCol;
    chTex = inTex;
}
