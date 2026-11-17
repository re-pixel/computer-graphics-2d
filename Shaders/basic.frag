#version 330 core

in vec4 chCol;
in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform bool useTexture;
uniform vec4 uTint;

void main()
{
    if (useTexture) outCol = texture(uTex, chTex) * uTint;
    else outCol = chCol * uTint;
}
