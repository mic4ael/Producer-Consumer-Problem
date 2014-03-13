#include <iostream>
#include <windows.h>
#include <time.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>

using namespace std;

HANDLE hMutex = NULL; // mutex
HANDLE hMemory = NULL; // shared memory
HANDLE allow_consume = NULL, allow_produce = NULL; // semaphors
const int BUFFER_SIZE = 12;
int ptr[BUFFER_SIZE] = {0};
int (*pointer)[BUFFER_SIZE];

int main(int argc, char *argv[])
{      
    srand(time(NULL));
    int producerID = atoi(argv[2]);
    int n = atoi(argv[1]);
    volatile int indice_to_produce;
    indice_to_produce = rand()%11;
    int delay = rand()%3+n*100;
    
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
                 for (int k=0; k < 3000000; k++) pow((2.2/delay),pow(2,n));
                 WaitForSingleObject(allow_produce,INFINITE);
                 pointer = (int (*)[BUFFER_SIZE])MapViewOfFile(hMemory,FILE_MAP_ALL_ACCESS, 0,0,sizeof(ptr));
                     WaitForSingleObject(hMutex,INFINITE);
                         while ((*pointer)[indice_to_produce]!=0)
                         {
                               indice_to_produce +=1;
                               if (indice_to_produce > 11)
                               {
                                  indice_to_produce = 0;
                               }
                         }
                         (*pointer)[indice_to_produce] = producerID;
                         UnmapViewOfFile(hMemory);
                    cout << "Here I am at the position: " << indice_to_produce + 1 << endl;
                 ReleaseMutex(hMutex);
                 ReleaseSemaphore(allow_consume,1,NULL);   
                                               
    }
    
    CloseHandle(hMutex);
    CloseHandle(hMemory);
    CloseHandle(allow_consume);
    CloseHandle(allow_produce);
    
    getchar();
    return 0;
}
