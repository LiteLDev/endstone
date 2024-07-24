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

#include "endstone/detail/scheduler/async_task.h"

namespace endstone::detail {

bool EndstoneAsyncTask::isSync() const
{
    return false;
}

void EndstoneAsyncTask::run()
{
    if (!isCancelled()) {
        is_running_ = true;
        try {
            EndstoneTask::run();
        }
        catch (std::exception &e) {
            getOwner()->getLogger().warning("Plugin {} generated an exception while executing task {}: {}",
                                            getOwner()->getName(), getTaskId(), e.what());
        }
        is_running_ = false;
    }
    // TODO: remove ourself from scheduler task queue
}

bool EndstoneAsyncTask::isRunning() const
{
    return is_running_.load();
}

}  // namespace endstone::detail