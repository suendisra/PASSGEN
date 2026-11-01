/**
  @file     passgen.c
  @brief    Source file for STUDY application
*/
#include "passgen.h"

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    if(Logging(TRUE))
    {
        LTRACE(logger, IDS_TRACE_ENTER, L"wWinMain()");

        // instantiate win struct for windows config
        WinInit(instance, previnst, cmd, show);

        // load in any previously save settings
        Settings(TRUE);

        // create and run the main dialog
        if(Dialog(IDD_PASSGEN, NULL, &wnd))
        {
            LINFO(logger, IDS_DIALOG_CREATED, APP_TITLE);

            WindowConfig(Standup, NULL, Shutdown, &wnd);
            WindowFunc(WM_PAINT, (void*)DrawPasswords, &wnd);
            WindowFunc(WM_COMMAND, (void*)Command, &wnd);
            LINFO(logger, IDS_DIALOG_CONFIGURED, APP_TITLE);

            Loop(wnd);
        }

        LTRACE(logger, IDS_TRACE_EXIT, L"wWinMain()");
        Logging(FALSE);
    }

    return(0);
}
