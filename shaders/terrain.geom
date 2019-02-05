#version 330 core

uniform sampler3D testTexture;
uniform float gridSize;
uniform float scale;
layout (points) in;
//layout (line_strip, max_vertices = 4) out;
layout (triangle_strip, max_vertices = 15) out;
//
//in vec4 color[]; // Output from vertex shader for each vertex
in vec4 oPos[]; // Output from vertex shader for each vertex
in vec4 val1[]; // Output from vertex shader for each vertex
in vec4 val2[]; // Output from vertex shader for each vertex

out vec4 normal; // Output to fragment shader
out vec4 pos; // Output to fragment shader

const int num_vec[256] = int[256]( 0 , 1 , 1 , 2 , 1 , 2 , 2 , 3 ,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 2 ,  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  
                  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  2 , 3 , 3 , 2 , 3 , 4 , 4 , 3 ,  3 , 4 , 4 , 3 , 4 , 5 , 5 , 2 ,  
                  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 4 ,  
                  2 , 3 , 3 , 4 , 3 , 4 , 2 , 3 ,  3 , 4 , 4 , 5 , 4 , 5 , 3 , 2 ,  3 , 4 , 4 , 3 , 4 , 5 , 3 , 2 ,  4 , 5 , 5 , 4 , 5 , 2 , 4 , 1 ,  
                  1 , 2 , 2 , 3 , 2 , 3 , 3 , 4 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 3 ,  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 2 , 4 , 3 , 4 , 3 , 5 , 2 ,  
                  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 4 ,  3 , 4 , 4 , 3 , 4 , 5 , 5 , 4 ,  4 , 3 , 5 , 2 , 5 , 4 , 2 , 1 ,  
                  2 , 3 , 3 , 4 , 3 , 4 , 4 , 5 ,  3 , 4 , 4 , 5 , 2 , 3 , 3 , 2 ,  3 , 4 , 4 , 5 , 4 , 5 , 5 , 2 ,  4 , 3 , 5 , 4 , 3 , 2 , 4 , 1 ,  
                  3 , 4 , 4 , 5 , 4 , 5 , 3 , 4 ,  4 , 5 , 5 , 2 , 3 , 4 , 2 , 1 ,  2 , 3 , 3 , 2 , 3 , 4 , 2 , 1 ,  3 , 2 , 4 , 1 , 2 , 1 , 1 , 0 );

const unsigned char edeges_to_vertices[24] = int[256]( 0, 1, 1, 2, 2, 3,  3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 ;
vec3 index_to_pos[8] = vec3[8](vec3(-1,-1,1), vec3 (-1,1,1), vec3(1,1,1), vec3(1,-1,1),   vec3(-1,-1,-1), vec3(-1,1,-1), vec3(1,1,-1), vec3(1,-1,-1));

void main() {    
	gColor = color[0]; // Point has only one vertex
    gl_Position = gl_in[0].gl_Position + vec4(0.0,0.1,0.0,0.0); 
	pos = gl_in[0].gl_Position + vec4(0.0,0.1,0.0,0.0); 
    EmitVertex();
	
	gColor = color[0]; 
    gl_Position = gl_in[0].gl_Position + vec4(0.0,-0.1,0.0,0.0); 
	pos = gl_in[0].gl_Position + vec4(0.0,-0.1,0.0,0.0); 
    EmitVertex();
    
    EndPrimitive();
	
	gColor = color[0]; // Point has only one vertex
    gl_Position = gl_in[0].gl_Position + vec4(0.1,0.0,0.0,0.0); 
	pos = gl_in[0].gl_Position + vec4(0.1,0.0,0.0,0.0); 
    EmitVertex();
	
	gColor = color[0]; 
    gl_Position = gl_in[0].gl_Position + vec4(-0.1,0.0,0.0,0.0); 
	pos = gl_in[0].gl_Position + vec4(-0.1,0.0,0.0,0.0); 
    EmitVertex();
    
    EndPrimitive();
}  */

int compute_index(){
	vec3 location = oPos[0].xyz*scale;
	int index = 0;

	
	vec3 texpos = oPos[0].xyz*scale+vec3(0.5);
	index += (texture(testTexture,texpos.xyz+vec3(1,0,0)/gridSize).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,texpos.xyz+vec3(1,1,0)/gridSize).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,texpos.xyz+vec3(0,1,0)/gridSize).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,texpos.xyz+vec3(0,0,0)/gridSize).r > 0) ? 1:0;
	index *=2;

	index += (texture(testTexture,texpos.xyz+vec3(1,0,1)/gridSize).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,texpos.xyz+vec3(1,1,1)/gridSize).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,texpos.xyz+vec3(0,1,1)/gridSize).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,texpos.xyz+vec3(0,0,1)/gridSize).r > 0) ? 1:0;
	return index;
	//texture(testTexture,oPos[0].xyz+vec3(0,0,1)/(float)gridSize)
}
void main(){
	vec3 texpos = oPos[0].xyz*scale+vec3(0.5);
	
	vec3 vertices[8]=vec3[8](texpos.xyz+vec3(1,0,0)/gridSize,
							texpos.xyz+vec3(1,1,0)/gridSize,
							texpos.xyz+vec3(0,1,0)/gridSize,
							texpos.xyz+vec3(0,0,0)/gridSize,
							texpos.xyz+vec3(1,0,1)/gridSize,
							texpos.xyz+vec3(1,1,1)/gridSize,
							texpos.xyz+vec3(0,1,1)/gridSize,
							texpos.xyz+vec3(0,0,1)/gridSize);
	int index = 0;

	index += (texture(testTexture,vertices[0]).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,vertices[1]).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,vertices[2]).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,vertices[3]).r > 0) ? 1:0;
	index *=2;

	index += (texture(testTexture,vertices[4]).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,vertices[5]).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,vertices[6]).r > 0) ? 1:0;
	index *=2;
	index += (texture(testTexture,vertices[7]).r > 0) ? 1:0;

	const int edeges_to_vertices[24] = int[24](0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 1,5, 2,6, 3,7);
	int triangleNumber = num_vec[index];
	for(int i =0; i< triangleNumber;i++){
		for(int j=0; j<3;j++){
			int p1 = vertex_to_edge[triangleNumber*15+i*3+j];
			gl_Position = vec4(oPos[0].xyz + (vertices[edeges_to_vertices[p1*2]]+vertices[edeges_to_vertices[p1*2+1]])/2-texpos, 1.0);
			EmitVertex();
		}
		EndPrimitive();
	}
}*/
void main() {
	gColor = color[0]; // Point has only one vertex
	gl_Position = gl_in[0].gl_Position + vec4(0.0,0.1,0.0,0.0);
	pos = gl_in[0].gl_Position + vec4(0.0,0.1,0.0,0.0);
	EmitVertex();

	gColor = color[0];
	gl_Position = gl_in[0].gl_Position + vec4(0.0,-0.1,0.0,0.0);
	pos = gl_in[0].gl_Position + vec4(0.0,-0.1,0.0,0.0);
	EmitVertex();

	EndPrimitive();

	gColor = color[0]; // Point has only one vertex
	gl_Position = gl_in[0].gl_Position + vec4(0.1,0.0,0.0,0.0);
	pos = gl_in[0].gl_Position + vec4(0.1,0.0,0.0,0.0);
	EmitVertex();

	gColor = color[0];
	gl_Position = gl_in[0].gl_Position + vec4(-0.1,0.0,0.0,0.0);
	pos = gl_in[0].gl_Position + vec4(-0.1,0.0,0.0,0.0);
	EmitVertex();

	EndPrimitive();

