/*
    wmanager.cpp. SunDog window manager
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#include "../../main/user_code.h"
#include "../../core/debug.h"
#include "../../time/timemanager.h"
#include "../../utils/utils.h"
#include "../wmanager.h"
#include <stdarg.h>

#ifdef PALMOS
    #include <PalmOS.h>
    #include "palm_functions.h"
#endif

unsigned char font8x8[ 2050 ] = 
{
40,8,28,62,62,62,28,0,
126,129,165,129,189,153,129,126,
126,255,219,255,195,231,255,126,
108,254,254,254,124,56,16,0,
8,28,62,127,62,28,8,0,
28,28,28,127,127,107,8,28,
16,16,56,124,254,124,16,56,
0,0,24,60,60,24,0,0,
255,255,231,195,195,231,255,255,
0,60,102,66,66,102,60,0,
255,195,153,189,189,153,195,255,
15,7,15,125,204,204,204,120,
60,102,102,102,60,24,126,24,
63,51,63,48,48,112,240,224,
127,99,127,99,99,103,230,192,
24,219,60,231,231,60,219,24,
128,224,248,254,248,224,128,0,
2,14,62,254,62,14,2,0,
24,60,126,24,24,126,60,24,
102,102,102,102,102,0,102,0,
127,219,219,123,27,27,27,0,
62,99,56,108,108,56,204,120,
0,0,0,0,126,126,126,0,
24,60,126,24,126,60,24,255,
24,60,126,24,24,24,24,0,
24,24,24,24,126,60,24,0,
0,24,12,254,12,24,0,0,
0,48,96,254,96,48,0,0,
0,0,192,192,192,254,0,0,
0,36,102,255,102,36,0,0,
0,24,60,126,255,255,0,0,
0,255,255,126,60,24,0,0,
0,0,0,0,0,0,0,0,
48,120,120,48,48,0,48,0,
108,108,108,0,0,0,0,0,
108,108,254,108,254,108,108,0,
24,62,96,60,6,124,24,0,
0,99,102,12,24,51,99,0,
28,54,28,59,110,102,59,0,
48,48,96,0,0,0,0,0,
12,24,48,48,48,24,12,0,
48,24,12,12,12,24,48,0,
0,102,60,255,60,102,0,0,
0,48,48,252,48,48,0,0,
0,0,0,0,0,24,24,0,
0,0,0,126,0,0,0,0,
0,0,0,0,0,24,24,0,
3,6,12,24,48,96,64,0,
62,99,99,107,99,99,62,0,
24,56,88,24,24,24,126,0,
60,102,6,28,48,102,126,0,
60,102,6,28,6,102,60,0,
14,30,54,102,127,6,15,0,
126,96,124,6,6,102,60,0,
28,48,96,124,102,102,60,0,
126,102,6,12,24,24,24,0,
60,102,102,60,102,102,60,0,
60,102,102,62,6,12,56,0,
0,24,24,0,0,24,24,0,
0,24,24,0,0,24,24,48,
12,24,48,96,48,24,12,0,
0,0,126,0,0,126,0,0,
48,24,12,6,12,24,48,0,
60,102,6,12,24,0,24,0,
62,99,111,105,111,96,62,0,
24,60,102,126,102,102,102,0,
124,102,102,124,102,102,124,0,
60,102,96,96,96,102,60,0,
120,108,102,102,102,108,120,0,
126,96,96,120,96,96,126,0,
126,96,96,120,96,96,96,0,
60,102,96,110,102,102,60,0,
102,102,102,126,102,102,102,0,
60,24,24,24,24,24,60,0,
30,12,12,12,12,108,56,0,
102,108,120,112,120,108,102,0,
96,96,96,96,96,96,126,0,
99,119,127,107,99,99,99,0,
102,118,126,126,110,102,102,0,
60,102,102,102,102,102,60,0,
124,102,102,124,96,96,96,0,
60,102,102,102,102,60,14,0,
124,102,102,124,120,108,102,0,
60,102,96,60,6,102,60,0,
126,24,24,24,24,24,24,0,
102,102,102,102,102,102,60,0,
102,102,102,102,102,60,24,0,
99,99,99,107,127,119,99,0,
102,102,60,24,60,102,102,0,
102,102,102,60,24,24,24,0,
126,6,12,24,48,96,126,0,
60,48,48,48,48,48,60,0,
96,48,24,12,6,3,1,0,
60,12,12,12,12,12,60,0,
8,28,54,99,0,0,0,0,
0,0,0,0,0,0,0,255,
24,24,12,0,0,0,0,0,
0,0,60,6,62,102,62,0,
0,96,96,124,102,102,124,0,
0,0,60,96,96,96,60,0,
0,6,6,62,102,102,62,0,
0,0,60,102,126,96,60,0,
0,14,24,62,24,24,24,0,
0,0,62,102,102,62,6,124,
0,96,96,124,102,102,102,0,
0,24,0,56,24,24,60,0,
6,0,6,6,6,6,60,0,
0,96,96,108,120,108,102,0,
0,56,24,24,24,24,60,0,
0,0,102,127,127,107,99,0,
0,0,124,102,102,102,102,0,
0,0,60,102,102,102,60,0,
0,0,124,102,102,124,96,0,
0,0,62,102,102,62,6,0,
0,0,124,102,96,96,96,0,
0,0,62,96,60,6,124,0,
0,24,126,24,24,24,14,0,
0,0,102,102,102,102,62,0,
0,0,102,102,102,60,24,0,
0,0,99,107,127,62,54,0,
0,0,102,60,24,60,102,0,
0,0,102,102,62,12,120,0,
0,0,126,12,24,48,126,0,
14,24,24,112,24,24,14,0,
12,12,12,0,12,12,12,0,
112,24,24,14,24,24,112,0,
59,110,0,0,0,0,0,0,
0,8,28,54,99,99,127,0,
0,0,59,110,100,110,59,0,
0,60,102,124,102,124,96,96,
0,126,102,96,96,96,96,0,
0,127,54,54,54,54,54,0,
126,102,48,24,48,102,126,0,
0,0,63,108,108,108,56,0,
0,51,51,51,51,62,48,96,
0,59,110,12,12,12,12,0,
126,24,60,102,102,60,24,0,
28,54,99,127,99,54,28,0,
28,54,99,99,54,54,119,0,
14,24,12,62,102,102,60,0,
0,0,126,219,219,126,0,0,
6,12,126,219,219,126,96,192,
28,48,96,124,96,48,28,0,
60,102,102,102,102,102,102,0,
0,126,0,126,0,126,0,0,
48,96,96,0,0,0,0,0,
48,48,96,0,0,0,0,0,
12,24,48,24,12,0,126,0,
14,27,27,24,24,24,24,24,
24,24,24,24,24,216,216,112,
24,24,0,126,0,24,24,0,
0,59,110,0,59,110,0,0,
28,54,54,28,0,0,0,0,
0,0,0,252,12,12,0,0,
0,0,0,0,24,0,0,0,
15,12,12,12,236,108,60,28,
120,108,108,108,108,0,0,0,
0,0,60,60,60,60,0,0,
248,204,204,250,198,207,198,199,
14,27,24,60,24,24,216,112,
0,0,0,0,0,0,0,0,
24,24,0,24,24,24,24,0,
0,24,60,102,96,102,60,24,
28,54,50,120,48,115,126,0,
0,102,24,36,36,24,102,0,
102,102,102,60,126,24,126,24,
12,12,12,0,12,12,12,0,
62,99,56,108,108,56,204,120,
36,126,96,96,120,96,126,0,
124,130,154,162,162,154,130,124,
30,54,54,31,0,63,0,0,
0,51,102,204,102,51,0,0,
0,0,0,252,12,12,0,0,
0,0,0,126,0,0,0,0,
124,130,186,186,178,170,130,124,
255,0,0,0,0,0,0,0,
28,54,54,28,0,0,0,0,
24,24,126,24,24,0,126,0,
112,24,48,96,120,0,0,0,
112,24,48,24,112,0,0,0,
48,48,96,0,0,0,0,0,
0,0,102,102,102,102,91,192,
127,219,219,123,27,27,27,0,
0,0,0,12,12,0,0,0,
36,0,60,102,126,96,60,0,
48,112,48,48,48,0,0,0,
28,54,54,28,0,62,0,0,
0,204,102,51,102,204,0,0,
195,198,204,219,55,111,207,3,
195,198,204,222,51,102,204,15,
193,98,196,107,215,47,79,3,
24,0,24,48,96,102,60,0,
24,60,102,126,102,102,102,0,
124,96,96,124,102,102,124,0,
124,102,102,124,102,102,124,0,
126,96,96,96,96,96,96,0,
14,26,50,50,50,127,65,0,
126,96,96,120,96,96,126,0,
107,42,62,28,62,42,107,0,
62,103,67,12,67,103,62,0,
99,99,103,111,123,115,99,0,
107,99,103,111,123,115,99,0,
102,108,120,112,120,108,102,0,
8,28,54,54,99,99,99,0,
99,119,127,107,99,99,99,0,
102,102,102,126,102,102,102,0,
60,102,102,102,102,102,60,0,
126,102,102,102,102,102,102,0,
124,102,102,124,96,96,96,0,
60,102,96,96,96,102,60,0,
126,24,24,24,24,24,24,0,
102,102,102,102,62,12,120,0,
24,60,90,90,90,60,24,0,
102,102,60,24,60,102,102,0,
102,102,102,102,102,103,63,0,
102,102,102,102,62,6,6,0,
99,99,99,99,107,107,127,0,
99,99,99,107,107,106,125,0,
112,48,62,51,51,51,62,0,
99,99,115,107,107,107,115,0,
96,96,124,102,102,102,124,0,
62,99,3,15,3,99,62,0,
102,107,107,123,123,107,102,0,
62,102,102,62,14,22,38,0,
0,0,60,6,62,102,62,0,
0,60,96,124,102,102,60,0,
0,112,104,124,102,102,124,0,
0,0,124,96,96,96,96,0,
0,0,12,20,52,52,126,0,
0,0,60,102,126,96,60,0,
0,0,107,62,28,62,107,0,
0,0,60,102,12,102,60,0,
0,0,102,102,102,102,62,0,
24,0,102,102,102,102,62,0,
0,0,102,108,120,108,102,0,
0,0,24,60,102,102,102,0,
0,0,102,127,127,107,99,0,
0,0,102,102,126,102,102,0,
0,0,60,102,102,102,60,0,
0,0,126,102,102,102,102,0,
0,0,124,102,102,124,96,0,
0,0,60,96,96,96,60,0,
0,0,126,24,24,24,24,0,
0,0,102,102,62,12,120,0,
0,24,60,90,90,60,24,0,
0,0,102,60,24,60,102,0,
0,0,102,102,102,103,63,0,
0,0,102,102,102,62,6,0,
0,0,99,99,107,107,127,0,
0,0,99,107,107,106,125,0,
0,0,112,62,51,51,62,0,
0,0,99,115,107,107,115,0,
0,0,96,124,102,102,124,0,
0,0,62,99,15,99,62,0,
0,0,102,107,123,107,102,0,
0,0,62,102,62,22,38,0
};

//################################
//## MAIN FUNCTIONS:            ##
//################################

uint16 *g_mul = 0;

int win_init( char *windowname, int xsize, int ysize, int flags, window_manager *wm )
{
    int retval = 0;

    wm->wm_initialized = 0;

    wm->events_count = 0;
    wm->current_event_num = 0;
    sundog_mutex_init( &wm->events_mutex, 0 );

    wm->exit_request = 0;
    wm->exit_code = 0;

    wm->root_win = 0;

    wm->trash = 0;

    wm->window_counter = 0;

    mem_set( wm->timers, sizeof( wm->timers ), 0 );
    wm->timers_num = 0;

    wm->screen_lock_counter = 0;
    wm->screen_is_active = 0;

    wm->pen_x = -1;
    wm->pen_y = -1;
    wm->focus_win = 0;

    wm->handler_of_unhandled_events = 0;

    wm->user_font = 0;

    wm->cur_font_color = COLORMASK;
    wm->cur_font_invert = false;
    wm->cur_font_draw_bgcolor = false; 
    wm->cur_font_bgcolor = 0;
    wm->cur_transparency = 255;
    wm->cur_font_zoom = 256;

    //SECOND STEP: SCREEN SIZE SETTING and some device init
    dprint( "MAIN: device start\n" );

    wm->fb_offset = 0;
    wm->fb_xpitch = 1;
    wm->fb_ypitch = 0;
    int err = device_start( windowname, xsize, ysize, flags, wm ); //DEVICE DEPENDENT PART
    if( err )
    {
	dprint( "Error in device_start() %d\n", err );
	return 1; //Error
    }

    dprint( "MAIN: screen_xsize = %d\n", wm->screen_xsize );
    dprint( "MAIN: screen_ysize = %d\n", wm->screen_ysize );
      
    dprint( "MAIN: system palette init\n" );
    for( int c = 0; c < 16; c++ )
    {
	int r = c * 16;
	int g = c * 16;
	int b = c * 16;
	if( r >= 255 ) r = 255;
	if( g >= 255 ) g = 255;
	if( b >= 255 ) b = 255;
	if( r < 0 ) r = 0;
	if( g < 0 ) g = 0;
	if( b < 0 ) b = 0;
	wm->colors[ c ] = get_color( r, g, b );
    }
    wm->white = get_color( 255, 255, 255 );
    wm->black = get_color( 0, 0, 0 );
#ifdef GRAYSCALE
    wm->green = get_color( 220, 220, 220 );
    wm->yellow = get_color( 250, 250, 250 );
    wm->red = get_color( 230, 230, 230 );
#else
    wm->green = get_color( 0, 255, 0 );
    wm->yellow = get_color( 255, 255, 0 );
    wm->red = get_color( 255, 0, 0 );
#endif

    wm->decor_border_size = 4;
    wm->decor_header_size = 14;
    wm->scrollbar_size = 18;
    wm->button_xsize = 70;
    if( wm->screen_xsize > 320 && wm->screen_ysize > 320 )
    {
	wm->gui_size_increment = 1;
    }
    else
    {
	wm->gui_size_increment = 0;
    }
    if( wm->gui_size_increment )
    {
	wm->decor_border_size += 1;
	wm->decor_header_size += 1;
	wm->scrollbar_size += 4;
	wm->button_xsize += 20;
    }

    dprint( "MAIN: wmanager initialized\n" );

    wm->wm_initialized = 1;

    return retval;
}

void win_close( window_manager *wm )
{
    //Clear trash with a deleted windows:
    if( wm->trash )
    {
	for( unsigned int a = 0; a < mem_get_size( wm->trash ) / sizeof( WINDOWPTR ); a++ )
	{
	    if( wm->trash[ a ] ) mem_free( wm->trash[ a ] );
	}
	mem_free( wm->trash );
    }

    if( g_mul ) mem_free( g_mul );

    if( wm->screen_lock_counter > 0 )
    {
	dprint( "MAIN: WARNING. Screen is still locked (%d)\n", wm->screen_lock_counter );
	while( wm->screen_lock_counter > 0 ) device_screen_unlock( wm );
    }

    device_end( wm ); //DEVICE DEPENDENT PART (defined in eventloop.h)

    sundog_mutex_destroy( &wm->events_mutex );
}

//################################
//## WINDOWS FUNCTIONS:         ##
//################################

WINDOWPTR get_from_trash( window_manager *wm );

WINDOWPTR new_window( 
    char *name, 
    int x, 
    int y, 
    int xsize, 
    int ysize, 
    COLOR color, 
    WINDOWPTR parent, 
    int (*win_handler)( sundog_event*, window_manager* ),
    window_manager *wm )
{
    //Create window structure:
    WINDOWPTR win = get_from_trash( wm );
    if( win == 0 ) 
    {
	win = (WINDOWPTR)mem_new( HEAP_DYNAMIC, sizeof( WINDOW ), name, 0 );
	mem_set( win, sizeof( WINDOW ), 0 );
	win->id = (int16)wm->window_counter;
	wm->window_counter++;
    }

    //Setup properties:
    win->name = name;
    win->x = x;
    win->y = y;
    win->xsize = xsize;
    win->ysize = ysize;
    win->color = color;
    win->parent = parent;
    win->win_handler = ( int (*)( void*, void* ) )win_handler;
    win->click_time = time_ticks() - time_ticks_per_second() * 10;

    for( int a = 0; a < WIN_MAX_COMS; a++ )
    {
	win->x1com[ a ] = CEND;
	win->y1com[ a ] = CEND;
	win->x2com[ a ] = CEND;
	win->y2com[ a ] = CEND;
    }
    win->controllers_calculated = 0;
    win->controllers_defined = 0;

    win->action_handler = 0;
    win->handler_data = 0;
    win->action_result = 0;

    //Start init:
    if( win_handler )
    {
	sundog_font *prev_font = wm->user_font;
	if( win->user_font ) wm->user_font = win->user_font;
	sundog_event evt;
	mem_set( &evt, sizeof( evt ), 0 );
	evt.event_win = win;
	evt.event_type = EVT_GETDATASIZE;
	int datasize = win_handler( &evt, wm );
	if( datasize > 0 )
	{
	    win->data = mem_new( HEAP_DYNAMIC, datasize, "win data", 0 );
	}
	evt.event_type = EVT_AFTERCREATE;
	win_handler( &evt, wm );
	wm->user_font = prev_font;
    }

    //Save it to window manager:
    if( wm->root_win == 0 )
        wm->root_win = win;
    add_child( parent, win, wm );
    return win;
}

void set_window_controller( WINDOWPTR win, int ctrl_num, window_manager *wm, ... )
{
    va_list p;
    va_start( p, wm );
    int ptr = 0;
    win->controllers_defined = 1;
    while( ptr < WIN_MAX_COMS )
    {
	int command = va_arg( p, int );
	switch( ctrl_num )
	{
	    case 0: win->x1com[ ptr ] = command; break;
	    case 1: win->y1com[ ptr ] = command; break;
	    case 2: win->x2com[ ptr ] = command; break;
	    case 3: win->y2com[ ptr ] = command; break;
	}
	if( command == CEND ) break;
	ptr++;
    }
    va_end( p );
}

void move_to_trash( WINDOWPTR win, window_manager *wm )
{
    if( win )
    {
	win->visible = 0;
	win->flags |= WIN_FLAG_TRASH;
	if( wm->trash == 0 )
	{
	    wm->trash = (WINDOWPTR*)MEM_NEW( HEAP_DYNAMIC, sizeof( WINDOWPTR ) * 16 );
	    mem_set( wm->trash, sizeof( WINDOWPTR ) * 16, 0 );
	    wm->trash[ 0 ] = win;
	}
	else
	{
	    unsigned int w = 0;
	    for( w = 0; w < mem_get_size( wm->trash ) / sizeof( WINDOWPTR ); w++ )
	    {
		if( wm->trash[ w ] == 0 ) break;
	    }
	    if( w < mem_get_size( wm->trash ) / sizeof( WINDOWPTR ) ) 
		wm->trash[ w ] = win;
	    else
	    {
		wm->trash = (WINDOWPTR*)mem_resize( wm->trash, mem_get_size( wm->trash ) + sizeof( WINDOWPTR ) * 16 );
		wm->trash[ w ] = win;
	    }
	}
    }
}

WINDOWPTR get_from_trash( window_manager *wm )
{
    if( wm->trash == 0 ) return 0;
    unsigned int w = 0;
    for( w = 0; w < mem_get_size( wm->trash ) / sizeof( WINDOWPTR ); w++ )
    {
	if( wm->trash[ w ] != 0 ) 
	{
	    WINDOWPTR win = wm->trash[ w ];
	    wm->trash[ w ] = 0;
	    win->id++;
	    win->flags = 0;
	    return win;
	}
    }
    return 0;
}

void remove_window( WINDOWPTR win, window_manager *wm )
{
    if( win )
    {
	if( win->flags & WIN_FLAG_TRASH )
	{
	    dprint( "ERROR: can't remove already removed window (%s)\n", win->name );
	    return;
	}
	if( win->win_handler )
	{
	    //Sent EVT_BEFORECLOSE to window handler:
	    sundog_event evt;
	    evt.button = 0;
	    evt.mouse_button = 0;
	    evt.event_win = win;
	    evt.event_type = EVT_BEFORECLOSE;
	    win->win_handler( &evt, wm );
	}
	if( win->childs )
	{
	    //Remove childs:
	    while( win->childs_num )
		remove_window( win->childs[ 0 ], wm );
	    mem_free( win->childs );
	    win->childs = 0;
	}
	//Remove data:
	if( win->data )
	    mem_free( win->data );
	win->data = 0;
	//Remove direct draw data:
	wbd_deinit( win, wm );
	//Remove region:
	if( win->reg ) GdDestroyRegion( win->reg );
	win->reg = 0;
	//Remove window:
	if( win == wm->focus_win )
	    wm->focus_win = 0;
	if( win == wm->root_win )
	{
	    //It's root win:
	    move_to_trash( win, wm );
	    wm->root_win = 0;
	}
	else
	{
	    remove_child( win->parent, win, wm );
	    //Full remove:
	    move_to_trash( win, wm );
	}
    }
}

void add_child( WINDOWPTR win, WINDOWPTR child, window_manager *wm )
{
    if( win == 0 || child == 0 ) return;

    if( win->childs == 0 )
    {
	win->childs = (WINDOWPTR*)mem_new( HEAP_DYNAMIC, 4 * 4, "childs", 0 );
    }
    else
    {
	int old_size = mem_get_size( win->childs ) / 4;
	if( win->childs_num >= old_size )
	    win->childs = (WINDOWPTR*)mem_resize( win->childs, ( old_size + 4 ) * 4 );
    }
    win->childs[ win->childs_num ] = child;
    win->childs_num++;
    child->parent = win;
}

void remove_child( WINDOWPTR win, WINDOWPTR child, window_manager *wm )
{
    if( win == 0 || child == 0 ) return;

    //Remove link from parent window:
    int c;
    for( c = 0; c < win->childs_num; c++ )
    {
	if( win->childs[ c ] == child ) break;
    }
    if( c < win->childs_num )
    {
	for( ; c < win->childs_num - 1; c++ )
	{
	    win->childs[ c ] = win->childs[ c + 1 ];
	}
	win->childs_num--;
	child->parent = 0;
    }
}

void set_handler( WINDOWPTR win, int (*handler)(void*,WINDOWPTR,window_manager*), void *handler_data, window_manager *wm )
{
    if( win )
    {
	win->action_handler = (int (*)(void*,void*,void*)) handler;;
	win->handler_data = handler_data;
    }
}

void draw_window( WINDOWPTR win, window_manager *wm )
{
    win_draw_lock( win, wm );
    if( win && win->visible && win->reg )
    {
	sundog_event evt;
	evt.event_win = win;
	evt.event_type = EVT_DRAW;
	evt.button = 0;
	evt.mouse_button = 0;
	evt.pressure = 0;
	if( win->reg->numRects )
	{
	    sundog_font *prev_font = wm->user_font;
	    if( win->user_font ) wm->user_font = win->user_font;
	    if( win->win_handler && win->win_handler( &evt, wm ) )
	    {
		//Draw event was handled
	    }
	    else
	    {
		win_draw_box( win, 0, 0, win->xsize, win->ysize, win->color, wm );
	    }
	    wm->user_font = prev_font;
	}
	if( win->childs_num )
	{
	    for( int c = 0; c < win->childs_num; c++ )
	    {
		draw_window( win->childs[ c ], wm );
	    }
	}
    }
    win_draw_unlock( win, wm );
}

void show_window( WINDOWPTR win, window_manager *wm )
{
    if( win && ( win->flags & WIN_FLAG_ALWAYS_INVISIBLE ) == 0 )
    {
	win->visible = 1;
	for( int c = 0; c < win->childs_num; c++ )
	{
	    show_window( win->childs[ c ], wm );
	}
    }
}

void hide_window( WINDOWPTR win, window_manager *wm )
{
    if( win )
    {
	win->visible = 0;
	for( int c = 0; c < win->childs_num; c++ )
	{
	    hide_window( win->childs[ c ], wm );
	}
    }
}

void recalc_controllers( WINDOWPTR win, window_manager *wm );

void run_controller( WINDOWPTR win, int *c, int *val, int size, window_manager *wm )
{
    int p = 0;
    WINDOWPTR other_win = 0;
    int a;
    int mode = 0;
    int perc = 0;
    while( p < WIN_MAX_COMS && c[ p ] != CEND )
    {
	switch( c[ p ] )
	{
	    case CWIN: 
		p++;
		other_win = (WINDOWPTR)c[ p ];
		if( other_win->controllers_calculated == 0 ) recalc_controllers( other_win, wm );
		break;
	    case CX1:
		a = other_win->x;
		if( mode == 0 ) *val = a;
		if( mode == 1 ) *val -= a;
		if( mode == 2 ) *val += a;
		if( mode == 3 ) if( *val > a ) *val = a;
		if( mode == 4 ) if( *val < a ) *val = a;
		break;
	    case CY1: 
		a = other_win->y;
		if( mode == 0 ) *val = a;
		if( mode == 1 ) *val -= a;
		if( mode == 2 ) *val += a;
		if( mode == 3 ) if( *val > a ) *val = a;
		if( mode == 4 ) if( *val < a ) *val = a;
		break;
	    case CX2:
		a = other_win->x + other_win->xsize;
		if( mode == 0 ) *val = a;
		if( mode == 1 ) *val -= a;
		if( mode == 2 ) *val += a;
		if( mode == 3 ) if( *val > a ) *val = a;
		if( mode == 4 ) if( *val < a ) *val = a;
		break;
	    case CY2: 
		a = other_win->y + other_win->ysize;
		if( mode == 0 ) *val = a;
		if( mode == 1 ) *val -= a;
		if( mode == 2 ) *val += a;
		if( mode == 3 ) if( *val > a ) *val = a;
		if( mode == 4 ) if( *val < a ) *val = a;
		break;
	    case CSUB: mode = 1; break;
	    case CADD: mode = 2; break;
	    case CPERC: perc = 1; break;
	    case CMAXVAL: mode = 3; break;
	    case CMINVAL: mode = 4; break;
	    default: 
		if( mode == 0 ) 
		{
		    if( perc )
		    {
			*val = ( c[ p ] * size ) / 100;
		    }
		    else
		    {
			*val = c[ p ]; //Simple number
		    }
		}
		if( mode == 1 ) 
		{
		    //Sub:
		    if( perc )
		    {
			*val -= ( c[ p ] * size ) / 100;
		    }
		    else
		    {
			*val -= c[ p ]; //Simple number
		    }
		}
		if( mode == 2 ) 
		{
		    //Add:
		    if( perc )
		    {
			*val += ( c[ p ] * size ) / 100;
		    }
		    else
		    {
			*val += c[ p ]; //Simple number
		    }
		}
		if( mode == 3 ) 
		{
		    //Max val:
		    if( perc )
		    {
			if( *val > ( c[ p ] * size ) / 100 ) *val = ( c[ p ] * size ) / 100;
		    }
		    else
		    {
			if( *val > c[ p ] ) *val = c[ p ]; //Simple number
		    }
		}
		if( mode == 4 ) 
		{
		    //Min val:
		    if( perc )
		    {
			if( *val < ( c[ p ] * size ) / 100 ) *val = ( c[ p ] * size ) / 100;
		    }
		    else
		    {
			if( *val < c[ p ] ) *val = c[ p ]; //Simple number
		    }
		}
		mode = 0;
		perc = 0;
		break;
	}
	p++;
    }
}

void recalc_controllers( WINDOWPTR win, window_manager *wm )
{
    if( win && win->controllers_calculated == 0 )
    {
	if( !win->controllers_defined ) 
	    win->controllers_calculated = 1;
	else
	{
	    int x1 = win->x;
	    int y1 = win->y;
	    int x2 = win->x + win->xsize;
	    int y2 = win->y + win->ysize;
	    run_controller( win, win->x1com, &x1, win->parent->xsize, wm );
	    run_controller( win, win->x2com, &x2, win->parent->xsize, wm );
	    run_controller( win, win->y1com, &y1, win->parent->ysize, wm );
	    run_controller( win, win->y2com, &y2, win->parent->ysize, wm );
	    int temp;
	    if( x1 > x2 ) { temp = x1; x1 = x2; x2 = temp; }
	    if( y1 > y2 ) { temp = y1; y1 = y2; y2 = temp; }
	    win->x = x1;
	    win->y = y1;
	    win->xsize = x2 - x1;
	    win->ysize = y2 - y1;
	    win->controllers_calculated = 1;
	}
    }
}

void recalc_region( WINDOWPTR win, MWCLIPREGION *reg, int cut_x, int cut_y, int cut_x2, int cut_y2, int px, int py, window_manager *wm )
{
    if( !win->visible )
    {
	if( win->reg ) GdDestroyRegion( win->reg );
	win->reg = 0;
	return;
    }
    if( win->controllers_defined && win->controllers_calculated == 0 )
    {
	recalc_controllers( win, wm );
    }
    win->screen_x = win->x + px;
    win->screen_y = win->y + py;
    int x1 = win->x + px;
    int y1 = win->y + py;
    int x2 = win->x + px + win->xsize;
    int y2 = win->y + py + win->ysize;
    if( cut_x > x1 ) x1 = cut_x;
    if( cut_y > y1 ) y1 = cut_y;
    if( cut_x2 < x2 ) x2 = cut_x2;
    if( cut_y2 < y2 ) y2 = cut_y2;
    if( win->childs_num && !( x1 > x2 || y1 > y2 ) )
    {
	for( int c = win->childs_num - 1; c >= 0; c-- )
	{
	    recalc_region( 
		win->childs[ c ], 
		reg, 
		x1, y1, 
		x2, y2, 
		win->x + px, 
		win->y + py, 
		wm );
	}
    }
#ifdef OPENGLCOMP
    MWCLIPREGION *win_region;
    win_region = GdAllocRectRegion( x1, y1, x2, y2 );
    if( win->reg ) GdDestroyRegion( win->reg );
    win->reg = win_region;
#else
    MWCLIPREGION *win_region;
    if( x1 > x2 || y1 > y2 )
	win_region = GdAllocRegion();
    else
	win_region = GdAllocRectRegion( x1, y1, x2, y2 );
    //reg - current invisible region.
    //Calc corrected win region:
    GdSubtractRegion( win_region, win_region, reg );
    if( win->reg ) GdDestroyRegion( win->reg );
    win->reg = win_region;
    //Calc corrected invisible region:
    GdUnionRegion( reg, reg, win_region );
#endif
}

void clean_regions( WINDOWPTR win, window_manager *wm )
{
    if( win )
    {
	for( int c = 0; c < win->childs_num; c++ )
	    clean_regions( win->childs[ c ], wm );
	if( win->reg ) GdDestroyRegion( win->reg );
	win->controllers_calculated = 0;
	win->reg = 0;
    }
}

void recalc_regions( window_manager *wm )
{
    MWCLIPREGION *reg = GdAllocRegion();
    if( wm->root_win )
    {
	//Control ALWAYS_ON_TOP flag: ===
	int size = wm->root_win->childs_num;
	for( int i = 0; i < size; i++ )
	{
	    WINDOWPTR win = wm->root_win->childs[ i ];
	    if( win && win->flags & WIN_FLAG_ALWAYS_ON_TOP && i < size - 1 )
	    {
		//Bring this window to front:
		for( int i2 = i; i2 < size - 1; i2++ )
		{
		    wm->root_win->childs[ i2 ] = wm->root_win->childs[ i2 + 1 ];
		}
		wm->root_win->childs[ size - 1 ] = win;
	    }
	}
	//===============================
	clean_regions( wm->root_win, wm );
	recalc_region( wm->root_win, reg, 0, 0, wm->screen_xsize, wm->screen_ysize, 0, 0, wm );
    }
    if( reg ) GdDestroyRegion( reg );
}

void set_focus_win( WINDOWPTR win, window_manager *wm )
{
    if( win && win->flags & WIN_FLAG_TRASH )
    {
	//This window removed by someone. But we can't focus on removed window.
	//So.. Just focus on NULL window:
	win = 0;
    }

    WINDOWPTR prev_focus_win = wm->focus_win;
    uint16 prev_focus_win_id = wm->focus_win_id;

    WINDOWPTR new_focus_win = win;
    uint16 new_focus_win_id = 0;
    if( win ) new_focus_win_id = win->id;

    wm->focus_win = new_focus_win;
    wm->focus_win_id = new_focus_win_id;

    if( prev_focus_win != new_focus_win || 
	( prev_focus_win == new_focus_win && prev_focus_win_id != new_focus_win_id ) )
    {
	//Focus changed:

	sundog_event evt2;
	mem_set( &evt2, sizeof( evt2 ), 0 );

        wm->prev_focus_win = prev_focus_win;
        wm->prev_focus_win_id = prev_focus_win_id;

	if( new_focus_win )
	{
	    //Send FOCUS event:
	    //In this event's handling user can remember previous focused window (wm->prev_focus_win)
	    evt2.event_type = EVT_FOCUS;
	    evt2.event_win = new_focus_win;
	    handle_event( &evt2, wm );
	}

	if( prev_focus_win != new_focus_win ) //Only, if prev_focus_win is not removed
	{
	    if( prev_focus_win )
	    {
		//Send UNFOCUS event:
		evt2.event_type = EVT_UNFOCUS;
		evt2.event_win = prev_focus_win;
		handle_event( &evt2, wm );
	    }
	}
    }
}

int find_focus_window( WINDOWPTR win, WINDOW **focus_win, window_manager *wm )
{
    if( win == 0 ) return 0;
    if( win->reg && GdPtInRegion( win->reg, wm->pen_x, wm->pen_y ) )
    {
	*focus_win = win;
	return 1;
    }
    for( int c = 0; c < win->childs_num; c++ )
    {
	if( find_focus_window( win->childs[ c ], focus_win, wm ) )
	    return 1;
    }
    return 0;
}

int send_event( 
    WINDOWPTR win,
    int event_type, 
    int all_childs,
    int x, 
    int y, 
    int mouse_button, 
    int button, 
    int pressure,
    window_manager* wm )
{
    int retval = 0;
    
    sundog_mutex_lock( &wm->events_mutex );
    
    if( wm->events_count + 1 <= EVENTS )
    {
	//Get pointer to new event:
	int new_ptr = ( wm->current_event_num + wm->events_count ) & ( EVENTS - 1 );

	//Save new event to FIFO buffer:
	wm->events[ new_ptr ].event_type = event_type;
	wm->events[ new_ptr ].event_time = time_ticks();
	wm->events[ new_ptr ].event_win = win;
	wm->events[ new_ptr ].all_childs = all_childs;
	wm->events[ new_ptr ].x = (int16)x;
	wm->events[ new_ptr ].y = (int16)y;
	wm->events[ new_ptr ].mouse_button = mouse_button;
	wm->events[ new_ptr ].button = button;
	wm->events[ new_ptr ].pressure = pressure;

	//Increment number of unhandled events:
	volatile int new_event_count = wm->events_count + 1;
	wm->events_count = new_event_count;

	retval = 0;
    }
    else
    {
	retval = 1;
    }

    sundog_mutex_unlock( &wm->events_mutex );
    
    return retval;
}

int check_event( sundog_event *evt, window_manager *wm )
{
    if( evt == 0 ) return 1;

    if( evt->event_win == 0 )
    {
	if( !wm->wm_initialized ) return 1;
	
	if( evt->event_type == EVT_MOUSEBUTTONDOWN ||
	    evt->event_type == EVT_MOUSEBUTTONUP ||
	    evt->event_type == EVT_MOUSEMOVE )
	{
	    if( evt->x < 0 ) return 1;
	    if( evt->y < 0 ) return 1;
	    if( evt->x >= wm->screen_xsize ) return 1;
	    if( evt->y >= wm->screen_ysize ) return 1;
	    wm->pen_x = evt->x;
	    wm->pen_y = evt->y;
	    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    if( wm->last_unfocused_window )
	    {
		evt->event_win = wm->last_unfocused_window;
		if( evt->event_type == EVT_MOUSEBUTTONUP )
		{
		    wm->last_unfocused_window = 0;
		}
		return 0;
	    }
	    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    if( evt->event_type == EVT_MOUSEBUTTONDOWN )
	    { //If mouse click:
		if( evt->mouse_button & BUTTON_SCROLLUP ||
		    evt->mouse_button & BUTTON_SCROLLDOWN )
		{
		    //Mouse scroll up/down...
		    WINDOWPTR scroll_win = 0;
		    if( find_focus_window( wm->root_win, &scroll_win, wm ) )
		    {
			evt->event_win = scroll_win;
			return 0;
		    }
		    else
		    {
			//Window not found under the pointer:
			return 1;
		    }
		}
		else
		{
		    //Mouse click on some window...
		    WINDOWPTR focus_win = 0;
		    if( find_focus_window( wm->root_win, &focus_win, wm ) )
		    {
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if( focus_win->flags & WIN_FLAG_ALWAYS_UNFOCUSED )
			{
			    evt->event_win = focus_win;
			    wm->last_unfocused_window = focus_win;
			    return 0;
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			set_focus_win( focus_win, wm );
		    }
		    else
		    {
			//Window not found under the pointer:
			return 1;
		    }
		}
	    }
	}
	if( wm->focus_win )
	{
	    //Set pointer to window:
	    evt->event_win = wm->focus_win;

	    if( evt->event_type == EVT_MOUSEBUTTONDOWN && 
		( evt->event_time - wm->focus_win->click_time ) < ( DOUBLE_CLICK_PERIOD * time_ticks_per_second() ) / 1000 )
	    {
		evt->event_type = EVT_MOUSEDOUBLECLICK;
		wm->focus_win->click_time = evt->event_time - time_ticks_per_second() * 10; //Reset click time
	    }
	    if( evt->event_type == EVT_MOUSEBUTTONDOWN ) wm->focus_win->click_time = evt->event_time;
	}
    }

    return 0;
}

void handle_event( sundog_event *evt, window_manager *wm )
{
    if( evt->event_type == EVT_MOUSEDOUBLECLICK )
    {
	evt->event_type = EVT_MOUSEBUTTONDOWN;
	handle_event( evt, wm );
	evt->event_type = EVT_MOUSEDOUBLECLICK;
    }
    if( !user_event_handler( evt, wm ) || evt->all_childs )
    {
	//Event hot handled by simple event handler.
	//Send it to window:
	if( handle_event_by_window( evt, wm ) == 0 )
	{
	    evt->event_win = wm->handler_of_unhandled_events;
	    handle_event_by_window( evt, wm );
	}
    }
}

int handle_event_by_window( sundog_event *evt, window_manager *wm )
{
    int retval = 0;
    WINDOWPTR win = evt->event_win;
    if( win )
    {
	if( win->flags & WIN_FLAG_TRASH )
	{
	    dprint( "ERROR: can't handle event by removed window (%s)\n", win->name );
	    retval = 1;
	}
	else
	if( evt->event_type == EVT_DRAW ) 
	{
	    draw_window( win, wm );
	    retval = 1;
	}
	else
	{
	    sundog_font *prev_font = wm->user_font;
	    if( win->user_font ) wm->user_font = win->user_font;
	    if( win->win_handler )
	    {
		if( !win->win_handler( evt, wm ) || evt->all_childs )
		{
		    //Send event to children:
		    for( int c = 0; c < win->childs_num; c++ )
		    {
			evt->event_win = win->childs[ c ];
			if( handle_event_by_window( evt, wm ) && evt->all_childs == 0 )
			{
			    evt->event_win = win;
			    retval = 1;
			    goto end_of_handle;
			}
		    }
		    evt->event_win = win;
		}
		else
		{
		    retval = 1;
		    goto end_of_handle;
		}
	    }
end_of_handle:
	    wm->user_font = prev_font;
	    if( win == wm->root_win )
	    {
		if( evt->event_type == EVT_SCREENRESIZE )
		{
		    //On screen resize:
		    recalc_regions( wm );
		    draw_window( wm->root_win, wm );
		}
	    }
	}
    }
    return retval;
}

int EVENT_LOOP_BEGIN( sundog_event *evt, window_manager *wm )
{
    int rv = 0;
    if( wm->timers_num )
    {
	ticks_t cur_time = time_ticks();
	for( int t = 0; t < TIMERS; t++ )
	{
	    if( wm->timers[ t ].handler )
	    {
		if( wm->timers[ t ].deadline <= cur_time )
		{
		    wm->timers[ t ].handler( wm->timers[ t ].data, &wm->timers[ t ], wm ); 
		    wm->timers[ t ].deadline += wm->timers[ t ].delay;
		}
	    }
	}
    }
    device_event_handler( wm );
    evt->event_type = 0;
    user_event_handler( evt, wm );
    if( wm->events_count )
    {
	sundog_mutex_lock( &wm->events_mutex );

	//There are unhandled events:
	//Copy current event to "evt" buffer (prepare it for handling):
	mem_copy( evt, &wm->events[ wm->current_event_num ], sizeof( sundog_event ) );
	//This event will be handled. So decrement count of events:
	wm->events_count--;
	//And increment FIFO pointer:
	wm->current_event_num = ( wm->current_event_num + 1 ) & ( EVENTS - 1 );

	sundog_mutex_unlock( &wm->events_mutex );
	
	//Check the event:
	if( check_event( evt, wm ) == 0 )
	{
	    handle_event( evt, wm );
	    rv = 1;
	}
    }
    return rv;
}

int EVENT_LOOP_END( window_manager *wm )
{
    g_frame++;
    if( g_skip_frames == 0 || g_frame % g_skip_frames == 0 )
    {
	user_screen_redraw( wm );
	device_redraw_framebuffer( wm );
    }
    if( wm->exit_request ) return 1;
    return 0;
}

int add_timer( void (*handler)( void*, sundog_timer*, window_manager* ), void *data, ticks_t delay, window_manager *wm )
{
    int t = -1;
    for( t = 0; t < TIMERS; t++ )
    {
	if( wm->timers[ t ].handler == 0 )
	{
	    break;
	}
    }
    if( t < TIMERS )
    {
	wm->timers[ t ].handler = handler;
	wm->timers[ t ].data = data;
	wm->timers[ t ].deadline = time_ticks() + delay;
	wm->timers[ t ].delay = delay;
	wm->timers_num++;
    }
    else
    {
	t = -1;
    }
    return t;
}

void remove_timer( int timer, window_manager *wm )
{
    if( timer >= 0 && timer < TIMERS )
    {
	if( wm->timers[ timer ].handler )
	{
	    wm->timers[ timer ].handler = 0;
	    wm->timers_num--;
	}
    }
}

//################################
//## WINDOWS DECORATIONS:       ##
//################################

#define DRAG_LEFT	1
#define DRAG_RIGHT	2
#define DRAG_TOP	4
#define DRAG_BOTTOM	8
#define DRAG_MOVE	16
struct decorator_data
{
    int start_win_x;
    int start_win_y;
    int start_win_xs;
    int start_win_ys;
    int start_pen_x;
    int start_pen_y;
    int drag_mode;
    int fullscreen;
    int prev_x;
    int prev_y;
    int prev_xsize;
    int prev_ysize;
};

int decorator_handler( sundog_event *evt, window_manager *wm )
{
    int retval = 0;
    WINDOWPTR win = evt->event_win;
    decorator_data *data = (decorator_data*)win->data;
    int dx, dy;
    int rx = evt->x - win->screen_x;
    int ry = evt->y - win->screen_y;
    switch( evt->event_type )
    {
	case EVT_GETDATASIZE:
	    retval = sizeof( decorator_data );
	    break;
	case EVT_AFTERCREATE:
	    data->fullscreen = 0;
	    break;
	case EVT_MOUSEDOUBLECLICK:
	    if( evt->mouse_button & BUTTON_LEFT )
	    {
		//Make fullscreen:
		if( data->fullscreen == 1 )
		{
		    win->x = data->prev_x;
		    win->y = data->prev_y;
		    win->xsize = data->prev_xsize;
		    win->ysize = data->prev_ysize;
		}
		else
		{
		    data->prev_x = win->x;
		    data->prev_y = win->y;
		    data->prev_xsize = win->xsize;
		    data->prev_ysize = win->ysize;
		    win->x = 0; 
		    win->y = 0;
		    win->xsize = win->parent->xsize;
		    win->ysize = win->parent->ysize;
		}
		win->childs[ 0 ]->xsize = win->xsize - DECOR_BORDER_SIZE * 2;
		win->childs[ 0 ]->ysize = win->ysize - DECOR_BORDER_SIZE * 2 - DECOR_HEADER_SIZE;
		data->fullscreen ^= 1;
		data->drag_mode = 0;
		recalc_regions( wm );
		draw_window( wm->root_win, wm );
	    }
	    retval = 1;
	    break;
	case EVT_MOUSEBUTTONDOWN:
	    if( evt->mouse_button & BUTTON_LEFT )
	    {
		//Bring to front: ================================================
		int i;
		for( i = 0; i < win->parent->childs_num; i++ )
		{
		    if( win->parent->childs[ i ] == win ) break;
		}
		if( i < win->parent->childs_num - 1 )
		{
		    for( int i2 = i; i2 < win->parent->childs_num - 1; i2++ )
			win->parent->childs[ i2 ] = win->parent->childs[ i2 + 1 ];
		    win->parent->childs[ win->parent->childs_num - 1 ] = win;
		    recalc_regions( wm );
		}
		//================================================================
		data->start_pen_x = evt->x;
		data->start_pen_y = evt->y;
		data->start_win_x = win->x;
		data->start_win_y = win->y;
		data->start_win_xs = win->xsize;
		data->start_win_ys = win->ysize;
		data->drag_mode = 0;
		if( ry >= DECOR_HEADER_SIZE )
		{
		    if( rx < DECOR_BORDER_SIZE + 8 ) data->drag_mode |= DRAG_LEFT;
		    if( rx >= win->xsize - DECOR_BORDER_SIZE - 8 ) data->drag_mode |= DRAG_RIGHT;
		    if( ry >= win->ysize - DECOR_BORDER_SIZE - 8 ) data->drag_mode |= DRAG_BOTTOM;
		}
		else data->drag_mode = DRAG_MOVE;
		draw_window( win, wm );
	    }
	    retval = 1;
	    break;
	case EVT_MOUSEBUTTONUP:
	    data->drag_mode = 0;
	    retval = 1;
	    break;
	case EVT_MOUSEMOVE:
	    if( evt->mouse_button & BUTTON_LEFT )
	    {
		dx = evt->x - data->start_pen_x;
		dy = evt->y - data->start_pen_y;
		if( data->drag_mode == DRAG_MOVE )
		{
		    //Move:
		    int prev_x = win->x;
		    int prev_y = win->y;
		    win->x = data->start_win_x + dx;
		    win->y = data->start_win_y + dy;
		    if( prev_x != win->x || prev_y != win->y )
		    {
			if( data->fullscreen == 1 )
			{
			    win->xsize = data->prev_xsize;
			    win->ysize = data->prev_ysize;
			    data->fullscreen = 0;
			}
		    }
		}
		if( data->drag_mode & DRAG_LEFT )
		{
		    int prev_x = win->x;
		    int prev_xsize = win->xsize;
		    win->x = data->start_win_x + dx;
		    win->xsize = data->start_win_xs - dx;
		    if( prev_x != win->x || prev_xsize != win->xsize )
		    {
			if( win->xsize < 16 ) win->xsize = 16;
			data->fullscreen = 0;
		    }
		}
		if( data->drag_mode & DRAG_RIGHT )
		{
		    int prev_xsize = win->xsize;
		    win->xsize = data->start_win_xs + dx;
		    if( prev_xsize != win->xsize )
		    {
			if( win->xsize < 16 ) win->xsize = 16;
			data->fullscreen = 0;
		    }
		}
		if( data->drag_mode & DRAG_BOTTOM )
		{
		    int prev_ysize = win->ysize;
		    win->ysize = data->start_win_ys + dy;
		    if( prev_ysize != win->ysize )
		    {
			if( win->ysize < 16 ) win->ysize = 16;
			data->fullscreen = 0;
		    }
		}
		if( win->childs_num )
		{
		    win->childs[ 0 ]->xsize = win->xsize - DECOR_BORDER_SIZE * 2;
		    win->childs[ 0 ]->ysize = win->ysize - DECOR_BORDER_SIZE * 2 - DECOR_HEADER_SIZE;
		}
		recalc_regions( wm );
		draw_window( wm->root_win, wm );
	    }
	    retval = 1;
	    break;
	case EVT_DRAW:
	    win_draw_lock( win, wm );
	    win_draw_box( win, 0, 0, win->xsize, win->ysize, win->color, wm );
	    {
		int ssize = string_size( win->childs[0]->name, wm );
		if( ssize > 0 )
		{
		    for( int ll = 0; ll < 8; ll++ )
			win_draw_line( win, ssize + 8 + ll, ll, win->xsize, ll, blend( wm->white, win->color, ll * 32 ), wm );
		}
		else
		{
		    for( int ll = 0; ll < 8; ll++ )
			win_draw_line( win, 0, ll, win->xsize, ll, blend( wm->white, win->color, ll * 32 ), wm );
		}
	    }
	    if( win->childs_num )
	    {
		win_draw_string( win, win->childs[0]->name, DECOR_BORDER_SIZE, ( DECOR_HEADER_SIZE - char_y_size( wm ) ) / 2, get_color(0,0,0), win->color, wm );
		win_draw_line( win, 0, 0, win->xsize, 0, get_color(0,0,0), wm );
		win_draw_line( win, win->xsize-1, 0, win->xsize-1, win->ysize, get_color(0,0,0), wm );
		win_draw_line( win, 0, 0, 0, win->ysize, get_color(0,0,0), wm );
		win_draw_line( win, 0, win->ysize-1, win->xsize, win->ysize-1, get_color(0,0,0), wm );
	    }
	    else
	    {
		//No childs more :( Empty decorator. Lets remove it:
		remove_window( win, wm );
		recalc_regions( wm );
		draw_window( wm->root_win, wm );
	    }
	    win_draw_unlock( win, wm );
	    retval = 1;
	    break;
    }
    return retval;
}

WINDOWPTR new_window_with_decorator( 
    char *name, 
    int x, 
    int y, 
    int xsize, 
    int ysize, 
    COLOR color,
    WINDOWPTR parent, 
    int (*win_handler)( sundog_event*, window_manager* ),
    int flags,
    window_manager *wm )
{
    int border = DECOR_BORDER_SIZE;
    int header = DECOR_HEADER_SIZE;
    x -= border;
    y -= border + header;
    int dec_xsize = xsize + border * 2;
    int dec_ysize = ysize + border * 2 + header;
    if( flags & DECOR_FLAG_CENTERED )
    {
	x = ( parent->xsize - dec_xsize ) / 2;
	y = ( parent->ysize - dec_ysize ) / 2;
    }
    if( flags & DECOR_FLAG_CHECK_SIZE )
    {
	if( x < 0 )
	{
	    dec_xsize -= ( -x );
	    x = 0;
	}
	if( y < 0 )
	{
	    dec_ysize -= ( -y );
	    y = 0;
	}
	if( x + dec_xsize > parent->xsize )
	{
	    dec_xsize -= ( ( x + dec_xsize ) - parent->xsize );
	}
	if( y + dec_ysize > parent->ysize )
	{
	    dec_ysize -= ( ( y + dec_ysize ) - parent->ysize );
	}
    }
    WINDOWPTR dec = new_window( 
	"decorator", 
	x, y,
	dec_xsize, dec_ysize,
	blend( color, wm->black, 64 ),
	parent,
	decorator_handler,
	wm );
    xsize = dec_xsize - border * 2;
    ysize = dec_ysize - ( header + border * 2 );
    WINDOWPTR win = new_window( name, border, header + border, xsize, ysize, color, dec, win_handler, wm );
    return dec;
}

//################################
//## DRAWING FUNCTIONS:         ##
//################################

int char_x_size( uchar c, window_manager *wm )
{
    if( wm->user_font == 0 )
	return 8;
    else
	return wm->user_font->real_width[ c ];
}

int char_y_size( window_manager *wm )
{
    if( wm->user_font == 0 )
	return 8;
    else
	return wm->user_font->max_ysize;
}

int string_size( char *str, window_manager *wm )
{
    int size = 0;
    int p = 0;
    while( str[ p ] )
    {
	size += char_x_size( (unsigned char)str[ p ], wm );
	p++;
    }    
    return size;
}

void win_draw_lock( WINDOWPTR win, window_manager *wm )
{
    device_screen_lock( wm );
}

void win_draw_unlock( WINDOWPTR win, window_manager *wm )
{
    device_screen_unlock( wm );
}

void win_draw_box( WINDOWPTR win, int x, int y, int xsize, int ysize, COLOR color, window_manager *wm )
{
    if( win && win->visible && win->reg && win->reg->numRects )
    {
	x += win->screen_x;
	y += win->screen_y;
	if( win->reg->numRects )
	{
	    for( int r = 0; r < win->reg->numRects; r++ )
	    {
		int rx1 = win->reg->rects[ r ].left;
		int rx2 = win->reg->rects[ r ].right;
		int ry1 = win->reg->rects[ r ].top;
		int ry2 = win->reg->rects[ r ].bottom;

		//Control box size:
		int nx = x;
		int ny = y;
		int nxsize = xsize;
		int nysize = ysize;
		if( nx < rx1 ) { nxsize -= ( rx1 - nx ); nx = rx1; }
		if( ny < ry1 ) { nysize -= ( ry1 - ny ); ny = ry1; }
		if( nx + nxsize <= rx1 ) continue;
		if( ny + nysize <= ry1 ) continue;
		if( nx + nxsize > rx2 ) nxsize -= nx + nxsize - rx2;
		if( ny + nysize > ry2 ) nysize -= ny + nysize - ry2;
		if( nx >= rx2 ) continue;
		if( ny >= ry2 ) continue;
		if( nxsize < 0 ) continue;
		if( nysize < 0 ) continue;
        	
		//Draw it:
		device_draw_box( nx, ny, nxsize, nysize, color, wm );
	    }
	}
    }
}

void win_draw_frame( WINDOWPTR win, int x, int y, int xsize, int ysize, COLOR color, window_manager *wm )
{
    win_draw_line( win, x, y, x + xsize - 1, y, color, wm );
    win_draw_line( win, x + xsize - 1, y, x + xsize - 1, y + ysize - 1, color, wm );
    win_draw_line( win, x + xsize - 1, y + ysize - 1, x, y + ysize - 1, color, wm );
    win_draw_line( win, x, y + ysize - 1, x, y, color, wm );
}

void win_draw_frame3d( WINDOWPTR win, int x, int y, int xsize, int ysize, COLOR color, int inout, window_manager *wm )
{
    int depth = inout >> 8;
    inout &= 255;
    int tr = 110;
    int wtr = 120;
    /*if( depth == 2 )
    {
	win_draw_line( win, x + 1, y + 1, x + xsize - 2, y + 1, blend( color, wm->white, wtr / 2 ), wm );
	win_draw_line( win, x + 1, y + ysize - 2, x + xsize - 2, y + ysize - 2, blend( color, wm->black, tr / 2 ), wm );
    }*/
    for( int l = 0; l < 1; l++ )
    {
	if( inout == 0 )
	{
	    win_draw_line( win, x, y, x + xsize - 1, y, blend( color, wm->black, tr ), wm );
	    win_draw_line( win, x + xsize - 1, y, x + xsize - 1, y + ysize - 1, blend( color, wm->black, tr ), wm );
	    win_draw_line( win, x, y, x, y + ysize - 1, blend( color, wm->white, wtr ), wm );
	    win_draw_line( win, x, y + ysize - 1, x + xsize - 1, y + ysize - 1, blend( color, wm->white, wtr ), wm );
	}
	else
	{
	    win_draw_line( win, x, y, x + xsize - 1, y, blend( color, wm->white, wtr ), wm );
	    win_draw_line( win, x + xsize - 1, y, x + xsize - 1, y + ysize - 1, blend( color, wm->white, wtr ), wm );
	    win_draw_line( win, x, y, x, y + ysize - 1, blend( color, wm->black, tr ), wm );
	    win_draw_line( win, x, y + ysize - 1, x + xsize - 1, y + ysize - 1, blend( color, wm->black, tr ), wm );
	}
	x++; 
	y++;
	xsize -= 2;
	ysize -= 2;
	tr >>= 1;
	wtr >>= 1;
    }
}

