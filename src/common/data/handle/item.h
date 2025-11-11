#pragma once

#include <util/std_aliases.h>
#include <data/namespaced_id.h>
#include <data/definitions/item.h>

namespace assets { class Manager; }

namespace data {

    class ItemHandle {
    public:
        ItemHandle(const data::id& id, const definition::Item&);
        
        const data::id& id() const;
    
    private:
        data::id _id;
        str _texture_path;
        
        friend class assets::Manager;
    };

}