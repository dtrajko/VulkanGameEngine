#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "lve_buffer.hpp"
#include "lve_game_object.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <chrono>


const float MAX_FRAME_TIME = 0.33f;

namespace lve {

	FirstApp::FirstApp()
	{
		globalPool = LveDescriptorPool::Builder(lveDevice)
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {

		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT/* | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT*/);
			uboBuffers[i]->map();
		}

		std::unique_ptr<LveDescriptorSetLayout> globalSetLayout =
			LveDescriptorSetLayout::Builder(lveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		PointLightSystem pointLightSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		LveCamera camera{};

        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.5f));

        auto viewerObject = LveGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose())
        {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

			if (auto commandBuffer = lveRenderer.beginFrame())
			{
				int frameIndex = lveRenderer.getFrameIndex();

				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				// update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
 				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<LveModel> lveModel;
		
		lveModel = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
		auto flatVase = LveGameObject::createGameObject();
		flatVase.model = lveModel;
		flatVase.transform.translation = { -0.5f, 0.6f, 0.0f };
		flatVase.transform.scale = glm::vec3(3.0f);
        gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
		auto smoothVase = LveGameObject::createGameObject();
		smoothVase.model = lveModel;
		smoothVase.transform.translation = { 0.5f, 0.6f, 0.0f };
		smoothVase.transform.scale = glm::vec3(3.0f);
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "models/quad.obj");
		auto floor = LveGameObject::createGameObject();
		floor.model = lveModel;
		floor.transform.translation = { 0.0f, 0.7f, 0.0f };
		floor.transform.scale = glm::vec3(4.0f);
		gameObjects.emplace(floor.getId(), std::move(floor));

		std::vector<glm::vec3> lightColors {
			{ 1.0f, 0.1f, 0.1f },
			{ 0.1f, 0.1f, 1.0f },
			{ 0.1f, 1.0f, 0.1f },
			{ 1.0f, 1.0f, 0.1f },
			{ 0.1f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = LveGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.0f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.0f, -1.0f, 0.0f}
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}

		// using pointLight again invalid...
	}

} // namespace lve
