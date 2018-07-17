#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}



CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	//first curve
	m_controlPoints.push_back(glm::vec3(-90.0, 5.0, -110.0));
	m_controlPoints.push_back(glm::vec3(-110.0, 5.0, -90.0));
	m_controlPoints.push_back(glm::vec3(-110.0, 5.0, -70.0));
	m_controlPoints.push_back(glm::vec3(-100.0, 5.0, -50.0));
	m_controlPoints.push_back(glm::vec3(-80.0, 5.0, -40.0));

	//second curve
	m_controlPoints.push_back(glm::vec3(-50.0, 5.0, -30.0));
	m_controlPoints.push_back(glm::vec3(-30.0, 5.0, -10.0));
	m_controlPoints.push_back(glm::vec3(-30.0, 5.0, 10.0));

	//third curve
	m_controlPoints.push_back(glm::vec3(-50.0, 5.0, 30.0));
	m_controlPoints.push_back(glm::vec3(-70.0, 5.0, 50.0));
	m_controlPoints.push_back(glm::vec3(-80.0, 5.0, 80.0));
	m_controlPoints.push_back(glm::vec3(-70.0, 10.0, 110.0));
	m_controlPoints.push_back(glm::vec3(-50.0, 10.0, 130.0));

	//straight
	m_controlPoints.push_back(glm::vec3(-30.0, 15.0, 140.0));
	m_controlPoints.push_back(glm::vec3(-10.0, 15.0, 140.0));
	m_controlPoints.push_back(glm::vec3(10.0, 20.0, 140.0));
	m_controlPoints.push_back(glm::vec3(30.0, 20.0, 140.0));

	//wiggle
	m_controlPoints.push_back(glm::vec3(50.0, 20.0, 130.0));
	m_controlPoints.push_back(glm::vec3(70.0, 20.0, 100.0));
	m_controlPoints.push_back(glm::vec3(50.0, 20.0, 70.0));
	m_controlPoints.push_back(glm::vec3(70.0, 20.0, 40.0));
	m_controlPoints.push_back(glm::vec3(50.0, 20.0, 10.0));

	//loop the loop
	m_controlPoints.push_back(glm::vec3(30.0, 20.0, -10.0));
	m_controlPoints.push_back(glm::vec3(10.0, 20.0, -30.0));
	m_controlPoints.push_back(glm::vec3(0.0, 20.0, -60.0));
	m_controlPoints.push_back(glm::vec3(10.0, 15.0, -100.0));
	m_controlPoints.push_back(glm::vec3(20.0, 15.0, -120.0));
	m_controlPoints.push_back(glm::vec3(40.0, 15.0, -130.0));
	m_controlPoints.push_back(glm::vec3(60.0, 10.0, -120.0));
	m_controlPoints.push_back(glm::vec3(70.0, 10.0, -90.0));
	m_controlPoints.push_back(glm::vec3(60.0, 5.0, -50.0));
	m_controlPoints.push_back(glm::vec3(30.0, 5.0, -60.0));
	m_controlPoints.push_back(glm::vec3(10.0, 5.0, -90.0));
	m_controlPoints.push_back(glm::vec3(-10.0, 5.0, -100.0));
	m_controlPoints.push_back(glm::vec3(-30.0, 5.0, -110.0));
	m_controlPoints.push_back(glm::vec3(-60.0, 5.0, -120.0));





	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
}


void CCatmullRom::setPathWidth(float amount) {
	pathWidth = amount;
	wallHeight = amount / 10.0f;
}

