#pragma once

#include <utility>
#include <compare>
#include <fmt/format.h>

#include <glaze/glaze.hpp>
#include <prelude.h>
#include <alias/utility.h>

class id {
private:
    constexpr id(pair<str,str>&& pair) : _namespace(move(pair.first)), _name(move(pair.second)) {}
public:
    constexpr id(const str& nmspace, const str& name) : _namespace(nmspace), _name(name) {}
    constexpr id(const str& id) : id(find_namespace_and_name(id)) {}
    constexpr id() : id("", "") {}
    constexpr id(const id&) = default;

    inline const str& nmspace() const { return _namespace; }
    inline const str& name() const { return _name; }

    inline str to_str() const { return fmt::format("{}::{}", _namespace, _name); }
    inline operator str() const { return to_str(); }
    
    inline std::strong_ordering operator<=>(const id& rhs) const { return to_str() <=> rhs.to_str(); }
    inline bool operator==(const id& rhs) const { return _namespace == rhs._namespace && _name == rhs._name; }

    constexpr id& operator=(const id& rhs) {
        _namespace = rhs._namespace;
        _name = rhs._name;
        return *this;
    }
    
private:
    str _namespace, _name;

    friend class glz::meta<id>;
    static constexpr pair<str, str> find_namespace_and_name(const str& id_str) {
        auto divider = id_str.find("::");
        if (divider == str::npos) return make_pair("", id_str);
        str nmspace = id_str.substr(0, divider);
        str name; if (divider + 2 < str::npos) name = id_str.substr(divider + 2);
        return std::make_pair(nmspace, name);
    }
};

constexpr id const operator ""_id(const char* literal, size_t) { return { str(literal) }; }
static const id nullid = ""_id;

inline auto format_as(const id& id) { return id.to_str(); }
template<> struct std::hash<id> {
    size_t operator()(const id& id) const noexcept {
        size_t h1 = std::hash<str>{}(id.nmspace());
        size_t h2 = std::hash<str>{}(id.name());
        return h1 ^ (h2 << 1);
    }
};

template<> struct glz::meta<id> {
    static constexpr auto read_id = [](id& id, str val, glz::context& ctx) {
        auto [nmspace, name] = id::find_namespace_and_name(val);
        if (nmspace.empty() || name.empty()) {
            ctx.error = glz::error_code::constraint_violated;
            if (name.empty()) ctx.custom_error_message = "id has empty name";
            else ctx.custom_error_message = "id has no namespace";
        }
        else id = ::id(nmspace, name);
    };

    static constexpr auto value = glz::custom<read_id, &id::to_str>;
};