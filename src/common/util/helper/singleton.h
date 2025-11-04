#pragma once
#include <memory>
#include <util/macro/wrap.h>

// -- Singleton --

#define DECL_SINGLETON(name, ...) \
    __VA_OPT__(private:) name() = default; __VA_ARGS__##__VA_OPT__(: )static name& inst()
#define DEFINE_SINGLETON(name) \
    name& name::inst() { static std::unique_ptr<name> instance(new name); return *instance; } static_assert(true, "")

// -- Registry --

// __register and __unregister only have the underscores because register is inexplicably a reserved word in C++ despite not doing anything

#define DECL_REGISTRY(type, ...) \
    class Registry { Registry() = delete; public: \
        __VA_OPT__(protected: )__VA_ARGS__ __VA_OPT__(class type;) \
        static void __register(type&); static void __unregister(type&); \
    }

#define __REG_METHODS(type) static void __register(type&); static void __unregister(type&);
#define DECL_MULTI_REGISTRY(...) \
    class Registry { Registry() = delete; public: \
        WRAP_CALL(__REG_METHODS, __VA_ARGS__) \
    }

#define DECL_REGISTRY_WITH_ARGS(type, ...) \
    class Registry { Registry() = delete; public: \
        static void __register(type& __VA_OPT__(,)__VA_ARGS__); static void __unregister(type&); \
    }