/*float CCatmullRom::getPathWidth() {
return pathWidth;
}
*/
// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}

}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();

	int numSamples = 500;

	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(numSamples);
	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < numSamples + 1; i++)
	{
		vbo.AddData(&m_centrelinePoints[i%numSamples], sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	glm::vec3 p;
	glm::vec3 pNext;

	glm::vec3 T; //tangent
	glm::vec3 B; //binormal

	glm::vec3 y(0.0f, 1.0f, 0.0f);

	int size = m_centrelinePoints.size();

	for (int i = 0; i < size; i++) {
		p = m_centrelinePoints[i];

		pNext = m_centrelinePoints[(i + 1) % size];

		T = glm::normalize(pNext - p);
		N.push_back(glm::cross(T, y)); //store the normal to be accessed by the game file when placing objects on the path.
		B = glm::cross(N[i], T);

		glm::vec3 left = p - (pathWidth / 2) * N[i];
		glm::vec3 right = p + (pathWidth / 2) * N[i];

		m_leftOffsetPoints.push_back(left);
		m_rightOffsetPoints.push_back(right);

	}



	GLuint* VAO[2];
	VAO[0] = &m_vaoLeftOffsetCurve;
	VAO[1] = &m_vaoRightOffsetCurve;

	CVertexBufferObject VBO[2];


	glm::vec2 texCoord = glm::vec2(0.0, 0.0);

	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	glGenVertexArrays(1, VAO[0]);
	glBindVertexArray(*VAO[0]);
	VBO[0].Create();
	VBO[0].Bind();
	for (int i = 0; i < size + 1; i++)
	{
		VBO[0].AddData(&m_leftOffsetPoints[i%size], sizeof(glm::vec3));
		VBO[0].AddData(&texCoord, sizeof(glm::vec2));
		VBO[0].AddData(&normal, sizeof(glm::vec3));
	}
	//VBO[0].AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));

	VBO[0].UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));



	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.

	glGenVertexArrays(1, VAO[1]);
	glBindVertexArray(*VAO[1]);
	VBO[1].Create();
	VBO[1].Bind();
	for (int i = 0; i < size + 1; i++)
	{
		VBO[1].AddData(&m_rightOffsetPoints[i%size], sizeof(glm::vec3));
		VBO[1].AddData(&texCoord, sizeof(glm::vec2));
		VBO[1].AddData(&normal, sizeof(glm::vec3));
	}


	VBO[1].UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


}

void CCatmullRom::CreateWallCurves() {
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	glm::vec3 pLeft;
	glm::vec3 pLeftNext;

	glm::vec3 pRight;
	glm::vec3 pRightNext;

	glm::vec3 T_left; //tangent
	glm::vec3 N_left; //normal
	glm::vec3 B_left; //binormal

	glm::vec3 T_right; //tangent
	glm::vec3 N_right; //normal
	glm::vec3 B_right; //binormal

	glm::vec3 y(0.0f, 1.0f, 0.0f);

	int size = m_centrelinePoints.size();

	for (int i = 0; i < size; i++) {
		pLeft = m_leftOffsetPoints[i];
		pLeftNext = m_leftOffsetPoints[(i + 1) % size];

		pRight = m_rightOffsetPoints[i];
		pRightNext = m_rightOffsetPoints[(i + 1) % size];

		T_left = glm::normalize(pLeftNext - pLeft);
		N_left = glm::cross(T_left, y);
		B_left = glm::cross(N_left, T_left);

		T_right = glm::normalize(pRightNext - pRight);
		N_right = glm::cross(T_right, y);
		B_right = glm::cross(N_right, T_right);

		glm::vec3 left = pLeft + wallHeight * B_left;
		glm::vec3 right = pRight + wallHeight * B_right;

		m_leftWallPoints.push_back(left);
		m_rightWallPoints.push_back(right);

	}



	GLuint* VAO[2];
	VAO[0] = &m_vaoLeftWallCurve;
	VAO[1] = &m_vaoRightWallCurve;

	CVertexBufferObject VBO[2];


	glm::vec2 texCoord = glm::vec2(0.0, 0.0);

	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	glGenVertexArrays(1, VAO[0]);
	glBindVertexArray(*VAO[0]);
	VBO[0].Create();
	VBO[0].Bind();
	for (int i = 0; i < size + 1; i++)
	{
		VBO[0].AddData(&m_leftWallPoints[i%size], sizeof(glm::vec3));
		VBO[0].AddData(&texCoord, sizeof(glm::vec2));
		VBO[0].AddData(&normal, sizeof(glm::vec3));
	}
	//VBO[0].AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));

	VBO[0].UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));



	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.

	glGenVertexArrays(1, VAO[1]);
	glBindVertexArray(*VAO[1]);
	VBO[1].Create();
	VBO[1].Bind();
	for (int i = 0; i < size + 1; i++)
	{
		VBO[1].AddData(&m_rightWallPoints[i%size], sizeof(glm::vec3));
		VBO[1].AddData(&texCoord, sizeof(glm::vec2));
		VBO[1].AddData(&normal, sizeof(glm::vec3));
	}


	VBO[1].UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


}

