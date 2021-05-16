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

template<typename T> using Unique = std::unique_ptr<T>;

int main()
{
	Unique<Window> window = std::make_unique<Window>(1440, 810, "Hair Simulation", 4);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glLineWidth(3.f);

	PerspectiveCamera cam;
	cam.setProjectionAspectRatio(1440.f / 810);
	cam.setPosition(glm::vec3(-5.f, 3.f, 5.f));
	cam.setCenter(glm::vec3(0.f));

	// Light source model
	Unique<Sphere> lightSphere = std::make_unique<Sphere>(10, 5, 0.5f);
	lightSphere->scale(glm::vec3(0.1f));
	lightSphere->translate(glm::vec3(1.f, 2.f, 1.f));
	lightSphere->color = glm::vec3(1.f);

	// Sphere(head sim)
	Unique<Sphere> headModel = std::make_unique<Sphere>(50, 30, 0.5f);
	headModel->scale(glm::vec3(2.f));
	headModel->color = glm::vec3(0.85f, 0.48f, 0.2f);

	// Skybox
	Unique<Cube> skybox = std::make_unique<Cube>(glm::vec3(-1.f), glm::vec3(1.f));
	Texture skyboxCubemap("cubemap.jpg", GL_TEXTURE_CUBE_MAP, false);

	// Basic hair
	Unique<Hair> hair = std::make_unique<Hair>(2000, Hair::HairType::Curly);
	hair->color = glm::vec3(0.22f, 0.12f, 0.02f);

	// Shaders setup
	DrawingShader basicShader("BasicVertexShader.glsl", "BasicFragmentShader.glsl");
	DrawingShader lightingShader("LightVertexShader.glsl", "LightFragmentShader.glsl");
	DrawingShader skyboxShader("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");
	DrawingShader hairShader("HairVertexShader.glsl", "HairGeometryShader.glsl", "BasicFragmentShader.glsl");

	// Scene light setup
	lightingShader.use();
	lightingShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
	lightingShader.setVec3("light.color", lightSphere->color);
	lightingShader.setFloat("light.constant", 1.f);
	lightingShader.setFloat("light.linear", 0.024f);
	lightingShader.setFloat("light.quadratic", 0.0021f);

	bool doPhysics = false;
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
		
		lightingShader.use();
		lightingShader.setMat4("projection", cam.getProjection());
		lightingShader.setMat4("view", cam.getView());
		lightingShader.setMat4("model", headModel->getTransformMatrix());
		headModel->updateColorsBasedOnMaterial(lightingShader, Entity::Material::PLASTIC);
		headModel->draw();

		if (hair->curlRadius == 0.f)
		{
			basicShader.use();
			basicShader.setMat4("projection", cam.getProjection());
			basicShader.setMat4("view", cam.getView());
			basicShader.setMat4("model", hair->getTransformMatrix());
			basicShader.setVec3("objectColor", hair->color);
		} 
		else
		{
			hairShader.use();
			hairShader.setMat4("projection", cam.getProjection());
			hairShader.setMat4("view", cam.getView());
			hairShader.setMat4("model", hair->getTransformMatrix());
			hairShader.setFloat("curlRadius", hair->curlRadius);
			hairShader.setVec3("objectColor", hair->color);
		}

		hair->draw();


		if (window->isKeyPressed(GLFW_KEY_W))
			cam.moveCamera(Camera::Directions::FORWARD, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_S))
			cam.moveCamera(Camera::Directions::BACKWARD, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_A))
			cam.moveCamera(Camera::Directions::LEFT, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_D))
			cam.moveCamera(Camera::Directions::RIGHT, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_SPACE))
			cam.moveCamera(Camera::Directions::UP, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
			cam.moveCamera(Camera::Directions::DOWN, window->getTime().deltaTime);

		if (window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
			cam.rotateCamera(window->getCursorOffset());

		if (window->isKeyTapped(GLFW_KEY_I))
			hair->increaseStrandCount();
		else if (window->isKeyTapped(GLFW_KEY_O))
			hair->decreaseStrandCount();

		if (doPhysics && window->isKeyTapped(GLFW_KEY_2))
			hair->setFrictionFactor(hair->getFrictionFactor() + 0.01f);
		else if (doPhysics && window->isKeyTapped(GLFW_KEY_1))
			hair->setFrictionFactor(hair->getFrictionFactor() - 0.01f);
		
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
}
