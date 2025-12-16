#include "player_actor.h"

PlayerActor::PlayerActor(const str& ident) : _player_ident(ident), INetworkedActor(::network_id("player"_id, ident)) {
    set_local_rect(DEFAULT_RECT);
}

PlayerActor::PlayerActor(PlayerActor&& rhs) : _player_ident(move(rhs._player_ident)), INetworkedActor(move(rhs)) {
    set_local_rect(DEFAULT_RECT);
}