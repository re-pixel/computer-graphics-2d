#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <vector>

#include "../Header/Util.h"
#include "../Header/Render.h"
#include "../Header/Clothes.h"

#define NUM_SLICES 60

const double FPS = 75.0;

const float MACHINE_X = 0.2f;
const float DRUM_X = MACHINE_X;
const float DRUM_Y = -0.15f;
const float DRUM_R = 0.45f;

const float DOOR_OPEN_X = DRUM_X + 0.95f;
const float DOOR_CLOSED_X = DRUM_X;
const float DOOR_ANIM_TIME = 1.0f;

const float CLOTH_SIZE = 0.34f;
const float CLOTH_DRUM_S = 0.75f;
const float PATH_R = DRUM_R - CLOTH_SIZE / 2 * CLOTH_DRUM_S - 0.02f;

const float ANGULAR_ACC = 1.5f;
const float MAX_OMEGA = 3.5f;
const float FULL_SPEED_TIME = 20.0f;

enum State { LOADING, CLOSING, WASHING, OPENING, DONE };
State state = LOADING;

int screenWidth = 800;
int screenHeight = 800;
float aspect = 1.0f;

std::vector<Clothing> clothes;
int dragged = -1;
float dragOffsetX = 0, dragOffsetY = 0;
int clothesInMachine = 0;

double stateStartTime = 0.0;
float doorX = DOOR_OPEN_X;
float omega = 0.0f;
float pinkAmount = 0.0f;
bool redInMachine = false;

GLFWcursor* cursor;

void getCursorWorld(GLFWwindow* window, float& x, float& y) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    x = ((xpos / screenWidth) * 2 - 1) * aspect;
    y = -((ypos / screenHeight) * 2 - 1);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) && action == GLFW_PRESS) {
        if (state == LOADING && clothesInMachine > 0 && dragged == -1) {
            state = CLOSING;
            stateStartTime = glfwGetTime();
        }
    }
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT || state != LOADING) return;

    float mx, my;
    getCursorWorld(window, mx, my);

    if (action == GLFW_PRESS) {
        for (int i = (int)clothes.size() - 1; i >= 0; --i) {
            Clothing& c = clothes[i];
            if (c.inMachine) continue;
            if (fabs(mx - c.x) < CLOTH_SIZE / 2 && fabs(my - c.y) < CLOTH_SIZE / 2) {
                dragged = i;
                dragOffsetX = c.x - mx;
                dragOffsetY = c.y - my;
                break;
            }
        }
    }
    if (action == GLFW_RELEASE && dragged != -1) {
        Clothing& c = clothes[dragged];
        float dx = c.x - DRUM_X, dy = c.y - DRUM_Y;
        if (sqrt(dx * dx + dy * dy) < DRUM_R) {
            placeInDrum(c, clothesInMachine, DRUM_X, DRUM_Y, PATH_R);
            clothesInMachine++;
            if (c.isRed) redInMachine = true;
        }
        dragged = -1;
    }
}

void updateState(double now, float dt) {
    float t = now - stateStartTime;

    if (state == CLOSING) {
        float k = std::min(t / DOOR_ANIM_TIME, 1.0f);
        doorX = DOOR_OPEN_X + (DOOR_CLOSED_X - DOOR_OPEN_X) * k;
        if (k >= 1.0f) {
            state = WASHING;
            stateStartTime = now;
        }
    }
    else if (state == WASHING) {
        float accelTime = MAX_OMEGA / ANGULAR_ACC;
        float totalTime = accelTime + FULL_SPEED_TIME;
        omega = std::min(ANGULAR_ACC * t, MAX_OMEGA);

        for (Clothing& c : clothes)
            if (c.inMachine) updateClothing(c, omega, dt, DRUM_X, DRUM_Y, PATH_R);

        if (redInMachine) pinkAmount = std::min(t / totalTime, 1.0f);

        if (t >= totalTime) {
            omega = 0.0f;
            state = OPENING;
            stateStartTime = now;
        }
    }
    else if (state == OPENING) {
        float k = std::min(t / DOOR_ANIM_TIME, 1.0f);
        doorX = DOOR_CLOSED_X + (DOOR_OPEN_X - DOOR_CLOSED_X) * k;
        if (k >= 1.0f) state = DONE;
    }
}

void drawShape(unsigned int shader, unsigned int VAO, GLenum mode, int count, float x, float y, float s = 1.0f, float a = 1.0f) {
    setUniforms(shader, x, y, s, false, 1, 1, 1, a);
    glBindVertexArray(VAO);
    glDrawArrays(mode, 0, count);
}

