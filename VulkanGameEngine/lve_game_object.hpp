#pragma once

#include "lve_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>


namespace lve {

	struct TransformComponent {
		glm::vec3 translation{}; // (position offset)
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		// Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
		// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
		glm::mat4 mat4() {
			auto transform = glm::translate(glm::mat4{ 1.0f }, translation);

			transform = glm::rotate(transform, rotation.y, { 0.0f, 1.0f, 0.0f });
			transform = glm::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
			transform = glm::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });

			transform = glm::scale(transform, scale);
			return transform;
		}
	};

	struct RigidBody2dComponent {
		glm::vec2 velocity;
		float mass{1.0f};
	};

	class LveGameObject
	{
	public:
		using id_t = unsigned int;

		static LveGameObject createGameObject() {
			static id_t currentId = 0;
			return LveGameObject{ currentId++ };
		}

		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		id_t getId() const { return id; }

		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};
		RigidBody2dComponent rigidBody2d{};

	private:
		LveGameObject(id_t objId) : id{ objId } {}

		id_t id;


	};

} // namespace lve
