#include <iostream>
#include <list>
#include <fstream>
#include "procList.h"
#include <ctime>

//Maksymalny czas trwania programu w milisekundach
#define TIME 180000 //3 minuty

using namespace std;

int main(int argc, char* argv[])
{
    //Sprawdzenie czy podane zostaly wszystkie niezbedne informacje
    if(argc<=1) { cout << "PARALLEL TASKS SCHEDULING\nAby uruchomic podaj na wejsciu:\n[plik zrodlowy] [ilosc linii do wczytania|opcjonalne]\n"; return 0; }
    //vector zawierajacy wszystkie procesory dostepne w danej chwili
    vector<int> processors;
    //zmienne potrzebne do pomiaru czasowego
    clock_t s,e;
    //rozpoczecie pomiaru czasowego
    s = clock();
    /*
    Listy zawierajace procesy, ktore zostaja wczytane z pliku
    lista processList zawiera wszystkie wczytane procesy
    listy readyList i execList zawieraja procesy gotowe(tj. takie ktorych czas pojawienia sie <= timer) oraz wykonywane w danej chwili
    lista tmp zawiera kopie listy procesy aby mozliwe bylo utworzenie rozwiazania na wypadek przekroczenia limitu czasowego
    */
    list<Process> processList = readProcList(argc, argv);
    list<Process> readyList, execList;
    list<Process> tmp = processList;

    list<Analysis> analysis;

    list<long> finishTimes;

    //zmienna kontrolujaca czas
    long timer = 0;
    //zmienna przechowujaca maksymalna ilosc procesorow
    long counter = Process::maxProcs;
    //Na vector procesorow trafiaja odpowiednie wartosci, tj. procesory od 1 do counter
    for(long i=counter; i >= 1; i--) processors.push_back(i);
    //Otworzony zostaje plik o nazwie, ktora sklada sie z oryginalnej nazwy pliku oraz koncowki _s.txt
    ofstream output;
    string filename = argv[1];
    filename = filename.substr(0,filename.find("."));
    filename+="_s.txt";
    output.open(filename.c_str());


    /*
    Glowna petla programu, wykonywana tak dlugo az wszystkie listy sa puste.
    */
    while(!processList.empty() || !readyList.empty() || !execList.empty())
    {
        //cout << "Procesy w procesy:" << procesy.size() << "\nProcesy w ready " << ready.size() << "\nProcesy w exec " << exec.size() << "\nProcesy w finishTimes " << finishTimes.size() << "\nT: " << timer << endl << endl;
        //Sprawdzenie czy przekroczony zostal limit czasowy i wygenerowanie poprawnego wyniku gdyby przekroczono czas
        if((float)(clock()-s)/CLOCKS_PER_SEC * 1000 >= TIME)
        {
            cout << "Osiagniety limit czasowy!\n";
            output.close();
            output.open(filename.c_str(),std::ofstream::trunc);
            timer = 0;
            for(list<Process>::iterator it = tmp.begin(); it != tmp.end() && !tmp.empty();it++)
            {
                output << it->id << " " << timer << " " << timer+it->exec << " ";
                timer+=it->exec;
                for(int i=1;i<=it->nproc;i++) output << i << " ";
                output << "\n";
            }
            tmp.clear();
            break;
        }

        //Program przesuwa sie o pewna jednostke czasu, wynikajaca z dwoch list
        //Listy procesow wchodzacych oraz listy czasow zakonczenia aktualnie dzialajacych zadan
        //Program wybiera z obu list ta, na ktorej znajduje sie najblizsze zadanie i przeskakuje do niego
        if(!processList.empty() && !finishTimes.empty())
        {
            if(processList.front().ready < finishTimes.front())
            {
                timer = processList.front().ready;
                updateReady(timer,&readyList,&processList);
                readyList.sort(sortReady);
                runProc(timer,&finishTimes,&execList,&readyList,&counter,&processors, analysis);
            }
            else
            {
                timer = finishTimes.front();
                while(finishTimes.front() == timer) finishTimes.pop_front();
                terminateProc(timer,&execList,&counter,&output,&processors);
                runProc(timer,&finishTimes,&execList,&readyList,&counter,&processors, analysis);
            }
        }
        if(!processList.empty() && finishTimes.empty())
        {
            timer = processList.front().ready;
            updateReady(timer,&readyList,&processList);
            readyList.sort(sortReady);
            runProc(timer,&finishTimes,&execList,&readyList,&counter,&processors, analysis);
        }
        if(processList.empty() && !finishTimes.empty())
        {
            timer = finishTimes.front();
            while(finishTimes.front() == timer) finishTimes.pop_front();
            terminateProc(timer,&execList,&counter,&output,&processors);
            runProc(timer,&finishTimes,&execList,&readyList,&counter,&processors, analysis);
        }
    }
    cout << "Czas uszeregowania na podstawie p_j: " << timer << endl;
    //Wypisanie wiadomosci koncowych oraz wyczyszczenie tablic
    cout << "Plik wynikowy " << filename << " zostal pomyslnie zapisany.\n";
    output.close();
    e = clock();
    processors.clear();
    tmp.clear();
    cout << "Czas wykonania programu: " << (float)(e-s)/CLOCKS_PER_SEC << "s" << endl;
    printAnalysis(analysis);
    return 0;
}
