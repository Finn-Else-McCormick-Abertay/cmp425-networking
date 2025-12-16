#pragma once

#include <data/namespaced_id.h>

class network_id {
public:
    network_id(const id& type_id, const str& inst_id);
    network_id(const network_id&) = default;

    const id& type() const;
    const str& inst() const;

    str to_str() const;
    static network_id from_str(const str&);
    
    strong_ordering operator<=>(const network_id& rhs) const;
    bool operator==(const network_id& rhs) const;

    network_id& operator=(const network_id& rhs);
private:
    id _type_id; str _inst_id;
    friend class glz::meta<network_id>;
};

inline auto format_as(const network_id& id) { return id.to_str(); }
template<> struct std::hash<network_id> {
    size_t operator()(const network_id& network_id) const noexcept {
        size_t h1 = std::hash<id>{}(network_id.type());
        size_t h2 = std::hash<str>{}(network_id.inst());
        return h1 ^ (h2 << 1);
    }
};

template<> struct glz::meta<network_id> {
    static constexpr auto read_id = [](network_id& network_id, const str& val) {
        network_id = network_id::from_str(val);
    };

    static constexpr auto value = glz::custom<read_id, &network_id::to_str>;
};