#pragma once

#include <alias/str.h>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

// Adapted from https://holyblackcat.github.io/blog/2025/09/28/type-names.html#at-runtime-using-typeidtname
// Replace `a` with `b` in `source`, return the new string.
inline str replace(str_view source, str_view a, str_view b) {
    str ret;
    size_t cur_pos; size_t last_pos = 0;
    while ((cur_pos = source.find(a, last_pos)) != std::string::npos) {
        ret.append(source, last_pos, cur_pos - last_pos);
        ret += b;
        last_pos = cur_pos + a.size();
    }
    ret.append(source, last_pos);
    return ret;
}

inline str demangle_type_name(str_view mangled_type) {
    #ifdef _MSC_VER
    return str(mangled_type);
    #else
    size_t buf_size; int status = -5;
    char* buf = abi::__cxa_demangle(mangled_type.data(), nullptr, &buf_size, &status);
    if (status != 0 || !buf) return str(mangled_type);
    str ret; ret.assign(buf, buf + buf_size);
    return ret;
    #endif
}

inline str adjust_type_name(str_view type) {
    #ifdef _MSC_VER
    str ret = type;
    ret = replace(ret, "class ", "");
    ret = replace(ret, "struct ", "");
    ret = replace(ret, "enum ", "");
    ret = replace(ret, "union ", "");
    return ret;
    #else
    return str(type);
    #endif
}

inline str clean_type_name(str_view mangled_type) { return adjust_type_name(demangle_type_name(mangled_type)); }