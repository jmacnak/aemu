// Copyright 2019 The Android Open Source Project
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

#include "aemu/base/Compiler.h"
#include "aemu/base/containers/StaticMap.h"
#include "aemu/base/export.h"
#include "aemu/base/ManagedDescriptor.hpp"
#include "vm_operations.h"

#include <atomic>

#include <inttypes.h>
#include <unordered_map>
#include <optional>

// A global mapping from opaque host memory IDs to host virtual
// addresses/sizes.  This is so that the guest doesn't have to know the host
// virtual address to be able to map them. However, we do also provide a
// mechanism for obtaining the offsets into page for such buffers (as the guest
// does need to know those).
//
// This is currently used only in conjunction with virtio-gpu-next and Vulkan /
// address space device, though there are possible other consumers of this, so
// it becomes a global object. It exports methods into VmOperations.

using android::base::ManagedDescriptor;

namespace android {
namespace emulation {

#define STREAM_HANDLE_TYPE_MEM_OPAQUE_FD 0x1
#define STREAM_HANDLE_TYPE_MEM_DMABUF 0x2
#define STREAM_HANDLE_TYPE_MEM_OPAQUE_WIN32 0x3
#define STREAM_HANDLE_TYPE_MEM_SHM 0x4
#define STREAM_HANDLE_TYPE_MEM_ZIRCON 0x5

#define STREAM_HANDLE_TYPE_SIGNAL_OPAQUE_FD 0x10
#define STREAM_HANDLE_TYPE_SIGNAL_SYNC_FD 0x20
#define STREAM_HANDLE_TYPE_SIGNAL_OPAQUE_WIN32 0x30
#define STREAM_HANDLE_TYPE_SIGNAL_ZIRCON 0x40

struct VulkanInfo {
    uint32_t memoryIndex;
    uint8_t deviceUUID[16];
    uint8_t driverUUID[16];
};

struct ManagedDescriptorInfo {
    ManagedDescriptor descriptor;
    uint32_t handleType;
    uint32_t caching;
    std::optional<VulkanInfo> vulkanInfoOpt;
};

class HostmemIdMapping {
public:
    HostmemIdMapping() = default;

    AEMU_EXPORT static HostmemIdMapping* get();

    using Id = uint64_t;
    using Entry = HostmemEntry;

    static const Id kInvalidHostmemId;

    // Returns kInvalidHostmemId if hva or size is 0.
    AEMU_EXPORT Id add(const struct MemEntry *entry);

    // No-op if kInvalidHostmemId or a nonexistent entry
    // is referenced.
    AEMU_EXPORT void remove(Id id);

    void addMapping(Id id, const struct MemEntry *entry);
    void addDescriptorInfo(Id id, ManagedDescriptor descriptor, uint32_t handleType,
                           uint32_t caching, std::optional<VulkanInfo> vulkanInfoOpt);

    std::optional<ManagedDescriptorInfo> removeDescriptorInfo(Id id);

    // If id == kInvalidHostmemId or not found in map,
    // returns entry with id == kInvalidHostmemId,
    // hva == 0, and size == 0.
    AEMU_EXPORT Entry get(Id id) const;

    // Restores to starting state where there are no entries.
    AEMU_EXPORT void clear();

private:
    std::atomic<Id> mCurrentId {1};
    base::StaticMap<Id, Entry> mEntries;
    std::unordered_map<Id, ManagedDescriptor> mDescriptors;
    std::unordered_map<Id, ManagedDescriptorInfo> mDescriptorInfos;
    DISALLOW_COPY_ASSIGN_AND_MOVE(HostmemIdMapping);
};

} // namespace android
} // namespace emulation

// C interface for use with vm operations
extern "C" {

AEMU_EXPORT uint64_t android_emulation_hostmem_register(const struct MemEntry *entry);
AEMU_EXPORT void android_emulation_hostmem_unregister(uint64_t id);
AEMU_EXPORT HostmemEntry android_emulation_hostmem_get_info(uint64_t id);

} // extern "C"
