#version 330 core


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aCoords;
layout(location = 2) in vec3 aNormal;

struct MVP {
	mat4 Model;
	mat4 View;
	mat4 Projection;
};


out vec2 oCoords;
out vec3 oNormal;
out vec3 oFragPosition;


uniform MVP uMVP;

void main() {

	oCoords = aCoords;
	oNormal = aNormal;
	oFragPosition = vec3(uMVP.Model * vec4(aPosition, 1.0));

	gl_Position = uMVP.Projection * uMVP.View * uMVP.Model * vec4(aPosition, 1.0);
}