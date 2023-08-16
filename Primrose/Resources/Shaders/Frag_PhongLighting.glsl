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


//For Lights
uniform vec4 uLightDirection; //If w is 0 then use direction otherwise use position for direction
uniform vec3 uLightPosition;
uniform vec4 uLightColor;

//For point lights
uniform float uIntensity;
uniform float uAttenuation;
uniform float uSourceRadius;

//For Spotlight
uniform float uInnerCutoffAngle;
uniform float uOuterCutoffAngle;


uniform vec4 uTint;


void main() {


	//vec3 LightDirection;
	//	float Result = 1.0; //Do the calc regardless?
	//	if (uLightDirection.w == 0.0f) {
	//		LightDirection = normalize(uLightDirection.xyz);		
	//	}
	//	else if (uCutoffAngle != 0.0f) {
	//		LightDirection = normalize(uLightPosition - oFragPosition);  //Direction to frag, Terrible name
	//		float theta = dot(LightDirection, normalize(-uLightDirection.xyz));
	//	
	//		if (theta > uCutoffAngle) 
	//		{       
	//		  	float Distance = length(uLightPosition.xyz - oFragPosition);
	//			Result = 1.0f / (1.0f + (uAttenuation * Distance) + (uSourceRadius * (Distance * Distance)));
	//		}
	//	}
	//	else if (uLightDirection.w == 1.0f) {
	//		LightDirection = normalize(uLightPosition.xyz - oFragPosition);  
	//		float Distance = length(uLightPosition.xyz - oFragPosition);
	//		Result = 1.0f / (1.0f + (uAttenuation * Distance) + (uSourceRadius * (Distance * Distance)));
	//	}
	

	//Ambient
	vec4 Ambient = vec4(texture(uMaterial.Ambient, oCoords).rgb * uMaterial.AmbientStrength, 1.0) * uTint;

	
	vec3 LightDirection = normalize(uLightPosition.xyz - oFragPosition);
	float theta = dot(LightDirection, normalize(-uLightDirection.xyz));
	float epsilon   = uInnerCutoffAngle - uOuterCutoffAngle;
	float FadeOut = clamp((theta - uOuterCutoffAngle) / epsilon, 0.0, 1.0);  

	//Attenuation
	float Distance = length(uLightPosition.xyz - oFragPosition);
	float AttenuationResults = 1.0f / (1.0f + (uAttenuation * Distance) + (uSourceRadius * (Distance * Distance)));
	AttenuationResults *= uIntensity;
		

	//Diffuse
	vec3 Normal = normalize(uNormalMatrix * oNormal);
	float LightDotNormal = max(dot(Normal, LightDirection), 0.0);
	vec4 Diffuse = texture(uMaterial.Diffuse, oCoords) * LightDotNormal * uLightColor * uTint;

	//Specular
	vec3 ViewDirection = normalize(uViewCameraPosition - oFragPosition);
	vec3 ReflectDirection = reflect(-LightDirection, Normal);
	float SpecularValue = pow(max(dot(ViewDirection, ReflectDirection), 0.0), uMaterial.SpecularShininess);
	vec4 Specular = texture(uMaterial.Specular, oCoords) * uMaterial.SpecularStrength * SpecularValue * uLightColor * uTint;  


	Diffuse.xyz *= AttenuationResults;
	Ambient.xyz *= AttenuationResults;
	Specular.xyz *= AttenuationResults;

	Diffuse.xyz *= FadeOut;
	Ambient.xyz *= FadeOut;
	Specular.xyz *= FadeOut;

	
	FragColor = (Ambient + Diffuse + Specular); //* uTint;
}