void win_draw_bitmap_ext( 
    WINDOWPTR win, 
    int x, 
    int y, 
    int dest_xsize, 
    int dest_ysize,
    int source_x,
    int source_y,
    int source_xsize,
    int source_ysize,
    COLOR *data, 
    window_manager *wm )
{
    if( source_x < 0 ) { dest_xsize += source_x; x -= source_x; source_x = 0; }
    if( source_y < 0 ) { dest_ysize += source_y; y -= source_y; source_y = 0; }
    if( source_x >= source_xsize ) return;
    if( source_y >= source_ysize ) return;
    if( source_x + dest_xsize > source_xsize ) dest_xsize -= ( source_x + dest_xsize ) - source_xsize;
    if( source_y + dest_ysize > source_ysize ) dest_ysize -= ( source_y + dest_ysize ) - source_ysize;
    if( dest_xsize <= 0 ) return;
    if( dest_ysize <= 0 ) return;
    if( win && win->visible && win->reg && win->reg->numRects )
    {
	x += win->screen_x;
	y += win->screen_y;
	int xsize = dest_xsize;
	int ysize = dest_ysize;
	if( win->reg->numRects )
	{
	    for( int r = 0; r < win->reg->numRects; r++ )
	    {
		int rx1 = win->reg->rects[ r ].left;
		int rx2 = win->reg->rects[ r ].right;
		int ry1 = win->reg->rects[ r ].top;
		int ry2 = win->reg->rects[ r ].bottom;

		//Control box size:
		int src_x = source_x;
		int src_y = source_y;
		int nx = x;
		int ny = y;
		int nxsize = xsize;
		int nysize = ysize;
		if( nx < rx1 ) { nxsize -= ( rx1 - nx ); src_x += ( rx1 - nx ); nx = rx1; }
		if( ny < ry1 ) { nysize -= ( ry1 - ny ); src_y += ( ry1 - ny ); ny = ry1; }
		if( nx + nxsize <= rx1 ) continue;
		if( ny + nysize <= ry1 ) continue;
		if( nx + nxsize > rx2 ) nxsize -= nx + nxsize - rx2;
		if( ny + nysize > ry2 ) nysize -= ny + nysize - ry2;
		if( nx >= rx2 ) continue;
		if( ny >= ry2 ) continue;
		if( nxsize < 0 ) continue;
		if( nysize < 0 ) continue;
        	
		//Draw it:
		device_draw_bitmap( nx, ny, nxsize, nysize, src_x, src_y, source_xsize, source_ysize, data, wm );
	    }
	}
    }
}

