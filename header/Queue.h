#ifndef QUEUE_H
#define QUEUE_H

#include "LinkedList.h"

template<class type>
class Queue{
    public:
        Queue(){
            _size = 0;
        }

        Queue(const Queue<type> &other){
            _queue = other._queue;
            _size = other._size;
        }

        Queue<type> &operator=(const Queue<type> &other){
            _queue = other._queue;
            _size = other._size;
            return *this;
        }

        void push(type data){
            _queue.push_back(data);
            _size++;
        }

        void push_front(type data){
            _queue.push_front(data);
            _size++;
        }

        type pop(){
            type data = _queue.front();
            _queue.pop_front();
            _size--;
            return data;
        }

        type front(){
            return _queue.front();
        }

        type back(){
            return _queue.back();
        }

        bool empty(){
            return _queue.empty();
        }

        int size(){
            return _size;
        }

    private:
        LinkedList<type> _queue;
        int _size;
};

#endif // QUEUE_H