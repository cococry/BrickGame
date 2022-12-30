#version 460 core

out vec4 oColor;

in vec2 vTexcoord;
in vec4 vVertex;
in float visibility;
in float vLighting;

uniform sampler2D uTexture;

uniform vec4 uFogColor;

in vec4 gl_FragCoord;

void main() {
    oColor = texture(uTexture, vTexcoord); 
    oColor = mix(vec4(uFogColor), oColor, visibility);

}