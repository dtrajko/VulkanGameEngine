#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"


namespace lve {

	class FirstApp {

	public:
		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		void run();

	private:
		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		LvePipeline lvePipeline{ 
			lveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv", LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

	};

} // namespace lve
