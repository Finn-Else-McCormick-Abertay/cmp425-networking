#pragma once

#include <prelude.h>
#include <prelude/vec.h>
#include <maths/rect.h>
#include <data/namespaced_id.h>
#include <render/drawable.h>

namespace actor {
    enum class PhysicsMode {
        NONE,
        DYNAMIC,
        STATIC
    };
}

class IActor : IDrawable {
public:
    const id& type_id() const;
    actor::PhysicsMode physics_mode() const; void set_physics_mode(actor::PhysicsMode);

    const fvec2& position() const;      fvec2& position();      void set_position(const fvec2&);
    const fvec2& velocity() const;      fvec2& velocity();      void set_velocity(const fvec2&);
    const fvec2& acceleration() const;  fvec2& acceleration();  void set_acceleration(const fvec2&);
    
    const frect2& rect() const;         frect2& rect();         void set_rect(const frect2&);
    frect2 global_rect() const;

    bool grounded() const;              void set_grounded(bool);
private:
    id _type_id; actor::PhysicsMode _physics_mode;
    frect2 _rect;
    fvec2 _position{0}, _velocity{0}, _acceleration{0};
    bool _grounded = false;

protected:
    IActor(const id&, const frect2& = frect2(), actor::PhysicsMode = actor::PhysicsMode::DYNAMIC);
    IActor(const IActor&); IActor(IActor&&);
    virtual ~IActor();
    
    #ifdef CLIENT
    virtual dyn_arr<draw_layer> draw_layers() const override;
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif
};