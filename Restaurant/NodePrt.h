
#ifndef __NODEPR_H_
#define __NODEPR_H_

template < typename T>
class NodePrt
{
private :
	T item; // A data item
	NodePrt<T>* next; // Pointer to next node
	double Priority; // an indication of the priority of the VIP customer
public :
	NodePrt();
	NodePrt( const T & r_Item);	//passing by const ref.
	NodePrt( const T & r_Item, NodePrt<T>* nextNodePtr);
	void setItem( const T & r_Item);
	void setNext(NodePrt<T>* nextNodePtr);
	void setPriority (const double P);
	T getItem() const ;
	NodePrt<T>* getNext() const ;
	double getPriority () const;
}; // end Node


template < typename T>
NodePrt<T>::NodePrt() 
{
	next = nullptr;
} 

template < typename T>
NodePrt<T>::NodePrt( const T& r_Item)
{
	item = r_Item;
	next = nullptr;
} 

template < typename T>
NodePrt<T>::NodePrt( const T& r_Item, NodePrt<T>* nextNodePtr)
{
	item = r_Item;
	next = nextNodePtr;
}
template < typename T>
void NodePrt<T>::setItem( const T& r_Item)
{
	item = r_Item;
} 

template < typename T>
void NodePrt<T>::setNext(NodePrt<T>* nextNodePtr)
{
	next = nextNodePtr;
} 
template < typename T>
void NodePrt<T>::setPriority (const double P)
{
	Priority = P;
}
template < typename T>
T NodePrt<T>::getItem() const
{
	return item;
} 

template < typename T>
NodePrt<T>* NodePrt<T>::getNext() const
{
	return next;
} 

template < typename T>
double NodePrt<T>::getPriority() const
{
	return Priority;
}

#endif