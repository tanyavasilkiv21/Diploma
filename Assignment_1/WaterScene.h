#pragma once
#include "SceneInterface.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <imgui.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const float g = 9.81f;
const float waveDamping = 0.97f;
const float waveSpeed = 5000.5f;
const float waterDensity = 1000.0f;
const float airDensity = 1.225f;
const float dragCoefficient = 0.47f;
const float SCALE = 100.0f;
const float equilibriumThreshold = 20.0f;

struct Water;

struct Ball {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    float mass;
    float volume;
    bool inWater = false;
    bool atEquilibrium = false;
    bool wasInWater = false;

    Ball(float x, float y, float r, float m)
        : position(x, y), radius(r), mass(m) {
        volume = static_cast<float>((4.0 / 3.0) * M_PI * std::pow(radius, 3));
        velocity = sf::Vector2f(0, 0);
    }

    sf::Vector2f getGravityAndAirResistance(float dt) {
        sf::Vector2f gravityForce(0, mass * g);
        float area = M_PI * std::pow(radius, 2);
        float dragForce = 0.5f * dragCoefficient * airDensity * std::pow(velocity.y, 2) * area;
        sf::Vector2f airResistance(0, (velocity.y > 0 ? -dragForce : dragForce));
        return inWater ? sf::Vector2f(0, gravityForce.y) : (gravityForce + airResistance);
    }

    void applyForce(sf::Vector2f force, float dt) {
        velocity += (force / mass) * dt;
    }

    void update(float dt) {

        position += velocity * dt;
    }
};

struct Water {
    std::vector<float> surfaceHeights;
    std::vector<float> velocities;
    float left, right, top, bottom;
    float dx;
    float poolWidth;
    float initialWaterVolume;

    Water(float l, float t, float width, float height, int resolution = 400)
        : left(l), top(t), right(l + width), bottom(t + height), poolWidth(width) {
        dx = width / static_cast<float>(resolution);
        initialWaterVolume = (width / SCALE) * ((bottom - top) / SCALE);
        for (int i = 0; i <= resolution; i++) {
            surfaceHeights.push_back(top);
            velocities.push_back(0.0f);
        }
    }

    void updateWaterLevel(std::vector<Ball>& balls) {
        static float lastTotalVolume = initialWaterVolume;
        float totalSubmergedVolume = 0.0f;

        for (auto& ball : balls) {
            if (ball.inWater) {
                float h = std::max(0.0f, std::min((ball.position.y + ball.radius) * SCALE - top, 2 * ball.radius * SCALE));
                float submergedVolume = (M_PI * h * h * (3 * ball.radius * SCALE - h)) / (3.0f * SCALE * SCALE * SCALE);
                totalSubmergedVolume += submergedVolume;
            }

            if (ball.position.y + ball.radius >= bottom / SCALE) {
                ball.position.y = bottom / SCALE - ball.radius;
                ball.velocity.y = 0;
                ball.atEquilibrium = true;
            }
        }

        float totalWaterVolume = initialWaterVolume + totalSubmergedVolume;
        float waterRise = ((totalWaterVolume - lastTotalVolume) / (poolWidth / SCALE)) * SCALE;

        if (std::abs(totalWaterVolume - lastTotalVolume) > 0.0001f) {
            top -= waterRise;

            for (auto& height : surfaceHeights) {
                height -= waterRise;
            }

            lastTotalVolume = totalWaterVolume;
        }
    }
    void createWave(Ball& ball, float force) {
        int index = static_cast<int>((ball.position.x * SCALE - left) / dx);
        if (index >= 0 && index < static_cast<int>(surfaceHeights.size())) {
            float spreadFactor = std::max(5.0f, std::min(ball.radius * 10.0f, 50.0f));
            float impactDepth = std::min(force * 0.02f, 5.0f);
            surfaceHeights[index] += impactDepth;
            velocities[index] -= force * 0.2f;

            for (int i = -static_cast<int>(spreadFactor); i <= static_cast<int>(spreadFactor); i++) {
                int waveIndex = index + i;
                if (waveIndex >= 0 && waveIndex < static_cast<int>(surfaceHeights.size())) {
                    float waveFactor = exp(-std::abs(i) / spreadFactor);
                    velocities[waveIndex] -= force * waveFactor;
                }
            }
        }
    }

