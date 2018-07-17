/*
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
- Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
- Christy Quinn, Sam Kellett and others

For educational use by Department of Computer Science, City University London UK.

This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting,
different camera controls, different shaders, etc.

Template version 5.0a 29/01/2017
Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk)
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "CatmullRom.h"
#include "Tetrahedron.h"
#include "Star.h"
#include "cube.h"
#include "Audio.h"



// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pFtFont = NULL;
	m_pGrassMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pCatmullRom = NULL;
	m_pTetrahedron = NULL;
	m_pStar = NULL;
	m_pPlayerMesh = NULL;
	m_pCube = NULL;
	m_pAudio = NULL;
	m_pStarSound = NULL;
	


	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	//begin game in dark mode
	lightMode = false;
}

 void Game::Reset() {
	 delete m_pCamera;
	 delete m_pSkybox;
	 delete m_pFtFont;
	 delete m_pGrassMesh;
	 delete m_pStatue;
	 delete m_pTree;
	 delete m_pSphere;
	 delete m_pCatmullRom;
	 delete m_pTetrahedron;
	 delete m_pStar;
	 delete m_pPlayerMesh;
	 delete m_pCube;
	 m_pAudio->StopMusicStream();
	 delete m_pAudio;



	 if (m_pShaderPrograms != NULL) {
		 for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			 delete (*m_pShaderPrograms)[i];
	 }
	 delete m_pShaderPrograms;

	 //setup objects
	 delete m_pHighResolutionTimer;

	 

}

// Destructor
Game::~Game()
{
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pFtFont;
	delete m_pGrassMesh;
	delete m_pStatue;
	delete m_pTree;
	delete m_pSphere;
	delete m_pCatmullRom;
	delete m_pTetrahedron;
	delete m_pStar;
	delete m_pPlayerMesh;
	delete m_pCube;
	delete m_pAudio;
	


	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;
}


// Initialisation:  This method only runs once at startup
void Game::Initialise()
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pFtFont = new CFreeTypeFont;
	m_pGrassMesh = new COpenAssetImportMesh;
	m_pStatue = new COpenAssetImportMesh;
	m_pTree = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pCatmullRom = new CCatmullRom;
	m_pTetrahedron = new CTetrahedron;
	m_pStar = new CStar;
	m_pPlayerMesh = new COpenAssetImportMesh;
	m_pCube = new CCube;
	m_pAudio = new CAudio;


	starPos.clear();
	cubePos.clear();
	offset.clear();
	offsetNorm.clear();
	collected.clear();

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height);
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("starShader.vert");
	sShaderFileNames.push_back("starShader.frag");
	sShaderFileNames.push_back("spotlightShader.vert");
	sShaderFileNames.push_back("spotlightShader.frag");


	for (int i = 0; i < (int)sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int)sShaderFileNames[i].size() - 4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\" + sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	//create the star shader program
	CShaderProgram *pStarProgram = new CShaderProgram;
	pStarProgram->CreateProgram();
	pStarProgram->AddShaderToProgram(&shShaders[4]);
	pStarProgram->AddShaderToProgram(&shShaders[5]);
	pStarProgram->LinkProgram();
	m_pShaderPrograms->push_back(pStarProgram);

	//create the spotlight shader program
	CShaderProgram *pSpotlightShader = new CShaderProgram;
	pSpotlightShader->CreateProgram();
	pSpotlightShader->AddShaderToProgram(&shShaders[6]);
	pSpotlightShader->AddShaderToProgram(&shShaders[7]);
	pSpotlightShader->LinkProgram();
	m_pShaderPrograms->push_back(pSpotlightShader);


	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);

	//create the CatmullRom spline and track
	m_pCatmullRom->setPathWidth(5.0f);
	m_pCatmullRom->LoadPathTexture("resources\\textures\\cyber-path.jpg");
	m_pCatmullRom->LoadWallTexture("resources\\textures\\cyber-walls.jpg");
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateWallCurves();
	m_pCatmullRom->CreateTrack();
	m_pCatmullRom->CreateWalls();

	//create the cube with texture
	m_pCube->Create(1.0f, "resources\\textures\\", "tt4.jpg");

	//create the textrahedron specifying two textures
	m_pTetrahedron->Create("resources\\textures\\", "tetra-texture-3.jpg", "short-grass.jpg", 100.0f, 100.0f, 20.0f);

	//create the stars
	m_pStar->Create(10.0f);

	//create the font for UI
	m_pFtFont->LoadFont("resources\\fonts\\Philosopher\\Philosopher-Bold.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pGrassMesh->Load("resources\\models\\grass-block\\grass-block3.obj");
	m_pPlayerMesh->Load("resources\\models\\player\\alienGlyder\\alienglyder\\alienglyder3.obj");
	m_pStatue->Load("resources\\models\\statue\\statue.obj");
	m_pTree->Load("resources\\models\\maple+Blend\\maple.obj");

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);

	m_pAudio->Initialise();
	m_pAudio->LoadBoxEventSound("resources\\audio\\little_robot_sound_factory_WarpDrive_03.wav");
	m_pAudio->LoadMusicStream("resources\\audio\\zapsplat_science_fiction_spacecraft_engines_drone_10569-MONO2.wav");
	m_pAudio->LoadStarEventSound("resources\\audio\\170147__timgormly__8-bit-coin-MONO.wav");
	m_pAudio->PlayMusicStream();


	//get the normal values to the catmullrom path
	for (int i = 0; i < m_pCatmullRom->getCenterLinePointSize(); i++) {
		offsetNorm.push_back(m_pCatmullRom->getPathNorm(i));
	}

	//create random offset values
	for (int i = 0; i < 100; i += 3) {
		int r = -1 + rand() % (int)(1 - (-1) + 1);
		offset.push_back((float)r);
		offset.push_back((float)r);
		offset.push_back((float)r);
	}


	//for every 6th point in the spline, randomly set 0 - 5 stars using the random offset values multiplied along the normal of the catmullrom path.
	for (int i = 0; i < m_pCatmullRom->getCenterLinePointSize(); i += 6) {

		int r = rand() % 5;

		for (int j = 0; j < r; j += 1) {
			int index = i + j;
			glm::vec3 Pos = m_pCatmullRom->getCenterLinePoint(index);
			starPos.push_back(Pos + glm::vec3(0.0, 0.3f, 0.0) + offset[i % 100] * offsetNorm[i]);
			collected.push_back(false);
		}

		//randomly set locations for the cubes along the path
		int index = i + r;
		if (offset[(i + 25) % 100] < m_pCatmullRom->getHalfPathWidth())
			cubePos.push_back(m_pCatmullRom->getCenterLinePoint(index) + glm::vec3(0.0, 0.5, 0.0) + offset[(i + 25) % 100] * offsetNorm[i]);
		else {
			cubePos.push_back(m_pCatmullRom->getCenterLinePoint(index) + glm::vec3(0.0, 0.5, 0.0) + offset[(i + 1) % 100] * offsetNorm[i]);
		}
	}

	seconds = 0;
	minutes = 0;
	playerScore = 0;
	int index = 0;
	playerPos = m_pCatmullRom->getCenterLinePoint(index);
	m_pCamera->setSpeed(0);
	lap = 0;
	m_currentDistance = 0;
	collectedCount = 0;
}



// Render method runs repeatedly in a loop
void Game::RenderScene()
{

	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();


	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("renderSkybox", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("normalMap", 0);
	pMainProgram->SetUniform("activate", lightMode);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);


	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());



	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	glm::vec4 lightPosition1 = glm::vec4(0, 100, -100, 1); // Position of light source *in world coordinates*

	glm::vec4 lightPosition3 = glm::vec4(playerPos + glm::vec3(0.0f, 1.0f, 0.0f), 1);

	CShaderProgram *pSpotlightProgram = (*m_pShaderPrograms)[3];
	pSpotlightProgram->UseProgram();
	pSpotlightProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	pSpotlightProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	pSpotlightProgram->SetUniform("sampler0", 0);
	pSpotlightProgram->SetUniform("bUseTexture", true);
	pSpotlightProgram->SetUniform("bumpMapActive", false);
	pSpotlightProgram->SetUniform("activate", !lightMode);
	pSpotlightProgram->SetUniform("collided", collided);
	pSpotlightProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	
	//main lighting
	pSpotlightProgram->SetUniform("light1.La", glm::vec3(0.8f));
	pSpotlightProgram->SetUniform("light1.Ld", glm::vec3(0.8f));		// Diffuse colour of light
	pSpotlightProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light	
	pSpotlightProgram->SetUniform("material1.shininess", 100.0f);
	pSpotlightProgram->SetUniform("material1.Ma", glm::vec3(0.5f));
	pSpotlightProgram->SetUniform("material1.Md", glm::vec3(0.5f));
	pSpotlightProgram->SetUniform("material1.Ms", glm::vec3(0.5));

	//statue lights
	glm::vec4 lightPosition2(0, 10, 20, 1);
	pSpotlightProgram->SetUniform("light2.position", viewMatrix*lightPosition2); // Light position in eye coordinates
	pSpotlightProgram->SetUniform("light2.La", glm::vec3(1.0f, 0.8f, 0.0f));
	pSpotlightProgram->SetUniform("light2.Ld", glm::vec3(1.0f, 0.8f, 0.0f));
	pSpotlightProgram->SetUniform("light2.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light2.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, 1, -1)));
	pSpotlightProgram->SetUniform("light2.exponent", 30.0f);
	pSpotlightProgram->SetUniform("light2.cutoff", 30.0f);
	glm::vec4 lightPosition5(-5, 5, 10, 1);
	pSpotlightProgram->SetUniform("light5.position", viewMatrix*lightPosition5); // Light position in eye coordinates
	pSpotlightProgram->SetUniform("light5.La", glm::vec3(1.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light5.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light5.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light5.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, 1, -1)));
	pSpotlightProgram->SetUniform("light5.exponent", 30.0f);
	pSpotlightProgram->SetUniform("light5.cutoff", 30.0f);
	glm::vec4 lightPosition6(5, 5, 10, 1);
	pSpotlightProgram->SetUniform("light6.position", viewMatrix*lightPosition6); // Light position in eye coordinates
	pSpotlightProgram->SetUniform("light6.La", glm::vec3(1.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light6.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light6.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light6.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, 1, -1)));
	pSpotlightProgram->SetUniform("light6.exponent", 30.0f);
	pSpotlightProgram->SetUniform("light6.cutoff", 30.0f);

	//player light
	pSpotlightProgram->SetUniform("light3.position", viewMatrix*lightPosition3);
	pSpotlightProgram->SetUniform("light3.La", glm::vec3(0.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light3.Ld", glm::vec3(0.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light3.Ls", glm::vec3(1.0f));
	pSpotlightProgram->SetUniform("light3.direction", glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	pSpotlightProgram->SetUniform("light3.exponent", 25.0f);
	pSpotlightProgram->SetUniform("light3.cutoff", 30.0f);

	//player headlight
	pSpotlightProgram->SetUniform("light4.La", glm::vec3(0.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light4.Ld", glm::vec3(0.0f, 1.0f, 1.0f));
	pSpotlightProgram->SetUniform("light4.Ls", glm::vec3(1.0f));
	pSpotlightProgram->SetUniform("light4.position", viewMatrix*lightPosition3);
	pSpotlightProgram->SetUniform("light4.direction", glm::normalize(viewNormalMatrix*glm::normalize(glm::vec3(T) + glm::vec3(0.0, -0.3, 0.0))));
	pSpotlightProgram->SetUniform("light4.exponent", 10.0f);
	pSpotlightProgram->SetUniform("light4.cutoff", 40.0f);

	//tree lights
	glm::vec3 ambL(0.2f);
	glm::vec3 diffL(1.0f, 0.0f, 1.0f);
	glm::vec3 specL(1.0f);
	glm::vec3 norm(0, -1, 0);
	float exponent = 10.0f;
	float cutoff = 50.0f;
	pSpotlightProgram->SetUniform("tree[0].La", ambL);
	pSpotlightProgram->SetUniform("tree[0].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[0].Ls", specL);
	pSpotlightProgram->SetUniform("tree[0].position", viewMatrix*glm::vec4(-9, 10.5f, 7, 1));
	pSpotlightProgram->SetUniform("tree[0].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[0].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[0].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[1].La", ambL);
	pSpotlightProgram->SetUniform("tree[1].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[1].Ls", specL);
	pSpotlightProgram->SetUniform("tree[1].position", viewMatrix*glm::vec4(9, 10.0f, 14, 1));
	pSpotlightProgram->SetUniform("tree[1].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[1].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[1].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[2].La", ambL);
	pSpotlightProgram->SetUniform("tree[2].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[2].Ls", specL);
	pSpotlightProgram->SetUniform("tree[2].position", viewMatrix*glm::vec4(-9, 10.5f, 21, 1));
	pSpotlightProgram->SetUniform("tree[2].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[2].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[2].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[3].La", ambL);
	pSpotlightProgram->SetUniform("tree[3].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[3].Ls", specL);
	pSpotlightProgram->SetUniform("tree[3].position", viewMatrix*glm::vec4(9, 10.5f, 28, 1));
	pSpotlightProgram->SetUniform("tree[3].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[3].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[3].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[4].La", ambL);
	pSpotlightProgram->SetUniform("tree[4].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[4].Ls", specL);
	pSpotlightProgram->SetUniform("tree[4].position", viewMatrix*glm::vec4(-9, 10.5f, 35, 1));
	pSpotlightProgram->SetUniform("tree[4].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[4].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[4].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[5].La", ambL);
	pSpotlightProgram->SetUniform("tree[5].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[5].Ls", specL);
	pSpotlightProgram->SetUniform("tree[5].position", viewMatrix*glm::vec4(9, 10.5f, 42, 1));
	pSpotlightProgram->SetUniform("tree[5].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[5].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[5].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[6].La", ambL);
	pSpotlightProgram->SetUniform("tree[6].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[6].Ls", specL);
	pSpotlightProgram->SetUniform("tree[6].position", viewMatrix*glm::vec4(-9, 10.5f, 49, 1));
	pSpotlightProgram->SetUniform("tree[6].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[6].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[6].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[7].La", ambL);
	pSpotlightProgram->SetUniform("tree[7].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[7].Ls", specL);
	pSpotlightProgram->SetUniform("tree[7].position", viewMatrix*glm::vec4(9, 10.5f, 56, 1));
	pSpotlightProgram->SetUniform("tree[7].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[7].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[7].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[8].La", ambL);
	pSpotlightProgram->SetUniform("tree[8].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[8].Ls", specL);
	pSpotlightProgram->SetUniform("tree[8].position", viewMatrix*glm::vec4(-9, 10.5f, 63, 1));
	pSpotlightProgram->SetUniform("tree[8].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[8].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[8].cutoff", cutoff);

	pSpotlightProgram->SetUniform("tree[9].La", ambL);
	pSpotlightProgram->SetUniform("tree[9].Ld", diffL);
	pSpotlightProgram->SetUniform("tree[9].Ls", specL);
	pSpotlightProgram->SetUniform("tree[9].position", viewMatrix*glm::vec4(9, 10.5f, 70, 1));
	pSpotlightProgram->SetUniform("tree[9].direction", glm::normalize(viewNormalMatrix*glm::normalize(norm)));
	pSpotlightProgram->SetUniform("tree[9].exponent", exponent);
	pSpotlightProgram->SetUniform("tree[9].cutoff", cutoff);

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pSpotlightProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = m_pCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pSpotlightProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	//Render the tetrahedron
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, -100.0f, 0.0f));
	modelViewMatrixStack.Scale(2.0f);
	pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pTetrahedron->Render();
	modelViewMatrixStack.Pop();


	//Render the cubes at each cube position specified in the initialise function.
	for (unsigned int i = 0; i < cubePos.size(); i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(cubePos[i]);
		modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), (float)(29 * i));
		pSpotlightProgram->SetUniform("sampler0", 0);
		pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCube->Render();
		modelViewMatrixStack.Pop();

	}

	//Render the grass meshes
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(30.0, 0.1, 30.0));
	modelViewMatrixStack.Scale(1.0, 0.5, 1.0);
	modelViewMatrixStack.Scale(7.0f);
	pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pGrassMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-30.0, 1.0, 30.0));
	modelViewMatrixStack.Scale(-1.0, 1.0, 1.0);
	modelViewMatrixStack.Scale(7.0f);
	pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pGrassMesh->Render();
	modelViewMatrixStack.Pop();

	//Render the statue
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, -20.0f));
	modelViewMatrixStack.Scale(20.0f);
	pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pStatue->Render();
	modelViewMatrixStack.Pop();

	//render the player mesh
	modelViewMatrixStack.Push(); {
		modelViewMatrixStack.Translate(playerPos);
		modelViewMatrixStack.Scale(0.1f);
		modelViewMatrixStack *= playerOrientation;
		pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlayerMesh->Render();
	}
	modelViewMatrixStack.Pop();


	//pSpotlightProgram->SetUniform("light2.La", glm::vec3())
	for (unsigned int i = 1; i < 11; i++)
	{
		float x = (float)pow(-1, i)*10.0f;
		float z = i*7.0f;
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(x, 0.0f, z));
		modelViewMatrixStack.Scale(2.0f, 1.5f, 2.0f);
		pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTree->Render();
		modelViewMatrixStack.Pop();
	}

	//render the catmullrom path
	modelViewMatrixStack.Push();
	pSpotlightProgram->SetUniform("light3.position", viewMatrix*lightPosition3);
	pSpotlightProgram->SetUniform("light4.position", viewMatrix*lightPosition3);
	pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	if (!lightMode) {
		pSpotlightProgram->SetUniform("material1.Ma", glm::vec3(1.0f));
		pSpotlightProgram->SetUniform("material1.Md", glm::vec3(1.0f));
		pSpotlightProgram->SetUniform("material1.Ms", glm::vec3(1.0f));
	}
	pSpotlightProgram->SetUniform("material1.shininess", 50.0f);
	m_pCatmullRom->RenderCentreline();
	m_pCatmullRom->RenderOffsetCurves();
	m_pCatmullRom->RenderWallCurves();
	m_pCatmullRom->RenderTrack();
	m_pCatmullRom->RenderWalls();
	modelViewMatrixStack.Pop();

	CShaderProgram *pStarProgram = (*m_pShaderPrograms)[2];
	pStarProgram->SetUniform("sampler0", 0);
	pStarProgram->UseProgram();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pStarProgram->SetUniform("material1.Ma", glm::vec3(1.0f, 0.5f, 0.0f));
	pStarProgram->SetUniform("material1.Md", glm::vec3(1.0f, 0.5f, 0.0f));
	pStarProgram->SetUniform("material1.Ms", glm::vec3(1.0f, 1.0f, 1.0f));
	pStarProgram->SetUniform("material1.shininess", 50.0f);
	pStarProgram->SetUniform("light1.La", glm::vec3(1.0f, 0.15f, 0.15f));
	pStarProgram->SetUniform("light1.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pStarProgram->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pStarProgram->SetUniform("light1.position", viewMatrix*lightPosition1);

	//Render the stars at each star position specified in the initialise function, and on the collected status of each star.
	for (unsigned int i = 0; i < starPos.size(); i++) {
		if (!collected[i]) {
			modelViewMatrixStack.Push(); {
				modelViewMatrixStack.Translate(starPos[i]);
				modelViewMatrixStack.Scale(0.05f);
				pStarProgram->SetUniform("sampler0", 0);
				pStarProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
				pStarProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pStarProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				m_pStar->Render();
			}
			modelViewMatrixStack.Pop();
		}
	}

	pSpotlightProgram->UseProgram();

	//Render the sphere using the inverse view of the skybox.
	glm::mat4 inverseViewMatrix = glm::inverse(m_pCamera->GetViewMatrix());
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 40.0f, 0.0f));
	modelViewMatrixStack.Scale(2.0f);
	pSpotlightProgram->SetUniform("material1.Ma", glm::vec3(0.0f));
	pSpotlightProgram->SetUniform("material1.Md", glm::vec3(0.3f, 0.3f, 0.3f));
	pSpotlightProgram->SetUniform("material1.Ms", glm::vec3(0.3f, 0.3f, 0.3f));
	pSpotlightProgram->SetUniform("material1.shininess",10.0f);
	pSpotlightProgram->SetUniform("matrices.inverseViewMatrix", inverseViewMatrix);
	pSpotlightProgram->SetUniform("reflectOn", true);
	pSpotlightProgram->SetUniform("bUseTexture", false);
	pSpotlightProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSpotlightProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSphere->Render();
	modelViewMatrixStack.Pop();

	pSpotlightProgram->SetUniform("reflectOn", false);


	DisplayFrameRate();

	SwapBuffers(m_gameWindow.Hdc());

}

// Update method runs repeatedly with the Render method
void Game::Update()
{
	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_currentDistance += (float)m_dt*0.01f*m_pCamera->getSpeed();
	//Find p and pNext in the catmullrom path
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 10.0f, pNext);


	m_dt += 0.001f * (float)m_dt;

	glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);



	m_pCamera->Update(m_dt, p, pNext, m_currentDistance);
	m_pCamera->SwitchCameraState(m_dt);



	T = glm::normalize(pNext - p);
	N = glm::normalize(glm::cross(T, y));
	B = glm::normalize(glm::cross(N, T));


	GetPlayerMovement();
	playerPos = p + playerOffset*N + T*3.5f + B*0.3f;

	//Only check and update the player score while they are still in play (i.e. there are still stars left to collect)
	if(collectedCount != starPos.size())
		CheckPlayerScore(T);

	playerOrientation = glm::mat4(glm::mat3(T, B, N));

	lap = m_pCatmullRom->CurrentLap(m_currentDistance);
	m_pCamera->setSpeed((float)lap);
	
	if(!restart)
	m_pAudio->Update(m_pCamera, playerPos, playerSpeed, T, m_dt);


}

//checks for keyboard events to move the player left and right, only when not in free mode. 
//This is so that the player can move the camera using the same keys during free mode.
void Game::GetPlayerMovement() {
	if (m_pCamera->getCameraState() != CameraState::free) {
		if (GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {
			if (playerOffset > -m_pCatmullRom->getHalfPathWidth() + 0.6) {
				playerOffset += (float)(-0.01 * m_dt * playerSpeed);
			}
		}

		if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {
			if (playerOffset < m_pCatmullRom->getHalfPathWidth() - 0.6) {
				playerOffset += (float)(0.01 * m_dt * playerSpeed);
			}
		}
	}
}


void Game::CheckPlayerScore(glm::vec3 T) {

	//check for player->star collision
	for (unsigned int i = 0; i < starPos.size(); i++) {

		float distance = glm::distance(playerPos, starPos[i]);

		//increase score by 1, and mark star as collected
		if (distance <0.5f && !collected[i]) {
			playerScore++;
			collectedCount++;
			collected[i] = true;
			if(!restart)
			m_pAudio->UpdateStar(m_pCamera, starPos[i], T, m_dt);
			if(!mute)
				m_pAudio->PlayStarSound();
		}
	}


	//check for player->box collision 
	for (unsigned int i = 0; i < cubePos.size(); i++) {
		float distance = glm::distance(playerPos, cubePos[i]);

		//decrease score and set collided to true while player is within collision distance
		if ((distance < 0.5f) && (!collided)) {
			collided = !collided;
			playerScore--;
			//shake camera on box impact
			if ((m_pCamera->getCameraState() != CameraState::free)) {
				m_pCamera->cameraShake(m_dt, 1);
			}
			if(!restart)
			m_pAudio->UpdateBox(m_pCamera, cubePos[i], T, m_dt);
			if(!mute)
				m_pAudio->PlayBoxSound();
			collided = !collided;
		}
	}
}

void Game::DisplayFrameRate()
{

	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	//calculate time in seconds and minutes based on elapsed time
	if(collectedCount != starPos.size()){
	seconds += (int)m_elapsedTime / 1000;
	if (seconds > 60) {
		minutes++;
		seconds = 0;
	}
}


	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
	{
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
	}


	//when the framecount is non-zero and the player still has stars to collect..
	//..display the gameplay HUD
	if (HudOn) {
		if ((m_framesPerSecond > 0) && (collectedCount != starPos.size())) {
			// Use the font shader program and render the text
			fontProgram->UseProgram();

			glDisable(GL_DEPTH_TEST);

			fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
			fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
			fontProgram->SetUniform("vColour", fontColour);
			m_pFtFont->Render(width / 2 - 50, height - 30, 30, "Time: %i", minutes);
			m_pFtFont->Render(width / 2 + 50, height - 30, 20, ":%i", seconds);
			m_pFtFont->Render(20, height - 40, 40, "Score: %d", playerScore);
			m_pFtFont->Render(20, height - 60, 20, "FPS: %d", m_framesPerSecond);

			m_pFtFont->Render(width - 150, height - 40, 40, "Lap: %d", lap);
			m_pFtFont->Render(width - 180, height - 60, 20, "Speed Bonus: %.2f", m_pCamera->getSpeed() - 1.5f);

			//m_pFtFont->Render(width - 600, height - 100, 20, "Pos: %f, %f, %f", m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z);

		}
		else if ((m_framesPerSecond > 0) && (collectedCount == starPos.size())) {
			//if the player has collected all stars, display the finished HUD
			fontProgram->UseProgram();
			glDisable(GL_DEPTH_TEST);
			fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
			fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
			fontProgram->SetUniform("vColour", fontColour);
			m_pFtFont->Render(width / 2 - 150, height / 2, 60, "FINISHED!");
			m_pFtFont->Render(width / 2 - 120, height / 2 - 100, 50, "Time: %i", minutes);
			m_pFtFont->Render(width / 2 + 60, height / 2 - 100, 35, ":%i", seconds);
			m_pFtFont->Render(width / 2 - 120, height / 2 - 50, 60, "Score: %d", playerScore);
			m_pFtFont->Render(width / 2 - 140, 100, 30, "Press 'R' to restart");
		}
	}
}




// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	RenderScene();
	m_dt = m_pHighResolutionTimer->Elapsed();

	
}


WPARAM Game::Execute()
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if (!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();


	MSG msg;

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (m_appActive) {
			//If the restart boolean is set to true, reinitialise the level
			if (restart) {
				Reset();
				Initialise(); 
				restart = false;
			}
			GameLoop();
		}
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch (LOWORD(w_param))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			m_appActive = true;
			m_pHighResolutionTimer->Start();
			break;
		case WA_INACTIVE:
			m_appActive = false;
			break;
		}
		break;
	}

	case WM_SIZE:
		RECT dimensions;
		GetClientRect(window, &dimensions);
		m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch (w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;

		case 'L':
			if (lightMode == true) {
				lightMode = false;
			}
			else if (lightMode == false) {
				lightMode = true;
			}
			break;
		case 'R':
			if (collectedCount == starPos.size()) {
				restart = true;
			}
			break;

		case 'M':
			mute = !mute;
			break;

		case 'H': 
			HudOn = !HudOn;
			break;
		}
		
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance()
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance)
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int)
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}