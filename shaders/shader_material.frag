#version 330 core



out vec4 FragColor;
in vec2 vertexTexCoord;
in vec3 interpPos;
in vec3 interpNormal;
in vec2 interpTexCoord;

// material parameters
uniform sampler2D  Tmetallic;
uniform sampler2D  Troughness;
uniform sampler2D  Talbedo;
uniform sampler2D  shadowSampler;
float ao;


uniform vec3 lightPos;
vec3 lightColors;

uniform vec3 cameraPos;

const float PI = 3.14159265359;
  


in vec4 lightSpacePos;


vec3 add_water(vec3 color, vec3 pos,vec3 cameraPos){
    vec3 water_color = vec3(0.3,0.55,0.80);
    if (pos.y>0) return color;
    float distance = 0;
    if (cameraPos.y>0){
        vec3 dir = normalize(pos - cameraPos);
        distance = length(pos - cameraPos) -length(dir*cameraPos.y);
    }
    else{
        distance = length(pos - cameraPos);
    }
    return mix(color,water_color,clamp(distance/40,0.0,0.5));
}

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
	float bias = max(0.01*(1-dot(vec3(0,1,0),normalize(lightPos))),0.01);
	 //bias = 0.0;
	float shadow = current - bias < closest ? 0.0 : 1.0;
	return shadow;

}
void main()
{		
	float metallic = texture2D(Tmetallic, interpTexCoord)[0];
	float roughness = texture2D(Troughness, interpTexCoord)[0];
	vec3 albedo = texture2D(Talbedo, interpTexCoord).rgb;
	lightColors = vec3(300.0);
	ao = 1.0;

    vec3 N = normalize(interpNormal);
    vec3 V = normalize(cameraPos - interpPos);

    vec3 F0 = vec3(0.045);
	
	// calculate F0 value taking material properties into account
    F0 = mix(F0, albedo, metallic);

    // calculate radiance
    vec3 L = normalize(lightPos - interpPos);
    vec3 H = normalize(V + L);
    float distance    = length(lightPos - interpPos);
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
  
    vec3 ambient = vec3(0.01) * albedo * ao;
	Lo=Lo*(1.0-calculate_shadow(N));
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.0));  
	
    color = add_water(color,interpPos,cameraPos);

    FragColor = vec4(color, 1.0);
}  
