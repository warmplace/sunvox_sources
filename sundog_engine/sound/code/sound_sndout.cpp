/*
    SunDog: sound_sndout.cpp. Multiplatform functions for the sound output
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#define DEFAULT_BUFFER_SIZE	4096	/*in samples*/
//for PalmOS devices it's 1024 samples

//#define OUTPUT_TO_FILE		1	/*output file: audio.raw*/

#ifdef WIN
    #define WIN_DSOUND	1
    //#define WIN_MMSOUND	1
#endif
#ifdef WINCE
    #define WIN_MMSOUND	1
#endif

#ifdef LINUX
    #define LINUX_ALSA
    //#define LINUX_OSS
#endif

#include "../sound.h"
#include "core/debug.h"
#include "filesystem/v3nus_fs.h"
#include "window_manager/wmanager.h"

#ifdef OUTPUT_TO_FILE
FILE *g_audio_output = 0;
#endif

#ifndef NOSOUND

sound_struct g_snd; //Main sound structure;

int g_paused = 0;
int g_first_play = 1;

//################################
//## LINUX:                     ##
//################################
#ifdef LINUX
#include <pthread.h>
int BUFLEN = DEFAULT_BUFFER_SIZE;
int dsp;
pthread_t pth;
#ifdef LINUX_OSS
    #include <linux/soundcard.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
#endif //...LINUX_OSS
#ifdef LINUX_ALSA
    #include <alsa/asoundlib.h>
    snd_pcm_t *g_alsa_playback_handle = 0;
