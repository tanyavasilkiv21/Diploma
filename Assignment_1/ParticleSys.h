#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <imgui.h>

class ParticleSys
{
public:
    enum class ParticleShape { Torch, Firework, Fountain, Spiral, Explosion, Rain };

private:
    struct Particle
    {
        sf::Vector2f velocity;
        int lifetime = 0;

        float angle = 0.f;
        float radius = 0.f;
        float angularSpeed = 0.f;
    };

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Vector2u m_position;
    float m_size = 8;
    float m_count = 1000;
    float m_time = 60;
    ParticleShape m_shape = ParticleShape::Torch;
    sf::Color baseColor = sf::Color(255, 100, 30, 255);

    void resetParticles(size_t count = 1000, float size = 8)
    {
        m_particles = std::vector<Particle>(m_count);
        m_vertices = sf::VertexArray(sf::Quads, m_count * 4);
        m_size = size;

        for (size_t p = 0; p < m_particles.size(); p++)
        {
            resetParticle(p, true);
        }
    }

    void resetParticle(std::size_t index, bool first = false)
    {
        sf::Vector2f center;

        float half = m_size / 2.0f;

        switch (m_shape)
        {
        case ParticleShape::Rain:
            center = sf::Vector2f(rand() % m_position.x, 0);
            m_vertices[4 * index + 0].position = center + sf::Vector2f(-half, -half);
            m_vertices[4 * index + 1].position = center + sf::Vector2f(half, -half);
            m_vertices[4 * index + 2].position = center + sf::Vector2f(half, half);
            m_vertices[4 * index + 3].position = center + sf::Vector2f(-half, half);
            break;
        case ParticleShape::Explosion:
        case ParticleShape::Spiral:
        case ParticleShape::Firework:
            m_vertices[4 * index + 0].position = sf::Vector2f(m_position.x, m_position.y);
            m_vertices[4 * index + 1].position = sf::Vector2f(m_position.x + m_size, m_position.y);
            m_vertices[4 * index + 2].position = sf::Vector2f(m_position.x + m_size, m_position.y + m_size);
            m_vertices[4 * index + 3].position = sf::Vector2f(m_position.x, m_position.y + m_size);

            break;
        case ParticleShape::Torch:
        case ParticleShape::Fountain:
        default:
            m_vertices[4 * index + 0].position = sf::Vector2f(m_position.x, m_position.y);
            m_vertices[4 * index + 1].position = sf::Vector2f(m_position.x + m_size, m_position.y);
            m_vertices[4 * index + 2].position = sf::Vector2f(m_position.x + m_size, m_position.y + m_size);
            m_vertices[4 * index + 3].position = sf::Vector2f(m_position.x, m_position.y + m_size);
            break;
        }

        sf::Color c = baseColor;
        if (first)
        {
            c = sf::Color(0, 0, 0, 0);
            m_particles[index].lifetime = 0;
            return;
        }

        for (int i = 0; i < 4; ++i)
            m_vertices[4 * index + i].color = c;

        switch (m_shape)
        {
        case ParticleShape::Torch: {
            float vx = ((float)rand() / RAND_MAX - 0.5f) * 1.0f;
            float vy = -((float)rand() / RAND_MAX) * 2.0f - 1.0f;
            m_particles[index].velocity = sf::Vector2f(vx, vy);
            break;
        }
        case ParticleShape::Firework: {
            float theta = ((float)rand() / RAND_MAX) * 2 * 3.14159f;
            float radius = ((float)rand() / RAND_MAX) * 5;
            float rx = radius * cos(theta);
            float ry = radius * sin(theta);
            m_particles[index].velocity = sf::Vector2f(rx, ry);
            break;
        }
        case ParticleShape::Fountain: {
            float vx = ((float)rand() / RAND_MAX - 0.5f) * 0.5f;
            float vy = -((float)rand() / RAND_MAX) * 3.0f;
            m_particles[index].velocity = sf::Vector2f(vx, vy);
            break;
        }
        case ParticleShape::Spiral: {
            m_particles[index].angle = ((float)rand() / RAND_MAX) * 2 * 3.14159f;
            m_particles[index].radius = 0.0f;
            m_particles[index].angularSpeed = 0.1f + ((float)rand() / RAND_MAX) * 0.2f;
            m_particles[index].velocity = sf::Vector2f(0, 0);
            break;
        }
        case ParticleShape::Explosion: {
            float theta = ((float)rand() / RAND_MAX) * 2 * 3.14159f;
            float power = 2.5f + ((float)rand() / RAND_MAX) * 2.0f;
            float vx = power * cos(theta);
            float vy = power * sin(theta);
            m_particles[index].velocity = sf::Vector2f(vx, vy);
            break;
        }
        case ParticleShape::Rain: {
            float vx = ((float)rand() / RAND_MAX - 0.5f) * 0.2f;
            float vy = 1.0f + ((float)rand() / RAND_MAX) * 1.0f;
            m_particles[index].velocity = sf::Vector2f(vx, vy);
            break;
        }
        }

        m_particles[index].lifetime = 30 + rand() % int(m_time);
    }

public:
    ParticleSys() = default;

