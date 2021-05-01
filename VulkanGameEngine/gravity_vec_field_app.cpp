#include "gravity_vec_field_app.hpp"

#include "simple_render_system.hpp"
#include "gravity_physics_system.hpp"
#include "vec2_field_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>


namespace lve {

	GravityVecFieldApp::GravityVecFieldApp()
	{
		loadGameObjects();
	}

	GravityVecFieldApp::~GravityVecFieldApp()
	{
	}

	void GravityVecFieldApp::run() {

		// create some models
		std::shared_ptr<LveModel> squareModel = createSquareModel(
			lveDevice,
			{ .5f, .0f });  // offset model by .5 so rotation occurs at edge rather than center of square
		std::shared_ptr<LveModel> circleModel = createCircleModel(lveDevice, 64);

		// create physics objects
		std::vector<LveGameObject> physicsObjects{};
		auto red = LveGameObject::createGameObject();
		red.transform2d.scale = glm::vec2{ .05f };
		red.transform2d.translation = { .5f, .5f };
		red.color = { 1.f, 0.f, 0.f };
		red.rigidBody2d.velocity = { -.5f, .0f };
		red.rigidBody2d.mass = 1.0f;
		red.model = circleModel;
		physicsObjects.push_back(std::move(red));
		auto blue = LveGameObject::createGameObject();
		blue.transform2d.scale = glm::vec2{ .05f };
		blue.transform2d.translation = { -.45f, -.25f };
		blue.color = { 0.f, 0.f, 1.f };
		blue.rigidBody2d.velocity = { .5f, .0f };
		blue.rigidBody2d.mass = 1.0f;
		blue.model = circleModel;
		physicsObjects.push_back(std::move(blue));

		// create vector field
		std::vector<LveGameObject> vectorField{};
		int gridCount = 40;
		for (int i = 0; i < gridCount; i++) {
			for (int j = 0; j < gridCount; j++) {
				auto vf = LveGameObject::createGameObject();
				vf.transform2d.scale = glm::vec2(0.005f);
				vf.transform2d.translation = {
					-1.0f + (i + 0.5f) * 2.0f / gridCount,
					-1.0f + (j + 0.5f) * 2.0f / gridCount };
				vf.color = glm::vec3(0.02f);
				vf.model = squareModel;
				vectorField.push_back(std::move(vf));
			}
		}

		GravityPhysicsSystem gravitySystem{ 0.81f };
		Vec2FieldSystem vecFieldSystem{};

		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass() };

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = lveRenderer.beginFrame()) {
				// update systems
				gravitySystem.update(physicsObjects, 1.f / 60, 5);
				vecFieldSystem.update(gravitySystem, physicsObjects, vectorField);

				// render system
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, physicsObjects);
				simpleRenderSystem.renderGameObjects(commandBuffer, vectorField);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	void GravityVecFieldApp::loadGameObjects()
	{
		std::vector<LveModel::Vertex> vertices{
			{{ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
			{{ 0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
			{{-0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }},
		};

		auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);

		auto triangle = LveGameObject::createGameObject();
		triangle.model = lveModel;
		triangle.color = { 0.1f, 0.8f, 0.1f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = { 2.0f, 0.5f };
		triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}

} // namespace lve