#endif //...LINUX_ALSA
void *sound_thread( void *arg )
{
    if( get_option( OPT_SOUNDBUFFER ) != -1 ) BUFLEN = get_option( OPT_SOUNDBUFFER );
    char buf[ BUFLEN * 4 ];
    long len = BUFLEN;
    for(;;) 
    {
	main_callback( (sound_struct *)arg, 0, buf, len );
#ifdef LINUX_OSS
	if( dsp >= 0 ) 
	{
	    write( dsp, buf, len * 4 ); 
#endif //...LINUX_OSS
#ifdef LINUX_ALSA
	if( g_alsa_playback_handle >= 0 ) 
	{
	    snd_pcm_writei( g_alsa_playback_handle, buf, len );
#endif //...LINUX_ALSA
#ifdef OUTPUT_TO_FILE
	    fwrite( buf, 1, len * 4, g_audio_output );
#endif
	} 
	else 
	{
	    break;
	}
    }
    pthread_exit( 0 );
    return 0;
}
#endif

//################################
//## PALMOS:                    ##
//################################
#ifdef PALMOS
#include "PalmOS.h"
SndStreamRef main_stream = 0;
MemHandle ARM_code_handler;
char *ARM_code;
#endif

//################################
//## WINDOWS DIRECT SOUND       ##
//################################
#ifdef WIN_DSOUND
#include "dsound.h"
#define NUMEVENTS 2
LPDIRECTSOUND               lpds;
DSBUFFERDESC                dsbdesc;
LPDIRECTSOUNDBUFFER         lpdsb = 0;
LPDIRECTSOUNDBUFFER         lpdsbPrimary;
LPDIRECTSOUNDNOTIFY         lpdsNotify;
WAVEFORMATEX                *pwfx;
HMMIO                       hmmio;
MMCKINFO                    mmckinfoData, mmckinfoParent;
DSBPOSITIONNOTIFY           rgdsbpn[NUMEVENTS];
HANDLE                      rghEvent[NUMEVENTS];

//Sound thread:
HANDLE sound_thread;
SECURITY_ATTRIBUTES atr;
bool StreamToBuffer( ulong dwPos )
{
    LONG            lNumToWrite;
    DWORD           dwStartOfs;
    VOID            *lpvPtr1, *lpvPtr2;
    DWORD           dwBytes1, dwBytes2;
    static DWORD    dwStopNextTime = 0xFFFF;

    if( dwStopNextTime == dwPos )   // All data has been played
    {
	lpdsb->Stop();
	dwStopNextTime = 0xFFFF;
	return TRUE;
    }

    if( dwStopNextTime != 0xFFFF )  // No more to stream, but keep
		                    // playing to end of data
        return TRUE;

    if( dwPos == 0 )
	dwStartOfs = rgdsbpn[ 1 ].dwOffset;
    else
	dwStartOfs = rgdsbpn[ 0 ].dwOffset;

    lNumToWrite = (LONG) rgdsbpn[ dwPos ].dwOffset - dwStartOfs;
    if( lNumToWrite < 0 ) lNumToWrite += dsbdesc.dwBufferBytes;

    IDirectSoundBuffer_Lock( lpdsb,
                dwStartOfs,       // Offset of lock start
                lNumToWrite,      // Number of bytes to lock
                &lpvPtr1,         // Address of lock start
                &dwBytes1,        // Count of bytes locked
                &lpvPtr2,         // Address of wrap around
                &dwBytes2,        // Count of wrap around bytes
                0 );              // Flags

    //Write data to the locked buffer:
    main_callback( &g_snd, 0, lpvPtr1, dwBytes1 >> 2 );

    IDirectSoundBuffer_Unlock( lpdsb, lpvPtr1, dwBytes1, lpvPtr2, dwBytes2 );

    return TRUE;
}

unsigned long __stdcall sound_callback( void *par )
{
    int last_evt = 99;
    while( 1 )
    {
	DWORD dwEvt = MsgWaitForMultipleObjects(
			    NUMEVENTS,      // How many possible events
			    rghEvent,       // Location of handles
			    FALSE,          // Wait for all?
			    INFINITE,       // How long to wait
			    QS_ALLINPUT);   // Any message is an event

	dwEvt -= WAIT_OBJECT_0;

	// If the event was set by the buffer, there's input
	// to process. 

	if( dwEvt < NUMEVENTS && dwEvt != last_evt ) 
	{
	    if( lpdsb )	StreamToBuffer( dwEvt ); // copy data to output stream
	    last_evt = dwEvt;
	}
    }
    return 0;
}

LPGUID guids[ 128 ];
int guids_num = 0;

BOOL CALLBACK DSEnumCallback (
    LPGUID GUID,
    LPCSTR Description,
    LPCSTR Module,
    VOID *Context
)
{
    dprint( "Found sound device %d: %s\n", guids_num, Description );
    guids[ guids_num ] = GUID;
    guids_num++;
    return 1;
}
#endif

//################################
//## WINDOWS MMSOUND            ##
//################################
#ifdef WIN_MMSOUND

#define USE_THREAD
#define MAX_BUFFERS	2

int			g_bufferBytes = 4 * DEFAULT_BUFFER_SIZE;
HWAVEOUT		g_waveOutStream = 0;
HANDLE			g_waveOutThread = 0;
volatile int		g_waveOutExitRequest = 0;
WAVEHDR			g_outBuffersHdr[ MAX_BUFFERS ];

void SendBuffer( WAVEHDR *waveHdr )
{
    main_callback( &g_snd, 0, waveHdr->lpData, waveHdr->dwBufferLength / 4 );
    MMRESULT mres = waveOutWrite( g_waveOutStream, waveHdr, sizeof( WAVEHDR ) );
    if( mres != MMSYSERR_NOERROR )
    {
        dprint( "ERROR in waveOutWrite: %d\n", mres );
    }
}

DWORD WINAPI WaveOutThreadProc( LPVOID lpParameter )
{
    while( g_waveOutExitRequest == 0 )
    {
	MSG msg;
	WAVEHDR *waveHdr = NULL;
        while( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
    	    switch( msg.message )
            {
        	case MM_WOM_DONE:
            	    waveHdr = (WAVEHDR*)msg.lParam;
		    SendBuffer( waveHdr );
		    break;
	    }
	}
	Sleep( 5 );
    }
    g_waveOutExitRequest = 0;
    return 0;
}

ulong WaveOut( ulong hwo, unsigned short uMsg, ulong dwInstance, ulong dwParam1, ulong dwParam2 )
{
    if( uMsg == WOM_DONE )
    {
    }
    return 0;
}

#endif //...WIN_MMSOUND

//################################
//## OSX:                       ##
//################################
#ifdef OSX

#include <CoreAudio/AudioHardware.h>

AudioDeviceID g_audio_device;
signed short *g_temp_audio_buf = 0;

OSStatus osx_audio_proc(
    AudioDeviceID inDevice, 
    const AudioTimeStamp* inNow,
    const AudioBufferList* inInputData,
    const AudioTimeStamp* inInputTime,
    AudioBufferList* outOutputData, 
    const AudioTimeStamp* inOutputTime,
    void *inClientData )
{
    float* buffer = (float*)outOutputData->mBuffers[ 0 ].mData;

    int size = ( outOutputData->mBuffers[ 0 ].mDataByteSize / outOutputData->mBuffers[ 0 ].mNumberChannels ) / sizeof( float );

    main_callback( (sound_struct *)inInputData, 0, g_temp_audio_buf, size );
    for( int i = 0; i < size; i++ )
    {
    	buffer[ i * 2 ] = myInBuffer[ i * 2 ] * ( 1.0F / 32768.0F );
    	buffer[ i * 2 + 1 ] = myInBuffer[ i * 2 + 1 ] * ( 1.0F / 32768.0F );
    }

    return noErr;
}

#endif //...OSX

//################################
//## OTHER FUNCTIONS:           ##
//################################

int sound_stream_init( int freq, int channels )
{
#ifdef OUTPUT_TO_FILE
    g_audio_output = fopen( "audio.raw", "wb" );
#endif

    g_snd.user_data = 0;
    g_snd.freq = freq;
    g_snd.channels = channels;
    g_snd.stop_counter = 0;
    g_snd.main_sound_callback_working = 0;

#ifdef LINUX
#ifdef LINUX_OSS
    //Start first time:
    int temp;
    dsp = -1;
    int adev = get_option( OPT_AUDIODEVICE );
    if( adev != -1 )
    {
	char *ts = (char*)adev;
	dsp = open ( ts, O_WRONLY, 0 );
    }
    if( dsp == -1 )
    {
	dsp = open ( "/dev/dsp", O_WRONLY, 0 );
	if( dsp == -1 )
	    dsp = open ( "/dev/.static/dev/dsp", O_WRONLY, 0 );
    }
    if( dsp == -1 )
    {
        dprint( "OSS ERROR: Can't open sound device\n" );
        return 1;
    }
    temp = 1;
    ioctl (dsp, SNDCTL_DSP_STEREO, &temp);
    g_snd.channels = 2;
    temp = 16;
    ioctl (dsp, SNDCTL_DSP_SAMPLESIZE, &temp);
    temp = freq;
    ioctl (dsp, SNDCTL_DSP_SPEED, &temp);
    temp = 16 << 16 | 8;
    ioctl (dsp, SNDCTL_DSP_SETFRAGMENT, &temp);
    ioctl (dsp, SNDCTL_DSP_GETBLKSIZE, &temp);
    
    //Create sound thread:
    if( pthread_create( &pth, NULL, sound_thread, &g_snd ) != 0 )
    {
        dprint( "OSS ERROR: Can't create sound thread!\n" );
        return 1;
    }
#endif //...LINUX_OSS
#ifdef LINUX_ALSA
    int err;
    snd_pcm_hw_params_t *hw_params;
    if( get_option( OPT_AUDIODEVICE ) != -1 )
    {
	err = snd_pcm_open( &g_alsa_playback_handle, (char*)get_option( OPT_AUDIODEVICE ), SND_PCM_STREAM_PLAYBACK, 0 );
    }
    else
    {
	err = snd_pcm_open( &g_alsa_playback_handle, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0 );
    }
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't open audio device: %s\n", snd_strerror( err ) );
	return 1;
    }
    err = snd_pcm_hw_params_malloc( &hw_params );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't allocate hardware parameter structure: %s\n", snd_strerror( err ) );
	return 1;
    }
    err = snd_pcm_hw_params_any( g_alsa_playback_handle, hw_params );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't initialize hardware parameter structure: %s\n", snd_strerror( err ) );
	return 1;
    }
    err = snd_pcm_hw_params_set_access( g_alsa_playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't set access type: %s\n", snd_strerror( err ) );
	return 1;
    }
    err = snd_pcm_hw_params_set_format( g_alsa_playback_handle, hw_params, SND_PCM_FORMAT_S16_LE );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't set sample format: %s\n", snd_strerror( err ) );
	return 1;
    }
    err = snd_pcm_hw_params_set_rate_near( g_alsa_playback_handle, hw_params, (unsigned int*)&freq, 0 );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't set sample rate: %s\n", snd_strerror( err ) );
	return 1;
    }
    err = snd_pcm_hw_params_set_channels( g_alsa_playback_handle, hw_params, channels );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't set channel count: %s\n", snd_strerror( err ) );
	return 1;
    }
    snd_pcm_uframes_t frames;
    if( get_option( OPT_SOUNDBUFFER ) != -1 )
	frames = get_option( OPT_SOUNDBUFFER );
    else
	frames = DEFAULT_BUFFER_SIZE;
    err = snd_pcm_hw_params_set_buffer_size_near( g_alsa_playback_handle, hw_params, &frames );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't set buffer size: %s\n", snd_strerror( err ) );
	return 1;
    }
    err = snd_pcm_hw_params( g_alsa_playback_handle, hw_params );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't set parameters: %s\n", snd_strerror( err ) );
	return 1;
    }
    snd_pcm_hw_params_free( hw_params );
    err = snd_pcm_prepare( g_alsa_playback_handle );
    if( err < 0 ) 
    {
	dprint( "ALSA ERROR: Can't prepare audio interface for use: %s\n", snd_strerror( err ) );
	return 1;
    }    
    //Create sound thread:
    if( pthread_create( &pth, NULL, sound_thread, &g_snd ) != 0 )
    {
        dprint( "ALSA ERROR: Can't create sound thread!\n" );
        return 1;
    }
