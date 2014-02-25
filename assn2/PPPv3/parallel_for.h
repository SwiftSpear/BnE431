// Penn Parallel Primitives Library
// Author: Prof. Milo Martin
// University of Pennsylvania
// Spring 2010

#ifndef PPP_FOR_H
#define PPP_FOR_H

#include "ppp.h"
#include "Task.h"
#include "TaskGroup.h"

namespace ppp {
  
  namespace internal {
    template <typename T>
    extern inline
    void parallel_for(int64_t start, int64_t end, T* functor, int64_t grainsize=0)
    {
      // ASSIGNMENT: make this parallel via recursive divide and conquer
      if (grainsize == 0) {
        grainsize = (start-end+1) / (get_thread_count()*4);
      }
      PPP_DEBUG_MSG("parallel_for grainsize: " + to_string(grainsize));
      functor->calculate(start, end);
    }
  }
}

#endif
