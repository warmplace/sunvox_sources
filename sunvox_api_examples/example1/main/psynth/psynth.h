#ifndef __PSYTEXXSYNTHINT__
#define __PSYTEXXSYNTHINT__

//Use it in your synth code

//SunDog Engine include files:
#include "core/core.h"
#include "memory/memory.h"
#ifdef SUNVOX_GUI
#include "window_manager/wmanager.h"
#endif

enum {
    COMMAND_NOP = 0,
    COMMAND_GET_DATA_SIZE,
    COMMAND_GET_SYNTH_NAME,
    COMMAND_GET_SYNTH_INFO,
    COMMAND_GET_INPUTS_NUM,	//Get maximum number of input channels
    COMMAND_GET_OUTPUTS_NUM,	//Get maximum number of output channels
    COMMAND_GET_FLAGS,		//Get additional flags (for example: PSYNTH_FLAG_GENERATOR)
    COMMAND_GET_WIDTH,
    COMMAND_GET_HEIGHT,
    COMMAND_INIT,
    COMMAND_SETUP_FINISHED,
    COMMAND_CLEAN,              //Clean all data in sound buffers to 0
    COMMAND_RENDER_REPLACE,	//Replace data in the destination buffer
    COMMAND_NOTE_ON,
    COMMAND_SET_FREQ,
    COMMAND_SET_VELOCITY,
    COMMAND_NOTE_OFF,
    COMMAND_ALL_NOTES_OFF,
    COMMAND_SET_LOCAL_CONTROLLER,
    COMMAND_SET_GLOBAL_CONTROLLER,
    COMMAND_CLOSE
};

#define PSYTEXX_SYNTH_PARAMETERS \
    void *data_ptr, \
    int synth_id, \
    STYPE **inputs, \
    STYPE **outputs, \
    int sample_frames, \
    int command, \
    void *net

//Control type:
typedef int	CTYPE;

//Global defines (don't forget to define it): STYPE_FLOAT; STYPE_INT16

//Sample operations (selected by user):
//==== Float ====
//Sample type:
#ifdef STYPE_FLOAT
typedef float		STYPE;
typedef float		STYPE_CALC;
#define STYPE_DESCRIPTION "Float (32bit)"
#define STYPE_FLOATINGPOINT
#define INT16_TO_STYPE( res, val ) { res = (float)(val) / (float)32768; }
#define STYPE_TO_FLOAT( res, val ) { res = val; }
#define STYPE_TO_INT16( res, val ) { \
    int temp_res; \
    temp_res = (int)( (val) * (float)32768 ); \
    if( temp_res > 32767 ) res = 32767; else \
    if( temp_res < -32768 ) res = -32768; else \
    res = (signed short)temp_res; \
}
#endif
//==== Int16 (fixed point. 1.0 = 1024) ====
//Sample type:
#ifdef STYPE_INT16
typedef signed short	STYPE;
typedef int		STYPE_CALC;
#define STYPE_DESCRIPTION "Int16 (fixed point. 1.0 = 1024)"
#define INT16_TO_STYPE( res, val ) { res = (signed short)( (val) >> 5 ); }
#define STYPE_TO_FLOAT( res, val ) { res = (float)val / (float)1024; }
#define STYPE_TO_INT16( res, val ) { \
    if( val > 1023 ) res = 32767; else \
    if( val < -1023 ) res = -32768; else \
    res = (signed short)(val) << 5; \
}
#endif

struct psynth_control
{
    char	    *ctl_name;  //For example: "Delay", "Feedback"
    char	    *ctl_label; //For example: "dB", "samples"
    CTYPE	    ctl_min;
    CTYPE	    ctl_max;
    CTYPE	    ctl_def;
    CTYPE	    *ctl_val;
    int		    type;
};

//One item of external net:
#define PSYNTH_FLAG_EXISTS		1
#define PSYNTH_FLAG_OUTPUT		2
#define PSYNTH_FLAG_GENERATOR		8
#define PSYNTH_FLAG_EFFECT		16
#define PSYNTH_FLAG_RENDERED		32
#define PSYNTH_FLAG_INITIALIZED		64
#define PSYNTH_MAX_CHANNELS		8
#define PSYNTH_MAX_CONTROLLERS		17
struct psynth_net_item
{
    int		    flags;

    char	    item_name[ 32 ];
    int		    name_counter;			    //For generation unique names

