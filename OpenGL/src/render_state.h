#pragma once

#include "shader.h"
#include <memory>


class RenderState
{
public:
	static void Init();

	inline static const std::shared_ptr<Shader>& GetShader()
	{
		return mShader;
	}
private:
	static std::shared_ptr<Shader> mShader;
};