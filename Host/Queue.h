#ifndef __QUEUE_H__
#define __QUEUE_H__

#include<cstdlib>
#include<cstring>
#include<algorithm>

template<typename T>
struct QueueNode
{
    T data;
    QueueNode *next;
};

template<typename T>
class Queue
{
protected:
    QueueNode<T> *head,*tail,*tmp;
    unsigned int cnt;
public:
    Queue():head(NULL),tail(NULL),tmp(NULL),cnt(0)
    {
    }

    bool push(T t)
    {
        tmp = new QueueNode<T>;
        if(tmp == NULL)
            return false;
        tmp -> data = t;
        tmp -> next = NULL;
        tail -> next = tmp;
        tail = tmp;
        ++cnt;
        return true;
    }

    void pop()
    {
        if(head ==  NULL)
        {
            return;
        }

        tmp = head -> next;
        delete head;
        head = tmp;
        --cnt;
    }

    T front()
    {
        if(head != NULL)
        {
            return head -> data;
        }
        else
        {
            return T(0);
        }
    }

    T back()
    {
        if(tail != NULL)
        {
            return tail -> data;
        }
        else
        {
            return T(0);
        }
    }

    unsigned int size() const
    {
        return cnt;
    }

    bool empty() const
    {
        return cnt == 0;
    }
};

template<typename T>
class CircleQueue
{
protected:
    unsigned int num,cnt,pos;
    T *dataPtr;
public:
    CircleQueue(unsigned int n):num(n),cnt(0),pos(0)
    {
        dataPtr = new T[n];
		memset(dataPtr, 0, sizeof(T) * n);
    }

    void push(T t)
    {
        *(dataPtr + pos) = t;
        pos = (pos + 1) % num;
        ++cnt;
    }

    T front()
    {
        return dataPtr[(pos - 1 + num) % num];
    }

    T back()
    {
        return dataPtr[pos];
    }
};

template<typename T>
class CircleQueue_Avg:public CircleQueue<T>
{
protected:
    T Avg;
public:
    CircleQueue_Avg(unsigned int n = 8):CircleQueue<T>(n),Avg(0)
    {
    }

    void push(T t)
    {
        Avg -= CircleQueue<T>::back();
        CircleQueue<T>::push(t);
        Avg += t;
    }

    T avg()
    {
        return Avg / (double)min(CircleQueue<T>::num,CircleQueue<T>::cnt);
    }
};

template<typename T>
class Queue_Avg:public Queue<T>
{
protected:
    T Avg;
public:
    Queue_Avg():Queue<T>(),Avg(0)
    {
    }

    bool push(T t)
    {
        Avg += t;
        return Queue<T>::push(t);
    }

    void pop()
    {
        Avg -= Queue<T>::back();
        Queue<T>::pop();
    }

    T avg()
    {
        return Avg / Queue<T>::cnt;
    }
};

#endif // __QUEUE_H__
