#include "myList.h"

void myList::push(Analysis x)
{
    std::list<Analysis>::iterator it = this->begin();
    while(it->unusedProcs > x.unusedProcs && it != end()) it++;
    this->insert(it, x);
    if(this->size() > 5)
        this->pop_back();
}

void myList::print()
{
    for(std::list<Analysis>::iterator it = begin(); it != end(); it++)
        it->print();

    std::cout << "\nFailure: " << Analysis::unsucceed << "\tSuccess: " << Analysis::succeed;
}