#endif //...LINUX_ALSA
#endif

#ifdef PALMOS
    ulong processor; //Processor type
    ARM_code = (char*)main_callback;
    FtrGet( sysFileCSystem, sysFtrNumProcessorID, &processor );
    if( sysFtrNumProcessorIsARM( processor ) )
    SndStreamCreate( &main_stream,
                     sndOutput,
		     freq,
		     sndInt16Little,
		     sndStereo,
		     (SndStreamBufferCallback) ARM_code,
		     &g_snd,
		     4096,
		     1 );
    else main_stream = 0;
    g_snd.channels = 2;
#endif

#ifdef WIN_DSOUND
    HWND hWnd = GetForegroundWindow();
    if( hWnd == NULL )
    {
        hWnd = GetDesktopWindow();
    }
    LPVOID EnumContext = 0;
    DirectSoundEnumerate( DSEnumCallback, EnumContext );
    if FAILED( DirectSoundCreate( 0, &lpds, NULL ) )
    {
        MessageBox( hWnd, "DSound: DirectSoundCreate error", "Error", MB_OK );
        return 1;
    }
    if FAILED( IDirectSound_SetCooperativeLevel(
	lpds, hWnd, DSSCL_PRIORITY ) )
    {
	MessageBox( hWnd, "DSound: SetCooperativeLevel error", "Error", MB_OK );
	return 1;
    }

    WAVEFORMATEX wfx;
    memset( &wfx, 0, sizeof( WAVEFORMATEX ) ); 
    wfx.wFormatTag = WAVE_FORMAT_PCM; 
    wfx.nChannels = channels; 
    wfx.nSamplesPerSec = freq; 
    wfx.wBitsPerSample = 16; 
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    pwfx = &wfx;

    // Create primary buffer:

    /*

    DSBUFFERDESC buf_desc;
    ZeroMemory( &buf_desc, sizeof( DSBUFFERDESC ) );
    buf_desc.dwSize        = sizeof( DSBUFFERDESC );
    buf_desc.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    buf_desc.dwBufferBytes = 0;
    buf_desc.lpwfxFormat   = NULL;
    if FAILED( lpds->CreateSoundBuffer(
		&buf_desc, &lpdsbPrimary, NULL ) )
    {
	MessageBox( hWnd, "DSound: Create primary buffer error", "Error", MB_OK );
	return;
    }
    if FAILED( lpdsbPrimary->SetFormat( &wfx ) )
    {
	MessageBox( hWnd, "DSound: Can't set format of the primary buffer", "Error", MB_OK );
	return;
    }
    lpdsbPrimary->Release();

    */

    // Secondary buffer:
	
    memset( &dsbdesc, 0, sizeof( DSBUFFERDESC ) ); 
    dsbdesc.dwSize = sizeof( DSBUFFERDESC ); 
    dsbdesc.dwFlags = 
            DSBCAPS_GETCURRENTPOSITION2   // Always a good idea
            | DSBCAPS_GLOBALFOCUS         // Allows background playing
            | DSBCAPS_CTRLPOSITIONNOTIFY; // Needed for notification
 
    // The size of the buffer is arbitrary, but should be at least
    // two seconds, to keep data writes well ahead of the play
    // position.
 
    if( get_option( OPT_SOUNDBUFFER ) != -1 )
	dsbdesc.dwBufferBytes = get_option( OPT_SOUNDBUFFER ) * 2 * channels * 2;
    else
	dsbdesc.dwBufferBytes = DEFAULT_BUFFER_SIZE * 2 * channels * 2;
    dsbdesc.lpwfxFormat = pwfx;

    if FAILED( IDirectSound_CreateSoundBuffer(
               lpds, &dsbdesc, &lpdsb, NULL ) )
    {
	MessageBox( hWnd,"DSound: Create secondary buffer error","Error",MB_OK);
	return 1;
    }

    //Create buffer events:
	
    for( int i = 0; i < NUMEVENTS; i++ )
    {
        rghEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        if( NULL == rghEvent[ i ] ) 
	{
	    MessageBox( hWnd,"DSound: Create event error","Error",MB_OK);
	    return 1;
	}
    }
    rgdsbpn[ 0 ].dwOffset = 0;
    rgdsbpn[ 0 ].hEventNotify = rghEvent[ 0 ];
    rgdsbpn[ 1 ].dwOffset = ( dsbdesc.dwBufferBytes / 2 );
    rgdsbpn[ 1 ].hEventNotify = rghEvent[ 1 ];
	
    if FAILED( lpdsb->QueryInterface( IID_IDirectSoundNotify, (VOID **)&lpdsNotify ) )
    {
    	MessageBox( hWnd,"DSound: QueryInterface error","Error",MB_OK);
    	return 1;
    }
 
    if FAILED( IDirectSoundNotify_SetNotificationPositions(
             lpdsNotify, NUMEVENTS, rgdsbpn ) )
    {
        IDirectSoundNotify_Release( lpdsNotify );
	MessageBox( hWnd, "DSound: SetNotificationPositions error"," Error", MB_OK );
	return 1;
    }

    IDirectSoundBuffer_Play( lpdsb, 0, 0, DSBPLAY_LOOPING );

    //Create main thread:
    atr.nLength = sizeof(atr);
    atr.lpSecurityDescriptor = 0;
    atr.bInheritHandle = 0;
    sound_thread = CreateThread( &atr, 8000, &sound_callback, &g_snd, 0, 0 );
    SetThreadPriority( sound_thread, THREAD_PRIORITY_TIME_CRITICAL );
