/**
  @file     passgen-logic.c
  @brief    Source file for PASSGEN application
  @author   Charles Murray
*/
#include "passgen.h"

static GPH      gph = {0};

/* start up application */
BOOL Standup(void)
{
    QUAD    dims = {0};
    QUAD    opts = {0};

    BOOL    success = FALSE;

    // load and set application icon
    if(Resource(RSC_TYPE_ICO, IDI_PASSGEN, &wnd.icon, sizeof(wnd.icon)) == TRUE)
    {
        WindowIcon(wnd.handl, 0, wnd.icon);

        // TODO
        success = TRUE;
    }

    return(success);
}

/* stop and tear down application */
void Shutdown(void)
{
    // TODO - GphKill(&gph);
}
