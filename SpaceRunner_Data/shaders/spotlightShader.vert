#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
	mat4 inverseViewMatrix;
} matrices;



// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading
out vec2 vTexCoord;	// Texture coordinate
out vec3 worldPosition;
out vec4 vEyePosition; 
out vec3 vEyeNorm;

out vec3 reflected;

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.




// This is the entry point into the vertex shader
void main()
{	
	worldPosition = inPosition;

	// Transform the vertex spatial position using 
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);

	vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	
	// Pass through the texture coordinate
	vTexCoord = inCoord;

	//This code is based on the reflectance example code in Lecture 8
	reflected = (matrices.inverseViewMatrix *vec4(reflect(vEyePosition.xyz, vEyeNorm), 1)).xyz;

} 
	