#endif

#ifdef WIN_MMSOUND
    g_waveOutExitRequest = 0;
    
    int soundDevices = waveOutGetNumDevs();
    if( soundDevices == 0 ) 
	{ dprint( "ERROR: No sound devices :(\n" ); return 1; } //No sound devices
    dprint( "SOUND: Number of sound devices: %d\n", soundDevices );
    
    int ourDevice;
    int dev = -1;
    for( ourDevice = 0; ourDevice < soundDevices; ourDevice++ )
    {
    	WAVEOUTCAPS deviceCaps;
	waveOutGetDevCaps( ourDevice, &deviceCaps, sizeof( deviceCaps ) );
	if( deviceCaps.dwFormats & WAVE_FORMAT_4S16 ) 
	{
	    dev = ourDevice;
	    break;
	}
    }
    if( dev == -1 )
    {
	dprint( "ERROR: Can't find compatible sound device\n" );
	return 1;
    }
    dprint( "SOUND: Dev: %d. Sound freq: %d\n", dev, freq );

    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = channels;
    waveFormat.nSamplesPerSec = freq;
    waveFormat.nAvgBytesPerSec = freq * 4;
    waveFormat.nBlockAlign = 4;
    waveFormat.wBitsPerSample = 16;
    waveFormat.cbSize = 0;
