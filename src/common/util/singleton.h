#pragma once
#include <memory>

#define SINGLETON_DECL(name) name() = default; static name& inst()

#define SINGLETON_INST_DEF(name) name& name::inst() { static std::unique_ptr<name> instance(new name); return *instance; }

#define DECL_REGISTRY_FUNCS(name, type, ...)

#define SINGLETON_REGISTRY(name, type, ...) class Registry { Registry() = delete; public:\
    static void register_##name(type& __VA_OPT__(,) __VA_ARGS__);\
    static void unregister_##name(type&);\
}