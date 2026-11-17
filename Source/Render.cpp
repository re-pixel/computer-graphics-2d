#define _USE_MATH_DEFINES
#include <cmath>

#include "../Header/Render.h"
#include "../Header/Util.h"

void preprocessTexture(unsigned& texture, const char* filepath) {
    texture = loadImageToTexture(filepath);
    glBindTexture(GL_TEXTURE_2D, texture);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int formVAO(const std::vector<float>& vertices) {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    return VAO;
}

static void pushVertex(std::vector<float>& v, float x, float y, float r, float g, float b, float a, float s, float t) {
    v.insert(v.end(), { x, y, r, g, b, a, s, t });
}

std::vector<float> rectVertices(float width, float height, float r, float g, float b, float a) {
    float w = width / 2, h = height / 2;
    std::vector<float> v;
    pushVertex(v, -w,  h, r, g, b, a, 0.0f, 1.0f);
    pushVertex(v, -w, -h, r, g, b, a, 0.0f, 0.0f);
    pushVertex(v,  w, -h, r, g, b, a, 1.0f, 0.0f);
    pushVertex(v,  w,  h, r, g, b, a, 1.0f, 1.0f);
    return v;
}

std::vector<float> circleVertices(float radius, int slices, float r, float g, float b, float a) {
    std::vector<float> v;
    pushVertex(v, 0.0f, 0.0f, r, g, b, a, 0.5f, 0.5f);
    for (int i = 1; i < slices + 2; ++i) {
        float angle = i * 2 * M_PI / slices;
        pushVertex(v, cos(angle) * radius, sin(angle) * radius, r, g, b, a, 0.0f, 0.0f);
    }
    return v;
}

std::vector<float> ringVertices(float rInner, float rOuter, int slices, float r, float g, float b, float a) {
    std::vector<float> v;
    for (int i = 0; i <= slices; ++i) {
        float angle = i * 2 * M_PI / slices;
        pushVertex(v, cos(angle) * rInner, sin(angle) * rInner, r, g, b, a, 0.0f, 0.0f);
        pushVertex(v, cos(angle) * rOuter, sin(angle) * rOuter, r, g, b, a, 0.0f, 0.0f);
    }
    return v;
}

void setUniforms(unsigned int shader, float x, float y, float s, bool useTexture, float r, float g, float b, float a) {
    glUniform2f(glGetUniformLocation(shader, "uPos"), x, y);
    glUniform1f(glGetUniformLocation(shader, "uS"), s);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), useTexture);
    glUniform4f(glGetUniformLocation(shader, "uTint"), r, g, b, a);
}
