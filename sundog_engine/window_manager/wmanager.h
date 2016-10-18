#ifndef __WINDOWMANAGER__
#define __WINDOWMANAGER__

#include "../utils/utils.h"
#include "struct.h"

//################################
//## MAIN FUNCTIONS:            ##
//################################

int win_init( char*, int xsize, int ysize, int flags, window_manager* );     
                                                  //init window manager
void win_close( window_manager* );	          //close window manager

//################################
//## WINDOWS FUNCTIONS:         ##
//################################

WINDOWPTR new_window( 
    char *name, 
    int x, 
    int y, 
    int xsize, 
    int ysize, 
    COLOR color,
    WINDOWPTR parent, 
    int (*)( sundog_event*, window_manager* ),
    window_manager *wm );
void set_window_controller( WINDOWPTR win, int ctrl_num, window_manager *wm, ... );
void remove_window( WINDOWPTR win, window_manager *wm );
void add_child( WINDOWPTR win, WINDOWPTR child, window_manager *wm );
void remove_child( WINDOWPTR win, WINDOWPTR child, window_manager *wm );
void set_handler( WINDOWPTR win, int (*handler)(void*,WINDOWPTR,window_manager*), void *handler_data, window_manager *wm );
void draw_window( WINDOWPTR win, window_manager *wm );
void show_window( WINDOWPTR win, window_manager *wm );
void hide_window( WINDOWPTR win, window_manager *wm );
void recalc_regions( window_manager *wm );

void set_focus_win( WINDOWPTR win, window_manager *wm );

int send_event( WINDOWPTR win, int event_type, int all_childs, int x, int y, int mouse_button, int button, int pressure, window_manager* );
int check_event( sundog_event *evt, window_manager *wm );
void handle_event( sundog_event *evt, window_manager *wm );
int handle_event_by_window( sundog_event *evt, window_manager *wm );

int EVENT_LOOP_BEGIN( sundog_event *evt, window_manager *wm );
int EVENT_LOOP_END( window_manager *wm );

int add_timer( void (*)( void*, sundog_timer*, window_manager* ), void *data, ticks_t delay, window_manager *wm );
void remove_timer( int timer, window_manager *wm );

//################################
//## WINDOWS DECORATIONS:       ##
//################################

int decorator_handler( sundog_event *evt, window_manager *wm );
WINDOWPTR new_window_with_decorator( 
    char *name, 
    int x, 
    int y, 
    int xsize, 
    int ysize, 
    COLOR color,
    WINDOWPTR parent, 
    int (*)( sundog_event*, window_manager* ),
    int flags, //DECOR_FLAG_xxx
    window_manager *wm );

//################################
//## DRAWING FUNCTIONS:         ##
//################################

inline COLOR get_color( uchar r, uchar g, uchar b ); //Get color value by RGB
inline long red( COLOR c );
inline long green( COLOR c );
inline long blue( COLOR c );
inline COLOR blend( COLOR c1, COLOR c2, int value );
int char_x_size( uchar c, window_manager *wm );
int char_y_size( window_manager *wm );
int string_size( char *str, window_manager *wm );

void win_draw_lock( WINDOWPTR win, window_manager *wm );    //Only draw_window() and event_handler (during the EVT_DRAW handling) do this automaticaly!
void win_draw_unlock( WINDOWPTR win, window_manager *wm );  //Only draw_window() and event_handler (during the EVT_DRAW handling) do this automaticaly!

void win_draw_box( WINDOWPTR win, int x, int y, int xsize, int ysize, COLOR color, window_manager *wm );
void win_draw_frame( WINDOWPTR win, int x, int y, int xsize, int ysize, COLOR color, window_manager *wm );
void win_draw_frame3d( WINDOWPTR win, int x, int y, int xsize, int ysize, COLOR color, int inout, window_manager *wm );
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
    window_manager *wm );
void win_draw_bitmap( 
    WINDOWPTR win, 
    int x, 
    int y, 
    int xsize, 
    int ysize, 
    COLOR *data, 
    window_manager *wm );
void win_draw_char( WINDOWPTR win, int c, int x, int y, COLOR f, COLOR b, window_manager *wm );
void win_draw_string( WINDOWPTR win, char *str, int x, int y, COLOR f, COLOR b, window_manager *wm );
void win_draw_line( WINDOWPTR win, int x1, int y1, int x2, int y2, COLOR color, window_manager *wm );

