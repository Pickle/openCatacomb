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

#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BASE_W          320
#define BASE_H          200
#define SCREEN_W        800
#define SCREEN_H        480
#define SCALE           1

#define TILE_W          8
#define TILE_H          8
#define TILE_SIZE       TILE_W*TILE_H
#define TILE_CGA_BPP    4
#define TILE_EGA_BYTES  32
#define TILE_COL        64
#define TILE_ROW        32

#define CGAPICS_OFFSET  0x117D0
#define CGAPICS_LENGTH  0x5B60
#define EGAPICS_OFFSET  0x06100
#define EGAPICS_LENGTH  0xB6C0
#define TITLE_OFFSET    0x17997
#define TITLE_LENGTH    0x4000
#define END_OFFSET      0x1B997
#define END_LENGTH      0x4000

#define PATH_CACHE      "cache"
#define PATH_SOUND      PATH_CACHE"/sounds"
#define CATACOMB_UNLZ   "catacomb.unlz"

enum {
    file_ext,
    file_exe,
    file_sound,
    file_demo,
    file_score,
    file_cgachars,
    file_cgapics,
    file_egachars,
    file_egapics,
    file_total
};

typedef struct CONFIG_T {
    graphmode_t graphmode;
    soundmode_t soundmode;
    inputmode_t inputmode;
    GLuint      filtermode;
    uint8_t     mousesensitivity;
} config_t;

extern config_t config;
extern boolean force_refresh;
extern uint8_t episode;
extern SDL_Rect picdims[graphmode_total][pic_total];
extern char pathdata[STRSIZE];
extern const char *filelist[episode_total][file_total];

/*
0  2    Width       Sprite width \ 8
2  2    Height      Sprite height
4  4    Offset      Offset from plane start to start reading data \16
8  4    Mask offset Offset for mask data, = [Offset] + [Height] * [Width]
12 8    Hitbox      The hitbox co-ords; starting from 0,0; the left, then top, then
                    right then bottom limits of the sprite's hitbox; each two bytes
                    long.
20 12   Name        Sprite name, padded with nulls
*/
typedef struct SPRITE_HEADER_T {
    uint16_t width;
    uint16_t height;
    uint32_t offset;
    uint32_t offset_mask;
    int8_t  hitbox[8];
    char name[12];
} sprite_header_t;

/*
0  2    Width       Image width \ 8
2  2    Height      Image height
4  4    Offset      Offset from plane start to start reading data \16
8  8    Name        Image name, padded with nulls
*/
typedef struct PIC_HEADER_T {
    uint16_t width;
    uint16_t height;
    uint32_t offset;
    char name[8];
} pic_header_t;

/*
0   2    Blank
2   4    Font start  Start of 8x8 font \ 16
6   4    Tile start  Start of 16x16 tiles \ 16
10  4    Image start Start of unmasked images \ 16
14  4    Sprit start Start of masked sprites \ 16
18  4    Image en st Start of image header entries \ 16
22  4    Sprit en st Start of sprite graphics header entries \ 16
26  4    Blue pl st  Start of the blue plane (At end of header)
30  4    Green pl st Start of green plane
34  4    Red pl st   Start of red plane
38  2    Inten pl st Start of intensity plane
40  2    Num font    Number of font entries
42  2    Num til     Number of tiles
44  2    Num unmsk   Number of unmasked images in file
46  2    Num spr     Number of sprite images
48  16x  Image ent   Image entries, name, size, etc
?   32x  Mask ent    Sprite entries, name, size, etc
?   ?    EGA data    Raw uncompressed EGA data for all graphics
*/
typedef struct PICHEADER_T {
    uint32_t offset_char;
    uint32_t offset_tile;
    uint32_t offset_pic;
    uint32_t offset_sprite;
    uint32_t offset_picheaders;
    uint32_t offset_spriteheaders;
    uint32_t offset_plane[4]; /* BGRI */
    uint16_t count_char;
    uint16_t count_tile;
    uint16_t count_pic;
    uint16_t count_sprite;
} picheader_t;

void ParseArgs( int32_t argc, char *argv[] );
void SystemInit( void );
void SystemShutdown( void );
void LoadConfig( const char *path );
void SaveConfig( const char *path );
boolean LoadData( uint8_t type, const char *path );
void CloseData( void );
boolean LoadBMP( const char *path, tex_t *texture );
boolean SaveBMP( SDL_Surface *image, const char *path );
boolean LoadLocs( const char* path, SDL_Rect *locs );
boolean LoadDataFromExe( const char *path, uint8_t **data, uint32_t offset, uint32_t length );
SDL_Surface* ReadImageCGA( uint8_t *data, uint32_t w, uint32_t h );
SDL_Surface* ReadImageEGA( uint8_t *data, uint32_t offset, uint32_t *plane, uint32_t w, uint32_t h );
boolean LoadCGA_PICFormat( tex_t *texture, uint8_t *data, const char *path, uint32_t image_w, uint32_t image_h );
boolean LoadCGA_Tiles( tex_t *texture, uint8_t *data, uint32_t length, const char *path, uint32_t tile_w, uint32_t tile_h );
boolean LoadEGA_Tiles( tex_t *texture, uint8_t *data, uint32_t length, const char *path, uint32_t tile_w, uint32_t tile_h );
boolean LoadCGA_Pics( tex_t *texture, uint8_t *data, const char *path, uint32_t image_w, uint32_t image_h );
boolean LoadEGA_Pics( tex_t *texture, uint8_t *data, const char *path, uint32_t image_w, uint32_t image_h );

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_H_INCLUDED */
