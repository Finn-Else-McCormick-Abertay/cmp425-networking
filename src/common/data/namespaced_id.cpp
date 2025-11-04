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

data::id literal::operator ""_id(const char* literal, size_t size) { return data::id(std::string(literal)); }