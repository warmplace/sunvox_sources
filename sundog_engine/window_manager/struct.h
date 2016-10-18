#ifndef __WINDOWMANAGER_STRUCT__
#define __WINDOWMANAGER_STRUCT__

#include "../core/core.h"
#include "../memory/memory.h"
#include "../filesystem/v3nus_fs.h"
#include "../time/timemanager.h"
#include "regions/device.h" //Part of the Nano-X project (functions for working with regions)

#define EVENTS	32             //must be 2/4/8/16/32...
#define TIMERS	4

#if defined(WIN) || defined(WINCE)
    #include <windows.h>
#endif

#ifdef UNIX
    #include <pthread.h>
#endif

#ifdef OPENGL
    #include <GL/gl.h>
    #include <GL/glu.h>
#ifdef UNIX
    #include <GL/glx.h>
    #include <X11/keysym.h>
    #include <X11/XKBlib.h>
#endif
#endif

#ifdef X11
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysym.h>
    #include <X11/XKBlib.h>
    #include <stdlib.h>
#endif

#if defined(DIRECTDRAW) && defined(UNIX)
    #include "SDL/SDL.h"
#endif

#if defined(DIRECTDRAW) && defined(WIN)
    #include "ddraw.h"
#endif

#ifdef COLOR8BITS
    #define COLOR     uchar
    #define COLORPTR  uchar*
    #define COLORLEN  (long)1
    #define COLORBITS 8
    #define COLORMASK 0xFF
    #define CLEARCOLOR 0
#endif
#ifdef COLOR16BITS
    #define COLOR     uint16
    #define COLORPTR  uint16*
    #define COLORLEN  (long)2
    #define COLORBITS 16
    #define COLORMASK 0xFFFF
    #define CLEARCOLOR 0
#endif
#ifdef COLOR32BITS
    #define COLOR     ulong
    #define COLORPTR  ulong*
    #define COLORLEN  (long)4
    #define COLORBITS 32
    #define COLORMASK 0xFFFFFFFF
    #define CLEARCOLOR 0xFF000000
#endif

#define CW COLORMASK
#define CB CLEARCOLOR

#define WIN_INIT_FLAG_SCALABLE	    	1
#define WIN_INIT_FLAG_NOBORDER	    	2
#define WIN_INIT_FLAG_FULL_CPU_USAGE	4

enum {
    EVT_NULL = 0,
    EVT_GETDATASIZE,
    EVT_AFTERCREATE,
    EVT_BEFORECLOSE,
    EVT_DRAW,
    EVT_FOCUS,
    EVT_UNFOCUS,	    //When user click on other window
    EVT_MOUSEDOUBLECLICK,
    EVT_MOUSEBUTTONDOWN,
    EVT_MOUSEBUTTONUP,
    EVT_BUTTONDOWN,
    EVT_BUTTONUP,
    EVT_MOUSEMOVE,
    EVT_SCREENRESIZE,	    //After main screen resize
    EVT_SCREENFOCUS,
    EVT_SCREENUNFOCUS
};

//buttons:
#define BUTTON_LEFT 1
#define BUTTON_MIDDLE 2
#define BUTTON_RIGHT 4
#define BUTTON_SCROLLUP 8
#define BUTTON_SCROLLDOWN 16

//Keyboard codes:
//There is full ASCII table with 256 key-codes
//Some of them:
#define KEY_SPACE       0x20
#define KEY_ENTER       10
#define KEY_BACKSPACE   127
#define KEY_TAB         9
#define KEY_ESCAPE      27

//Additional keys:
enum {
    KEY_F1 = 256,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_CAPS
};

//Special key flags:
#define KEY_SHIFT       512
#define KEY_CTRL        1024
#define KEY_ALT         2048

//Scroll flags:
#define SF_LEFT         1
#define SF_RIGHT        2
#define SF_UP           4
#define SF_DOWN         8
#define SF_STATICWIN    16

#define WINDOWPTR	sundog_window*
#define WINDOW		sundog_window