    void init(sf::Vector2u windowSize)
    {
        m_position = windowSize;
        resetParticles();
    }

    void update()
    {
        renderGUI();

        for (size_t p = 0; p < m_particles.size(); p++)
        {
            if (m_particles[p].lifetime <= 0)
            {
                resetParticle(p);
            }
            else
            {
                float ratio = static_cast<float>(m_particles[p].lifetime) / 90.0f;

                sf::Color c;
                c.r = baseColor.r;
                c.g = static_cast<sf::Uint8>(baseColor.g * ratio);
                c.b = static_cast<sf::Uint8>(baseColor.b * ratio);
                c.a = static_cast<sf::Uint8>(baseColor.a * ratio);

                if (m_shape == ParticleShape::Spiral) {
                    m_particles[p].angle += m_particles[p].angularSpeed;
                    m_particles[p].radius += 0.5f;

                    float x = m_position.x / 2 + cos(m_particles[p].angle) * m_particles[p].radius;
                    float y = m_position.y / 2 + sin(m_particles[p].angle) * m_particles[p].radius;

                    sf::Vector2f pos(x, y);
                    float half = m_size / 2.0f;

                    m_vertices[4 * p + 0].position = pos + sf::Vector2f(-half, -half);
                    m_vertices[4 * p + 1].position = pos + sf::Vector2f(half, -half);
                    m_vertices[4 * p + 2].position = pos + sf::Vector2f(half, half);
                    m_vertices[4 * p + 3].position = pos + sf::Vector2f(-half, half);
                }
                else {
                    for (int i = 0; i < 4; ++i)
                        m_vertices[4 * p + i].position += m_particles[p].velocity;
                }

                for (int i = 0; i < 4; ++i)
                    m_vertices[4 * p + i].color = c;

                if (m_shape == ParticleShape::Fountain || m_shape == ParticleShape::Rain)
                    m_particles[p].velocity.y += 0.05f;

                m_particles[p].lifetime--;
            }
        }
    }

    void draw(sf::RenderWindow& window) const
    {
        window.draw(m_vertices);
    }

    void renderGUI()
    {
        ImGui::Begin("Particles");

        if (ImGui::SliderFloat("Size", &m_size, 1.0f, 10.0f))
        {
            resetParticles(m_count, m_size);
        }
        if (ImGui::SliderFloat("Quantity", &m_count, 100.0f, 100000.0f))
        {
            resetParticles(m_count, m_size);
        }
        if (ImGui::SliderFloat("Time", &m_time, 30.0f, 150.0f))
        {
        }
        static const char* shapeItems[] = {
            "Torch", "Firework", "Fountain", "Spiral", "Explosion", "Rain"
        };
        static int currentShape = 0;

        if (ImGui::Combo("Shape", &currentShape, shapeItems, IM_ARRAYSIZE(shapeItems)))
        {
            m_shape = static_cast<ParticleShape>(currentShape);
            resetParticles(m_particles.size(), m_size);
        }

        float color[3] = { baseColor.r / 255.0f, baseColor.g / 255.0f, baseColor.b / 255.0f };
        if (ImGui::ColorEdit3("Base Color", color))
        {
            baseColor.r = static_cast<sf::Uint8>(color[0] * 255);
            baseColor.g = static_cast<sf::Uint8>(color[1] * 255);
            baseColor.b = static_cast<sf::Uint8>(color[2] * 255);
            baseColor.a = 255;
            resetParticles(m_particles.size(), m_size);
        }

        ImGui::End();
    }
};