#ifdef USE_THREAD
    g_waveOutThread = (HANDLE)CreateThread( NULL, 0, WaveOutThreadProc, NULL, 0, NULL );
    SetThreadPriority( g_waveOutThread, THREAD_PRIORITY_TIME_CRITICAL );
    MMRESULT mres = waveOutOpen( &g_waveOutStream, dev, &waveFormat, (ulong)g_waveOutThread, 0, CALLBACK_THREAD );
#else
    MMRESULT mres = waveOutOpen( &g_waveOutStream, dev, &waveFormat, (ulong)&WaveOut, 0, CALLBACK_FUNCTION );
#endif
    if( mres != MMSYSERR_NOERROR )
    {
	dprint( "ERROR: Can't open sound device (%d)\n", mres );
	switch( mres )
	{
	    case MMSYSERR_INVALHANDLE: dprint( "ERROR: MMSYSERR_INVALHANDLE\n" ); break;
	    case MMSYSERR_BADDEVICEID: dprint( "ERROR: MMSYSERR_BADDEVICEID\n" ); break;
	    case MMSYSERR_NODRIVER: dprint( "ERROR: MMSYSERR_NODRIVER\n" ); break;
	    case MMSYSERR_NOMEM: dprint( "ERROR: MMSYSERR_NOMEM\n" ); break;
	    case WAVERR_BADFORMAT: dprint( "ERROR: WAVERR_BADFORMAT\n" ); break;
	    case WAVERR_SYNC: dprint( "ERROR: WAVERR_SYNC\n" ); break;
	}
	return 1;
    }
    dprint( "SOUND: waveout device opened\n" );

    for( int b = 0; b < MAX_BUFFERS; b++ )
    {
        ZeroMemory( &g_outBuffersHdr[ b ], sizeof( WAVEHDR ) );
        g_outBuffersHdr[ b ].lpData = (char *)malloc( g_bufferBytes );
        g_outBuffersHdr[ b ].dwBufferLength = g_bufferBytes;
	mres = waveOutPrepareHeader( g_waveOutStream, &g_outBuffersHdr[ b ], sizeof( WAVEHDR ) );
	if( mres != MMSYSERR_NOERROR )
	{
	    dprint( "ERROR: Can't prepare %d waveout header (%d)\n", b, mres );
	}
	SendBuffer( &g_outBuffersHdr[ b ] );
    }
