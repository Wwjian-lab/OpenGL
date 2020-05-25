#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>

#include <vector>

enum carDirection
{
	CarForward,
	CarBackward,
	CarLeft,
	CarRight
};

class Car
{
public:
	glm::vec3 CarPosition;
	glm::vec3 Front;
	float Yaw;
	float MovementSpeed;
	float TurningSpeed;
	float Accelerate = 2.0f;

	//¥Ê¥¢æ…yaw, µœ÷∆Ø“∆
	queue<float> HistoryYaw;
	queue<glm::vec3> HistoryPosition;
	glm::vec3 LastPostion;
	int DelayFrameNum = 20;
	float lastYaw;

	Car(glm::vec3 position = glm::vec3(0, 0, 0))
		: MovementSpeed(30.0f)
		, TurningSpeed(90.0f)
		, Yaw(0.0f)
		, lastYaw(0.0f)
	{
		CarPosition = position;
		updateFront();
	}

	glm::vec3 getPosition() {
		return CarPosition;
	}

	float getYaw() {
		return Yaw;
	}

	float getLastYaw() {
		return lastYaw;
	}

	float getdeltaYaw() {
		return Yaw - lastYaw;
	}

	float getMidYaw() {
		return (lastYaw + Yaw) / 2;
	}

	glm::vec3 getMidPosition() {
		return (LastPostion + CarPosition) / 2.0f;
	}

	glm::mat4 GetViewMatrix(glm::vec3 cameraPosition) {
		return glm::lookAt(cameraPosition, CarPosition, glm::vec3(0, 1.0f, 1.0f));
	}

	//¥¶¿Ìº¸≈Ã ‰»Î
	void ProcessKeyboardInput(carDirection direction, float deltaTime) {
		if (direction == CarForward)
			CarPosition += Front * MovementSpeed * deltaTime;
		if (direction == CarBackward)
			CarPosition -= Front * MovementSpeed * deltaTime;
		if (direction == CarLeft)
			Yaw += TurningSpeed * deltaTime;
		if (direction == CarRight)
			Yaw -= TurningSpeed * deltaTime;
		updateFront();
	}

	void UpdateLastYaw() {
		HistoryYaw.push(Yaw);
		while (HistoryYaw.size() > DelayFrameNum)
		{
			HistoryYaw.pop();
		}
		lastYaw = HistoryYaw.front();
	}

	void UpdateLastPosition() {
		HistoryPosition.push(CarPosition);
		while (HistoryPosition.size() > DelayFrameNum)
		{
			HistoryPosition.pop();
		}
		LastPostion = HistoryPosition.front();
	}


	~Car() {

	}
	
private:
	void updateFront() {
		glm::vec3 front;
		front.x = -sin(glm::radians(getMidYaw()));
		front.y = 0.0f;
		front.z = -cos(glm::radians(getMidYaw()));
		Front = glm::normalize(front);
	}
};