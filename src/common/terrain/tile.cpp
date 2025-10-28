#include "tile.h"

Tile::Tile(Type type, Shape shape) : _type(type), _shape(shape) {}

Tile::Type Tile::type() const { return _type; }
Tile::operator Type() const { return _type; }

Tile::Shape Tile::shape() const { return _shape; }
void Tile::set_shape(Shape shape) { _shape = shape; }