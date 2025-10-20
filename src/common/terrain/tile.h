#pragma once

#include <util/primitive_aliases.h>

class Tile {
public:
    static constexpr uint SIZE = 8;

    enum Type : uint8 {
        Air,
        Stone,
    };

    Tile(Type = Air);

    Type type() const;
    explicit operator Type() const;

private:
    Type _type;
};