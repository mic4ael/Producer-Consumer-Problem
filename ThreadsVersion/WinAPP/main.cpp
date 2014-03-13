#include <iostream>
#include <windows.h>
#include <cstdio>
#include <conio.h>
#include <math.h>
#include <ctime>

using namespace std;

// functions
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI consumer(LPVOID);
DWORD WINAPI producent(LPVOID);
void showWindow();

// scalars
volatile int indice_to_consume = 0, indice_to_produce;
const int BUFFER_SIZE = 12; // buffer size

// char, int arrays
int ptr[BUFFER_SIZE] = {0}; // main buffer
char *curr[BUFFER_SIZE];
char szClassName[ ] = "WindowsApp";

// structures
typedef struct producentStr { // structure which defines which consument or producer is actually working
  int val;
  char *id;
  int productionTime;     
}prod;

typedef struct consumentStr { // structure which defines which consument or producer is actually working
  char *id;
  int val;
  int consumingTime;   
}cons;

// handles
HANDLE allow_consume = NULL, allow_produce = NULL; // Semaphors
HANDLE producers[3]; // producents
HANDLE consumers[3]; // consuments

// critical section

CRITICAL_SECTION cs; 

// application handle, variable
HWND hwnd;
DWORD ID = 0;  

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    srand(time(NULL));
    
    prod prod[3] = {{1,"Prod1",350},{2,"Prod2",300},{3,"Prod3",250}};
    cons cons[3] = {{"Cons1",1,300},{"Cons2",2,250},{"Cons3",3,200}};
    
    indice_to_produce = rand()%11;
    
    InitializeCriticalSection(&cs);
    
    allow_consume = CreateSemaphore(NULL,0,BUFFER_SIZE,NULL);
    
    if (!allow_consume)
    {
		return 0;
	}
    
    allow_produce = CreateSemaphore(NULL,BUFFER_SIZE,BUFFER_SIZE,NULL);
    
    if (!allow_produce)
    {
		return 0;
	}

    producers[0] = CreateThread(NULL,0,producent,&prod[0],0,&ID);
    producers[1] = CreateThread(NULL,0,producent,&prod[1],0,&ID);
    producers[2] = CreateThread(NULL,0,producent,&prod[2],0,&ID);
    consumers[0] = CreateThread(NULL,0,consumer,&cons[0],0,&ID);
    consumers[1] = CreateThread(NULL,0,consumer,&cons[1],0,&ID);
    consumers[2] = CreateThread(NULL,0,consumer,&cons[2],0,&ID);
    
    for (int i=0;i<3;++i)
    {
		if (producers[i] == NULL || consumers[i] == NULL)
			return 0;
	}
    
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    HBRUSH bgColor = CreateSolidBrush(RGB(0,100,0));

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */

    wincl.hbrBackground = bgColor;

    if (!RegisterClassEx (&wincl))
        return 0;
        
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Consumer-Producer Problem by Micha³ Ko³odziejski - Thread Version",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           705,                 /* The programs width */
           270,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    ShowWindow (hwnd, nFunsterStil);
    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_PAINT:
             showWindow();
        break;
        case WM_DESTROY:
             CloseHandle(allow_consume);
             CloseHandle(allow_produce);
             for (int i=0;i<3;i++)
             {
                 CloseHandle(producers[i]);
                 CloseHandle(consumers[i]);
             }
             DeleteCriticalSection(&cs);
             PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
             break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

