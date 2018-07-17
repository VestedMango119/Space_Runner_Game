#include "Audio.h"

#pragma comment(lib, "lib/fmod_vc.lib")

CAudio::CAudio()
{}

CAudio::~CAudio()
{}

//filter coefficients
const unsigned arraylength = 51;
double filter1[] = { -0.00105629273977400, - 0.00668663055577820, - 0.0153475833344421, - 0.0150870366179431, - 0.000258350047325549,	0.0105966219597473,	0.000584761428809673, - 0.0106141054411205,	0.000805016668660919,	0.0123155686803500, - 0.00314058215662753, - 0.0147991856276618,	0.00672733797272538,	0.0176767809627379, - 0.0121326129216169, - 0.0206982772449007,	0.0202345005567439,	0.0236287680064174, - 0.0327333671060792, - 0.0262117023875876,	0.0540002142734338,	0.0282411586746969, - 0.100065523991518, - 0.0295311308762527,	0.316265357787943,	0.529991781472912,	0.316265357787943, - 0.0295311308762527, - 0.100065523991518,	0.0282411586746969,	0.0540002142734338, - 0.0262117023875876, - 0.0327333671060792,	0.0236287680064174,	0.0202345005567439, - 0.0206982772449007, - 0.0121326129216169,	0.0176767809627379,	0.00672733797272538, - 0.0147991856276618, - 0.00314058215662753,	0.0123155686803500,	0.000805016668660919, - 0.0106141054411205,	0.000584761428809673,	0.0105966219597473 ,- 0.000258350047325549, - 0.0150870366179431, - 0.0153475833344421, - 0.00668663055577820, - 0.00105629273977400 };

static int sample_count = 0;
static float *buffer = NULL;


//DSP callback function for FIR filter using circular buffer
FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float * outbuffer, unsigned int length, int inchannels, int *outchannels) {
	
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;

	const int buffer_size = 1024;

	if (buffer == NULL)
		buffer = (float*)malloc(buffer_size * sizeof(float) * inchannels);

	for (unsigned int samp = 0; samp < length; samp++) {
		
		for (int chan = 0; chan < inchannels; chan++)
		{
			buffer[(sample_count * inchannels) % buffer_size + chan] = inbuffer[(samp * inchannels) + chan];

			if (sample_count < arraylength)
				outbuffer[(samp * inchannels) + chan] = 0;
			else {
				
				for (int j = 0; j < arraylength; j++) {
					outbuffer[(samp * inchannels) + chan] += filter1[j] * buffer[((sample_count-j) * inchannels) % buffer_size + chan];
				}
			}
		}
		sample_count++;

	}
	return FMOD_OK;
}


