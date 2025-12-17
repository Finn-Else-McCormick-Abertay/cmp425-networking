#pragma once

#include <prelude.h>
#include <data/namespaced_id.h>
#include <util/helper/enum_serialization.h>

DECL_ENUM_FORMATTED(enum class, AssetType, (
    unknown,
    texture,
    texture_tileset,
    texture_item,
    font
));

class asset_id {
public:
    asset_id(const id& = ""_id, AssetType type = AssetType::unknown);
    asset_id(const asset_id&) = default;

    const str& nmspace() const;
    const str& name() const;
    AssetType type() const;

    str to_str() const;
    
    strong_ordering operator<=>(const asset_id& rhs) const;
    bool operator==(const asset_id& rhs) const;

    asset_id& operator=(const asset_id& rhs);

    asset_id with_type(AssetType type) const;
private:
    id _id; AssetType _type;
    friend class std::hash<asset_id>;
};

inline auto format_as(const asset_id& id) { return id.to_str(); }
template<> struct std::hash<asset_id> {
    size_t operator()(const asset_id& asset_id) const noexcept {
        size_t h1 = std::hash<id>{}(asset_id._id);
        size_t h2 = std::to_underlying(asset_id.type());
        return h1 ^ (h2 << 1);
    }
};