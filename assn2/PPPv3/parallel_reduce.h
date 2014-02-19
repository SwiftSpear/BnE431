// Penn Parallel Primitives Library
// Author: Prof. Milo Martin
// University of Pennsylvania
// Spring 2010

#ifndef PPP_REDUCE_H
#define PPP_REDUCE_H

#include "ppp.h"
#include "Task.h"
#include "TaskGroup.h"

namespace ppp {
  
  template <typename T>
  extern inline
  T parallel_reduce(T* array, int64_t start, int64_t end, int64_t grainsize=0)
  {
    // ASSIGNMENT: make this parallel via recursive divide and conquer

    // Sequential code
    T sum;
    sum = T(0);
    for (int i=start; i<end; i++) {
      sum = sum + array[i];
    }
    return sum;
  }
}

#endif
