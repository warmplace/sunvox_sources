/*
    wm_wince.h. Platform-dependent module : WindowsCE
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#ifndef __WINMANAGER_WINCE__
#define __WINMANAGER_WINCE__

#include <windows.h>
#include <wingdi.h>
#include "wince_res.h" //(IDI_ICON1) Must be defined in your project

WCHAR *className = L"SunDogEngine";
char *windowName = "SunDogEngine_winCE";
HGLRC hGLRC;
WNDCLASS wndClass;
HWND hWnd = 0;
int win_flags = 0;

HANDLE systemIdleTimerThread = 0;

int shift_status = 0;
int ctrl_status = 0;
int alt_status = 0;
int resulted_status = 0;
int resulted_key = 0;

window_manager *current_wm;

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

void SetupPixelFormat(HDC hDC);
LRESULT APIENTRY WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int Win32CreateWindow(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow);
void CreateButtonsTable( window_manager *wm );

#ifdef FRAMEBUFFER
    COLORPTR framebuffer = 0;
#endif

#ifdef DIRECTDRAW
    #include "gx_loader.h"
    tGXOpenDisplay GXOpenDisplay = 0;
    tGXCloseDisplay GXCloseDisplay = 0;
    tGXBeginDraw GXBeginDraw = 0;
    tGXEndDraw GXEndDraw = 0;
    tGXOpenInput GXOpenInput = 0;
    tGXCloseInput GXCloseInput = 0;
    tGXGetDisplayProperties GXGetDisplayProperties = 0;
    tGXGetDefaultKeys GXGetDefaultKeys = 0;
    tGXSuspend GXSuspend = 0;
    tGXResume GXResume = 0;
    tGXSetViewport GXSetViewport = 0;
    GXDisplayProperties gx_dp;
#endif

typedef void (*tSystemIdleTimerReset)( void );
tSystemIdleTimerReset SD_SystemIdleTimerReset = 0;

DWORD WINAPI SystemIdleTimerProc( LPVOID lpParameter )
{
    while( 1 )
    {
	Sleep( 1000 * 10 );
	if( SD_SystemIdleTimerReset )
	    SD_SystemIdleTimerReset();
    }
    ExitThread( 0 );
    return 0;
}

int device_start( char *windowname, int xsize, int ysize, int flags, window_manager *wm )
{
    int retval = 0;

    if( windowname ) windowName = windowname;
    current_wm = wm;
    win_flags = flags;
	
    //Get OS version:
    OSVERSIONINFO ver;
    GetVersionEx( &ver );
    wm->os_version = ver.dwMajorVersion;

#ifdef GDI
    wm->gdi_bitmap_info[ 0 ] = 888;
    if( get_option( OPT_SCREENX ) != -1 ) xsize = get_option( OPT_SCREENX );
    if( get_option( OPT_SCREENY ) != -1 ) ysize = get_option( OPT_SCREENY );
    if( xsize > GetSystemMetrics( SM_CXSCREEN ) ) xsize = GetSystemMetrics( SM_CXSCREEN );
    if( ysize > GetSystemMetrics( SM_CYSCREEN ) - 64 ) ysize = GetSystemMetrics( SM_CYSCREEN ) - 64;
#endif

#ifdef DIRECTDRAW
    xsize = GetSystemMetrics( SM_CXSCREEN );
    ysize = GetSystemMetrics( SM_CYSCREEN );
#endif

    wm->screen_xsize = xsize;
    wm->screen_ysize = ysize;

#ifdef DIRECTDRAW
    HMODULE gapiLibrary;
    FARPROC proc;
    gapiLibrary = LoadLibrary( TEXT( "gx.dll" ) );
    if( gapiLibrary == 0 ) 
    {
	MessageBox( hWnd, L"GX not found", L"Error", MB_OK );
	dprint( "ERROR: GX.dll not found\n" );
	return 1;
    }
    IMPORT( gapiLibrary, proc, tGXOpenDisplay, "?GXOpenDisplay@@YAHPAUHWND__@@K@Z", GXOpenDisplay );
    IMPORT( gapiLibrary, proc, tGXGetDisplayProperties, "?GXGetDisplayProperties@@YA?AUGXDisplayProperties@@XZ", GXGetDisplayProperties );
    IMPORT( gapiLibrary, proc, tGXOpenInput,"?GXOpenInput@@YAHXZ", GXOpenInput );
    IMPORT( gapiLibrary, proc, tGXCloseDisplay, "?GXCloseDisplay@@YAHXZ", GXCloseDisplay );
    IMPORT( gapiLibrary, proc, tGXBeginDraw, "?GXBeginDraw@@YAPAXXZ", GXBeginDraw );
    IMPORT( gapiLibrary, proc, tGXEndDraw, "?GXEndDraw@@YAHXZ", GXEndDraw );
    IMPORT( gapiLibrary, proc, tGXCloseInput,"?GXCloseInput@@YAHXZ", GXCloseInput );
    IMPORT( gapiLibrary, proc, tGXSetViewport,"?GXSetViewport@@YAHKKKK@Z", GXSetViewport );
    IMPORT( gapiLibrary, proc, tGXSuspend, "?GXSuspend@@YAHXZ", GXSuspend );
    IMPORT( gapiLibrary, proc, tGXResume, "?GXResume@@YAHXZ", GXResume );
    if( GXOpenDisplay == 0 ) retval = 1;
    if( GXGetDisplayProperties == 0 ) retval = 2;
    if( GXOpenInput == 0 ) retval = 3;
    if( GXCloseDisplay == 0 ) retval = 4;
    if( GXBeginDraw == 0 ) retval = 5;
    if( GXEndDraw == 0 ) retval = 6;
    if( GXCloseInput == 0 ) retval = 7;
    if( GXSuspend == 0 ) retval = 8;
    if( GXResume == 0 ) retval = 9;
    if( retval )
    {
	MessageBox( hWnd, L"Some GX functions not found", L"Error", MB_OK );
	dprint( "ERROR: some GX functions not found (%d)\n", retval );
	return retval;
    }
#endif

    CreateButtonsTable( wm );
    Win32CreateWindow( wm->hCurrentInst, wm->hPreviousInst, (char*)wm->lpszCmdLine, wm->nCmdShow ); //create main window

#ifdef DIRECTDRAW
    wm->gx_suspended = 0;
    if( GXOpenDisplay( hWnd, GX_FULLSCREEN ) == 0 )
    {
	MessageBox( hWnd, L"Can't open GAPI display", L"Error", MB_OK );
	dprint( "ERROR: Can't open GAPI display\n" );
	return 1;
    }
    else
    {
	gx_dp = GXGetDisplayProperties();
	dprint( "Width: %d\n", gx_dp.cxWidth );
	dprint( "Height: %d\n", gx_dp.cyHeight );
	dprint( "cbxPitch: %d\n", gx_dp.cbxPitch );
	dprint( "cbyPitch: %d\n", gx_dp.cbyPitch );
	dprint( "cBPP: %d\n", gx_dp.cBPP );
	if( gx_dp.ffFormat & kfLandscape ) dprint( "kfLandscape\n" );
	if( gx_dp.ffFormat & kfPalette ) dprint( "kfPalette\n" );
	if( gx_dp.ffFormat & kfDirect ) dprint( "kfDirect\n" );
	if( gx_dp.ffFormat & kfDirect555 ) dprint( "kfDirect555\n" );
	if( gx_dp.ffFormat & kfDirect565 ) dprint( "kfDirect565\n" );
	if( gx_dp.ffFormat & kfDirect888 ) dprint( "kfDirect888\n" );
	if( gx_dp.ffFormat & kfDirect444 ) dprint( "kfDirect444\n" );
	if( gx_dp.ffFormat & kfDirectInverted ) dprint( "kfDirectInverted\n" );
	wm->fb_xpitch = gx_dp.cbxPitch;
	wm->fb_ypitch = gx_dp.cbyPitch;
	wm->fb_xpitch /= COLORLEN;
	wm->fb_ypitch /= COLORLEN;
	//LANDSCAPE MODE TESTING ***
	//gx_dp.ffFormat |= kfLandscape;
	//int ttt = wm->screen_xsize; wm->screen_xsize = wm->screen_ysize; wm->screen_ysize = ttt;
	//**************************
    	GXSetViewport( 0, gx_dp.cyHeight, 0, 0 );
	GXOpenInput();
    }
#endif

    //Create framebuffer:
#ifndef DIRECTDRAW
#ifdef FRAMEBUFFER
    framebuffer = (COLORPTR)MEM_NEW( HEAP_DYNAMIC, wm->screen_xsize * wm->screen_ysize * COLORLEN );
    wm->fb_xpitch = 1;
    wm->fb_ypitch = wm->screen_xsize;
#endif
#endif

    HMODULE coreLibrary;
    coreLibrary = LoadLibrary( TEXT( "coredll.dll" ) );
    if( coreLibrary == 0 ) 
    {
	dprint( "ERROR: coredll.dll not found\n" );
    }
    else
    {
	IMPORT( coreLibrary, proc, tSystemIdleTimerReset, "SystemIdleTimerReset", SD_SystemIdleTimerReset );
	if( SD_SystemIdleTimerReset == 0 )
	    dprint( "SystemIdleTimerReset() not found\n" );
    }
    systemIdleTimerThread = (HANDLE)CreateThread( NULL, 0, SystemIdleTimerProc, NULL, 0, NULL );
    
    return retval;
}

void device_end( window_manager *wm )
{
    if( systemIdleTimerThread )
	CloseHandle( systemIdleTimerThread );

#ifdef GDI
#endif

#ifdef FRAMEBUFFER
#ifndef DIRECTDRAW
    mem_free( framebuffer );
#endif
#endif

    DestroyWindow( hWnd );
    Sleep( 500 );
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
    return 0;
}

void SetupPixelFormat( HDC hDC )
{
    if( hDC == 0 ) return;
}

#ifdef GDI
#define GET_WINDOW_COORDINATES \
    /*Real coordinates -> window_manager coordinates*/\
    x = lParam & 0xFFFF;\
    y = lParam>>16;
