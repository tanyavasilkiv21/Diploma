#pragma once
#include <SFML/Graphics.hpp>

class SceneInterface {
public:
    virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual ~SceneInterface() = default;
};

