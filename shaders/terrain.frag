#version 430 core


uniform sampler2D shadowSampler;

uniform samplerCube cubemapTexture; 
uniform vec3 cameraPos;
uniform vec3 lightPos;

in vec3 normal;
in vec3 pos;
in vec4 lightSpacePos;


float calculate_shadow(vec3 normal)
{
	vec3 position = (lightSpacePos.xyz/lightSpacePos.w)*0.5+0.5;
	float closest = texture2D(shadowSampler,position.xy).r;
	float current = position.z;
	float bias = max(0.05*(1-dot(normal,normalize(lightPos))),0.005);
	bias = 0.02;
	float shadow = current - bias < closest ? 0.0 : 1.0;
	return shadow;
}
vec3 add_water(vec3 color, vec3 pos,vec3 cameraPos){
    vec3 water_color = vec3(0.5,0.75,1.0);
    if (pos.y>0) return color;
    float distance = 0;
    if (cameraPos.y>0){
        vec3 dir = normalize(pos - cameraPos);
        distance = length(pos - cameraPos) -length(dir*cameraPos.y);
    }
    else{
        distance = length(pos - cameraPos);
    }
    return mix(color,water_color,clamp(distance/20,0.0,0.5));
}

void main()
{
	float shadow = calculate_shadow(normal);
	vec3 color = (normal+vec3(1.0))/2;
	color = add_water(color,pos,cameraPos);
	gl_FragColor = vec4(mix(color,vec3(0,0,0),0.5*shadow), 1.0);
	//gl_FragColor = vec4(color, 1.0);
}
