#include "namespaced_id.h"

data::id::id(const str& nmspace, const str& name) : _namespace(nmspace), _name(name) {}
data::id::id(const str& id) {
    auto pair = find_namespace_and_name(id);
    _namespace = pair.first;
    _name = pair.second;
}
data::id::id() : _namespace(), _name("null") {}

const str& data::id::nmspace() const { return _namespace; }
const str& data::id::name() const { return _name; }

str data::id::as_string() const { return _namespace + "::" + _name; }

data::id::operator str() const { return as_string(); }

std::strong_ordering data::id::operator<=>(const data::id& rhs) const {
    return as_string() <=> rhs.as_string();
}

bool data::id::operator==(const data::id& rhs) const {
    return _namespace == rhs._namespace && _name == rhs._name;
}

data::id& data::id::operator=(const id& rhs) {
    _namespace = rhs._namespace;
    _name = rhs._name;
    return *this;
}