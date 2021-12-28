#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve {

    class GravityPhysicsSystem
    {
    public:
        GravityPhysicsSystem(float strength) : strengthGravity{ strength } {}

        const float strengthGravity;

        // dt stands for delta time, and specifies the amount of time to advance the simulation
        // substeps is how many intervals to divide the forward time step in. More substeps result in a
        // more stable simulation, but takes longer to compute
        void update(std::vector<LveGameObject>& objs, float dt, unsigned int substeps = 1) {
            const float stepDelta = dt / substeps;
            for (unsigned int i = 0; i < substeps; i++) {
                stepSimulation(objs, stepDelta);
            }
        }

        glm::vec2 computeForce(LveGameObject& fromObj, LveGameObject& toObj) const {
            auto offset = fromObj.transform.translation - toObj.transform.translation;
            float distanceSquared = glm::dot(offset, offset);

            // clown town - just going to return 0 if objects are too close together...
            if (glm::abs(distanceSquared) < 1e-10f) {
                return { .0f, .0f };
            }

            float force =
                strengthGravity * toObj.rigidBody2d.mass * fromObj.rigidBody2d.mass / distanceSquared;
            return force * offset / glm::sqrt(distanceSquared);
        }

    private:
        void stepSimulation(std::vector<LveGameObject>& physicsObjs, float dt) {
            // Loops through all pairs of objects and applies attractive force between them
            for (auto iterA = physicsObjs.begin(); iterA != physicsObjs.end(); ++iterA) {
                auto& objA = *iterA;
                for (auto iterB = iterA; iterB != physicsObjs.end(); ++iterB) {
                    if (iterA == iterB) continue;
                    auto& objB = *iterB;

                    auto force = computeForce(objA, objB);
                    objA.rigidBody2d.velocity += dt * -force / objA.rigidBody2d.mass;
                    objB.rigidBody2d.velocity += dt * force / objB.rigidBody2d.mass;
                }
            }

            // update each objects position based on its final velocity
            for (auto& obj : physicsObjs) {
                obj.transform.translation += dt * glm::vec3(obj.rigidBody2d.velocity, 0.0f);
            }
        }
    };

    std::unique_ptr<LveModel> createSquareModel(LveDevice& device, glm::vec3 offset) {

        LveModel::Builder modelBuilder{};

        modelBuilder.vertices =
        {
            {{-0.5f, -0.5f, 0.0f }},
            {{ 0.5f,  0.5f, 0.0f }},
            {{-0.5f,  0.5f, 0.0f }},
            {{-0.5f, -0.5f, 0.0f }},
            {{ 0.5f, -0.5f, 0.0f }},
            {{ 0.5f,  0.5f, 0.0f }},  //
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }
        return std::make_unique<LveModel>(device, modelBuilder);
    }

    std::unique_ptr<LveModel> createCircleModel(LveDevice& device, unsigned int numSides) {
        std::vector<LveModel::Vertex> uniqueVertices{};
        for (unsigned int i = 0; i < numSides; i++) {
            float angle = i * glm::two_pi<float>() / numSides;
            uniqueVertices.push_back({ {glm::cos(angle), glm::sin(angle), 0.0f } });
        }
        uniqueVertices.push_back({});  // adds center vertex at 0, 0

        LveModel::Builder modelBuilder{};

        for (unsigned int i = 0; i < numSides; i++) {
            modelBuilder.vertices.push_back(uniqueVertices[i]);
            modelBuilder.vertices.push_back(uniqueVertices[(i + 1) % numSides]);
            modelBuilder.vertices.push_back(uniqueVertices[numSides]);
        }
        return std::make_unique<LveModel>(device, modelBuilder);
    }

}  // namespace lve
