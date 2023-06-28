#version 330 core


out vec4 FragColor;

in vec4 oColor;
in vec2 oCoords;

uniform sampler2D uDiffuse;

void main() {


	FragColor = 1 + texture(uDiffuse, oCoords) * oColor;
}