#endif

#ifdef DIRECTDRAW
#define GET_WINDOW_COORDINATES \
    /*Real coordinates -> window_manager coordinates*/\
    x = lParam & 0xFFFF;\
    y = lParam>>16;
    /*if( gx_dp.ffFormat & kfLandscape ) { \
	int ttt = x; \
	x = ( current_wm->screen_xsize - 1 ) - y;\
	y = ttt; \
    }*/
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

    switch( message ) 
    {
	case WM_CREATE:
	    break;

	case WM_DESTROY:
	    current_wm->exit_request = 1;
#ifdef DIRECTDRAW
	    GXCloseInput();
	    GXCloseDisplay();
#endif
	    PostQuitMessage( 0 );
	    break;

	case WM_SIZE:
	    current_wm->screen_xsize = (int) LOWORD(lParam);
	    current_wm->screen_ysize = (int) HIWORD(lParam);
	    send_event( current_wm->root_win, EVT_SCREENRESIZE, 1, 0, 0, 0, 0, 1023, current_wm );
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

	case WM_DEVICECHANGE:
	    if( wParam == 0x8000 ) //DBT_DEVICEARRIVAL
	    {
		//It may be device power ON:
		send_event( current_wm->root_win, EVT_DRAW, 1, 0, 0, 0, 0, 1023, current_wm );
	    }
	    break;

#ifdef DIRECTDRAW
	case WM_KILLFOCUS:
	    if( current_wm->screen_lock_counter > 0 )
		GXEndDraw();
	    GXSuspend();
	    current_wm->gx_suspended = 1;
	    send_event( current_wm->root_win, EVT_SCREENUNFOCUS, 1, 0, 0, 0, 0, 1023, current_wm );
	    break;
	case WM_SETFOCUS:
	    if( current_wm->gx_suspended )
	    {
		if( current_wm->screen_lock_counter > 0 )
		{
		    framebuffer = (COLORPTR)GXBeginDraw();
		}
		GXResume();
    		current_wm->gx_suspended = 0;
	    }
	    send_event( current_wm->root_win, EVT_SCREENFOCUS, 1, 0, 0, 0, 0, 1023, current_wm );
	    send_event( current_wm->root_win, EVT_DRAW, 1, 0, 0, 0, 0, 1023, current_wm );
	    break;
#endif

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
	    switch(wParam & ~(MK_SHIFT+MK_CONTROL)) {
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
	    /*
	    unsigned short tbuf[ 32 ];
	    swprintf( tbuf, c2w( "KEY: %x" ), (int)wParam ); 
	    RECT rr;
	    rr.top = 0;
	    rr.left = 0;
	    rr.right = 128;
	    rr.bottom = 32;
	    DrawText( hDC, tbuf, 16, &rr, 0 );
	    */
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
	    /*
	    swprintf( tbuf, c2w( "KEY: %x" ), (int)wParam ); 
	    rr.top = 0;
	    rr.left = 0;
	    rr.right = 128;
	    rr.bottom = 32;
	    DrawText( hDC, tbuf, 16, &rr, 0 );
	    */
	    if( resulted_key ) 
	    {
	        resulted_key |= resulted_status;
	        send_event( 0, EVT_BUTTONUP, 0, 0, 0, 0, resulted_key, 1023, current_wm );
	    }
	    break;
	default:
	    return DefWindowProc( hWnd, message, wParam, lParam );
	    break;
    }
    return 0;
}