inline COLOR get_color( uchar r, uchar g, uchar b )
{
    COLOR res = 0; //resulted color
#ifdef COLOR8BITS
    #ifdef GRAYSCALE
	res = (COLOR)( (int)( ( r + g + b + b ) >> 2 ) );
    #else
	res += ( b & 192 );
	res += ( g & 224 ) >> 2;
	res += ( r >> 5 );
    #endif
#endif
#ifdef COLOR16BITS

#ifdef RGB555
    res += ( r >> 3 ) << 10;
    res += ( g >> 3 ) << 5;
    res += ( b >> 3 );
#else
    res += ( r >> 3 ) << 11;
    res += ( g >> 2 ) << 5;
    res += ( b >> 3 );
    #ifdef PALMOS
    #ifndef DIRECTDRAW
    res = ( res >> 8 ) + ( ( res & 255 ) << 8 );
    #endif
    #endif
#endif

#endif
#ifdef COLOR32BITS
    res += 255 << 24;
    res += r << 16;
    res += g << 8;
    res += b;
#endif
    return res;
}

inline long red( COLOR c )
{
    long res = 0;
#ifdef COLOR8BITS
    #ifdef GRAYSCALE
	res = c;
    #else
	res = ( c << 5 ) & 255;
	if( res ) res |= 0x1F;
    #endif
#endif
#ifdef COLOR16BITS

#ifdef RGB555
    res = ( ( c >> 10 ) << 3 ) & 0xF8;
    if( res ) res |= 7;
#else
    #ifdef PALMOS
    #ifndef DIRECTDRAW
    c = ( c >> 8 ) + ( ( c & 255 ) << 8 );
    #endif
    #endif
    res = ( ( c >> 11 ) << 3 ) & 0xF8;
    if( res ) res |= 7;
#endif

#endif
#ifdef COLOR32BITS
    res = ( c >> 16 ) & 255;
#endif
    return res;
}

inline long green( COLOR c )
{
    long res = 0;
#ifdef COLOR8BITS
    #ifdef GRAYSCALE
	res = c;
    #else
	res = ( c << 2 ) & 0xE0;
	if( res ) res |= 0x1F;
    #endif
#endif
#ifdef COLOR16BITS

#ifdef RGB555
    res = ( ( c >> 5 ) << 3 ) & 0xF8;
    if( res ) res |= 7;
#else
    #ifdef PALMOS
    #ifndef DIRECTDRAW
    c = ( c >> 8 ) + ( ( c & 255 ) << 8 );
    #endif
    #endif
    res = ( ( c >> 5 ) << 2 ) & 0xFC;
    if( res ) res |= 3;
#endif

#endif
#ifdef COLOR32BITS
    res = ( c >> 8 ) & 255;
#endif
    return res;
}

inline long blue( COLOR c )
{
    long res = 0;
#ifdef COLOR8BITS
    #ifdef GRAYSCALE
	res = c;
    #else
	res = ( c & 192 );
	if( res ) res |= 0x3F;
    #endif
#endif
#ifdef COLOR16BITS

#ifdef RGB555
    res = ( c << 3 ) & 0xF8;
    if( res ) res |= 7;
#else
    #ifdef PALMOS
    #ifndef DIRECTDRAW
    c = ( c >> 8 ) + ( ( c & 255 ) << 8 );
    #endif
    #endif
    res = ( c << 3 ) & 0xF8;
    if( res ) res |= 7;
#endif

#endif
#ifdef COLOR32BITS
    res = c & 255;
#endif
    return res;
}

