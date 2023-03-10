// Copyright 2022 The gRPC Authors
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

#ifndef GRPC_CORE_LIB_EVENT_ENGINE_IOMGR_ENGINE_EV_EPOLL1_LINUX_H
#define GRPC_CORE_LIB_EVENT_ENGINE_IOMGR_ENGINE_EV_EPOLL1_LINUX_H
#include <grpc/support/port_platform.h>

#include <list>
#include <memory>
#include <vector>

#include "absl/base/thread_annotations.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"

#include "src/core/lib/event_engine/iomgr_engine/event_poller.h"
#include "src/core/lib/event_engine/iomgr_engine/wakeup_fd_posix.h"
#include "src/core/lib/gprpp/time.h"
#include "src/core/lib/iomgr/port.h"

#ifdef GRPC_LINUX_EPOLL
#include <sys/epoll.h>
#endif

#define MAX_EPOLL_EVENTS 100

namespace grpc_event_engine {
namespace iomgr_engine {

class Epoll1EventHandle;

// Definition of epoll1 based poller.
class Epoll1Poller : public EventPoller {
 public:
  explicit Epoll1Poller(Scheduler* scheduler);
  EventHandle* CreateHandle(int fd, absl::string_view name,
                            bool track_err) override;
  absl::Status Work(grpc_core::Timestamp deadline,
                    std::vector<EventHandle*>& pending_events) override;
  void Kick() override;
  Scheduler* GetScheduler() { return scheduler_; }
  void Shutdown() override;
  ~Epoll1Poller() override;

 private:
  absl::Status ProcessEpollEvents(int max_epoll_events_to_handle,
                                  std::vector<EventHandle*>& pending_events);
  absl::Status DoEpollWait(grpc_core::Timestamp deadline);
  struct HandlesList {
    Epoll1EventHandle* handle;
    Epoll1EventHandle* next;
    Epoll1EventHandle* prev;
  };
  friend class Epoll1EventHandle;
#ifdef GRPC_LINUX_EPOLL
  struct EpollSet {
    int epfd;

    // The epoll_events after the last call to epoll_wait()
    struct epoll_event events[MAX_EPOLL_EVENTS];

    // The number of epoll_events after the last call to epoll_wait()
    int num_events;

    // Index of the first event in epoll_events that has to be processed. This
    // field is only valid if num_events > 0
    int cursor;
  };
#else
  struct EpollSet {};
#endif
  absl::Mutex mu_;
  Scheduler* scheduler_;
  // A singleton epoll set
  EpollSet g_epoll_set_;
  bool was_kicked_ ABSL_GUARDED_BY(mu_);
  std::list<EventHandle*> free_epoll1_handles_list_ ABSL_GUARDED_BY(mu_);
  std::unique_ptr<WakeupFd> wakeup_fd_;
};

// Return an instance of a epoll1 based poller tied to the specified event
// engine.
Epoll1Poller* GetEpoll1Poller(Scheduler* scheduler);

}  // namespace iomgr_engine
}  // namespace grpc_event_engine

#endif  // GRPC_CORE_LIB_EVENT_ENGINE_IOMGR_ENGINE_EV_EPOLL1_LINUX_H
