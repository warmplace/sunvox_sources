#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef UNIX
#include <pthread.h>
#endif

#if defined(WIN) || defined(WINCE)
#include <windows.h>
#endif

//LIST:

#define MAX_SIZE 16000
#define MAX_ITEMS (long)4000

struct list_data
{
    char *items;     //Items data
    long *items_ptr; //Item pointers
    long items_num;  //Number of items; 
    long selected_item;
    long start_item;
};

void list_init( list_data *data );
void list_close( list_data *data );
void list_clear( list_data *data );
void list_reset_selection( list_data *data );
void list_add_item( char *item, char attr, list_data *data );
void list_delete_item( long item_num, list_data *data );
void list_move_item_up( long item_num, list_data *data );
void list_move_item_down( long item_num, list_data *data );
char* list_get_item( long item_num, list_data *data );
char list_get_attr( long item_num, list_data *data );
long list_get_selected_num( list_data *data );
void list_set_selected_num( long sel, list_data *data );
long list_compare_items( long item1, long item2, list_data *data );
void list_sort( list_data *data );

//RANDOM GENERATOR:

void set_seed( unsigned long seed );
unsigned long pseudo_random( void );

//MUTEXES:

struct sundog_mutex
{
#ifdef UNIX
    pthread_mutex_t mutex;
#endif
#if defined(WIN) || defined(WINCE)
    HANDLE mutex;
#endif
#ifdef PALMOS
    volatile int mutex_cnt;
    volatile int *main_sound_callback_working;
#endif
};

int sundog_mutex_init( sundog_mutex *mutex, int attr );
int sundog_mutex_destroy( sundog_mutex *mutex );
int sundog_mutex_lock( sundog_mutex *mutex );
int sundog_mutex_unlock( sundog_mutex *mutex );

//PROFILES:

#define KEY_SCREENX		"width"
#define KEY_SCREENY		"height"
#define KEY_SCREENFLIP		"flip"
#define KEY_FULLSCREEN		"fullscreen"
#define KEY_SOUNDBUFFER		"buffer"
#define KEY_AUDIODEVICE		"audiodevice"
#define KEY_FREQ		"frequency"
#define KEY_WINDOWNAME		"windowname"
#define KEY_NOBORDER		"noborder"

struct profile_data
{
    char **keys;
    char **values;
    int num;
};

void profile_new( profile_data *p );
void profile_resize( int new_num, profile_data *p );
int profile_add_value( char *key, char *value, profile_data *p );
int profile_get_int_value( char *key, profile_data *p );
char* profile_get_str_value( char *key, profile_data *p );
void profile_close( profile_data *p );
void profile_load( char *filename, profile_data *p );

//STRINGS:

void int_to_string( int value, char *str );
void hex_int_to_string( int value, char *str );
int string_to_int( char *str );
int hex_string_to_int( char *str );
char int_to_hchar( int value );

#ifdef WINCE
#include <windows.h>
WCHAR* c2w( char *s );
char* w2c( WCHAR *s );
#endif

#endif
