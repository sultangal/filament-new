/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MetalResourceTracker.h"

#include <utils/debug.h>

namespace filament {
namespace backend {

bool MetalResourceTracker::trackResource(CommandBuffer buffer, Resource resource,
        ResourceDeleter deleter) {
    std::lock_guard<std::mutex> lock(mMutex);

    auto found = mResources.find(buffer);
    if (found == mResources.end()) {
        Resources& resourcesForBuffer = mResources[buffer] = {};
        resourcesForBuffer.insert({resource, deleter});
        return true;
    }

    assert_invariant(found != mResources.end());
    Resources& resources = found.value();
    auto inserted = resources.insert({resource, deleter});
    return inserted.second;
}

void MetalResourceTracker::clearResources(CommandBuffer buffer) {
    std::lock_guard<std::mutex> lock(mMutex);

    auto found = mResources.find(buffer);
    if (found == mResources.end()) {
        return;
    }

    for (const auto& resource : found.value()) {
        resource.deleter(resource.resource);
    }
    mResources.erase(found);
}

} // namespace backend
} // namespace filament
