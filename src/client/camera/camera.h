#pragma once
#include <SFML/Graphics/View.hpp>
#include <util/vec.h>
#include <string>

class Camera {
public:
    Camera(std::string identifier, int priority = 0);
    virtual ~Camera();

    const fvec2& position() const;
    sf::View as_view() const;

    const std::string& identifier() const;
    int priority() const; void set_priority(int);

private:
    std::string _identifier; int _priority;
    fvec2 _position; float _size = 300.f;
};