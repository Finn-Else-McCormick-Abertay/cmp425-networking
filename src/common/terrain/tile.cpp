#include "tile.h"

Tile::Tile(Type type) : _type(type) {}

Tile::Type Tile::type() const { return _type; }

Tile::operator Type() const { return _type; }