//initialise audio channel
bool CAudio::Initialise()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// Initialise the system
	result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	result = m_FmodSystem->set3DSettings(50.0f, 10.0f, 10.0f);

	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	//Create the DSP effect
	//based on Sound Processing lecture from City University of London
	{
		FMOD_DSP_DESCRIPTION dspdesc;
		memset(&dspdesc, 0, sizeof(dspdesc));

		strncpy_s(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
		dspdesc.numinputbuffers = 1;
		dspdesc.numoutputbuffers = 1;
		dspdesc.read = DSPCallback;

		result = m_FmodSystem->createDSP(&dspdesc, &m_dsp);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	return true;
}



// Load an event sound
bool CAudio::LoadBoxEventSound(char *filename)
{
	result = m_FmodSystem->createSound(filename, NULL, 0, &m_BoxEventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;


	return true;
}

//Load star event sound
bool CAudio::LoadStarEventSound(char *filename)
{
	result = m_FmodSystem->createSound(filename, NULL, 0, &m_StarEventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;


	return true;
}

// Play an event sound
bool CAudio::PlayStarSound()
{
	result = m_FmodSystem->playSound(m_StarEventSound, NULL, false, &m_StarEventChannel);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	setStarEventVolume(0.6f);

	m_StarEventChannel->setMode(FMOD_3D);
	result = m_StarEventChannel->set3DAttributes(&starPosition,0, 0);
	FmodErrorCheck(result); 
	if (result != FMOD_OK)
		return false;

	return true;
}

// Play an event sound
bool CAudio::PlayBoxSound()
{
	result = m_FmodSystem->playSound(m_BoxEventSound, NULL, false, &m_BoxEventChannel);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	setBoxEventVolume(0.7f);

	m_BoxEventChannel->setMode(FMOD_3D);
	result = m_BoxEventChannel->set3DAttributes(&boxPosition, 0, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	return true;
}


// Load a music stream
bool CAudio::LoadMusicStream(char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);


	if (result != FMOD_OK)
		return false;

	return true;


}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	setVolume(1.0f);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	
	m_musicChannel->setMode(FMOD_3D);
	result = m_musicChannel->set3DAttributes(&enginePos, &forward, 0);
	m_musicChannel->set3DDopplerLevel(5.0);
	

	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	
	
	return true;
}

void CAudio::StopMusicStream() {
	m_musicChannel->stop();
}

// Check for error
void CAudio::FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}



//Update for the engine sound
void CAudio::Update(CCamera * camera, glm::vec3 playerPos,  float playerSpeed, glm::vec3 T, double m_dt) {
	
	engineSpeed = playerSpeed;
	ToFMODVector(camera->GetPosition(), &camPos);
	ToFMODVector(glm::normalize(camera->GetUpVector()), &camUp); 
	ToFMODVector(glm::normalize(-T), &camFwd);  
	ToFMODVector(glm::vec3(-T*camera->getSpeed()/(float)m_dt), &camVel);


	ToFMODVector(playerPos, &enginePos);
	ToFMODVector(glm::vec3((T * engineSpeed)/(float)m_dt), &forward);
	
	
	m_musicChannel->setPitch(camera->getSpeed()/3);
	result = m_musicChannel->set3DAttributes(&enginePos, &forward, 0);
	result = m_FmodSystem->set3DListenerAttributes(0, &camPos, &camVel, &camFwd, &camUp);
	FmodErrorCheck(result);
	m_FmodSystem->update();
}

//update for star sound
void CAudio::UpdateStar(CCamera * camera, glm::vec3 starPos, glm::vec3 T, double m_dt) {
	ToFMODVector(camera->GetPosition(), &camPos);
	ToFMODVector(glm::normalize(camera->GetUpVector()), &camUp);
	ToFMODVector(glm::normalize(-T), &camFwd);
	ToFMODVector(glm::vec3(-T * camera->getSpeed() / (float)m_dt), &camVel);


	ToFMODVector(starPos, &starPosition);

	m_StarEventChannel->set3DAttributes(&starPosition, 0, 0);
	result = m_FmodSystem->set3DListenerAttributes(0, &camPos, &camVel, &camFwd, &camUp);
	FmodErrorCheck(result);
	m_FmodSystem->update();
}

//update for box sound
void CAudio::UpdateBox(CCamera * camera, glm::vec3 boxPos, glm::vec3 T, double m_dt) {
	ToFMODVector(camera->GetPosition(), &camPos);
	ToFMODVector(glm::normalize(camera->GetUpVector()), &camUp);
	ToFMODVector(glm::normalize(-T), &camFwd);
	ToFMODVector(glm::vec3(-T*camera->getSpeed() / (float)m_dt), &camVel);


	ToFMODVector(boxPos, &boxPosition);

	result = m_StarEventChannel->set3DAttributes(&boxPosition, 0, 0);
	result = m_FmodSystem->set3DListenerAttributes(0, &camPos, &camVel, &camFwd, &camUp);
	FmodErrorCheck(result);
	m_FmodSystem->update();
}

//set engine volume
void CAudio::setVolume(float vol) {
	result = m_musicChannel->setVolume(vol);
	FmodErrorCheck(result);
}

//set star volume
void CAudio::setStarEventVolume(float vol) {
	result = m_StarEventChannel->setVolume(vol);
	
	FmodErrorCheck(result);
}

//set box volume
void CAudio::setBoxEventVolume(float vol) {
	result = m_BoxEventChannel->setVolume(vol);

	FmodErrorCheck(result);
}

//convert glm vector to FMOD vector
void CAudio::ToFMODVector(glm::vec3 &glVec3, FMOD_VECTOR *fmodVec) {
	fmodVec->x = glVec3.x;
	fmodVec->y = glVec3.y; 
	fmodVec->z = glVec3.z;
}


