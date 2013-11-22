#ifndef PROCESS_H
#define PROCESS_H
#include <vector>


class Process
{
public:
    long id, ready, exec, nproc, start, finish;
    std::vector<long> processors;
    static long maxJobs, maxProcs;

    Process(int id, int ready, int exec, int nproc);

    bool operator< (Process& b)
    {
        return (this->ready < b.ready);
    }
};

#endif // PROCESS_H
