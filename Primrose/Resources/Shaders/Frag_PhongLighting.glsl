#version 330 core

struct Material {
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


uniform sampler2D uDiffuseTexUnit;


uniform mat3 uNormalMatrix;
uniform vec3 uViewCameraPosition;
uniform vec3 uLightPosition;
uniform vec4 uLightColor;
uniform vec4 uTint;


void main() {
	
	//Ambient
	float AmbientStrength = 0.2;
    vec4 Ambient = vec4(texture(uDiffuseTexUnit, oCoords).xyz * AmbientStrength, 1.0) * uTint;  //? Does the texture only have 3 comps?

	//Diffuse
	vec3 Normal = normalize(uNormalMatrix * oNormal);
	vec3 LightDirection = normalize(uLightPosition - oFragPosition);  
	float LightDotNormal = max(dot(Normal, LightDirection), 0.0);
	vec4 Diffuse = texture(uDiffuseTexUnit, oCoords) * LightDotNormal * uLightColor * uTint;

	//Specular
	float SpecularStrength = 0.5;
	int SpecularShininess = 32;
	vec3 ViewDirection = normalize(uViewCameraPosition - oFragPosition);
	vec3 ReflectDirection = reflect(-LightDirection, Normal);
	float SpecularValue = pow(max(dot(ViewDirection, ReflectDirection), 0.0), SpecularShininess);
	vec4 Specular = SpecularStrength * SpecularValue * uLightColor * uTint;  


	//List of stuff for next time
	//Simple window with text saying whats happening in the project
	//When the project is fully loaded and ready, it opens the editor
	//Materials
	//Support for ambient and specualr maps.
	//Separate engine textures and data into own storage and load it from Editor folder! Separate operation to track in the system log
	//Just make a trello board or something at this point

	FragColor = (Ambient + Diffuse + Specular) * uTint;
}