void CCatmullRom::RenderWallCurves() {

	int size = m_centrelinePoints.size();

	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftWallCurve);
	glPointSize(2.0f);
	glLineWidth(1.0f);
	glDrawArrays(GL_POINTS, 0, size);
	glDrawArrays(GL_LINE_STRIP, 0, size + 1);

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightWallCurve);
	glPointSize(2.0f);
	glLineWidth(1.0f);
	glDrawArrays(GL_POINTS, 0, size);
	glDrawArrays(GL_LINE_STRIP, 0, size + 1);
}


void CCatmullRom::CreateTrack()
{
	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	CVertexBufferObject VBO;
	VBO.Create();
	VBO.Bind();

	int size = m_leftOffsetPoints.size();


	int vertexNum = 11;
	int halfVertexNum = vertexNum / 2;
	vector<glm::vec2> texCoord;
	for (int i = 0; i < vertexNum / 2; i++) {
		texCoord.push_back(glm::vec2(0.0, (float)i / 10));
		texCoord.push_back(glm::vec2(1.0, (float)i / 10));
	}


	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < size + 1; i += halfVertexNum - 1) {
		for (int j = 0; j < halfVertexNum; j++) {
			VBO.AddData(&m_leftOffsetPoints[(i + j) % size], sizeof(glm::vec3));
			VBO.AddData(&texCoord[(2 * j) % vertexNum], sizeof(glm::vec2));
			VBO.AddData(&normal, sizeof(glm::vec3));

			VBO.AddData(&m_rightOffsetPoints[(i + j) % size], sizeof(glm::vec3));
			VBO.AddData(&texCoord[((2 * j) + 1) % vertexNum], sizeof(glm::vec2));
			VBO.AddData(&normal, sizeof(glm::vec3));

			m_vertexCount += 2;
		}
	}

	VBO.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

