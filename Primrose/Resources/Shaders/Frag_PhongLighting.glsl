#version 330 core

struct Material {
	sampler2D Diffuse;
	sampler2D Ambient;
	sampler2D Specular;

	float AmbientStrength;
	float SpecularStrength;
	int   SpecularShininess;
	
};

struct DirectionalLight {
	vec4  LightDirection;
	vec4  LightColor;
	float Intensity;
};
struct PointLight {
    vec3  LightPosition;
	vec4  LightColor;
	float Intensity;

	float Attenuation;
	float SourceRadius;
};
struct SpotLight {
    vec3  LightPosition;
	vec4  LightDirection;
	vec4  LightColor;
	float Intensity;

	float Attenuation;
	float SourceRadius;

	float InnerCutoffAngle;
	float OuterCutoffAngle;
};



vec4 CalculateDirectionalLight(DirectionalLight light, Material material, vec3 normal, vec3 viewDirection); 
vec4 CalculatePointLight(PointLight light, Material material, vec3 normal, vec3 fragPosition, vec3 viewDirection);
vec4 CalculateSpotLight(SpotLight light, Material material, vec3 normal, vec3 fragPosition, vec3 viewDirection);


uniform int uPointLightsCount;
uniform int uSpotLightsCount;

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLights[50];
uniform SpotLight uSpotLights[50];



uniform Material uMaterial;

uniform mat3 uNormalMatrix;
uniform vec3 uViewCameraPosition;

uniform vec4 uTint;



in vec3 oFragPosition;
in vec2 oCoords;
in vec3 oNormal;

out vec4 FragColor;


void main() {

	vec3 Normal = normalize(uNormalMatrix * oNormal);
	vec3 ViewDirection = normalize(uViewCameraPosition - oFragPosition);

	vec4 Output = vec4(0.0f);
	Output += CalculateDirectionalLight(uDirectionalLight, uMaterial, Normal, ViewDirection);

	for (int i = 0; i < uPointLightsCount; i++) {
		Output += CalculatePointLight(uPointLights[i], uMaterial, Normal, oFragPosition, ViewDirection);
	}

	for (int i = 0; i < uSpotLightsCount; i++) {
		Output += CalculateSpotLight(uSpotLights[i], uMaterial, Normal, oFragPosition, ViewDirection);
	}


	FragColor = Output;
}



vec4 CalculateDirectionalLight(DirectionalLight light, Material material, vec3 normal, vec3 viewDirection) {
	
	vec3 LightDirection = normalize(-light.LightDirection.xyz);

	//Diffuse
	float LightDotNormal = max(dot(normal, LightDirection), 0.0);
	vec4 Diffuse = texture(material.Diffuse, oCoords) * LightDotNormal * light.LightColor * uTint;

	//Ambient
	vec4 Ambient = vec4(texture(material.Ambient, oCoords).rgb * uMaterial.AmbientStrength, 1.0) * uTint;

	//Specular
	vec3 ReflectDirection = reflect(-LightDirection, normal);
	float SpecularValue = pow(max(dot(viewDirection, ReflectDirection), 0.0), material.SpecularShininess);
	vec4 Specular = texture(material.Specular, oCoords) * material.SpecularStrength * SpecularValue * light.LightColor * uTint;  

	return vec4((Ambient + Diffuse + Specular) * light.Intensity);
}


vec4 CalculatePointLight(PointLight light, Material material, vec3 normal, vec3 fragPosition, vec3 viewDirection) {
	
	vec3 LightDirection = normalize(light.LightPosition.xyz - fragPosition); //Name this directiontofrag or something otherwise its confusing.

	//Diffuse
	float LightDotNormal = max(dot(normal, LightDirection), 0.0);
	vec4 Diffuse = texture(material.Diffuse, oCoords) * LightDotNormal * light.LightColor * uTint;

	//Ambient
	vec4 Ambient = vec4(texture(material.Ambient, oCoords).rgb * uMaterial.AmbientStrength, 1.0) * uTint;

	//Specular
	vec3 ReflectDirection = reflect(-LightDirection, normal);
	float SpecularValue = pow(max(dot(viewDirection, ReflectDirection), 0.0), material.SpecularShininess);
	vec4 Specular = texture(material.Specular, oCoords) * material.SpecularStrength * SpecularValue * light.LightColor * uTint;  

	//Attenuation
	float Distance = length(light.LightPosition.xyz - fragPosition);
	float Attenuation = 1.0f / (1.0f + (light.Attenuation * Distance) + (light.SourceRadius * (Distance * Distance)));
	
	
	Diffuse *= Attenuation;
	Ambient *= Attenuation;
	Specular *= Attenuation;


    return vec4((Ambient + Diffuse + Specular) * light.Intensity);
}



vec4 CalculateSpotLight(SpotLight light, Material material, vec3 normal, vec3 fragPosition, vec3 viewDirection) {
	
	vec3 LightDirection = normalize(light.LightPosition.xyz - fragPosition);

	//Diffuse
	float LightDotNormal = max(dot(normal, LightDirection), 0.0);
	vec4 Diffuse = texture(material.Diffuse, oCoords) * LightDotNormal * light.LightColor * uTint;

	//Ambient
	vec4 Ambient = vec4(texture(material.Ambient, oCoords).rgb * uMaterial.AmbientStrength, 1.0) * uTint;

	//Specular
	vec3 ReflectDirection = reflect(-LightDirection, normal);
	float SpecularValue = pow(max(dot(viewDirection, ReflectDirection), 0.0), material.SpecularShininess);
	vec4 Specular = texture(material.Specular, oCoords) * material.SpecularStrength * SpecularValue * light.LightColor * uTint;  

	//Attenuation
	float Distance = length(light.LightPosition.xyz - fragPosition);
	float Attenuation = 1.0f / (1.0f + (light.Attenuation * Distance) + (light.SourceRadius * (Distance * Distance)));
	
	Diffuse *= Attenuation;
	Ambient *= Attenuation;
	Specular *= Attenuation;

	//Fade
	float theta = dot(LightDirection, normalize(-light.LightDirection.xyz));
	float epsilon = light.InnerCutoffAngle - light.OuterCutoffAngle;
	float FadeOut = clamp((theta - light.OuterCutoffAngle) / epsilon, 0.0, 1.0);  

	Diffuse.xyz *= FadeOut;
	Ambient.xyz *= FadeOut;
	Specular.xyz *= FadeOut;


    return vec4((Ambient + Diffuse + Specular) * light.Intensity);
}

