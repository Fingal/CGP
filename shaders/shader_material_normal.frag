#version 330 core



out vec4 FragColor;
in vec2 vertexTexCoord;
in vec3 interpPos;
in vec3 interpNormal;
in vec2 texCoord;

// material parameters
uniform sampler2D  Tmetallic;
uniform sampler2D  Troughness;
uniform sampler2D  Talbedo;
uniform sampler2D  Tnormal;
uniform sampler2D  shadowSampler;
float ao;


uniform vec3 lightPos;
uniform vec3 cameraPos;


in vec3 pos;

in vec3 interpLight;
in vec3 interpCamera;

in vec3 interpTangent;
in vec3 bTangent;

vec3 lightColors;

in vec4 lightSpacePos;

const float PI = 3.14159265359;
  
float Distribution(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickS1(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}


float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickS1(NdotV, roughness);
    float ggx1  = GeometrySchlickS1(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  


float calculate_shadow(vec3 normal)
{
	vec3 position = (lightSpacePos.xyz/lightSpacePos.w)*0.5+0.5;
	float closest = texture2D(shadowSampler,position.xy).r;
	float current = position.z;
	float bias = max(0.04*(1-dot(normal,normalize(lightPos))),0.005);
	//bias = 0.005;
	float shadow = current - bias < closest ? 0.0 : 1.00;
	return shadow;

}

void main()
{		
	vec2 interpTexCoord = vec2(texCoord.x,1-texCoord.y);
	float metallic = texture2D(Tmetallic, interpTexCoord)[0];
	float roughness = texture2D(Troughness, interpTexCoord)[0];
	vec3 albedo = texture2D(Talbedo, interpTexCoord).rgb;
	lightColors = vec3(300.0);
	ao = 1.0;

	
	vec3 normal = 2*(texture2D(Tnormal,interpTexCoord).xyz + -0.5);
	//normal = vec3(normal.x,normal.z,normal.y);
	
    vec3 Tangent = normalize(normalize(interpTangent) - dot(normalize(interpTangent), interpNormal) * interpNormal);
    vec3 Bitangent = cross(Tangent, interpNormal);
	
    mat3 TBN = mat3(interpTangent, Bitangent, interpNormal);
    vec3 N = normalize(TBN*normal);
    vec3 V = normalize(cameraPos - pos);

    vec3 F0 = vec3(0.045);
	
	// calculate F0 value taking material properties into account
    F0 = mix(F0, albedo, metallic);

    // calculate radiance
    vec3 L = normalize(lightPos - pos);
    vec3 H = normalize(V + L);
    float distance    = length(lightPos - pos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = lightColors * attenuation;        
        
    // BRDF TODO:
	// ...
	vec3 kS = F0;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

            
    // calculate outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    //float G = GeometrySmith(N, V, L, roughness) ;
    //float D = Distribution(N, H, roughness);
    //vec3 F = fresnelSchlick(max(dot(L, H),0), F0);
    //vec3 specular = (F * G * D) / (4 * max(dot(N, V), 0.01) * max(dot(N, L), 0.01));
	vec3 specular = (fresnelSchlick(dot(L,H),F0) *  GeometrySmith(N, V, L, roughness) * Distribution(N,H,roughness)) / (4 * max(dot(N, L),0.01) * max(dot(N, V),0.01));
    vec3 Lo =  4.0*(kD * albedo / PI + specular) * radiance * NdotL; 
  
    vec3 ambient = vec3(0.015) * albedo * ao;
	//ambient = vec3(0.0);
    //vec3 color = ambient + Lo;
	Lo = Lo * (1.0-calculate_shadow(interpNormal));
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.0));  
   
    FragColor = vec4(color, 1.0);
}  
