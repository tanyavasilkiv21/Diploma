#pragma once
#include "SceneInterface.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>


class ShapeEntity {
public:
    sf::ConvexShape shape;

    ShapeEntity(const std::vector<sf::Vector2f>& points, sf::Vector2f pos)
    {
        shape.setPointCount(points.size());
        for (size_t i = 0; i < points.size(); ++i)
        {
            shape.setPoint(i, points[i]);
        }
        shape.setPosition(pos);
        shape.setOutlineThickness(1);
        shape.setOutlineColor(sf::Color::Black);
        shape.setFillColor(sf::Color::Transparent);
    }
};
struct Intersect
{
    bool result;
    sf::Vector2f pos;
    float t;
};

float crossProduct(sf::Vector2f a, sf::Vector2f b)
{
    return (a.x * b.y - a.y * b.x);
}

Intersect LineIntersect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d)
{
    auto r = b - a;
    auto s = d - c;
    float rxs = crossProduct(r, s);
    auto cma = c - a;
    float t = crossProduct(cma, s) / rxs;
    float u = crossProduct(cma, r) / rxs;
    if (rxs != 0 && t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        return { true, a + t * r, t };
    }
    return { false, sf::Vector2f(0, 0), 0 };
}

void drawLine(sf::Vector2f p1, sf::Vector2f p2, sf::RenderWindow& window, sf::Color color)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = p1;
    line[0].color = color;
    line[1].position = p2;
    line[1].color = color;
    window.draw(line);
}

sf::Vector2f rotateVector(const sf::Vector2f& vec, float angle)
{
    float cosTheta = std::cos(angle);
    float sinTheta = std::sin(angle);
    return sf::Vector2f(
        vec.x * cosTheta - vec.y * sinTheta,
        vec.x * sinTheta + vec.y * cosTheta
    );
}

Intersect drawRayWithIntersection(sf::CircleShape& player, sf::Vector2f& target, std::vector<ShapeEntity>& shapes, sf::RenderWindow& window,
    float angleOffset = 0, bool isLineDraw = 0)
{
    sf::Vector2f start(player.getPosition().x + player.getRadius(), player.getPosition().y + player.getRadius());
    sf::Vector2f direction = target - start;
    direction = rotateVector(direction, angleOffset);

    sf::Vector2f end = start + 1000.0f * direction;

    Intersect closestIntersect = { false, end, 1.f };

    for (const auto& shape : shapes)
    {
        for (size_t i = 0; i < shape.shape.getPointCount(); ++i)
        {
            sf::Vector2f p1 = shape.shape.getTransform().transformPoint(shape.shape.getPoint(i));
            sf::Vector2f p2 = shape.shape.getTransform().transformPoint(shape.shape.getPoint((i + 1) % shape.shape.getPointCount()));
            Intersect intersect = LineIntersect(start, end, p1, p2);
            if (intersect.result && intersect.t < closestIntersect.t)
            {
                closestIntersect = intersect;
            }
        }
    }
    if (isLineDraw)
    {
        drawLine(start, closestIntersect.pos, window, sf::Color(127, 0, 255));
    }
    return closestIntersect;
}
class LightScene : public SceneInterface {
    std::vector<ShapeEntity> shapes;
    sf::CircleShape player;
    bool isLinesDraw = false;
    bool isPolygonDraw = true;

