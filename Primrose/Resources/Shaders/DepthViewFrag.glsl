#version 330 core


out vec4 FragColor;


uniform float uNear;
uniform float uFar;


float LinearizeDepth(float depth){
	
	float Results = depth * 2.0f - 1.0f;
	return (2.0f * uNear * uFar) / (uFar * uNear - Results * (uFar - uNear));
}

void main() {

	float Depth = LinearizeDepth(gl_FragCoord.z) / uFar;
	FragColor = vec4(vec3(Depth), 1.0f);
}