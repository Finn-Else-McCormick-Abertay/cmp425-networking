#pragma once

#include <utility>
#include <compare>
#include <fmt/format.h>

#include <glaze/glaze.hpp>
#include <prelude.h>

namespace data {

    class id {
    public:
        id(const str& nmspace, const str& name);
        id(const str& id);
        id();
        id(const id&) = default; //id(id&&) = default;

        const str& nmspace() const;
        const str& name() const;

        str as_string() const;
        operator str() const;
        
        std::strong_ordering operator<=>(const data::id& rhs) const;
        bool operator==(const data::id& rhs) const;

        id& operator=(const id&);
        
    private:
        str _namespace, _name;

        friend class glz::meta<data::id>;
        static constexpr std::pair<str, str> find_namespace_and_name(const str& id_str) {
            auto divider = id_str.find("::");
            if (divider == str::npos) return std::make_pair("", id_str);
            str nmspace = id_str.substr(0, divider);
            str name; if (divider + 2 < str::npos) name = id_str.substr(divider + 2);
            return std::make_pair(nmspace, name);
        }
    };
}

template<> struct std::hash<data::id> {
    size_t operator()(const data::id& id) const noexcept {
        size_t h1 = std::hash<str>{}(id.nmspace());
        size_t h2 = std::hash<str>{}(id.name());
        return h1 ^ (h2 << 1);
    }
};

inline data::id operator ""_id(const char* literal, size_t) { return { str(literal) }; }

template <> struct fmt::formatter<data::id>: formatter<string_view> {
    inline auto format(const data::id& id, format_context& ctx) const {
        return formatter<str>().format(fmt::format("{}::{}", id.nmspace(), id.name()), ctx);
    }
};

template<> struct glz::meta<data::id> {
    static constexpr auto read_id = [](data::id& id, str val, glz::context& ctx) {
        auto [nmspace, name] = data::id::find_namespace_and_name(val);
        if (nmspace.empty() || name.empty()) {
            ctx.error = glz::error_code::constraint_violated;
            if (name.empty()) ctx.custom_error_message = "id has empty name";
            else ctx.custom_error_message = "id has no namespace";
        }
        else id = data::id(nmspace, name);
    };

    static constexpr auto value = glz::custom<read_id, &data::id::as_string>;
};