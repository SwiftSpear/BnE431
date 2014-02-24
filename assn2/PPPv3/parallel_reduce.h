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
  namespace internal{
  template <typename T>
  class ReduceTask: public ppp::Task {
  public:
  	
  	ReduceTask(T* array, int64_t start, int64_t end, int64_t grainsize){
  		m_array = array;
  		m_start = start;
  		m_end = end;
  		m_grainsize = grainsize;
  	}
  	void add(T* array, int64_t start, int64_t end){
  		for (int64_t i=start; i<end; i++) {
      	sum = sum + array[i];
    }
  	}

  	void execute(){
    assert (m_start < m_end);
    int64_t mid = (m_start + m_end)/2;

    if(m_start >= m_end)
    {
    	return;
    }
    
    if ((m_end - m_start) < m_grainsize) {
  		add(m_array, m_start, m_end);
  		return;
    }
    assert(mid < m_end);

   	ppp:TaskGroup tg;
   	ReduceTask r1(m_array, m_start, mid, m_grainsize);
   	ReduceTask r2(m_array, mid, m_end, m_grainsize);
   	tg.spawn(r1);
    tg.spawn(r2);
   	tg.wait();


  	}

  	T get_sum(){
  		return sum;
  	}
  	private:
  	  T sum = T(0);
      T* m_array;
      int64_t m_start;
      int64_t m_end;
      int64_t m_grainsize;

 };
}




  template <typename T>
  extern inline
  T parallel_reduce(T* array, int64_t start, int64_t end, int64_t grainsize=0)
  {
    // ASSIGNMENT: make this parallel via recursive divide and conquer
  		if (grainsize == 0)
  	{
  		grainsize = (start-end+1) / (get_thread_count()*4);
  	}
    // Sequential code
    
    internal::ReduceTask<T> t(array, start, end, grainsize);
    t.execute();
    //printf("%d\n", t.get_sum() );
    return t.get_sum();
  }
}

#endif
