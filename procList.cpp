#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <list>
#include "procList.h"
using namespace std;



bool mysort(Process a, Process b)
{
    return (a.ready < b.ready);
}

//Zmienne globalnie dostepne ze struktury Processan.push_back(Analysis(time, *counter, *r));
long Process::maxJobs = 0;
long Process::maxProcs = 0;

int Analysis::succeed = 0;
int Analysis::unsucceed = 0;

//Funkcja czytajaca podana ilosc procesow z pliku w formacje standard workload
std::list<Process> readProcList(int argc, char** argv)
{
    //Licznik blednych wpisow oraz zmienne potrzebne do wczytywania procesow
    int blad = 0;
    ifstream input;
    string line = "";
    long lines;

    list<Process> processList;


    input.open(argv[1]);
    if(!input.good()) cout << "\nUWAGA: Plik zrodlowy nie istnieje!\n\n";
    //Jezeli podana zostala odpowiednia lista parametrow
    if(argc > 2)
    {
        //ustawiona jest ilosc linii do wczytania
        lines = atoi(argv[2]);
        if(lines==0) lines=-1;
    }
    else lines = -1;
    /*Tak dlugo jak mozliwe jest wczytanie linii z pliku, linia zostaje wczytana, a nastepnie wyodrebnione
    zostaja potrzebne informacje takie jak MaxJobs czy MaxProcs
    */
    while(getline(input,line) && lines!=0)
    {
        if(line[0] == ';') /// jezeli wczytana linia zaczyna sie od ;
        {
            if(line.find("MaxJobs:")!=string::npos) /// i jest w niej taki wpis
            {
                stringstream data(line);
                string tmp;
                //Zamiana znaku : na spacje w celu zagwarantowania poprawnosci wczytywania danych
                int pos = line.find(":");
                line[pos] = ' ';
                while(data>>tmp) {};
                Process::maxJobs = atoi(tmp.c_str());
                if(lines==-1 || lines>Process::maxJobs) lines = Process::maxJobs;
            }
            if(line.find("MaxProcs:")!=string::npos) // albo taki
            {
                stringstream data(line);
                string tmp;
                int pos = line.find(":");
                line[pos] = ' ';
                while(data>>tmp) {};
                Process::maxProcs = atoi(tmp.c_str());
            }
        }
        else
        {
            ///linie bez ;
            /*
            Job Number -- a counter field, starting from 1.
            Submit Time -- in seconds. The earliest time the log refers to is zero, and is usually the submittal time of the first job. The lines in the log are sorted by ascending submittal times. It makes sense for jobs to also be numbered in this order.
            Wait Time -- malo przydatne
            Run Time -- in seconds. The wall clock time the job was running (end time minus start time).
            Number of Allocated Processors -- an integer. In most cases this is also the number of processors the job uses; if the job does not use all of them, we typically don't know about it.
            Reszta zbedna
            */
            stringstream stringInput(line); /// nowy strumien, dzieki niemu rodzielony zostaje wczytany string
            string tmp; //zmienna tymczasowa, dane z tej zmiennej trafiaja do struktury Proces
            int i = 0;

            vector<int> temp;
            while (stringInput >> tmp && i < 5) //poki cos jest i to cos nalezy do 1 z 5 informacji wypisanych w komentarzu wyzej, to czytamy
            {
                i++;
                temp.push_back(atoi(tmp.c_str()));
            }
            //Sprawdzanie poprawnosci
            if(temp[0]>=1 && temp[1]>=0 && temp[3]>0 && temp[4]>0)
            {
                //Jezeli proces jest poprawny to go ladujemy na liste procesy
                Process p;
                p.id = temp[0];
                p.ready = temp[1];
                p.exec = temp[3];
                p.nproc = temp[4];
                processList.push_back(p);
            }
            else
            {
                //W innym wypadku pomijamy taka linie i zwiekszamy zmienna blad o 1
                blad++;
            }
            lines--; ///limitowanie ilosci linii do wczytania, wg. zalozen zadania, -1 to wszystkie wpisy
        }
    }
    //Wypisanie informacji nt. pliku wczytywanego
    cout << "Max Jobs:" << Process::maxJobs << "\nMax Procs:" << Process::maxProcs << endl; ///wazne dane
    cout << "Pominiete bledne wpisy: " << blad << endl << endl;
    input.close();

    // funkcja sortuje listę wg. czasu gotowosci procesu
    processList.sort(mysort);

    return processList;
}

