#include <iostream>
#include <windows.h>
#include <time.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

HANDLE hMutex = NULL; // mutex
HANDLE hMemory = NULL, hNames = NULL; // sharememory
HANDLE allow_consume = NULL, allow_produce = NULL; // semaphors
const int BUFFER_SIZE = 12;
int ptr[BUFFER_SIZE] = {0};
int (*pointer)[BUFFER_SIZE];

int main(int argc, char *argv[])
{   
    int consumerID = atoi(argv[3]);
    char *nameOfConsumer = (char *)malloc((strlen(argv[2])+1)*sizeof(char));
    strcpy(nameOfConsumer,argv[2]);
    int n = atoi(argv[1]);
    volatile int indice_to_consume = 0;
    int delay = rand()%3+n*100;  
    int i = 0;
    
    allow_consume = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"Global\\allowConsume");
    
	if (!allow_consume)
	{
	    return 0;
	}
	    
	allow_produce = OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,"Global\\allowProduce");
	    
	if (!allow_consume)
	{
	    return 0;
	}
	    
	hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,TEXT("Global\\mainMutex"));
	    
	if (!hMutex)
	{
	    return 0;
	}
	    
	hMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,"shareMemory");
	    
	if (!hMemory)
	{
	      return 0;
	}
    
    while(true)
    {
             // delay 
             //Sleep((rand()%10)*delay + delay);
             for (int k=0; k < 5000000; k++) pow((2.2/delay),pow(n,n));
             WaitForSingleObject(allow_consume,INFINITE);
             pointer = (int (*)[BUFFER_SIZE])MapViewOfFile(hMemory,FILE_MAP_ALL_ACCESS, 0,0,sizeof(ptr));
                 WaitForSingleObject(hMutex,INFINITE);
                     system("cls");
                         cout << "Here I am " << i << endl;
                         i++;
                     while ((*pointer)[indice_to_consume]==0)
                     {
                           indice_to_consume +=1;
                           if (indice_to_consume > 11)
                           {
                              indice_to_consume = 0;
                           }
                     }
                     if ((*pointer)[indice_to_consume]==1)
                     {
                        cout << nameOfConsumer << " consumed a product of \nProducer1 at index: " << indice_to_consume+1 << endl;
                     } else {
                        cout << nameOfConsumer << " consumed a product of \nProducer2 at index: " << indice_to_consume+1 << endl;
                     }
                     (*pointer)[indice_to_consume] = 0;
                     UnmapViewOfFile(hMemory);
                 ReleaseMutex(hMutex);
             ReleaseSemaphore(allow_produce,1,NULL);            
    }
    
    CloseHandle(hMutex);
    CloseHandle(hMemory);
    CloseHandle(allow_consume);
    CloseHandle(allow_produce);
    free(nameOfConsumer);
    
    getchar();
    return 0;
}
