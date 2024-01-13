#ifndef STACK_H
#define STACK_H

#include "LinkedList.h"

template <class type>
class Stack
{
    public:
        Stack()
        {
            size = 0;
        }

        void push(type data)
        {
            list.push_back(data);
            size++;
        } 

        type pop()
        {
            type temp = (type)list.back();
            list.pop_back();
            size--;
            return temp;
        }

        type peek()
        {
            return list.back();
        }

        bool isEmpty()
        {
            return size == 0;
        }

    private:
        LinkedList<type> list;
        int size;
};

#endif