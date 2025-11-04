#include "tile_type.h"
#include <terrain/tile.h>

#include <util/console.h>

TileType::TileType(std::string name, Meta meta) : _name(name), _meta(meta) { TileTypeManager::Registry::__register(*this); }

const std::string& TileType::name() const { return _name; }
uint16 TileType::id() const { return _id; }
const TileType::Meta& TileType::meta() const { return _meta; }

#ifdef CLIENT
const sf::Texture* TileType::texture() const { return &_tileset; }
#endif

bool TileType::has_tag(const std::string& tag) const { return _meta.tags.contains(tag); }

bool TileType::operator==(const TileType& rhs) const { return _id == rhs._id; }

DEFINE_SINGLETON(TileTypeManager);

void TileTypeManager::Registry::__register(TileType& type) {
    type._id = inst()._tiles.size();
    inst()._tiles.insert(&type);
    print<info>("Registered tile '{}' as id {}.", type.name(), type.id());

    for (auto& tag : type.meta().tags) { inst()._tags[tag].insert(&type); }

#ifdef CLIENT
    if (type._tileset.loadFromFile("resources/assets/textures/tileset/" + type.name() + ".png")) { type._tileset.setRepeated(false); }
    else if(type._tileset.loadFromFile("resources/assets/textures/placeholder.png")) { type._tileset.setRepeated(true); }
    type._tileset.setSmooth(false);
#endif
}
// There is no concept of unregistering, as it would mess with the ids
void TileTypeManager::Registry::__unregister(TileType& type) {}