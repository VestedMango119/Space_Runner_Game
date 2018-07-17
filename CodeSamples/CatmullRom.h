#pragma once
#include "Common.h"
#include "vertexBufferObject.h"

#include "Texture.h"


class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	void setPathWidth(float amount);
	float getHalfPathWidth();

	void CreateCentreline();
	void RenderCentreline();
	glm::vec3 getCenterLinePoint(int &index);
	int getCenterLinePointSize();

	void CreateOffsetCurves();
	void RenderOffsetCurves();
	float getLeftOffset(int &index);
	float getRightOffset(int &index);

	void CreateWallCurves();
	void RenderWallCurves();

	glm::vec3 getPathNorm(int index); //returns the normal to the path at each centre line point.

	void LoadPathTexture(string filename);

	void LoadWallTexture(string filename);

	void CreateTrack();
	void RenderTrack();
	void CreateWalls();
	void RenderWalls();

	int CurrentLap(float d); // Return the currvent lap (starting from 0) based on distance along the control curve.

	bool Sample(float d, glm::vec3 &p, glm::vec3 &up = glm::vec3(0, 0, 0)); // Return a point on the centreline based on a certain distance along the control curve.

private:

	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);


	vector<float> m_distances;
	CTexture m_pathTexture;
	CTexture m_textureNormal;

	CTexture m_wallTexture;
	CTexture m_wallNormal;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoTrack;

	GLuint m_vaoLeftWallCurve;
	GLuint m_vaoRightWallCurve;
	GLuint m_vaoWalls[2];

	vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	vector<glm::vec3> m_centrelinePoints;	// Centreline points
	vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors

	vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	vector<glm::vec3> m_rightOffsetPoints;	// Right offset curve points

	vector<glm::vec3> m_leftWallPoints;
	vector<glm::vec3> m_rightWallPoints;

	vector<glm::vec3> N;



	unsigned int m_vertexCount = 0;
	unsigned int m_leftWallVertexCount = 0;
	unsigned int m_rightWallVertexCount = 0;// Number of vertices in the track VBO

	float pathWidth;
	float wallHeight;
};
