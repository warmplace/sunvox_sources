/*
    utils.cpp. Various functions: string list; random generator
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#include "../../core/core.h"
#include "../../memory/memory.h"
#include "../../filesystem/v3nus_fs.h"
#include "../utils.h"

//WORKING WITH A STRING LISTS:

void list_init( list_data *data )
{
    data->items = (char*)mem_new( HEAP_STORAGE, MAX_SIZE, "list items", 0 );
    data->items_ptr = (long*)mem_new( HEAP_STORAGE, MAX_ITEMS * 4, "list items ptrs", 0 );
    list_clear( data );
    list_reset_selection( data );
}

void list_close( list_data *data )
{
    if( data->items )
    {
	mem_free( data->items );
	data->items = 0;
    }
    if( data->items_ptr )
    {
	mem_free( data->items_ptr );
	data->items_ptr = 0;
    }
}

void list_clear( list_data *data )
{
    data->items_num = 0;
    data->start_item = 0;
}

void list_reset_selection( list_data *data )
{
    data->selected_item = -1;
}

void list_add_item( char *item, char attr, list_data *data )
{
    long ptr, p;

    mem_off();
    
    if( data->items_num == 0 ) 
    {
	data->items_ptr[ 0 ] = 0;
	ptr = 0;
    }
    else 
    {
	ptr = data->items_ptr[ data->items_num ];
    }
    
    data->items[ ptr++ ] = attr | 128;
    for( p = 0; ; p++, ptr++ )
    {
	data->items[ ptr ] = item[ p ];
	if( item[ p ] == 0 ) break;
    }
    ptr++;
    data->items_num++;
    data->items_ptr[ data->items_num ] = ptr;
    
    mem_on();
}

void list_delete_item( long item_num, list_data *data )
{
    long ptr, p;

    if( item_num < data->items_num && item_num >= 0 )
    {
	mem_off();
	
	ptr = data->items_ptr[ item_num ]; //Offset of our item (in the "items")
	
	//Get item size (in chars):
	long size;
	for( size = 0; ; size++ )
	{
	    if( data->items[ ptr + size ] == 0 ) break;
	}
	
	//Delete it:
	for( p = ptr; p < MAX_SIZE - size - 1 ; p++ )
	{
	    data->items[ p ] = data->items[ p + size + 1 ];
	}
	for( p = 0; p < data->items_num; p++ )
	{
	    if( data->items_ptr[ p ] > ptr ) data->items_ptr[ p ] -= size + 1;
	}
	for( p = item_num; p < data->items_num; p++ )
	{
	    if( p + 1 < MAX_ITEMS ) 
		data->items_ptr[ p ] = data->items_ptr[ p + 1 ];
	    else
		data->items_ptr[ p ] = 0;
	}
	data->items_num--;
	if( data->items_num < 0 ) data->items_num = 0;

	if( data->selected_item >= data->items_num ) data->selected_item = data->items_num - 1;
	
	mem_on();
    }
}

void list_move_item_up( long item_num, list_data *data )
{
    if( item_num < data->items_num && item_num >= 0 )
    {
	mem_off();
	if( item_num != 0 )
	{
	    long temp = data->items_ptr[ item_num - 1 ];
	    data->items_ptr[ item_num - 1 ] = data->items_ptr[ item_num ];
	    data->items_ptr[ item_num ] = temp;
	    if( item_num == data->selected_item ) data->selected_item--;
	}
	mem_on();
    }
}	

void list_move_item_down( long item_num, list_data *data )
{
    if( item_num < data->items_num && item_num >= 0 )
    {
	mem_off();
	if( item_num != data->items_num - 1 )
	{
	    long temp = data->items_ptr[ item_num + 1 ];
	    data->items_ptr[ item_num + 1 ] = data->items_ptr[ item_num ];
	    data->items_ptr[ item_num ] = temp;
	    if( item_num == data->selected_item ) data->selected_item++;
	}
	mem_on();
    }
}

char* list_get_item( long item_num, list_data *data )
{
    if( item_num >= data->items_num ) return 0;
    if( item_num >= 0 )
	return data->items + data->items_ptr[ item_num ] + 1;
    else 
	return 0;
}

char list_get_attr( long item_num, list_data *data )
{
    if( item_num >= data->items_num ) return 0;
    if( item_num >= 0 )
	return data->items[ data->items_ptr[ item_num ] ] & 127;
    else
	return 0;
}

long list_get_selected_num( list_data *data )
{
    return data->selected_item;
}

void list_set_selected_num( long sel, list_data *data )
{
    data->selected_item = sel;
}

//Return values:
//1 - item1 > item2
//0 - item1 <= item2
long list_compare_items( long item1, long item2, list_data *data )
{
    char *i1 = data->items + data->items_ptr[ item1 ];
    char *i2 = data->items + data->items_ptr[ item2 ];
    char a1 = i1[ 0 ] & 127;
    char a2 = i2[ 0 ] & 127;
    i1++;
    i2++;
    
    long retval = 0;
    
    //Compare:
    if( a1 != a2 )
    {
	if( a1 == 1 ) retval = 0;
	if( a2 == 1 ) retval = 1;
    }
    else
    {
	for( int a = 0; ; a++ )
	{
	    if( i1[ a ] == 0 ) break;
	    if( i2[ a ] == 0 ) break;
	    if( i1[ a ] < i2[ a ] ) { break; }             //item1 < item2
	    if( i1[ a ] > i2[ a ] ) { retval = 1; break; } //item1 > item2
	}
    }
    
    return retval;
}

void list_sort( list_data *data )
{
    mem_off();
    for(;;)
    {
	int s = 0;
	for( long a = 0; a < data->items_num - 1; a++ )
	{
	    if( list_compare_items( a, a + 1, data ) )
	    {
		s = 1;
		long temp = data->items_ptr[ a + 1 ];
		data->items_ptr[ a + 1 ] = data->items_ptr[ a ];
		data->items_ptr[ a ] = temp;
	    }
	}
	if( s == 0 ) break;
    }
    mem_on();
}

//RANDOM GENERATOR:

unsigned long rand_next = 1;

void set_seed( unsigned long seed )
{
    rand_next = seed;
}

unsigned long pseudo_random( void )
{
    rand_next = rand_next * 1103515245 + 12345;
    return ((unsigned int)(rand_next/65536)%32768);
}

//MUTEXES:

#ifdef PALMOS
#include "../../sound/sound.h"
#endif

int sundog_mutex_init( sundog_mutex *mutex, int attr )
{
    int retval = 0;
#ifdef UNIX
    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_NORMAL );
    retval = pthread_mutex_init( &mutex->mutex, &mutexattr );
    pthread_mutexattr_destroy( &mutexattr );
#endif
#if defined(WIN) || defined(WINCE)
    mutex->mutex = CreateMutex( 0, 0, 0 );
#endif
#ifdef PALMOS
    mutex->mutex_cnt = 0;
    mutex->main_sound_callback_working = &g_snd.main_sound_callback_working;
#endif
    return retval;
}

int sundog_mutex_destroy( sundog_mutex *mutex )
{
    int retval = 0;
#ifdef UNIX
    retval = pthread_mutex_destroy( &mutex->mutex );
#endif
#if defined(WIN) || defined(WINCE)
    CloseHandle( mutex->mutex );
#endif
#ifdef PALMOS
    mutex->mutex_cnt = 0;
#endif
    return retval;
}

int sundog_mutex_lock( sundog_mutex *mutex )
{
    int retval = 0;
#ifdef UNIX
    retval = pthread_mutex_lock( &mutex->mutex );
#endif
#if defined(WIN) || defined(WINCE)
    WaitForSingleObject( mutex->mutex, INFINITE );
#endif
#ifdef PALMOS
    if( *mutex->main_sound_callback_working )
    {
	//We are in the main sound callback.
	if( mutex->mutex_cnt == 0 ) 
	{
	    mutex->mutex_cnt = 1;
	}
	else
	{
	    //Mutex is locked. But we can't wait for unlocking here,
	    //because all another "threads" are frozen (PalmOS is not multitask).
	    retval = 1;
	}
    }
    else
    {
	while( mutex->mutex_cnt != 0 ) {}
	mutex->mutex_cnt = 1;
    }
#endif
    return retval;
}

int sundog_mutex_unlock( sundog_mutex *mutex )
{
    int retval = 0;
#ifdef UNIX
    retval = pthread_mutex_unlock( &mutex->mutex );
#endif
#if defined(WIN) || defined(WINCE)
    ReleaseMutex( mutex->mutex );
#endif
#ifdef PALMOS
    mutex->mutex_cnt = 0;
#endif
    return retval;
}

//PROFILES:

profile_data g_profile;

void profile_new( profile_data *p )
{
    if( p == 0 ) p = &g_profile;

    p->num = 4;
    p->keys = (char**)MEM_NEW( HEAP_DYNAMIC, sizeof( char* ) * p->num );
    p->values = (char**)MEM_NEW( HEAP_DYNAMIC, sizeof( char* ) * p->num );
    mem_set( p->keys, sizeof( char* ) * p->num, 0 );
    mem_set( p->values, sizeof( char* ) * p->num, 0 );
}

void profile_resize( int new_num, profile_data *p )
{
    if( p == 0 ) p = &g_profile;

    if( new_num > p->num )
    {
	p->keys = (char**)mem_resize( p->keys, sizeof( char* ) * new_num );
	p->values = (char**)mem_resize( p->values, sizeof( char* ) * new_num );
	mem_set( p->keys + sizeof( char* ) * p->num, sizeof( char* ) * ( new_num - p->num ), 0 );
	mem_set( p->values + sizeof( char* ) * p->num, sizeof( char* ) * ( new_num - p->num ), 0 );
	p->num = new_num;
    }
}

int profile_add_value( char *key, char *value, profile_data *p )
{
    int rv = -1;

    if( p == 0 ) p = &g_profile;

    if( key )
    {
	for( rv = 0; rv < p->num; rv++ )
	{
	    if( p->keys[ rv ] == 0 ) break;
	}
	if( rv == p->num )
	{
	    //Free item not found.
	    profile_resize( p->num + 4, p );
	}
	if( p->values[ rv ] ) mem_free( p->values[ rv ] );
	p->values[ rv ] = 0;
	if( value )
	{
	    p->values[ rv ] = (char*)MEM_NEW( HEAP_DYNAMIC, mem_strlen( value ) + 1 );
	    p->values[ rv ][ 0 ] = 0;
	    mem_strcat( p->values[ rv ], value );
	}
	p->keys[ rv ] = (char*)MEM_NEW( HEAP_DYNAMIC, mem_strlen( key ) + 1 );
	p->keys[ rv ][ 0 ] = 0;
	mem_strcat( p->keys[ rv ], key );
    }
    return rv;
}

int profile_get_int_value( char *key, profile_data *p )
{
    int rv = -1;

    if( p == 0 ) p = &g_profile;

    if( key )
    {
	int i;
	for( i = 0; i < p->num; i++ )
	{
	    if( p->keys[ i ] )
		if( mem_strcmp( p->keys[ i ], key ) == 0 ) 
		    break;
	}
	if( i < p->num && p->values[ i ] )
	{
	    rv = string_to_int( p->values[ i ] );
	}
    }
    return rv;
}

char* profile_get_str_value( char *key, profile_data *p )
{
    char* rv = 0;

    if( p == 0 ) p = &g_profile;

    if( key )
    {
	int i;
	for( i = 0; i < p->num; i++ )
	{
	    if( p->keys[ i ] )
		if( mem_strcmp( p->keys[ i ], key ) == 0 ) 
		    break;
	}
	if( i < p->num && p->values[ i ] )
	{
	    rv = p->values[ i ];
	}
    }
    return rv;
}

void profile_close( profile_data *p )
{
    if( p == 0 ) p = &g_profile;

    if( p->num )
    {
	for( int i = 0; i < p->num; i++ )
	{
	    if( p->keys[ i ] ) mem_free( p->keys[ i ] );
	    if( p->values[ i ] ) mem_free( p->values[ i ] );
	}
	mem_free( p->keys );
	mem_free( p->values );
	p->keys = 0;
	p->values = 0;
	p->num = 0;
    }
}

#define PROFILE_KEY_CHAR( cc ) ( !( cc == ' ' || cc == 0x09 || cc == 0x0A || cc == 0x0D || cc == -1 ) )

void profile_load( char *filename, profile_data *p )
{
    char str1[ 129 ];
    char str2[ 129 ];
    str1[ 128 ] = 0;
    str2[ 128 ] = 0;
    int i;

    if( p == 0 ) p = &g_profile;

    profile_close( p );
    profile_new( p );

    V3_FILE f = v3_open( filename, "rb" );
    if( f )
    {
	int c;
	char comment_mode = 0;
	char key_mode = 0;
	while( 1 )
	{
	    c = v3_getc( f );
	    if( c == -1 ) break; //EOF
	    if( c == 0xD || c == 0xA ) 
	    {
		comment_mode = 0; //Reset comment mode at the end of line
		key_mode = 0;
	    }
	    if( comment_mode == 0 )
	    {
		if( c == '/' ) 
		{
		    comment_mode = 1; //Comments
		    continue;
		}
		if( PROFILE_KEY_CHAR( c ) )
		{
		    if( key_mode == 0 )
		    {
			//Get key name:
			for( i = 0; i < 128; i++ )
			{
			    str1[ i ] = c;
			    c = v3_getc( f );
			    if( !PROFILE_KEY_CHAR( c ) ) 
			    { 
				str1[ i + 1 ] = 0; 
				break; 
			    }
			}
			key_mode = 1;
		    }
		    else
		    {
			//Get value:
			str2[ 0 ] = 0;
			for( i = 0; i < 128; i++ )
			{
			    str2[ i ] = c;
			    c = v3_getc( f );
			    if( c == 0xD || c == 0xA || c == -1 ) 
			    { 
				str2[ i + 1 ] = 0; 
				break; 
			    }
			}
			profile_add_value( str1, str2, p );
			key_mode = 0;
		    }
		}
	    }
	}
	v3_close( f );
    }
}

//WORKING WITH A STRINGS:

void int_to_string( int value, char *str )
{
    int n, p = 0, f = 0;
    
    if( value < 0 )
    {
    	str[ p++ ] = '-';
	value = -value;
    }
    
    n = value / 100000000; if( n || f ) { str[p++] = (char)n+48; value -= n * 100000000; f = 1; }
    n = value / 10000000; if( n || f ) { str[p++] = (char)n+48; value -= n * 10000000; f = 1; }
    n = value / 1000000; if( n || f ) { str[p++] = (char)n+48; value -= n * 1000000; f = 1; }
    n = value / 100000; if( n || f ) { str[p++] = (char)n+48; value -= n * 100000; f = 1; }
    n = value / 10000; if( n || f ) { str[p++] = (char)n+48; value -= n * 10000; f = 1; }
    n = value / 1000; if( n || f ) { str[p++] = (char)n+48; value -= n * 1000; f = 1; }
    n = value / 100; if( n || f ) { str[p++] = (char)n+48; value -= n * 100; f = 1; }
    n = value / 10; if( n || f ) { str[p++] = (char)n+48; value -= n * 10; f = 1; }
    str[ p++ ] = (char)value + 48;
    str[ p ] = 0;
}

void hex_int_to_string( int value, char *str )
{
    long n, p = 0, f = 0;
    
    if( value < 0 )
    {
    	str[ p++ ] = '-';
	value = -value;
    }
    
    n = value >> 20; if( n || f ) { str[p] = (char)n+48; if(n>9) str[p]+=7; p++; value -= n << 20; f = 1; }
    n = value >> 16; if( n || f ) { str[p] = (char)n+48; if(n>9) str[p]+=7; p++; value -= n << 16; f = 1; }
    n = value >> 12; if( n || f ) { str[p] = (char)n+48; if(n>9) str[p]+=7; p++; value -= n << 12; f = 1; }
    n = value >> 8; if( n || f ) { str[p] = (char)n+48; if(n>9) str[p]+=7; p++; value -= n << 8; f = 1; }
    n = value >> 4; if( n || f ) { str[p] = (char)n+48; if(n>9) str[p]+=7; p++; value -= n << 4; f = 1; }
    str[p] = (char)value+48; if(value>9) str[p]+=7; p++; 
    str[p] = 0;	
}

int string_to_int( char *str )
{
    int res = 0;
    int d = 1;
    int minus = 1;
    for( int a = mem_strlen( str ) - 1; a >= 0; a-- )
    {
	int v = str[ a ];
	if( v >= '0' && v <= '9' )
	{
	    v -= '0';
	    res += v * d;
	    d *= 10;
	}
	else
	if( v == '-' ) minus = -1;
    }
    return res * minus;
}

int hex_string_to_int( char *str )
{
    int res = 0;
    int d = 1;
    int minus = 1;
    for( int a = mem_strlen( str ) - 1; a >= 0; a-- )
    {
	int v = str[ a ];
	if( ( v >= '0' && v <= '9' ) || ( v >= 'A' && v <= 'F' ) || ( v >= 'a' && v <= 'f' ) )
	{
	    if( v >= '0' && v <= '9' ) v -= '0';
	    else
	    if( v >= 'A' && v <= 'F' ) { v -= 'A'; v += 10; }
	    else
	    if( v >= 'a' && v <= 'f' ) { v -= 'a'; v += 10; }
	    res += v * d;
	    d *= 16;
	}
	else
	if( v == '-' ) minus = -1;
    }
    return res * minus;
}

char int_to_hchar( int value )
{
    if( value < 10 ) return value + '0';
	else return ( value - 10 ) + 'A';
}

#ifdef WINCE
unsigned short g_temp_wstr[ 1024 * 4 ];
unsigned char g_temp_cstr[ 1024 * 4 ];
int g_str_num1 = 0;
int g_str_num2 = 0;
WCHAR* c2w( char *s )
{
    unsigned char *ss = (unsigned char*)s;
    unsigned short *wstr = &g_temp_wstr[ 1024 * g_str_num1 ];
    int p1 = 0;
    int p2 = 0;
    while( ss[ p2 ] != 0 )
    {
	wstr[ p1 ] = ss[ p2 ];	
	if( wstr[ p1 ] > 127 )
	{
	    wstr[ p1 ] -= 128;
	    wstr[ p1 ] <<= 8;
	    p2++;
	    wstr[ p1 ] |= (unsigned short)( ss[ p2 ] );
	}
	p1++;
	p2++;
	if( p1 > 1020 ) break;
	if( p2 > 1020 ) break;
    }
    wstr[ p1 ] = 0;
    g_str_num1++; g_str_num1 &= 3;
    return (WCHAR*)wstr;
}
char* w2c( WCHAR *s )
{
    unsigned char *cstr = &g_temp_cstr[ 1024 * g_str_num2 ];
    int p1 = 0;
    int p2 = 0;
    while( s[ p2 ] != 0 )
    {
	cstr[ p1 ] = (unsigned char)s[ p2 ];
	if( s[ p2 ] > 127 )
	{
	    cstr[ p1 ] = (unsigned char)( 128 + ( s[ p2 ] >> 8 ) );
	    p1++;
	    cstr[ p1 ] = (unsigned char)( s[ p2 ] );
	}
	p1++;
	p2++;
	if( p1 > 1020 ) break;
	if( p2 > 1020 ) break;
    }
    cstr[ p1 ] = 0;
    g_str_num2++; g_str_num2 &= 3;
    return (char*)cstr;
}
#endif