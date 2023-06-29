#version 330 core


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aCoords;

out vec4 oColor;
out vec2 oCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {

	oColor = aColor;
	oCoords = aCoords;
	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}