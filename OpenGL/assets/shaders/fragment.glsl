#version 460 core

out vec4 oColor;

in vec2 vTexcoord;
in vec4 vVertex;
in float visibility;
in float vLighting;

uniform sampler2D diffuse1;

uniform vec4 uFogColor;
uniform vec4 diffuse;
uniform int no_textures;

in vec4 gl_FragCoord;

void main() {
    if(no_textures == 0)
        oColor = texture(diffuse1, vTexcoord); 
    else if(no_textures == 1)
        oColor = diffuse;
    oColor = mix(vec4(uFogColor), oColor, visibility);

}