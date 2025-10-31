#include "tile.h"

Tile::Tile(const TileType& type, Shape shape) : _type(&type), _shape(shape) {}

const TileType& Tile::type() const { return *_type; }
Tile::Shape Tile::shape() const { return _shape; }
void Tile::set_shape(Shape shape) { _shape = shape; }

bool Tile::is(const TileType& type) const { return type == *_type; }

bool Tile::should_connect_to(const Tile& other) { return Tile::should_connect(*_type, *other._type); }
bool Tile::should_connect(const TileType& lhs, const TileType& rhs) {
    if (lhs == rhs) return true;
    return false;
}