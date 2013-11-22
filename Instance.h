#ifndef INSTANCE_H_INCLUDED
#define INSTANCE_H_INCLUDED

#include <vector>
#include <list>
#include <fstream>
#include <ctime>
#include <iostream>
#define TIME 180000

#include "procList.h"
#include "Analysis.h"
#include "myList.h"
#include "Process.h"

using namespace std;

class Instance
{
public:
    vector<int> processors;
    list<Process> processList, readyList, execList, tmp;
    myList analysis;
    list<long> finishTimes;
    std::ofstream *output;
    int counter;
    long timer;
    clock_t startTime, endTime;

    Instance(list<Process> processList, std::ofstream *f);

    void startScheduler();

    //Funckja wypisujaca procesy na ekran konsoli
    void printProcList(list<Process> p);
    //Funkcja przenoszaca procesy z listy "Procesy" na "ready"
    void updateReady();
    //Wlasna funkcja sortujaca
    static bool sortReady(Process a, Process b);
    //Funkcja usuwajaca wykonane procesy z programu
    void terminateProc();
    //Funkcja przydzielajaca procesory oraz przenoszaca proces z "ready" na "exec"
    void runProc();




};

#endif // INSTANCE_H_INCLUDED
