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

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		  // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
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
