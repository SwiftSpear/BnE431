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
  	ReduceTask(T* array, int64_t start, int64_t end, int64_t grainsize, T *sum){
  		
  		m_array = array;
  		m_start = start;
  		m_end = end;
  		m_grainsize = grainsize;
  		m_sum = sum;
  	}
  	void add(T* array, int64_t start, int64_t end){
  		T sum = *m_sum;
  		for (int64_t i=start; i<end; i++) {
      		sum = sum + array[i];
    	}
    	*m_sum = sum;
    	PPP_DEBUG_MSG("m_sum = " + to_string(m_sum));
  	}

  	void execute(){
    assert (m_start < m_end);
    int64_t mid = floor((m_start + m_end)/2);
   
    PPP_DEBUG_MSG("Msum before loop " + to_string(m_sum));
    if(m_start >= m_end)
    {
    	return;
    }
    
    if ((m_end - m_start) < m_grainsize) {
    	PPP_DEBUG_MSG("Msum before adding" + to_string(m_sum));
  		add(m_array, m_start, m_end);
  		return;
    }
    T lhs;
    T rhs;
    lhs = T(0);
    rhs = T(0);
    assert(mid < m_end);
    PPP_DEBUG_MSG("start = " + to_string(m_start));
    PPP_DEBUG_MSG("end= " + to_string(m_end));
   	ppp:TaskGroup tg;
   	ReduceTask r1(m_array, m_start, mid, m_grainsize, &lhs);
   	ReduceTask r2(m_array, mid, m_end, m_grainsize, &rhs);
   	tg.spawn(r1);
    tg.spawn(r2);
    PPP_DEBUG_MSG("Msum after spawning" + to_string(m_sum));
   	tg.wait();
   	if (lhs + rhs != 0){
   	lhs = lhs + rhs;
   }
   	PPP_DEBUG_MSG("LHS + RHS " + to_string(lhs));
   	PPP_DEBUG_MSG("Msum after threads" + to_string(m_sum));



  	}

  	T get_sum(){
  		PPP_DEBUG_MSG("Msum before return " + to_string(m_sum));
  		return m_sum;
  	}
  	private:

  	  
      T* m_array;
      int64_t m_start;
      int64_t m_end;
      int64_t m_grainsize;
      T* m_sum;

 };
}




  template <typename T>
  extern inline
  T parallel_reduce(T* array, int64_t start, int64_t end, int64_t grainsize=0)
  {
  	T sum;
  	sum = T(0);
    // ASSIGNMENT: make this parallel via recursive divide and conquer
  		if (grainsize == 0)
  	{
  		grainsize = (start-end+1) / (get_thread_count()*4);
  	}
    // Sequential code
    
    internal::ReduceTask<T> t(array, start, end, grainsize, &sum);
    t.execute();
    return sum;
}
}
#endif
