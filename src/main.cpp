#include "Sphere.h"
#include "Window.h"
#include <memory>
#include "Camera.h"
#include "Texture.h"
#include "Cube.h"
#include "Hair.h"
#include "DrawingShader.h"
#include <glm/gtc/matrix_access.hpp>
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <array>

template<typename T> using Unique = std::unique_ptr<T>;

int main(void)
{
	Unique<Window> window = std::make_unique<Window>(1440, 810, "Hair Simulation", 4);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);

	PerspectiveCamera cam;
	cam.setProjectionAspectRatio(1440.f / 810);
	cam.setPosition(glm::vec3(-5.f, 3.f, 5.f));
	cam.setCenter(glm::vec3(0.f));
	cam.setProjectionViewingAngle(100.f);

	// Light source model
	Unique<Sphere> lightSphere = std::make_unique<Sphere>(10, 5, 0.5f);
	lightSphere->scale(glm::vec3(0.1f));
	lightSphere->translate(glm::vec3(1.f, 2.f, 1.f));
	lightSphere->color = glm::vec3(1.f);

	// Skybox
	Unique<Cube> skybox = std::make_unique<Cube>(glm::vec3(-1.f), glm::vec3(1.f));
	Texture skyboxCubemap("room.png", GL_TEXTURE_CUBE_MAP, false);

	// Basic hair
	Unique<Hair> hair = std::make_unique<Hair>(2000, 4.f, 0.f);
	hair->color = glm::vec3(0.45f, 0.18f, 0.012f);

	// Shaders setup
	DrawingShader basicShader("BasicVertexShader.glsl", "BasicFragmentShader.glsl");
	DrawingShader lightingShader("LightVertexShader.glsl", "LightFragmentShader.glsl");
	DrawingShader skyboxShader("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");
	DrawingShader hairShader("HairVertexShader.glsl", "HairGeometryShader.glsl", "HairFragmentShader.glsl");

	// Scene light setup
	lightingShader.use();
	lightingShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
	lightingShader.setVec3("light.color", lightSphere->color);
	lightingShader.setFloat("light.constant", 1.f);
	lightingShader.setFloat("light.linear", 0.024f);
	lightingShader.setFloat("light.quadratic", 0.0021f);

	hairShader.use();
	hairShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
	hairShader.setVec3("light.color", lightSphere->color);
	hairShader.setFloat("light.constant", 1.f);
	hairShader.setFloat("light.linear", 0.024f);
	hairShader.setFloat("light.quadratic", 0.0021f);

	bool doPhysics = false;

	enum Control {
		LIGHT_MOVEMENT,
		HAIR_MOVEMENT,
		HAIR_ROTATION,
		HAIR_FRICTION,
		HAIR_CURLINESS,
		HAIR_STRAND_COUNT,
		VELOCITY_DAMPING
	};

	/*
	* Controls which action arrows currently correspond to
	*/
	Control control = Control::LIGHT_MOVEMENT;

	float angleX = 0.f;
	float angleY = 0.f;

	glViewport(0, 0, window->getWindowSize().x, window->getWindowSize().y);
	do {
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skyboxShader.use();
		skyboxShader.setMat4("projection", cam.getProjection());
		skyboxShader.setMat4("view", cam.getView());
		skyboxCubemap.activateAndBind(GL_TEXTURE0);
		skybox->draw();

		if (doPhysics && glm::abs(window->getTime().deltaTime - window->getTime().lastDeltaTime) < 0.1f)
			hair->applyPhysics(window->getTime().deltaTime, window->getTime().runningTime);

		glEnable(GL_CULL_FACE);
		basicShader.use();
		basicShader.setMat4("projection", cam.getProjection());
		basicShader.setMat4("view", cam.getView());
		basicShader.setMat4("model", lightSphere->getTransformMatrix());
		basicShader.setVec3("objectColor", lightSphere->color);
		lightSphere->draw();

		basicShader.setVec3("objectColor", glm::vec3(1.f, 0.f, 0.f));
		if (window->isKeyPressed(GLFW_KEY_M)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (const auto& s : hair->getEllipsoids()) {
				basicShader.setMat4("model", hair->getTransformMatrix() * s->getTransformMatrix());
				s->draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		lightingShader.use();
		lightingShader.setMat4("projection", cam.getProjection());
		lightingShader.setMat4("view", cam.getView());
		lightingShader.setVec3("eyePosition", cam.getPosition());
		lightingShader.setMat4("model", hair->getTransformMatrix());
		lightingShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
		glm::vec3 tempColor = hair->color;
		hair->color = glm::vec3(1.f, 0.576f, 0.229f);
		hair->updateColorsBasedOnMaterial(lightingShader, Entity::Material::PLASTIC);
		hair->drawHead();

		hair->color = tempColor;
		hairShader.use();
		hairShader.setMat4("projection", cam.getProjection());
		hairShader.setMat4("view", cam.getView());
		hairShader.setMat4("model", hair->getTransformMatrix());
		hairShader.setFloat("curlRadius", hair->getCurlRadius());
		hairShader.setVec3("eyePosition", cam.getPosition());
		hairShader.setUint("particlesPerStrand", hair->getParticlesPerStrand());
		hairShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
		hair->updateColorsBasedOnMaterial(hairShader, Entity::Material::HAIR);
		hair->draw();

		float deltaTime = window->getTime().deltaTime;
		if (window->isKeyPressed(GLFW_KEY_W))
			cam.moveCamera(Camera::Directions::FORWARD, deltaTime);
		if (window->isKeyPressed(GLFW_KEY_S))
			cam.moveCamera(Camera::Directions::BACKWARD, deltaTime);
		if (window->isKeyPressed(GLFW_KEY_A))
			cam.moveCamera(Camera::Directions::LEFT, deltaTime);
		if (window->isKeyPressed(GLFW_KEY_D))
			cam.moveCamera(Camera::Directions::RIGHT, deltaTime);
		if (window->isKeyPressed(GLFW_KEY_SPACE))
			cam.moveCamera(Camera::Directions::UP, deltaTime);
		if (window->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
			cam.moveCamera(Camera::Directions::DOWN, deltaTime);

		if (window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
			cam.rotateCamera(window->getCursorOffset());

		for (int i = 0; i <= 6; ++i)
		{
			if (window->isKeyTapped(i + GLFW_KEY_0))
			{
				control = (Control)i;
				std::cout << "Currently controlling: ";
				switch (i)
				{
					case 0:
						std::cout << "Light movement" << std::endl;
						break;
					case 1:
						std::cout << "Hair movement" << std::endl;
						break;
					case 2:
						std::cout << "Hair rotation" << std::endl;
						break;
					case 3:
						std::cout << "Hair friction" << std::endl;
						break;
					case 4:
						std::cout << "Hair curliness" << std::endl;
						break;
					case 5:
						std::cout << "Hair strand count" << std::endl;
						break;
					case 6:
						std::cout << "Velocity damping" << std::endl;
						break;
				}
				break;
			}
		}

		switch (control)
		{
			case LIGHT_MOVEMENT:
				if (window->isKeyPressed(GLFW_KEY_UP))
					lightSphere->translate(lightSphere->getTranslation() + glm::cross(glm::vec3(0.f, 1.f, 0.f), cam.getUDirection()) * deltaTime * 10.f);
				if (window->isKeyPressed(GLFW_KEY_DOWN))
					lightSphere->translate(lightSphere->getTranslation() - glm::cross(glm::vec3(0.f, 1.f, 0.f), cam.getUDirection()) * deltaTime * 10.f);
				if (window->isKeyPressed(GLFW_KEY_RIGHT))
					lightSphere->translate(lightSphere->getTranslation() + cam.getUDirection() * deltaTime * 10.f);
				if (window->isKeyPressed(GLFW_KEY_LEFT))
					lightSphere->translate(lightSphere->getTranslation() - cam.getUDirection() * deltaTime * 10.f);
				break;

			case HAIR_MOVEMENT:
				if (doPhysics)
				{
					if (window->isKeyPressed(GLFW_KEY_UP))
						hair->translate(hair->getTranslation() + glm::cross(glm::vec3(0.f, 1.f, 0.f), cam.getUDirection()) * deltaTime * 4.f);
					if (window->isKeyPressed(GLFW_KEY_DOWN))
						hair->translate(hair->getTranslation() - glm::cross(glm::vec3(0.f, 1.f, 0.f), cam.getUDirection()) * deltaTime * 4.f);
					if (window->isKeyPressed(GLFW_KEY_RIGHT))
						hair->translate(hair->getTranslation() + cam.getUDirection() * deltaTime * 4.f);
					if (window->isKeyPressed(GLFW_KEY_LEFT))
						hair->translate(hair->getTranslation() - cam.getUDirection() * deltaTime * 4.f);
				}
				break;

			case HAIR_ROTATION:
				if (doPhysics)
				{
					if (window->isKeyPressed(GLFW_KEY_UP) && angleX + deltaTime * 200.f < 90.f)
					{
						hair->rotate(deltaTime * 200.f, glm::vec3(1.f, 0.f, 0.f));
						angleX += deltaTime * 200.f;
					}

					if (window->isKeyPressed(GLFW_KEY_DOWN) && angleX - deltaTime * 200.f > -90.f)
					{
						hair->rotate(-deltaTime * 200.f, glm::vec3(1.f, 0.f, 0.f));
						angleX -= deltaTime * 200.f;
					}

					if (window->isKeyPressed(GLFW_KEY_RIGHT) && angleY + deltaTime * 200.f < 90.f)
					{
						hair->rotate(deltaTime * 200.f, glm::vec3(0.f, 1.f, 0.f));
						angleY += deltaTime * 200.f;
					}

					if (window->isKeyPressed(GLFW_KEY_LEFT) && angleY - deltaTime * 200.f > -90.f)
					{
						hair->rotate(-deltaTime * 200.f, glm::vec3(0.f, 1.f, 0.f));
						angleY -= deltaTime * 200.f;
					}
				}
				break;

			case HAIR_FRICTION:
				if (doPhysics && window->isKeyTapped(GLFW_KEY_UP))
					hair->setFrictionFactor(hair->getFrictionFactor() + 0.01f);
				else if (doPhysics && window->isKeyTapped(GLFW_KEY_DOWN))
					hair->setFrictionFactor(hair->getFrictionFactor() - 0.01f);
				break;

			case HAIR_CURLINESS:
				if (window->isKeyTapped(GLFW_KEY_UP))
					hair->increaseCurlRadius();
				else if (window->isKeyTapped(GLFW_KEY_DOWN))
					hair->decreaseCurlRadius();
				break;

			case HAIR_STRAND_COUNT:
				if (window->isKeyTapped(GLFW_KEY_UP))
					hair->increaseStrandCount();
				else if (window->isKeyTapped(GLFW_KEY_DOWN))
					hair->decreaseStrandCount();
				break;

			case VELOCITY_DAMPING:
				if (window->isKeyTapped(GLFW_KEY_UP))
					hair->increaseVelocityDamping();
				else if (window->isKeyTapped(GLFW_KEY_DOWN))
					hair->decreaseVelocityDamping();
				break;
		}

		if (window->isKeyTapped(GLFW_KEY_ENTER))
			doPhysics = !doPhysics;

		if (window->isResized())
		{
			glm::ivec2 windowSize = window->getWindowSize();
			cam.setProjectionAspectRatio((float)windowSize.x / windowSize.y);
			glViewport(0, 0, windowSize.x, windowSize.y);
		}

		window->onUpdate();
	} while (!window->shouldClose());

	return 0;
}
