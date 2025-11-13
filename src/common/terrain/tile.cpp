#include "tile.h"

#include <data/data_manager.h>
#include <exception>


Tile::Tile(const data::id& id, Shape shape) : _type_id(id), _shape(shape) {}

const data::TileHandle& Tile::type() const {
    auto opt = data::Manager::get_tile(_type_id);
    if (!opt) throw std::exception("Attempted to access type handle for tile with invalid type.");
    return opt.value();
}


Tile::Shape Tile::shape() const { return _shape; }
void Tile::set_shape(Shape shape) { _shape = shape; }

bool Tile::is(const data::id& id) const { return _type_id == id; }

bool Tile::should_connect_to(const Tile& other) { return Tile::should_connect(_type_id, other._type_id); }
bool Tile::should_connect(const data::id& lhs, const data::id& rhs) {
    if (lhs == rhs) return true;
    return false;
}