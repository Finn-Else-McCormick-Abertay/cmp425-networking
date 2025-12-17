#pragma once

#include <prelude.h>
#include <prelude/vec.h>
#include <maths/rect.h>
#include <data/namespaced_id.h>
#include <render/drawable.h>
#include <network/networked.h>

class IActor : IDrawable {
public:
    const id& type_id() const;

    const fvec2& pos() const;
    void set_pos(const fvec2&);

    const fvec2& velocity() const;
    void set_velocity(const fvec2&);
    
    const fvec2& acceleration() const;
    void set_acceleration(const fvec2&);
    
    const frect2& local_rect() const;
    const frect2& global_rect() const;
protected:
    IActor(const id&); virtual ~IActor();
    
    void set_local_rect(const frect2&);
    
    #ifdef CLIENT
    virtual dyn_arr<draw_layer> draw_layers() const override;
    virtual void draw(sf::RenderTarget&, draw_layer layer) override;
    #endif

private:
    id _type_id;
    fvec2 _pos{0}, _velocity{0}, _accel{0}; frect2 _local_rect;
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