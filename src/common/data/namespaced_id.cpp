#include "namespaced_id.h"

id::id() : _namespace(), _name() {}
id::id(const str& nmspace, const str& name) : _namespace(nmspace), _name(name) {}

//id::id(pair<str,str>&& pair) : _namespace(move(pair.first)), _name(move(pair.second)) {}
//id::id(const str& id) : id(find_namespace_and_name(id)) {}
id::id(const str& id) : _namespace(find_namespace_and_name(id).first), _name(find_namespace_and_name(id).second) {}

const str& id::nmspace() const { return _namespace; }
const str& id::name() const { return _name; }

str id::to_str() const { return fmt::format("{}::{}", _namespace, _name); }

strong_ordering id::operator<=>(const id& rhs) const {
    auto namespace_ordering = _namespace <=> rhs._namespace;
    if (namespace_ordering == strong_ordering::less || namespace_ordering == strong_ordering::greater) return namespace_ordering;
    return _name <=> rhs._name;
}

bool id::operator==(const id& rhs) const { return _namespace == rhs._namespace && _name == rhs._name; }

pair<str, str> id::find_namespace_and_name(const str& id_str) {
    if (id_str.empty()) return make_pair("", "");
    if (auto divider = id_str.find("::"); divider != str::npos) return make_pair(id_str.substr(0, divider), id_str.substr(divider + 2));
    return make_pair("default", id_str);
}

id& id::operator=(const id& rhs){
    _namespace = rhs._namespace;
    _name = rhs._name;
    return *this;
}