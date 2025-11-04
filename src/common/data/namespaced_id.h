#pragma once

#include <string>
#include <utility>
#include <fmt/format.h>

namespace data {

    class id {
    public:
        id(const std::string& nmspace, const std::string& name);
        id(const std::string& id);

        const std::string& nmspace() const;
        const std::string& name() const;

        std::string as_string() const;
        operator std::string() const;
        
    private:
        std::string _namespace, _name;

        static constexpr std::pair<std::string, std::string> find_namespace_and_name(const std::string& id_str) {
            auto divider = id_str.find("::");
            if (divider == std::string::npos) return std::make_pair("", id_str);
            std::string nmspace = id_str.substr(0, divider);
            std::string name; if (divider + 2 < std::string::npos) name = id_str.substr(divider + 2);
            return std::make_pair(nmspace, name);
        }
    };
}

namespace literal {
    data::id operator ""_id(const char*, size_t);
}

template <> struct fmt::formatter<data::id>: formatter<string_view> {
    inline auto format(const data::id& id, format_context& ctx) const {
        return formatter<std::string>().format(fmt::format("{}::{}", id.nmspace(), id.name()), ctx);
    }
};