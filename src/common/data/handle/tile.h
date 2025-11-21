#pragma once

#include <data/namespaced_id.h>
#include <data/definitions/tile.h>

namespace assets { class Manager; }

namespace data {

    class TileHandle {
    public:
        TileHandle(const id& id, const definition::Tile&);

        enum class ModelType { None, Block, Custom };
        
        const id& id() const;

        ModelType model_type() const;
    
    private:
        ::id _id;
        ModelType _model_type;
        str _texture_path, _model_path;

        friend class assets::Manager;
    };

}