#define DECOR_BORDER_SIZE	wm->decor_border_size
#define DECOR_HEADER_SIZE	wm->decor_header_size
#define DECOR_FLAG_CENTERED	1
#define DECOR_FLAG_CHECK_SIZE	2

#define SCROLLBAR_SIZE	    wm->scrollbar_size
#define BUTTON_YSIZE( wm )  ( char_y_size( wm ) + 8 + 4 )
#define BUTTON_XSIZE( wm )  wm->button_xsize
#define INTERELEMENT_SPACE  2

#define DOUBLE_CLICK_PERIOD 200

#define WIN_MAX_COMS	    8
#define CEND		    40000
#define CWIN		    30000
#define CX1		    30001
#define CX2		    30002
#define CY1		    30003
#define CY2		    30004
#define CSUB		    30005
#define CADD		    30006
#define CPERC		    30007
#define CMAXVAL		    30008
#define CMINVAL		    30009

struct sundog_font
{
    uchar	    *data;
    char	    xsize[ 256 ];
    char	    real_width[ 256 ];
    char	    ysize[ 256 ];
    char	    y_offset[ 256 ];
    int		    descent;
    char	    max_ysize;
    int		    offset[ 256 ];
    COLOR	    rendered[ 32 * 32 ];
};

#define WIN_FLAG_ALWAYS_INVISIBLE		1
#define WIN_FLAG_ALWAYS_ON_TOP			2
#define WIN_FLAG_ALWAYS_UNFOCUSED		4
#define WIN_FLAG_TRASH				8

struct sundog_window
{
    int		    visible;
    uint16	    flags;
    uint16	    id;
    char	    *name;			    //window name
    int16	    x, y;			    //x/y (relative)
    int16	    screen_x, screen_y;		    //real x/y
    int16	    xsize, ysize;		    //window size
    COLOR	    color;
    sundog_font	    *user_font;
    MWCLIPREGION    *reg;
    WINDOWPTR	    parent;
    WINDOWPTR	    *childs;
    int		    childs_num;
    int		    (*win_handler)( void*, void* ); //window handler: (*event, *window_manager)
    void	    *data;

    int		    x1com[ WIN_MAX_COMS ];	    //Controllers of window coordinates (special commands)
    int		    y1com[ WIN_MAX_COMS ];
    int		    x2com[ WIN_MAX_COMS ];
    int		    y2com[ WIN_MAX_COMS ];
    int16	    controllers_defined;
    int16	    controllers_calculated;

    int		    (*action_handler)( void*, void*, void* );
    int		    action_result;
    void	    *handler_data;

    ticks_t	    click_time;

    //WBD (Window Buffer Draw) properties:
    COLORPTR	    wbd_buf;
    int16	    wbd_xsize;
    int16	    wbd_ysize;
};

//SunDog event handling:
// EVENT 
//   |
// user_event_handler()
//   | (if not handled OR all_childs)
// window 
//   | (if not handled OR all_childs)
// window childs
//   | (if not handled)
// handler_of_unhandled_events()

struct sundog_event
{
    int	            event_type;		//event type
    ticks_t	    event_time;
    WINDOWPTR	    event_win;
    int		    all_childs;		//1 - send this event to each childred in any case
    int16           x;
    int16           y;
    int             mouse_button;
    int             button;
    int             pressure;		//Key pressure (0..1023)
};

struct window_manager;
struct sundog_timer;

struct sundog_timer
{
    void            (*handler)( void*, sundog_timer*, window_manager* );
    void	    *data;
    ticks_t	    deadline;
    ticks_t	    delay;
};

struct window_manager
{
    int		    flags;
    
    long            events_count;	    //number of events to execute
    long	    current_event_num;
    sundog_event    events[ EVENTS ];
    sundog_mutex    events_mutex;

    WINDOWPTR	    *trash;

    ulong	    window_counter;	    //for ID calculation

    WINDOWPTR	    root_win;

    sundog_timer    timers[ TIMERS ];
    int		    timers_num;

    int             screen_xsize;
    int             screen_ysize;

    int		    screen_lock_counter;
    int	            screen_is_active;
    