void win_draw_bitmap( 
    WINDOWPTR win, 
    int x, 
    int y, 
    int xsize, 
    int ysize, 
    COLOR *data, 
    window_manager *wm )
{
    if( win && win->visible && win->reg && win->reg->numRects )
    {
	x += win->screen_x;
	y += win->screen_y;
	if( win->reg->numRects )
	{
	    for( int r = 0; r < win->reg->numRects; r++ )
	    {
		int rx1 = win->reg->rects[ r ].left;
		int rx2 = win->reg->rects[ r ].right;
		int ry1 = win->reg->rects[ r ].top;
		int ry2 = win->reg->rects[ r ].bottom;

		//Control box size:
		int src_x = 0;
		int src_y = 0;
		int nx = x;
		int ny = y;
		int nxsize = xsize;
		int nysize = ysize;
		if( nx < rx1 ) { nxsize -= ( rx1 - nx ); src_x += ( rx1 - nx ); nx = rx1; }
		if( ny < ry1 ) { nysize -= ( ry1 - ny ); src_y += ( ry1 - ny ); ny = ry1; }
		if( nx + nxsize <= rx1 ) continue;
		if( ny + nysize <= ry1 ) continue;
		if( nx + nxsize > rx2 ) nxsize -= nx + nxsize - rx2;
		if( ny + nysize > ry2 ) nysize -= ny + nysize - ry2;
		if( nx >= rx2 ) continue;
		if( ny >= ry2 ) continue;
		if( nxsize < 0 ) continue;
		if( nysize < 0 ) continue;
        	
		//Draw it:
		device_draw_bitmap( nx, ny, nxsize, nysize, src_x, src_y, xsize, ysize, data, wm );
	    }
	}
    }
}

