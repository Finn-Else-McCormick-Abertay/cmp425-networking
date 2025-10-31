#pragma once

#include <util/primitive_aliases.h>
#include <terrain/tile_type.h>
#include <terrain/tiles.h>

class Tile {
public:
    static constexpr uint SIZE = 8;

    enum class Shape : uint8 {
        SINGLE = 0,
        OPEN_R = 1, OPEN_LR = 2, OPEN_L = 3,
        OPEN_B = 6, OPEN_TB = 12, OPEN_T = 18,

        OPEN_RB = 7, OPEN_LRB = 8, OPEN_LB = 9,
        OPEN_RTB = 13, OPEN_LRTB = 14, OPEN_LTB = 15,
        OPEN_RT = 19, OPEN_LRT = 20, OPEN_LT = 21,
        
        OPEN_RB_CORNER_RB = 37, OPEN_LRB_CORNER_LB_RB = 38, OPEN_LB_CORNER_LB = 39,
        OPEN_RTB_CORNER_RT_RB = 43, OPEN_LRTB_CORNER_LT_RT_LB_RB = 44, OPEN_LTB_CORNER_LT_LB = 45,
        OPEN_RT_CORNER_RT = 49, OPEN_LRT_CORNER_LT_RT = 50, OPEN_LT_CORNER_LT = 51,
        
        OPEN_LRTB_CORNER_LB_RB = 24, OPEN_LRTB_CORNER_LT_RT = 30,

        OPEN_LRTB_CORNER_RT_RB = 4, OPEN_LRTB_CORNER_LT_LB = 5,
        OPEN_LRTB_CORNER_LT = 22, OPEN_LRTB_CORNER_RT = 23,
        OPEN_LRTB_CORNER_LB = 28, OPEN_LRTB_CORNER_RB = 29,
        OPEN_LRTB_CORNER_LT_RB = 34, OPEN_LRTB_CORNER_LB_RT = 35,
        OPEN_LRTB_CORNER_LB_RB_RT = 40, OPEN_LRTB_CORNER_LT_LB_RB = 41,
        OPEN_LRTB_CORNER_LT_RT_RB = 46, OPEN_LRTB_CORNER_LT_LB_RT = 47,

        OPEN_LRB_CORNER_RB = 25, OPEN_LRB_CORNER_LB = 26,
        OPEN_LRT_CORNER_RT = 31, OPEN_LRT_CORNER_LT = 32,

        OPEN_RTB_CORNER_RB = 10, OPEN_LTB_CORNER_LB = 11,
        OPEN_RTB_CORNER_RT = 16, OPEN_LTB_CORNER_LT = 17,
    };

    Tile(const TileType& = tiles::air, Shape = Shape::SINGLE);

    const TileType& type() const; 
    Shape shape() const; void set_shape(Shape);

    bool is(const TileType&) const;

    static bool should_connect(const TileType&, const TileType&);
    bool should_connect_to(const Tile&);

private:
    const TileType* _type;
    Shape _shape;
};