    int		    frame_counter;

    sundog_font	    *user_font;

    COLOR	    colors[ 16 ];	    //System palette
    COLOR	    white;
    COLOR	    black;
    COLOR	    yellow;
    COLOR	    green;
    COLOR	    red;
    COLOR	    dialog_color;
    COLOR	    decorator_color;
    COLOR	    decorator_border;
    COLOR	    button_color;
    COLOR	    menu_color;
    COLOR	    selection_color;
    COLOR	    text_background;
    COLOR	    list_background;
    COLOR	    scroll_color;
    COLOR	    scroll_background_color;

    //Standart window decoration properties: ==================================================================
    int		    decor_border_size;
    int		    decor_header_size;
    int		    scrollbar_size;
    int		    button_xsize;
    int		    gui_size_increment;
    //=========================================================================================================
    					  
    long            wm_initialized;
    
    long	    exit_request;
    long	    exit_code;

    //WBD (Window Buffer Draw): ===============================================================================
    COLOR           *g_screen;		    //graphics screen pointer (each element is BGR233 / RGB565 / RGB888 )
    WINDOWPTR	    cur_window;		  
    long            pscreen_x;		    //screen size (x) in pixels
    long            pscreen_y;		    //screen size (y) in pixels
    COLOR           cur_font_color;
    bool            cur_font_invert;
    bool            cur_font_draw_bgcolor; 
    COLOR           cur_font_bgcolor;
    int		    cur_font_zoom;	    //256 - normal; 512 - x2
    int		    cur_transparency;
    //=========================================================================================================

    int		    pen_x;
    int		    pen_y;
    WINDOWPTR	    focus_win;
    WINDOWPTR	    prev_focus_win;
    uint16	    focus_win_id;
    uint16	    prev_focus_win_id;
    WINDOWPTR	    last_unfocused_window;

    WINDOWPTR	    handler_of_unhandled_events;

    char	    temp_string[ 512 ];

    int             argc;
    char            **argv;

    //DEVICE DEPENDENT PART:

    uint16          buttons_table[256];	    //keyboard table for event-manager
    int		    fb_xpitch;		    //framebuffer xpitch
    int		    fb_ypitch;		    //...
    int		    fb_offset;		    //...
#if defined(OPENGL) || defined(OPENGLCOMP)
#ifdef UNIX
    GLboolean       doubleBuffer;
#endif //UNIX
    int		    real_window_width;
    int		    real_window_height;
#endif //OPENGL
#if defined(DIRECTDRAW) && defined(WIN)
    LPDIRECTDRAWSURFACE lpDDSPrimary;	    //DirectDraw primary surface (for win32)
    LPDIRECTDRAWSURFACE lpDDSBack;	    //DirectDraw back surface (for win32)
#endif
#ifdef GDI
    ulong           gdi_bitmap_info[2048];  //GDI bitmap info (for win32)
#endif
#ifdef WIN
    HDC	            hdc;		    //graphics content handler
    HWND	    hwnd;
    HINSTANCE       hCurrentInst;
    HINSTANCE       hPreviousInst;
    LPSTR           lpszCmdLine;
    int             nCmdShow;
#endif
#ifdef WINCE
    HDC             hdc;		    //graphics content handler
    HINSTANCE       hCurrentInst;
    HINSTANCE       hPreviousInst;
    LPWSTR          lpszCmdLine;
    int             nCmdShow;
    int		    os_version;
    int		    gx_suspended;
#endif
#ifdef UNIX
    pthread_mutex_t sdl_lock_mutex;
#ifdef DIRECTDRAW
    SDL_Surface     *sdl_screen;
#endif
#ifdef X11
    Display         *dpy;
    Colormap	    cmap;
    Window          win;
    Visual          *win_visual;
    GC              win_gc;
    XImage          *win_img;
    XImage          *win_img_back_pattern;
    char            *win_img_back_pattern_data;
    long	    win_img_depth;
    char            *win_buffer;
    int	            win_depth;
#endif //X11
#endif //UNIX
};

#endif

