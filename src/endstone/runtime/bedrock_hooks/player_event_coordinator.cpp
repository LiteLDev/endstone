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

#include "bedrock/world/events/player_event_coordinator.h"

#include "bedrock/entity/gamerefs_entity/gamerefs_entity.h"
#include "bedrock/gamerefs/weak_ref.h"
#include "bedrock/server/server_instance.h"
#include "bedrock/world/level/level.h"
#include "endstone/core/json.h"
#include "endstone/core/level/level.h"
#include "endstone/core/player.h"
#include "endstone/core/signal_handler.h"
#include "endstone/runtime/hook.h"

using endstone::core::EndstonePlayer;

void PlayerEventCoordinator::sendEvent(const EventRef<PlayerGameplayEvent<void>> &ref)
{
    void (PlayerEventCoordinator::*fp)(const EventRef<PlayerGameplayEvent<void>> &) =
        &PlayerEventCoordinator::sendEvent;
    auto visitor = endstone::overloaded{
        [](const Details::ValueOrRef<PlayerFormCloseEvent const> &arg) {
            const auto &event = arg.value();
            const StackResultStorageEntity entity(event.player);
            if (const auto *player = static_cast<Player *>(Actor::tryGetFromEntity(entity.getStackRef(), false))) {
                // Players can be null if they are dead when we receive the event
                player->getEndstoneActor<EndstonePlayer>().onFormClose(event.form_id, event.form_close_reason);
            }
        },
        [](const Details::ValueOrRef<PlayerFormResponseEvent const> &arg) {
            const auto &event = arg.value();
            const StackResultStorageEntity entity(event.player);
            if (const auto *player = static_cast<Player *>(Actor::tryGetFromEntity(entity.getStackRef(), false))) {
                // Players can be null if they are dead when we receive the event
                player->getEndstoneActor<EndstonePlayer>().onFormResponse(event.form_id, event.form_response);
            }
        },
        [](auto &&ignored) {},
    };
    std::visit(visitor, ref.get().variant);
    ENDSTONE_HOOK_CALL_ORIGINAL(fp, this, ref);
}
