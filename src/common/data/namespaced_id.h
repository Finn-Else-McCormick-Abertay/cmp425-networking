#pragma once

#include <prelude.h>
#include <alias/utility.h>
#include <alias/compare.h>
#include <fmt/format.h>
#include <glaze/glaze.hpp>

class id {
private: 
public:
    id(const str& nmspace, const str& name); id(const str& id); id(); id(const id&) = default;

    const str& nmspace() const;
    const str& name() const;

    str to_str() const;
    
    strong_ordering operator<=>(const id& rhs) const;
    bool operator==(const id& rhs) const;

    id& operator=(const id& rhs);
private:
    str _namespace, _name;
    friend class glz::meta<id>;
    static pair<str, str> find_namespace_and_name(const str& id_str); //id(pair<str,str>&& pair);
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