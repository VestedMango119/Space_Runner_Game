#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include "./include/glm/gtc/type_ptr.hpp"
#include "Camera.h"


class CAudio
{
public:
	CAudio();
	~CAudio();
	bool Initialise();
	bool LoadBoxEventSound(char *filename);
	bool LoadStarEventSound(char *filename);
	bool PlayStarSound();
	bool PlayBoxSound();
	bool LoadMusicStream(char *filename);
	bool PlayMusicStream();
	void Update(CCamera *camera, glm::vec3 playerPos, float playerSpeed, glm::vec3 T, double m_dt);
	void UpdateStar(CCamera * camera, glm::vec3 starPos, glm::vec3 T, double m_dt);
	void UpdateBox(CCamera * camera, glm::vec3 boxPos, glm::vec3 T, double m_dt);
	void setVolume(float vol);
	void setStarEventVolume(float vol);
	void setBoxEventVolume(float vol);
	void StopMusicStream();

private:


	void FmodErrorCheck(FMOD_RESULT result);
	float currentVolume = 0.5;
	float engineSpeed;

	void ToFMODVector(glm::vec3 &glVec3, FMOD_VECTOR *fmodVec);
	bool m_musicFilterActive;

	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD
	FMOD::Sound *m_BoxEventSound;
	FMOD::Sound *m_StarEventSound;
	FMOD::Channel *m_BoxEventChannel;
	FMOD::Channel *m_StarEventChannel;

	FMOD::DSP *m_dsp;
	FMOD::DSP *m_dsp2;
	FMOD::DSP *m_dynamicFilter;
	FMOD::Sound *m_music;
	FMOD::Channel* m_musicChannel;
	FMOD::DSP *m_musicDSPHead;
	FMOD::DSP *m_musicDSPHeadInput;


	FMOD_VECTOR camPos;
	FMOD_VECTOR camUp;
	FMOD_VECTOR camFwd;
	FMOD_VECTOR camVel;

	FMOD_VECTOR enginePos;
	FMOD_VECTOR prevEnginePos;
	FMOD_VECTOR forward;
	FMOD_VECTOR vel;
	//float engineSpeed;
	
	FMOD_VECTOR starPosition; 
	FMOD_VECTOR boxPosition;

	FMOD_DSP_DESCRIPTION * dspdesc;
	
};