#endif

#ifdef OSX
    ulong prop_size;
    prop_size = sizeof( g_audio_device );
    if( AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &prop_size, &g_audio_device ) != kAudioHardwareNoError )
    {
	dprint( "ERROR in AudioHardwareGetProperty()\n" );
	return 1;
    }
    if( g_audio_device == kAudioDeviceUnknown )
    {
	dprint( "ERROR: unknown audio device\n" );
        return 1;
    }

    AudioStreamBasicDescription descr;
    prop_size = sizeof( descr );
    if( AudioDeviceGetProperty( g_audio_device, 0, 0, kAudioDevicePropertyStreamFormat, &prop_size, &descr ) != kAudioHardwareNoError )
    {
	dprint( "ERROR in AudioDeviceGetProperty()\n" );
	return 1;
    }
    
    //change device sampling frequency:
    dprint( "SOUND: device sampling freq: %d\n", descr.mSampleRate );
    if( descr.mSampleRate != freq )
    {
        descr.mSampleRate = freq;
	AudioDeviceSetProperty( g_audio_device, NULL, 0, 0, AudioDevicePropertyStreamFormat, prop_size, &descr );
    }
    
    //change device channels:
    dprint( "SOUND: device channels: %d\n", descr.mChannelsPerFrame );
    if( descr.mChannelsPerFrame != channels )
    {
    	mySoundBasicDescription.mChannelsPerFrame = channels;
	AudioDeviceSetProperty( g_audio_device, NULL, 0, 0, AudioDevicePropertyStreamFormat, prop_size, &descr );
    }

    if( descr.mFormatID != kAudioFormatLinearPCM )
    {
	dprint( "ERROR: format of audio device is not PCM\n" );
        return 1;
    }

    prop_size = sizeof( descr );
    if( AudioDeviceGetProperty( g_audio_device, 0, 0, kAudioDevicePropertyStreamFormat, &prop_size, &descr ) != kAudioHardwareNoError )
    {
	dprint( "ERROR in AudioDeviceGetProperty() [2]\n" );
	return 1;
    }
    dprint( "SOUND: new sampling freq: %d\n", descr.mSampleRate );
    dprint( "SOUND: new channels: %d\n", descr.mChannelsPerFrame );
    if( descr.mSampleRate != freq )
    {
	dprint( "ERROR: device can't change sampling frequency to %d\n", freq );
	return 1;
    }
    if( descr.mChannelsPerFrame != channels )
    {
	dprint( "ERROR: device can't change number of channels to %d\n", channels );
	return 1;
    }

    //Set buffer size:
    ulong buffer_bytes;
    if( get_option( OPT_SOUNDBUFFER ) != -1 )
	buffer_bytes = get_option( OPT_SOUNDBUFFER ) * 2 * channels * 2;
    else
	buffer_bytes = DEFAULT_BUFFER_SIZE * 2 * channels * 2;
	
    //Create temp audio buffer for 16bit sound:
    g_temp_audio_buf = (signed short*)malloc( buffer_bytes );

    //Set audio proc:
    if( AudioDeviceAddIOProc( g_audio_device, osx_audio_proc, (void*)&g_snd ) != kAudioHardwareNoError )
    {
	dprint( "ERROR: can't set audio proc\n" );
	return 1;
    }
