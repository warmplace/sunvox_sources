#ifndef __SOUND__
#define __SOUND__

//Main sound header

#include "core/core.h"

//Structures:

enum 
{
    STATUS_STOP = 0,
    STATUS_PLAY,
};

struct sound_struct
{
    volatile long	status;		    //Current playing status
    volatile long	need_to_stop;	    //Set it to 1 if you want to stop sound stream
    volatile long	stream_stoped;	    //If stream really stoped
    volatile long	stop_counter;

    void		*user_data;	    //Data for user defined render_piece_of_sound()

    int			freq;
    int			channels;

    volatile int 	main_sound_callback_working;
};

//Variables:

extern sound_struct g_snd;

#ifdef UNIX
extern int dsp;
#endif

//Functions:

int main_callback( void*, long, void*, long );
extern int render_piece_of_sound( signed short *buffer, int buffer_size, int freq, void *user_data );

int sound_stream_init( int freq, int channels );
void sound_stream_play( void );
void sound_stream_stop( void );
void sound_stream_close( void );

#endif
