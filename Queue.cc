
#include <iostream>
#include <cstdlib>
using namespace std;

#include "Queue.h"

// Default Constructor
Queue::Queue()
{
} // Queue::Queue()

// Insert an object into the queue
void Queue::enqueue(int data)
{
    insertBack(data);
} // Queue::enqueue(data)

int Queue::dequeue()
{
    removeFront();
} // Queue::dequeue()

int Queue::front()
{
    if (!isEmpty())
        return *begin();
    else
    {
        cout << "attempt to acces the front of an empty queue" << endl;
        exit(1);
    }
} // Queue::front

void Queue::makeEmpty()
{
    List::makeEmpty();
} // Queue::makeEmpty()

bool Queue::isEmpty()
{
    return List::isEmpty();
} // Queue::isEmpty()

void Queue:: concatenate(Queue &other)
{
    List::concatenate(other);
} // Queue::concatenate(&other)

int Queue::stored()
{
    return getSize();
} // Queue::stored

