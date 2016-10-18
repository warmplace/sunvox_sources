/*
    wbd.cpp. WBD (Window Buffer Draw) functions
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

//How to use it in a window handler:
//EVT_AFTERCREATE:
//  wbd_init( win, win->xsize, win->ysize, wm );
//EVT_BEFORECLOSE:
//  wbd_deinit( win, wm );
//EVT_DRAW:	
//  win_draw_lock( win, wm );
//  if( win->wbd_xsize != win->xsize ||
//	win->wbd_ysize != win->ysize )
//	wbd_resize( win, win->xsize, win->ysize, wm );
//  wbd_set_current_window( win, wm );
//  ...
//  wbd_draw( win, wm );
//  win_draw_unlock( win, wm );

#include "../../main/user_code.h"
#include "../../core/debug.h"
#include "../../utils/utils.h"
#include "../wmanager.h"

extern unsigned char font8x8[ 2050 ];

void wbd_init( WINDOWPTR win, int xsize, int ysize, window_manager *wm )
{
    if( win )
    {
	if( win->wbd_buf )
	{
	    //Clear previous buffer:
	    mem_free( win->wbd_buf );
	    win->wbd_buf = 0;
	}
	win->wbd_buf = (COLORPTR)MEM_NEW( HEAP_DYNAMIC, xsize * ysize * COLORLEN );
	win->wbd_xsize = xsize;
	win->wbd_ysize = ysize;
    }
}

void wbd_deinit( WINDOWPTR win, window_manager *wm )
{
    if( win && win->wbd_buf )
    {
	mem_free( win->wbd_buf );
	win->wbd_buf = 0;
    }
}

void wbd_set_current_window( WINDOWPTR win, window_manager *wm )
{
    if( win && win->wbd_buf )
    {
	wm->cur_window = win;
	wm->g_screen = win->wbd_buf;
	wm->pscreen_x = win->wbd_xsize;
	wm->pscreen_y = win->wbd_ysize;
    }
}

void wbd_resize( WINDOWPTR win, int new_xsize, int new_ysize, window_manager *wm )
{
    if( win && win->wbd_buf )
    {
	int s = 0;
        if( wm->g_screen == win->wbd_buf ) s = 1;
	int real_size = mem_get_size( win->wbd_buf ) / COLORLEN;
	int new_size = new_xsize * new_ysize;
	if( new_size > real_size )
	{
	    win->wbd_buf = (COLORPTR)mem_resize( win->wbd_buf, ( new_size + ( new_size / 4 ) ) * COLORLEN );
	}
	win->wbd_xsize = new_xsize;
	win->wbd_ysize = new_ysize;
	if( s ) 
	{
	    wm->g_screen = win->wbd_buf;
	    wm->pscreen_x = win->wbd_xsize;
	    wm->pscreen_y = win->wbd_ysize;
	}
    }
}

void wbd_draw( WINDOWPTR win, window_manager *wm )
{
    if( win && win->wbd_buf )
    {
	win_draw_bitmap( win, 0, 0, win->wbd_xsize, win->wbd_ysize, win->wbd_buf, wm );
    }
}

void draw_screen( window_manager *wm )
{
    wbd_draw( wm->cur_window, wm );
}

void clear_screen( window_manager *wm )
{
    if( wm->cur_window && wm->cur_window->wbd_buf )
    {
#ifdef COLOR32BITS
#ifdef OPENGL
	COLORPTR ptr = wm->cur_window->wbd_buf;
	COLORPTR size = ptr + wm->cur_window->wbd_xsize * wm->cur_window->wbd_ysize;
	while( ptr < size ) *ptr++ = 0xFF000000;
#else
	COLORPTR ptr = wm->cur_window->wbd_buf;
	COLORPTR size = ptr + wm->cur_window->wbd_xsize * wm->cur_window->wbd_ysize;
	while( ptr < size ) *ptr++ = 0;
#endif
#else
	int isize = wm->cur_window->wbd_xsize * wm->cur_window->wbd_ysize;
	if( ( ( isize * COLORLEN ) & 3 ) == 0 )
	{
	    long *lptr = (long*)wm->cur_window->wbd_buf;
	    long *lsize = lptr + ( ( isize * COLORLEN ) / 4 );
	    while( lptr < lsize ) *lptr++ = 0;
	}
	else
	{
	    COLORPTR ptr = wm->cur_window->wbd_buf;
	    COLORPTR size = ptr + isize;
	    while( ptr < size ) *ptr++ = 0;
	}
#endif
    }
}

#define bottom 1
#define top 2
#define left 4
#define right 8
int make_code( int x, int y, int clip_x, int clip_y )
{
    int code = 0;
    if( y >= clip_y ) code = bottom;
    else if( y < 0 ) code = top;
    if( x >= clip_x ) code += right;
    else if( x < 0 ) code += left;
    return code;
}

void draw_line( int x1, int y1, int x2, int y2, COLOR color, window_manager *wm )
{
    if( x1 == x2 )
    {
	if( x1 >= 0 && x1 < wm->pscreen_x )
	{
	    if( y1 > y2 ) { int temp = y1; y1 = y2; y2 = temp; }
	    if( y1 < 0 ) y1 = 0;
	    if( y1 >= wm->pscreen_y ) return;
	    if( y2 < 0 ) return;
	    if( y2 >= wm->pscreen_y ) y2 = wm->pscreen_y - 1;
	    int ptr = y1 * wm->pscreen_x + x1;
	    int transp = wm->cur_transparency;
	    if( transp >= 256 )
	    {
		for( y1; y1 <= y2; y1++ )
		{
		    wm->g_screen[ ptr ] = color;
		    ptr += wm->pscreen_x;
		}
	    }
	    else
	    {
		for( y1; y1 <= y2; y1++ )
		{
		    wm->g_screen[ ptr ] = blend( wm->g_screen[ ptr ], color, transp );
		    ptr += wm->pscreen_x;
		}
	    }
	}
	return;
    }
    if( y1 == y2 )
    {
	if( y1 >= 0 && y1 < wm->pscreen_y )
	{
	    if( x1 > x2 ) { int temp = x1; x1 = x2; x2 = temp; }
	    if( x1 < 0 ) x1 = 0;
	    if( x1 >= wm->pscreen_x ) return;
	    if( x2 < 0 ) return;
	    if( x2 >= wm->pscreen_x ) x2 = wm->pscreen_x - 1;
	    int ptr = y1 * wm->pscreen_x + x1;
	    int transp = wm->cur_transparency;
	    if( transp >= 256 )
	    {
		for( x1; x1 <= x2; x1++ )
		{
		    wm->g_screen[ ptr ] = color;
		    ptr ++;
		}
	    }
	    else
	    {
		for( x1; x1 <= x2; x1++ )
		{
		    wm->g_screen[ ptr ] = blend( wm->g_screen[ ptr ], color, transp );
		    ptr ++;
		}
	    }
	}
	return;
    }

    //Cohen-Sutherland line clipping algorithm:
    int code0;
    int code1;
    int out_code;
    int x, y;
    code0 = make_code( x1, y1, wm->pscreen_x, wm->pscreen_y );
    code1 = make_code( x2, y2, wm->pscreen_x, wm->pscreen_y );
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

	    if( out_code & bottom )
	    {
		//Clip the line to the bottom of the viewport
		y = wm->pscreen_y - 1;
		x = x1 + ( x2 - x1 ) * ( y - y1 ) / ( y2 - y1 );
	    }
	    else 
	    if( out_code & top )
	    {
		y = 0;
		x = x1 + ( x2 - x1 ) * ( y - y1 ) / ( y2 - y1 );
	    }
	    else
	    if( out_code & right )
	    {
		x = wm->pscreen_x - 1;
		y = y1 + ( y2 - y1 ) * ( x - x1 ) / ( x2 - x1 );
	    }
	    else
	    if( out_code & left )
	    {
		x = 0;
		y = y1 + ( y2 - y1 ) * ( x - x1 ) / ( x2 - x1 );
	    }

	    if( out_code == code0 )
	    { //Modify the first coordinate 
		x1 = x; y1 = y;
		code0 = make_code( x1, y1, wm->pscreen_x, wm->pscreen_y );
	    }
	    else
	    { //Modify the second coordinate
		x2 = x; y2 = y;
		code1 = make_code( x2, y2, wm->pscreen_x, wm->pscreen_y );
	    }
	}
    }

    //Draw line:
    int len_x = x2 - x1; if( len_x < 0 ) len_x = -len_x;
    int len_y = y2 - y1; if( len_y < 0 ) len_y = -len_y;
    int ptr = y1 * wm->pscreen_x + x1;
    int delta;
    int v = 0, old_v = 0;
    int transp = wm->cur_transparency;
    if( len_x > len_y )
    {
	//Horisontal:
	if( len_x != 0 )
	    delta = ( len_y << 10 ) / len_x;
	else
	    delta = 0;
	if( transp >= 256 )
	    for( int a = 0; a <= len_x; a++ )
	    {
		wm->g_screen[ ptr ] = color;
		old_v = v;
		v += delta;
		if( x2 - x1 > 0 ) ptr++; else ptr--;
		if( ( old_v >> 10 ) != ( v >> 10 ) ) 
		{
		    if( y2 - y1 > 0 )
			ptr += wm->pscreen_x;
		    else
			ptr -= wm->pscreen_x;
		}
	    }
	else
	    for( int a = 0; a <= len_x; a++ )
	    {
		wm->g_screen[ ptr ] = blend( wm->g_screen[ ptr ], color, transp );
		old_v = v;
		v += delta;
		if( x2 - x1 > 0 ) ptr++; else ptr--;
		if( ( old_v >> 10 ) != ( v >> 10 ) ) 
		{
		    if( y2 - y1 > 0 )
			ptr += wm->pscreen_x;
		    else
			ptr -= wm->pscreen_x;
		}
	    }
    }
    else
    {
	//Vertical:
	if( len_y != 0 ) 
	    delta = ( len_x << 10 ) / len_y;
	else
	    delta = 0;
	if( transp >= 256 )
	    for( int a = 0; a <= len_y; a++ )
	    {
		wm->g_screen[ ptr ] = color;
		old_v = v;
		v += delta;
		if( y2 - y1 > 0 ) 
		    ptr += wm->pscreen_x;
		else
		    ptr -= wm->pscreen_x;
		if( ( old_v >> 10 ) != ( v >> 10 ) ) 
		{
		    if( x2 - x1 > 0 ) ptr++; else ptr--;
		}
	    }
	else
	    for( int a = 0; a <= len_y; a++ )
	    {
		wm->g_screen[ ptr ] = blend( wm->g_screen[ ptr ], color, transp );
		old_v = v;
		v += delta;
		if( y2 - y1 > 0 ) 
		    ptr += wm->pscreen_x;
		else
		    ptr -= wm->pscreen_x;
		if( ( old_v >> 10 ) != ( v >> 10 ) ) 
		{
		    if( x2 - x1 > 0 ) ptr++; else ptr--;
		}
	    }
    }
}

void draw_frame( int x, int y, int x_size, int y_size, COLOR color, window_manager *wm )
{
    draw_line( x, y, x + x_size, y, color, wm );
    draw_line( x + x_size, y, x + x_size, y + y_size, color, wm );
    draw_line( x + x_size, y + y_size, x, y + y_size, color, wm );
    draw_line( x, y + y_size, x, y, color, wm );
}

void draw_box( int x, int y, int x_size, int y_size, COLOR color, window_manager *wm )
{
    if( x < 0 ) { x_size += x; x = 0; }
    if( y < 0 ) { y_size += y; y = 0; }
    if( x + x_size <= 0 ) return;
    if( y + y_size <= 0 ) return;
    if( x + x_size > wm->pscreen_x ) x_size -= x + x_size - wm->pscreen_x;
    if( y + y_size > wm->pscreen_y ) y_size -= y + y_size - wm->pscreen_y;
    if( x >= wm->pscreen_x ) return;
    if( y >= wm->pscreen_y ) return;
    if( x_size < 0 ) return;
    if( y_size < 0 ) return;

    COLORPTR ptr = wm->g_screen + y * wm->pscreen_x + x;
    int transp = wm->cur_transparency;
    if( transp >= 255 )
	for( int cy = 0; cy < y_size; cy++ )
	{
	    COLORPTR size = ptr + x_size;
	    while( ptr < size ) *ptr++ = color;
	    ptr += wm->pscreen_x - x_size;
	}
    else
	for( int cy = 0; cy < y_size; cy++ )
	{
	    COLORPTR size = ptr + x_size;
	    while( ptr < size ) *ptr++ = blend( *ptr, color, transp );
	    ptr += wm->pscreen_x - x_size;
	}
}

int draw_char( uchar c, int x, int y, window_manager *wm )
{
    //Bounds control:
    if( c == 0 ) return wbd_char_x_size( c, wm );
    if( x < 0 ) return wbd_char_x_size( c, wm );
    if( y < 0 ) return wbd_char_x_size( c, wm );
    if( x > wm->pscreen_x - wbd_char_x_size( c, wm ) ) return wbd_char_x_size( c, wm );
    if( y > wm->pscreen_y - wbd_char_y_size( wm ) ) return wbd_char_x_size( c, wm );

    int sp = y * wm->pscreen_x + x;

    int p = c * 8;

    int transp = wm->cur_transparency;
    int pscreen_x = wm->pscreen_x;
    COLORPTR g_screen = wm->g_screen;
    COLOR cur_font_color = wm->cur_font_color;
    COLOR cur_font_bgcolor = wm->cur_font_bgcolor;
    int cur_font_draw_bgcolor = wm->cur_font_draw_bgcolor;

    uchar invert_mask = 0;
    if( transp >= 255 )
    {
	if( wm->cur_font_invert ) invert_mask = 0xFF;
	if( wm->cur_font_zoom < 512 )
	{
	    for( int cy = 0; cy < 8; cy++ )
	    {
		int fpart = font8x8[ p ] ^ invert_mask;
		for( int cx = 0; cx < 8; cx++ )
		{
		    if( fpart & 128 ) g_screen[ sp ] = cur_font_color;
			else if( cur_font_draw_bgcolor ) g_screen[ sp ] = cur_font_bgcolor;
		    fpart <<= 1;
		    sp++;
		}
		sp += pscreen_x - 8;
		p++;
	    }
	}
	else
	{
	    for( int cy = 0; cy < 8; cy++ )
	    {
		int fpart = font8x8[ p ] ^ invert_mask;
		for( int cx = 0; cx < 8; cx++ )
		{
		    if( fpart & 128 ) 
		    {
			g_screen[ sp ] = cur_font_color;
			g_screen[ sp+1 ] = cur_font_color;
			g_screen[ sp+pscreen_x ] = cur_font_color;
			g_screen[ sp+pscreen_x+1 ] = cur_font_color;
		    }
		    else if( cur_font_draw_bgcolor ) 
		    {
			g_screen[ sp ] = cur_font_bgcolor;
			g_screen[ sp+1 ] = cur_font_bgcolor;
			g_screen[ sp+pscreen_x ] = cur_font_bgcolor;
			g_screen[ sp+pscreen_x+1 ] = cur_font_bgcolor;
		    }
		    fpart <<= 1;
		    sp += 2;
		}
		sp += pscreen_x*2 - 16;
		p++;
	    }
	}
    }
    else
    {
	if( wm->cur_font_invert ) invert_mask = 0xFF;
	if( wm->cur_font_zoom < 512 )
	{
	    for( int cy = 0; cy < 8; cy++ )
	    {
		int fpart = font8x8[ p ] ^ invert_mask;
		for( int cx = 0; cx < 8; cx++ )
		{
		    if( fpart & 128 ) g_screen[ sp ] = blend( g_screen[ sp ], cur_font_color, transp );
		    else if( cur_font_draw_bgcolor ) g_screen[ sp ] = blend( g_screen[ sp ], cur_font_bgcolor, transp );
		    fpart <<= 1;
		    sp++;
		}
		sp += pscreen_x - 8;
		p++;
	    }
	}
	else
	{
	    for( int cy = 0; cy < 8; cy++ )
	    {
		int fpart = font8x8[ p ] ^ invert_mask;
		for( int cx = 0; cx < 8; cx++ )
		{
		    if( fpart & 128 ) 
		    {
			g_screen[ sp ] = blend( g_screen[ sp ], cur_font_color, transp );
			g_screen[ sp+1 ] = blend( g_screen[ sp+1 ], cur_font_color, transp );
			g_screen[ sp+pscreen_x ] = blend( g_screen[ sp+pscreen_x ], cur_font_color, transp );
			g_screen[ sp+pscreen_x+1 ] = blend( g_screen[ sp+pscreen_x+1 ], cur_font_color, transp );
		    }
		    else if( cur_font_draw_bgcolor ) 
		    {
			g_screen[ sp ] = blend( g_screen[ sp ], cur_font_bgcolor, transp );
			g_screen[ sp+1 ] = blend( g_screen[ sp+1 ], cur_font_bgcolor, transp );
			g_screen[ sp+pscreen_x ] = blend( g_screen[ sp+pscreen_x ], cur_font_bgcolor, transp );
			g_screen[ sp+pscreen_x+1 ] = blend( g_screen[ sp+pscreen_x+1 ], cur_font_bgcolor, transp );
		    }
		    fpart <<= 1;
		    sp += 2;
		}
		sp += pscreen_x*2 - 16;
		p++;
	    }
	}
    }
    return wbd_char_x_size( c, wm );
}

void draw_string( char *str, int x, int y, window_manager *wm )
{
    //Bounds control:
    if( y < 0 ) return;
    if( x > wm->pscreen_x ) return;

    int start_x = x;

    int p = 0;
    while( str[ p ] )
    {
	if( str[ p ] == 0xA ) 
	{ 
	    y += wbd_char_y_size( wm ); x = start_x; 
	}
	else
	{
	    if( str[ p ] != 0xD ) 
		draw_char( (unsigned char)str[ p ], x, y, wm );
	    x += wbd_char_x_size( (unsigned char)str[ p ], wm );
	}
	p++;
    }
}

void draw_pixel( int x, int y, COLOR color, window_manager *wm )
{
    if( x >= 0 && x < wm->pscreen_x &&
	y >= 0 && y < wm->pscreen_y )
    {
	int ptr = y * wm->pscreen_x + x;
	if( wm->cur_transparency >= 255 )
	    wm->g_screen[ ptr ] = color;
	else
	    wm->g_screen[ ptr ] = blend( wm->g_screen[ ptr ], color, wm->cur_transparency );
    }
}

int wbd_char_x_size( uchar c, window_manager *wm )
{
    return ( 8 * wm->cur_font_zoom ) / 256;
}

int wbd_char_y_size( window_manager *wm )
{
    return ( 8 * wm->cur_font_zoom ) / 256;
}

int wbd_string_size( char *str, window_manager *wm )
{
    int size = 0;
    int p = 0;
    while( str[ p ] )
    {
	size += wbd_char_x_size( (unsigned char)str[ p ], wm );
	p++;
    }    
    return size;
}
