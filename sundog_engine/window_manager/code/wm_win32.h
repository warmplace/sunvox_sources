/*
    wm_win32.h. Platform-dependent module : Win32
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#ifndef __WINMANAGER_WIN32__
#define __WINMANAGER_WIN32__

#include <windows.h>
#include "win_res.h" //(IDI_ICON1) Must be in your project

char *className = "SunDogEngine";
char *windowName = "SunDogEngine_win32";
HGLRC hGLRC;
WNDCLASS wndClass;
HWND hWnd = 0;
int win_flags = 0;

int shift_status = 0;
int ctrl_status = 0;
int alt_status = 0;
int resulted_status = 0;
int resulted_key = 0;

#if defined(OPENGL) || defined(OPENGLCOMP)
    #include <GL/gl.h>
    #include "wm_opengl.h"
#endif

window_manager *current_wm;

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

void SetupPixelFormat(HDC hDC);
LRESULT APIENTRY WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int Win32CreateWindow(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow);
void CreateButtonsTable( window_manager *wm );
#ifdef DIRECTDRAW
    #include "wm_directx.h"
#endif

#ifdef FRAMEBUFFER
    COLORPTR framebuffer = 0;
#endif

int device_start( char *windowname, int xsize, int ysize, int flags, window_manager *wm )
{
    int retval = 0;

    if( windowname ) windowName = windowname;
    current_wm = wm;
    win_flags = flags;
	
#ifdef GDI
    wm->gdi_bitmap_info[ 0 ] = 888;
    if( profile_get_int_value( KEY_SCREENX, 0 ) != -1 ) xsize = profile_get_int_value( KEY_SCREENX, 0 );
    if( profile_get_int_value( KEY_SCREENY, 0 ) != -1 ) ysize = profile_get_int_value( KEY_SCREENY, 0 );
#endif
#ifdef DIRECTDRAW
    if( profile_get_int_value( KEY_SCREENX, 0 ) != -1 ) xsize = profile_get_int_value( KEY_SCREENX, 0 );
    if( profile_get_int_value( KEY_SCREENY, 0 ) != -1 ) ysize = profile_get_int_value( KEY_SCREENY, 0 );
    fix_fullscreen_resolution( &xsize, &ysize, wm );
#endif
#ifdef OPENGL
    wm->real_window_width = xsize = 512;
    wm->real_window_height = ysize = 512;
#endif
#ifdef OPENGLCOMP
    if( profile_get_int_value( KEY_SCREENX, 0 ) != -1 ) xsize = profile_get_int_value( KEY_SCREENX, 0 );
    if( profile_get_int_value( KEY_SCREENY, 0 ) != -1 ) ysize = profile_get_int_value( KEY_SCREENY, 0 );
#endif

    wm->screen_xsize = xsize;
    wm->screen_ysize = ysize;

    CreateButtonsTable( wm );
    if( Win32CreateWindow( wm->hCurrentInst, wm->hPreviousInst, wm->lpszCmdLine, wm->nCmdShow ) ) //create main window
	return 1; //Error

    //Create framebuffer:
#ifdef FRAMEBUFFER
    #ifdef DIRECTDRAW
	framebuffer = 0;
    #else
	framebuffer = (COLORPTR)MEM_NEW( HEAP_DYNAMIC, wm->screen_xsize * wm->screen_ysize * COLORLEN );
	wm->fb_xpitch = 1;
	wm->fb_ypitch = wm->screen_xsize;
    #endif
#endif

    return retval;
}

void device_end( window_manager *wm )
{
#ifdef GDI
#endif

#ifdef DIRECTX
    dd_close();
#endif

#ifdef FRAMEBUFFER
#ifndef DIRECTDRAW
    mem_free( framebuffer );
#endif
#endif

    DestroyWindow( hWnd );
}

long device_event_handler( window_manager *wm )
{
    MSG msg;
    while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
    {
	if( GetMessage( &msg, NULL, 0, 0 ) == 0 ) return 1; //Exit
	TranslateMessage( &msg );
	DispatchMessage( &msg );
    }
    Sleep( 1 );
    /*if( GetMessage( &msg, NULL, 0, 0 ) ) 
    {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    } else return 1;*/

    return 0;
}

