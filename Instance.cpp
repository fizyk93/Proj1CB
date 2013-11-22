#include "Instance.h"

Instance::Instance(list<Process> processList, std::ofstream *f)
{
    this->processList = processList;
    output = f;
    counter = Process::maxProcs;
    for(long i=counter; i >= 1; i--) processors.push_back(i);
    timer = 0;
}

void Instance::startScheduler()
{
    startTime = clock();
    while(!processList.empty() || !readyList.empty() || !execList.empty())
    {
        //cout << "Procesy w procesy:" << procesy.size() << "\nProcesy w ready " << ready.size() << "\nProcesy w exec " << exec.size() << "\nProcesy w finishTimes " << finishTimes.size() << "\nT: " << timer << endl << endl;
        //Sprawdzenie czy przekroczony zostal limit czasowy i wygenerowanie poprawnego wyniku gdyby przekroczono czas
//        if((float)(clock()-startTime)/CLOCKS_PER_SEC * 1000 >= TIME)
//        {
//            cout << "Osiagniety limit czasowy!\n";
//            output->close();
//            output->open(filename.c_str(),std::ofstream::trunc);
//            timer = 0;
//            for(list<Process>::iterator it = tmp.begin(); it != tmp.end() && !tmp.empty();it++)
//            {
//                (*output) << it->id << " " << timer << " " << timer+it->exec << " ";
//                timer+=it->exec;
//                for(int i=1;i<=it->nproc;i++) output << i << " ";
//                output << "\n";
//            }
//            tmp.clear();
//            break;
//        }

        //Program przesuwa sie o pewna jednostke czasu, wynikajaca z dwoch list
        //Listy procesow wchodzacych oraz listy czasow zakonczenia aktualnie dzialajacych zadan
        //Program wybiera z obu list ta, na ktorej znajduje sie najblizsze zadanie i przeskakuje do niego
        if(!processList.empty() && !finishTimes.empty())
        {
            if(processList.front().ready < finishTimes.front())
            {
                timer = processList.front().ready;
                updateReady();
                readyList.sort(sortReady);
                runProc();
            }
            else
            {
                timer = finishTimes.front();
                while(finishTimes.front() == timer) finishTimes.pop_front();
                terminateProc();
                runProc();
            }
        }
        if(!processList.empty() && finishTimes.empty())
        {
            timer = processList.front().ready;
            updateReady();
            readyList.sort(sortReady);
            runProc();
        }
        if(processList.empty() && !finishTimes.empty())
        {
            timer = finishTimes.front();
            while(finishTimes.front() == timer) finishTimes.pop_front();
            terminateProc();
            runProc();
        }
    }

    endTime = clock();

}
//Funkcja wypisuje wszystkie procesy na konsole
void Instance::printProcList(list<Process> p)
{
    for(list<Process>::iterator it = p.begin(); it != p.end() && !p.empty(); it++)
        cout << "\n<<Proces>>\nPole 1: " << it->id << "\nPole 2: " << it->ready << "\nPole 4: " << it->exec << "\nPole 5: " << it->nproc  << "\n\n";

}

//Funkcja przenoszaca procesy z listy "procesy" na liste procesow gotowych do wykonania jezeli dane procesy maja czas wejscia <= obecny czas
void Instance::updateReady()
{
    for(list<Process>::iterator it = processList.begin(); it != processList.end() && it->ready <= timer; )
    {
        readyList.push_back(processList.front());
        it++;
        processList.pop_front();
    }
    readyList.sort(sortReady);

}
//Wlasna funkcja sortowania potrzebna do dzialania programu
bool Instance::sortReady(Process a, Process b)
{
    return (a.exec > b.exec);
}
/*Funkcja wyszukujaca wykonane procesy i usuwajaca je z listy exec.
Funkcja jednoczesnie zapisuje takie procesy do pliku wynikowego
Zwolnione procesory trafiaja ponownie na wektor dostepnych procesorow
*/

void Instance::terminateProc()
{
    for(list<Process>::iterator it = execList.begin(); it != execList.end() && !execList.empty();)
        if(it->finish == timer)
        {
            (*output) << it->id << " " << it->start << " " << it->finish << " "; //zapis danych procesu do pliku
            for(long i=0;i<(long)it->processors.size();i++)
            {
                (*output) << it->processors[i] << " "; //zapis procesorow wykorzystywanych przez zadanie do pliku
                processors.push_back(it->processors[i]); //zwracanie procesorow do listy wolnych procesorow
            }
            (*output) << "\n";
            counter += it->nproc; //zmienna zawierajaca aktualna ilosc procesorow jest zwiekszana o zwrocone procesory
            it=execList.erase(it);//usuwamy zadanie z listy wykonywanych

        }
        else it++;
}

//Funkcja przydzielajaca procesory procesom oraz przenoszaca je z listy "ready" na liste "exec"
void Instance::runProc()//long time, list<long> *finishTimes, list<Process> *x, list<Process> *r, long *counter,vector<int> *proc, myList& an)
{
    //list<Process> tmp = *r;
    for(list<Process>::iterator it = readyList.begin(); it != readyList.end() && counter != 0 && !(readyList.empty());)
    {
        if(it->nproc <= counter) //jezeli zadanie
        {
            it->start = timer; //faktyczny czas rozpoczecia zadania
            it->finish = timer + it->exec; //faktyczny czas zakonczenia zadania
            long i = it->nproc; //dodawanie procesorow do zadania
           // cout << it->id << " " << i << endl; system("PAUSE");
            while(i>0)
            {
                //if((*proc).empty()) { cout << "BLAD"; system("PAUSE"); }

                i--;
                it->processors.push_back(processors[processors.size()-1]); //procesory z proc sa przenoszone do wektora procesorow uzywanych przez zadanie
                processors.pop_back();
            }
            //if(it->processors.size() != it->nproc) { cout << "BLAD " << it->id; system("PAUSE"); }
            execList.push_back(*it); //zadanie trafia na liste procesow wykonywanych
            finishTimes.push_back(it->finish); //czas zakonczenia zadania trafia na liste zawierajaca czasy zakonczenia dzialajacych zadan
            counter -= it->nproc;
            it=readyList.erase(it);

        }
        else
        {
            it++;
            if(it == readyList.end())
                if(readyList.empty() || counter == 0) Analysis::succeed++;
                else
                {
                    analysis.push(Analysis(timer, counter, readyList));
                    Analysis::unsucceed++;
                }


        }
    }
    finishTimes.sort(); //lista czasow w ktorych koncza sie zadania jest sortowana
}
