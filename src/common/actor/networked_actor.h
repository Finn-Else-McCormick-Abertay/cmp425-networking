#pragma once

#include <actor/actor.h>
#include <network/networked.h>

class INetworkedActor : public IActor, protected INetworked {
public:
    bool is_authority() const;
    void set_authority(bool);

protected:
    bool _authority;

    INetworkedActor(const network_id&);
    INetworkedActor(INetworkedActor&&);

    virtual dyn_arr<LogicalPacket> get_outstanding_messages() override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;
};
