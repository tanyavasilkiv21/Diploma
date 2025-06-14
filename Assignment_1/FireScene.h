#pragma once
#include "SceneInterface.h"
#include "ParticleSys.h"

class FireScene : public SceneInterface {
    ParticleSys particles;
    sf::Vector2u windowSize;

public:
    FireScene() {
        windowSize = sf::Vector2u(1280 / 2, 720 / 2);
        particles.init(windowSize);
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i clickPosition = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(clickPosition);
            particles.init(sf::Vector2u(worldPos));
        }
    }

    void update(float dt) override {
        particles.update();
    }

    void render(sf::RenderWindow& window) override {
        particles.draw(window);
    }
};
