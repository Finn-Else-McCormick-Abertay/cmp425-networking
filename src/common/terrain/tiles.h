#if !defined(TERRAIN__TILES_H) || defined(__TILE_SYMBOL_DEFINITIONS__)
#define TERRAIN__TILES_H

// All this palaver is to allow this header to be used for both the extern declarations and the actual definition
#define TILE(Name, ...) extern TileType Name
#ifdef __TILE_SYMBOL_DEFINITIONS__
#undef TILE
#define TILE(Name, ...) TileType Name(#Name __VA_OPT__(, TileType::Meta{) __VA_ARGS__ __VA_OPT__(}))
#endif
#undef __TILE_SYMBOL_DEFINITIONS__

#include <terrain/tile.h>

namespace tiles {
    TILE(air,       .display = false);
    TILE(dirt,      .tags = { "dirt" });
    TILE(mud,       .tags = { "dirt" });
    TILE(stone,     .tags = { "stone" });
    TILE(granite,   .tags = { "stone" });
}

#endif