inline COLOR blend( COLOR c1, COLOR c2, int value )
{
    if( value == 0 ) return c1;
    else
    if( value > 255 ) return c2;
#ifdef COLOR32BITS
    int r1 = ( c1 >> 16 ) & 255;
    int g1 = ( c1 >> 8 ) & 255;
    int b1 = c1 & 255;
    int r2 = ( c2 >> 16 ) & 255;
    int g2 = ( c2 >> 8 ) & 255;
    int b2 = c2 & 255;
    int ival = 256 - value; 
    int r3 = ( r1 * ival + r2 * value ) >> 8;
    int g3 = ( g1 * ival + g2 * value ) >> 8;
    int b3 = ( b1 * ival + b2 * value ) >> 8;
    return 0xFF000000 | ( r3 << 16 ) | ( g3 << 8 ) | b3;
#endif
#ifdef COLOR16BITS
    #ifdef PALMOS
    #ifndef DIRECTDRAW
	c1 = ( c1 >> 8 ) + ( ( c1 & 255 ) << 8 );
	c2 = ( c2 >> 8 ) + ( ( c2 & 255 ) << 8 );
    #endif
    #endif
#ifdef RGB555
    int r1 = ( c1 >> 9 ) & 63; r1 |= 1;
    int g1 = ( c1 >> 4 ) & 63; g1 |= 1;
    int b1 = c1 & 31; b1 <<= 1; b1 |= 1;
    int r2 = ( c2 >> 9 ) & 63; r2 |= 1;
    int g2 = ( c2 >> 4 ) & 63; g2 |= 1;
    int b2 = c2 & 31; b2 <<= 1; b2 |= 1;
#else
    int r1 = c1 >> 10; r1 |= 1;
    int g1 = ( c1 >> 5 ) & 63; g1 |= 1;
    int b1 = c1 & 31; b1 <<= 1; b1 |= 1;
    int r2 = c2 >> 10; r2 |= 1;
    int g2 = ( c2 >> 5 ) & 63; g2 |= 1;
    int b2 = c2 & 31; b2 <<= 1; b2 |= 1;
#endif
    int ival = 256 - value; 
    ival >>= 2;
    value >>= 2;
    int r3 = r1 * ival + r2 * value;
    int g3 = g1 * ival + g2 * value;
    int b3 = b1 * ival + b2 * value;
#ifdef RGB555
    r3 >>= 7;
    g3 >>= 7;
    b3 >>= 7;
    int res = ( r3 << 10 ) | ( g3 << 5 ) | b3;
#else
    r3 >>= 7;
    g3 >>= 6;
    b3 >>= 7;
    int res = ( r3 << 11 ) | ( g3 << 5 ) | b3;
#endif
    #if defined(PALMOS) && !defined(DIRECTDRAW)
	return ( res >> 8 ) + ( ( res & 255 ) << 8 );
    #else    
	return res;
    #endif
#endif
#ifdef COLOR8BITS
    #ifdef GRAYSCALE
	int ival = 256 - value; 
	return (COLOR)( ( c1 * ival + c2 * value ) >> 8 );
    #else
	int b1 = c1 >> 5; b1 &= ~1; if( b1 ) b1 |= 1; //because B is 2 bits only
	int g1 = ( c1 >> 3 ) & 7; 
	int r1 = c1 & 7; 
	int b2 = c2 >> 5; b2 &= ~1; if( b2 ) b2 |= 1;
	int g2 = ( c2 >> 3 ) & 7; 
	int r2 = c2 & 7; 
	int ival = 256 - value; 
	int r3 = r1 * ival + r2 * value;
	int g3 = g1 * ival + g2 * value;
	int b3 = b1 * ival + b2 * value;
	b3 >>= 9;
	g3 >>= 8;
	r3 >>= 8;
	return ( b3 << 6 ) | ( g3 << 3 ) | r3;
    #endif
#endif
    return 0;
}

//################################
//## FONTS:                     ##
//################################

sundog_font *win_load_font( char *filename, window_manager *wm );
void win_remove_font( sundog_font *fnt );

//################################
//## WBD FUNCTIONS:             ##
//################################

//See code in wbd.cpp
void wbd_init( WINDOWPTR win, int xsize, int ysize, window_manager *wm );
void wbd_deinit( WINDOWPTR win, window_manager *wm );
void wbd_set_current_window( WINDOWPTR win, window_manager *wm );
void wbd_resize( WINDOWPTR win, int new_xsize, int new_ysize, window_manager *wm );
void wbd_draw( WINDOWPTR win, window_manager *wm );

void draw_screen( window_manager *wm );
void clear_screen( window_manager *wm );
void draw_line( int x1, int y1, int x2, int y2, COLOR color, window_manager *wm );
void draw_frame( int x, int y, int x_size, int y_size, COLOR color, window_manager *wm );
void draw_box( int x, int y, int x_size, int y_size, COLOR color, window_manager *wm );
int draw_char( uchar c, int x, int y, window_manager *wm );
void draw_string( char *str, int x, int y, window_manager *wm );
void draw_pixel( int x, int y, COLOR color, window_manager *wm );

int wbd_char_x_size( uchar c, window_manager *wm );
int wbd_char_y_size( window_manager *wm );
int wbd_string_size( char *str, window_manager *wm );

//################################
//## STANDARD WINDOW HANDLERS:  ##
//################################

