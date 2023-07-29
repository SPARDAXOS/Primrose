#version 330 core


in vec3 oFragPosition;
in vec2 oCoords;
in vec3 oNormal;
in mat3 oNormalMatrix;

out vec4 FragColor;


uniform sampler2D uDiffuseTexUnit;

uniform vec3 uViewCameraPosition;
uniform vec3 uLightPosition;
uniform vec4 uLightColor;
uniform vec4 uTint;


void main() {
	

	float AmbientStrength = 0.2;
    vec4 Ambient = vec4(texture(uDiffuseTexUnit, oCoords).xyz * AmbientStrength, 1.0) * uTint; 

	vec3 Normal = oNormalMatrix * normalize(oNormal);
	vec3 LightDirection = normalize(uLightPosition - oFragPosition);  
	float LightDotNormal = max(dot(Normal, LightDirection), 0.0);
	vec4 Diffuse = texture(uDiffuseTexUnit, oCoords) * LightDotNormal * uLightColor;


	float SpecularStrength = 0.5;
	int SpecularShininess = 32;
	vec3 ViewDirection = normalize(uViewCameraPosition - oFragPosition);
	vec3 ReflectDirection = reflect(-LightDirection, Normal);
	float SpecularValue = pow(max(dot(ViewDirection, ReflectDirection), 0.0), SpecularShininess);
	vec4 Specular = SpecularStrength * SpecularValue * uLightColor;  


	vec4 Results = (Ambient + Diffuse + Specular) * uTint;
	Results.w = 1.0f;

	FragColor = Results;
}

