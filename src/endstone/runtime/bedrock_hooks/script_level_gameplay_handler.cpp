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

#include "bedrock/scripting/event_handlers/script_level_gameplay_handler.h"

#include <entt/entt.hpp>

#include "bedrock/gameplayhandlers/gameplay_handler_result.h"
#include "bedrock/world/actor/player/player.h"
#include "bedrock/world/events/level_events.h"
#include "endstone/core/server.h"
#include "endstone/event/weather/thunder_change_event.h"
#include "endstone/event/weather/weather_change_event.h"
#include "endstone/runtime/hook.h"

using endstone::core::EndstoneLevel;
using endstone::core::EndstoneServer;

GameplayHandlerResult<CoordinatorResult> ScriptLevelGameplayHandler::handleEvent(LevelWeatherChangedEvent &event)
{
    auto &server = entt::locator<EndstoneServer>::value();
    auto &level = *server.getLevel();

    if (event.from_rain != event.to_rain) {
        endstone::WeatherChangeEvent e(level, event.to_rain);
        server.getPluginManager().callEvent(e);
        if (e.isCancelled()) {
            event.to_rain = event.from_rain;
        }
    }

    if (event.from_lightning != event.to_lightning) {
        endstone::ThunderChangeEvent e(level, event.to_lightning);
        server.getPluginManager().callEvent(e);
        if (e.isCancelled()) {
            event.to_lightning = event.from_lightning;
        }
    }

    return ENDSTONE_HOOK_CALL_ORIGINAL(&ScriptLevelGameplayHandler::handleEvent, this, event);
}