void win_draw_char( WINDOWPTR win, int c, int x, int y, COLOR f, COLOR b, window_manager *wm )
{
    if( c < 0 ) return;
    if( wm->user_font == 0 )
    {
	COLOR bmp[ 8 * 8 ];
	c *= 8;
	int ptr = 0;
	for( int l = 0; l < 8; l++ )
	{
	    uchar v = font8x8[ c ]; c++;
	    for( int x = 0; x < 8; x++ )
	    {
		if( v & 0x80 )
		    bmp[ ptr ] = f;
		else
		    bmp[ ptr ] = b;
		v <<= 1;
		ptr++;
	    }
	}
	win_draw_bitmap( win, x, y, 8, 8, bmp, wm );
    }
    else
    {
	int xsize = wm->user_font->xsize[ c ];
	int ysize = wm->user_font->ysize[ c ];
	int real_width = wm->user_font->real_width[ c ];
	int descent = wm->user_font->descent;
	int y_offset = wm->user_font->y_offset[ c ];
	int bytes = xsize / 8;
	if( xsize % 8 ) bytes++;
	int fp = wm->user_font->offset[ c ];
	COLORPTR bmp = wm->user_font->rendered;
	int ptr = 0;
	int max_ptr = real_width * wm->user_font->max_ysize;
	if( ysize < wm->user_font->max_ysize )
	{
	    for( int a = 0; a < ( wm->user_font->max_ysize - ysize - ( descent - y_offset ) ) * real_width; a++ )
		bmp[ ptr++ ] = b;
	}
	for( int l = 0; l < ysize; l++ )
	{
	    int xx = xsize;
	    for( int bt = 0; bt < bytes; bt++ )
	    {
		uchar v = wm->user_font->data[ fp ];
		fp++;
		for( int cx = 0; ( cx < 8 ) && ( xx > 0 ); cx++, xx-- )
		{
		    if( v & 0x80 )
			bmp[ ptr ] = f;
		    else
			bmp[ ptr ] = b;
		    v <<= 1;
		    ptr++;
		}
	    }
	    if( xsize < real_width ) 
	    {
		for( int a = 0; a < ( real_width - xsize ); a++ ) bmp[ ptr++ ] = b;
	    }
	}
	if( ptr < max_ptr )
	{
	    for( ; ptr < max_ptr; ) bmp[ ptr++ ] = b;
	}
	win_draw_bitmap( 
	    win, 
	    x, y, 
	    real_width, 
	    wm->user_font->max_ysize, 
	    wm->user_font->rendered, 
	    wm );
    }
}

