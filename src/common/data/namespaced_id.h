#pragma once

#include <prelude.h>
#include <alias/utility.h>
#include <alias/compare.h>
#include <fmt/format.h>
#include <glaze/glaze.hpp>

class id {
private: 
public:
    constexpr id() : _namespace(), _name() {}
    constexpr id(const str& nmspace, const str& name) : _namespace(nmspace), _name(name) {}
    constexpr id(const str& id_str) : _namespace(find_namespace_and_name(id_str).first), _name(find_namespace_and_name(id_str).second) {}
    constexpr id(const id& rhs) : id(rhs._namespace, rhs._name) {}

    const str& nmspace() const;
    const str& name() const;

    str to_str() const;
    
    id& operator=(const id& rhs);
    
    constexpr bool operator==(const id& rhs) const { return _namespace == rhs._namespace && _name == rhs._name; }
    constexpr strong_ordering operator<=>(const id& rhs) const {
        auto namespace_ordering = _namespace <=> rhs._namespace;
        if (namespace_ordering == strong_ordering::less || namespace_ordering == strong_ordering::greater) return namespace_ordering;
        return _name <=> rhs._name;
    }
private:
    str _namespace, _name;
    friend class glz::meta<id>;
    static constexpr pair<str, str> find_namespace_and_name(const str& id_str) {
        if (id_str.empty()) return make_pair("", "");
        if (auto divider = id_str.find("::"); divider != str::npos) return make_pair(id_str.substr(0, divider), id_str.substr(divider + 2));
        return make_pair("default", id_str);
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