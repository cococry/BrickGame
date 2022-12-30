#pragma once
#include <GLFW/glfw3.h>


struct glfwKeyboardStruct {
    bool keys[GLFW_KEY_LAST];
    bool keysChanged[GLFW_KEY_LAST];
};

struct glfwMouseStruct {
    bool activeButtons[GLFW_KEY_LAST];
    bool changedButtons[GLFW_KEY_LAST];

    double ypos = 0, xpos = 0;
    double scrollDeltaX = 0, scrollDeltaY = 0;
    double lastXpos = 0, lastYpos = 0;
    double xposDelta = 0, yposDelta = 0;

    bool firstMouse = true;
};
class Input {
public:
    static bool keyWentDown(uint32_t key);

    static bool isKeyDown(uint32_t key);

    static bool isKeyReleased(uint32_t key);

    static bool keyChanged(uint32_t key);

    static bool mouseButtonWentDown(uint32_t button);

    static bool isMouseButtonDown(uint32_t button);

    static bool isMouseButtonReleased(uint32_t button);

    static bool mouseButtonChanged(uint32_t buton);

    static double getMouseX();

    static double getMouseY();

    static double getMouseScrollX();

    static double getMouseScrollY();

    static double getMouseXDelta();

    static double getMouseYDelta();

    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void glfwButtonCallback(GLFWwindow* window, int button, int action, int mods);

    static void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static void glfwCursorCallback(GLFWwindow* window, double xpos, double ypos);


    static glfwKeyboardStruct mKeyboard;
    static glfwMouseStruct mMouse;
};