void win_draw_string( WINDOWPTR win, char *str, int x, int y, COLOR f, COLOR b, window_manager *wm )
{
    uchar *st = (uchar*)str;
    int p = 0;
    int start_x = x;
    while( st[ p ] != 0 )
    {
	if( st[ p ] == 0xA ) 
	{ 
	    y += char_y_size( wm ); 
	    x = start_x; 
	}
	else
	{
	    if( st[ p ] > 0x0F ) 
	    {
		win_draw_char( win, st[ p ], x, y, f, b, wm );
	    }
	    x += char_x_size( st[ p ], wm );
	}
	p++;
    }
}

#define cbottom 1
#define ctop 2
#define cleft 4
#define cright 8
int make_code( int x, int y, int clip_x1, int clip_y1, int clip_x2, int clip_y2 )
{
    int code = 0;
    if( y >= clip_y2 ) code = cbottom;
    else if( y < clip_y1 ) code = ctop;
    if( x >= clip_x2 ) code += cright;
    else if( x < clip_x1 ) code += cleft;
    return code;
}

void draw_line2( 
    int x1, int y1,
    int x2, int y2, 
    int clip_x1, int clip_y1,
    int clip_x2, int clip_y2,
    COLOR color, window_manager *wm )
{
    //Cohen-Sutherland line clipping algorithm:
    int code0;
    int code1;
    int out_code;
    int x, y;
    code0 = make_code( x1, y1, clip_x1, clip_y1, clip_x2, clip_y2 );
    code1 = make_code( x2, y2, clip_x1, clip_y1, clip_x2, clip_y2 );
    while( code0 || code1 )
    {
	if( code0 & code1 ) return; //Trivial reject
	else
	{
	    //Failed both tests, so calculate the line segment to clip
	    if( code0 )
		out_code = code0; //Clip the first point
	    else
		out_code = code1; //Clip the last point

	    if( out_code & cbottom )
	    {
		//Clip the line to the bottom of the viewport
		y = clip_y2 - 1;
		x = x1 + ( x2 - x1 ) * ( y - y1 ) / ( y2 - y1 );
	    }
	    else 
	    if( out_code & ctop )
	    {
		y = clip_y1;
		x = x1 + ( x2 - x1 ) * ( y - y1 ) / ( y2 - y1 );
	    }
	    else
	    if( out_code & cright )
	    {
		x = clip_x2 - 1;
		y = y1 + ( y2 - y1 ) * ( x - x1 ) / ( x2 - x1 );
	    }
	    else
	    if( out_code & cleft )
	    {
		x = clip_x1;
		y = y1 + ( y2 - y1 ) * ( x - x1 ) / ( x2 - x1 );
	    }

	    if( out_code == code0 )
	    { //Modify the first coordinate 
		x1 = x; y1 = y;
		code0 = make_code( x1, y1, clip_x1, clip_y1, clip_x2, clip_y2 );
	    }
	    else
	    { //Modify the second coordinate
		x2 = x; y2 = y;
		code1 = make_code( x2, y2, clip_x1, clip_y1, clip_x2, clip_y2 );
	    }
	}
    }

    //Draw line:
    device_draw_line( x1, y1, x2, y2, color, wm );
}

