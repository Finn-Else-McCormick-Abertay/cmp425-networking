#include "tile.h"

Tile::Tile(const data::id& id, Shape shape) : _type_id(id), _shape(shape) {}

const data::id& Tile::type() const { return _type_id; }
Tile::Shape Tile::shape() const { return _shape; }
void Tile::set_shape(Shape shape) { _shape = shape; }

bool Tile::is(const data::id& id) const { return _type_id == id; }

bool Tile::should_connect_to(const Tile& other) { return Tile::should_connect(_type_id, other._type_id); }
bool Tile::should_connect(const data::id& lhs, const data::id& rhs) {
    if (lhs == rhs) return true;
    return false;
}