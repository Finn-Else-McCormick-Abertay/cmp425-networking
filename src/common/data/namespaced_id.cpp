#include "namespaced_id.h"

data::id::id(const std::string& nmspace, const std::string& name) : _namespace(nmspace), _name(name) {}
data::id::id(const std::string& id) {
    auto pair = find_namespace_and_name(id);
    _namespace = pair.first;
    _name = pair.second;
}

const std::string& data::id::nmspace() const { return _namespace; }
const std::string& data::id::name() const { return _name; }

std::string data::id::as_string() const { return _namespace + "::" + _name; }

data::id::operator std::string() const { return as_string(); }

std::strong_ordering data::id::operator<=>(const data::id& rhs) const {
    return as_string() <=> rhs.as_string();
}

bool data::id::operator==(const data::id& rhs) const {
    return _namespace == rhs._namespace && _name == rhs._name;
}