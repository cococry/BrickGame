#include "render_state.h"

std::shared_ptr<Shader> RenderState::mShader;

void RenderState::Init()
{
	mShader = std::make_shared<Shader>("assets/shaders/vertex.glsl",
		"assets/shaders/fragment.glsl");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
