#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec2 vTexCoord;

out vec4 vEyePosition; 
out vec3 vEyeNorm;

void main()
{	
	vTexCoord = inCoord;
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0);
	vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0);
} 
