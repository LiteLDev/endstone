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

#pragma once

#include "bedrock/gameplayhandlers/level_gameplay_handler.h"
#include "bedrock/world/actor/actor.h"
#include "bedrock/world/actor/actor_initialization_method.h"
#include "bedrock/world/events/event_coordinator.h"
#include "bedrock/world/events/level_event_listener.h"

class LevelEventCoordinator : public EventCoordinatorPimpl<LevelEventListener> {
public:
private:
    ENDSTONE_HOOK void _postReloadActorAdded(Actor &, ActorInitializationMethod);

    std::unique_ptr<LevelGameplayHandler> level_gameplay_handler_;     // +112
    std::shared_ptr<Bedrock::PubSub::SubscriptionBase> subscription_;  // +120
};
