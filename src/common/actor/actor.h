#pragma once

#include <prelude.h>
#include <prelude/vec.h>
#include <maths/rect.h>
#include <data/namespaced_id.h>
#include <render/drawable.h>
#include <network/networked.h>

struct ActorTransform {
    fvec2 position{0}, velocity{0}, acceleration{0};
};

class IActor : IDrawable {
public:
    const id& type_id() const;

    const ActorTransform& transform() const; void set_transform(const ActorTransform&);

    const fvec2& position() const;      void set_position(const fvec2&);
    const fvec2& velocity() const;      void set_velocity(const fvec2&);
    const fvec2& acceleration() const;  void set_acceleration(const fvec2&);
    
    const frect2& local_rect() const;   void set_local_rect(const frect2&);
    const frect2& global_rect() const;

    bool grounded() const;              void set_grounded(bool);
protected:
    IActor(const id&); virtual ~IActor();
    
    #ifdef CLIENT
    virtual dyn_arr<draw_layer> draw_layers() const override;
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif

private:
    id _type_id;
    frect2 _local_rect;
    ActorTransform _transform;

    bool _grounded = false;
    // Cache global rect as it will need to be called very often during collision checks
    mutable frect2 _global_rect; mutable bool _global_rect_dirty = true;
};

class INetworkedActor : public IActor, protected INetworked {
protected:
    INetworkedActor(const ::network_id&);
    INetworkedActor(INetworkedActor&&);

    //virtual dyn_arr<LogicalPacket> get_outstanding_messages() override;
    //virtual result<success_t, str> read_message(LogicalPacket&&) override;
};