void win_draw_line( WINDOWPTR win, int x1, int y1, int x2, int y2, COLOR color, window_manager *wm )
{
    if( win && win->visible && win->reg && win->reg->numRects )
    {
	x1 += win->screen_x;
	y1 += win->screen_y;
	x2 += win->screen_x;
	y2 += win->screen_y;
	if( win->reg->numRects )
	{
	    for( int r = 0; r < win->reg->numRects; r++ )
	    {
		int rx1 = win->reg->rects[ r ].left;
		int rx2 = win->reg->rects[ r ].right;
		int ry1 = win->reg->rects[ r ].top;
		int ry2 = win->reg->rects[ r ].bottom;

		draw_line2( x1, y1, x2, y2, rx1, ry1, rx2, ry2, color, wm );
	    }
	}
    }
}

//################################
//## FONTS:                     ##
//################################

void font_load_string( char *str, int max_size, V3_FILE f )
{
    int p = 0;
    int begin = 0;
    while( 1 )
    {
	int c = v3_getc( f );
	if( v3_eof( f ) ) break;
	if( ( c >= '0' && c <= '9' ) || 
	    ( c >= 'a' && c <= 'z' ) || 
	    ( c >= 'A' && c <= 'Z' ) ||
	    c == '_' )
	{
	    begin = 1;
	}
	if( begin )
	{
	    str[ p ] = (char)c;
	    if( c == ' ' ) break;
	    if( c == 0xA || c == 0xD ) break;
	    p++;
	    if( p >= max_size ) break;
	}
    }
    str[ p ] = 0;
}

