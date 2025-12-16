#pragma once

#include <data/namespaced_id.h>
#include <data/definitions/item.h>

class AssetManager;

namespace data {

    class ItemHandle {
    public:
        ItemHandle(const id& id, const definition::Item&);
        
        const id& id() const;
    
    private:
        ::id _id;
        opt<str> _texture_path;
        
        friend class ::AssetManager;
    };

}