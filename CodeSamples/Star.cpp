#include "Star.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CStar::CStar() {

}

CStar::~CStar() {

}

void CStar::Create(float size) {

	float squareHalf = size / 10.0f;
	float pointSize = size / 2.0f;
	float width = size / 3.0f;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

	//create vertex points around the origin
	//star made up of cube with points on each face.
	glm::vec3 starVertices[14] = {
		glm::vec3(-squareHalf, -squareHalf, squareHalf), //0
		glm::vec3(squareHalf, -squareHalf, squareHalf), //1
		glm::vec3(-squareHalf, squareHalf, squareHalf), //2
		glm::vec3(squareHalf, squareHalf, squareHalf), //3

		glm::vec3(-squareHalf, squareHalf, -squareHalf), //4
		glm::vec3(squareHalf, squareHalf, -squareHalf), //5
		glm::vec3(-squareHalf, -squareHalf, -squareHalf), //6
		glm::vec3(squareHalf, -squareHalf, -squareHalf),//7

		glm::vec3(0, pointSize, 0), //8
		glm::vec3(0, -pointSize, 0), //9
		glm::vec3(width,0, 0 ),   //10
		glm::vec3(-width, 0, 0), //11
		glm::vec3(0, 0, width),  //12
		glm::vec3(0, 0, -width)  //13
		//add point coordinates
	};

	int faceIndex[] = {
		8, 2, 3,
		8, 3, 5,
		8, 5, 4,
		8, 4, 2,

		9, 0, 6, 
		9, 6, 7, 
		9, 7, 1, 
		9, 1, 0,

		11, 0, 2, 
		11, 2, 4, 
		11, 4, 6, 
		11, 6, 0, 

		10, 3, 1,
		10, 1, 7, 
		10, 7, 5,
		10, 5, 3,

		12, 0, 1, 
		12, 1, 3, 
		12, 3, 2, 
		12, 2, 0, 

		13, 4, 5, 
		13, 5, 7, 
		13, 7, 6, 
		13, 6, 4
	};

	glm::vec2 texCoords = glm::vec2(0.0, 0.0);

	//calculate normals to each plane of star
	vector<glm::vec3> starNormals; 
	for (int i = 0; i < 72; i += 3) {
		starNormals.push_back(CalculateSurfaceNormal(starVertices[faceIndex[i]], starVertices[faceIndex[i + 1]], starVertices[faceIndex[i + 2]]));
	}


	//put the vertex attributes in the VBO
	for (unsigned int j = 0; j < 24; j++) {
		for (unsigned int i = 0; i < 3; i++) {
			m_vbo.AddData(&starVertices[faceIndex[(j * 3) + i]], sizeof(glm::vec3));
			m_vbo.AddData(&texCoords, sizeof(glm::vec2));
			m_vbo.AddData(&starNormals[j], sizeof(glm::vec3));
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


void CStar::Render() {

	glBindVertexArray(m_vao);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, 72);
	
	glEnable(GL_CULL_FACE);
}

void CStar::Release() {
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();

}

//calculate the surface normal of a triangle
glm::vec3 CStar::CalculateSurfaceNormal(glm::vec3 point, glm::vec3 v1, glm::vec3 v2){

	glm::vec3 line1 = point - v1; 
	glm::vec3 line2 = point - v2; 

	return glm::normalize(glm::cross(line1, line2));

}