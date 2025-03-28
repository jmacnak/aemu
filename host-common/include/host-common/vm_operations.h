// Copyright 2015 The Android Open Source Project
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

#include "aemu/base/c_header.h"

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

// Caching types
#define MAP_CACHE_MASK      0x0f
#define MAP_CACHE_NONE      0x00
#define MAP_CACHE_CACHED    0x01
#define MAP_CACHE_UNCACHED  0x02
#define MAP_CACHE_WC        0x03

ANDROID_BEGIN_HEADER
// This file includes interfaces to VMMs.

// A struct describing the information about host memory associated
// with a host memory id. Used with virtio-gpu-next.
struct HostmemEntry {
    uint64_t id;
    void* hva;
    uint64_t size;
    uint32_t caching;
};

// Called by hostmemRegister(..)
struct MemEntry {
    void* hva;
    uint64_t size;
    uint32_t register_fixed;
    uint64_t fixed_id;
    uint32_t caching;
};

// A callback to consume a single line of output (including newline).
// |opque| is a handle to a context object. Functions that use this callback
// will also take a context handle that will be passed to the callback.
// |buff| contains the data to be consumed, of length |len|.
// The function should return the number of chars consumed successfully.
typedef int (*LineConsumerCallback)(void* opaque, const char* buff, int len);

// Enumeration of various causes for shutdown. Please keep this in sync
// with the similar enum in include/sysem/sysemu.h.
typedef enum QemuShutdownCause {
    QEMU_SHUTDOWN_CAUSE_NONE,              /* No shutdown request pending */
    QEMU_SHUTDOWN_CAUSE_HOST_ERROR,        /* An error prevents further use of guest */
    QEMU_SHUTDOWN_CAUSE_HOST_QMP,          /* Reaction to a QMP command, like 'quit' */
    QEMU_SHUTDOWN_CAUSE_HOST_SIGNAL,       /* Reaction to a signal, such as SIGINT */
    QEMU_SHUTDOWN_CAUSE_HOST_UI,           /* Reaction to UI event, like window close */
    QEMU_SHUTDOWN_CAUSE_GUEST_SHUTDOWN,    /* Guest shutdown/suspend request, via
                                              ACPI or other hardware-specific means */
    QEMU_SHUTDOWN_CAUSE_GUEST_RESET,       /* Guest reset request, and command line
                                              turns that into a shutdown */
    QEMU_SHUTDOWN_CAUSE_GUEST_PANIC,       /* Guest panicked, and command line turns
                                              that into a shutdown */
    QEMU_SHUTDOWN_CAUSE__MAX,
} QemuShutdownCause;

#define SHUTDOWN_CAUSE_STATIC_ASSERT_ERROR_MESSAGE "QEMU shutdown cause values differ from AndroidEmu's!" \