//Funkcja wypisuje wszystkie procesy na konsole
void printProcList(list<Process> p)
{
    for(list<Process>::iterator it = p.begin(); it != p.end() && !p.empty(); it++)
        cout << "\n<<Proces>>\nPole 1: " << it->id << "\nPole 2: " << it->ready << "\nPole 4: " << it->exec << "\nPole 5: " << it->nproc  << "\n\n";

}

//Funkcja przenoszaca procesy z listy "procesy" na liste procesow gotowych do wykonania jezeli dane procesy maja czas wejscia <= obecny czas
void updateReady(long timer, list<Process>* r, list<Process>* all)
{
    for(list<Process>::iterator it = all->begin(); it != all->end() && it->ready <= timer; )
    {
        (*r).push_back((*all).front());
        it++;
        (*all).pop_front();
    }
    (*r).sort(sortReady);

}
//Wlasna funkcja sortowania potrzebna do dzialania programu
bool sortReady(Process a, Process b)
{
    return (a.exec > b.exec);
}
/*Funkcja wyszukujaca wykonane procesy i usuwajaca je z listy exec.
Funkcja jednoczesnie zapisuje takie procesy do pliku wynikowego
Zwolnione procesory trafiaja ponownie na wektor dostepnych procesorow
*/

void terminateProc(long time, list<Process> *x, long *counter,ofstream *file,vector<int> *proc)
{
    for(list<Process>::iterator it = x->begin(); it != x->end() && !x->empty();)
        if(it->finish == time)
        {
            (*file) << it->id << " " << it->start << " " << it->finish << " "; //zapis danych procesu do pliku
            for(long i=0;i<(long)it->processors.size();i++)
            {
                (*file) << it->processors[i] << " "; //zapis procesorow wykorzystywanych przez zadanie do pliku
                (*proc).push_back(it->processors[i]); //zwracanie procesorow do listy wolnych procesorow
            }
            (*file) << "\n";
            *counter += it->nproc; //zmienna zawierajaca aktualna ilosc procesorow jest zwiekszana o zwrocone procesory
            it=x->erase(it);//usuwamy zadanie z listy wykonywanych

        }
        else it++;
}

//Funkcja przydzielajaca procesory procesom oraz przenoszaca je z listy "ready" na liste "exec"
void runProc(long time, list<long> *finishTimes, list<Process> *x, list<Process> *r, long *counter,vector<int> *proc, myList& an)
{
    //list<Process> tmp = *r;
    for(list<Process>::iterator it = r->begin(); it != r->end() && counter != 0 && !(r->empty());)
    {
        if(it->nproc <= *counter) //jezeli zadanie
        {
            it->start = time; //faktyczny czas rozpoczecia zadania
            it->finish = time + it->exec; //faktyczny czas zakonczenia zadania
            long i = it->nproc; //dodawanie procesorow do zadania
           // cout << it->id << " " << i << endl; system("PAUSE");
            while(i>0)
            {
                //if((*proc).empty()) { cout << "BLAD"; system("PAUSE"); }

                i--;
                it->processors.push_back((*proc)[(*proc).size()-1]); //procesory z proc sa przenoszone do wektora procesorow uzywanych przez zadanie
                (*proc).pop_back();
            }
            //if(it->processors.size() != it->nproc) { cout << "BLAD " << it->id; system("PAUSE"); }
            x->push_back(*it); //zadanie trafia na liste procesow wykonywanych
            (*finishTimes).push_back(it->finish); //czas zakonczenia zadania trafia na liste zawierajaca czasy zakonczenia dzialajacych zadan
            *counter -= it->nproc;
            it=r->erase(it);

        }
        else
        {
            it++;
            if(it == r->end())
                if(r->empty() || *counter == 0) Analysis::succeed++;
                else
                {
                    an.push(Analysis(time, *counter, *r));
                    Analysis::unsucceed++;
                }


        }
    }
    (*finishTimes).sort(); //lista czasow w ktorych koncza sie zadania jest sortowana
}

void Analysis::print()
{
    printf("Time: %d\tUnused processors: %d\t Ready tasks: ", time, unusedProcs);
    for(list<Process>::iterator it = readyTasks.begin(); it != readyTasks.end(); it++)
        printf("%d:%d ", it->id, it->nproc);
    printf("\n");
}



void printAnalysis(list<Analysis> a)
{
    a.sort();
    for(list<Analysis>::iterator it = a.begin(); it != a.end(); it++)
        it->print();

    cout << "\nFailure: " << Analysis::unsucceed << "\tSuccess: " << Analysis::succeed;
}

