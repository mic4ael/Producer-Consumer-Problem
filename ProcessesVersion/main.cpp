#include <iostream>
#include <windows.h>
#include <cstdio>
#include <conio.h>
#include <math.h>
#include <cstring>

// functions
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void showWindow();

// scalars
const int BUFFER_SIZE = 12;

// handles
HANDLE hMutex = NULL; // mutex
HANDLE allow_consume = NULL, allow_produce = NULL; // semaphors
HANDLE sharedMemory = NULL;

// char, int arrays
int ptr[BUFFER_SIZE]; // main buffer
char szClassName[ ] = "WindowsApp";
BOOL processes[4];
int (*pointer)[BUFFER_SIZE] = &ptr;

// application handle
HWND hwnd;   

PROCESS_INFORMATION pi1,pi2,pi3,pi4; 

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    allow_consume = CreateSemaphore(NULL,0,BUFFER_SIZE,"Global\\allowConsume");
    
    if (!allow_consume)
    {
       return 0;
    }
    
    allow_produce = CreateSemaphore(NULL,BUFFER_SIZE,BUFFER_SIZE,"Global\\allowProduce");
    
    if (!allow_consume)
    {
       return 0;
    }
    
    hMutex = CreateMutex(NULL,FALSE,TEXT("Global\\mainMutex"));
    
    if (!hMutex)
    {
       return 0;
    }
    
    sharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(ptr),"shareMemory");
    
    if (!sharedMemory)
    {
       return 0;
    }
    
    pointer = (int (*)[BUFFER_SIZE])MapViewOfFile(sharedMemory,FILE_MAP_ALL_ACCESS,0,0,sizeof(ptr));
    
    if (!pointer)
    {
      return 0;
    }
            
    MSG messages;           
    WNDCLASSEX wincl;       
    
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      
    wincl.style = CS_DBLCLKS;                 
    wincl.cbSize = sizeof (WNDCLASSEX);
    
    HBRUSH bgColor = CreateSolidBrush(RGB(52,1,56));
    
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 
    wincl.cbClsExtra = 0;                      
    wincl.cbWndExtra = 0;                      
    wincl.hbrBackground = bgColor;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Producer-Consumer Problem by Micha³ Ko³odziejski - Process Version",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           0,       /* Windows decides the position */
           0,       /* where the window ends up on the screen */
           700,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
           
    ShowWindow (hwnd, nFunsterStil);
	
	STARTUPINFO si1 = {
   sizeof(si1), NULL,NULL,  "Producer 1", 350, 380, 310, 200, 0, 0,0xf2,
        STARTF_USEFILLATTRIBUTE | STARTF_USEPOSITION | STARTF_USESIZE, 
        0, 0 ,NULL,NULL,NULL  };
        
        STARTUPINFO si2 = {
   sizeof(si2), NULL,NULL,  "Producer 2", 0, 380, 310, 200, 0, 0,0xf2,
        STARTF_USEFILLATTRIBUTE | STARTF_USEPOSITION | STARTF_USESIZE, 
        0, 0 ,NULL,NULL,NULL  };
        
        STARTUPINFO si3 = {
   sizeof(si3), NULL,NULL,  "Consumer 1", 710, 0, 310, 200,0, 0,0xf2,STARTF_USEFILLATTRIBUTE | STARTF_USEPOSITION | STARTF_USESIZE,
        0, 0 ,NULL,NULL,NULL  };
        
        STARTUPINFO si4 = {
   sizeof(si4), NULL,NULL,  "Consumer 2", 710, 270, 310, 200, 0, 0,0xf2,
        STARTF_USEFILLATTRIBUTE | STARTF_USEPOSITION | STARTF_USESIZE,
        0, 0 ,NULL,NULL,NULL  };
    
    // creating processes
    processes[0] = CreateProcess(0,"producer 2 1", NULL,NULL,0, CREATE_NEW_CONSOLE, NULL,NULL, &si1, &pi1);
    processes[1] = CreateProcess(0,"producer 3 2", NULL,NULL,0, CREATE_NEW_CONSOLE, NULL,NULL, &si2, &pi2);
    processes[2] = CreateProcess(0,"consumer 4 Consumer1 1", NULL,NULL,0, CREATE_NEW_CONSOLE, NULL,NULL, &si3, &pi3);
    processes[3] = CreateProcess(0,"consumer 5 Consumer2 2", NULL,NULL,0, CREATE_NEW_CONSOLE, NULL,NULL, &si4, &pi4);
    
    for (int i=0;i<4;i++)
    {
        if (!processes[i])
        {
           return 0;
        }
    }
	
    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        RedrawWindow(hwnd,NULL,NULL,RDW_INTERNALPAINT);
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
             TerminateProcess( pi1.hProcess,1 ); TerminateProcess( pi2.hProcess,1 ); TerminateProcess( pi3.hProcess,1 );  TerminateProcess( pi4.hProcess,1 ); 
             CloseHandle( pi1.hThread ); CloseHandle( pi2.hThread );  CloseHandle( pi3.hThread ); CloseHandle( pi4.hThread ); 
             PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
             break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
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
        
        SetTextColor(DC,RGB(255,255,255));
        SetBkMode(DC,TRANSPARENT);
        TextOut(DC,295,100,"Legend: ",8);
        TextOut(DC,30,122,"Producer 1: ",12);
        RECT rect1; RECT rect2;
        rect1.left = 110; rect1.right = 130; rect1.top = 120; rect1.bottom = 140;
        FillRect(DC,&rect1,first);
        rect2.left = 110; rect2.right = 130; rect2.top = 160; rect2.bottom = 180;
        FillRect(DC,&rect2,second);
        TextOut(DC,30,162,"Producer 2: ",12);
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
                //FillRect(DC,&tmp,third);
                //DrawText(DC,(*names)[i],5,&tmp,DT_CENTER);
                
                if ((*pointer)[i] == 1)
                        FillRect(DC,&Rect,first);
                else if ((*pointer)[i] == 2)
                        FillRect(DC,&Rect,second);
                else
                        FillRect(DC,&Rect,blank);
        };
        DeleteObject((HBRUSH)first);
        DeleteObject((HBRUSH)second);
        DeleteObject((HBRUSH)third);
        DeleteObject((HBRUSH)blank);
        ReleaseDC(hwnd,DC);
};
