#include "Camera.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#define SPACE_MOVE (moveLength * deltaTime)
#define FULL_CIRCLE (2 * glm::pi<float>())

Camera::Camera()
{
	recalculateEulerAngles();
}

void Camera::recalculateEulerAngles()
{
	wDirection = glm::normalize(center - position);
	center = position + wDirection;
	uDirection = glm::cross(wDirection, lookUp);
	vDirection = glm::cross(uDirection, wDirection);

	pitch = glm::asin(glm::dot(glm::vec3(0.f, 1.f, 0.f), wDirection));
	yaw = glm::atan(glm::dot(glm::vec3(0.f, 0.f, -1.f), wDirection) / glm::dot(glm::vec3(1.f, 0.f, 0.f), wDirection));

	if (wDirection.z < 0.f) 
	{
		yaw = FULL_CIRCLE - yaw;
	}

	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setPosition(const glm::vec3& position)
{
	this->position = position;
	center = position + wDirection;
	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setCenter(const glm::vec3& center)
{
	this->center = center;
	recalculateEulerAngles();
}

void Camera::setUp(const glm::vec3& up)
{
	lookUp = up;
	recalculateEulerAngles();
}

void Camera::setProjectionNearPlane(float near)
{
	nearPlane = near;
	project();
}

void Camera::setProjectionFarPlane(float far)
{
	farPlane = far;
	project();
}

void Camera::rotateCamera(const glm::vec2& cursorOffset)
{
	float mouseOffsetX = cursorOffset.x;
	float mouseOffsetY = cursorOffset.y;

	yaw += mouseOffsetX * mouseSensitivity;
	if (yaw < 0.f)
		yaw += FULL_CIRCLE;
	else if (yaw > FULL_CIRCLE)
		yaw -= FULL_CIRCLE;

	if (pitch + (mouseOffsetY * mouseSensitivity) < glm::pi<float>() / 2 && pitch + (mouseOffsetY * mouseSensitivity) > -glm::pi<float>() / 2)
		pitch += mouseOffsetY * mouseSensitivity;

	wDirection.x = glm::cos(yaw) * glm::cos(pitch);
	wDirection.y = glm::sin(pitch);
	wDirection.z = glm::sin(yaw) * glm::cos(pitch);
	wDirection = glm::normalize(wDirection);
	center = position + wDirection;
	uDirection = glm::cross(wDirection, lookUp);
	vDirection = glm::cross(uDirection, wDirection);

	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setMouseSensitivity(float sensitivity)
{
	mouseSensitivity = sensitivity;
	recalculateEulerAngles();
}

void Camera::setMoveSensitivity(float sensitivity)
{
	moveLength = sensitivity;
	recalculateEulerAngles();
}

void Camera::moveCamera(Directions direction, float deltaTime)
{
	switch (direction)
	{
		case Directions::FORWARD:
			position += wDirection * SPACE_MOVE;
			center += wDirection * SPACE_MOVE;
			break;

		case Directions::BACKWARD:
			position -= wDirection * SPACE_MOVE;
			center -= wDirection * SPACE_MOVE;
			break;

		case Directions::LEFT:
			position -= uDirection * SPACE_MOVE;
			center -= uDirection * SPACE_MOVE;
			break;

		case Directions::RIGHT:
			position += uDirection * SPACE_MOVE;
			center += uDirection * SPACE_MOVE;
			break;

		case Directions::UP:
			position += lookUp * SPACE_MOVE;
			center += lookUp * SPACE_MOVE;
			break;

		case Directions::DOWN:
			position -= lookUp * SPACE_MOVE;
			center -= lookUp * SPACE_MOVE;
			break;
	}

	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

PerspectiveCamera::PerspectiveCamera()
{
	project();
}

void PerspectiveCamera::setProjectionViewingAngle(float angle)
{
	viewingAngle = angle;
	project();
}

void PerspectiveCamera::setProjectionAspectRatio(float ratio)
{
	aspectRatio = ratio;
	project();
}

void PerspectiveCamera::project()
{
	projection = glm::perspective(glm::radians(viewingAngle), aspectRatio, nearPlane, farPlane);
	projectionChanged = true;
}

OrthograficCamera::OrthograficCamera()
{
	project();
}

void OrthograficCamera::setProjectionLeftPlane(float left)
{
	leftPlane = left;
	project();
}

void OrthograficCamera::setProjectionRightPlane(float right)
{
	rightPlane = right;
	project();
}

void OrthograficCamera::setProjectionTopPlane(float top)
{
	topPlane = top;
	project();
}

void OrthograficCamera::setProjectionBottomPlane(float bottom)
{
	bottomPlane = bottom;
	project();
}

void OrthograficCamera::project()
{
	projection = glm::ortho(leftPlane, rightPlane, nearPlane, farPlane);
	projectionChanged = true;
}