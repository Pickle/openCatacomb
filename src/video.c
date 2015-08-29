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

#include "video.h"
#include "system.h"
#include "opengl.h"
#include "catacomb.h"
#if defined(USE_EGL_RAW) || defined(USE_EGL_SDL) || defined(USE_EGL_RPI)
#include "eglport.h"
#endif

SDL_Surface *screen;

int32_t ticks_start = 0;
int32_t delay = 0;
int32_t frames = 0;
int32_t fpstime = 0;
#define FRAMEDUR (1000/30)

uint16_t tiles[TILEVIEW_W][TILEVIEW_H];
vertex_t vtx[TILEVIEW_W*TILEVIEW_H*VERTICES];
GLushort idx[TILEVIEW_W*TILEVIEW_H*INDICES];

enum xga_colors {
    black,          /* 0 */
    blue,           /* 1 */
    green,          /* 2 */
    cyan,           /* 3 */
    red,            /* 4 */
    magenta,        /* 5 */
    brown,          /* 6 */
    light_gray,     /* 7 */
    gray,           /* 8 */
    light_blue,     /* 9 */
    light_green,    /* 10 */
    light_cyan,     /* 11 */
    light_red5,     /* 12 */
    light_magenta,  /* 13 */
    yellow,         /* 14 */
    white           /* 15 */
};

uint32_t xga_palette[16] = {
    0xFF000000, /* 0 */
    0xFF0000AA, /* 1 blue */
    0xFF00AA00, /* 2 green */
    0xFF00AAAA, /* 3 */
    0xFFAA0000, /* 4 red */
    0xFFAA00AA, /* 5 */
    0xFFAA5500, /* 6 */
    0xFFAAAAAA, /* 7 */
    0xFF555555, /* 8 */
    0xFF5555FF, /* 9 */
    0xFF55FF55, /* 10 */
    0xFF55FFFF, /* 11 */
    0xFFFF5555, /* 12 */
    0xFFFF55FF, /* 13 */
    0xFFFFFF55, /* 14 */
    0xFFFFFFFF  /* 15 */
};

uint8_t cga_palette_mode[4][4] = {
    { 0,  2,  4,  6 },  /* palette 0 */
    { 0, 10, 12, 14 },  /* palette 0 high intensity */
    { 0,  3,  5,  7 },  /* palette 1 */
    { 0, 11, 13, 15 }   /* palette 1 high intensity */
};

boolean VideoInit( uint16_t width, uint16_t height )
{
    int32_t result;
    uint8_t x,y;
    uint32_t i,v;
    uint32_t tile[4];

    printf( "[VIDEO] Initialize SDL Video and Timer\n" );

    result = SDL_InitSubSystem( SDL_INIT_VIDEO|SDL_INIT_TIMER );
    if (result != 0)
    {
        printf( "[VIDEO] ERROR InitVideo SDL Video Error: %s\n", SDL_GetError() );
        return False;
    }

#if defined(USE_EGL_RAW) || defined(USE_EGL_SDL) || defined(USE_EGL_RPI)
    if (EGL_Open() != 0) {
        return False;
    }
#else
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,      5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,    6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,     5);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,    0);
#endif

    screen = SDL_SetVideoMode( width, height, 16, SCREEN_FLAGS );

    if (screen == NULL) {
        printf( "[VIDEO] ERROR InitVideo SDL Video screen error: %s\n", SDL_GetError() );
        return False;
    }

#if defined(USE_EGL_RAW) || defined(USE_EGL_SDL) || defined(USE_EGL_RPI)
    if (EGL_Init() != 0) {
        return False;
    }
