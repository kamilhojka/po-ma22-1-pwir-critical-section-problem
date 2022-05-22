// pwir-critical-section-problem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <locale.h>
#include <windows.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <omp.h>

using namespace std;

mutex mtx;

void ShowIntroInformation(HANDLE hConsole);
void SetIterationValue(HANDLE hConsole, int& iteration);
void SetDelayValue(HANDLE hConsole, int& delay);
void SetRepeatValue(HANDLE hConsole, int& repeat);
void RunFunctionSequently(HANDLE hConsole, int iteration, int delay);
void RunFunctionParallel(HANDLE hConsole, int iteration, int delay);
void RunFunctionParallelMutex(HANDLE hConsole, int iteration, int delay);
void RunFunctionParallelOpenMP(HANDLE hConsole, int iteration, int delay);
void RunFunctionParallelOpenMPCritical(HANDLE hConsole, int iteration, int delay);

int main()
{
    setlocale(LC_CTYPE, "Polish");
    int iteration, delay, repeat;
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    ShowIntroInformation(hConsole);
    while (true) {
        SetIterationValue(hConsole, iteration);
        SetDelayValue(hConsole, delay);
        RunFunctionSequently(hConsole, iteration, delay);
        RunFunctionParallel(hConsole, iteration, delay);
        RunFunctionParallelMutex(hConsole, iteration, delay);
        RunFunctionParallelOpenMP(hConsole, iteration, delay);
        RunFunctionParallelOpenMPCritical(hConsole, iteration, delay);

        SetRepeatValue(hConsole, repeat);
        if (repeat == 0) break;
    }
}

void ShowIntroInformation(HANDLE hConsole)
{
    SetConsoleTextAttribute(hConsole, 11);
    for (int i = 0; i < 70; i++) cout << '*';
    SetConsoleTextAttribute(hConsole, 3);
    cout << "\n\n  PROGRAMOWANIE WSPÓ£BIE¯NE I ROZPROSZONE 21/22L\n  Problem sekcji krytycznej\n  Autor programu: ";
    SetConsoleTextAttribute(hConsole, 15);
    cout << "Kamil Hojka -- 97632\n\n";
    SetConsoleTextAttribute(hConsole, 11);
    for (int i = 0; i < 70; i++) cout << '*';
    cout << "\n";
    SetConsoleTextAttribute(hConsole, 15);
}

void SetIterationValue(HANDLE hConsole, int& iteration)
{
    while (true) {
        SetConsoleTextAttribute(hConsole, 14);
        cout << "\n -> WprowadŸ liczbê iteracji dla zadania - sugerowana wartoœæ [40 - 60]: ";
        SetConsoleTextAttribute(hConsole, 15);
        cin >> iteration;
        if (!cin.good() || iteration <= 0)
        {
            SetConsoleTextAttribute(hConsole, 4);
            cout << "    ! Wartoœæ musi byæ liczb¹ wiêksz¹ od 0\n";
            SetConsoleTextAttribute(hConsole, 15);
            cin.clear();
            cin.ignore();
        }
        else break;
    }
}

void SetDelayValue(HANDLE hConsole, int& delay)
{
    while (true) {
        SetConsoleTextAttribute(hConsole, 14);
        cout << "\n -> WprowadŸ wartoœæ opóŸnienia - sugerowane [100 - 500]: ";
        SetConsoleTextAttribute(hConsole, 15);
        cin >> delay;
        if (!cin.good() || delay < 0)
        {
            SetConsoleTextAttribute(hConsole, 4);
            cout << "    ! Wartoœæ musi byæ liczb¹ wiêksz¹, b¹dŸ równ¹ 0\n";
            SetConsoleTextAttribute(hConsole, 15);
            cin.clear();
            cin.ignore();
        }
        else break;
    }
}


void SetRepeatValue(HANDLE hConsole, int& repeat)
{
    while (true) {
        SetConsoleTextAttribute(hConsole, 14);
        cout << "\n -> Czy powtórzyæ program? [1/0]: ";
        SetConsoleTextAttribute(hConsole, 15);
        cin >> repeat;
        if (!cin.good() || (repeat != 0 && repeat != 1))
        {
            SetConsoleTextAttribute(hConsole, 4);
            cout << "    ! Wartoœæ musi byæ liczb¹ 0 lub 1\n";
            SetConsoleTextAttribute(hConsole, 15);
            cin.clear();
            cin.ignore();
        }
        else break;
    }
}

