#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>

struct IntPair{
    int first;
    int second;
};

template<class type>
struct LinkedListNode
{
    type data;
    LinkedListNode *next;

    LinkedListNode()
    {
        data = 0;
        next = nullptr;
    }

    LinkedListNode(type data)
    {
        this->data = data;
        this->next = nullptr;
    }

    LinkedListNode(const LinkedListNode &other)
    {
        data = other.data;
        next = nullptr;
    }

    LinkedListNode &operator=(const LinkedListNode &other)
    {
        data = other.data;
        next = nullptr;
        return *this;
    }
};

template <class type>
class LinkedList
{
    public:
        typedef LinkedListNode<type> Node;

        LinkedList()
        {
            head = nullptr;
            tail = nullptr;
        }

        LinkedList(const LinkedList<type> &other)
        {
            head = nullptr;
            tail = nullptr;
            for (Node *temp = other.head; temp != nullptr; temp = temp->next)
            {
                push_back(temp->data);
            }
        }

        LinkedList<type> &operator=(const LinkedList<type> &other)
        {
            for (Node *temp = head; temp != nullptr; temp = head)
            {
                head = head->next;
                delete temp;
            }
            head = nullptr;
            tail = nullptr;
            for (Node *temp = other.head; temp != nullptr; temp = temp->next)
            {
                push_back(temp->data);
            }
            return *this;
        }

        ~LinkedList()
        {
            for (Node *temp = head; temp != nullptr; temp = head)
            {
                head = head->next;
                delete temp;
            }
        }

        void push_back(type data)
        {
            Node *temp = new Node(data);
            if (head == nullptr)
            {
                head = tail = temp;
            }
            else
            {
                tail->next = temp;
                tail = temp;
            }
        }

        void push_front(type data){
            Node *temp = new Node(data);
            if (head == nullptr)
            {
                head = tail = temp;
            }
            else
            {
                temp->next = head;
                head = temp;
            }
        }

        void pop_back()
        {
            if (head == nullptr)
            {
                return;
            }
            else if (head == tail)
            {
                delete head;
                head = tail = nullptr;
            }
            else
            {
                Node *temp = head;
                while (temp->next != tail)
                {
                    temp = temp->next;
                }
                delete tail;
                tail = temp;
                tail->next = nullptr;
            }
        }
        
        void pop_front(){
            if(head == nullptr){
                return;
            }
            else if(head == tail){
                delete head;
                head = tail = nullptr;
            }
            else{
                Node* temp = head;
                head = head->next;
                delete temp;
            }
        }

        type front()
        {
            return head->data;
        }

        type back()
        {
            if(tail == nullptr) return (type)0;
            return tail->data;
        }

        bool empty()
        {
            return head == nullptr;
        }

    private:
        Node *head;
        Node *tail;
};

#endif