void SetupPixelFormat( HDC hDC )
{
    if( hDC == 0 ) return;

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  /* size */
        1,                              /* version */
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,               /* support double-buffering */
        PFD_TYPE_RGBA,                  /* color type */
        16,                             /* prefered color depth */
        0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
        0,                              /* no alpha buffer */
        0,                              /* alpha bits (ignored) */
        0,                              /* no accumulation buffer */
        0, 0, 0, 0,                     /* accum bits (ignored) */
        16,                             /* depth buffer */
        0,                              /* no stencil buffer */
        0,                              /* no auxiliary buffers */
        PFD_MAIN_PLANE,                 /* main layer */
        0,                              /* reserved */
        0, 0, 0,                        /* no layer, visible, damage masks */
    };
    int pixelFormat;

    pixelFormat = ChoosePixelFormat( hDC, &pfd );
    if( pixelFormat == 0 ) 
    {
        MessageBox(WindowFromDC(hDC), "ChoosePixelFormat failed.", "Error",
                MB_ICONERROR | MB_OK);
        exit(1);
    }

    if( SetPixelFormat( hDC, pixelFormat, &pfd ) != TRUE ) 
    {
        MessageBox(WindowFromDC(hDC), "SetPixelFormat failed.", "Error",
                MB_ICONERROR | MB_OK);
        exit(1);
    }
}

#ifdef OPENGL
#define GET_WINDOW_COORDINATES \
    /*Real coordinates -> window_manager coordinates*/\
    x = lParam & 0xFFFF;\
    y = lParam>>16;\
    x = ( x << 11 ) / current_wm->real_window_width; x *= current_wm->screen_xsize; x >>= 11;\
    y = ( y << 11 ) / current_wm->real_window_height; y *= current_wm->screen_ysize; y >>= 11;
#else
#define GET_WINDOW_COORDINATES \
    /*Real coordinates -> window_manager coordinates*/\
    x = lParam & 0xFFFF;\
    y = lParam>>16;
#endif

