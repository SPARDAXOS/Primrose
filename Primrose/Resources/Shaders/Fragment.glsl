#version 330 core


out vec4 FragColor;

in vec4 oColor;
in vec2 oCoords;

uniform sampler2D uDiffuse;

uniform vec4 uTint;

void main() {


	FragColor = texture(uDiffuse, oCoords) * uTint;
}

