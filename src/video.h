/*
  Catacomb Source Code
  Copyright (C) 1993-2014 Flat Rock Software

  openCatacomb
  Copyright (C) 2015 Scott R. Smith

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define RMASK 0x0000FF00
    #define GMASK 0x00FF0000
    #define BMASK 0xFF000000
    #define AMASK 0x000000FF
#else
    #define RMASK 0x00FF0000
    #define GMASK 0x0000FF00
    #define BMASK 0x000000FF
    #define AMASK 0xFF000000
#endif

#define GET_CGA_COLOR(X) xga_palette[cga_palette_mode[3][X]]
#define GET_EGA_COLOR(X) xga_palette[X]

#if defined(USE_EGL_RAW) || defined(USE_EGL_SDL) || defined(USE_EGL_RPI)
#define SCREEN_FLAGS    SDL_SWSURFACE|SDL_FULLSCREEN
#else
#define SCREEN_FLAGS    SDL_OPENGL|SDL_FULLSCREEN
#endif

extern uint16_t tiles[TILEVIEW_W][TILEVIEW_H];
extern uint32_t xga_palette[16];
extern uint8_t cga_palette_mode[4][4];
extern SDL_Surface *screen;

boolean VideoInit( uint16_t width, uint16_t height );
void VideoShutDown( void );
void VideoSwapBuffers( void );
void VideoClearBuffer( void );
void RefreshTiles( tex_t *texture );
void SetTileView( int16_t c, uint8_t w, uint8_t h );
void CharOut( int16_t x, int16_t  y, int16_t c );

#ifdef __cplusplus
}
#endif

#endif /* VIDEO_H_INCLUDED */
