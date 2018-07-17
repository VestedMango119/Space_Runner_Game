#include "Cube.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CCube::CCube() {

}

CCube::~CCube() {

}

void CCube::Create(float size, string directory, string filename) {

	m_texture.Load(directory + filename, true);

	m_directory = directory;
	m_filename = filename;

	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	float squareHalf = size * 0.5f;


	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	glm::vec3 cubeVertices[8] = {
		glm::vec3(-squareHalf, -squareHalf, squareHalf), //0
		glm::vec3(squareHalf, -squareHalf, squareHalf), //1
		glm::vec3(-squareHalf, squareHalf, squareHalf), //2
		glm::vec3(squareHalf, squareHalf, squareHalf), //3

		glm::vec3(-squareHalf, -squareHalf, -squareHalf), //4
		glm::vec3(squareHalf, -squareHalf, -squareHalf), //5
		glm::vec3(-squareHalf, squareHalf, -squareHalf), //6
		glm::vec3(squareHalf, squareHalf, -squareHalf),//7
	};

	//Representing the triangles of each face of the cube made up of indexes for the 
	//cube vertices coordinates vector.
	int faceIndex[] = {
		0, 1, 3,
		3, 2, 0,

		1, 5, 7,
		7, 3, 1,

		4, 5, 1,
		1, 0, 4,

		5, 4, 6,
		6, 7, 5,

		4, 0, 2,
		2, 6, 4,

		2, 3, 7,
		7, 6, 2

	};

	glm::vec2 texCoords[6] = {
		glm::vec2(0.0, 0.0),
		glm::vec2(1.0, 0.0),
		glm::vec2(1.0, 1.0),


		glm::vec2(1.0, 1.0),
		glm::vec2(0.0, 1.0),
		glm::vec2(0.0, 0.0)
	};

	//calculate normals to each plane of cube
	vector<glm::vec3> cubeNormals;
	for (int i = 0; i < 36; i += 6) {
		cubeNormals.push_back(CalculateSurfaceNormal(cubeVertices[faceIndex[i]], cubeVertices[faceIndex[i + 1]], cubeVertices[faceIndex[i + 2]]));
	}


	//put the vertex attributes in the VBO
	for (unsigned int j = 0; j < 6; j++) {
		for (unsigned int i = 0; i < 6; i++) {
			m_vbo.AddData(&cubeVertices[faceIndex[(j * 6) + i]], sizeof(glm::vec3));
			m_vbo.AddData(&texCoords[i], sizeof(glm::vec2));
			m_vbo.AddData(&cubeNormals[j], sizeof(glm::vec3));
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


void CCube::Render() {

	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


void CCube::Release() {
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();

}

//Calculate the surface normal of a triangle.
glm::vec3 CCube::CalculateSurfaceNormal(glm::vec3 point, glm::vec3 v1, glm::vec3 v2) {

	glm::vec3 line1 = point - v1;
	glm::vec3 line2 = point - v2;

	return glm::normalize(glm::cross(line1, line2));

}