int Win32CreateWindow(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow)
{
    /* register window class */
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hCurrentInst;
    wndClass.hIcon = LoadIcon( hCurrentInst, MAKEINTRESOURCE(IDI_ICON1) );
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
#ifdef GDI
    AdjustWindowRectEx( &Rect, WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 0, 0 );
    if( win_flags & WIN_INIT_FLAG_SCALABLE )
    {
	hWnd = CreateWindow(
	    className, c2w( windowName ),
	    WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
	    ( GetSystemMetrics(SM_CXSCREEN) - (Rect.right - Rect.left) ) / 2, 
	    ( GetSystemMetrics(SM_CYSCREEN) - (Rect.bottom - Rect.top) ) / 2, 
	    Rect.right - Rect.left, Rect.bottom - Rect.top,
	    NULL, NULL, hCurrentInst, NULL
	);
    }
    else
    {
	hWnd = CreateWindow(
	    className, c2w( windowName ),
	    WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
	    ( GetSystemMetrics(SM_CXSCREEN) - (Rect.right - Rect.left) ) / 2, 
	    ( GetSystemMetrics(SM_CYSCREEN) - (Rect.bottom - Rect.top) ) / 2, 
	    Rect.right - Rect.left, Rect.bottom - Rect.top,
	    NULL, NULL, hCurrentInst, NULL
	);
    }
#endif //GDI
#ifdef DIRECTDRAW
    hWnd = CreateWindow(
	className, c2w( windowName ),
	WS_VISIBLE,
	0, 0,
	GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
	NULL, NULL, hCurrentInst, NULL
    );
#endif //DIRECTDRAW
    /* display window */
    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    current_wm->hdc = GetDC( hWnd );

    return 0;
}