LRESULT APIENTRY
WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    long x, y, d;
    short d2;
    POINT point;

    switch (message) 
    {
	case WM_CREATE:
	    break;

	case WM_DESTROY:
	    current_wm->exit_request = 1;
	    PostQuitMessage(0);
	    break;

	case WM_SIZE:
#ifdef OPENGL
	    /* track window size changes */
	    if( hGLRC ) 
	    {
		current_wm->real_window_width = (int) LOWORD(lParam);
		current_wm->real_window_height = (int) HIWORD(lParam);
                gl_resize();
		return 0;
	    }
#endif
#ifdef OPENGLCOMP
	    /* track window size changes */
	    if( hGLRC ) 
	    {
		current_wm->real_window_width = (int) LOWORD(lParam);
		current_wm->real_window_height = (int) HIWORD(lParam);
		current_wm->screen_xsize = (int) LOWORD(lParam);
		current_wm->screen_ysize = (int) HIWORD(lParam);
		send_event( current_wm->root_win, EVT_SCREENRESIZE, 1, 0, 0, 0, 0, 1023, current_wm );
                gl_resize();
		return 0;
	    }
#endif
#ifdef GDI
	    current_wm->screen_xsize = (int) LOWORD(lParam);
	    current_wm->screen_ysize = (int) HIWORD(lParam);
	    send_event( current_wm->root_win, EVT_SCREENRESIZE, 1, 0, 0, 0, 0, 1023, current_wm );
#endif
	    break;

	case WM_PAINT:
	    {
		PAINTSTRUCT gdi_ps;
		BeginPaint( hWnd, &gdi_ps );
		EndPaint( hWnd, &gdi_ps );
		send_event( current_wm->root_win, EVT_DRAW, 1, 0, 0, 0, 0, 1023, current_wm );
		return 0;
	    }
	    break;
	case 0x020A: //WM_MOUSEWHEEL
	    GET_WINDOW_COORDINATES;
	    point.x = x;
	    point.y = y;
	    ScreenToClient( hWnd, &point );
	    x = point.x;
	    y = point.y;
	    resulted_key = 0;
	    d = (unsigned long)wParam >> 16;
	    d2 = (short)d;
	    if( d2 < 0 ) resulted_key = BUTTON_SCROLLDOWN;
	    if( d2 > 0 ) resulted_key = BUTTON_SCROLLUP;
	    send_event( 0,
			EVT_MOUSEBUTTONDOWN,
			0,
			x, 
			y, 
			resulted_key,
			0,
			1023, 
			current_wm );
	    break;
	case WM_LBUTTONDOWN:
	    GET_WINDOW_COORDINATES;
	    send_event( 0, EVT_MOUSEBUTTONDOWN, 0, x, y, BUTTON_LEFT, resulted_status, 1023, current_wm );
	    break;
	case WM_LBUTTONUP:
	    GET_WINDOW_COORDINATES;
	    send_event( 0, EVT_MOUSEBUTTONUP, 0, x, y, BUTTON_LEFT, resulted_status, 1023, current_wm );
	    break;
	case WM_MBUTTONDOWN:
	    GET_WINDOW_COORDINATES;
	    send_event( 0, EVT_MOUSEBUTTONDOWN, 0, x, y, BUTTON_MIDDLE, resulted_status, 1023, current_wm );
	    break;
	case WM_MBUTTONUP:
	    GET_WINDOW_COORDINATES;
	    send_event( 0, EVT_MOUSEBUTTONUP, 0, x, y, BUTTON_MIDDLE, resulted_status, 1023, current_wm );
	    break;
	case WM_RBUTTONDOWN:
	    GET_WINDOW_COORDINATES;
	    send_event( 0, EVT_MOUSEBUTTONDOWN, 0, x, y, BUTTON_RIGHT, resulted_status, 1023, current_wm );
	    break;
	case WM_RBUTTONUP:
	    GET_WINDOW_COORDINATES;
	    send_event( 0, EVT_MOUSEBUTTONUP, 0, x, y, BUTTON_RIGHT, resulted_status, 1023, current_wm );
	    break;
	case WM_MOUSEMOVE:
	    GET_WINDOW_COORDINATES;
	    switch(wParam & ~(MK_SHIFT+MK_CONTROL)) 
	    {
		case MK_LBUTTON:
		    send_event( 0, EVT_MOUSEMOVE, 0, x, y, BUTTON_LEFT, resulted_status, 1023, current_wm );
		    break;
		case MK_MBUTTON:
		    send_event( 0, EVT_MOUSEMOVE, 0, x, y, BUTTON_MIDDLE, resulted_status, 1023, current_wm );
		    break;
		case MK_RBUTTON:
		    send_event( 0, EVT_MOUSEMOVE, 0, x, y, BUTTON_RIGHT, resulted_status, 1023, current_wm );
		    break;
	    }
	    break;
	case WM_KEYDOWN:
	    if( (int)wParam == VK_SHIFT )
		    shift_status = KEY_SHIFT;
	    if( (int)wParam == VK_CONTROL )
		    ctrl_status = KEY_CTRL;
	    //if( (int)wParam == VK_ALT )
	    //	shift_status = KEY_ALT;
	    resulted_status = shift_status | ctrl_status | alt_status;
	    resulted_key = current_wm->buttons_table[ (int)wParam & 255 ];
	    if( resulted_key ) 
	    {
		    resulted_key |= resulted_status;
		    send_event( 0, EVT_BUTTONDOWN, 0, 0, 0, 0, resulted_key, 1023, current_wm );
	    }
	    break;
	case WM_KEYUP:
	    if( (int)wParam == VK_SHIFT )
		    shift_status = 0;
	    if( (int)wParam == VK_CONTROL )
		    ctrl_status = 0;
	    //if( (int)wParam == VK_ALT )
	    //	shift_status = 0;
	    resulted_status = shift_status | ctrl_status | alt_status;
	    resulted_key = current_wm->buttons_table[ (int)wParam & 255 ];
	    if( resulted_key ) 
	    {
		    resulted_key |= resulted_status;
		    send_event( 0, EVT_BUTTONUP, 0, 0, 0, 0, resulted_key, 1023, current_wm );
	    }
	    break;

	default:
	    return DefWindowProc(hWnd, message, wParam, lParam);
	    break;
    }

    return 0;
}

