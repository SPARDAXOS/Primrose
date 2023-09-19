#version 330 core


layout(location = 0) in vec3 aPosition;

struct MVP {
	mat4 Model;
	mat4 View;
	mat4 Projection;
};


uniform MVP uMVP;


void main() {

	gl_Position = uMVP.Projection * uMVP.View * uMVP.Model * vec4(aPosition, 1.0);
}