sundog_font *win_load_font( char *filename, window_manager *wm )
{
    sundog_font *fnt = 0;
    V3_FILE f = v3_open( filename, "rb" );
    if( f )
    {
	fnt = (sundog_font*)MEM_NEW( HEAP_DYNAMIC, sizeof( sundog_font ) );
	char str[ 65 ];
	int xbound = 0;
	int ybound = 0;
	int cur_char = 0;
	int ptr = 0;
	int data_size = 0;
	for(;;)
	{
	    font_load_string( str, 64, f );
	    if( v3_eof( f ) ) break;
	    if( mem_strcmp( str, "FONTBOUNDINGBOX" ) == 0 )
	    {
		font_load_string( str, 64, f );
		xbound = string_to_int( str );
		font_load_string( str, 64, f );
		ybound = string_to_int( str );
		fnt->max_ysize = ybound;
		int fntsize = xbound / 8;
		if( xbound % 8 ) fntsize++;
		data_size = fntsize * ybound * 256;
		fnt->data = (uchar*)MEM_NEW( HEAP_DYNAMIC, data_size );
	    }
	    else
	    if( mem_strcmp( str, "FONT_DESCENT" ) == 0 )
	    {
		font_load_string( str, 64, f );
		fnt->descent = string_to_int( str );
	    }
	    else
	    if( mem_strcmp( str, "ENCODING" ) == 0 )
	    {
		font_load_string( str, 64, f );
		cur_char = string_to_int( str );
		if( cur_char > 255 ) cur_char = 0;
		fnt->offset[ cur_char ] = ptr;
	    }
	    else
	    if( mem_strcmp( str, "DWIDTH" ) == 0 )
	    {
		font_load_string( str, 64, f );
		fnt->real_width[ cur_char ] = string_to_int( str );
	    }
	    else
	    if( mem_strcmp( str, "BBX" ) == 0 )
	    {
		font_load_string( str, 64, f );
		int xs = string_to_int( str );
		font_load_string( str, 64, f );
		int ys = string_to_int( str );
		font_load_string( str, 64, f );
		font_load_string( str, 64, f );
		int y_offset = string_to_int( str );
		fnt->xsize[ cur_char ] = xs;
		fnt->ysize[ cur_char ] = ys;
		fnt->y_offset[ cur_char ] = y_offset;
		if( fnt->real_width[ cur_char ] < fnt->xsize[ cur_char ] )
		    fnt->real_width[ cur_char ] = fnt->xsize[ cur_char ];
	    }
	    else
	    if( mem_strcmp( str, "BITMAP" ) == 0 )
	    {
		//Load char data:
		for(;;)
		{
		    font_load_string( str, 2, f );
		    if( str[ 0 ] == 'E' && str[ 1 ] == 'N' ) break;
		    if( v3_eof( f ) ) break;
		    fnt->data[ ptr ] = hex_string_to_int( str );
		    ptr++;
		}
	    }
	}
	v3_close( f );
    }
    return fnt;
}

