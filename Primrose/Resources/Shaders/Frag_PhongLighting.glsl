#version 330 core

struct Material {
	sampler2D Diffuse;
	sampler2D Ambient;
	sampler2D Specular;

	float AmbientStrength;
	float SpecularStrength;
	int   SpecularShininess;
	
};

struct LightValues {
	float Intensity;
	//Color
	//Position
	//ViewCameraPosition?

	
};





in vec3 oFragPosition;
in vec2 oCoords;
in vec3 oNormal;

out vec4 FragColor;

uniform Material uMaterial;

uniform mat3 uNormalMatrix;
uniform vec3 uViewCameraPosition;
uniform vec3 uLightPosition;
uniform vec4 uLightColor;
uniform vec4 uTint;


void main() {
	
	//Ambient
    vec4 Ambient = vec4(texture(uMaterial.Ambient, oCoords).xyz * uMaterial.AmbientStrength, 1.0) * uTint;  //? Does the texture only have 3 comps?

	//Diffuse
	vec3 Normal = normalize(uNormalMatrix * oNormal);
	vec3 LightDirection = normalize(uLightPosition - oFragPosition);  
	float LightDotNormal = max(dot(Normal, LightDirection), 0.0);
	vec4 Diffuse = texture(uMaterial.Diffuse, oCoords) * LightDotNormal * uLightColor * uTint;

	//Specular
	vec3 ViewDirection = normalize(uViewCameraPosition - oFragPosition);
	vec3 ReflectDirection = reflect(-LightDirection, Normal);
	float SpecularValue = pow(max(dot(ViewDirection, ReflectDirection), 0.0), uMaterial.SpecularShininess);
	vec4 Specular = texture(uMaterial.Specular, oCoords) * uMaterial.SpecularStrength * SpecularValue * uLightColor * uTint;  


	FragColor = (Ambient + Diffuse + Specular) * uTint;
}

