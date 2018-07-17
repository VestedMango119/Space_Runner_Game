#include "Common.h"
#include "Tetrahedron.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CTetrahedron::CTetrahedron() {}
CTetrahedron::~CTetrahedron() { }

void CTetrahedron::Create(string directory, string filename, string filename2, float baseHeight, float height, float textureRepeat) {

	//load the main texture
	m_texture.Load(directory + filename, true);

	m_directory = directory; 
	m_filename[0] = filename; 

	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


	//load the base texture
	m_baseTexture.Load(directory + filename2, true);
	m_filename[1] = filename2;

	m_baseTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_baseTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_baseTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	m_baseTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	//determine the tetrahedron coordinates
	m_baseHeight = baseHeight;
	m_height = height;

	//calculate the apothem to create an equilateral triangle with centre point at the origin
	//calculation based on equilateral triangle properties found at https://en.wikipedia.org/wiki/Equilateral_triangle
	float halfHeight = 0.5f * m_height;
	float halfSideLength = m_baseHeight / 2.0f;
	float apothem = sqrt(3.0f)*m_height / 8.0f; // length from centre of triangle to midpoint of length

		//tetra equilateral 143.3										// Vertex Positions
	glm::vec3 tetraVertices[4] = {
		glm::vec3(halfSideLength, halfHeight, -apothem),
		glm::vec3(-halfSideLength, halfHeight, -apothem),
		glm::vec3(0, halfHeight, 3.0*apothem),
		glm::vec3(0.0f, -halfHeight, 0.0f)
	};

	int faceIndex[] = {
		0, 1, 2, //base face
		2, 3, 0,
		1, 2, 3,
		0, 3, 1
	};

	glm::vec2 texCoords[3] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, textureRepeat*0.5),
		glm::vec2(textureRepeat*0.5, 0.0f),
	};

	//calculate normals to each plane of tetra
	glm::vec3 tetraNormals[4] = {
		glm::vec3(0.0f, 1.0f, 0.0f), // base normal
		CalculateSurfaceNormal(tetraVertices[3], tetraVertices[0], tetraVertices[1]), 
		CalculateSurfaceNormal(tetraVertices[3], tetraVertices[2], tetraVertices[1]),
		CalculateSurfaceNormal(tetraVertices[3], tetraVertices[2], tetraVertices[0])
	};


	
	//put the vertex attributes in the VBO
	for (unsigned int j = 0; j < 4; j++) {
		for (unsigned int i = 0; i < 3; i++) {
			m_vbo.AddData(&tetraVertices[faceIndex[(j * 3) + i]], sizeof(glm::vec3));
			m_vbo.AddData(&texCoords[i], sizeof(glm::vec2));
			m_vbo.AddData(&tetraNormals[j], sizeof(glm::vec3));
		}
	}
	

	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));

	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CTetrahedron::Render() {
	
		glBindVertexArray(m_vao);

		//render the base equilateral
		m_baseTexture.Bind();
		glEnable(GL_TEXTURE_2D);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		
		//render the other triangles
		m_texture.Bind();
		glEnable(GL_TEXTURE_2D);
		glDrawArrays(GL_TRIANGLE_STRIP, 3, 9);

}

void CTetrahedron::Release() {
	m_baseTexture.Release();
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
	
}

//calculate the surface normals to the non-base faces of the tetrahedron
glm::vec3 CTetrahedron::CalculateSurfaceNormal(glm::vec3 point, glm::vec3 base1, glm::vec3 base2) {
	glm::vec3 v1 = base1 - point; 
	glm::vec3 v2 = base2 - point; 

	return glm::cross(v1, v2);

}