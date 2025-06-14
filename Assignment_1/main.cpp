#include <SFML/Graphics.hpp>
#include <memory>
#include "SceneInterface.h"
#include "FireScene.h"
#include "WaterScene.h"
#include "LightScene.h"
#include <imgui-SFML.h>

enum class SceneType { Fire, Water, Light };

int main() {
    
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Combined Simulation");
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    SceneType currentType = SceneType::Fire;
    std::unique_ptr<SceneInterface> currentScene = std::make_unique<FireScene>();

    sf::Clock deltaClock;
    sf::Clock clock;

    while (window.isOpen()) {
        ImGui::SFML::Update(window, deltaClock.restart());
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (ImGui::GetIO().WantCaptureMouse)
                {
                    continue;
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    currentScene = std::make_unique<FireScene>();
                    currentType = SceneType::Fire;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    currentScene = std::make_unique<WaterScene>();
                    currentType = SceneType::Water;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    currentScene = std::make_unique<LightScene>();
                    currentType = SceneType::Light;
                }
            }

            currentScene->handleEvent(event, window);
        }

        float dt = clock.restart().asSeconds();
        currentScene->update(dt);

        if (currentType == SceneType::Light)
        {
            window.clear(sf::Color::White);
        }
        else
        {
            window.clear(sf::Color::Black);
        }
        currentScene->render(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}