#pragma once

#include <util/macro/token_paste.h>
#include <util/macro/arg_count.h>

#define VAL_IF_EXISTS(val, ...) __VA_OPT__(val)
#define PREFIX_COMMA(...) __VA_OPT__(,)__VA_ARGS__

#define __INTERNAL_VALIF0_0(x,y) x
#define __INTERNAL_VALIF0_1(x,y) y
#define __INTERNAL_VALIF0_2(x,y) y
#define __INTERNAL_VALIF0_3(x,y) y
#define __INTERNAL_VALIF0_4(x,y) y
#define __INTERNAL_VALIF0_5(x,y) y

#define __INTERNAL_VALIF1_0(x,y) y
#define __INTERNAL_VALIF1_1(x,y) x
#define __INTERNAL_VALIF1_2(x,y) y
#define __INTERNAL_VALIF1_3(x,y) y
#define __INTERNAL_VALIF1_4(x,y) y
#define __INTERNAL_VALIF1_5(x,y) y

#define __INTERNAL_VALIF2_0(x,y) y
#define __INTERNAL_VALIF2_1(x,y) y
#define __INTERNAL_VALIF2_2(x,y) x
#define __INTERNAL_VALIF2_3(x,y) y
#define __INTERNAL_VALIF2_4(x,y) y
#define __INTERNAL_VALIF2_5(x,y) y

#define __INTERNAL_VALIF3_0(x,y) y
#define __INTERNAL_VALIF3_1(x,y) y
#define __INTERNAL_VALIF3_2(x,y) y
#define __INTERNAL_VALIF3_3(x,y) x
#define __INTERNAL_VALIF3_4(x,y) y
#define __INTERNAL_VALIF3_5(x,y) y

#define __INTERNAL_VALIF4_0(x,y) y
#define __INTERNAL_VALIF4_1(x,y) y
#define __INTERNAL_VALIF4_2(x,y) y
#define __INTERNAL_VALIF4_3(x,y) y
#define __INTERNAL_VALIF4_4(x,y) x
#define __INTERNAL_VALIF4_5(x,y) y

#define __INTERNAL_VALIF5_0(x,y) y
#define __INTERNAL_VALIF5_1(x,y) y
#define __INTERNAL_VALIF5_2(x,y) y
#define __INTERNAL_VALIF5_3(x,y) y
#define __INTERNAL_VALIF5_4(x,y) y
#define __INTERNAL_VALIF5_5(x,y) x



#define VAL_IF_NUM(num, val, query) __INTERNAL_VALIF##num##_##query(val,)
#define VAL_IF_ARG_COUNT(num, val, ...) TOKEN_PASTE_INDIRECT(__INTERNAL_VALIF##num##_, GET_ARG_COUNT(__VA_ARGS__))(val,)

#define VAL_IF_NUM_ELSE(num, val, elseval, query) __INTERNAL_VALIF##num##_##query(val,elseval)
#define VAL_IF_ARG_COUNT_ELSE(num, val, elseval, ...) TOKEN_PASTE_INDIRECT(__INTERNAL_VALIF##num##_, GET_ARG_COUNT(__VA_ARGS__))(val,elseval)