void win_remove_font( sundog_font *fnt )
{
    if( fnt )
    {
	if( fnt->data ) mem_free( fnt->data );
	mem_free( fnt );
    }
}

//###################################
//### DIALOGS:                    ###
//###################################

char dialog_filename[ 1024 ];

char *dialog_open_file( char *name, char *mask, char *id, window_manager *wm )
{
    WINDOWPTR prev_focus = wm->focus_win;

    FILES_PROPS = id;
    FILES_MASK = mask;
    FILES_RESULTED_FILENAME = dialog_filename;
    WINDOWPTR win = new_window_with_decorator( 
	name, 
	0, 0, 
	320, 200, 
	wm->colors[ 10 ],
	wm->root_win, 
	files_handler,
	DECOR_FLAG_CENTERED | DECOR_FLAG_CHECK_SIZE,
	wm );
    show_window( win, wm );
    recalc_regions( wm );
    draw_window( win, wm );

    while( 1 )
    {
	sundog_event evt;
	EVENT_LOOP_BEGIN( &evt, wm );
	if( EVENT_LOOP_END( wm ) ) break;
	if( win->visible == 0 ) break;
    }

    set_focus_win( prev_focus, wm );

    if( dialog_filename[ 0 ] == 0 ) return 0;
    return dialog_filename;
}

int dialog( char *name, char *ok, char *cancel, window_manager *wm )
{
    WINDOWPTR prev_focus = wm->focus_win;
    int result = 0;

    DIALOG_OK_TEXT = ok;
    DIALOG_CANCEL_TEXT = cancel;
    DIALOG_TEXT = name;
    DIALOG_RESULT = &result;
    WINDOWPTR win = new_window_with_decorator( 
	"", 
	0, 0, 
	320, 200,
	wm->colors[ 14 ], 
	wm->root_win, 
	dialog_handler,
	DECOR_FLAG_CENTERED | DECOR_FLAG_CHECK_SIZE,
	wm );
    show_window( win, wm );
    recalc_regions( wm );
    draw_window( win, wm );

    while( 1 )
    {
	sundog_event evt;
	EVENT_LOOP_BEGIN( &evt, wm );
	if( EVENT_LOOP_END( wm ) ) break;
	if( win->visible == 0 ) break;
    }

    set_focus_win( prev_focus, wm );

    return result;
}

//###################################
//### DEVICE DEPENDENT FUNCTIONS: ###
//###################################

// device_start(), device_end() and device_event_handler() :

void fix_fullscreen_resolution( int *xsize, int *ysize, window_manager *wm )
{
    if( *xsize <= 320 ) { *xsize = 320; *ysize = 200; }
    else if( *xsize <= 640 ) { *xsize = 640; *ysize = 480; }
    else if( *xsize <= 800 ) { *xsize = 800; *ysize = 600; }
    else if( *xsize <= 1024 ) { *xsize = 1024; *ysize = 768; }
    else if( *xsize <= 1152 ) { *xsize = 1152; *ysize = 864; }
    else if( *xsize <= 1280 ) 
    {
	*xsize = 1280;
	if( *ysize <= 720 ) *ysize = 720;
	else if( *ysize <= 768 ) *ysize = 768;
	else if( *ysize <= 960 ) *ysize = 960;
	else if( *ysize <= 1024 ) *ysize = 1024;
    }
}

#ifndef FRAMEBUFFER
#endif

#ifndef NONPALM
    #include "wm_palmos.h"
#endif

#ifdef WIN
    #include "wm_win32.h"
#endif

#ifdef WINCE
    #include "wm_wince.h"
#endif

#ifdef UNIX
    #if defined(OPENGL) || defined(X11)
	#include "wm_unixgraphics.h"
    #endif
    #ifdef DIRECTDRAW
	#include "wm_unixgraphics_sdl.h"
    #endif
#endif

//###################################
//###################################
//###################################
