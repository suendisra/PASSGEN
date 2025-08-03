/**
  @file     passgen-logic.c
  @brief    Source file for PASSGEN application
  @author   suendisra
*/
#include "passgen.h"

#define CLIPBOARD_ERROR     L"Unable to copy the selected item to the clipboard at the moment."
#define CLIPBOARD_SUCCESS   L"The selected item has been copied to the clipboard."

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
static CLRPOOL  pool = {0};
static INDEX    font = -1;
static wchar_t  list[PASS_COUNT][STR_NORM] = {0};

// STATIC PROTOTYPES
static void Defaults(void);
static BOOL DrawPassword(const QUAD cell, const long index);
static void SetupDialog(void);
static BOOL SetupGDI(void);

/* copy selected password to clipboard */
void CopyPassword(const POINT click)
{
    INDEX   idx = -1;

    idx = GridClick(grid, click, NULL, NULL, NULL);
    if((idx >= 0) && (idx < grid.rows))
    {
        // successfully identified the password to copy over
        if(StrClipboard(wnd.handl, L"%s", list[idx]) == TRUE)
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
BOOL DrawPassword(const QUAD cell, const long index)
{
    GphFontColor(gph, pool.color[index % pool.count], GTRANS);
    GphText(gph, &cell, ALIGN_LEFT, L"%s", list[index]);
    return(TRUE);
}

/* draw the grid list of passwords */
void DrawPasswords(void)
{
    GphClear(gph, NULL, LIST_BACK_COLOR);
    GridFunc(grid, DrawPassword);
    GphBlit(gph);
}

/* create randomized passwords */
void GenPasswords(const BOOL grabconfig)
{
    long    n = 0;
    wchar_t af[STR_TINY] = {0};

    if(grabconfig == TRUE)
    {
        // reach back to dialog to update app config settings
        TextGet(wnd.handl, IDC_PASS_LEN, af, sizeof(af));
        Convert(CONVERT_STR_LNG, &app.len, af, sizeof(app.len));
        TextGet(wnd.handl, IDC_CHAR_SET, app.bank, sizeof(app.bank));
        app.locked = (SendDlgItemMessage(wnd.handl, IDC_CHAR_LOCK, BM_GETCHECK, 0, 0) == BST_CHECKED);
    }

    // generate all the passwords
    for(n = 0; (n < PASS_COUNT); ++n)
    {
        StrPassword(app.bank, sizeof(app.bank), &list[n][0], sizeof(list[n]));
        list[n][app.len] = CHARNULL;
    }
}

/* revert dialog settings to default, regenerate passwords */
void ResetDialog(void)
{
    // set in defaults, place in dialog
    Defaults();
    SetupDialog();

    // now regenerate and draw the passwords
    GenPasswords(FALSE);
    DrawPasswords();
}

/* load in previously saved settings or save settings out to file */
BOOL Settings(const BOOL load)
{
    const enum FILEMODE mode = ((load == TRUE) ? FILE_READ : FILE_WRITE);

    CFILE               cf = {0};
    wchar_t             path[STR_PATH] = {0};

    BOOL                success = FALSE;

    // get system path for save data
    if(TRUE == FolderApp(path, sizeof(path), L"%s", APP_FILE))
    {
        // open save data
        if(TRUE == FileOpen(mode, path, &cf))
        {
            // iterate through each file_save or file_load call for demos
            if(load == TRUE)
            {
                success = (FileRead(cf, &app, sizeof(app)) == sizeof(app));
            }else{
                success = (FileWrite(cf, &app, sizeof(app)) == sizeof(app));
            }

            FileClose(&cf);
        }
    }

    if((load == TRUE) && (success == FALSE))
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
    SendDlgItemMessage(wnd.handl, IDC_PASS_LIST, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)gph.mem.bmp);

    // apply spin control range of 1 - 50
    SendDlgItemMessage(wnd.handl, IDC_PASS_SPIN, UDM_SETRANGE, 0, MAKELPARAM(PASS_SPIN_MAX, PASS_SPIN_MIN));
    TextSet(wnd.handl, IDC_PASS_LEN, L"%ld", max(app.len, PASS_SPIN_MIN));

    // drop in the character bank
    TextSet(wnd.handl, IDC_CHAR_SET, L"%s", app.bank);

    // apply locking mechanism, if necessary
    SendDlgItemMessage(wnd.handl, IDC_CHAR_LOCK, BM_SETCHECK, ((app.locked == TRUE) ? BST_CHECKED : BST_UNCHECKED), 0);
    Enable(wnd.handl, IDC_CHAR_SET, !app.locked);
}

/* helper function to initialize win32 graphics */
BOOL SetupGDI(void)
{
    QUAD    dims = {0};
    QUAD    ctrl = {0};

    BOOL    success = FALSE;

    // adjust dialog client dimensions for password list
    if(Dims(wnd.handl, IDC_CONTROL_GROUP, &ctrl, NULL) == TRUE)
    {
        // calculate the area needed to display the list, don't forget to cut off the title bar
        Quad(ctrl.x1, wnd.client.y1, ctrl.x2, (wnd.client.y2 - ctrl.cy - (wnd.bounds.cy - wnd.client.cy)), &dims);
        Quad(0, 0, dims.cx, dims.cy, &dims);

        // stand up the graphics
        if(Gph(GetDlgItem(wnd.handl, IDC_PASS_LIST), dims, &gph) == TRUE)
        {
            // put the list into a grid
            if(Grid(&gph, &dims, PASS_COUNT, PASS_COLS, HEADER_NONE, &grid) == TRUE)
            {
                // create the font needed for the password list
                font = FontGDI(LIST_FONT_NAME, LIST_FONT_SIZE, TRUE, FALSE, FALSE);
                if(font >= 0)
                {
                    // assign the font
                    GphFont(&gph, font);

                    // build the color pool for the passwords
                    MemClear(&pool, sizeof(pool));
                    pool.color[pool.count++] = GSEAGREEN;
                    pool.color[pool.count++] = GORANGE;
                    pool.color[pool.count++] = GLAVENDER;
                    pool.color[pool.count++] = GKHAKI;

                    // all good with GDI, move on
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

    // load and set application icon
    if(Resource(RSC_TYPE_ICO, IDI_PASSGEN, &wnd.icon, sizeof(wnd.icon)) == TRUE)
    {
        WindowIcon(wnd.handl, 0, wnd.icon);

        // stand up GDI graphics
        if(SetupGDI() == TRUE)
        {
            // configure the fields in the dialog
            SetupDialog();

            // all is well, move on
            GenPasswords(FALSE);
            DrawPasswords();
            success = TRUE;
        }
    }

    return(success);
}

/* stop and tear down application */
void Shutdown(void)
{
    FontKillGDI(font);
    GphKill(&gph);
    Settings(FALSE);
}
