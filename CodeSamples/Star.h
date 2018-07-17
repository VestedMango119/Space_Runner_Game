#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

class CStar {
public:
	CStar();
	~CStar();
	void Create(float size);
	glm::vec3 CalculateSurfaceNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
	void Render();
	void Release();

private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	float size;

};