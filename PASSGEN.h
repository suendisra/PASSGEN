/**
  @file     passgen.h
  @brief    Header file for PASSGEN application
  @author   suendisra
*/
#ifndef _PASSGEN_H_
#define _PASSGEN_H_

#include <util.h>
#include <gph.h>
#include <win.h>

#include "resource.h"

#define APP_COPYRIGHT   L"2025"
#define APP_FILE        L"pg.dat"
#define APP_TITLE       L"PASSGEN"
#define APP_VERSION     L"1.0.0"

// bring in appropriate version of library
#ifndef _DEBUG
#pragma comment(lib, "GPH.lib")
#pragma comment(lib, "UTIL.lib")
#pragma comment(lib, "WIN.lib")
#else
#pragma comment(lib, "GPHD.lib")
#pragma comment(lib, "UTILD.lib")
#pragma comment(lib, "WIND.lib")
#endif

struct
{
    wchar_t bank[STR_XLARGE];
    ULONG   len;
    BOOL    locked;
}app;

WNDW    wnd;

/**
  @fn           void CopyPassword(const POINT click);
  @brief        copy selected password to clipboard
*/
void CopyPassword(const POINT click);

/**
  @fn           void DrawPasswords(void)
  @brief        draw the list of passwords that have been generated
*/
void DrawPasswords(void);

/**
  @fn           void GenPasswords(const BOOL grabconfig)
  @brief        generate the passwords, pulling in dialog settings if specified
  @param[in]    grabconfig if true, grabs current configuration in dialog
*/
void GenPasswords(const BOOL grabconfig);

/**
  @fn           void ResetDialog(void)
  @brief        Force dialog configuration to return to default
*/
void ResetDialog(void);

/**
  @fn           BOOL Settings(const BOOL load)
  @brief        load in previously saved settings or save settings out to file
  @param[in]    load if true, attempts to load settings file; false writes to it
  @return       true if successful, false otherwise
*/
BOOL Settings(const BOOL load);

/**
  @fn           BOOL Standup(void)
  @brief        start up application
  @return       true if successful, false otherwise
*/
BOOL Standup(void);

/**
  @fn           void Shutdown(void)
  @brief        stop and tear down application
*/
void Shutdown(void);

#endif
