#pragma once
#include <memory>

#define SINGLETON_DECL(name) name() = default; static name& inst()

#define SINGLETON_INST_DEF(name) name& name::inst() { static std::unique_ptr<name> instance(new name); return *instance; }

// These only have the underscores because register is inexplicably a reserved word in C++ despite not doing anything
#define SINGLETON_REGISTRY(type, ...) class Registry { Registry() = delete; public:\
    static void __register(type& __VA_OPT__(,) __VA_ARGS__);\
    static void __unregister(type&);\
}