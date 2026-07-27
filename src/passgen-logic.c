/**
  @file     passgen-logic.c
  @brief    Source file for PASSGEN application
  @author   suendisra
*/
#include "passgen.h"

#define CLIPBOARD_ERROR         L"Unable to copy the selected item to the clipboard at the moment."
#define CLIPBOARD_SUCCESS       L"The selected item has been copied to the clipboard."

#define LIST_BACK_COLOR         GCOAL
#define LIST_FONT_NAME          L"Consolas"
#define LIST_FONT_SIZE          24
#define LIST_VIEW_SHRINK        0.95

#define PASS_COLS               1
#define PASS_COUNT              16
#define PASS_DEFAULT_BANK       L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&()*+`-./:;<=>?@[\\]^-'{|}~"
#define PASS_DEFAULT_LENGTH     20
#define PASS_SPIN_MIN           13
#define PASS_SPIN_MAX           50

static GPH      gph = {0};
static GRID     grid = {0};
static MOUSE    mouse = {0};
static CLRPOOL  pool = NULL;
static INDEX    font = -1;
static wchar_t  list[PASS_COUNT][STR_NORM] = {0};

// STATIC PROTOTYPES
static void Defaults(void);
static BOOL DrawPassword(const QUAD cell, const INDEX index, void *userdata);
static void SetupDialog(void);
static BOOL SetupGDI(void);

/* handle user commands to the dialog window */
BOOL Command(const HWND hwnd, const WPARAM wp, const LPARAM lp)
{
    BOOL    handled = TRUE;

    if(lp == 0){}
    switch(LOWORD(wp))
    {
        case IDC_CHAR_LOCK:
            Enable(hwnd, IDC_CHAR_SET, !ButtonChecked(hwnd, IDC_CHAR_LOCK));
            break;

        case IDC_CHAR_RESET:
            ResetDialog(hwnd);
            break;

        case IDC_GENERATE:
            GenPasswords(TRUE);
            DrawPasswords(hwnd);
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

        default:
            handled = FALSE;
            break;
    }

    return(handled);
}

/* copy selected password to clipboard */
void CopyPassword(const POINT click)
{
    INDEX   idx = -1;
    wchar_t temp[STR_NORM] = {0};

    idx = GridClick(grid, click, NULL, NULL, NULL);
    if(Clamped(idx, -1, PASS_COUNT))
    {
        // successfully identified the password to copy over
        StrCopy(temp, sizeof(temp), list[idx], 0);
        StrReplace(temp, sizeof(temp), L"&&", L"&", FALSE);
        if(StrClipboard(wnd.handl, L"%s", temp))
        {
            Message(MSG_OK, wnd.handl, L"%s", CLIPBOARD_SUCCESS);
        }else{
            Message(MSG_ERR, wnd.handl, L"%s", CLIPBOARD_ERROR);
        }
    }
}

/* set in the defaults for the app */
void Defaults(void)
{
    MemClear(&app, sizeof(app));
    StrCopy(app.bank, sizeof(app.bank), PASS_DEFAULT_BANK, 0);
    app.len = PASS_DEFAULT_LENGTH;
    app.locked = TRUE;
}

/* helper function to draw each password in the list */
static BOOL DrawPassword(const QUAD cell, const INDEX index, void *userdata)
{
    const long      count = PoolCount(pool);
    const COLORREF  clr = PoolColor((index % count), pool);

    if(userdata == NULL)
    {
        GphFontColor(gph, clr, GTRANS);
        GphText(gph, &cell, ALIGN_LEFT, L"%s", list[index]);
    }

    return(TRUE);
}

/* draw the grid list of passwords */
void DrawPasswords(const HWND hwnd)
{
    const HWND  plist = GetDlgItem(hwnd, IDC_PASS_LIST);

    if(GphPaint(gph, plist))
    {
        GphClear(gph, NULL, LIST_BACK_COLOR);
        GridFunc(grid, NULL, DrawPassword);
        GphBlit(gph);
        GphPaint(gph, plist);
    }
}

/* create randomized passwords */
void GenPasswords(const BOOL grabconfig)
{
    long    n = 0;
    wchar_t af[STR_TINY] = {0};

    if(grabconfig)
    {
        // reach back to dialog to update app config settings
        TextGet(wnd.handl, IDC_CHAR_SET, app.bank, sizeof(app.bank));
        TextGet(wnd.handl, IDC_PASS_LEN, af, sizeof(af));
        app.len = wcstol(af, NULL, BASE_10);
        app.locked = ButtonChecked(wnd.handl, IDC_CHAR_LOCK);
    }

    // generate all the passwords
    for(n = 0; (n < PASS_COUNT); ++n)
    {
        StrPassword(app.bank, sizeof(app.bank), &list[n][0], sizeof(list[n]));
        list[n][app.len] = CHARNULL;
        StrReplace(list[n], sizeof(list[n]), L"&", L"&&", FALSE);
    }
}