    int		    (*synth)(  
			PSYTEXX_SYNTH_PARAMETERS
		    );
    void	    *data_ptr;				    //User data
    STYPE	    *channels_in[ PSYNTH_MAX_CHANNELS ];
    STYPE	    *channels_out[ PSYNTH_MAX_CHANNELS ];
    int		    in_empty[ PSYNTH_MAX_CHANNELS ];	    //Number of NULL bytes
    int		    out_empty[ PSYNTH_MAX_CHANNELS ];	    //Number of NULL bytes

    int		    x, y;   //In percents (0..1024)
    int		    instr_num;

    //Standart properties:
    int		    finetune;	//-256...256
    int		    relative_note;

    //Number of channels:
    int		    input_channels;
    int		    output_channels;

    //Links to an input synths:
    int		    *input_links;
    int		    input_num;

    //Controllers:
    psynth_control  ctls[ PSYNTH_MAX_CONTROLLERS ];
    int		    ctls_num;

#ifdef SUNVOX_GUI
    //Visual (optional)
    WINDOWPTR	    visual;
#endif

    //Data chunks:
    char	    **chunks;
    int		    *chunk_flags;
};

//External sound net (created by host):
#define MAX_PERIOD_PTR			30720
struct psynth_net
{
    //Net items (nodes):
    
    psynth_net_item	*items;
    int			items_num;
    
    //Some info:
    
    int			sampling_freq;
    int			tick_size;	//One tick size (256 - one sample)
    int			ticks_per_line;
    int			global_volume;	//256 - normal volume (1.0)
    
    //Additional parameters for particular synth:
    
    int			note;		//In. (write to synth)
    int			velocity;	//In. 0..256
    ulong		channel_id;	//In. 0xAAAABBBB: AAAA - pattern num; BBBB - pattern channel
    int			synth_channel;	//In.
    int			period_ptr;	//In. Value of note with finetune. 256 - one note. 256 * 12 - octave
					//    MAX_PERIOD_PTR - note 0; 0 - note 120

    int			ctl_num;	//In. Number of controller
    int			ctl_val;	//In. Controller value (0..65535)

    int			draw_request;	//Out. Sound rendering procedure can set this variable for GUI redraw
};

//Octaves:
//0..9
//smp_freq = 2 ^ ( 2 + note / 12 ) * 689.0625
//delta = smp_freq / real_sampling_freq

extern void psynth_register_ctl( 
    int		synth_id, 
    char	*ctl_name,  //For example: "Delay", "Feedback"
    char	*ctl_label, //For example: "dB", "samples"
    CTYPE	ctl_min,
    CTYPE	ctl_max,
    CTYPE	ctl_def,
    int		type,	    //0 - level (input values 0..0x8000); 1 - numeric
    CTYPE	*value,
    void	*net );

//Global tables:
extern ulong g_linear_tab[ 768 ];
extern uchar g_vibrato_tab[ 256 ];

//Global variables:
#ifdef SUNVOX_GUI
extern window_manager *g_wm;
#endif

//Data chunks. They can be used to storage such data as samples, envelopes.
//This data will be saved to file with sunvox tune.
#define PSYNTH_CHUNK_FLAG_SAMPLE_8BIT	    ( 1 << 0 )
#define PSYNTH_CHUNK_FLAG_SAMPLE_16BIT	    ( 1 << 1 )
#define PSYNTH_CHUNK_FLAG_SAMPLE_FLOAT	    ( 1 << 2 )
#define PSYNTH_CHUNK_FLAG_SAMPLE_STEREO	    ( 1 << 3 )
extern void psynth_new_chunk( int synth_id, int num, int size, int flags, void *net ); //Create new chunk of "size" bytes
extern void *psynth_get_chunk( int synth_id, int num, void *net ); //Get ptr to selected chunk, or NULL if chunk is not exists
extern int psynth_get_chunk_info( int synth_id, int num, void *net, ulong *size, int *flags );
extern void *psynth_resize_chunk( int synth_id, int num, ulong new_size, void *net );
extern void psynth_clear_chunk( int synth_id, int num, void *net );
extern void psynth_clear_chunks( int synth_id, void *net ); //Remove all chunks in synth

//Number of inputs/outputs:
extern int psynth_get_number_of_outputs( int synth_id, void *net );
extern int psynth_get_number_of_inputs( int synth_id, void *net );
extern void psynth_set_number_of_outputs( int num, int synth_id, void *net );
extern void psynth_set_number_of_inputs( int num, int synth_id, void *net );

#endif
