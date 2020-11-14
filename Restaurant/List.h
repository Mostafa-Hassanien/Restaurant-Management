#ifndef __LIST_H_
#define __LIST_H_

#include "Generic_DS\Node.h"

template <class T>
class List 
{
private:
	Node<T>* Head;
	Node<T>* GetNodeAt(int pos);
public:
	List();
	bool Is_Empty();
	int Get_Length();
	bool Insert(int pos, T item );
	bool Insert_Priority(double Priority_newEntry,const T& newEntry); // add an item to the list according to a priority equation
	bool Remove_Entry(T &item);
	void clear();
	T Get_Entry(int pos);
	bool Get_EntryID(int oID, T & item); // return the item(order pointer) with the given ID, it is mainly used in promotion and cancellation events
	T* toArray(int& count);	//returns array of T (array if items)
};
template <class T>
Node<T>* List<T>::GetNodeAt(int pos)
{
	if ( pos >=0 && pos < Get_Length())
	{
		int count = 0;
		Node<T>* ptr = Head;
		while ( ptr )
		{
			if ( count == pos )
			{
				return ptr;
			} 
		   count ++;
		   ptr = ptr->getNext();
		}
	}
	return nullptr;
}
template <class T>
List<T>::List()
{
   Head = nullptr;
}
template <class T>
bool List<T>::Is_Empty()
{
	return ( Head == nullptr);
}
template <class T>
int List<T>::Get_Length()
{
	int count = 0;
	Node<T>* ptr = Head;
	while ( ptr )
	{
	   count ++;
	   ptr = ptr ->getNext();
	}
	return count;
}
template <class T>
void List<T>::clear()
{
	while (head)
	   {
	      remove (0);
	   }
}
template <class T>
T List<T>::Get_Entry(int pos)
{
	if ( pos >= 0 && pos < (Get_Length()+1) ) 
	{
	Node<T>* ptr = GetNodeAt(pos);
	if(ptr)
	return ptr->getItem();
	}
	return nullptr;
}
template <class T>
bool List<T>::Insert(int pos, T item )
{
	if ( pos < 0 || pos > Get_Length() ) return false;
	Node<T>* ptr = new Node<T>;
	ptr ->setItem(item);
	if ( pos == 0 )
	{
		ptr ->setNext(Head);
		Head = ptr;
		return true;
	}
	Node<T>* prev_ptr = GetNodeAt(pos-1);
	ptr ->setNext(prev_ptr->getNext());
	prev_ptr->setNext(ptr);
	return true;
}
template <class T>
bool List<T>::Insert_Priority(double Priority_newEntry,const T& newEntry)
{
	Node<T>* Newnode = new Node<T>;
	Newnode ->setItem(newEntry);
	Newnode->Set_Priority(Priority_newEntry); 
	if ( Head == nullptr)
	{
	  Head = Newnode; 
	  Newnode ->setNext(nullptr);
	  return true;
	}
	if ( Priority_newEntry > Head->getPriority() )
	{
		Newnode ->setNext(Head);
		Head = Newnode;
		return true;
	}
	Node<T>* ptr = Head;
	if (!ptr->getNext())
	{
		Head ->setNext(Newnode);
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

template <class T>
T* List<T>::toArray(int& count)	//returns array of T (array if items)
{
	count=0;

	if(!Head)
		return nullptr;
	//counting the no. of items in the Queue
	Node<T>* p = Head;
	while(p)
	{
		count++;
		p = p->getNext();
	}


	T* Arr= new T[count];
	p = Head;
	for(int i=0; i<count;i++)
	{
		Arr[i] = p->getItem();
		p = p->getNext();
	}
	return Arr;
}

template <typename T>
bool List<T>::Get_EntryID(int oID, T & item)
{
	if ( Head != NULL )
	{
		Node<T>* ptr = Head;
		while (ptr)
		{
			Order* pOrder= dynamic_cast<Order*> (ptr->getItem());
			if ( pOrder && pOrder->GetID() == oID && pOrder->getStatus() == WAIT)
			{
					item = ptr ->getItem();
					return true;
			}
			ptr = ptr->getNext();
		}
	}
	return false;
}
template <typename T>
bool List<T>::Remove_Entry(T &item)
{
	if ( Head == nullptr) return false;
	if (Head->getItem() == item)
	{
		Node<T>* deleteptr = Head;
		Head = Head->getNext();
		deleteptr->setNext(nullptr);
		item  = deleteptr ->getItem();
		return true;
	}
	Node<T>* ptr = Head;
	while(ptr->getNext())
	{
		if ( ptr->getNext()->getItem() == item)
		{
			Node<T>* deleteptr =  ptr->getNext();
			ptr->setNext(deleteptr->getNext());
			deleteptr->setNext(nullptr);
			item  = deleteptr ->getItem();
			return true;
		}
		ptr = ptr ->getNext();
	}
	return false;
}

#endif