#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "lve_buffer.hpp"
#include "simple_render_system.hpp"

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

	struct GlobalUbo {
		glm::mat4 projectionView{1.0f};
		glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
	};

	FirstApp::FirstApp() { loadGameObjects(); }

	FirstApp::~FirstApp() {}

	void FirstApp::run() {

		LveBuffer globalUboBuffer{
			lveDevice,
			sizeof(GlobalUbo),
			LveSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			lveDevice.properties.limits.minUniformBufferOffsetAlignment,
		};
		globalUboBuffer.map();

		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass() };
        LveCamera camera{};

        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.5f));

        auto viewerObject = LveGameObject::createGameObject();
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
					camera
				};

				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				globalUboBuffer.writeToIndex(&ubo, frameIndex);
				globalUboBuffer.flushIndex(frameIndex);

				// render
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
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
		flatVase.transform.translation = { -0.5f, 0.6f, 0.6f };
		flatVase.transform.scale = glm::vec3(3.0f);
        gameObjects.push_back(std::move(flatVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
		auto smoothVase = LveGameObject::createGameObject();
		smoothVase.model = lveModel;
		smoothVase.transform.translation = { 0.5f, 0.6f, 0.6f };
		smoothVase.transform.scale = glm::vec3(3.0f);
		gameObjects.push_back(std::move(smoothVase));
	}

} // namespace lve