#define VK_APP_LAUNCH1       0xC1
#define VK_APP_LAUNCH2       0xC2
#define VK_APP_LAUNCH3       0xC3
#define VK_APP_LAUNCH4       0xC4
#define VK_APP_LAUNCH5       0xC5
#define VK_APP_LAUNCH6       0xC6
#define VK_APP_LAUNCH7       0xC7
#define VK_APP_LAUNCH8       0xC8
#define VK_APP_LAUNCH9       0xC9
#define VK_APP_LAUNCH10      0xCA
#define VK_APP_LAUNCH11      0xCB
#define VK_APP_LAUNCH12      0xCC
#define VK_APP_LAUNCH13      0xCD
#define VK_APP_LAUNCH14      0xCE
#define VK_APP_LAUNCH15      0xCF

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
    
    wm->buttons_table[ VK_APP_LAUNCH1 ] = 0;//KEY_ESCAPE;
    wm->buttons_table[ VK_APP_LAUNCH2 ] = 0;//KEY_ESCAPE;
    wm->buttons_table[ VK_APP_LAUNCH3 ] = 0;//KEY_ESCAPE;
    wm->buttons_table[ VK_APP_LAUNCH4 ] = 0;//KEY_ESCAPE;
    
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
    if( wm->screen_lock_counter == 1 && wm->gx_suspended == 0 )
    {
#ifdef DIRECTDRAW
	if( framebuffer )
	{
	    GXEndDraw();
	    framebuffer = 0;
	}
#endif //DIRECTDRAW
    }

    if( wm->screen_lock_counter > 0 )
    {
	wm->screen_lock_counter--;
    }

    if( wm->gx_suspended == 0 && wm->screen_lock_counter > 0 )
	wm->screen_is_active = 1;
    else
	wm->screen_is_active = 0;
}