    static float angleBetween(const sf::Vector2f& p1, const sf::Vector2f& p2) {
        return std::atan2(p2.y - p1.y, p2.x - p1.x);
    }

public:
    LightScene() {
        shapes.emplace_back(std::vector<sf::Vector2f>{{0, 0}, { 50, 0 }, { 50, 50 }, { 0, 50 }}, sf::Vector2f(100, 100));
        shapes.emplace_back(std::vector<sf::Vector2f>{{0, 0}, { 30, 0 }, { 30, 30 }, { 0, 30 }}, sf::Vector2f(200, 200));
        shapes.emplace_back(std::vector<sf::Vector2f>{{0, 0}, { 70, 0 }, { 70, 70 }, { 0, 70 }}, sf::Vector2f(300, 300));
        shapes.emplace_back(std::vector<sf::Vector2f>{{0, 0}, { 60, 0 }, { 30, 50 }}, sf::Vector2f(400, 100));
        shapes.emplace_back(std::vector<sf::Vector2f>{{0, 0}, { 80, 0 }, { 40, 60 }}, sf::Vector2f(500, 300));
        shapes.emplace_back(std::vector<sf::Vector2f>{{0, 0}, { 20, 0 }, { 10, 15 }}, sf::Vector2f(250, 400));

        player.setPointCount(20);
        player.setFillColor(sf::Color(127, 0, 255));
        player.setRadius(3);
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::MouseMoved)
        {
            sf::Vector2i position = sf::Mouse::getPosition(window);
            player.setPosition(static_cast<float>(position.x) - player.getRadius(), static_cast<float>(position.y) - player.getRadius());
        }
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::R:
            {
                isLinesDraw = !isLinesDraw;
                break;
            }
            case sf::Keyboard::T:
            {
                isPolygonDraw = !isPolygonDraw;
                break;
            }
            }

        }
    }
    void update(float dt) override {
       
    }

    void render(sf::RenderWindow& window) override {
        sf::Vector2f topLeft(0, 0);
        sf::Vector2f topRight(window.getSize().x, 0);
        sf::Vector2f bottomLeft(0, window.getSize().y);
        sf::Vector2f bottomRight(window.getSize().x, window.getSize().y);

        for (const auto& shape : shapes)
        {
            window.draw(shape.shape);
        }

        std::vector<sf::Vector2f> intersectPos;

        std::vector<sf::Vector2f> corners = { topLeft, topRight, bottomLeft, bottomRight };

        for (auto& corner : corners)
        {
            intersectPos.push_back(drawRayWithIntersection(player, corner, shapes, window).pos);
        }

        for (const auto& shape : shapes)
        {
            for (size_t i = 0; i < shape.shape.getPointCount(); ++i)
            {
                if (isLinesDraw)
                {
                    sf::Vector2f vertex = shape.shape.getTransform().transformPoint(shape.shape.getPoint(i));
                    intersectPos.push_back(drawRayWithIntersection(player, vertex, shapes, window, 0.00001f, true).pos);
                    intersectPos.push_back(drawRayWithIntersection(player, vertex, shapes, window, 0, true).pos);
                    intersectPos.push_back(drawRayWithIntersection(player, vertex, shapes, window, -0.00001f, true).pos);
                }
                else
                {
                    sf::Vector2f vertex = shape.shape.getTransform().transformPoint(shape.shape.getPoint(i));
                    intersectPos.push_back(drawRayWithIntersection(player, vertex, shapes, window, 0.00001f).pos);
                    intersectPos.push_back(drawRayWithIntersection(player, vertex, shapes, window).pos);
                    intersectPos.push_back(drawRayWithIntersection(player, vertex, shapes, window, -0.00001f).pos);
                }

            }
        }

        sf::Vector2f playerCenter = player.getPosition() + sf::Vector2f(player.getRadius(), player.getRadius());

        std::sort(intersectPos.begin(), intersectPos.end(), [&playerCenter](const sf::Vector2f& a, const sf::Vector2f& b)
            {
                return angleBetween(playerCenter, a) < angleBetween(playerCenter, b);
            });

        if (isPolygonDraw)
        {
            for (size_t i = 0; i < intersectPos.size(); i++)
            {
                sf::ConvexShape polygon;
                polygon.setPointCount(3);
                if (i == intersectPos.size() - 1)
                {
                    polygon.setPoint(0, intersectPos[0]);
                    polygon.setPoint(1, player.getPosition());
                    polygon.setPoint(2, intersectPos[i]);
                }
                else
                {
                    polygon.setPoint(0, intersectPos[i]);
                    polygon.setPoint(1, player.getPosition());
                    polygon.setPoint(2, intersectPos[i + 1]);
                }
                polygon.setFillColor(sf::Color(127, 0, 255, 40));
                window.draw(polygon);
            }
        }   
        window.draw(player);

    }
};
