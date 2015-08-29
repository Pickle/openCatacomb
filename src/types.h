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

#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#if defined(WIN32)
#include "dirent.h"
#endif

#if !defined(WIN32)
#define GL_GLEXT_PROTOTYPES 1
#endif

#include <SDL.h>
#include <SDL_mixer.h>

#if defined(USE_GLES1)
    #include <GLES/gl.h>
    #include <GLES/glext.h>
#elif defined(USE_GLES2)
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #if defined(PANDORA)
    #include <GLES2/gl2extimg.h>
    #endif
#else
    #include <SDL_opengl.h>
#endif

#pragma pack(1)

#define TITLE           "openCatacomb"
#define VERSION         1.0
#define OC_COPYRIGHT    "Copyright (C) 2014 Scott Smith"
#define CATA_COPYRIGHT  "Copyright (C) 1993-2014 Flat Rock Software"
#define SELECTOR        "->"
#define STRSIZE         256

#define MAXPICS         2047
#define NUMTILES        24*24-1     /* {number of tiles displayed on screen} */
#define NUMLEVELS       10
#define MAXOBJ          200         /* {maximum possible active objects} */
#define SOLIDWALL       129
#define BLANKFLOOR      128
#define LEFTOFF         11
#define TOPOFF          11
#define TILE2S          256         /* {tile number where the 2*2 pictures start} */
#define TILE3S          TILE2S+64*4
#define TILE4S          TILE3S+19*9
#define TILE5S          TILE4S+19*16
#define LASTTILE        TILE5S+19*25
#define TILEVIEW_W      40
#define TILEVIEW_H      25

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LIM(a,b,c) MIN(b,MAX(a,c))

typedef uint8_t boolean;

enum BOOLEAN_T {
    False,
    True
};

typedef enum EPISODE_T {
    catacomb1,
    catacomb2,
    episode_total
} episode_t;

typedef enum PICTYPE_T {
    pic_cga,
    pic_ega,
    pic_vga,
    pic_noega,
    pic_novga,
    pic_nosound,
    pic_speaker,
    pic_keyboard,
    pic_joy1,
    pic_joy2,
    pic_mouse,
    pic_nojoy,
    pic_nomouse,
    pic_side,
    pic_title,
    pic_end,
    pic_total
} pictype_t;

typedef enum SOUNDTYPE_T {
    nosnd=-1,
    blockedsnd,
    itemsnd,
    treasuresnd,
    bigshotsnd,
    shotsnd,
    tagwallsnd,
    tagmonsnd,
    tagplayersnd,
    killmonsnd,
    killplayersnd,
    opendoorsnd,
    potionsnd,
    spellsnd,
    noitemsnd,
    gameoversnd,
    highscoresnd,
    leveldonesnd,
    foundsnd,
    soundstotal
} soundtype_t;

typedef enum THINKTYPE_T {
    playercmd,
    gargcmd,
    dragoncmd,
    ramstraight,
    ramdiag,
    straight,
    idle,
    fade,
    explode,    /* added with C2 */
    gunthinke,  /* added with C2 */
    gunthinks   /* added with C2 */
} thinktype_t;

typedef enum TAGTYPE_T {
    benign,
    monster,
    pshot,
    mshot,
    nukeshot
} tagtype_t;

typedef enum CLASSTYPE_T {
    nothing,
    player,
    goblin,
    skeleton,
    ogre,
    gargoyle,
    dragon,
    turbogre,   /* added with C2 */
    wallhit,
    shot,
    bigshot,
    rock,
    dead1,
    dead2,
    dead3,
    dead4,
    dead5,
    dead6,
    teleporter,
    torch,
    secretgate, /* added with C2 */
    gune,       /* added with C2 */
    guns,       /* added with C2 */
    lastclass
} classtype_t;

typedef enum INPMODE_T {
    keyboard,
    mouse,
    joystick1,
    joystick2,
    inputmode_total
} inputmode_t;

typedef enum GRAPHMODE_T {
    CGAgr,
    EGAgr,
    VGAgr,
    graphmode_total
} graphmode_t;

typedef enum soundmode_t {
    sound_off,
    sound_on,
    soundmode_total
} soundmode_t;

typedef enum PLAYMODE_T {
    intro,
    game,
    demogame,
    demosave,
    editor
} playmode_t;

typedef enum GAMEXIT_T {
    quited,
    killed,
    reseted,
    victorious
} gamexit_t;

typedef enum DIRTYPE_T {
    north,east,south,west,
    northeast,southeast,southwest,northwest,nodir,
    dirtype_total
} dirtype_t;

