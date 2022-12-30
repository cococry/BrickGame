#pragma once
#include "input.h"

glfwKeyboardStruct Input::mKeyboard;
glfwMouseStruct Input::mMouse;

bool Input::keyWentDown(uint32_t key) {
    return keyChanged(key) && mKeyboard.keys[key];
}

bool Input::isKeyDown(uint32_t key) {
    return mKeyboard.keys[key];
}

bool Input::isKeyReleased(uint32_t key) {
    return keyChanged(key) && !mKeyboard.keys[key];
}

bool Input::keyChanged(uint32_t key) {
    bool ret = mKeyboard.keysChanged[key];
    mKeyboard.keysChanged[key] = false;
    return ret;
}

bool Input::mouseButtonWentDown(uint32_t button) {
    return mouseButtonChanged(button) && mMouse.activeButtons[button];
}

bool Input::isMouseButtonDown(uint32_t button) {
    return mMouse.activeButtons[button];
}

bool Input::isMouseButtonReleased(uint32_t button) {
    return mouseButtonChanged(button) && !mMouse.activeButtons[button];
}

bool Input::mouseButtonChanged(uint32_t button) {
    bool ret = mMouse.changedButtons[button];
    mMouse.changedButtons[button] = false;
    return ret;
}

double Input::getMouseX() {
    return mMouse.xpos;
}

double Input::getMouseY() {
    return mMouse.ypos;
}

double Input::getMouseScrollX() {
    return mMouse.scrollDeltaX;
}

double Input::getMouseScrollY() {
    return mMouse.scrollDeltaY;
}

double Input::getMouseXDelta() {
    return mMouse.xposDelta;
}

double Input::getMouseYDelta() {
    return mMouse.yposDelta;
}

void Input::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_RELEASE) {
        if (!mKeyboard.keys[key]) {
            mKeyboard.keys[key] = true;
        }
    }
    else {
        mKeyboard.keys[key] = false;
    }

    mKeyboard.keysChanged[key] = (action != GLFW_REPEAT);

}

void Input::glfwButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action != GLFW_RELEASE) {
        if (!mMouse.activeButtons[button]) {
            mMouse.activeButtons[button] = true;
        }
    }
    else {
        mMouse.activeButtons[button] = false;
    }

    mMouse.changedButtons[button] = (action != GLFW_REPEAT);
 
}

void Input::glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    mMouse.scrollDeltaX = xoffset;
    mMouse.scrollDeltaY = yoffset;
}

void Input::glfwCursorCallback(GLFWwindow* window, double xpos, double ypos) {
    mMouse.xpos = xpos;
    mMouse.ypos = ypos;

    if (mMouse.firstMouse) {
        mMouse.lastXpos = xpos;
        mMouse.lastYpos = ypos;
        mMouse.firstMouse = false;
    }

    mMouse.xposDelta = mMouse.xpos - mMouse.lastXpos;
    mMouse.yposDelta = mMouse.ypos - mMouse.lastYpos;
    mMouse.lastXpos = xpos;
    mMouse.lastYpos = ypos;
}
