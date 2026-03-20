#pragma once

#include <data/namespaced_id.h>
#include <data/definitions/tile.h>
#include <alias/filesystem.h>

class AssetManager;

namespace data {

    class TileHandle {
    public:
        TileHandle(const id& id, const definition::Tile&);

        enum class ModelType { None, Block, Custom };
        enum class CollisionType { None, Block };
        
        const id& id() const;

        ModelType model_type() const;
        CollisionType collision_type() const;
    
    private:
        ::id _id;
        ModelType _model_type;
        CollisionType _collision_type;
        opt<fs::path> _texture_path, _model_path;

        friend class ::AssetManager;
    };

}