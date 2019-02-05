#version 330 core

//uniform sampler3D testTexture;
uniform sampler2D vertex_to_edge;

uniform mat4 modelViewProjectionMatrix;
uniform float scale;
uniform vec4 clip_plane;

layout (points) in;
layout (triangle_strip, max_vertices = 15) out;
//
//in vec4 color[]; // Output from vertex shader for each vertex
in vec3 oPos[]; // Output from vertex shader for each vertex
in vec4 val1[]; // Output from vertex shader for each vertex
in vec4 val2[]; // Output from vertex shader for each vertex

out vec3 normal; // Output to fragment shader
out vec3 pos; // Output to fragment shader

const int num_vec[256] = int[256]( 0 , 1 , 1 , 2 , 1 , 2 , 2 , 3 ,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 2 ,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  
                  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  2 , 3 , 3 , 2 , 3 , 4 , 4 , 3 ,  3 , 4 , 4 , 3 , 4 , 5 , 5 , 2 ,  
                  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 4 ,  
                  2 , 3 , 3 , 4 , 3 , 4 , 2 , 3 ,  3 , 4 , 4 , 5 , 4 , 5 , 3 , 2 ,  3 , 4 , 4 , 3 , 4 , 5 , 3 , 2 ,  4 , 5 , 5 , 4 , 5 , 2 , 4 , 1 ,  
                  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 2 , 4 , 3 , 4 , 3 , 5 , 2 ,  
                  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 4 ,  3 , 4 , 4 , 3 , 4 , 5 , 5 , 4 ,  4 , 3 , 5 , 2 , 5 , 4 , 2 , 1 ,  
                  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 4 , 4 , 5 , 2 , 3 , 3 , 2 ,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 2 ,  4 , 3 , 5 , 4 , 3 , 2 , 4 , 1 ,  
                  3 , 4 , 4 , 5 , 4 , 5 , 3 , 4 ,  4 , 5 , 5 , 2 , 3 , 4 , 2 , 1 ,  2 , 3 , 3 , 2 , 3 , 4 , 2 , 1 ,  3 , 2 , 4 , 1 , 2 , 1 , 1 , 0 );

const int edeges_to_vertices[24] = int[24]( 0, 1, 1, 2, 2, 3,  3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7);
vec3 index_to_pos[8] = vec3[8](vec3(-1,-1,1), vec3 (-1,1,1), vec3(1,1,1), vec3(1,-1,1),   vec3(-1,-1,-1), vec3(-1,1,-1), vec3(1,1,-1), vec3(1,-1,-1));


float get_density(vec3 pos){
	return pos.y-10*sin(pos.x/10);
}