#endif

    g_first_play = 1;
    return 0;
    dprint( "Sound stream initialized\n" );
}

void sound_stream_play( void )
{
    if( g_snd.stop_counter > 0 ) g_snd.stop_counter--;
    if( g_snd.stop_counter > 0 ) return;

    g_snd.need_to_stop = 0;
    
#ifndef NONPALM
    if( main_stream )
    {
	if( g_paused ) 
	{
	    SndStreamPause( main_stream, 0 );
	    g_paused = 0;
	}
	if( g_first_play )
	{
	    SndStreamStart( main_stream );
	    g_first_play = 0;
	}
    }
#endif

#ifdef WIN_MMSOUND
    dprint( "SOUND: play...\n" );
#endif

#ifdef OSX
    if( g_paused ) 
    {
	AudioDeviceStart( g_audio_device, osx_audio_proc );
        g_paused = 0;
    }
    if( g_first_play )
    {
	AudioDeviceStart( g_audio_device, osx_audio_proc );
        g_first_play = 0;
    }
#endif
}

void sound_stream_stop( void )
{
#ifdef LINUX
    g_snd.stream_stoped = 0;
    g_snd.need_to_stop = 1;
    while( g_snd.stream_stoped == 0 ) {} //Waiting for stoping
#endif

#ifndef NONPALM
    if( main_stream && g_paused == 0 )
    {
	g_snd.stream_stoped = 0;
	g_snd.need_to_stop = 1;
	while( g_snd.stream_stoped == 0 ) {} //Waiting for stoping
	SndStreamPause( main_stream, 1 );
	g_paused = 1;
    }
#endif

#ifdef WIN_DSOUND
    if( lpdsb )
    {
	g_snd.stream_stoped = 0;
	g_snd.need_to_stop = 1;
	while( g_snd.stream_stoped == 0 ) {} //Waiting for stoping
    }
#endif

#ifdef WIN_MMSOUND
    dprint( "SOUND: req. for stop...\n" );
    if( g_waveOutStream )
    {
	g_snd.stream_stoped = 0;
	g_snd.need_to_stop = 1;
	while( g_snd.stream_stoped == 0 ) { Sleep( 1 ); } //Waiting for stoping
    }
    dprint( "SOUND: req. for stop... ok\n" );
#endif

#ifdef OSX
    if( g_paused == 0 )
    {
	g_snd.stream_stoped = 0;
	g_snd.need_to_stop = 1;
	while( g_snd.stream_stoped == 0 ) {} //Waiting for stoping
	AudioDeviceStop( g_audio_device, osx_audio_proc );
	g_paused = 1;
    }
#endif

    g_snd.stop_counter++;
}

