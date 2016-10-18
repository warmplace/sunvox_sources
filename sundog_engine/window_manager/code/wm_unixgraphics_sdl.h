/*
    wm_unixgfx_sdl.h. Platform-dependent module : Unix SDL
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#ifndef __WINMANAGER_UNIX_SDL__
#define __WINMANAGER_UNIX_SDL__

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>   	//for getenv()
#include <sys/time.h> 	//timeval struct
#include <sched.h>	//for sched_yield()

#include "SDL/SDL.h"

COLORPTR framebuffer;

pthread_t g_evt_pth;

void small_pause( long milliseconds )
{
    timeval t;
    t.tv_sec = 0;
    t.tv_usec = (long) milliseconds * 1000;
    select( 0 + 1, 0, 0, 0, &t );
}

void *event_thread( void *arg )
{
    window_manager *wm = (window_manager*)arg;
    SDL_Event event;
    volatile int need_exit = 0;
    printf( "SDL event thread started\n" );
    int mbuttons = 0;
    int mod_keys = 0; //Shift / alt / ctrl...
    while( need_exit == 0 )
    {
	int button = 0;
	SDL_WaitEvent( &event );
	switch( event.type ) 
	{
	    case SDL_MOUSEBUTTONDOWN:
		if( event.button.button == SDL_BUTTON_LEFT ) button |= BUTTON_LEFT;
		if( event.button.button == SDL_BUTTON_MIDDLE ) button |= BUTTON_MIDDLE;
		if( event.button.button == SDL_BUTTON_RIGHT ) button |= BUTTON_RIGHT;
		if( event.button.button == SDL_BUTTON_WHEELUP ) 
		{
		    send_event( 0, EVT_MOUSEBUTTONDOWN, 0, event.button.x, event.button.y, BUTTON_SCROLLUP, mod_keys, 1023, wm );
		}
		if( event.button.button == SDL_BUTTON_WHEELDOWN ) 
		{
		    send_event( 0, EVT_MOUSEBUTTONDOWN, 0, event.button.x, event.button.y, BUTTON_SCROLLDOWN, mod_keys, 1023, wm );
		}
		if( button )
		{
		    mbuttons |= button;
		    send_event( 0, EVT_MOUSEBUTTONDOWN, 0, event.button.x, event.button.y, button, mod_keys, 1023, wm );
		}
		break;
	    case SDL_MOUSEBUTTONUP:
		if( event.button.button == SDL_BUTTON_LEFT ) button |= BUTTON_LEFT;
		if( event.button.button == SDL_BUTTON_MIDDLE ) button |= BUTTON_MIDDLE;
		if( event.button.button == SDL_BUTTON_RIGHT ) button |= BUTTON_RIGHT;
		if( button )
		{
		    mbuttons &= ~button;
		    send_event( 0, EVT_MOUSEBUTTONUP, 0, event.button.x, event.button.y, button, mod_keys, 1023, wm );
		}
		break;
	    case SDL_MOUSEMOTION:
		if( event.motion.state == SDL_PRESSED )
		{
		    send_event( 0, EVT_MOUSEMOVE, 0, event.motion.x, event.motion.y, mbuttons, mod_keys, 1023, wm );
		}
		break;
	    case SDL_KEYDOWN:
	    case SDL_KEYUP:
		button = event.key.keysym.sym;
		if( button > 255 )
		{
		    switch( button )
		    {
			case SDLK_UP: button = KEY_UP; break;
			case SDLK_DOWN: button = KEY_DOWN; break;
			case SDLK_LEFT: button = KEY_LEFT; break;
			case SDLK_RIGHT: button = KEY_RIGHT; break;
			case SDLK_INSERT: button = KEY_INSERT; break;
			case SDLK_HOME: button = KEY_HOME; break;
			case SDLK_END: button = KEY_END; break;
			case SDLK_PAGEUP: button = KEY_PAGEUP; break;
			case SDLK_PAGEDOWN: button = KEY_PAGEDOWN; break;
			case SDLK_F1: button = KEY_F1; break;
			case SDLK_F2: button = KEY_F2; break;
			case SDLK_F3: button = KEY_F3; break;
			case SDLK_F4: button = KEY_F4; break;
			case SDLK_F5: button = KEY_F5; break;
			case SDLK_F6: button = KEY_F6; break;
			case SDLK_F7: button = KEY_F7; break;
			case SDLK_F8: button = KEY_F8; break;
			case SDLK_CAPSLOCK: button = KEY_CAPS; break;
			case SDLK_RSHIFT:
			case SDLK_LSHIFT:
			    if( event.type == SDL_KEYDOWN ) mod_keys |= KEY_SHIFT; else mod_keys &= ~KEY_SHIFT;
			    button = 0;
			    break;
			case SDLK_RCTRL:
			case SDLK_LCTRL:
			    if( event.type == SDL_KEYDOWN ) mod_keys |= KEY_CTRL; else mod_keys &= ~KEY_CTRL;
			    button = 0;
			    break;
			case SDLK_RALT:
			case SDLK_LALT:
			    if( event.type == SDL_KEYDOWN ) mod_keys |= KEY_ALT; else mod_keys &= ~KEY_ALT;
			    button = 0;
			    break;
			default: button = 0; break;
		    }	    
		}
		else
		{
		    switch( button )
		    {
			case SDLK_RETURN: button = KEY_ENTER; break;
			case SDLK_DELETE: button = KEY_DELETE; break;
			case SDLK_BACKSPACE: button = KEY_BACKSPACE; break;
		    }
		}
		if( button != 0 )
		{
		    if( event.type == SDL_KEYDOWN )
	    		send_event( 0, EVT_BUTTONDOWN, 0, 0, 0, 0, button | mod_keys, 1023, wm );
		    else
	    		send_event( 0, EVT_BUTTONUP, 0, 0, 0, 0, button | mod_keys, 1023, wm );
		}
		break;
	    case SDL_VIDEORESIZE:
    		pthread_mutex_lock( &wm->sdl_lock_mutex );
		SDL_SetVideoMode( event.resize.w, event.resize.h, COLORBITS,
                                  SDL_HWSURFACE | SDL_RESIZABLE ); // Resize window
		wm->screen_xsize = event.resize.w;
		wm->screen_ysize = event.resize.h;
		if( wm->root_win )
		{
		    int need_recalc = 0;
		    if( wm->root_win->x + wm->root_win->xsize > wm->screen_xsize ) 
		    {
			wm->root_win->xsize = wm->screen_xsize - wm->root_win->x;
			need_recalc = 1;
		    }
		    if( wm->root_win->y + wm->root_win->ysize > wm->screen_ysize ) 
		    {
			wm->root_win->ysize = wm->screen_ysize - wm->root_win->y;
			need_recalc = 1;
		    }
		    if( need_recalc ) recalc_regions( wm );
		}
    		pthread_mutex_unlock( &wm->sdl_lock_mutex );
	    	send_event( wm->root_win, EVT_SCREENRESIZE, 0, 0, 0, 0, 0, 0, wm );
		break;
	    case SDL_QUIT:
		need_exit = 1;
		break;
	}
    }
    wm->exit_request = 1;
    pthread_exit( 0 );
}

int device_start( char *windowname, int xsize, int ysize, int flags, window_manager *wm )
{
    int retval = 0;

    //Create SDL screen lock mutex:
    if( pthread_mutex_init( &wm->sdl_lock_mutex, 0 ) != 0 )
    {
	dprint( "Can't create SDL screen lock mutex\n" );
	return 1;
    }

    if( get_option( OPT_SCREENX ) != -1 ) xsize = get_option( OPT_SCREENX );
    if( get_option( OPT_SCREENY ) != -1 ) ysize = get_option( OPT_SCREENY );

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
    {
	dprint( "Can't initialize SDL: %s\n", SDL_GetError() );
	return 1;
    }
    
    int fs = 0;
    if( get_option( OPT_FULLSCREEN ) != -1 ) fs = 1;
    
    if( fs )
    {
	fix_fullscreen_resolution( &xsize, &ysize, wm );

	//Get list of available video-modes:
	SDL_Rect **modes;
	modes = SDL_ListModes( NULL, SDL_FULLSCREEN | SDL_HWSURFACE );
	if( modes != (SDL_Rect **)-1 )
	{
	    dprint( "Available Video Modes:\n" );
	    for( int i = 0; modes[ i ]; ++i )
		printf( "  %d x %d\n", modes[ i ]->w, modes[ i ]->h );
	}
    }
    
    int video_flags = 0;
    if( flags & WIN_INIT_FLAG_SCALABLE ) video_flags |= SDL_RESIZABLE;
    if( flags & WIN_INIT_FLAG_NOBORDER ) video_flags |= SDL_NOFRAME;
    if( fs ) video_flags = SDL_FULLSCREEN;
    wm->sdl_screen = SDL_SetVideoMode( xsize, ysize, COLORBITS, SDL_HWSURFACE | video_flags );
    if( wm->sdl_screen == 0 ) 
    {
	dprint( "SDL. Can't set videomode: %s\n", SDL_GetError() );
	return 1;
    }

    wm->screen_xsize = xsize;
    wm->screen_ysize = ysize;

    //Set window name:
    if( fs == 0 )
    {
	SDL_WM_SetCaption( windowname, windowname );
    }

    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );

    //Start event thread:
    if( pthread_create( &g_evt_pth, NULL, event_thread, wm ) != 0 )
    {
	dprint( "Can't create event thread\n" );
	return 1;
    }

    return retval;
}

void device_end( window_manager *wm )
{
    SDL_Event evt;
    evt.type = SDL_QUIT;
    if( SDL_PushEvent( &evt ) != 0 )
    {
	dprint( "Can't push SDL_QUIT event\n" );
    }
    sleep( 1 );

    dprint( "SDL_Quit()...\n" );
    SDL_Quit();
    
    //Remove mutexes:
    dprint( "Removing mutexes...\n" );
    pthread_mutex_destroy( &wm->sdl_lock_mutex );
}

long device_event_handler( window_manager *wm )
{
    //sched_yield();
    small_pause( 1 );
    if( wm->exit_request ) return 1;
    return 0;
}

void device_screen_lock( window_manager *wm )
{
    if( wm->screen_lock_counter == 0 )
    {
	pthread_mutex_lock( &wm->sdl_lock_mutex );
	if( SDL_MUSTLOCK( wm->sdl_screen ) ) 
	{
    	    if( SDL_LockSurface( wm->sdl_screen ) < 0 ) 
	    {
        	return;
	    }
        }
	framebuffer = (COLORPTR)wm->sdl_screen->pixels;
	wm->fb_ypitch = wm->sdl_screen->pitch / COLORLEN;
	wm->fb_xpitch = 1;
    }
    wm->screen_lock_counter++;

    if( wm->screen_lock_counter > 0 )
	wm->screen_is_active = 1;
    else
	wm->screen_is_active = 0;
}

void device_screen_unlock( window_manager *wm )
{
    if( wm->screen_lock_counter == 1 )
    {
	if( SDL_MUSTLOCK( wm->sdl_screen ) ) 
	{
	    SDL_UnlockSurface( wm->sdl_screen );
	}
	framebuffer = 0;
	pthread_mutex_unlock( &wm->sdl_lock_mutex );
    }

    if( wm->screen_lock_counter > 0 )
    {
	wm->screen_lock_counter--;
    }

    if( wm->screen_lock_counter > 0 )
	wm->screen_is_active = 1;
    else
	wm->screen_is_active = 0;
}

#ifdef FRAMEBUFFER

#include "wm_framebuffer.h"

#endif

//#################################
//#################################
//#################################

#endif
