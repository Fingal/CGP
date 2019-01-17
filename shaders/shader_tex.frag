#version 430 core

uniform vec3 lightPos;
uniform vec3 cameraPos;


uniform sampler2D colorTexture;
uniform sampler2D  shadowSampler;


in vec3 interpNormal;
in vec2 interpTexCoord;
in vec3 interpPosition;
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
void main()
{
	vec3 normal = normalize(interpNormal);
	//lightDir = lightVec;
	//vec3 normal = normalize(2.0 * texture2D (normalMap, interpTexCoord.st).rgb - 1.0);
	vec3 toEye = normalize(cameraPos - interpPosition);
	//vec3 toEye = normalize(tang - eyeVec);
	float diffuse = max(dot(normal, -normalize(interpPosition-lightPos)), 0.1);

	float specular = pow(max(dot(toEye, reflect(normalize(interpPosition-lightPos), normal)), 0.0), 30.0);
	float shadow = calculate_shadow(normal);
	vec3 color=texture2D(colorTexture, interpTexCoord).rgb * diffuse * (1- 0.8*calculate_shadow(normal)) + vec3(1.0) * specular*(1-calculate_shadow(normal));
	color = add_water(color,interpPosition,cameraPos);
	//gl_FragColor = vec4(texture2D(colorTexture, interpTexCoord).rgb * diffuse * (1- 0.8*calculate_shadow(normal)) + vec3(1.0) * specular*(1-calculate_shadow(normal)), 1.0);
	gl_FragColor = vec4(color,1.0);
}