void sound_stream_close( void )
{
    dprint( "SOUND: sound_stream_close()\n" );
    sound_stream_stop();

#ifdef LINUX
#ifdef LINUX_OSS
    int our_dsp = dsp;
    dsp = -1;
    if( our_dsp >= 0 ) close( our_dsp );
#endif //...LINUX_OSS
#ifdef LINUX_ALSA
    snd_pcm_close( g_alsa_playback_handle );
#endif //...LINUX_ALSA
#endif

#ifdef PALMOS
    if( main_stream )
    {
	SndStreamDelete( main_stream );
    }
#endif

#ifdef WIN_DSOUND
    CloseHandle( sound_thread );
    if( lpdsb )
    {
        if (lpdsNotify)
	    lpdsNotify->Release();
	if (lpds)
	    lpds->Release();
    }
#endif

#ifdef WIN_MMSOUND
    g_waveOutExitRequest = 1;
    while( g_waveOutExitRequest ) { Sleep( 1 ); } //Waiting for thread stop
    CloseHandle( g_waveOutThread );
    dprint( "SOUND: CloseHandle (soundThread) ok\n" );
        
    MMRESULT mres;
    mres = waveOutReset( g_waveOutStream );
    if( mres != MMSYSERR_NOERROR )
    {
        dprint( "ERROR in waveOutReset (%d)\n", mres );
    }
    
    for( int b = 0; b < MAX_BUFFERS; b++ )
    {
	mres = waveOutUnprepareHeader( g_waveOutStream, &g_outBuffersHdr[ b ], sizeof( WAVEHDR ) );
	if( mres != MMSYSERR_NOERROR )
	{
	    dprint( "ERROR: Can't unprepare waveout header %d waveout header (%d)\n", b, mres );
	}
        free( g_outBuffersHdr[ b ].lpData );
    }

    mres = waveOutClose( g_waveOutStream );
    if( mres != MMSYSERR_NOERROR ) 
	dprint( "ERROR in waveOutClose: %d\n", mres );
    else
	dprint( "SOUND: waveOutClose ok\n" );
#endif

#ifdef OSX
    AudioDeviceRemoveIOProc( g_audio_device, osx_audio_proc );
    free( g_temp_audio_buf );
    g_temp_audio_buf = 0;
#endif

#ifdef OUTPUT_TO_FILE
    if( g_audio_output ) fclose( g_audio_output );
#endif
}

#endif //NOSOUND
