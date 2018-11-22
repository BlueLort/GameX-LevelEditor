#include "Camera.h"

int Camera::Window_Width = 1280;
int Camera::Window_Height = 720;

// Constructor with vectors
Camera Camera::cameraInstance = Camera();
 Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
	firstTime = true;
}

// Constructor with scalar values

 Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
	firstTime = true;
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix

  glm::mat4 Camera::getViewMatrix()
{
	  return camView; 
}

  glm::mat4 Camera::getProjMatrix()
  {
	  
	  return camProj; 
  }

glm::vec3 Camera::getPosition() { return Position; }

 void Camera::setPosition(glm::vec3 newPos) { Position = newPos; }

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
 void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.

 void Camera::ProcessMouseMovement(float x, float y, float deltaTime, GLboolean constrainPitch)
{

	 if (firstTime)
	 {
		 lastX = x;
		 lastY = y;
		 firstTime = false;
	 }

	float xoffset = (x - lastX) * MouseSensitivity;
	float yoffset = (lastY - y) * MouseSensitivity;

	Yaw   += xoffset * deltaTime;
	Pitch += yoffset * deltaTime;

	lastX = x;
	lastY = y;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis

 void Camera::ProcessMouseScroll(float yoffset)
{
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

 void Camera::reset()
 {
	 firstTime = true;
 }

float Camera::getYaw() { return Yaw; }

void Camera::setYaw(float newYaw) { Yaw = newYaw; }

float Camera::getPitch() { return Pitch; }

void Camera::setPitch(float newPitch) { Pitch = newPitch; }

// Calculates the front vector from the Camera's (updated) Euler Angles

 void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}

 Camera* Camera::getInstance() {
	 return &cameraInstance;
 }

 void Camera::update()
 {
	 camView = glm::lookAt(Position, Position + Front, Up);
	 camProj = glm::perspective(glm::radians(this->Zoom), (float)Window_Width / (float)Window_Height, zNear, zFar);
 }