    void update(float dt, std::vector<Ball>& balls) {
        updateWaterLevel(balls);
        std::vector<float> newHeights = surfaceHeights;
        for (size_t i = 1; i < surfaceHeights.size() - 1; i++) {
            float left = surfaceHeights[i - 1];
            float right = surfaceHeights[i + 1];
            float center = surfaceHeights[i];
            float waveAcceleration = (left + right - 2 * center) * waveSpeed;
            velocities[i] += waveAcceleration * dt;
            velocities[i] *= waveDamping;
            newHeights[i] += velocities[i] * dt;

            if ((velocities[i] > 0 && velocities[i - 1] < 0) || (velocities[i] < 0 && velocities[i + 1] > 0)) {
                float energyLossFactor = 0.2f;
                float opposingWaveStrength = std::min(std::abs(velocities[i - 1]), std::abs(velocities[i + 1]));
                velocities[i] -= opposingWaveStrength * energyLossFactor * (velocities[i] > 0 ? 1 : -1);
            }
        }
        surfaceHeights = newHeights;
    }

    sf::Vector2f calculateWaterForces(Ball& ball, float dt) {
        sf::Vector2f force(0, 0);
        int index = static_cast<int>((ball.position.x * SCALE - left) / dx);

        if (index < 0 || index >= static_cast<int>(surfaceHeights.size()))
            return force;

        bool hittingWater = (surfaceHeights[index] < (ball.position.y + ball.radius) * SCALE) && !ball.inWater;

        if (hittingWater) {
            float impactForce = calculateImpactForce(ball);
            force.y -= impactForce * 0.9f;

            float waveImpulse = 0.1f * impactForce;
            createWave(ball, waveImpulse);

            ball.velocity.y *= 0.4f;
            ball.inWater = true;
            ball.wasInWater = true;

            return force;
        }

        if (surfaceHeights[index] > (ball.position.y + ball.radius) * SCALE) {
            ball.inWater = false;
            ball.atEquilibrium = false;
            ball.wasInWater = false;
            return force;
        }

        ball.wasInWater = true;
        ball.inWater = true;

        float h = std::max(0.0f, std::min((ball.position.y + ball.radius) * SCALE - surfaceHeights[index], 2 * ball.radius * SCALE));
        float submergedVolume = (M_PI * h * h * (3 * ball.radius * SCALE - h)) / (3.0f * SCALE * SCALE * SCALE);

        float buoyancyFactor = 1 - exp(-h / (ball.radius * 2));
        float velocityDamping = std::min(1.0f, 0.8f + 0.2f * exp(-std::abs(ball.velocity.y) / 2.0f));
        float buoyancyForce = waterDensity * g * submergedVolume * buoyancyFactor * velocityDamping;

        const float waterViscosity = 6 * M_PI * 0.001002f;
        float dragForce = waterViscosity * ball.radius * ball.velocity.y;

        float submergedArea = M_PI * std::pow(ball.radius, 2);
        float reynolds = std::max(1.0f, (waterDensity * std::abs(ball.velocity.y) * (2 * ball.radius)) / (0.001002f));
        float Cd = (reynolds < 2000) ? (24.0f / reynolds) : (0.47f + 0.5f / sqrt(reynolds));
        Cd = std::max(0.1f, std::min(Cd, 1.2f));

        float effectiveSubmergedArea = submergedArea * (submergedVolume / ball.volume);
        float turbulentDrag = 0.5f * Cd * waterDensity * effectiveSubmergedArea * ball.velocity.y * std::abs(ball.velocity.y);

        if (reynolds > 10'000) {
            turbulentDrag *= 0.6f;
        }

        float vortexResistance = -0.2f * waterDensity * effectiveSubmergedArea * ball.velocity.y;
        if (std::abs(ball.velocity.y) < 0.2f) {
            vortexResistance *= 0.5f;
        }

        float depthFactor = 1.0f + (h / (2 * ball.radius));
        float viscousDamping = waterDensity * submergedVolume * g / (10.0f * depthFactor);
        float viscousForce = -viscousDamping * ball.velocity.y;

        float surfaceDampingFactor = 2.0f * (1 - exp(-h / ball.radius));
        float surfaceForce = -surfaceDampingFactor * ball.velocity.y;

        float airResistanceFactor = std::max(0.0f, 1.0f - (h / (2 * ball.radius)));
        float airResistance = -airDensity * submergedArea * std::pow(ball.velocity.y, 2) * airResistanceFactor;

        if (ball.velocity.y < -10.0f) {
            ball.velocity.y *= 0.7f;
        }

        buoyancyForce = buoyancyForce + (turbulentDrag * 0.9);
        force.y = -dragForce - buoyancyForce + viscousForce + surfaceForce + airResistance + vortexResistance;

        if (std::abs(force.y) < equilibriumThreshold && std::abs(ball.velocity.y) < 0.001f) {
            ball.velocity.y = 0;
            ball.atEquilibrium = true;
        }
        else {
            ball.atEquilibrium = false;
        }

        return force;
    }

