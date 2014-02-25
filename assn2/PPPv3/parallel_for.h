// Penn Parallel Primitives Library
// Author: Prof. Milo Martin
// University of Pennsylvania
// Spring 2010

#ifndef PPP_FOR_H
#define PPP_FOR_H

#include <math.h>
#include "ppp.h"
#include "Task.h"
#include "TaskGroup.h"

namespace ppp {
  
  namespace internal {
    template <typename T>
    class ForTask: public ppp::Task {
    public:
      ForTask(int64_t start, int64_t end, T* functor, int64_t grainsize) {
        m_function = functor;
        m_start = start;
        m_end = end;
        m_grainsize = grainsize;
      }
      
      void execute() {
        PPP_DEBUG_MSG("Executing for from " + to_string(m_start) + " to " + to_string(m_end));
        assert(m_start < m_end);
        if (m_end-m_start <= 1) {
          PPP_DEBUG_MSG("Performing work of size " + to_string(m_end-m_start))
          m_function->calculate(m_start, m_end);
          return;
        }
        if (m_end-m_start <= m_grainsize) {
          PPP_DEBUG_MSG("Performing work of size " + to_string(m_end-m_start))
          m_function->calculate(m_start, m_end);
          return;
        }
        ppp::TaskGroup tg;
        int64_t tempend;
        PPP_DEBUG_MSG("Spliting work into " + to_string(ceil(double(m_end - m_start)/double(m_grainsize))) + " threads");
        for (int64_t counter = m_start; counter <= m_end; counter = counter + m_grainsize) {
          tempend = counter + m_grainsize - 1;
          if (tempend > m_end) {
            tempend = m_end;
          }
          ForTask* temp = new ForTask(counter, tempend, m_function, m_grainsize);
          tg.spawn(*temp);
          
        }
        tg.wait();
      }
      private:
        T* m_function;
        int64_t m_start;
        int64_t m_end;
        int64_t m_grainsize;
    };
  }
  template <typename T>
  extern inline
  void parallel_for(int64_t start, int64_t end, T* functor, int64_t grainsize=0)
  {
    // ASSIGNMENT: make this parallel via recursive divide and conquer
    if (grainsize == 0) {
      grainsize = (end-start+1) / (get_thread_count()*4);
    }
    PPP_DEBUG_MSG("parallel_for grainsize: " + to_string(grainsize));
    internal::ForTask<T> t(start, end, functor, grainsize);
    t.execute();
    PPP_DEBUG_MSG("parallel_for done")
  }
  
}

#endif
