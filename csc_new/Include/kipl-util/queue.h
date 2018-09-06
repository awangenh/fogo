#ifndef _queue_h_
#define _queue_h_

/***********************************************
  anced Container Classes Version 1.0
  
  Copyright 2002, Patrick Sturm
************************************************/


// Include-Files
#include <kipl-base/error.h>

namespace kipl {

// Exceptions
KIPL_NEW_STD_EXCEPTION(EEmptyQueue,"Queue is empty...")
KIPL_NEW_STD_EXCEPTION(ESizeTooSmall,"Queue is empty...")


template <typename ElementType>
class Queue
{
protected:
  ElementType* data;
  int size,delta;
  int first,last,count;
public:
  Queue(int initSize=0, int initDelta = -1);
  virtual ~Queue();

  inline void init(int initSize, int initDelta = -1);
  inline void resize(int initSize);
  inline void enqueue(const ElementType& element);
  inline void dequeue();
  inline const ElementType& current() const;
  inline int numOfElements() const;
  inline void clear() const;
  inline bool isEmpty() const;
};


// Implementation of Vector
template <typename ElementType>
inline Queue<ElementType>::Queue(int initSize, int initDelta)
{
  data=new ElementType[initSize];
  size=initSize;
  if (initDelta>=0) delta=initDelta;
  else delta=initSize;
  first=0;
  last=0;
  count=0;
}


template <typename ElementType>
Queue<ElementType>::~Queue()
{
  if (data!=NULL) delete[] data;
}


template <typename ElementType>
inline void Queue<ElementType>::init(int initSize, int initDelta)
{
  if (data!=NULL) delete[] data;
  data=new ElementType[initSize];
  size=initSize;
  delta=initDelta;
  first=0;
  last=0;
  count=0;
}


template <typename ElementType>
inline void Queue<ElementType>::resize(int initSize)
{
  if (initSize>=count)
  {
    ElementType* newData = new ElementType[initSize];

    if (newData==NULL) throw EOutOfMemory();
    if (first<last)
      memcpy(newData,&(data[first]),count*sizeof(ElementType));
    else if (count>0)
    {
      memcpy(newData,&(data[first]),(size-first)*sizeof(ElementType));
      memcpy(&(newData[size-first]),data,last*sizeof(ElementType));
    }
    first=0;
    last=count;
    size=initSize;
    delete[] data;
    data=newData;
  }
  else throw ESizeTooSmall();
}


template <typename ElementType>
inline void Queue<ElementType>::enqueue(const ElementType& element)
{
  if (count>=size) resize(size+delta);
  data[last]=element;
  count++;
  last++;
  if (last>=size) last=0;
}


template <typename ElementType>
inline void Queue<ElementType>::dequeue()
{
  if (count==0) throw EEmptyQueue();
  count--;
  first++;
  if (first>=size)
  {
    first=0;
    last=first+count;
  }
}


template <typename ElementType>
inline const ElementType& Queue<ElementType>::current() const
{
  if (count==0) throw EEmptyQueue();
    
  return data[first];
}


template <typename ElementType>
inline int Queue<ElementType>::numOfElements() const
{
  return count;
}


template <typename ElementType>
inline void Queue<ElementType>::clear() const
{
  first=0;
  last=0;
  count=0;
}


template <typename ElementType>
inline bool Queue<ElementType>::isEmpty() const
{
  return (count==0);
}

}

#endif