    float calculateImpactForce(Ball& ball) {
        float impactTime = 0.02f;
        float deltaV = std::abs(ball.velocity.y);
        float impulseForce = (ball.mass * deltaV) / impactTime;

        return impulseForce;
    }

    void draw(sf::RenderWindow& window) {
        sf::VertexArray waterShape(sf::TriangleStrip);
        for (size_t i = 0; i < surfaceHeights.size(); i++) {
            float x = left + i * dx;
            waterShape.append(sf::Vertex(sf::Vector2f(x, surfaceHeights[i]), sf::Color(0, 100, 255, 180)));
            waterShape.append(sf::Vertex(sf::Vector2f(x, bottom), sf::Color(0, 100, 255, 180)));
        }
        window.draw(waterShape);
    }
};

class WaterScene : public SceneInterface {
    Water water;
    std::vector<Ball> balls;

public:
    WaterScene() : water(50.0f, 250.0f, 700.0f, 300.0f) {}
    float radius = 0.40f;
    float mass = 1.2f;
    float dt1 = 0.01;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override {
        if (event.type == sf::Event::MouseButtonPressed) {
          
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                float ballX = static_cast<float>(event.mouseButton.x) / SCALE;
                float ballY = static_cast<float>(event.mouseButton.y) / SCALE;
                balls.emplace_back(ballX, ballY, radius, mass);
            }
        }
    }

    void update(float dt) override {
        ImGui::Begin("Water Settings");
        ImGui::SliderFloat("Ball Radius", &radius, 0.1f, 1.0f);
        ImGui::SliderFloat("Ball Mass", &mass, 0.1f, 500.0f);
        ImGui::End();
        for (auto& ball : balls) {
            sf::Vector2f gravityAndAir = ball.getGravityAndAirResistance(dt1);
            sf::Vector2f waterForces = water.calculateWaterForces(ball, dt1);
            sf::Vector2f totalForce = gravityAndAir + waterForces;
            ball.applyForce(totalForce, dt1);
            ball.update(dt1);
        }
        water.update(dt1, balls);
    }

    void render(sf::RenderWindow& window) override {
        water.draw(window);
        for (const auto& ball : balls) {
            sf::CircleShape shape(ball.radius * SCALE);
            shape.setFillColor(sf::Color::Red);
            shape.setOrigin(ball.radius * SCALE, ball.radius * SCALE);
            shape.setPosition(ball.position.x * SCALE, ball.position.y * SCALE);
            window.draw(shape);
        }
    }
};
