#include "namespaced_id.h"

const str& id::nmspace() const { return _namespace; }
const str& id::name() const { return _name; }

str id::to_str() const { return fmt::format("{}::{}", _namespace, _name); }

id& id::operator=(const id& rhs){
    _namespace = rhs._namespace;
    _name = rhs._name;
    return *this;
}