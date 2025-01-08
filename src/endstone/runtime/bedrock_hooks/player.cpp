// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bedrock/world/actor/player/player.h"

#include <entt/entt.hpp>

#include "bedrock/entity/components/abilities_component.h"
#include "bedrock/entity/components/actor_game_type_component.h"
#include "bedrock/network/packet/available_commands_packet.h"
#include "bedrock/world/actor/actor_flags.h"
#include "bedrock/world/level/level.h"
#include "endstone/core/server.h"
#include "endstone/event/player/player_teleport_event.h"
#include "endstone/runtime/hook.h"

using endstone::core::EndstonePlayer;
using endstone::core::EndstoneServer;

void Player::teleportTo(const Vec3 &pos, bool should_stop_riding, int cause, int entity_type, bool keep_velocity)
{
    Vec3 position = pos;
    const auto &server = entt::locator<EndstoneServer>::value();
    auto &player = getEndstoneActor<EndstonePlayer>();
    const endstone::Location to{&player.getDimension(), pos.x, pos.y, pos.z, getRotation().x, getRotation().y};
    endstone::PlayerTeleportEvent e{player, player.getLocation(), to};
    server.getPluginManager().callEvent(e);

    if (e.isCancelled()) {
        return;
    }
    position = {e.getTo().getX(), e.getTo().getY(), e.getTo().getZ()};
    ENDSTONE_HOOK_CALL_ORIGINAL_NAME(&Player::teleportTo, __FUNCDNAME__, this, position, should_stop_riding, cause,
                                     entity_type, keep_velocity);
}

void Player::setPermissions(CommandPermissionLevel level)
{
    ENDSTONE_HOOK_CALL_ORIGINAL(&Player::setPermissions, this, level);
    auto &player = getEndstoneActor<EndstonePlayer>();
    player.recalculatePermissions();
    player.updateCommands();
}