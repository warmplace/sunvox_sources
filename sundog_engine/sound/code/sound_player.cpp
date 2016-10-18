/*
    sound_player.cpp. Main sound playing function
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#ifndef NOSOUND

#include "../sound.h"

int main_callback( void *userData,
                   long stream,
                   void *_buffer,
                   long frameCount )
{
    //main variables: ============
    signed short *buffer = (signed short*) _buffer;
    long buffer_size = frameCount;
    sound_struct *U = (sound_struct*) userData;

    U->main_sound_callback_working = 1;
    
    //for stream stop: ===========
    if( U->need_to_stop ) 
    { 
	for( int i = 0; i < buffer_size * 2; i += 2 ) { buffer[ i ] = 0; buffer[ i + 1 ] = 0; }
	U->stream_stoped = 1; 
	U->main_sound_callback_working = 0;
	return 0; 
    }
    //============================
    
    //render piece of sound: =====
    if( render_piece_of_sound( buffer, buffer_size, U->freq, U->user_data ) == 0 )
    {
	for( int i = 0; i < buffer_size * 2; i += 2 ) { buffer[ i ] = 0; buffer[ i + 1 ] = 0; }
    }
    //============================

    U->main_sound_callback_working = 0;

    return 0;
}

#endif
