/*
    wm_opengl.h. Platform-dependent module : OpenGL functions
    This file is part of the SunDog engine.
    Copyright (C) 2002 - 2008 Alex Zolotov <nightradio@gmail.com>
*/

#ifndef __WINMANAGER_OPENGL__
#define __WINMANAGER_OPENGL__

extern window_manager *current_wm;

//#################################
//## DEVICE DEPENDENT FUNCTIONS: ##
//#################################

void gl_init( void )
{
    /* set viewing projection */
    glMatrixMode( GL_PROJECTION );
    glFrustum( -0.5F, 0.5F, -0.5F, 0.5F, 0.0F, 3.0F );

    /* position viewer */
    glMatrixMode( GL_MODELVIEW );
    //glTranslatef(0.0F, 0.0F, -2.2F);

    glClearDepth( 1.0f );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    //glEnable( GL_LIGHTING );
    //glEnable( GL_LIGHT0 );

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, 1 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glEnable( GL_COLOR_MATERIAL );
    glEnable( GL_BLEND );
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_LINE_SMOOTH_HINT );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	    
    glPixelTransferi( GL_INDEX_SHIFT, 0 );
    glPixelTransferi( GL_INDEX_OFFSET, 0 );
#ifdef COLOR8BITS
    glPixelTransferi( GL_MAP_COLOR, GL_TRUE );
#endif

    //Set palette:
    unsigned short map[ 256 ];
    long a;
    unsigned short const_alpha = 0xFFFF;
    for( a = 0; a < 256; a++ ) { map[ a ] = (unsigned short)( (a<<5)&224 ) << 8; if(map[a]) map[ a ] |= 0x1FFF; }
    glPixelMapusv( GL_PIXEL_MAP_I_TO_R, 256, map );
    for( a = 0; a < 256; a++ ) { map[ a ] = (unsigned short)( (a<<2)&224 ) << 8; if(map[a]) map[ a ] |= 0x1FFF; }
    glPixelMapusv( GL_PIXEL_MAP_I_TO_G, 256, map );
    for( a = 0; a < 256; a++ ) { map[ a ] = (unsigned short)( (a&192)|32 ) << 8; if(map[a]) map[ a ] |= 0x3FFF; }
    glPixelMapusv( GL_PIXEL_MAP_I_TO_B, 256, map );
    glPixelMapusv( GL_PIXEL_MAP_I_TO_A, 1, &const_alpha );
}

void gl_resize( void )
{
    /* set viewport to cover the window */
    glViewport( 0, 0, current_wm->real_window_width, current_wm->real_window_height );
}

//#################################
//#################################
//#################################

#endif
