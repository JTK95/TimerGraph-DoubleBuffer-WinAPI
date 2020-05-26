#pragma once

#define QUEUE_SIZE 100

#include "pch.h"

template <typename T>
class CQueue
{
public:
    //------------------------------
    // Queue Init
    //------------------------------
    CQueue()
        :_iFront(0), _iRear(0)
    {}

    //------------------------------
    // EnQueue
    //------------------------------
    BOOL EnQueue(T iKey)
    {
        // 원형 큐 Full
        if ((this->_iRear + 1) % QUEUE_SIZE == (this->_iFront))
        {
            puts("Queue is Full");
            return FALSE;
        }

        this->_iRear = (this->_iRear + 1) % QUEUE_SIZE;
        this->_queArr[this->_iRear] = iKey;

        return TRUE;
    }

    //-------------------------------
    // DeQueue
    //-------------------------------
    BOOL DeQueue()//(T* pKey)
    {
        // 원형 큐 Empty
        if (this->_iFront == this->_iRear)
        {
            puts("Queue is Empty");
            return FALSE;
        }

        this->_iFront = (this->_iFront + 1) % QUEUE_SIZE;
        //  *pKey = this->_queArr[this->_iFront];

        return TRUE;
    }

    //T Peek()
    //{
    //    // 원형 큐 Empty
    //    if (this->_iFront == this->_iRear)
    //    {
    //        puts("Queue is Empty");
    //        return;
    //    }

    //    return this->_queArr[(this->_iFront + 1) % QUEUE_SIZE];
    //}

    void InitTemp()
    {
        this->_iTemp = this->_iFront;
    }

    BOOL Peek(T* pData)
    {
        // 원형 큐 Empty
        if (this->_iTemp == this->_iRear)
        {
            puts("Queue is Empty");
            return FALSE;
        }

        this->_iTemp = (this->_iTemp + 1) % QUEUE_SIZE;
        *pData = this->_queArr[this->_iTemp];

        return TRUE;
    }

    T GetFrontValue()
    {
        return this->_queArr[this->_iFront];
    }

private:
    int _iRear;
    int _iFront;
    int _iTemp;
    int _queArr[QUEUE_SIZE];
};