void CCatmullRom::CreateWalls() {
	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

	glGenVertexArrays(1, &m_vaoWalls[0]);
	glBindVertexArray(m_vaoWalls[0]);

	CVertexBufferObject VBO[2];
	VBO[0].Create();
	VBO[0].Bind();

	int size = m_leftWallPoints.size();


	int vertexNum = 14;
	vector<glm::vec2> texCoord;
	for (int i = 0; i < vertexNum / 2; i++) {
		texCoord.push_back(glm::vec2((float)i / 6, 0.0));
		texCoord.push_back(glm::vec2((float)i / 6, 0.5));
	}


	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < size + 1; i += 6) {
		for (int j = 0; j < 7; j++) {

			VBO[0].AddData(&m_leftOffsetPoints[(i + j) % size], sizeof(glm::vec3));
			VBO[0].AddData(&texCoord[(2 * j) % vertexNum], sizeof(glm::vec2));
			VBO[0].AddData(&normal, sizeof(glm::vec3));

			VBO[0].AddData(&m_leftWallPoints[(i + j) % size], sizeof(glm::vec3));
			VBO[0].AddData(&texCoord[((2 * j) + 1) % vertexNum], sizeof(glm::vec2));
			VBO[0].AddData(&normal, sizeof(glm::vec3));

			m_leftWallVertexCount += 2;
		}
	}

	VBO[0].UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	glGenVertexArrays(1, &m_vaoWalls[1]);
	glBindVertexArray(m_vaoWalls[1]);

	VBO[1].Create();
	VBO[1].Bind();

	for (int i = 0; i < size + 1; i += 6) {
		for (int j = 0; j < 7; j++) {
			VBO[1].AddData(&m_rightWallPoints[(i + j) % size], sizeof(glm::vec3));
			VBO[1].AddData(&texCoord[(2 * j) % vertexNum], sizeof(glm::vec2));
			VBO[1].AddData(&normal, sizeof(glm::vec3));

			VBO[1].AddData(&m_rightOffsetPoints[(i + j) % size], sizeof(glm::vec3));
			VBO[1].AddData(&texCoord[((2 * j) + 1) % vertexNum], sizeof(glm::vec2));
			VBO[1].AddData(&normal, sizeof(glm::vec3));

			m_rightWallVertexCount += 2;
		}
	}

	VBO[1].UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);

	int size = m_centrelinePoints.size();
	glPointSize(2.0f);
	glLineWidth(1.0f);
	glDrawArrays(GL_POINTS, 0, size);
	glDrawArrays(GL_LINE_STRIP, 0, size + 1);

}

void CCatmullRom::LoadPathTexture(string filename) {

	m_pathTexture.Load(filename);

	m_pathTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_pathTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_pathTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	m_pathTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

}

void CCatmullRom::LoadWallTexture(string filename)
{
	m_wallTexture.Load(filename);

	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_IBM);
	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_IBM);

}


void CCatmullRom::RenderOffsetCurves()
{
	int size = m_centrelinePoints.size();

	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glPointSize(2.0f);
	glLineWidth(1.0f);
	glDrawArrays(GL_POINTS, 0, size);
	glDrawArrays(GL_LINE_STRIP, 0, size + 1);

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glPointSize(2.0f);
	glLineWidth(1.0f);
	glDrawArrays(GL_POINTS, 0, size);
	glDrawArrays(GL_LINE_STRIP, 0, size + 1);
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	glBindVertexArray(m_vaoTrack);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_pathTexture.Bind();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glLineWidth(0.1f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
	glEnable(GL_CULL_FACE);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void CCatmullRom::RenderWalls() {
	glBindVertexArray(m_vaoWalls[0]);
	m_wallTexture.Bind();
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_leftWallVertexCount);

	glBindVertexArray(m_vaoWalls[1]);
	m_wallTexture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_rightWallVertexCount);

	glEnable(GL_CULL_FACE);

	glDisable(GL_BLEND);
}

//return the current lap based onthe current and total distances
int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

//return the centre line point size
int CCatmullRom::getCenterLinePointSize() {
	return m_centrelinePoints.size();
}

//return the center line points for access to place game objects along the path
glm::vec3 CCatmullRom::getCenterLinePoint(int &index) {
	int size = getCenterLinePointSize();
	return m_centrelinePoints[index%size];
}

//return the distance between the left offset point at a specified index, to the centre line point at the same index
float CCatmullRom::getLeftOffset(int &index) {
	int size = getCenterLinePointSize();
	return glm::distance(m_leftOffsetPoints[index%size], m_centrelinePoints[index%size]);
}

//return the distance between the right offset point at a specified index, to the centre line point at the same index
float CCatmullRom::getRightOffset(int &index) {
	int size = getCenterLinePointSize();
	return glm::distance(m_rightOffsetPoints[index%size], m_centrelinePoints[index%size]);
}

//return hlaf the path width to limit objects along the path
float CCatmullRom::getHalfPathWidth() {
	return pathWidth / 2;
}

//return the normal vector of the path at point 'index' along the center line
glm::vec3 CCatmullRom::getPathNorm(int index) {
	return N[index];
}