typedef enum ITEM_T {
    item_reserved1,
    item_key,           /* 1 */
    item_potion,        /* 2 */
    item_bolt,          /* 3 */
    item_reserved2,
    item_nuke,          /* 5 */
    items_total
} item_t;


/*
  ActiveObj = Record
    active : boolean;     {if false, the object has not seen the player yet}
    class : classtype;
    x,y,                  {location of upper left corner in world}
    stage,                {animation frame being drawn}
    delay:byte;           {number of frames to pause without doing anything}
    dir : dirtype;        {direction facing}
    hp : byte;            {hit points}
    oldx,oldy: byte;      {position where it was last drawn}
    oldtile : integer;	  {origin tile when last drawn}
    filler : array [1..4] of byte;	{pad to 16 bytes}
  end;
*/

typedef struct ACTIVEOBJ_T {
    boolean     active;     /* {if false, the object has not seen the player yet} */
    classtype_t type;
    uint8_t     x, y;       /* {location of upper left corner in world} */
    uint8_t     stage;      /* {animation frame being drawn} */
    uint8_t     delay;      /* {number of frames to pause without doing anything} */
    dirtype_t   dir;        /* {direction facing} */
    int8_t      hp;         /* {hit points} */
    uint8_t     oldx, oldy; /* {position where it was last drawn} */
    int32_t     oldtile;    /* {origin tile when last drawn} */
    uint8_t     filler[2];  /* {pad to 16 bytes} */
} activeobj_t;
/* C2 made hp signed and oldtile full int so filler shrinks */

/*
  objdesc = record	{holds a copy of ActiveObj, and its class info}
    active : boolean;
    class : classtype;
    x,y,stage,delay:byte;
    dir : dirtype;
    hp : shortint;
    oldx,oldy: byte;
    oldtile : integer;
    filler1 : array [1..4] of byte;	{pad to 16 bytes}

    think : thinktype;
    contact : tagtype;
    solid : boolean;
    firstchar : word;
    size : byte;
    stages : byte;
    dirmask : byte;
    speed : word;
    hitpoints : byte;
    damage : byte;
    points : word;
    filler2 : array [1..2] of byte;	{pad to 32 bytes}
  end;
*/
typedef struct OBJDESC_T {
    boolean     active;     /* {if false, the object has not seen the player yet} */
    classtype_t type;
    uint8_t     x, y;       /* {location of upper left corner in world} */
    uint8_t     stage;      /* {animation frame being drawn} */
    uint8_t     delay;      /* {number of frames to pause without doing anything} */
    dirtype_t   dir;        /* {direction facing} */
    int8_t      hp;         /* {hit points} */
    uint8_t     oldx, oldy; /* {position where it was last drawn} */
    int32_t     oldtile;    /* {origin tile when last drawn} */
    uint8_t     filler[2];  /* {pad to 16 bytes} */

    thinktype_t think;
    tagtype_t   contact;
    boolean     solid;
    uint16_t    firstchar;
    uint8_t     size;
    uint8_t     stages;
    uint8_t     dirmask;
    uint16_t    speed;
    uint8_t     hitpoints;
    uint8_t     damage;
    uint16_t    points;
    uint8_t     filler2[2];      /* {pad to 32 bytes} */
} objdesc_t;

typedef struct OBJDEF_T {
    thinktype_t think;
    tagtype_t   contact;
    boolean     solid;
    uint16_t    firstchar;
    uint8_t     size;
    uint8_t     stages;
    uint8_t     dirmask;
    uint16_t    speed;
    uint8_t     hitpoints;
    uint8_t     damage;
    uint16_t    points;
    uint8_t     filler2[2];      /* {pad to 32 bytes} */
} objdef_t;

/*
  highscores : Array [1..5] of record
    score : longint;
    level : integer;
    initials : Array [1..3] of char;
  End;
*/

typedef struct HIGHSCORES_T {
    int32_t score;      /* score : longint; */
    uint16_t level;     /* level : integer; */
    char initials[4];   /* initials : Array [1..3] of char; */
} highscores_t;

typedef struct TEX_T {
    GLuint id;
    uint32_t w;
    uint32_t h;
} tex_t;

typedef struct FONT_T {
    tex_t texture;
    uint32_t w;
    uint32_t h;
} font_t;

#if defined(USE_FBO)
typedef struct FBO_T {
    tex_t texture;
    GLuint framebuffer;
} fbo_t;
#endif

#endif /* TYPE_H_INCLUDED */