#define SHUTDOWN_CAUSE_STATIC_MATCH(origCause) \
    static_assert((int)(QEMU_##origCause) == (int)(origCause), SHUTDOWN_CAUSE_STATIC_ASSERT_ERROR_MESSAGE);

#define STATIC_ASSERT_SHUTDOWN_CAUSE_MATCHES \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_NONE) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_HOST_ERROR) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_HOST_QMP) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_HOST_SIGNAL) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_HOST_UI) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_GUEST_SHUTDOWN) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_GUEST_RESET) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE_GUEST_PANIC) \
    SHUTDOWN_CAUSE_STATIC_MATCH(SHUTDOWN_CAUSE__MAX) \

typedef struct {
    int (*onStart)(void* opaque, const char* name);
    void (*onEnd)(void* opaque, const char* name, int res);
    void (*onQuickFail)(void* opaque, const char* name, int res);
    bool (*isCanceled)(void* opaque, const char* name);
} SnapshotCallbackSet;

typedef enum {
    SNAPSHOT_SAVE,
    SNAPSHOT_LOAD,
    SNAPSHOT_DEL,
    SNAPSHOT_OPS_COUNT
} SnapshotOperation;

struct SnapshotRamBlock;

typedef struct {
    void (*registerBlock)(void* opaque,
                          SnapshotOperation operation,
                          const struct SnapshotRamBlock* block);
    int (*startLoading)(void* opaque);
    void (*savePage)(void* opaque,
                     int64_t blockOffset,
                     int64_t pageOffset,
                     int32_t size);
    int (*savingComplete)(void* opaque);
    void (*loadRam)(void* opaque,
                    void* hostRamPtr,
                    uint64_t size);
} SnapshotRamCallbacks;

typedef struct {
    SnapshotCallbackSet ops[SNAPSHOT_OPS_COUNT];
    SnapshotRamCallbacks ramOps;
} SnapshotCallbacks;

typedef enum {
    HV_UNKNOWN,
    HV_NONE,
    HV_KVM,
    HV_HAXM,
    HV_HVF,
    HV_WHPX,
} VmHypervisorType;

typedef struct {
    VmHypervisorType hypervisorType;
    int32_t numberOfCpuCores;
    int64_t ramSizeBytes;
} VmConfiguration;

typedef enum EmuRunState {
    QEMU_RUN_STATE_DEBUG = 0,
    QEMU_RUN_STATE_INMIGRATE = 1,
    QEMU_RUN_STATE_INTERNAL_ERROR = 2,
    QEMU_RUN_STATE_IO_ERROR = 3,
    QEMU_RUN_STATE_PAUSED = 4,
    QEMU_RUN_STATE_POSTMIGRATE = 5,
    QEMU_RUN_STATE_PRELAUNCH = 6,
    QEMU_RUN_STATE_FINISH_MIGRATE = 7,
    QEMU_RUN_STATE_RESTORE_VM = 8,
    QEMU_RUN_STATE_RUNNING = 9,
    QEMU_RUN_STATE_SAVE_VM = 10,
    QEMU_RUN_STATE_SHUTDOWN = 11,
    QEMU_RUN_STATE_SUSPENDED = 12,
    QEMU_RUN_STATE_WATCHDOG = 13,
    QEMU_RUN_STATE_GUEST_PANICKED = 14,
    QEMU_RUN_STATE_COLO = 15,
    QEMU_RUN_STATE__MAX = 16,
} EmuRunState;

typedef enum SnapshotSkipReason {
    SNAPSHOT_SKIP_UNKNOWN = 0,
    SNAPSHOT_SKIP_UNSUPPORTED_VK_APP = 1,
    SNAPSHOT_SKIP_UNSUPPORTED_VK_API = 2,
} SnapshotSkipReason;

inline const char* toString_SnapshotSkipReason(SnapshotSkipReason reason) {
    switch (reason) {
        case SNAPSHOT_SKIP_UNKNOWN:
            return "UNKNOWN";
        case SNAPSHOT_SKIP_UNSUPPORTED_VK_APP:
            return "UNSUPPORTED_VK_APP";
        case SNAPSHOT_SKIP_UNSUPPORTED_VK_API:
            return "UNSUPPORTED_VK_API";
    }
    return "UNKNOWN";
}

// C interface to expose Qemu implementations of common VM related operations.
typedef struct QAndroidVmOperations {
    bool (*vmStop)(void);
    bool (*vmStart)(void);
    void (*vmReset)(void);
    void (*vmShutdown)(void);
    bool (*vmPause)(void);
    bool (*vmResume)(void);

    bool (*vmIsRunning)(void);

    // Snapshot-related VM operations.
    // |outConsuer| and |errConsumer| are used to report output / error
    // respectively. Each line of output is newline terminated and results in
    // one call to the callback. |opaque| is passed to the callbacks as context.
    // Returns true on success, false on failure.
    bool (*snapshotList)(void* opaque,
                         LineConsumerCallback outConsumer,
                         LineConsumerCallback errConsumer);
    bool (*snapshotSave)(const char* name,
                         void* opaque,
                         LineConsumerCallback errConsumer);
    bool (*snapshotLoad)(const char* name,
                         void* opaque,
                         LineConsumerCallback errConsumer);
    bool (*snapshotDelete)(const char* name,
                           void* opaque,
                           LineConsumerCallback errConsumer);
    bool (*snapshotRemap)(bool shared,
                          void* opaque,
                          LineConsumerCallback errConsumer);

    // Export the qcow2s associated with the given snapshot to the given destination.
    bool (*snapshotExport)(const char* snapshot,
                           const char* dest,
                           void* opaque,
                           LineConsumerCallback errConsumer);

    // Get the name of the last loaded snapshot (current snapshot).
    // Will print "(null)" if the emulator cold booted and loaded no snapshots.
    bool (*snapshotLastLoaded)(void* opaque,
                             LineConsumerCallback outConsumer,
                             LineConsumerCallback errConsumer);

    // Sets a set of callback to listen for snapshot operations.
    void (*setSnapshotCallbacks)(void* opaque,
                                 const SnapshotCallbacks* callbacks);

    // Sets a protobuf for the snapshot save and load
    void (*setSnapshotProtobuf)(void* pb);
    // callbacks to "plug" and "unplug" memory into the provided address range
    // on fly.
    void (*mapUserBackedRam)(uint64_t gpa, void* hva, uint64_t size);
    void (*unmapUserBackedRam)(uint64_t gpa, uint64_t size);

    // Fills in the supplied |out| with current VM configuration.
    void (*getVmConfiguration)(VmConfiguration* out);

    // Notifies QEMU of failed operations according to our own
    // android::snapshot::FailureReason.
    void (*setFailureReason)(const char* name, int failureReason);

    // Notifies QEMU that the emulator is exiting, can impact how
    // QEMU snapshot save calls work.
    void (*setExiting)(void);

    // Allow actual audio on host through to the guest.
    void (*allowRealAudio)(bool allow);

    // Get the host address of a guest physical address, if any.
    void* (*physicalMemoryGetAddr)(uint64_t gpa);

    // Query whether host audio is allowed.
    bool (*isRealAudioAllowed)(void);

    // Set whether to skip snapshotting on exit.
    void (*setSkipSnapshotSave)(bool used);

    // Retrieve the state of whether snapshotting is skipped.
    bool (*isSnapshotSaveSkipped)(void);

    // Create/register/getinfo for host memory Id's
    uint64_t (*hostmemRegister)(const struct MemEntry *entry);
    void (*hostmemUnregister)(uint64_t id);
    struct HostmemEntry (*hostmemGetInfo)(uint64_t id);
    EmuRunState (*getRunState)();

    // virtio display
    bool (*setDisplay)(int32_t id, int32_t w, int32_t h, uint32_t dpi);

    // Reset the machine
    void (*system_shutdown_request)(QemuShutdownCause reason);

    void (*vulkanInstanceRegister)(uint64_t id, const char* name);
    void (*vulkanInstanceUnregister)(uint64_t id);
    // get the list vk app id and name so we might be able to stop them
    // before saving snapshot
    void (*vulkanInstanceEnumerate)(uint32_t* pCount, uint64_t* pIds, char** pNames);

    // Set the reason to skip snapshotting on exit.
    void (*setSkipSnapshotSaveReason)(SnapshotSkipReason reason);

    // Get the reason to skip snapshotting on exit.
    SnapshotSkipReason (*getSkipSnapshotSaveReason)();

    // Set Vulkan snapshot is actively used, for stats.
    void (*setStatSnapshotUseVulkan)(void);

    // Check if Vulkan snapshot is actively used, for stats.
    bool (*snapshotUseVulkan)();
} QAndroidVmOperations;
ANDROID_END_HEADER
