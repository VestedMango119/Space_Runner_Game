#pragma once


#include "Texture.h"
#include "VertexBufferObject.h"

class CTetrahedron {
public:
	CTetrahedron();
	~CTetrahedron();
	void Create(string directory, string filename, string filename2, float baseHeight, float height, float textureRepeat);
	glm::vec3 CalculateSurfaceNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
	void Render();
	void Release();

private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	CTexture m_baseTexture;
	string m_directory;
	string m_filename[2];
	float m_baseHeight;
	float m_height;
};