void drawTextured(unsigned int shader, unsigned int VAO, unsigned int texture, float x, float y, float s, float r, float g, float b, float a) {
    setUniforms(shader, x, y, s, true, r, g, b, a);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawClothing(unsigned int shader, unsigned int VAOcloth, const Clothing& c) {
    float r = 1, g = 1, b = 1;
    if (c.isRed) {
        r = 0.9f; g = 0.12f; b = 0.12f;
    }
    else if (c.inMachine) {
        g = 1.0f - 0.45f * pinkAmount;
        b = 1.0f - 0.25f * pinkAmount;
    }
    float s = c.inMachine ? CLOTH_DRUM_S : 1.0f;
    drawTextured(shader, VAOcloth, c.texture, c.x, c.y, s, r, g, b, 1.0f);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    screenWidth = mode->width;
    screenHeight = mode->height;
    aspect = (float)screenWidth / screenHeight;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Ves masina", monitor, NULL);
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);

    cursor = loadImageToCursor("Resources/kursor.png");
    glfwSetCursor(window, cursor);

    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int basicShader = createShader("Shaders/basic.vert", "Shaders/basic.frag");
    glUseProgram(basicShader);
    glUniform1i(glGetUniformLocation(basicShader, "uTex"), 0);
    glUniform1f(glGetUniformLocation(basicShader, "uAspect"), aspect);

    unsigned shirtTex, pantsTex, towelTex, sockTex, signatureTex;
    preprocessTexture(shirtTex, "Resources/majica.png");
    preprocessTexture(pantsTex, "Resources/pantalone.png");
    preprocessTexture(towelTex, "Resources/peskir.png");
    preprocessTexture(sockTex, "Resources/carapa.png");
    preprocessTexture(signatureTex, "Resources/potpis.png");

    unsigned int VAObody = formVAO(rectVertices(1.3f, 1.6f, 0.92f, 0.92f, 0.94f, 1.0f));
    unsigned int VAOpanel = formVAO(rectVertices(1.3f, 0.25f, 0.75f, 0.77f, 0.8f, 1.0f));
    unsigned int VAOdisplay = formVAO(rectVertices(0.4f, 0.1f, 0.1f, 0.3f, 0.2f, 1.0f));
    unsigned int VAOknob = formVAO(circleVertices(0.08f, NUM_SLICES, 0.5f, 0.5f, 0.55f, 1.0f));
    unsigned int VAOdrum = formVAO(circleVertices(DRUM_R, NUM_SLICES, 0.22f, 0.22f, 0.25f, 1.0f));
    unsigned int VAOgasket = formVAO(ringVertices(DRUM_R, DRUM_R + 0.06f, NUM_SLICES, 0.6f, 0.6f, 0.62f, 1.0f));
    unsigned int VAOdoorFrame = formVAO(ringVertices(DRUM_R - 0.03f, DRUM_R + 0.1f, NUM_SLICES, 0.78f, 0.8f, 0.84f, 1.0f));
    unsigned int VAOdoorGlass = formVAO(circleVertices(DRUM_R - 0.03f, NUM_SLICES, 0.25f, 0.25f, 0.27f, 0.55f));
    unsigned int VAOcloth = formVAO(rectVertices(CLOTH_SIZE, CLOTH_SIZE, 1, 1, 1, 1));
    unsigned int VAOsignature = formVAO(rectVertices(0.8f, 0.2f, 1, 1, 1, 1));

    float leftX = -aspect + 0.35f;
    unsigned textures[] = { shirtTex, pantsTex, towelTex, sockTex };
    float ys[] = { 0.6f, 0.2f, -0.2f, -0.6f };
    for (int i = 0; i < 4; ++i) {
        Clothing c{};
        c.texture = textures[i];
        c.startX = c.x = leftX;
        c.startY = c.y = ys[i];
        c.isRed = (i == 3);
        clothes.push_back(c);
    }

    glClearColor(0.78f, 0.86f, 0.9f, 1.0f);

    int ringCount = 2 * (NUM_SLICES + 1);
    int circleCount = NUM_SLICES + 2;
    double lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double initFrameTime = glfwGetTime();
        float dt = initFrameTime - lastFrameTime;
        lastFrameTime = initFrameTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        if (dragged != -1) {
            float mx, my;
            getCursorWorld(window, mx, my);
            clothes[dragged].x = mx + dragOffsetX;
            clothes[dragged].y = my + dragOffsetY;
        }

        updateState(initFrameTime, dt);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(basicShader);

        drawShape(basicShader, VAObody, GL_TRIANGLE_FAN, 4, MACHINE_X, -0.05f);
        drawShape(basicShader, VAOpanel, GL_TRIANGLE_FAN, 4, MACHINE_X, 0.625f);
        drawShape(basicShader, VAOdisplay, GL_TRIANGLE_FAN, 4, MACHINE_X - 0.2f, 0.625f);
        drawShape(basicShader, VAOknob, GL_TRIANGLE_FAN, circleCount, MACHINE_X + 0.4f, 0.625f);
        drawShape(basicShader, VAOdrum, GL_TRIANGLE_FAN, circleCount, DRUM_X, DRUM_Y);
        drawShape(basicShader, VAOgasket, GL_TRIANGLE_STRIP, ringCount, DRUM_X, DRUM_Y);

        for (const Clothing& c : clothes)
            if (c.inMachine) drawClothing(basicShader, VAOcloth, c);

        drawShape(basicShader, VAOdoorGlass, GL_TRIANGLE_FAN, circleCount, doorX, DRUM_Y);
        drawShape(basicShader, VAOdoorFrame, GL_TRIANGLE_STRIP, ringCount, doorX, DRUM_Y);

        if (state == LOADING) {
            for (int i = 0; i < (int)clothes.size(); ++i)
                if (!clothes[i].inMachine && i != dragged) drawClothing(basicShader, VAOcloth, clothes[i]);
            if (dragged != -1) drawClothing(basicShader, VAOcloth, clothes[dragged]);
        }

        drawTextured(basicShader, VAOsignature, signatureTex, aspect - 0.45f, -0.88f, 1.0f, 1, 1, 1, 0.75f);

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - initFrameTime < 1 / FPS) {}
    }

    glDeleteProgram(basicShader);
    glfwDestroyCursor(cursor);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
