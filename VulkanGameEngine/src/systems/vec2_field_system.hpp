#pragma once

#include "gravity_physics_system.hpp"
#include "lve_game_object.hpp"

#include <vector>


namespace lve {

    class Vec2FieldSystem {
    public:
        void update(
            const GravityPhysicsSystem& physicsSystem,
            std::vector<LveGameObject>& physicsObjs,
            std::vector<LveGameObject>& vectorField) {
            // For each field line we caluclate the net graviation force for that point in space
            for (auto& vf : vectorField) {
                glm::vec2 direction{};
                for (auto& obj : physicsObjs) {
                    direction += physicsSystem.computeForce(obj, vf);
                }

                // This scales the length of the field line based on the log of the length
                // values were chosen just through trial and error based on what i liked the look
                // of and then the field line is rotated to point in the direction of the field
                vf.transform.scale.x =
                    0.005f + 0.045f * glm::clamp(glm::log(glm::length(direction) + 1) / 3.f, 0.f, 1.f);
                vf.transform.rotation = glm::vec3(0.0f, atan2(direction.y, direction.x), 0.0f);
            }
        }
    };

}
