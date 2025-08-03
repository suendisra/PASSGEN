/**
  @file     passgen.c
  @brief    Source file for STUDY application
  @author   suendisra
*/
#include "passgen.h"

static MOUSE    mouse = {0};

// STATIC PROTOTYPES
static INT_PTR CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

/* wWinMain */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previnst, LPWSTR cmd, int show)
{
    // instantiate win struct for windows config
    WinInit(instance, previnst, cmd, show);

    // load in any previously save settings
    Settings(TRUE);

    // create and run the main dialog
    if(Dialog(MainWndProc, IDD_PASSGEN, NULL, &wnd) == TRUE)
    {
        // loop application window procedure
        wnd.init = Standup;
        wnd.stop = Shutdown;
        Loop(&wnd);
    }

    return(0);
}

/**
  @fn           static INT_PTR CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
  @brief        Main window procedure
  @param[in]    hwnd handle to window whose proc is being processed
  @param[in]    msg windows message to be processed
  @param[in]    wp wparam associated with the message
  @param[in]    lp lparam associated with the message
  @return       system return value
*/
INT_PTR CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    long    rval = 0;

    switch(msg)
    {
        case WM_INITDIALOG:
            // set dialog caption and center on screen
            TextSet(hwnd, 0, L"%s %s", APP_TITLE, APP_VERSION);
            Center(hwnd, 0, NULL);
            break;

        case WM_SYSCOMMAND:
            switch(LOWORD(wp))
            {
                case SC_CLOSE:
                case SC_DEFAULT:
                    Kill(hwnd);
                    break;

                default:
                    rval = DefWindowProc(hwnd, msg, wp, lp);
                    break;
            }
            break;

        case WM_COMMAND:
            switch(LOWORD(wp))
            {
                case IDC_CHAR_LOCK:
                    Enable(hwnd, IDC_CHAR_SET, (SendDlgItemMessage(hwnd, IDC_CHAR_LOCK, BM_GETCHECK, 0, 0) == BST_UNCHECKED));
                    break;

                case IDC_CHAR_RESET:
                    ResetDialog();
                    break;

                case IDC_GENERATE:
                    GenPasswords(TRUE);
                    DrawPasswords();
                    break;

                case IDC_PASS_LIST:
                    switch(HIWORD(wp))
                    {
                        case STN_CLICKED:
                            // take mouse click and put it into perspective of the picture control
                            MouseScan(&mouse, NULL);
                            ScreenToClient(GetDlgItem(hwnd, IDC_PASS_LIST), &mouse.click);

                            // pass along re-calculated click to function to copy over to clipboard
                            CopyPassword(mouse.click);
                            break;
                    }
                    break;

                case IDCANCEL:
                    SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                    break;
            }
            break;
    }

    return(rval);
}
