#version 430 core

uniform samplerCube cubemapTexture; 
uniform vec3 cameraPos;
in vec3 texDirection;
in vec3 pos;

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

	vec3 color = texture(cubemapTexture, texDirection).rgb;
	color = add_water(color,pos,cameraPos);
	gl_FragColor = vec4(color, 1.0);
}
