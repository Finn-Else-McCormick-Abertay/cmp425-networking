#include "asset_id.h"

asset_id::asset_id(const id& id, AssetType type) : _id(id), _type(type) {}

const str& asset_id::nmspace() const { return _id.nmspace(); }
const str& asset_id::name() const { return _id.name(); }
AssetType asset_id::type() const { return _type; }

str asset_id::to_str() const { return fmt::format("{}#{}", _id, _type); }

strong_ordering asset_id::operator<=>(const asset_id& rhs) const {
    auto type_ordering = _type <=> rhs._type;
    if (type_ordering == strong_ordering::less || type_ordering == strong_ordering::greater) return type_ordering;
    return _id <=> rhs._id;
}
bool asset_id::operator==(const asset_id& rhs) const { return _type == rhs._type && _id == rhs._id; }

asset_id& asset_id::operator=(const asset_id& rhs) {
    _id = rhs._id;
    _type = rhs._type;
    return *this;
}

asset_id asset_id::with_type(AssetType type) const { return asset_id(_id, type); }