void Func(char character, int iterations, int delay, WORD color)
{
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 0; i < iterations; i++)
    {
        SetConsoleTextAttribute(hConsole, color);
        cout << character << " ";
        this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

void RunFunctionSequently(HANDLE hConsole, int iteration, int delay)
{
    cout << "\n ===========> Zadania realizowane SEKWENCYJNIE - liczba zadañ 3:";
    cout << "\n KIERUNEK UP£YWU CZASU ----> " << endl;
    auto begin = chrono::high_resolution_clock::now();
    Func('A', iteration, delay, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    Func('B', iteration, delay, FOREGROUND_RED | FOREGROUND_INTENSITY);
    Func('C', iteration, delay, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl << "\n Zmierzony czas: " << elapsed << " ms" << endl;
}

void RunFunctionParallel(HANDLE hConsole, int iteration, int delay)
{
    cout << "\n ===========> Zadania realizowane RÓWNOLEGLE - liczba zadañ 3:";
    cout << "\n KIERUNEK UP£YWU CZASU ----> " << endl;
    auto begin = chrono::high_resolution_clock::now();
    std::thread first(Func, 'A', iteration, delay, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::thread second(Func, 'B', iteration, delay, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::thread third(Func, 'C', iteration, delay, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    first.join();
    second.join();
    third.join();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl << "\n Zmierzony czas: " << elapsed << " ms" << endl;
}

void FuncMutex(char character, int iterations, int delay, WORD color)
{
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 0; i < iterations; i++)
    {
        mtx.lock();
        SetConsoleTextAttribute(hConsole, color);
        cout << character << " ";
        mtx.unlock();
        this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

void RunFunctionParallelMutex(HANDLE hConsole, int iteration, int delay)
{
    cout << "\n ===========> Zadania realizowane RÓWNOLEGLE (MUTEX) - liczba zadañ 3:";
    cout << "\n KIERUNEK UP£YWU CZASU ----> " << endl;
    auto begin = chrono::high_resolution_clock::now();
    std::thread first(FuncMutex, 'A', iteration, delay, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::thread second(FuncMutex, 'B', iteration, delay, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::thread third(FuncMutex, 'C', iteration, delay, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    first.join();
    second.join();
    third.join();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl << "\n Zmierzony czas: " << elapsed << " ms" << endl;
}

void RunFunctionParallelOpenMP(HANDLE hConsole, int iteration, int delay)
{
    cout << "\n ===========> Zadania realizowane RÓWNOLEGLE (OpenMP) - liczba zadañ 3:";
    cout << "\n KIERUNEK UP£YWU CZASU ----> " << endl;
    auto begin = chrono::high_resolution_clock::now();
#pragma omp parallel sections
    {
#pragma omp section
        Func('A', iteration, delay, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#pragma omp section
        Func('B', iteration, delay, FOREGROUND_RED | FOREGROUND_INTENSITY);
#pragma omp section
        Func('C', iteration, delay, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl << "\n Zmierzony czas: " << elapsed << " ms" << endl;
}

void FuncOpenMPCritical(char character, int iterations, int delay, WORD color)
{
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 0; i < iterations; i++)
    {
#pragma omp critical
        {
            SetConsoleTextAttribute(hConsole, color);
            cout << character << " ";
        }
        this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

void RunFunctionParallelOpenMPCritical(HANDLE hConsole, int iteration, int delay)
{
    cout << "\n ===========> Zadania realizowane RÓWNOLEGLE (OpenMP Critical) - liczba zadañ 3:";
    cout << "\n KIERUNEK UP£YWU CZASU ----> " << endl;
    auto begin = chrono::high_resolution_clock::now();
#pragma omp parallel sections
    {
#pragma omp section
        FuncOpenMPCritical('A', iteration, delay, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#pragma omp section
        FuncOpenMPCritical('B', iteration, delay, FOREGROUND_RED | FOREGROUND_INTENSITY);
#pragma omp section
        FuncOpenMPCritical('C', iteration, delay, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl << "\n Zmierzony czas: " << elapsed << " ms" << endl;
}