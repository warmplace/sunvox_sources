/*
    main.cpp. SunDog Engine main()
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#ifndef PALMOS
    #include <stdio.h>
#endif

#include "../user_code.h"

//################################
//## DEVICE VARIABLES:          ##
//################################

//PALMOS
#ifdef PALMOS
    #include <PalmOS.h>
    #define arm_startup __attribute__ ((section ("arm_startup")))
    #include "palm_functions.h"
#endif

//################################
//################################
//################################

//################################
//## APPLICATION MAIN:          ##
//################################

extern char *user_window_name;
extern char *user_profile_name;
extern char *user_debug_log_file_name;
extern int user_window_xsize;
extern int user_window_ysize;
extern int user_window_flags;

window_manager wm;
char *g_argv[ 64 ];

#if defined(WIN) || defined(WINCE)
void make_arguments( char *cmd_line )
{
    //Make standart argc and argv[] from windows lpszCmdLine:
    if( cmd_line && cmd_line[ 0 ] != 0 )
    {
        g_argv[ 0 ] = "prog";
        int arg_num = 1;
        int str_ptr = 0;
        int space = 1;
        while( cmd_line[ str_ptr ] != 0 )
        {
    	    if( cmd_line[ str_ptr ] != ' ' )
    	    {
	        if( space == 1 )
	        {
	    	    g_argv[ arg_num ] = &cmd_line[ str_ptr ];
		    arg_num++;
		}
		space = 0;
	    }
	    else
	    {
	        space = 1;
	    }
	    str_ptr++;
	}
	if( arg_num )
	{
	    wm.argc = arg_num;
	    wm.argv = g_argv;
	}
    }
}
#endif

//********************************
//WIN32 MAIN *********************
#ifdef WIN
int APIENTRY WinMain( HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow )
{
    {
	wm.hCurrentInst = hCurrentInst;
	wm.hPreviousInst = hPreviousInst; 
	wm.lpszCmdLine = lpszCmdLine;
	wm.nCmdShow = nCmdShow;
	wm.argc = 0;
	wm.argv = 0;
	if( lpszCmdLine && lpszCmdLine[ 0 ] != 0 )
	    make_arguments( lpszCmdLine );
#endif
//********************************
//********************************

//********************************
//WINCE MAIN *********************
#ifdef WINCE
char g_cmd_line[ 2048 ];
WCHAR g_window_name[ 2048 ];
extern WCHAR *className; //defined in window manager (wm_wince.h)
int WINAPI WinMain( HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPWSTR lpszCmdLine, int nCmdShow )
{
    {
	wm.hCurrentInst = hCurrentInst;
	wm.hPreviousInst = hPreviousInst;
	wm.lpszCmdLine = lpszCmdLine;
	wm.nCmdShow = nCmdShow;
	wm.argc = 0;
	wm.argv = 0;
	if( lpszCmdLine && lpszCmdLine[ 0 ] != 0 )
	{
	    wcstombs( g_cmd_line, lpszCmdLine, 2048 );
	    make_arguments( g_cmd_line );
	}
	mbstowcs( g_window_name, user_window_name, sizeof( g_window_name ) / sizeof( WCHAR ) );
	HWND wnd = FindWindow( className, g_window_name );
	if( wnd )
	{
	    //Already opened:
	    SetForegroundWindow( wnd ); //Make it foreground
	    return 0;
	}
#endif
//********************************
//********************************

//********************************
//PALMOS MAIN ********************
#ifdef PALMOS
long ARM_PalmOS_main( const void *emulStateP, void *userData, Call68KFuncType *call68KFuncP ) arm_startup;
long ARM_PalmOS_main( const void *emulStateP, void *userData, Call68KFuncType *call68KFuncP )
{
    {
	volatile void *oldGOT;
	volatile register void *gGOT asm ("r10");
	volatile ARM_INFO *arm_info = (ARM_INFO *)userData;
	oldGOT = (void*)gGOT;
	//gGOT = (void *)arm_info->GOT;
	volatile unsigned long newgot = (unsigned long)arm_info->GOT;
	__asm__ ( "mov r10, %0" : : "r" (newgot) );
	ownID = (unsigned short)arm_info->ID;
	g_form_handler = arm_info->FORMHANDLER; //g_form_handler defined in palm_functions.cpp
	g_new_screen_size = arm_info->new_screen_size;
	CALL_INIT
	dprint( "MAIN: ARM started\n" );
	int autooff_time = SysSetAutoOffTime( 0 );
	wm.argc = 0;
	wm.argv = 0;
#endif //PALMPOS
//********************************
//********************************

//********************************
//UNIX MAIN **********************
#ifdef UNIX
int main( int argc, char *argv[] )
{
    {
	wm.argc = argc;
	wm.argv = argv;
#endif
//********************************
//********************************

	debug_set_output_file( user_debug_log_file_name );
	debug_reset();

	dprint( "\n" );
	dprint( "\n" );
	dprint( "%s\n", SUNDOG_VERSION );
	dprint( "%s\n", SUNDOG_DATE );
	dprint( "\n" );
	dprint( "STARTING...\n" );
	dprint( "\n" );
	dprint( "\n" );

	get_disks();
	profile_new( 0 );
	profile_load( user_profile_name, 0 );
	char *window_name = user_window_name;
	if( profile_get_str_value( KEY_WINDOWNAME, 0 ) ) 
	    window_name = profile_get_str_value( KEY_WINDOWNAME, 0 );
	int flags = 0;
	if( profile_get_int_value( KEY_NOBORDER, 0 ) != -1 ) 
	    flags = WIN_INIT_FLAG_NOBORDER;
	if( win_init( window_name, user_window_xsize, user_window_ysize, user_window_flags | flags, &wm ) == 0 )
	{
	    int sound_stream_error = 0;    
#ifndef NOSOUND
	    if( profile_get_int_value( KEY_FREQ, 0 ) == -1 )
	    {
		sound_stream_error = sound_stream_init( 44100, 2 );
	    }
	    else
	    {
		int freq = profile_get_int_value( KEY_FREQ, 0 );
		if( freq < 44100 )
		{
		    dprint( "ERROR. Sampling frequency must be >= 44100\n" );
		    sound_stream_error = 1;
		}
#ifdef ONLY44100
		if( freq != 44100 ) 
		{
		    dprint( "ERROR. Sampling frequency must be 44100 for this device\n" );
		    sound_stream_error = 1;
		}
#endif
		sound_stream_error = sound_stream_init( freq, 2 );
	    }
	    if( !sound_stream_error )
		sound_stream_play();
#endif
	    if( !sound_stream_error )
	    {
		user_init( &wm );
		while( 1 )
		{
		    sundog_event evt;
		    EVENT_LOOP_BEGIN( &evt, &wm );
		    if( EVENT_LOOP_END( &wm ) ) break;
		}
		user_close( &wm );
#ifndef NOSOUND
		sound_stream_close(); //Close sound stream
#endif
	    }
	    win_close( &wm );     //Close window manager
	}
	profile_close( 0 );
	debug_close();
	mem_free_all();       //Close all memory blocks

	dprint( "\n" );
	dprint( "\n" );
	dprint( "BYE !\n" );
	dprint( "\n" );
	dprint( "\n" );

#ifdef PALMOS
	SysSetAutoOffTime( autooff_time );
	//gGOT = (void*)oldGOT;
	newgot = (unsigned long)oldGOT;
	__asm__ ( "mov r10, %0" : : "r" (newgot) );
	return 1;
#endif
    }

    return wm.exit_code;
}

//################################
//################################
//################################
