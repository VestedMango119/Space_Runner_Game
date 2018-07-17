#pragma once

#include "Common.h"
#include "GameWindow.h"



// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CCatmullRom;
class CTetrahedron;
class CDoor;
class CPlayer;
class CStar;
class CFrameBufferObject;
class CCube;
class CAudio;


class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	//void Render();
	void RenderScene();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh * m_pGrassMesh;
	COpenAssetImportMesh * m_pStatue;
	COpenAssetImportMesh * m_pTree;
	COpenAssetImportMesh * m_pPlayerMesh;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CCatmullRom *m_pCatmullRom;
	CTetrahedron *m_pTetrahedron;
	CStar *m_pStar;
	CCube *m_pCube;
	CAudio * m_pAudio;
	CAudio *m_pStarSound;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	float m_currentDistance;
	bool lightMode;

public:
	Game();
	~Game();
	void Reset();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;

	int m_frameCount;
	double m_elapsedTime;


	glm::vec3 playerPos;
	glm::vec3 *enginePos;
	glm::mat4 playerOrientation;
	glm::mat3 soundorientation;
	float playerOffset;
	float playerSpeed = 0.4f;
	void GetPlayerMovement();

	glm::vec3 p;
	glm::vec3 pNext;
	vector<glm::vec3> starPos;
	vector<bool> collected;
	int collectedCount = 0;
	int playerScore = 0;

	vector<glm::vec3> cubePos;
	bool collided;

	int seconds = 0;
	int minutes = 0;

	vector<float> offset;
	vector<glm::vec3> offsetNorm;
	int numberGen = rand() % 5;

	int lap;

	void CheckPlayerScore(glm::vec3 T);

	glm::vec3 T;
	glm::vec3 N;
	glm::vec3 B;

	int pathLightCount;
	int lightGap;
	vector<glm::vec3> pathLightPos;

	glm::vec4 fontColour = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	bool restart = false;

	bool mute = false;
	bool HudOn = true;
};