int Win32CreateWindow(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow)
{
    /* register window class */
#ifdef DIRECTDRAW
    wndClass.style = CS_BYTEALIGNCLIENT;
#else
    wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
#endif
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hCurrentInst;
    wndClass.hIcon = LoadIcon(hCurrentInst, (LPCTSTR)IDI_ICON1);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = className;
    RegisterClass(&wndClass);

    /* create window */
    RECT Rect;
    Rect.top = 0;
    Rect.bottom = current_wm->screen_ysize;
    Rect.left = 0;
    Rect.right = current_wm->screen_xsize;
    ulong WS = 0;
    ulong WS_EX = 0;
    if( win_flags & WIN_INIT_FLAG_SCALABLE )
	WS = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    else
	WS = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
    if( win_flags & WIN_INIT_FLAG_NOBORDER )
	WS = WS_POPUP;
#ifdef DIRECTDRAW
    int fullscreen = 1;
    //if( profile_get_int_value( KEY_FULLSCREEN, 0 ) != -1 ) fullscreen = 1;
    if( fullscreen ) 
    {
	WS = WS_POPUP;
	WS_EX = WS_EX_TOPMOST;
    }
#endif
    AdjustWindowRect( &Rect, WS, 0 );
    hWnd = CreateWindowEx(
	WS_EX,
        className, windowName,
        WS,
        ( GetSystemMetrics(SM_CXSCREEN) - (Rect.right - Rect.left) ) / 2, 
        ( GetSystemMetrics(SM_CYSCREEN) - (Rect.bottom - Rect.top) ) / 2, 
        Rect.right - Rect.left, Rect.bottom - Rect.top,
        NULL, NULL, hCurrentInst, NULL
    );
    /* display window */
    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    current_wm->hdc = GetDC( hWnd );

#ifdef OPENGL
    /* initialize OpenGL rendering */
    SetupPixelFormat( current_wm->hdc );
    hGLRC = wglCreateContext( current_wm->hdc );
    wglMakeCurrent( current_wm->hdc, hGLRC );
    gl_init();
    gl_resize();
#endif
#ifdef OPENGLCOMP
    /* initialize OpenGL compositor */
    SetupPixelFormat( current_wm->hdc );
    hGLRC = wglCreateContext( current_wm->hdc );
    wglMakeCurrent( current_wm->hdc, hGLRC );
    gl_init();
    gl_resize();
#endif
#ifdef DIRECTDRAW
    if( dd_init( fullscreen ) ) 
	return 1; //Error
#endif

    current_wm->hwnd = hWnd;

    return 0;
}