//void RenderTerrain::buildPolygons(glm::vec3 p,std::vector<glm::vec3>& positions, std::vector<glm::vec3>&   normals, float scale, float vertices[]) {
void main(){
	int index = 0;
	float vertices[8]=float[8](1,-1,-1,1,1,-1,-1,1);
	for (int i=0;i<8;i++){
		vertices[i] = get_density(oPos[0]+(index_to_pos[i]*scale)/2);
	}
	
	//vertices[0]=val1[0].x;vertices[1]=val1[0].y;vertices[2]=val1[0].z;vertices[3]=val1[0].w; vertices[4]=val2[0].x;vertices[5]=val2[0].y;vertices[6]=val2[0].z;vertices[7]=val2[0].w;
	index += (vertices[7] > 0) ? 1 : 0;
	index *= 2;
	index += (vertices[6] > 0) ? 1 : 0;
	index *= 2;
	index += (vertices[5] > 0) ? 1 : 0;
	index *= 2;
	index += (vertices[4]> 0) ? 1 : 0;
	index *= 2;

	index += (vertices[3] > 0) ? 1 : 0;
	index *= 2;
	index += (vertices[2] > 0) ? 1 : 0;
	index *= 2;
	index += (vertices[1] > 0) ? 1 : 0;
	index *= 2;
	index += (vertices[0] > 0) ? 1 : 0;
	int triangleNumber = num_vec[index];
	for (int i = 0; i < triangleNumber; i++) {
		vec3 edge = (texelFetch(vertex_to_edge,ivec2(i,index),0).xyz*256);
		int edges[3] = int[3](int(edge.x),int(edge.y),int(edge.z));
		vec3 positions[3];
		for (int j = 0; j < 3; j++) {
			int p1 = edges[j];
			//std::cout << p1 << std::endl;
			//std::cout << int(edeges_to_vertices[p1 * 2]) << " " << int(edeges_to_vertices[p1 * 2 + 1]) << std::endl;
			int v1 = edeges_to_vertices[p1 * 2];
			int v2 = edeges_to_vertices[p1 * 2 + 1];
			float value1 = abs(vertices[v1]);
			float value2 = abs(vertices[v2]);
			vec3 temp = (((index_to_pos[v1] * value2 + index_to_pos[v2] * value1) / (value1 + value2))*scale/2.0);
			vec3 pos = temp + oPos[0].xyz;
			positions[j]=pos;
		}
		vec3 n = -normalize(cross(positions[2] - positions[0], positions[1] - positions[0]));

		gl_Position =modelViewProjectionMatrix * vec4(positions[0],1.0);
		pos = positions[0];
		gl_ClipDistance[0]=dot(clip_plane,vec4(pos, 1.0)	);
		normal =n;
		EmitVertex();

		gl_Position =modelViewProjectionMatrix * vec4(positions[1],1.0);
		pos = positions[1];
		gl_ClipDistance[0]=dot(clip_plane,vec4(pos, 1.0)	);
		normal =n;
		EmitVertex();

		gl_Position =modelViewProjectionMatrix * vec4(positions[2],1.0);
		normal =n;
		pos = positions[2];
		gl_ClipDistance[0]=dot(clip_plane,vec4(pos, 1.0));
		EmitVertex();

		EndPrimitive();
	}
	
}


/*
vec4 ppp[14] = vec4[14]( vec4(-1,-1,-1,0), vec4(-1,-1,1,0), vec4(1,-1,1,0), vec4(1,-1,-1,0), vec4(1,1,-1,0), vec4(-1,1,-1,0), vec4(-1,-1,-1,0), vec4(-1,-1,1,0), vec4(-1,1,1,0), vec4(1,-1,1,0), vec4(1,1,1,0), vec4(1,1,-1,0), vec4(-1,1,-1,0), vec4(-1,1,1,0));
void main(){
	
	for (int i=0;i<14; i++){
		gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + ppp[i]*scale);
		pos = (gl_in[0].gl_Position + ppp[i]*scale).xyz;
		normal = vec3(0,1,0);
		EmitVertex();
	}
	EndPrimitive();
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(0.0,0.1,0.0,0.0));
	normal = vec3(0,0,1);
		EmitVertex();
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(0.10,0.1,0.0,0.0));
		normal = vec3(0,0,1);
		EmitVertex();
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position - vec4(0.0,0.1,0.0,0.0));
		normal = vec3(0,0,1);
		EmitVertex();
		EndPrimitive();


		
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(1.0,1.0,1.0,0.0)*scale*0.5);
	normal = vec3(0,0,1);
	EmitVertex();
	
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(-1.0,1.0,1.0,0.0)*scale*0.5);
	normal = vec3(0,0,1);
	EmitVertex();
	
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(-1.0,1.0,-1.0,0.0)*scale*0.5);
	normal = vec3(0,0,1);
	EmitVertex();
	
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(-1.0,-1.0,-1.0,0.0)*scale*0.5);
	normal = vec3(0,0,1);
	EmitVertex();
	
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(-1.0,-1.0,1.0,0.0)*scale*0.5);
	normal = vec3(0,0,1);
	EmitVertex();
	
	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(1.0,-1.0,1.0,0.0)*scale*0.5);
	normal = vec3(0,0,1);
	EmitVertex();

	gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + vec4(1.0,-1.0,-1.0,0.0)*scale*0.5);
	normal = vec3(0,0,1);
	EmitVertex();
		

	for (int i=0;i<14; i++){
		gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position + ppp[i]*scale/2);
		pos = (gl_in[0].gl_Position + ppp[i]*scale).xyz;
		normal = normalize(ppp[i].xyz);
		EmitVertex();
	}
}
*/