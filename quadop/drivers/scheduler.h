// Copyright 2017 Will Stockwell

#ifndef QUADOP_DRIVERS_SCHEDULER_H_
#define QUADOP_DRIVERS_SCHEDULER_H_

#include "stmlib/stmlib.h"

namespace quadop {

class SchedulerBinder {
 public:
    static void Run();
};

class Scheduler {
 public:
  Scheduler() { }
  ~Scheduler() { }

  void Init(uint32_t frequency);
  uint32_t Freq();

 private:
  uint32_t freq_;

  DISALLOW_COPY_AND_ASSIGN(Scheduler);
};

}  // namespace quadop

#endif  // QUADOP_DRIVERS_SCHEDULER_H_
