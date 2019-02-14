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
float phi(float x,float lambda){
	if (x<=0){
		return 0;
	}
	return exp(-1/((lambda*x)*(lambda*x)));
}

float green(vec3 pos, float slope){
	vec3 green = vec3(0.2,0.8,0.3);
	float height_mod = ( abs(pos.y)/60-0.4);
	//if (slope>height_mod){
	//	return green* min(1,slope-height_mod,);
	//}
	//else{
	//	return vec3(0);
	//}
	//float lambda = clamp(abs(pos.y/10)+1,1.0,20.0);
	return phi(slope-height_mod,5)*min(1-(pos.y)/300,1);
}
float gray(vec3 pos, float slope){
	float height_mod = ( abs(pos.y)/60-0.4);

	return 1-phi(slope-height_mod+0.5,2)*min(1-(pos.y)/300,1);
}

void main()
{
	float shadow = calculate_shadow(normal);
	vec3 green_color = vec3(0.2,0.8,0.3);
	//float slope = (dot(normal,vec3(0,1,0))+1)/2;
	float slope = dot(normal,vec3(0,1,0));
	vec3 color = vec3(0.0);
	color = mix(color,green_color,green(pos,slope));
	vec3 gray_color = mix(vec3(0.2,0.17,0.13),vec3(0.7),gray(pos,slope));
	color = mix(color,gray_color,1-green(pos,slope));
	color = add_water(color,pos,cameraPos);
	//color = mix(color,vec3(0,0,0),0.5*shadow);
	gl_FragColor = vec4(color, 1.0);
	//gl_FragColor = vec4(color, 1.0);
}
