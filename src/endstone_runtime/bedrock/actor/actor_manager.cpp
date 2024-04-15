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

#include "bedrock/actor/actor_manager.h"

#include "bedrock/actor/components/actor_identifier.h"
#include "bedrock/actor/entity_context.h"
#include "endstone/detail/actor/actor.h"
#include "endstone/detail/hook.h"
#include "endstone/detail/level.h"
#include "endstone/detail/server.h"
#include "endstone/event/actor/actor_spawn_event.h"

using endstone::detail::EndstoneActor;
using endstone::detail::EndstoneLevel;
using endstone::detail::EndstoneServer;

Actor *ActorManager::addActorEntity(IAddActorEntityProxy &proxy, OwnerPtr<EntityContext> ctx)
{
    if (!ctx.storage.context.has_value()) {
        return nullptr;
    }

    auto *actor = Actor::tryGetFromEntity(ctx.storage.context.value(), true);
    if (!actor || actor->isPlayer()) {
        return ENDSTONE_HOOK_CALL_ORIGINAL(&ActorManager::addActorEntity, this, proxy, ctx);
    }

    auto &server = entt::locator<EndstoneServer>::value();
    auto endstone_actor = std::make_unique<EndstoneActor>(server, *actor);
    endstone::ActorSpawnEvent e{*endstone_actor};
    server.getPluginManager().callEvent(e);
    if (e.isCancelled()) {
        return nullptr;
    }

    actor = ENDSTONE_HOOK_CALL_ORIGINAL(&ActorManager::addActorEntity, this, proxy, ctx);
    if (actor) {
        auto &bedrock_level = actor->getLevel();
        auto *level = static_cast<EndstoneLevel *>(server.getLevel(bedrock_level.getLevelId()));
        if (!level) {
            throw std::runtime_error("Unable to find the level associated with the actor.");
        }

        level->addActor(std::make_unique<EndstoneActor>(server, *actor));
    }
    return actor;
}