DWORD WINAPI consumer(LPVOID param)
{
       cons *currentConsumer = (cons *)param;
       char *tmp = currentConsumer->id;
       int n = currentConsumer->val;
       int delay = currentConsumer->consumingTime;
       while (true)
       {
             // delay 
             //Sleep((rand()%10)*delay*n + delay);
             for (int k=0; k < 5000000; k++) pow((2.2/delay),pow(n,n));

             WaitForSingleObject(allow_consume,INFINITE);
                 EnterCriticalSection(&cs);
                     while (ptr[indice_to_consume]==0)
                     {
                           indice_to_consume +=1;
                           if (indice_to_consume > 11)
                           {
                              indice_to_consume = 0;
                           }
                     }
                     ptr[indice_to_consume] = 0;
                     curr[indice_to_consume] = tmp;
                 LeaveCriticalSection(&cs);
             ReleaseSemaphore(allow_produce,1,NULL);
             RedrawWindow(hwnd,NULL,NULL,RDW_INTERNALPAINT);
       }
       return 0;
}

DWORD WINAPI producent(LPVOID param)
{
       prod *currentProducent = (prod *)param;
       char *tmp = currentProducent->id;
       int delay = currentProducent->productionTime;
       int n = currentProducent->val;
       while (true)
       {
             // delay
             Sleep((rand()%10)*delay + delay);
             //for (int k=0; k < 4500000; k++) pow((2.2/delay),n);
             WaitForSingleObject(allow_produce,INFINITE);
                 EnterCriticalSection(&cs);
                     while (ptr[indice_to_produce]!=0)
                     {
                           indice_to_produce +=1;
                           if (indice_to_produce > 11)
                           {
                              indice_to_produce = 0;
                           }
                     }
                     ptr[indice_to_produce] = n;
                 LeaveCriticalSection(&cs);
             ReleaseSemaphore(allow_consume,1,NULL);
             RedrawWindow(hwnd,NULL,NULL,RDW_INTERNALPAINT);
       }
       return 0;
}

void showWindow()
{
        HDC DC = GetDC(hwnd);
        HBRUSH first = CreateSolidBrush(RGB(127,255,1));
        HBRUSH second = CreateSolidBrush(RGB(25,25,112));
        HBRUSH third = CreateSolidBrush(RGB(0,0,0));
        HBRUSH blank = CreateSolidBrush(RGB(255,255,255));
        
        SetTextColor(DC,RGB(0,0,0));
        SetBkMode(DC,TRANSPARENT);
        TextOut(DC,275,45,"ID of consumers: ",17);
        TextOut(DC,295,100,"Legend: ",8);
        TextOut(DC,30,122,"Producer 1: ",12);
        RECT rect1; RECT rect2; RECT rect3;
        rect1.left = 110; rect1.right = 130; rect1.top = 120; rect1.bottom = 140;
        FillRect(DC,&rect1,first);
        rect2.left = 110; rect2.right = 130; rect2.top = 160; rect2.bottom = 180;
        FillRect(DC,&rect2,second);
        rect3.left = 110; rect3.right = 130; rect3.top = 200; rect3.bottom = 220;
        FillRect(DC,&rect3,third);
        TextOut(DC,30,162,"Producer 2: ",12);
        TextOut(DC,30,202,"Producer 3: ",12);
        for(int i=0;i < BUFFER_SIZE;i++)
        {
                RECT Rect;
                RECT tmp;
                Rect.left    = 11 + i*55;
                Rect.right   = 9 + (i + 1)*55;
                Rect.top     = 10;
                Rect.bottom  = 40;
                
                tmp.left    = 11 + i*55;
                tmp.right   = 9 + (i + 1)*55;
                tmp.top     = 70;
                tmp.bottom  = 90;
                FillRect(DC,&tmp,blank);
                DrawText(DC,curr[i],5,&tmp,DT_CENTER);
                
                if (ptr[i] == 1)
                        FillRect(DC,&Rect,first);
                else if (ptr[i] == 2)
                        FillRect(DC,&Rect,second);
                else if (ptr[i] == 3)
                        FillRect(DC,&Rect,third);
                else
                        FillRect(DC,&Rect,blank);
        };
        DeleteObject((HBRUSH)first);
        DeleteObject((HBRUSH)second);
        DeleteObject((HBRUSH)third);
        DeleteObject((HBRUSH)blank);
        ReleaseDC(hwnd,DC);
};
