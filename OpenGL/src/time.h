#pragma once

class Time
{
public:
	static void Update();

	inline static float GetDeltaTime()
	{
		return mDeltaTime;
	}
private:
	static float mDeltaTime;
	static float mLastTime;
};