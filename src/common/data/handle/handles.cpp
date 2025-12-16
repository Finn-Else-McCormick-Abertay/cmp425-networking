#include "tile.h"
#include "item.h"

data::TileHandle::TileHandle(const ::id& id, const definition::Tile& tile_def) : _id(id) {
    _texture_path = tile_def.texture;
    if (holds_alternative<str>(tile_def.model)) { _model_type = ModelType::Custom; _model_path = get<str>(tile_def.model); }
    else {
        switch (get<definition::ModelType>(tile_def.model)) {
            case definition::ModelType::none: _model_type = ModelType::None; break;
            case definition::ModelType::block: _model_type = ModelType::Block; break;
            default: _model_type = ModelType::None; break;
        }
    }
}
const id& data::TileHandle::id() const { return _id; }

data::TileHandle::ModelType data::TileHandle::model_type() const { return _model_type; }



data::ItemHandle::ItemHandle(const ::id& id, const definition::Item& item_def) : _id(id) {
    _texture_path = item_def.texture;
}
const id& data::ItemHandle::id() const { return _id; }
