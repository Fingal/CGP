#version 430 core

uniform sampler2D reflectSampler;
uniform sampler2D refractSampler;
uniform sampler2D dudvSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthSampler;

uniform float time;
uniform vec3 cameraPos;
uniform vec3 lightPos;
const float waveStrength = 0.02;

in vec3 cameraDirection;
in vec2 texDirection;
in vec4 clipSpace;
in vec3 vertexPos;

const float n_2 = 1.33;
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
vec3 countSpecular()
{
	float metallic = 0.9;
	float roughness = 0.7;
	vec3 albedo = vec3(0.9,0.9,0.9);
	vec3 lightColors = vec3(300.0);
	float ao = 1.0;

	
	float moveFactor=time/7;
	vec2 distortedTexCoords = texture(dudvSampler, vec2(texDirection.x + moveFactor, texDirection.y)).rg*0.1;
	distortedTexCoords = texDirection + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);	

	vec3 normal = texture(normalSampler, distortedTexCoords).xyz;

	//coodinates switched to make y point up
	normal = vec3(normal.x,normal.z,normal.y);
    vec3 N = normalize(mix((normal*2 -1.0),vec3(0,1,0),0.05));
    vec3 V = normalize(cameraPos - vertexPos);

    vec3 F0 = vec3(0.045);
	
	// calculate F0 value taking material properties into account
    F0 = mix(F0, albedo, metallic);

    // calculate radiance
    vec3 L = normalize(lightPos - vertexPos);
    vec3 H = normalize(V + L);
    //float distance    = length(lightPos - vertexPos);
    //float attenuation = 1.0 / (distance * distance);
    //vec3 radiance     = lightColors * attenuation;        
        
    // BRDF TODO:
	// ...
	//vec3 kS = F0;
	//vec3 kD = vec3(1.0) - kS;
	//kD *= 1.0 - metallic;

            
    // calculate outgoing radiance Lo
    //float NdotL = max(dot(N, L), 0.0);
    //float G = GeometrySmith(N, V, L, roughness) ;
    //float D = Distribution(N, H, roughness);
    //vec3 F = fresnelSchlick(max(dot(L, H),0), F0);
    //vec3 specular = (F * G * D) / (4 * max(dot(N, V), 0.01) * max(dot(N, L), 0.01));
	//vec3 specular = (fresnelSchlick(dot(L,H),F0) *  GeometrySmith(N, V, L, roughness) * Distribution(N,H,roughness)) / (4 * max(dot(N, L),0.01) * max(dot(N, V),0.01));
    //vec3 Lo =  4.0*(kD * albedo / PI + specular) * radiance * NdotL; 
	//Lo = Lo/(Lo+vec3(1.0));
    //vec3 ambient = vec3(0.01) * albedo * ao;
    //vec3 color = ambient + Lo;
    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.0));  
   	

	float shineDamper = 50.0;
	float reflectivity = 0.6;
	vec3 lightColor = vec3(1.0);
	vec3 reflectedLight = reflect((-L), N);
	float specular = max(dot(reflectedLight, V), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColor * specular * reflectivity;


    return specularHighlights;


}
float countDistance(vec2 coord){
	float depth = texture(depthSampler, coord).r;
	float near = 0.1;
	float far = 100.0;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * gl_FragCoord.z - 1.0) * (far - near));
	return floorDistance - waterDistance;

}

void main()
{
	vec2 cord = (clipSpace.xy/clipSpace.w)/2.0+0.5;
	vec2 reflectionCord = vec2(cord.x,-cord.y);
	vec2 refractionCord = cord;
	float distance = countDistance(refractionCord);
	float moveFactor=time/7;
	vec2 distortedTexCoords = texture(dudvSampler, vec2(texDirection.x + moveFactor, texDirection.y)).rg*0.1;
	distortedTexCoords = texDirection + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
	vec2 totalDistortion = (texture(dudvSampler, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength*clamp(distance/10,0.0,1.0);



	reflectionCord += totalDistortion;
	reflectionCord.x = clamp(reflectionCord.x,0.001,0.999);
	reflectionCord.y = clamp(reflectionCord.y,-0.999,-0.001);

	refractionCord = refractionCord + totalDistortion;
	refractionCord.x = clamp(refractionCord.x,0.001,0.999);
	refractionCord.y = clamp(refractionCord.y,0.001,0.999);

	

	vec3 normal = texture(normalSampler, distortedTexCoords).xyz;

	//coodinates switched to make y point up
	normal = vec3(normal.x,normal.z,normal.y) *2 -1.0;
	normal = normalize(mix(normal,vec3(0,1,0),0.2));
	// cos nie dziala
	//float frensel = pow(dot(normalize(cameraPos-vertexPos),normal),1.5);
	float frensel = pow(dot(normalize(cameraPos-vertexPos),vec3(0.0,1.0,0.0)),5);
	//float frensel = pow(dot(normalize(cameraPos-vertexPos),vec3(0,1,0)),1.5);

	frensel = clamp(frensel,0.001,0.999);
	//frensel = pow(frensel,1.5);
	float F_0 = pow((1-n_2)/(1+n_2),2);
	vec3 color = vec3(0,0,0);
	float schlick = F_0+(1-F_0)*pow(1-dot(normalize(cameraPos-vertexPos),normal),5);
	schlick = clamp(schlick,0,1);
	vec3 reflectColor = texture(reflectSampler,reflectionCord ).rgb;
	vec3 refractColor = texture(refractSampler,refractionCord ).rgb;


	if (cameraPos.y>=-0.01){
	//color  = vec3(0,0,1)*pow(dot(normalize(cameraPos-vertexPos),normal),5);
	vec3 specular = countSpecular()*clamp(distance/10,0.0,1.0);
	color = mix(refractColor,reflectColor,schlick) + specular;
	}
	else{
	color = refractColor;
	}
	color = mix(color,vec3(0.5,0.75,1.0),0.2);
	gl_FragColor = vec4(color , clamp(distance/10,0.0,1.0));
}
