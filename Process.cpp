#include "Process.h"

long Process::maxJobs = 0;
long Process::maxProcs = 0;

Process::Process(int id, int ready, int exec, int nproc)
{
    this->id = id;
    this->ready = ready;
    this->exec = exec;
    this->nproc = nproc;
}
