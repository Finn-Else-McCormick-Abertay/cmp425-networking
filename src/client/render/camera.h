#pragma once
#include <SFML/Graphics/View.hpp>

#include <prelude.h>
#include <prelude/vec.h>

class Camera {
public:
    Camera(str identifier, int priority = 0, float size = 300.f);
    virtual ~Camera();

    const fvec2& position() const;
    sf::View as_view() const;

    const str& identifier() const;
    int priority() const; void set_priority(int);

private:
    str _identifier; int _priority;
    fvec2 _position; float _size;
};