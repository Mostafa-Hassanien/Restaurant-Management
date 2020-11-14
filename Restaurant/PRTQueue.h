#ifndef __PRTQUEUE_H_
#define __PRTQUEUE_H_

#include "NodePrt.h"
template <typename T>
class PrtQueue
{
private :
	NodePrt<T>* frontPtr;
public :
	PrtQueue();	
	bool isEmpty() const ;
	bool enqueuePriority(double Priority_newEntry,const T& newEntry); // add an item to the queue according to the priority 
	bool dequeue(T& frntEntry);  // remove the first element in the queue 
	bool peekFront(T& frntEntry)  const;
	T* toArray(int& count);	//returns array of T (array if items)
	~PrtQueue();
};

template <typename T>
PrtQueue<T>::PrtQueue()
{
	frontPtr=nullptr;
}

template <typename T>
bool PrtQueue<T>::enqueuePriority(double Priority_newEntry , const T& newEntry)
{
	NodePrt<T>* Newnode = new NodePrt<T>;
	Newnode ->setItem(newEntry);
	Newnode->setPriority(Priority_newEntry); 
	if ( frontPtr == nullptr)
	{
	  frontPtr = Newnode; 
	  Newnode ->setNext(nullptr);
	  return true;
	}
	if ( Priority_newEntry > frontPtr->getPriority() )
	{
		Newnode ->setNext(frontPtr);
		frontPtr = Newnode;
		return true;
	}
	NodePrt<T>* ptr = frontPtr;
	if (!ptr->getNext())
	{
		frontPtr ->setNext(Newnode);
		Newnode ->setNext(nullptr);
		return true;
	}
	while ( ptr->getNext())
	{
	    if(ptr->getNext()->getPriority() < Priority_newEntry)
		{
			Newnode->setNext(ptr->getNext());
			ptr->setNext(Newnode);
			return true;
		}
	ptr = ptr->getNext();
	}
	ptr ->setNext(Newnode);
	Newnode->setNext(nullptr);
	return true;
}
template <typename T>
bool PrtQueue<T>:: isEmpty() const 
{
	return (frontPtr == nullptr);
}

template <typename T>
bool PrtQueue<T>:: dequeue(T& frntEntry)  
{
	if(isEmpty())
		return false;

	NodePrt<T>* nodeToDeletePtr = frontPtr;
	frntEntry = frontPtr->getItem();
	frontPtr = frontPtr->getNext();
	// Queue is not empty; remove front
		
	// Free memory reserved by the dequeued node
	delete nodeToDeletePtr;
	return true;

}

template <typename T>
bool PrtQueue<T>:: peekFront(T& frntEntry) const 
{
	if(isEmpty())
		return false;

	frntEntry = frontPtr->getItem();
	return true;

}

template <typename T>
PrtQueue<T>::~PrtQueue()
{
}

template <typename T>
T* PrtQueue<T>::toArray(int& count)
{
	count=0;

	if(!frontPtr)
		return nullptr;
	//counting the no. of items in the Queue
	NodePrt<T>* p = frontPtr;
	while(p)
	{
		count++;
		p = p->getNext();
	}


	T* Arr= new T[count];
	p = frontPtr;
	for(int i=0; i<count;i++)
	{
		Arr[i] = p->getItem();
		p = p->getNext();
	}
	return Arr;
}
#endif