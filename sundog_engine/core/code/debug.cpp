/*
    debug.cpp. Debug functions
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#include "../core.h"
#include "../../memory/memory.h"
#include "../../utils/utils.h"
#include "../debug.h"
#include <stdarg.h>

#ifndef PALMOS
    #include <stdio.h>
#else
    #include "palm_functions.h"
#endif

#define Y_LIMIT 50

int hide_debug_counter = 0;

void hide_debug( void )
{
    hide_debug_counter++;
}

void show_debug( void )
{
    if( hide_debug_counter > 0 )
	hide_debug_counter--;
}

long debug_count = 0;
char temp_debug_buf[ 256 ];
char *debug_buf = 0;
int debug_buf_size = 0;
int y = 10;

char *debug_output_file = "log.txt";

void debug_set_output_file( char *filename )
{
    debug_output_file = filename;
}

void debug_reset( void )
{
#ifdef NONPALM
#ifdef WINCE
    DeleteFile( c2w( debug_output_file ) );
#else
    remove( debug_output_file );
#endif
#endif
}

void sprint( char *dest_str, char *str, ... )
{
    va_list p;
    va_start( p, str );

    int ptr = 0;
    int ptr2 = 0;
    char num_str[ 64 ];
    int len;

    //Make a string:
    for(;;)
    {
	if( str[ ptr ] == 0 ) break;
	if( str[ ptr ] == '%' )
	{
	    if( str[ ptr + 1 ] == 'd' )
	    {
		//Integer value:
		int arg = va_arg( p, int );
		int_to_string( arg, num_str );
		len = mem_strlen( num_str );
		mem_copy( dest_str + ptr2, num_str, len );
		ptr2 += len;
		ptr++;
	    }
	    else
	    if( str[ ptr + 1 ] == 's' )
	    {
		//ASCII string:
		char *arg2 = va_arg( p, char* );
		if( arg2 )
		{
		    len = mem_strlen( arg2 );
		    if( len )
		    {
			mem_copy( dest_str + ptr2, arg2, len );
			ptr2 += len;
		    }
		}
		ptr++;
	    }
	}
	else
	{
	    dest_str[ ptr2 ] = str[ ptr ];
	    ptr2++;
	}
	ptr++;
    }
    dest_str[ ptr2 ] = 0;
    va_end( p );
}

void dprint( char *str, ... )
{
    if( hide_debug_counter ) return;

    va_list p;
    va_start( p, str );
    if( debug_buf_size == 0 )
    {
	debug_buf = temp_debug_buf;
	debug_buf_size = 256;
	debug_buf = (char*)MEM_NEW( HEAP_DYNAMIC, 256 );
    }
    int ptr = 0;
    int ptr2 = 0;
    char num_str[ 64 ];
    int len;

    //Make a number:
#ifdef PALMOS
    int_to_string( debug_count, num_str );
    len = mem_strlen( num_str );
    mem_copy( debug_buf, num_str, len );
    debug_buf[ len ] = ':';
    debug_buf[ len + 1 ] = ' ';
    ptr2 += len + 2;
#endif
    debug_count++;

    //Make a string:
    for(;;)
    {
	if( str[ ptr ] == 0 ) break;
	if( str[ ptr ] == '%' )
	{
	    if( str[ ptr + 1 ] == 'd' )
	    {
		int arg = va_arg( p, int );
		int_to_string( arg, num_str );
		len = mem_strlen( num_str );
		if( ptr2 + len >= debug_buf_size )
		{
		    //Resize debug buffer:
		    debug_buf_size += 256;
		    debug_buf = (char*)mem_resize( debug_buf, debug_buf_size );
		}
		mem_copy( debug_buf + ptr2, num_str, len );
		ptr2 += len;
		ptr++;
	    }
	    else
	    if( str[ ptr + 1 ] == 's' )
	    {
		//ASCII string:
		char *arg2 = va_arg( p, char* );
		if( arg2 )
		{
		    len = mem_strlen( arg2 );
		    if( len )
		    {
			if( ptr2 + len >= debug_buf_size )
			{
			    //Resize debug buffer:
			    debug_buf_size += 256;
			    debug_buf = (char*)mem_resize( debug_buf, debug_buf_size );
			}
			mem_copy( debug_buf + ptr2, arg2, len );
			ptr2 += len;
		    }
		}
		ptr++;
	    }
	}
	else
	{
	    debug_buf[ ptr2 ] = str[ ptr ];
	    ptr2++;
	    if( ptr2 >= debug_buf_size )
	    {
		//Resize debug buffer:
		debug_buf_size += 256;
		debug_buf = (char*)mem_resize( debug_buf, debug_buf_size );
	    }
	}
	ptr++;
    }
    debug_buf[ ptr2 ] = 0;
    va_end( p );

    //Save result:
#ifdef NONPALM
    FILE *f = fopen( debug_output_file, "ab" );
    if( f )
    {
	fprintf( f, "%s", debug_buf );
	fclose( f );
    }
    printf( "%s", debug_buf );
#else
    //PalmOS:
    int a;
    for( a = 0; a < 128; a++ )
    {
	if( debug_buf[ a ] == 0 ) break;
	if( debug_buf[ a ] == 0xA ) break;
	if( debug_buf[ a ] == 0xD ) break;
    }
    WinDrawChars( "                                                                                ", 80, 0, y );
    WinDrawChars( debug_buf, a, 0, y );
    y += 10;
    if( y >= Y_LIMIT ) y = 0;
#endif
}

void debug_close( void )
{
#ifndef PALMOS
#endif
    if( debug_buf && debug_buf != temp_debug_buf ) 
    {
	mem_free( debug_buf );
	debug_buf = temp_debug_buf;
	debug_buf_size = 256;
    }
}
