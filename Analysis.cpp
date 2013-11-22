#include "Analysis.h"
#include <cstdio>
#include <list>

using namespace std;

int Analysis::succeed = 0;
int Analysis::unsucceed = 0;

Analysis::Analysis(long t, int u, std::list<Process> r)
{
    time = t;
    unusedProcs = u;
    readyTasks = r;

}
void Analysis::print()
{
    printf("Time: %d\tUnused processors: %d\t Ready tasks: ", time, unusedProcs);
    for(list<Process>::iterator it = readyTasks.begin(); it != readyTasks.end(); it++)
        printf("%d:%d ", it->id, it->nproc);
    printf("\n");
}

bool Analysis::operator<(Analysis b)
{
    return (unusedProcs>b.unusedProcs);
}
