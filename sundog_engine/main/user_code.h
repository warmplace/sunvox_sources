#ifndef __SUNUSER__
#define __SUNUSER__

#include "core/core.h"
#include "core/debug.h"
#include "sound/sound.h"
#include "window_manager/wmanager.h"
#include "time/timemanager.h"

extern int g_frame;
extern int g_skip_frames;

void user_init( window_manager *wm );
int user_event_handler( sundog_event *evt, window_manager *wm );
void user_screen_redraw( window_manager *wm ); //Good place for screen drawing functions
void user_close( window_manager *wm );

#endif

