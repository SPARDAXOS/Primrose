#version 330 core


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aCoords;
layout(location = 2) in vec3 aNormal;

out vec2 oCoords;
out vec3 oNormal;
out vec3 oFragPosition;
out mat3 oNormalMatrix;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

void main() {

	oCoords = aCoords;
	oNormal = aNormal;
	oFragPosition = vec3(uModel * vec4(aPosition, 1.0));
	oNormalMatrix = uNormalMatrix;

	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}