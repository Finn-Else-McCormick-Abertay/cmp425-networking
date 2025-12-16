#include "drawable.h"

#ifdef CLIENT
#include <render/render_manager.h>

IDrawable::IDrawable() { RenderManager::Registry::__register(*this); }
IDrawable::~IDrawable() { RenderManager::Registry::__unregister(*this); }

IDrawable::IDrawable(const IDrawable&) {
    RenderManager::Registry::__register(*this);
}
IDrawable::IDrawable(IDrawable&& rhs) {
    RenderManager::Registry::__unregister(rhs);
    RenderManager::Registry::__register(*this);
}

dyn_arr<draw_layer> IDrawable::draw_layers() const { return { layers::unknown }; }

#else
IDrawable::IDrawable() { }
IDrawable::~IDrawable() { }

IDrawable::IDrawable(const IDrawable&) {}
IDrawable::IDrawable(IDrawable&&) {}

#endif