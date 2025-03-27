// Copyright (C) 2020 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdio.h>

#include "host-common/GraphicsAgentFactory.h"

namespace android {
namespace emulation {

// Creates a series of Mock Agents that can be used by the unit tests.
//
// Most of the agents are not defined, add your agents here if you need
// access to addition mock agents. They will be automatically injected
// at the start of the unit tests.
class MockGraphicsAgentFactory : public GraphicsAgentFactory {
public:
    const QAndroidVmOperations* android_get_QAndroidVmOperations()
            const override;

    const QAndroidMultiDisplayAgent*
    android_get_QAndroidMultiDisplayAgent() const override;

    const QAndroidEmulatorWindowAgent*
    android_get_QAndroidEmulatorWindowAgent() const override;


};

}  // namespace emulation
}  // namespace android
