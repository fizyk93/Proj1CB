#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <list>
#include "Process.h"
#include <iostream>


class Analysis
{
public:

	long time;
	int unusedProcs;
	std::list<Process> readyTasks;
	static int succeed, unsucceed;

    Analysis(long t, int u, std::list<Process> r);

	void print();

	bool operator<(Analysis b);
};

#endif // ANALYSIS_H
