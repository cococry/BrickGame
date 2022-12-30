#include "time.h"
#include <GLFW/glfw3.h>

float Time::mDeltaTime;
float Time::mLastTime;

void Time::Update()
{
	float currentTime = glfwGetTime();
	mDeltaTime = currentTime - mLastTime;
	mLastTime = currentTime;
}