#endif

    if (OGL_Init( BASE_W, BASE_H, width, height ) != 0) {
        return False;
    }

    VideoClearBuffer();
    VideoSwapBuffers();
    VideoClearBuffer();

    fpstime = ticks_start = SDL_GetTicks();

    for (y=v=i=0; y<TILEVIEW_H; y++)
    {
        for (x=0; x<TILEVIEW_W; x++)
        {
            tile[0] = x*TILE_W*SCALE;
            tile[1] = y*TILE_H*SCALE;
            tile[2] = TILE_W*SCALE;
            tile[3] = TILE_H*SCALE;

            vtx[v+0].pos[0]  = tile[0];             vtx[v+0].pos[1]  = tile[1];         /* A */
            vtx[v+1].pos[0]  = tile[0]+tile[2];     vtx[v+1].pos[1]  = tile[1];         /* B */
            vtx[v+2].pos[0]  = tile[0]+tile[2];     vtx[v+2].pos[1]  = tile[1]+tile[3]; /* C */
            vtx[v+3].pos[0]  = tile[0];             vtx[v+3].pos[1]  = tile[1]+tile[3]; /* D */

            idx[i+0] = v+0;     /* A */
            idx[i+1] = v+2;     /* C */
            idx[i+2] = v+1;     /* B */
            idx[i+3] = v+0;     /* A */
            idx[i+4] = v+3;     /* D */
            idx[i+5] = v+2;     /* C */

            v += VERTICES;
            i += INDICES;
        }
    }

    printf( "[VIDEO] SDL Video Completed!\n" );
    return True;
}

void VideoShutDown( void )
{
    printf( "[VIDEO] Shutdown SDL Video\n" );
#if defined(USE_EGL_RAW) || defined(USE_EGL_SDL) || defined(USE_EGL_RPI)
    EGL_Close();
#endif
    SDL_QuitSubSystem( SDL_INIT_VIDEO );
}

void RefreshTiles( tex_t *texture )
{
    uint8_t x,y;
    uint32_t v;
    uint16_t ch;
    uint16_t tx, ty;
    GLfloat tile[4];

    for (y=v=0; y<TILEVIEW_H; y++)
    {
        for (x=0; x<TILEVIEW_W; x++)
        {
            if (x<TILEVIEW_H && tiles[x][y] == 0)
            {
                ch = view[originy+y][originx+x];
            }
            else
            {
                ch = tiles[x][y];
            }

            if (ch > 450)
                tx = 0;

            tx = ty = 0;
            while (ch >= (texture->w / TILE_W))
            {
                ty++;
                ch -= (texture->w / TILE_W);
            }
            tx = ch;

            tile[0] = (float)(tx * TILE_W) / (float)texture->w;
            tile[1] = (float)(ty * TILE_H) / (float)texture->h;
            tile[2] = (float)TILE_W / (float)texture->w;
            tile[3] = (float)TILE_H / (float)texture->h;

            vtx[v+0].tex[0]  = tile[0];           vtx[v+0].tex[1]  = tile[1];
            vtx[v+1].tex[0]  = tile[0]+tile[2];   vtx[v+1].tex[1]  = tile[1];
            vtx[v+2].tex[0]  = tile[0]+tile[2];   vtx[v+2].tex[1]  = tile[1]+tile[3];
            vtx[v+3].tex[0]  = tile[0];           vtx[v+3].tex[1]  = tile[1]+tile[3];

            v += VERTICES;
        }
    }

    OGL_DrawTextures( GL_TRIANGLES, texture->id, TILEVIEW_W*TILEVIEW_H*INDICES, &vtx[0], &idx[0] );

#if defined(DEBUG)
    OGL_CheckErrors( __FILE__, __LINE__ );
#endif
}

void VideoSwapBuffers( void )
{
#if defined(USE_FBO)
    OGL_SelectBuffer( False );
    OGL_RenderBuffer();
#endif
#if defined(USE_EGL_RAW) || defined(USE_EGL_SDL) || defined(USE_EGL_RPI)
    EGL_SwapBuffers();
#else
    SDL_GL_SwapBuffers();
#endif
#if defined(USE_FBO)
    OGL_SelectBuffer( True );
#endif

    delay = MIN( 1000, MAX( 0, FRAMEDUR - (int32_t)(SDL_GetTicks() - ticks_start)));
    SDL_Delay( delay );

#if defined(DEBUG)
    if (SDL_GetTicks() - fpstime >= 1000) {
        fpstime = SDL_GetTicks();
        printf( "FPS %d\n", frames );
        frames = 0;
    }
    else frames++;
#endif

    ticks_start = SDL_GetTicks();

#if defined(DEBUG)
    OGL_CheckErrors( __FILE__, __LINE__ );
#endif
}

void VideoClearBuffer( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
}

void SetTileView( int16_t c, uint8_t w, uint8_t h )
{
    uint8_t x,y;

    /* Clear the view area */
    for (y=0; y<h; y++)
    {
        for (x=0; x<w; x++)
        {
            tiles[x][y] = c;
        }
    }
}

void CharOut( int16_t x, int16_t  y, int16_t c )
{
    tiles[x][y] = c;
}
