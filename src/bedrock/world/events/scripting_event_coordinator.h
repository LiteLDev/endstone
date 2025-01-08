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

#include "bedrock/gameplayhandlers/scripting_event_handler.h"
#include "bedrock/world/events/event_coordinator.h"
#include "bedrock/world/events/scripting_event_listener.h"
#include "bedrock/world/events/scripting_events.h"

class ScriptingEventCoordinator : public EventCoordinatorPimpl<ScriptingEventListener> {
public:
    ENDSTONE_HOOK CoordinatorResult sendEvent(EventRef<ScriptingGameplayEvent<CoordinatorResult>> ref);

private:
    std::unique_ptr<ScriptingEventHandler> scripting_event_handler_;  // +112 (+120)
};