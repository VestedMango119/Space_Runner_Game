#pragma once

#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"
#include "CatmullRom.h"

class CCatmullRom;

enum class CameraState {
	free = 0, 
	path = 2, 
	left = 4, 
	right = 6, 
	top = 8
};

class CCamera {
public:
	CCamera();										// Constructor - sets default values for camera position, viewvector, upvector, and speed
	~CCamera();										// Destructor

	glm::vec3 GetPosition() const;					// Gets the position of the camera centre of projection
	glm::vec3 GetView() const;						// Gets the position of the camera view point
	glm::vec3 GetUpVector() const;					// Gets the camera up vector
	glm::vec3 GetStrafeVector() const;				// Gets the camera strafe vector
	glm::mat4* GetPerspectiveProjectionMatrix();	// Gets the camera perspective projection matrix
	glm::mat4* GetOrthographicProjectionMatrix();	// Gets the camera orthographic projection matrix
	glm::mat4 GetViewMatrix();		 // Gets the camera view matrix - note this is not stored in the class but returned using glm::lookAt() in GetViewMatrix()

	// Set the camera position, viewpoint, and up vector
	void Set(glm::vec3 &position, glm::vec3 &viewpoint, glm::vec3 &upVector);
	
	// Rotate the camera viewpoint -- this effectively rotates the camera
	void RotateViewPoint(float angle, glm::vec3 &viewPoint);

	// Respond to mouse movement to rotate the camera
	void SetViewByMouse();

	void setSpeed(float factor);
	float getSpeed();

	// Respond to keyboard presses on arrow keys to translate the camera
	void TranslateByKeyboard(double dt);

	void SwitchCameraState(double st);

	// Strafe the camera (move it side to side)
	void Strafe(double direction);

	// Advance the camera (move it forward or backward)
	void Advance(double direction);

	void Update(double dt, glm::vec3 p, glm::vec3 pNext, float currentDistance);
	// Update the camera
	void UpdateFree(double dt);

	void UpdatePath(double dt, glm::vec3 p, glm::vec3 pNext, glm::vec3 &up, glm::vec3 &binormal, float currentDistance);

	void UpdateLeft(double dt, glm::vec3 p, glm::vec3 pNext, glm::vec3 &up, glm::vec3 &normal, glm::vec3 &binormal, float currentDistance);
	
	void UpdateRight(double dt, glm::vec3 p, glm::vec3 pNext, glm::vec3 &up, glm::vec3 &normal, glm::vec3 &binormal, float currentDistance);
	
	void UpdateTop(double dt, glm::vec3 p, glm::vec3 pNext, glm::vec3 &binormal, glm::vec3 &tangent, float currentDistance);

	// Set the projection matrices
	void SetPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearClippingPlane, float farClippingPlane);
	void SetOrthographicProjectionMatrix(int width, int height);

	glm::mat3 ComputeNormalMatrix(const glm::mat4 &modelViewMatrix);

	CameraState getCameraState();

	void cameraShake(double dt, int shake_height);

private:
	glm::vec3 m_position;			// The position of the camera's centre of projection
	glm::vec3 m_view;				// The camera's viewpoint (point where the camera is looking)
	glm::vec3 m_upVector;			// The camera's up vector
	glm::vec3 m_strafeVector;		// The camera's strafe vector

	float m_initSpeed;
	float m_speed;					// How fast the camera moves
	float m_freeSpeed;

	glm::mat4 m_perspectiveProjectionMatrix;		// Perspective projection matrix
	glm::mat4 m_orthographicProjectionMatrix;		// Orthographic projection matrix

	CameraState m_cameraState;
	float m_currentDistance;
	glm::vec3 p; 
	glm::vec3 pNext;

};
