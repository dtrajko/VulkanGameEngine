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

	class GravityVecFieldApp {

	public:
		static constexpr int WIDTH  = 800;
		static constexpr int HEIGHT = 800;

		GravityVecFieldApp();
		~GravityVecFieldApp();

		GravityVecFieldApp(const GravityVecFieldApp&) = delete;
		GravityVecFieldApp& operator=(const GravityVecFieldApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		LveWindow lveWindow{ WIDTH, HEIGHT, "Vulkan Game Engine - Gravity Vec Field App" };
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow, lveDevice };

		// note: order of declarations matters
		std::unique_ptr<LveDescriptorPool> globalPool{};
		LveGameObject::Map gameObjects;
	};

} // namespace lve