int null_handler( sundog_event *evt, window_manager *wm );
int desktop_handler( sundog_event *evt, window_manager *wm );
int hdivider_handler( sundog_event *evt, window_manager *wm );
int vdivider_handler( sundog_event *evt, window_manager *wm );
void bind_divider_to( WINDOWPTR win, WINDOWPTR bind_to, window_manager *wm );
int label_handler( sundog_event *evt, window_manager *wm );
extern int TEXT_CALL_HANDLER_ON_ANY_CHANGES;
int text_handler( sundog_event *evt, window_manager *wm );
void text_set_text( WINDOWPTR win, char *text, window_manager *wm );
char *text_get_text( WINDOWPTR win, window_manager *wm );
extern COLORPTR CREATE_BITMAP_BUTTON;
extern int CREATE_BITMAP_BUTTON_RECALC_COLORS;
extern int CREATE_BITMAP_BUTTON_XSIZE;
extern int CREATE_BITMAP_BUTTON_YSIZE;
extern char *CREATE_BUTTON_WITH_TEXT_MENU;
extern int CREATE_BUTTON_WITH_LEVELS;
extern int CREATE_BUTTON_WITH_AUTOREPEAT;
extern char text_up[ 2 ];
extern char text_down[ 2 ];
extern char text_left[ 2 ];
extern char text_right[ 2 ];
int button_handler( sundog_event *evt, window_manager *wm );
extern int CREATE_NUMBERED_LIST;
int list_handler( sundog_event *evt, window_manager *wm );
list_data *list_get_data( WINDOWPTR win, window_manager *wm );
int list_get_last_action( WINDOWPTR win, window_manager *wm );
void list_select_item( WINDOWPTR win, int item_num, window_manager *wm );
extern int CREATE_VERTICAL_SCROLLBAR;
extern int CREATE_REVERSE_SCROLLBAR;
extern int CREATE_COMPACT_SCROLLBAR;
int scrollbar_handler( sundog_event *evt, window_manager *wm );
void scrollbar_set_parameters( WINDOWPTR win, int cur, int max_value, int page_size, int step_size, window_manager *wm );
void scrollbar_set_value( WINDOWPTR win, int val, window_manager *wm );
int scrollbar_get_value( WINDOWPTR win, window_manager *wm );
void scrollbar_set_name( WINDOWPTR win, char *name, window_manager *wm );
void scrollbar_set_showing_offset( WINDOWPTR win, int offset, window_manager *wm );
int keyboard_handler( sundog_event *evt, window_manager *wm );
extern char *FILES_PROPS;
extern char *FILES_MASK;
extern char *FILES_RESULTED_FILENAME;
int files_handler( sundog_event *evt, window_manager *wm );
extern char *DIALOG_OK_TEXT;
extern char *DIALOG_CANCEL_TEXT;
extern char *DIALOG_TEXT;
extern int *DIALOG_RESULT;
int dialog_handler( sundog_event *evt, window_manager *wm );
extern char *CREATE_TEXT_POPUP;
extern int CREATE_LEVEL_POPUP;
extern int *SAVE_LEVEL_TO;
int popup_handler( sundog_event *evt, window_manager *wm );
int popup_menu( char* name, char* items, int levels, int x, int y, COLOR color, window_manager *wm );

//################################
//## DIALOGS:                   ##
//################################

char *dialog_open_file( char *name, char *mask, char *id, window_manager *wm );
int dialog( char *name, char *ok, char *cancel, window_manager *wm );

//################################
//## DEVICE DEPENDENT:          ##
//################################

void fix_fullscreen_resolution( int *xsize, int *ysize, window_manager *wm );

long device_event_handler( window_manager* );     //device event handler (return 1 for EXIT)
int device_start( char*, int xsize, int ysize, int flags, window_manager* ); 
                                                  //device start (before main loop)
void device_end( window_manager* );	          //device end   (after main loop)

void device_draw_line( int x1, int y1, int x2, int y2, COLOR color, window_manager *wm );
void device_draw_box( int x, int y, int xsize, int ysize, COLOR color, window_manager *wm );
void device_draw_bitmap( 
    int dest_x, int dest_y, 
    int dest_xs, int dest_ys,
    int src_x, int src_y,
    int src_xs, int src_ys,
    COLOR *data,
    window_manager *wm );
void device_screen_lock( window_manager *wm );
void device_screen_unlock( window_manager *wm );
void device_redraw_framebuffer( window_manager *wm );

#endif