void CreateButtonsTable( window_manager *wm )
{
    wm->buttons_table[ 0x30 ] = 0x30; // 0 1 2 3 ...
    wm->buttons_table[ 0x31 ] = 0x31;
    wm->buttons_table[ 0x32 ] = 0x32;
    wm->buttons_table[ 0x33 ] = 0x33;
    wm->buttons_table[ 0x34 ] = 0x34;
    wm->buttons_table[ 0x35 ] = 0x35;
    wm->buttons_table[ 0x36 ] = 0x36;
    wm->buttons_table[ 0x37 ] = 0x37;
    wm->buttons_table[ 0x38 ] = 0x38;
    wm->buttons_table[ 0x39 ] = 0x39;

    wm->buttons_table[ 0x41 ] = 0x61; // a b c d e ...
    wm->buttons_table[ 0x42 ] = 0x62;
    wm->buttons_table[ 0x43 ] = 0x63;
    wm->buttons_table[ 0x44 ] = 0x64;
    wm->buttons_table[ 0x45 ] = 0x65;
    wm->buttons_table[ 0x46 ] = 0x66;
    wm->buttons_table[ 0x47 ] = 0x67;
    wm->buttons_table[ 0x48 ] = 0x68;
    wm->buttons_table[ 0x49 ] = 0x69;
    wm->buttons_table[ 0x4A ] = 0x6A;
    wm->buttons_table[ 0x4B ] = 0x6B;
    wm->buttons_table[ 0x4C ] = 0x6C;
    wm->buttons_table[ 0x4D ] = 0x6D;
    wm->buttons_table[ 0x4E ] = 0x6E;
    wm->buttons_table[ 0x4F ] = 0x6F;
    wm->buttons_table[ 0x50 ] = 0x70;
    wm->buttons_table[ 0x51 ] = 0x71;
    wm->buttons_table[ 0x52 ] = 0x72;
    wm->buttons_table[ 0x53 ] = 0x73;
    wm->buttons_table[ 0x54 ] = 0x74;
    wm->buttons_table[ 0x55 ] = 0x75;
    wm->buttons_table[ 0x56 ] = 0x76;
    wm->buttons_table[ 0x57 ] = 0x77;
    wm->buttons_table[ 0x58 ] = 0x78;
    wm->buttons_table[ 0x59 ] = 0x79;
    wm->buttons_table[ 0x5A ] = 0x7A;

    wm->buttons_table[ VK_F1 ] = KEY_F1;
    wm->buttons_table[ VK_F2 ] = KEY_F2;
    wm->buttons_table[ VK_F3 ] = KEY_F3;
    wm->buttons_table[ VK_F4 ] = KEY_F4;
    wm->buttons_table[ VK_F5 ] = KEY_F5;
    wm->buttons_table[ VK_F6 ] = KEY_F6;
    wm->buttons_table[ VK_F7 ] = KEY_F7;
    wm->buttons_table[ VK_F8 ] = KEY_F8;

    wm->buttons_table[ VK_ESCAPE ] = KEY_ESCAPE;
    wm->buttons_table[ VK_SPACE ] = KEY_SPACE;
    wm->buttons_table[ VK_RETURN ] = KEY_ENTER;
    wm->buttons_table[ VK_BACK ] = KEY_BACKSPACE;
    wm->buttons_table[ VK_TAB ] = KEY_TAB;
    wm->buttons_table[ VK_CAPITAL ] = KEY_CAPS;
    wm->buttons_table[ VK_SHIFT ] = 0;
    wm->buttons_table[ VK_CONTROL ] = 0;
    //wm->buttons_table[ VK_ALT ] = 0;
    
    wm->buttons_table[ VK_UP ] = KEY_UP;
    wm->buttons_table[ VK_DOWN ] = KEY_DOWN;
    wm->buttons_table[ VK_LEFT ] = KEY_LEFT;
    wm->buttons_table[ VK_RIGHT ] = KEY_RIGHT;

    wm->buttons_table[ VK_INSERT ] = KEY_INSERT;
    wm->buttons_table[ VK_DELETE ] = KEY_DELETE;
    wm->buttons_table[ VK_HOME ] = KEY_HOME;
    wm->buttons_table[ VK_END ] = KEY_END;
    wm->buttons_table[ 33 ] = KEY_PAGEUP;
    wm->buttons_table[ 34 ] = KEY_PAGEDOWN;

    wm->buttons_table[ 189 ] = '-'; //  -
    wm->buttons_table[ 187 ] = '='; //  =
    wm->buttons_table[ 219 ] = '['; //  [
    wm->buttons_table[ 221 ] = ']'; //  ]
    wm->buttons_table[ 186 ] = ';'; //  ;
    wm->buttons_table[ 222 ] = 0x27; //  '
    wm->buttons_table[ 188 ] = ','; //  ,
    wm->buttons_table[ 190 ] = '.'; //  .
    wm->buttons_table[ 191 ] = '/'; //  /
    wm->buttons_table[ 220 ] = 0x5C; //  |
    wm->buttons_table[ 192 ] = '`'; //  `
}