/* revert dialog settings to default, regenerate passwords */
void ResetDialog(const HWND hwnd)
{
    // set in defaults, place in dialog
    Defaults();
    SetupDialog();

    // now regenerate and draw the passwords
    GenPasswords(FALSE);
    DrawPasswords(hwnd);
}

/* load in previously saved settings or save settings out to file */
BOOL Settings(const BOOL load)
{
    const enum FILEMODE mode = (load ? FILE_READ : FILE_WRITE);

    CFILE               cf = NULL;
    wchar_t             path[STR_PATH] = {0};

    BOOL                success = FALSE;

    // get system path for save data
    if(FolderApp(path, sizeof(path), L"%s", APP_FILE))
    {
        // open save data
        if(FileOpen(mode, path, &cf))
        {
            // iterate through each file_save or file_load call for demos
            if(load)
            {
                success = (FileRead(cf, &app, sizeof(app)) == sizeof(app));
            }else{
                success = (FileWrite(cf, &app, sizeof(app)) == sizeof(app));
            }

            FileClose(&cf);
        }
    }

    if(load && !success)
    {
        // load in some defaults
        Defaults();
    }

    return(success);
}

/* configure dialog field controls */
void SetupDialog(void)
{
    // stretch out the bitmap display area
    SendDlgItemMessage(wnd.handl, IDC_PASS_LIST, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)GphBmp(gph));

    // apply spin control range of 1 - 50
    UpDownRange(wnd.handl, IDC_PASS_SPIN, PASS_SPIN_MIN, PASS_SPIN_MAX);
    TextSet(wnd.handl, IDC_PASS_LEN, L"%ld", max(app.len, PASS_SPIN_MIN));

    // drop in the character bank
    TextSet(wnd.handl, IDC_CHAR_SET, L"%s", app.bank);

    // apply locking mechanism, if necessary
    ButtonCheck(wnd.handl, IDC_CHAR_LOCK, app.locked);
    Enable(wnd.handl, IDC_CHAR_SET, !app.locked);
}

/* helper function to initialize win32 graphics */
BOOL SetupGDI(void)
{
    QUAD    dims = {0};
    QUAD    ctrl = {0};
    long    c = 0;

    BOOL    success = FALSE;

    // adjust dialog client dimensions for password list
    if(Dims(wnd.handl, IDC_CONTROL_GROUP, &ctrl, NULL))
    {
        // calculate the area needed to display the list, don't forget to cut off the title bar
        Quad(ctrl.x1, wnd.client.y1, ctrl.x2, (wnd.client.y2 - ctrl.cy - (wnd.bounds.cy - wnd.client.cy)), &dims);
        Quad(0, 0, dims.cx, dims.cy, &dims);

        // stand up the graphics
        if(Gph(GetDlgItem(wnd.handl, IDC_PASS_LIST), dims, &gph))
        {
            // put the list into a grid
            if(Grid(gph, &dims, &grid))
            {
                GridConfig(&grid, HEADER_NONE, PASS_COUNT, PASS_COLS);

                // create the font needed for the password list and set it tot he GPH object
                font = FontGDI(LIST_FONT_NAME, LIST_FONT_SIZE, TRUE, FALSE, FALSE);
                if((font >= 0) && GphFontSet(gph, font))
                {
                    // build the color pool for the passwords
                    Pool(&pool);
                    PoolClear(pool);
                    PoolAdd(GSEAGREEN, c++, pool);
                    PoolAdd(GORANGE, c++, pool);
                    PoolAdd(GLAVENDER, c++, pool);
                    PoolAdd(GKHAKI, c++, pool);
                    success = TRUE;
                }
            }
        }
    }

    return(success);
}

/* start up application */
BOOL Standup(void)
{
    BOOL    success = FALSE;

    // set dialog caption and center on screen
    TextSet(wnd.handl, 0, L"%s %s", APP_TITLE, APP_VERSION);
    Center(wnd.handl, 0, NULL);

    // load and set application icon
    if(WindowIcon(IDI_PASSGEN, &wnd))
    {
        // stand up GDI graphics
        if(SetupGDI())
        {
            // configure the fields in the dialog
            SetupDialog();

            // all is well, move on
            GenPasswords(FALSE);
            DrawPasswords(wnd.handl);
            success = TRUE;
        }
    }

    return(success);
}

/* stop and tear down application */
void Shutdown(void)
{
    Settings(FALSE);
    PoolKill(&pool);
    FontKillGDI(font);
    GridKill(&grid);
    GphKill(&gph);
    WindowKill(&wnd);
}
