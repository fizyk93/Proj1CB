#ifndef PROCLIST_H_INCLUDED
#define PROCLIST_H_INCLUDED
#include <vector>
#include <list>
#include <cstdio>

//Struktura procesu, przechowuje wazne informacje nt. procesu wczytywanego z pliku
struct Process
{
    /*
    id - id procesu; ready - czas pojawienia sie w systemie; exec - czas potrzebny na wykonanie; nproc - potrzebne procesory
    start - czas rozpoczecia; finish - moment zwolnienia procesora, ustalana dopiero podczas wykonywania
    */
    long id, ready, exec, nproc, start, finish;
    std::vector<long> processors;
    static long maxJobs, maxProcs;
};

class Analysis
{
public:

	long time;
	int unusedProcs;
	std::list<Process> readyTasks;
	static int succeed, unsucceed;

    Analysis(long t, int u, std::list<Process> r)
	{
		time = t;
		unusedProcs = u;
		readyTasks = r;

	}
	void print();

	bool operator<(Analysis b)
	{
	    return (unusedProcs>b.unusedProcs);
	}
};

class myList :
    public std::list<Analysis>
{
    public:

    void push(Analysis x)
    {
        std::list<Analysis>::iterator it = this->begin();
        while(it->unusedProcs > x.unusedProcs && it != end()) it++;
        this->insert(it, x);
        if(this->size() > 5)
            this->pop_back();
    }

    void print()
    {
        for(std::list<Analysis>::iterator it = begin(); it != end(); it++)
            it->print();

        std::cout << "\nFailure: " << Analysis::unsucceed << "\tSuccess: " << Analysis::succeed;
    }
};

bool mysort(Process a, Process b);

//Funckja czytajaca procesy z pliku
std::list<Process> readProcList(int argc, char** argv);
//Funckja wypisujaca procesy na ekran konsoli
void printProcList(std::list<Process> p);
//Funkcja przenoszaca procesy z listy "Procesy" na "ready"
void updateReady(long time, std::list<Process>* r, std::list<Process>* all);
//Wlasna funkcja sortujaca
bool sortReady(Process a, Process b);
//Funkcja usuwajaca wykonane procesy z programu
void terminateProc(long time, std::list<Process> *x, long *counter, std::ofstream *file,std::vector<int> *proc);
//Funkcja przydzielajaca procesory oraz przenoszaca proces z "ready" na "exec"
void runProc(long time, std::list<long> *finishTimes, std::list<Process> *x, std::list<Process> *r, long *counter,std::vector<int> *proc, std::list<Analysis>& an);

void printAnalysis(std::list<Analysis> a);

#endif // PROCLIST_HPP_INCLUDED
