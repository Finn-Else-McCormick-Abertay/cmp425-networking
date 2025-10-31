#pragma once

#include <util/primitive_aliases.h>
#include <util/singleton.h>
#include <string>
#include <set>
#include <map>

#ifdef CLIENT
#include <SFML/Graphics/Texture.hpp>
#endif

class Tile;

class TileType {
public:
    struct Meta {
        bool display = true;
        std::set<std::string> tags = {};
        //std::set<std::string> connects_to = {};
    };

    TileType(std::string name, Meta = {});

    const std::string& name() const;
    uint16 id() const;
    const Meta& meta() const;
    
    #ifdef CLIENT
    const sf::Texture* texture() const;
    #endif
    
    bool has_tag(const std::string&) const;

    bool operator==(const TileType&) const;
    //operator Tile() const;

private:
    friend class TileTypeManager;
    std::string _name; uint16 _id;
    Meta _meta;
    
    #ifdef CLIENT
    sf::Texture _tileset;
    #endif
};

class TileTypeManager { SINGLETON_DECL(TileTypeManager);
public:
    SINGLETON_REGISTRY(TileType);

private:
    std::set<TileType*> _tiles;
    std::map<std::string, std::set<TileType*>> _tags;
};