void device_screen_lock( window_manager *wm )
{
    if( wm->screen_lock_counter == 0 && wm->gx_suspended == 0 )
    {
#ifdef DIRECTDRAW
	framebuffer = (COLORPTR)GXBeginDraw();
#endif //DIRECTDRAW
    }
    wm->screen_lock_counter++;
    
    if( wm->gx_suspended == 0 && wm->screen_lock_counter > 0 )
	wm->screen_is_active = 1;
    else
	wm->screen_is_active = 0;
}

#ifdef FRAMEBUFFER

#include "wm_framebuffer.h"

#else

void device_redraw_framebuffer( window_manager *wm ) {}	

void device_draw_line( int x1, int y1, int x2, int y2, COLOR color, window_manager *wm )
{
    if( wm->hdc == 0 ) return;
    HPEN pen;
    pen = CreatePen( PS_SOLID, 1, RGB( red( color ), green( color ), blue( color ) ) );
    SelectObject( wm->hdc, pen );
    MoveToEx( wm->hdc, x1, y1, 0 );
    LineTo( wm->hdc, x2, y2 );
    SetPixel( wm->hdc, x2, y2, RGB( red( color ), green( color ), blue( color ) ) );
    DeleteObject( pen );
}

void device_draw_box( int x, int y, int xsize, int ysize, COLOR color, window_manager *wm )
{
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
}

void device_draw_bitmap( 
    int dest_x, int dest_y, 
    int dest_xs, int dest_ys,
    int src_x, int src_y,
    int src_xs, int src_ys,
    COLOR *data,
    window_manager *wm )
{
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
    int new_src_y;
    if( wm->os_version > 4 )
    {
	new_src_y = src_ys - ( src_y + dest_ys );
    }
    else
    {
	new_src_y = src_y;
    }
    SetDIBitsToDevice(  wm->hdc,
			dest_x,		// Destination top left hand corner X Position
			dest_y,		// Destination top left hand corner Y Position
			dest_xs,	// Destinations Width
			dest_ys,	// Desitnations height
			src_x,          // Source low left hand corner's X Position
			new_src_y,      // Source low left hand corner's Y Position
			0,
			src_ys,
			data,		// Source's data
			(BITMAPINFO*)wm->gdi_bitmap_info, // Bitmap Info
			DIB_RGB_COLORS );
}

#endif

//#################################
//#################################
//#################################

#endif
