#ifndef MYLIST_H
#define MYLIST_H
#include "Analysis.h"


class myList :
    public std::list<Analysis>
{
    public:

    void push(Analysis x);

    void print();
};
#endif // MYLIST_H
