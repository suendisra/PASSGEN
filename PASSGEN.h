/**
  @file     passgen.h
  @brief    Header file for PASSGEN application
  @author   Charles Murray
  @version  0.0.0
*/
#ifndef _PASSGEN_H_
#define _PASSGEN_H_

#include <util.h>
#include <gph.h>
#include <win.h>

#include "resource.h"

#define APP_COPYRIGHT           L"2025"
#define APP_TITLE               L"PASSGEN"
#define APP_VERSION             L"0.0.0"

#pragma comment(lib, "GPH.lib")
#pragma comment(lib, "UTIL.lib")
#pragma comment(lib, "WIN.lib")

WNDW    wnd;

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
