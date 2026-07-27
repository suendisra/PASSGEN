/**
  @file     passgen.c
  @brief    Source file for STUDY application
  @author   suendisra
*/
#include "passgen.h"

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    // instantiate win struct for windows config
    WinInit(instance, previnst, cmd, show);

    // load in any previously save settings
    Settings(TRUE);

    // create and run the main dialog
    if(Dialog(IDD_PASSGEN, NULL, &wnd))
    {
        WindowConfig(Standup, NULL, Shutdown, &wnd);
        WindowFunc(WM_PAINT, (void*)DrawPasswords, &wnd);
        WindowFunc(WM_COMMAND, (void*)Command, &wnd);
        Loop(wnd);
    }

    return(0);
}
