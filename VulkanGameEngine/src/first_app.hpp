#pragma once

#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"
#include "lve_window.hpp"
#include "lve_descriptors.h"

// std
#include <memory>
#include <vector>


namespace lve {

	class FirstApp {

	public:
		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		LveWindow lveWindow{ WIDTH, HEIGHT, "Vulkan Game Engine" };
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow, lveDevice };

		// note: order of declarations matters
		std::unique_ptr<LveDescriptorPool> globalPool{};
		LveGameObject::Map gameObjects;
	};

} // namespace lve
