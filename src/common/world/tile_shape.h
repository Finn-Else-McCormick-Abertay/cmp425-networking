#pragma once

#include <alias/primitives.h>
#include <alias/bitset.h>

namespace tile_impl {
    inline constexpr uint8 TILESET_WIDTH = 6;
    constexpr uint8 idx(uint8 x, uint8 y) { return y * TILESET_WIDTH + x; }

    enum class TileShape : uint8 {
        ____ = idx(0,0),
        _R__ = idx(1,0), LR__ = idx(2,0), L___ = idx(3,0),
        ___B = idx(0,1), __TB = idx(0,2), __T_ = idx(0,3),

        _R_B = idx(1,1), LR_B = idx(2,1), L__B = idx(3,1),
        _RTB = idx(1,2), LRTB = idx(2,2), L_TB = idx(3,2),
        _RT_ = idx(1,3), LRT_ = idx(2,3), L_T_ = idx(3,3),
        
        _R_Bc______Br = idx(1,6), LR_Bc____BlBr = idx(2,6), L__Bc____Bl__ = idx(3,6),
        _RTBc__Tr__Br = idx(1,7), LRTBcTlTrBlBr = idx(2,7), L_TBcTl__Bl__ = idx(3,7),
        _RT_c__Tr____ = idx(1,8), LRT_cTlTr____ = idx(2,8), L_T_cTl______ = idx(3,8),
        
        LRTBc____BlBr = idx(0,4), LRTBcTlTr____ = idx(0,5),
        LRTBc__Tr__Br = idx(4,0), LRTBcTl__Bl__ = idx(5,0),
        LRTBcTl______ = idx(4,3), LRTBc__Tr____ = idx(5,3),
        LRTBc____Bl__ = idx(4,4), LRTBc______Br = idx(5,4),
        LRTBcTl____Br = idx(4,5), LRTBc__TrBl__ = idx(5,5),
        LRTBc__TrBlBr = idx(4,6), LRTBcTl__BlBr = idx(5,6),
        LRTBcTlTr__Br = idx(4,7), LRTBcTlTrBl__ = idx(5,7),

        LR_Bc______Br = idx(1,4), LR_Bc____Bl__ = idx(2,4),
        LRT_c__Tr____ = idx(1,5), LRT_cTl______ = idx(2,5),

        _RTBc______Br = idx(4,1), L_TBc____Bl__ = idx(5,1),
        _RTBc__Tr____ = idx(4,2), L_TBcTl______ = idx(5,2),
    };
    
    constexpr TileShape from_connections(const bitset<8>& connections) {
        // regex to match TileShape enum names
        //(?:(L)|_)(?:(R)|_)(?:(T)|_)(?:(B)|_)(?:c(?:(Tl)|__)(?:(Tr)|__)(?:(Bl)|__)(?:(Br)|__))?
        
        bitset<4> cardinal = bitset<4>((connections >> 4).to_ulong());
        bitset<4> corner = ~bitset<4>((connections & bitset<8>(0b0000'1111)).to_ulong());

        constexpr auto test = [](const bitset<4>& v, unsigned long bits) constexpr -> bool { return (v & bitset<4>(bits)) == bitset<4>(bits); };

        using enum TileShape;
        if (cardinal == bitset<4>(0b1111)) {
            return LRTB;
            if (test(corner, 0b1111)) return LRTBcTlTrBlBr;
            if (test(corner, 0b1110)) return LRTBcTlTrBl__;
            if (test(corner, 0b1101)) return LRTBcTlTr__Br;
            if (test(corner, 0b1011)) return LRTBcTl__BlBr;
            if (test(corner, 0b0111)) return LRTBc__TrBlBr;
            if (test(corner, 0b1100)) return LRTBcTlTr____;
            if (test(corner, 0b1010)) return LRTBcTl__Bl__;
            if (test(corner, 0b1001)) return LRTBcTl____Br;
            if (test(corner, 0b0110)) return LRTBc__TrBl__;
            if (test(corner, 0b0101)) return LRTBc__Tr__Br;
            if (test(corner, 0b0011)) return LRTBc____BlBr;
            if (test(corner, 0b1000)) return LRTBcTl______;
            if (test(corner, 0b0100)) return LRTBc__Tr____;
            if (test(corner, 0b0010)) return LRTBc____Bl__;
            if (test(corner, 0b0001)) return LRTBc______Br;
            return LRTB;
        }
        if (cardinal == bitset<4>(0b1110)) {
            return LRT_;
            if (test(corner, 0b1100)) return LRT_cTlTr____;
            if (test(corner, 0b1000)) return LRT_cTl______;
            if (test(corner, 0b0100)) return LRT_c__Tr____;
            return LRT_;
        }
        if (cardinal == bitset<4>(0b1101)) {
            return LR_B;
            if (test(corner, 0b0011)) return LR_Bc____BlBr;
            if (test(corner, 0b0010)) return LR_Bc____Bl__;
            if (test(corner, 0b0001)) return LR_Bc______Br;
            return LR_B;
        }
        if (cardinal == bitset<4>(0b1011)) {
            return L_TB;
            if (test(corner, 0b1010)) return L_TBcTl__Bl__;
            if (test(corner, 0b1000)) return L_TBcTl______;
            if (test(corner, 0b0010)) return L_TBc____Bl__;
            return L_TB;
        }
        if (cardinal == bitset<4>(0b0111)) {
            return _RTB;
            if (test(corner, 0b0101)) return _RTBc__Tr__Br;
            if (test(corner, 0b0100)) return _RTBc__Tr____;
            if (test(corner, 0b0001)) return _RTBc______Br;
            return _RTB;
        }
        if (cardinal == bitset<4>(0b1010)) {
            return L_T_;
            if (test(corner, 0b1000)) return L_T_cTl______;
            return L_T_;
        }
        if (cardinal == bitset<4>(0b1001)) {
            return L__B;
            if (test(corner, 0b0010)) return L__Bc____Bl__;
            return L__B;
        }
        if (cardinal == bitset<4>(0b0110)) {
            return _RT_;
            if (test(corner, 0b0100)) return _RT_c__Tr____;
            return _RT_;
        }
        if (cardinal == bitset<4>(0b0101)) {
            return _R_B;
            if (test(corner, 0b0001)) return _R_Bc______Br;
            return _R_B;
        }
        if (cardinal == bitset<4>(0b1100)) return LR__;
        if (cardinal == bitset<4>(0b0011)) return __TB;
        if (cardinal == bitset<4>(0b1000)) return L___;
        if (cardinal == bitset<4>(0b0100)) return _R__;
        if (cardinal == bitset<4>(0b0010)) return __T_;
        if (cardinal == bitset<4>(0b0001)) return ___B;
        return ____;
    }
}

using tile_impl::TileShape;