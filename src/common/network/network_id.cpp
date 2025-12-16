#include "network_id.h"

network_id::network_id(const id& type_id, const str& inst_id) : _type_id(type_id), _inst_id(inst_id) {}

const id& network_id::type() const { return _type_id; }
const str& network_id::inst() const { return _inst_id; }

str network_id::to_str() const { return fmt::format("{}#{}", _type_id, _inst_id); }
network_id network_id::from_str(const str& val) {
    if (auto divider = val.find('#'); divider != str::npos) return network_id(id(val.substr(0, divider)), val.substr(divider + 1));
    return network_id(id(val), "");
}

strong_ordering network_id::operator<=>(const network_id& rhs) const {
    auto type_ordering = _type_id <=> rhs._type_id;
    if (type_ordering == strong_ordering::less || type_ordering == strong_ordering::greater) return type_ordering;
    return _inst_id <=> rhs._inst_id;
}
bool network_id::operator==(const network_id& rhs) const { return _type_id == rhs._type_id && _inst_id == rhs._inst_id; }

network_id& network_id::operator=(const network_id& rhs) {
    _type_id = rhs._type_id;
    _inst_id = rhs._inst_id;
    return *this;
}