void device_screen_unlock( window_manager *wm )
{
    if( wm->screen_lock_counter == 1 )
    {
#ifdef DIRECTDRAW
	if( g_primary_locked )
	{
	    lpDDSPrimary->Unlock( g_sd.lpSurface );
	    g_primary_locked = 0;
	    framebuffer = 0;
	}
#endif //DIRECTDRAW
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

void device_screen_lock( window_manager *wm )
{
    if( wm->screen_lock_counter == 0 )
    {
#ifdef DIRECTDRAW
	if( g_primary_locked == 0 )
	{
	    if( lpDDSPrimary )
	    {
		lpDDSPrimary->Lock( 0, &g_sd, DDLOCK_SURFACEMEMORYPTR  | DDLOCK_WAIT, 0 );
		wm->fb_ypitch = g_sd.lPitch / COLORLEN;
		framebuffer = (COLORPTR)g_sd.lpSurface;
		g_primary_locked = 1;
	    }
	}
#endif //DIRECTDRAW
    }
    wm->screen_lock_counter++;
    
    if( wm->screen_lock_counter > 0 ) 
	wm->screen_is_active = 1;
    else
	wm->screen_is_active = 0;
}

#ifdef FRAMEBUFFER

#include "wm_framebuffer.h"

#else

void device_redraw_framebuffer( window_manager *wm ) 
{
#ifdef OPENGLCOMP
    SwapBuffers( wm->hdc );
#endif //OPENGLCOMP
}	

void device_draw_line( int x1, int y1, int x2, int y2, COLOR color, window_manager *wm )
{
#ifdef GDI
    if( wm->hdc == 0 ) return;
    HPEN pen;
    pen = CreatePen( PS_SOLID, 1, RGB( red( color ), green( color ), blue( color ) ) );
    SelectObject( wm->hdc, pen );
    MoveToEx( wm->hdc, x1, y1, 0 );
    LineTo( wm->hdc, x2, y2 );
    SetPixel( wm->hdc, x2, y2, RGB( red( color ), green( color ), blue( color ) ) );
    DeleteObject( pen );
#endif //GDI
}

void device_draw_box( int x, int y, int xsize, int ysize, COLOR color, window_manager *wm )
{
#ifdef GDI
    if( wm->hdc == 0 ) return;
    if( xsize == 1 && ysize == 1 )
    {
	SetPixel( wm->hdc, x, y, RGB( red( color ), green( color ), blue( color ) ) );
    }
    else
    {
	HPEN pen = CreatePen( PS_SOLID, 1, RGB( red( color ), green( color ), blue( color ) ) );
	HBRUSH brush = CreateSolidBrush( RGB( red( color ), green( color ), blue( color ) ) );
	SelectObject( wm->hdc, pen );
	SelectObject( wm->hdc, brush );
	Rectangle( wm->hdc, x, y, x + xsize, y + ysize );
	DeleteObject( brush );
	DeleteObject( pen );
    }
#endif //GDI
}

void device_draw_bitmap( 
    int dest_x, int dest_y, 
    int dest_xs, int dest_ys,
    int src_x, int src_y,
    int src_xs, int src_ys,
    COLOR *data,
    window_manager *wm )
{
#ifdef GDI
    if( wm->hdc == 0 ) return;
    BITMAPINFO *bi = (BITMAPINFO*)wm->gdi_bitmap_info;
    if( wm->gdi_bitmap_info[ 0 ] == 888 )
    {
	memset( bi, 0, sizeof( wm->gdi_bitmap_info ) );
	//Set 256 colors palette:
	int a;
#ifdef GRAYSCALE
	for( a = 0; a < 256; a++ ) 
	{ 
    	    bi->bmiColors[ a ].rgbRed = a; 
    	    bi->bmiColors[ a ].rgbGreen = a; 
    	    bi->bmiColors[ a ].rgbBlue = a; 
	}
#else
	for( a = 0; a < 256; a++ ) 
	{ 
    	    bi->bmiColors[ a ].rgbRed = (a<<5)&224; 
	    if( bi->bmiColors[ a ].rgbRed ) 
		bi->bmiColors[ a ].rgbRed |= 0x1F; 
	    bi->bmiColors[ a ].rgbReserved = 0;
	}
	for( a = 0; a < 256; a++ )
	{
	    bi->bmiColors[ a ].rgbGreen = (a<<2)&224; 
	    if( bi->bmiColors[ a ].rgbGreen ) 
		bi->bmiColors[ a ].rgbGreen |= 0x1F; 
	}
	for( a = 0; a < 256; a++ ) 
	{ 
	    bi->bmiColors[ a ].rgbBlue = (a&192);
	    if( bi->bmiColors[ a ].rgbBlue ) 
		bi->bmiColors[ a ].rgbBlue |= 0x3F; 
	}
#endif
    }
    bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi->bmiHeader.biWidth = src_xs;
    bi->bmiHeader.biHeight = -src_ys;
    bi->bmiHeader.biPlanes = 1;
    bi->bmiHeader.biBitCount = COLORBITS;
    bi->bmiHeader.biCompression = BI_RGB;
    SetDIBitsToDevice(  wm->hdc,
			dest_x,	    // Destination top left hand corner X Position
			dest_y,	    // Destination top left hand corner Y Position
			dest_xs,    // Destinations Width
			dest_ys,    // Desitnations height
			src_x,      // Source low left hand corner's X Position
			src_ys - ( src_y + dest_ys ),	    // Source low left hand corner's Y Position
			0,
			src_ys,
			data,	    // Source's data
			(BITMAPINFO*)wm->gdi_bitmap_info,   // Bitmap Info
			DIB_RGB_COLORS );
#endif //GDI
}

#endif

//#################################
//#################################
//#################################

#endif
