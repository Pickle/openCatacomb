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

#include "catacomb.h"

#include "types.h"
#include "objects.h"
#include "cat_play.h"
#include "cpanel.h"
#include "editor.h"
#include "util.h"
#include "system.h"
#include "input.h"
#include "video.h"
#include "sound.h"
#include "opengl.h"
#if defined(USE_EGL_RAW) || defined(USE_EGL_SDL) || defined(USE_EGL_RPI)
#include "eglport.h"
#endif

#include <stdio.h>

/* global variables */
playmode_t  playmode;               /* playmode : (game,demogame,demosave,editor);	{game / demo / editor} */
gamexit_t   gamexit;                /* gamexit : (quited,killed,reseted,victorious); {determines what to do after playloop} */
uint16_t    xormask;                /* xormask : word;                        {each character drawn is EOR'd} */
int16_t     sx, sy, leftedge;       /* sx, sy, leftedge : integer;            {0-39, 0-24 print cursor/return} */
int16_t     background[87][86];     /* Background : array [0..86,0..85] of integer; {base map} */
int16_t     view[87][86];           /* View : array [0..86,0..85] of integer; {base map with objects drawn in} */
int16_t     originx, originy;       /* originx, originy : integer;            {current world location of UL corn} */
uint8_t     priority[MAXPICS];      /* priority : array [0..MAXPICS] of byte;	{tile draw overlap priorities} */
int16_t     items[items_total];     /* items : array [1..5] of integer; */
int16_t     saveitems[items_total]; /* Added with C2 */
int16_t     shotpower;              /* shotpower : integer;                   {0-13 characters in power meter} */
int16_t     side;                   /* side : integer;                        {which side shots come from} */
int16_t     level;                  /* level : integer; */
int32_t     score, savescore;       /* score: longint; */
int16_t     boltsleft;              /* integer;			{number of shots left in a bolt} */

highscores_t highscores[HISCORE_MAX];         /* highscores : Array [1..5] of record */

activeobj_t o[MAXOBJ], saveo;       /* o : array [0..maxobj] of activeobj;	{everything that moves is here} */
objdesc_t   obj, altobj;            /* obj , altobj : objdesc;		{total info about objecton and alt} */
int16_t     altnum;                 /* altnum : integer;			{o[#] of altobj} */
int16_t     numobj, objecton;       /* numobj,objecton : integer;		{number of objects in O now} */

objdef_t    ObjDef[lastclass];      /* ObjDef : array [nothing..lastclass] of Record */

boolean     playdone, leveldone;    /* playdone, leveldone: boolean; */

boolean     tempb;                  /* tempb: boolean; */
uint8_t    *tempp;                  /* tempb: pointer; */

SDLKey ch;                          /*  ch: char; */
boolean altkey;                     /*  altkey:boolean;             {last key fetched by GET}*/

int16_t chkx,chky,chkspot;          /* chkx,chky,chkspot: integer;           {spot being checked by WALK}*/

dirtype_t dir;                      /*  dir: dirtype;*/
boolean button1, button2;           /* button1, button2: boolean;		{returned by playerIO}*/

uint8_t democmds[3000];             /* {bits 1-3=dir, 4=b1, 5=b2} */
uint16_t frameon;                   /* frameon: word; */
uint8_t *grmem;                     /* grmem: pointer; */
classtype_t clvar;                  /* clvar: classtype; */

uint8_t screencenterx, screencentery;

const char *sidebar = "  Level]]Score:]]Top  :]]K:]P:]B:]N:]] Shot Power]]]    Body]]";

const uint8_t altmeters[14][14] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,'\0'}, {190,1,1,1,1,1,1,1,1,1,1,1,1,'\0'},
    {190,192,1,1,1,1,1,1,1,1,1,1,1,'\0'}, {190,191,192,1,1,1,1,1,1,1,1,1,1,'\0'},
    {190,191,191,192,1,1,1,1,1,1,1,1,1,'\0'}, {190,191,191,191,192,1,1,1,1,1,1,1,1,'\0'},
    {190,191,191,191,191,192,1,1,1,1,1,1,1,'\0'}, {190,191,191,191,191,191,192,1,1,1,1,1,1,'\0'},
    {190,191,191,191,191,191,191,192,1,1,1,1,1,'\0'}, {190,191,191,191,191,191,191,191,192,1,1,1,1,'\0'},
    {190,191,191,191,191,191,191,191,191,192,1,1,1,'\0'}, {190,191,191,191,191,191,191,191,191,191,192,1,1,'\0'},
    {190,191,191,191,191,191,191,191,191,191,191,192,1,'\0'}, {190,191,191,191,191,191,191,191,191,191,191,191,193,'\0'}
};

const uint8_t meters[14][14] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,'\0'}, {194,1,1,1,1,1,1,1,1,1,1,1,1,'\0'},
    {194,196,1,1,1,1,1,1,1,1,1,1,1,'\0'}, {194,195,196,1,1,1,1,1,1,1,1,1,1,'\0'},
    {194,195,195,196,1,1,1,1,1,1,1,1,1,'\0'}, {194,195,195,195,196,1,1,1,1,1,1,1,1,'\0'},
    {194,195,195,195,195,196,1,1,1,1,1,1,1,'\0'}, {194,195,195,195,195,195,196,1,1,1,1,1,1,'\0'},
    {194,195,195,195,195,195,195,196,1,1,1,1,1,'\0'}, {194,195,195,195,195,195,195,195,196,1,1,1,1,'\0'},
    {194,195,195,195,195,195,195,195,195,196,2,1,1,'\0'}, {194,195,195,195,195,195,195,195,195,195,196,1,1,'\0'},
    {194,195,195,195,195,195,195,195,195,195,195,196,1,'\0'}, {194,195,195,195,195,195,195,195,195,195,195,195,197,'\0'}
};

const dirtype_t opposite[ dirtype_total ] =
{
    south,west,north,east,southwest,northwest,northeast,southeast,nodir
};

/*
{L VGAPALET.OBJ}
Procedure VGAPALET;	{not realy a procedure, just data...}
begin
end;

{$L CATASM.OBJ}
*/

/*
{=========================================}
{                                         }
{ DRAWOBJ                                 }
{ Draws the object to TILES in the proper }
{ direction and state.                    }
{                                         }
{=========================================}
*/

/*
Procedure DrawObj;
External;

Procedure EraseObj;
External;

Procedure DoAll;
External;

Procedure InitRnd (randomize:boolean);
External;

Function Random (maxval:word):WORD;
External;

Procedure WaitVBL;
External;

Procedure EGAmove;
External;

Procedure CGArefresh;
External;

Procedure EGArefresh;
External;

Procedure VGArefresh;
External;

Procedure CGAcharout (x,y,ch:integer);
external;

Procedure EGAcharout (x,y,ch:integer);
external;

Procedure VGAcharout (x,y,ch:integer);
external;

Function VideoID: integer;
external;

Procedure RLEexpand (source:pointer;dest:pointer;length:longint);
external;

Procedure RLEcompress (source:pointer;dest:pointer;length:longint);
external;
*/

/*
{==================================}
{                                  }
{ xxxCHAROUT                       }
{ Draw a single character at SX,SY }
{ in the various modes.            }
{                                  }
{==================================}
*/
/*
Procedure CharOut (x,y,ch:integer);
{call proper charout based on GRMODE}
Begin
  case graphmode of
    CGAgr: CGAcharout (x,y,ch);
    EGAgr: EGAcharout (x,y,ch);
    VGAgr: VGAcharout (x,y,ch);
  end;
End;
*/

/* moved to video */

/*
{======================================}
{                                      }
{ PLAYSOUND                            }
{ Starts a sound playing.  Sounds play }
{ until they are through, then quit.   }
{                                      }
{======================================}
*/

/*
Procedure PlaySound (soundnum: soundtype);
Begin
{$ifdef sounds}
  if playmode<>demogame then        {demo is allways silent}
    PlaySound1 (integer(soundnum));
{$endif}
End;
*/

/* Moved to sound.c */

/*
{========================================}
{                                        }
{ GETGRMODE                              }
{ SHows the title page and gets graphic  }
{ mode from user.                        }
{                                        }
{========================================}
*/
/*
Procedure GetGrMode;
var
  screen: byte absolute $b800:0000;
  gotmode: boolean;
  source: pointer;
  vidcard: integer;
Begin
{
; Subsystem ID values:
; 	 0  = (none)
; 	 1  = MDA
; 	 2  = CGA
; 	 3  = EGA
; 	 4  = MCGA
; 	 5  = VGA
; 	80h = HGC
; 	81h = HGC+
; 	82h = Hercules InColor
;
}

  regs.ax:=$0004;
  intr($10,regs);   {set graphic mode to 320*200 * 4 color}
  source := @titlescr;
  move (source^,screen,$4000);

  vidcard:=VideoID;

  gotmode := false;

  ch:=upcase(readkey);
  case ch of
    'C': Begin
	   graphmode:=CGAgr;
	   gotmode:=true;
	 end;
    'E': if (vidcard=3) or (vidcard=5) then
	 begin
	   graphmode:=EGAgr;
	   gotmode:=true;
	 end;
 {$IFNDEF SAMPLER}
    'V': if (vidcard=4) or (vidcard=5) then
	 begin
	   graphmode:=VGAgr;
	   gotmode:=true;
	 end;
 {$ENDIF}
  end;
  if not gotmode then
  begin
    if (vidcard=3) or (vidcard=5) then
      graphmode:=EGAgr
    else
      graphmode:=CGAgr;
  end
end;
*/

void GetGrMode( void )
{
    SDL_Rect dest;
    boolean gotmode = False;
#if !defined(CATACOMB1)
    if (episode == catacomb2)
    {
        SDLKey key;

        screencenterx = 19;
        screencentery = 11;

        SetTileView( 1, TILEVIEW_W, TILEVIEW_H );
        DrawWindowExpand( 33,13 );
        Print( "  Softdisk Publishing presents]]");
        Print( "          The Catacomb]]");
        Print( "        By John Carmack]]" );
        Print( "       Copyright 1990-93]");
        Print( "      Softdisk Publishing" );
        Print( "]]");
        Print( "]]");
        Print( "         Press a key:" );

        GetKey( &key, False );
    }
#endif

    dest.x = dest.y = 0;
    dest.w = BASE_W*SCALE;
    dest.h = BASE_H*SCALE;

    screencenterx = 11;
    screencentery = 11;

    while (gotmode == False)
    {
        if (episode == catacomb1) {
            OGL_DrawImage( &screen_title, False, &dest, NULL );
        } else {
            OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_title] );
        }

        InputPollEvents();

        VideoSwapBuffers();

#if defined(CATACOMB1)
        if (keydown[SDLK_c])
        {
            config.graphmode = CGAgr;
            gotmode = True;
        }
        if (keydown[SDLK_e])
        {
            config.graphmode = EGAgr;
            gotmode = True;
        }
        if (keydown[SDLK_v])
        {
            config.graphmode = VGAgr;
            gotmode = True;
        }
#else
        SDL_Delay(2000);
        gotmode = True;
#endif
    }
}

/*
{=================================}
{                                 }
{ PRINT                           }
{ Prints the string to the screen }
{ at SX,SY. ']' does a return to  }
{ LEFTEDGE.                       }
{ Automatically converts to lower }
{ case set for window drawing     }
{                                 }
{=================================}
*/
/*
Procedure Print (s:string);
Var
  i,cn:integer;
Begin
  For i:=1 to length (s) do
    If s[i]=']' then
      Begin
        sy:=sy+1;
        sx:=leftedge;    {return to left margin, and down a line}
      End
    Else
      Begin
	cn:=ord(s[i]);
	if (cn>=ord('a')) and (cn<=ord('z')) then
	  cn:=cn-32;
	charout (sx,sy,cn);
        sx:=sx+1;
      End;
End;
*/

void Print( const char *s, ... )
{
    uint16_t i;
    uint8_t cn;
    va_list fmtargs;
    char buffer[4096];

    va_start( fmtargs, s );
    vsnprintf( buffer, sizeof(buffer)-1, s, fmtargs );
    va_end( fmtargs );

    for (i=0; i<strlen(buffer); i++)
    {
        if (buffer[i] == ']')
        {
            sy++;
            sx = leftedge;  /* {return to left margin, and down a line} */
        }
        else
        {
            cn = buffer[i];
            if ((cn>='a') && (cn<='z'))
            {
                cn -= 32;
            }

            CharOut( sx,sy,cn );
            sx++;
        }
    }
}

void PrintArray( const uint8_t *s, uint32_t len )
{
    uint16_t i;

    for (i=0; i<len; i++)
    {
        if (s[i] == ']' || s[i] == '\0')
        {
            sy++;
            sx = leftedge;  /* {return to left margin, and down a line} */
        }
        else
        {
            CharOut( sx,sy,s[i] );
            sx++;
        }
    }
}

/*
{====================}
{                    }
{ SHORTNUM / LONGNUM }
{ PRINT's the number }
{                    }
{====================}
*/
/*
Procedure ShortNum (i:integer);
Var
  s: string [10];
  e: integer;
Begin
  str (i:1,s);
  print (s);
End;
*/
/*
Procedure LongNum (i:longint);
Var
  s: string [10];
  e: integer;
Begin
  str (i:1,s);
  print (s);
End;
*/

/* replaced with enhanced Print */

/*
{==============================}
{                              }
{ xxxREFRESH                   }
{ Refresh the changed areas of }
{ the tiles map in the various }
{ graphics modes.              }
{                              }
{==============================}
*/
/*
Procedure Refresh;
const
  demowin : array[0..4] of string[16] =
  (#14#15#15#15#15#15#15#15#15#15#15#15#15#15#15#16,
   #17' --- DEMO --- '#18,
   #17'SPACE TO START'#18,
   #17'F1 TO GET HELP'#18,
   #19#20#20#20#20#20#20#20#20#20#20#20#20#20#20#21);
var
  x,y,basex,basey: integer;
  underwin : array[0..4,0..15] of word;
Begin
  basex:=originx+4;
  basey:=originy+17;
  if playmode=demogame then
    for y:=0 to 4 do
      for x:=0 to 15 do
	begin
	  underwin[y,x]:=view[y+basey,x+basex];
	  view[y+basey,x+basex]:=word(demowin[y][x+1]) and $00ff;
	end;

  WaitVBL;
  case graphmode of
    CGAgr: CGArefresh;
    EGAgr: EGArefresh;
    VGAgr: VGArefresh;
  end;
  if playmode=demogame then
    for y:=0 to 4 do
      for x:=0 to 15 do
	view[y+basey,x+basex]:=underwin[y,x];
  WaitVBL;
End;
*/
/*
Procedure SimpleRefresh;
Begin
  WaitVBL;
  case graphmode of
    CGAgr: CGArefresh;
    EGAgr: EGArefresh;
    VGAgr: VGArefresh;
  end;
end;
*/

void Refresh( void )
{
    const char demowin[5][17] = {
        {14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,'\0'},
        {17,' ','-','-','-',' ','D','E','M','O',' ','-','-','-',' ',18,'\0'},
        {17,'S','P','A','C','E',' ','T','O',' ','S','T','A','R','T',18,'\0'},
        {17,'F','1',' ','T','O',' ','G','E','T',' ','H','E','L','P',18,'\0'},
        {19,20,20,20,20,20,20,20,20,20,20,20,20,20,20,21,'\0'}
    };

    int16_t x,y,basex,basey;
    uint16_t underwin[5][16];

    basex = originx+4;
    basey = originy+17;
    if (playmode==demogame)
    {
        for (y=0; y<=4; y++)
        {
            for (x=0; x<=15; x++)
            {
                underwin[y][x] = view[y+basey][x+basex];
                view[y+basey][x+basex] = (uint16_t)(demowin[y][x]) & 0x00ff;
            }
        }
    }

    RefreshTiles( &textiles[config.graphmode] );

    if (playmode==demogame)
    {
        for (y=0; y<=4; y++)
        {
            for (x=0; x<=15; x++)
            {
                view[y+basey][x+basex] = underwin[y][x];
            }
        }
    }

#if !defined(CATACOMB1)
    if (episode == catacomb2 && playmode != intro)
    {
        SDL_Rect dest = { 26*TILE_W, 18*TILE_H, 0, 0 };
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_side] );
    }
#endif

    VideoSwapBuffers();
}


/*
{======================================}
{                                      }
{ RESTORE                              }
{ Redraws every tile on the tiled area }
{ by setting oldtiles to -1.  Used to  }
{ erase any temporary windows.         }
{                                      }
{======================================}
*/
/*
Procedure ClearOld;
Begin
  fillchar (oldtiles,sizeof(oldtiles),chr($FF)); {clear all oldtiles}
end;

Procedure Restore;
Var
 i,j:integer;
Begin
  clearold;
  SimpleRefresh;
End;
*/


void ClearOld( void )
{
    SetTileView( 0, 24, TILEVIEW_H );
}

void Restore( void )
{
    ClearOld();
    Refresh();
}

/*
{===============================}
{                               }
{ DRAWWINDOW                    }
{ Draws a window that will fill }
{ the given rectangle.  The text}
{ area of the window DOES NOT   }
{ go to the edge.  A 3-D effect }
{ is produced.                  }
{                               }
{===============================}
*/
/*
Procedure DrawWindow (left,top,right,bottom:integer);
Var
  x,y:integer;
Begin
  charout (left,top,14);
  for x:=left+1 to right-1 do
    charout (x,top,15);
  charout (right,top,16);

  for y:=top+1 to bottom-1 do
    begin
      charout (left,y,17);
      for x:=left+1 to right-1 do
	charout (x,y,32);
      charout (right,y,18);
    end;

  charout (left,bottom,19);
  for x:=left+1 to right-1 do
    charout (x,bottom,20);
  charout (right,bottom,21);

  sx:=left+1;
  leftedge:=sx;
  sy:=top+1;
End;
*/

void DrawWindow( int16_t left, int16_t top, int16_t right, int16_t bottom )
{
    int16_t x,y;

    CharOut( left,top,14 );
    for (x=left+1; x<=right-1; x++)
    {
        CharOut( x,top,15 );
    }
    CharOut( right,top,16 );

    for (y=top+1; y<=bottom-1; y++)
    {
        CharOut( left,y,17 );
        for (x=left+1; x<=right-1; x++)
        {
            CharOut( x,y,32 );
        }
        CharOut( right,y,18 );
    }
    CharOut( left,bottom,19 );

    for (x=left+1; x<=right-1; x++)
    {
        CharOut( x,bottom,20 );
    }
    CharOut( right,bottom,21 );

    sx = left+1;
    leftedge = sx;
    sy = top+1;
}

/*
{======================}
{                      }
{ CENTERWINDOW         }
{ Centers a drawwindow }
{ that can hold a TEXT }
{ area of width/height }
{======================}
*/
/*
Procedure CenterWindow (width,height:integer);
Var
  x1,y1 : integer;
Begin
  if width>2 then
    centerwindow (width-2,height);
{  restore; }
  WaitVBL;
  x1:=11-width div 2;
  y1:=11-height div 2;
  DrawWindow (x1,y1,x1+width+1,y1+height+1);
End;
*/

void CenterWindow( int16_t width, int16_t height )
{
    int16_t x,y;

    if (width>2)
    {
        CenterWindow( width-2,height );
    }

    x = screencenterx-width / 2;
    y = screencentery-height / 2;
    DrawWindow( x, y, x+width+1, y+height+1 );
}

/*
///////////////////////////////
//
// expwin {h / v}
// Grows the window outward
//
///////////////////////////////
*/

#if !defined(CATACOMB1)
void DrawWindowExpand( uint8_t width, int height )
{
    if (width > 2)
    {
        if (height > 2)
            DrawWindowExpand( width-2,height-2 );
        else
            DrawWinExpHorz( width-2,height );
    }
    else if (height >2) {
        DrawWinExpVert( width,height-2 );
    }

    CenterWindow( width, height );
}

void DrawWinExpHorz( uint8_t width, int height )
{
    if (width > 2)
        DrawWinExpHorz( width-2,height );

    Refresh();

    CenterWindow( width, height );
}

void DrawWinExpVert( uint8_t width, int height )
{
    if (height >2)
        DrawWinExpVert( width,height-2 );

    Refresh();

    CenterWindow( width, height );
}
#endif

/*
{==============}
{              }
{ ClearKeyDown }
{              }
{==============}
*/
/*
Procedure ClearkeyDown;
var
  ch: char;
Begin
  fillchar (keydown,sizeof(keydown),0);
  while keypressed do
    ch:=readkey;
end;
*/

/* Not used */

/*
{================================}
{                                }
{ GET                            }
{ Basic keyboard input routine   }
{ returns upercase only.  Cursor }
{ appears at SX,SY               }
{ F keys are NOT handled         }
{================================}
*/
/*
Procedure Get (var ch:char);
Var
  cycle,direc,a:integer;
Begin
  clearkeydown;
  cycle:=10;
  direc:=1;
  While not Keypressed do
    Begin
      charout (sx,sy,cycle);
      for a:=1 to 5 do
	waitvbl;
      cycle:=cycle+direc;
      if cycle=14 then
        Begin
          direc:=-1;
          cycle:=13;
        end;
      if cycle=9 then
        Begin
          direc:=1;
          cycle:=10;
        end
    End;
  charout (sx,sy,ord(' '));
  altkey:=false;
  ch:=upcase(readkey);
  if ch=chr(0) then
    begin
      altkey:=true;
      ch:=readkey;
    end;
  clearkeydown;
end;
*/

uint16_t GetKey( SDLKey *key, boolean cursor )
{
    SDL_Event event;
    uint16_t cycle, a, unicode;
    int16_t direc;
    boolean gotkey = False;

    altkey = False;
    cycle = 10;
    direc = 1;
    unicode = 0;

    SDL_Delay( 100 );
    while (SDL_PollEvent(&event)) {}; /* Flush events */

    while (gotkey == False)
    {
        if (cursor == True)
        {
            CharOut( sx,sy,cycle );
            for (a=0; a<5; a++)
            {
                cycle = cycle+direc;
                if (cycle>=14)
                {
                    direc = -1;
                    cycle = 13;
                }
                if (cycle<=9)
                {
                    direc = 1;
                    cycle = 10;
                }
            }
        }

        Refresh();

        SDL_PollEvent(&event);

        if (event.type == SDL_KEYDOWN)
        {
            if ((event.key.keysym.sym == SDLK_LALT) ||
                (event.key.keysym.sym == SDLK_RALT)    )
            {
                altkey = True;
            }
            else
            {
                *key = event.key.keysym.sym;
                unicode = event.key.keysym.unicode;
                gotkey = True;
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            if ((event.key.keysym.sym == SDLK_LALT) ||
                (event.key.keysym.sym == SDLK_RALT)    )
            {
                altkey = False;
            }
        }
    }

    return unicode;
}

void GetButton( uint8_t *joy, boolean cursor )
{
    SDL_Event event;
    uint16_t cycle, a;
    int16_t direc;
    boolean gotbutton = False;

    altkey = False;
    cycle = 10;
    direc = 1;

    SDL_Delay( 100 );
    while (SDL_PollEvent(&event)) {}; /* Flush events */

    while (gotbutton == False)
    {
        if (cursor == True)
        {
            CharOut( sx,sy,cycle );
            for (a=0; a<5; a++)
            {
                cycle = cycle+direc;
                if (cycle>=14)
                {
                    direc = -1;
                    cycle = 13;
                }
                if (cycle<=9)
                {
                    direc = 1;
                    cycle = 10;
                }
            }
        }

        Refresh();

        SDL_PollEvent(&event);

        if (event.type == SDL_JOYBUTTONDOWN)
        {
            {
                *joy = event.jbutton.button;
                gotbutton = True;
            }
        }
    }
}

/*
{============================================}
{                                            }
{ INPUT                                      }
{ Reads a string of LENGTH from the keyboard }
{ Cursor is displayed at SX,SY               }
{                                            }
{============================================}
*/
/*
Procedure Input (var s:string; length:integer);
var
  i:integer;
Begin
  i:=1;
  Repeat
   get (ch);
   if altkey or (ord(ch)=8) then
     Begin
       if (i>1) and ( (ord(ch)=75) or (not altkey)) then{backspace}
	 begin
	   dec(i);
	   sx:=sx-1;
	  end;
     end
   else
     begin
       if (ch>=' ') and (ch<chr(127)) and (i<=length) then
	 Begin
	   charout (sx,sy,ord(ch));
	   s[i]:=ch;
	   inc (i);
	   inc(sx);
	 end;
     end;

   if ch=chr(27) then
     s[1]:=ch;
  until (ch=chr(13)) or (ch=chr(27));
  s[0]:=chr(i-1);
end;
*/

void Input( char *s, int16_t length )
{
    int32_t total = 0;
    uint16_t unicode;
    SDLKey key = SDLK_UNKNOWN;

    while (total<=length && key != inputmaps[input_select].key)
    {
        unicode = GetKey( &key, True );

        if ((key >= SDLK_0 && key <= SDLK_9) || (key >= SDLK_a && key <= SDLK_z))
        {
            s[total] = toupper(unicode);

            CharOut( sx, sy, s[total] );
            sx++;

            total++;
        }
        else if (total > 0 && key == SDLK_BACKSPACE)
        {
            CharOut( sx, sy, ' ' );
            sx--;

            total--;
        }
    }
    s[length+1] = '\0';
}

/*
{===========================}
{                           }
{ CHECKKEYS                 }
{ If a key has been pressed }
{ it will be assigned to CH/}
{ altkey, and if it is an F }
{ key, it will be processed.}
{                           }
{===========================}
*/
/*
procedure CheckKeys;
*/

/*
    Function Wantmore:boolean;
    Begin
      sx:=2;
      sy:=20;
      Print ('(SPACE for more/ESC)');
      sx:=12;
      sy:=21;
      get (ch);
      if ch=chr(27) then
        wantmore:=false
      else
        wantmore:=true;
    end;
*/

boolean Wantmore(void )
{
    sx=2;
    sy=20;
    Print( "  (%s for more)] (%s to go back)", inputmaps[input_select].keyname, inputmaps[input_back].keyname );
    sx=12;
    sy=22;

    do {
        GetKey( &ch, True );

        if (ch==inputmaps[input_back].key) {
            return False;
        } else if (ch==inputmaps[input_select].key) {
            return True;
        } else {
            SDL_Delay(10);
        }

    } while ((ch != inputmaps[input_back].key) && (ch != inputmaps[input_select].key));

    return False;
}

/*
    {         }
    { DrawPic }
    {         }
*/
/*
    Procedure DrawPic(x,y:integer; c:classtype; dir:dirtype; stage:integer);
    var
      xx,yy,size,tilenum:integer;
    Begin
      size:=ObjDef[c].size;
      tilenum:=ObjDef[c].firstchar+size*size
	*((integer(dir) and ObjDef[c].dirmask)*ObjDef[c].stages+stage);

      For yy:=y to y+size-1 do
        for xx:=x to x+size-1 do
          Begin
            charout (xx,yy,tilenum);
            inc(tilenum);
          end;
    End;
*/

void DrawPic( int16_t x, int16_t y, classtype_t c, dirtype_t direc, int16_t stage )
{
    int16_t xx,yy,size,tilenum;

    size = ObjDef[c].size;
    tilenum = ObjDef[c].firstchar+size*size*(((int16_t)(direc) & ObjDef[c].dirmask)*ObjDef[c].stages+stage);

    for (yy=y; yy<=y+size-1; yy++)
    {
        for (xx=x; xx<=x+size-1; xx++)
        {
            CharOut( xx,yy,tilenum );
            tilenum++;
        }
    }
}

void CheckKeys( void )
{
    InputPollEvents();

    if (inputmaps[input_help].pressed)
    {
        Help();
        Restore();
    }
    if (inputmaps[input_sound].pressed)
    {
        SoundChange();
        Restore();
    }
    if (inputmaps[input_control].pressed)
    {
#if defined(CATACOMB1)
        InputChange();
#else
        ControlPanel();
#endif
        Restore();
    }
    if (inputmaps[input_reset].pressed)
    {
        Reset();
        Restore();
    }

    if (inputmaps[input_save].pressed)
    {
        SaveGame();
        Restore();
    }

    if (inputmaps[input_load].pressed)
    {
        LoadGame();
        Restore();
    }

    if (inputmaps[input_pause].pressed)
    {
        Pause();
        Restore();
    }

    if (inputmaps[input_quit].pressed)
    {
        QuitGame();
        Restore();
    }
}

void Help( void )
{
    int16_t x, y;

    CenterWindow(20,22);
    Print( "  C A T A C O M B   ]" );
    Print( "   - - - - - - -    ]" );
    Print( " By John Carmack &  ]" );
    Print( "     PC Arcade      ]" );
    Print( "]" );
    Print( "%s = Help           ]", inputmaps[input_help].keyname );
#if defined(CATACOMB1)
    Print( "%-3s= Sound on / off ]", inputmaps[input_sound].keyname );
    Print( "%-3s= Controls       ]", inputmaps[input_control].keyname );
    Print( "%-3s= Game reset     ]", inputmaps[input_reset].keyname );
#else
    Print( "%-3s= Control Panel  ]", inputmaps[input_control].keyname );
    Print( "%-3s= Game reset     ]", inputmaps[input_reset].keyname );
    Print( "%-3s= Save game      ]", inputmaps[input_save].keyname );
    Print( "%-3s= Load game      ]", inputmaps[input_load].keyname );
#endif /* CATACOMB1 */
    Print( "%-3s= Pause          ]", inputmaps[input_pause].keyname );
    Print( "%-3s= Quit           ]", inputmaps[input_quit].keyname );
    Print( "]" );
#if defined(CATACOMB1)
    Print( "Watch the demo for  ]" );
    Print( "a play example.     ]" );
    Print( "]" );
#endif /* CATACOMB1 */
    Print( "Hit fire at the demo]" );
    Print( "to begin playing.   ]" );
    if (!Wantmore())
        return;

    CenterWindow(20,22);
    Print( "]Keyboard controls:  ]]" );
    Print( "Move    : Arrows    ]" );
    Print( "Button1 : %-9s ]", inputmaps[input_shoot].keyname );
    Print( "Button2 : %-9s ]", inputmaps[input_strafe].keyname );
    Print( "]To switch to mouse ]" );
    Print( "or joystick control,]" );
#if defined(CATACOMB1)
    Print( "hit %-14s. ]", inputmaps[input_control].keyname );
#else
    Print( "hit %-14s. ]", inputmaps[input_control].keyname );
#endif /* CATACOMB1 */

    if (!Wantmore())
        return;

    CenterWindow(20,22);
    Print( "Button 1 / Fire key:]" );
    Print( "Builds shot power.  ]" );
    Print( "If the shot power   ]" );
    Print( "meter is full when  ]" );
    Print( "the button is       ]" );
    Print( "released, a super   ]" );
    Print( "shot will be        ]" );
    Print( "launched.           ]" );
    Print( "]" );
    for (y=11; y<=18; y++)
    {
        for (x=3; x<=20; x++)
        {
            CharOut(x,y,128);
        }
    }

    DrawPic(4,14,player,east,2);
    DrawPic(19,15,shot,east,1);
    DrawPic(17,14,shot,east,0);
    DrawPic(15,15,shot,east,1);
    DrawPic(8,14,bigshot,east,0);

    if (!Wantmore())
        return;

    CenterWindow(20,22);
    Print( "Button 2/Strafe key:]" );
    Print( "Allows you to move  ]" );
    Print( "without changing the]" );
    Print( "direction you are   ]" );
    Print( "facing.  Good for   ]" );
    Print( "searching walls and ]" );
    Print( "fighting retreats.  ]" );
    for (y=11; y<=18; y++)
    {
        for (x=3; x<=20; x++)
        {
            if (y==15) {
                CharOut(x,y,129);
            } else if (y==16) {
                CharOut(x,y,131);
            } else {
                CharOut(x,y,128);
            }
        }
    }
    DrawPic(6,13,player,south,2);
    sx=6;
    sy=15;

    Print( "\35\35\36\36\37\37\0" );

    if (!Wantmore())
        return;

    CenterWindow(20,22);
    Print( "%s will ]", inputmaps[input_potion].keyname );
    Print( "take a healing      ]" );
    Print( "potion if you have  ]" );
    Print( "one.  This restores ]" );
    Print( "the body meter to   ]" );
    Print( "full strength.  Keep]" );
    Print( "a sharp eye on the  ]" );
    Print( "meter, because when ]" );
    Print( "it runs out, you are]" );
    Print( "dead!               ]]" );
    Print( "%s will cast a bolt]", inputmaps[input_bolt].keyname );
    Print( "spell if you have   ]" );
    Print( "any.  You can mow   ]" );
    Print( "down a lot of       ]" );
    Print( "monsters with a bit ]" );
    Print( "of skill.           ]" );

    if (!Wantmore())
        return;

    CenterWindow(20,22);
    Print( "%s will ]", inputmaps[input_nuke].keyname );
    Print( "cast a nuke spell.  ]" );
    Print( "This usually wipes  ]" );
    Print( "out all the monsters]" );
    Print( "near you.  Consider ]" );
    Print( "it a panic button   ]" );
    Print( "when you are being  ]" );
    Print( "mobbed by monsters! ]]" );
    Print( "               \200\200\200]" );
    Print( "Potions:       \200\242\200]" );
    Print( "               \200\200\200]" );
    Print( "Scrolls:       \200\243\200]" );
    Print( " (bolts/nukes) \200\200\200]" );
    Print( "Treasure:      \200\247\200]" );
    Print( " (points)      \200\200\200]" );
    Print( "               \200\200\200]" );

    if (!Wantmore())
        return;
}

/*
  {             }
  { SoundChange }
  {             }
*/
/*
  Procedure SoundChange;
  label
    select;
  Begin
    CenterWindow (15,1);
    Print ('Sound: ]');
select:
    sx:=11;
    sy:=12;
    if soundon then
      xormask:=$FFFF
    else
      xormask:=0;
    Print (' ON ');
    xormask:=xormask xor $FFFF;
    Print (' OFF ');
    xormask:=0;
    sx:=10;
    Get (ch);
    if altkey and ( (ord(ch)=75) or (ord(ch)=77) ) then
      Begin
        soundon:=not soundon;
        goto select;
      end
  end;
*/

void SoundChange( void )
{
    CenterWindow(15,1);
    Print( "Sound: ]" );

select:
    sx=11;
    sy=12;
    if (config.soundmode == sound_on) {
        xormask=0xFFFF;
    } else {
        xormask=0;
    }
    Print( " ON " );
    xormask=xormask ^ 0xFFFF;
    Print( " OFF " );
    xormask=0;
    sx=10;
    GetKey( &ch, True );
    /* if (altkey && ( (ch==75) || (ch==77) )) */
    if (altkey && (ch==SDLK_s))
    {
        /* soundon != soundon; */
        goto select;
    }
}

/*
  {             }
  { InputChange }
  {             }
*/
void InputChange( void )
{
    /*
    int16_t oldmode;
  procedure calibrate;
  var
    xl,yl,xh,yh,ox,dx,dy: integer;
  begin
    restore;
    centerwindow (20,9);
    Print ('Joystick calibration]');
    Print ('--------------------]');
    Print ('Push the joystick to]');
    Print ('the UPPER LEFT and]');
    ox:=sx+10;
    Print ('hit fire:(');
    Repeat
      sx:=ox;
      Rd_Joy (1,xl,yl);
      shortnum (xl);
      print (',');
      shortnum (yl);
      print (')  ');
      Rd_Joystick1 (dir,button1,button2);
    until keypressed or button1 or button2;
    while button1 or button2 do
      Rd_Joystick1 (dir,button1,button2);

    Print (']]Push the joystick to]');
    Print ('the LOWER RIGHT and]');
    Print ('hit fire:(');
    Repeat
      sx:=ox;
      Rd_Joy (1,xh,yh);
      shortnum (xh);
      print (',');
      shortnum (yh);
      print (')  ');
      Rd_Joystick1 (dir,button1,button2);
    until keypressed or button1 or button2;
    while button1 or button2 do
      Rd_Joystick1 (dir,button1,button2);

    dx:=(xh-xl) div 4;
    dy:=(yh-yl) div 4;
    joy_xlow:=xl+dx;
    joy_xhigh:=xh-dx;
    joy_ylow:=yl+dy;
    joy_yhigh:=yh-dy;

  end;

  Begin
    oldmode:=ord(inpmode);
    CenterWindow (15,5);
    Print ('Player Control:]]');
switch:
    sx:=leftedge;
    sy:=12;
    if inpmode=kbd then
      xormask:=$FFFF
    else
      xormask:=0;
    Print ('   KEYBOARD    ]');
    if inpmode=joy then
      xormask:=$FFFF
    else
      xormask:=0;
    Print ('   JOYSTICK    ]');
    if inpmode=mouse then
      xormask:=$FFFF
    else
      xormask:=0;
    Print ('     MOUSE     ]');
    xormask:=0;

    sx:=12;
    sy:=11;
    Get (ch);
    if altkey and ( (ord(ch)=80) or (ord(ch)=77) ) then
      Begin
        if inpmode=mouse then
          inpmode:=kbd
        else
          inpmode:=succ(inpmode);
        goto switch;
      end;

    if altkey and ( (ord(ch)=72) or (ord(ch)=75) ) then
      Begin
        if inpmode=kbd then
          inpmode:=mouse
        else
          inpmode:=pred(inpmode);
        goto switch;
      end;

    if inpmode=mouse then
      Begin
	if not mouseok then
	  begin
	    playsound (blockedsnd);
	    goto switch;
	  end;
        regs.ax:=0;
        intr($33,regs);   {initialize the mouse}
      end;

    if (inpmode=joy) { and (oldmode<>ord(joy)) } then
      calibrate;
  end;
  */
}

/*
  {       }
  { Reset }
  {       }
*/
void Reset( void )
{
    CenterWindow(18,1);
    Print( "Reset game (Y/N)?" );

    do {
        GetKey( &ch, True );

        if (ch==SDLK_y) {
            gamexit=killed;
            playdone=True;
        } else {
            SDL_Delay(10);
        }

    } while (ch != SDLK_y && ch != SDLK_n);
}

/*
  {       }
  { Pause }
  {       }
*/
void Pause( void )
{
    CenterWindow(7,1);
    Print( "PAUSED" );
    GetKey( &ch, True );
}

/*
  {          }
  { QuitGame }
  {          }
*/
void QuitGame( void )
{
    CenterWindow(12,1);
    Print( "Quit (Y/N)?" );

    do {
        GetKey( &ch, True );

        if (ch==SDLK_y) {
            SystemShutdown();
            exit(0);
        } else {
            SDL_Delay(5);
        }

    } while (ch != SDLK_y && ch != SDLK_n);
}

void SaveGame( void )
{
    char filename[15];
    FILE *file = NULL;

    DrawWindowExpand( 22, 4 );

    if (playmode != game)
    {
        Print( "Can't save game here!" );
        GetKey( &ch, False );
        return;
    }

    Print( "Save as game #(1-9):" );
    while (ch<SDLK_1 || ch>SDLK_9) {
        GetKey( &ch, False );
    }

    snprintf( filename, sizeof(filename), "%s/GAME%d.%s", pathdata, ch-SDLK_0, filelist[episode][file_ext] );

    file = fopen( filename, "rb" );

    if (file != NULL)
    {
        fclose( file );

        Print( "]Game exists,]overwrite (Y/N)?" );
        while (ch != SDLK_y && ch != SDLK_n) {
            GetKey( &ch, False );
            if (ch == SDLK_n) {
                return;
            }
        }
    }

    file = fopen( filename, "wb" );

    if (file == NULL)
    {
        Print( "]Error opening file" );
        GetKey( &ch, False );
        return;
    }

    sx = leftedge;
    Print( " " );
    sy--;
    sx = leftedge;
    Print( " " );
    sx = leftedge;
    sy--;

    fwrite( &saveitems, 1, sizeof(saveitems), file );
    fwrite( &savescore, 1, sizeof(savescore), file );
    fwrite( &level, 1, sizeof(level), file );
    fwrite( &saveo, 1, sizeof(activeobj_t), file );
    fclose( file );

    Print( "]Game saved. Hit %s]", inputmaps[input_save].keyname );
    Print( "when you wish to]" );
    Print( "restart the game." );
    GetKey( &ch, True );
}

void LoadGame( void )
{
    char filename[15];
    FILE *file = NULL;

    DrawWindowExpand( 22, 4 );
    Print( "Load game #(1-9):" );

    while (ch<SDLK_1 || ch>SDLK_9) {
        GetKey( &ch, False );
    }

    snprintf( filename, sizeof(filename), "%s/GAME%d.%s", pathdata, ch-SDLK_0, filelist[episode][file_ext] );

    file = fopen( filename, "rb" );

    if (file == NULL)
    {
        Print( "]Error opening file" );
        GetKey( &ch, False );
        return;
    }

    fread( &items, 1, sizeof(items), file );
    fread( &score, 1, sizeof(score), file );
    fread( &level, 1, sizeof(level), file );
    fread( &o[0], 1, sizeof(o[0]), file );
    fclose( file );

    //exitdemo = True;
    if (playmode != game)
        playdone = True;
    //drawside ();	// draw score, icons, etc
    leveldone = True;
}

/*
{=====================================}
{                                     }
{ PlayerIO                            }
{ Checks for special keys, then calls }
{ apropriate control routines:        }
{ KBDINPUT, JOYINPUT, MOUSEINPUT      }
{ then does any needed updating, etc  }
{                                     }
{=====================================}
*/
/*
Procedure PlayerIO (var direc: dirtype; var button1,button2: boolean);

Begin

{check for commands to switch modes or quit, etc}

  CheckKeys;
  case inpmode of
    kbd: Rd_Keyboard (direc,button1,button2);
    mouse: Rd_Mouse (direc,button1,button2);
    joy: Rd_Joystick1 (direc,button1,button2);
  end;
End;
*/

void PlayerIO( dirtype_t *direc, boolean *b1, boolean *b2 )
{
    /* {check for commands to switch modes or quit, etc} */

    CheckKeys();

    switch (config.inputmode)
    {
        case mouse:
            Rd_Mouse(direc,b1,b2);
            break;
        case joystick1:
            Rd_Joystick( 0, direc,b1,b2);
            break;
        case joystick2:
            Rd_Joystick( 1, direc,b1,b2);
            break;
        case keyboard:
        default:
            Rd_Keyboard(direc,b1,b2);
            break;
    }
}

/*
{==============================}
{                              }
{ IOERROR                      }
{ Handle errors, allowing user }
{ to abort the program if they }
{ want to, or try over.        }
{                              }
{==============================}
*/

/* dont use */


/* {=========================================================================} */

/*
{=====================================================}
{                                                     }
{ PARALIGN                                            }
{ Sets the heap so next variable will be PARA aligned }
{                                                     }
{=====================================================}
*/

/*
Procedure Paralign;
Var
  state: record
    case boolean of
      true: (p: pointer);
      false: (offset,segment:word);
    End;
Begin
  mark (state.p);
  If state.offset>0 then
    Begin
      state.offset:=0;
      inc(state.segment);
      release (state.p);
    end;
end;
*/

/* dont use */

/*
{========================================================}
{                                                        }
{ BLOAD                                                  }
{ Allocates paraligned memory for a file and loads it in }
{                                                        }
{========================================================}
*/

/*
function Bload (filename: string): pointer;
var
  iofile: file;
  len: longint;
  allocleft,recs: word;
  into,second: pointer;
begin
  Assign (iofile,filename);
  Reset (iofile,1);
  If ioresult<>0 then
    Begin
      writeln ('File not found: ',filename);
      halt;
    End;

  len:=filesize(iofile);
  paralign;

  if len>$fff0 then      {do this crap because getmem can only give $FFF0}
    begin
      getmem (into,$fff0);
      BlockRead (iofile,into^,$FFF0,recs);
      allocleft:=len-$fff0;
      while allocleft > $fff0 do
	begin
	  getmem (second,$fff0);
	  BlockRead (iofile,second^,$FFF0,recs);
	  allocleft:=allocleft-$fff0;
	end;
      getmem (second,allocleft);
      BlockRead (iofile,second^,$FFF0,recs);
    end
  else
    begin
      getmem (into,len);
      BlockRead (iofile,into^,len,recs);
    end;

  Close (iofile);
  bload:=into;
end;
*/

/* moved to util.c */

/*
{===================================}
{                                   }
{ INITGRAPHICS                      }
{ Loads the graphics and does any   }
{ needed maping or switching around }
{ Decides which files to load based }
{ on GRMODE                         }
{                                   }
{===================================}
*/

/*
Procedure InitGraphics;
const
  scindex = $3c4;
  scmapmask = 2;
  gcindex = $3ce;
  gcmode = 5;

Var
  iofile : file;
  x,y,memat,buff,recs,plane,planebit,t : word;
Begin
  mark (grmem);		{so memory can be released later}

  case graphmode of
    CGAgr: Begin
	     pics := ptr(seg(cgapics)+1,0);
{	     pics:=bload('CGAPICS.CAT');}
	     regs.ax:=$0004;
	     intr($10,regs);   {set graphic mode to 320*200 * 4 color}
	   end;

    EGAgr: Begin
	     pics := ptr(seg(egapics)+1,0);
{	     pics:=bload('EGAPICS.CAT');}
	     regs.ax:=$000D;
	     intr($10,regs);   {set graphic mode to 320*200 * 16 color}
	     EGAmove;		{move the tiles into latched video memory}
	   end;

    VGAgr: Begin
	     pics:=bload ('VGAPICS.CAT');
	     regs.ax:=$0013;
	     intr($10,regs);   {set graphic mode to 320*200 * 256 color}
	     regs.es:=seg(VGAPALET);
	     regs.dx:=ofs(VGAPALET);
	     regs.bx:=0;
	     regs.cx:=$100;
	     regs.ax:=$1012;
	     intr($10,regs);	{set up deluxepaint's VGA pallet}
	   end;
  end;

End;
*/

/* moved to video.c */


/*
{==============================}
{                              }
{ LOADLEVEL / SAVELEVEL        }
{ Loads map LEVEL into memory, }
{ and sets everything up.      }
{                              }
{==============================}
*/

/*
Procedure LoadLevel;
label
  tryopen,fileread;

const
  tokens: array[230..255] of classtype =
    (player,teleporter,goblin,skeleton,ogre,gargoyle,dragon,nothing,
     nothing,nothing,nothing,nothing,nothing,nothing,nothing,nothing,
     nothing,nothing,nothing,nothing,nothing,nothing,nothing,nothing,
     nothing,nothing);

Var
  filename : string;
  st: string;
  x,y,xx,yy,recs, btile : Integer;
  iofile: file;
  sm : array [0..4095] of byte;

Begin
  str(level:1,st);
  filename:=concat ('LEVEL',st,'.CAT');

tryopen:

  Assign (iofile,filename);
  Reset (iofile,1);
  If ioresult<>0 then
{file not found...}

      Begin
        Ioerror (filename);
        goto tryopen;       {try again...}
      End;

  BlockRead (iofile,packbuffer,4096,recs);
  close (iofile);

  RLEexpand (@packbuffer[4],@sm,4096);

  numobj:=0;
  o[0].x:=13;          {just defaults if no player token is found}
  o[0].y:=13;
  o[0].stage:=0;
  o[0].delay:=0;
  o[0].dir:=east;
  o[0].oldx:=0;
  o[0].oldy:=0;
  o[0].oldtile:=-1;


  for yy:=0 to 63 do
    for xx:=0 to 63 do
      Begin
        btile:=sm[yy*64+xx];
        if btile<230 then
          background[yy+topoff,xx+leftoff]:=btile
        else
          Begin

{hit a monster token}
            background[yy+topoff,xx+leftoff]:=blankfloor;
            if tokens[btile]=player then

{the player token determines where you start in level}

              Begin
                o[0].x:=xx+topoff;
                o[0].y:=yy+leftoff;
              end
            else

{monster tokens add to the object list}

              Begin
                inc(numobj);
                with o[numobj] do
                  Begin
                    active:=false;
                    class:=tokens[btile];
                    x:=xx+leftoff;
                    y:=yy+topoff;
                    stage:=0;
                    delay:=0;
		    dir:=dirtype(random(4));  {random 0-3}
		    hp:=ObjDef[class].hitpoints;
		    oldx:=x;
		    oldy:=y;
		    oldtile:=-1;
                  End;
              end;

            end;

          end;

fileread:


  originx := o[0].x-11;
  originy := o[0].y-11;

  shotpower:=0;
  for y:=topoff-1 to 64+topoff do
    for x:=leftoff-1 to 64+leftoff do
      view[y,x]:=background[y,x];

  sx:=33;                  {print the new level number on the right window}
  sy:=1;
  shortnum (level);
  Print (' ');          {in case it went from double to single digit}
  restore;
End;
*/

void LoadLevel( const char *ext )
{
    classtype_t tokens[26] = { /* 230 to 255 */
        player,teleporter,goblin,skeleton,ogre,gargoyle,dragon,nothing,
        nothing,nothing,nothing,nothing,nothing,nothing,nothing,nothing,
        nothing,nothing,nothing,nothing,nothing,nothing,nothing,nothing,
        nothing,nothing
    };

    char filename[STRSIZE];
    int16_t x, y, xx, yy, btile = 0;
    uint32_t length = 0;
    uint8_t *sm = NULL;
    uint8_t *packbuffer = NULL; /*  packbuffer: array[0..4095] of byte; */

    snprintf( filename, STRSIZE, "%s/LEVEL%d.%s", pathdata, level, ext );
    CheckPath( filename );

    length = LoadDataFromFile( filename, &packbuffer, 0, length );

    if (packbuffer != NULL)
    {
        RLEexpand( packbuffer, &sm, length );

        numobj = 0;
        o[0].x = 13;          /* {just defaults if no player token is found} */
        o[0].y = 13;
        o[0].stage = 0;
        o[0].delay = 0;
        o[0].dir = east;
        o[0].oldx = 0;
        o[0].oldy = 0;
        o[0].oldtile = -1;

        for (yy=0; yy<=63; yy++)
        {
            for (xx=0; xx<=63; xx++)
            {
                btile = sm[yy*64+xx];
                if (btile<230)
                {
                    background[yy+TOPOFF][xx+LEFTOFF] = btile;
                }
                else
                {
                    /* {hit a monster token} */
                    background[yy+TOPOFF][xx+LEFTOFF] = BLANKFLOOR;
                    if (tokens[btile-230]==player)
                    {
                        /*{the player token determines where you start in level} */
                        o[0].x = xx+TOPOFF;
                        o[0].y = yy+LEFTOFF;
                    }
                    else
                    {
                        /* {monster tokens add to the object list} */
                        numobj++;
                        o[numobj].active = False;
                        o[numobj].type = tokens[btile-230];
                        o[numobj].x = xx+LEFTOFF;
                        o[numobj].y = yy+TOPOFF;
                        o[numobj].stage = 0;
                        o[numobj].delay = 0;
                        o[numobj].dir = (dirtype_t)(Random(0,3));  /* {rand 0-3} */
                        o[numobj].hp = ObjDef[o[numobj].type].hitpoints;
                        o[numobj].oldx = o[numobj].x;
                        o[numobj].oldy = o[numobj].y;
                        o[numobj].oldtile = -1;
                    }
                }
            }
        }

        originx = o[0].x-11;
        originy = o[0].y-11;
        shotpower = 0;
        for (y=TOPOFF-1; y<=64+TOPOFF; y++) {
            for (x=LEFTOFF-1; x <=64+LEFTOFF; x++) {
                view[y][x] = background[y][x];
            }
        }

        sx=33;               /*{print the new level number on the right window} */
        sy=1;
        Print( "%d ", level );          /* {in case it went from double to single digit} */
        Restore();

        free(packbuffer);
        packbuffer = NULL;
        free(sm);
        sm = NULL;
    }
    else
    {
        printf ("ERROR: Could not load level %s\n", filename );
        SystemShutdown();
        exit(-1);
    }
}


/*
{=================================}
{                                 }
{ LOADHIGHSCORES / SAVEHIGHSCORES }
{ Loads / saves the file or creats}
{ a new one, as needed.           }
{                                 }
{=================================}
*/
/*
Procedure LoadHighScores;
Var
  iofile : file;
  recs,i: Integer;
Begin
  Assign (iofile,'TOPSCORS.CAT');
  Reset (iofile,1);
  BlockRead (iofile,highscores,sizeof(highscores),recs);
  Close (iofile);
  If (ioresult<>0) or (recs<>sizeof (highscores)) then

{create a default high score table}

    For i:=1 to 5 do
      With Highscores[i] do
        Begin
          level:=1;
          score:=100;
          initials[1]:='J';
          initials[2]:='D';
          initials[3]:='C';
        End
End;
*/

void LoadHighScores( const char *filename )
{
    uint8_t i;
    uint8_t *hs = (uint8_t*)&highscores;
    uint32_t length = sizeof(highscores);
    char path[STRSIZE];

    snprintf( path, STRSIZE, "%s/%s", pathdata, filename );

    if (LoadDataFromFile( path, &hs, 0, length ) != length)
    {
        /* {create a default high score table} */
        printf ("Creating default highscores\n" );
        for (i=0; i<HISCORE_MAX; i++)
        {
            highscores[i].level = 1;
            highscores[i].score = 100;
            highscores[i].initials[0] = 'J';
            highscores[i].initials[1] = 'D';
            highscores[i].initials[2] = 'C';
            highscores[i].initials[3] = '\0';
        }
    }
}

/*
Procedure SaveHighScores;
Var
  iofile : file;
  recs : Integer;
Begin
  Assign (iofile,'TOPSCORS.CAT');
  Rewrite (iofile,1);
  BlockWrite (iofile,highscores,sizeof(highscores),recs);
  Close (iofile);
End;
*/

void SaveHighScores( const char *filename )
{
    char path[STRSIZE];
    FILE *iofile;

    snprintf( path, STRSIZE, "%s/%s", pathdata, filename );
    iofile = fopen( path, "wb" );

    if (iofile != NULL)
    {
        fwrite( highscores, sizeof(uint8_t), sizeof(highscores), iofile);
        fclose( iofile );
    }
    else
    {
        printf ("ERROR: Could not write highscores %s\n", filename );
    }
}

/*
{=====================}
{                     }
{ LOADDEMO / SAVEDEMO }
{                     }
{=====================}
*/
/*
Procedure LoadDemo;
Var
  iofile : file;
  recs : Integer;
Begin
  Assign (iofile,'DEMO.CAT');
  Reset (iofile,1);
  BlockRead (iofile,democmds,sizeof(democmds),recs);
  Close (iofile);
End;
*/

void LoadDemo( const char *filename )
{
    uint8_t *cmds = (uint8_t*)&democmds;
    uint32_t length = sizeof(democmds);
    char path[STRSIZE];

    snprintf( path, STRSIZE, "%s/%s", pathdata, filename );

    if (LoadDataFromFile( path, &cmds, 0, length ) <= 0)
    {
        printf ("ERROR: Could not load democmds %s\n", filename );
    }
}

/*
Procedure SaveDemo;
Var
  iofile : file;
  recs : Integer;
Begin
  Assign (iofile,'DEMO.CAT');
  Rewrite (iofile,1);
  Blockwrite (iofile,democmds,sizeof(democmds),recs);
  Close (iofile);
End;
*/

void SaveDemo( const char *filename )
{
    char path[STRSIZE];
    FILE *iofile;

    snprintf( path, STRSIZE, "%s/%s", pathdata, filename );
    iofile = fopen( path, "wb" );

    if (iofile != NULL)
    {
        fwrite( democmds, sizeof(uint8_t), sizeof(democmds), iofile);
        fclose( iofile );
    }
    else
    {
        printf ("ERROR: Could not write democmds %s\n", filename );
    }
}

/*
{====================}
{                    }
{ Load the sounds in }
{                    }
{====================}
*/
/*
Procedure LoadSounds;
Begin
  SoundData:=Bload ('SOUNDS.CAT');
End;


{$i+} {i/o checking back on}

{==========================================================================}

{$i cat_play}  {the routines which handle game play}

{==========================================================================}
*/

/* moved to sound.c */

/*
{========================================}
{                                        }
{ Finished                               }
{ SHows the end page...                  }
{                                        }
{========================================}
*/
/*
Procedure Finished;
var
  screen: byte absolute $b800:0000;
  source: pointer;
Begin
  if graphmode<>CGAgr then
  begin
    regs.ax:=$0004;
    intr($10,regs);   {set graphic mode to 320*200 * 4 color}
  end;
  source := @endscr;
  move (source^,screen,$4000);

  playsound (treasuresnd);
  waitendsound;
  playsound (treasuresnd);
  waitendsound;
  playsound (treasuresnd);
  waitendsound;
  playsound (treasuresnd);
  waitendsound;

  while keypressed do
    ch:=readkey;

  ch:=upcase(readkey);

  release (grmem);
  initgraphics;
  DrawWindow (24,0,38,23);  {draw the right side window}
  Print ('  Level]]Score:]]Top  :]]K:]P:]B:]N:]]]');
  Print (' Shot Power]]]    Body]]]');
  PrintHighScore;
  PrintScore;
  PrintBody;
  PrintShotPower;


end;
*/

void Finished( void )
{
    SDL_Rect dest = { 0, 0, BASE_W*SCALE, BASE_H*SCALE };

    if (episode == catacomb1)
        OGL_DrawImage( &screen_end, False, &dest, NULL );
    else
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_end] );

    VideoSwapBuffers();

    PlaySound(treasuresnd);
    WaitEndSound();
    PlaySound(treasuresnd);
    WaitEndSound();
    PlaySound(treasuresnd);
    WaitEndSound();
    PlaySound(treasuresnd);
    WaitEndSound();

    GetKey( &ch, False );

    DrawWindow(24,0,38,23);  /* {draw the right side window} */
    Print( sidebar );
    PrintHighScore();
    PrintScore();
    PrintBody();
    PrintShotPower();
}

/*
{================================}
{                                }
{ PLAYSETUP                      }
{ Set up all data for a new game }
{ Does NOT start it playing      }
{                                }
{================================}
*/
/*
Procedure PlaySetup;
Var
  i:integer;
  lv: string;
Begin
  score:=0;
  shotpower:=0;
  level:=1;
  If keydown [$2E] and keydown [$14] then  {hold down 'C' and 'T' to CheaT!}
    Begin
      CenterWindow (16,2);
      Print ('Warp to which]level (1-99)?');
      input (lv,2);
      val (lv,level,i);
      if level<1 then
	level:=1;
      if level>20 then
	level:=20;
      restore;
    end;

  For i:=1 to 5 do
    items[i]:=0;

  with o[0] do
    Begin
      active := true;
      class := player;
      hp := 13;
      dir:=west;
      stage:=0;
      delay:=0;
    End;

  DrawWindow (24,0,38,23);  {draw the right side window}
  Print ('  Level]]Score:]]Top  :]]K:]P:]B:]N:]]]');
  Print (' Shot Power]]]    Body]]]');
  PrintHighScore;
  PrintBody;
  PrintShotPower;

{give them a few items to start with}

  givenuke;
  givenuke;
  givebolt;
  givebolt;
  givebolt;
  givepotion;
  givepotion;
  givepotion;

End;
*/

void PlaySetup( void )
{
    int16_t i;
    char lv[3];

    score = 0;
    shotpower = 0;
    level = 1;
    if (keydown[SDLK_c] && keydown[SDLK_t]) /* {hold down 'C' and 'T' to CheaT!} */
    {
        CenterWindow(16,2);
        Print( "Warp to which]level (1-99)?" );
        Input(lv,2);
        level = atoi( lv );
        if (level<1) {
            level = 1;
        }
        if (level>20) {
            level = 20;
        }
        Restore();
    }

    for (i=0; i<items_total; i++) {
        items[i]=0;
    }

    o[0].active = True;
    o[0].type = player;
    o[0].hp = 13;
    o[0].dir = west;
    o[0].stage = 0;
    o[0].delay = 0;

    ShowSidePanel();

    /* {give them a few items to start with} */
    GiveNuke();
    GiveNuke();
    GiveNuke();
    GiveBolt();
    GiveBolt();
    GiveBolt();
    GivePotion();
    GivePotion();
    GivePotion();
}

void ShowSidePanel( void )
{
    DrawWindow(24,0,39,24);  /* {draw the right side window} */
    Print( sidebar );
    PrintScore();
    PrintHighScore();
    PrintBody();
    PrintShotPower();
}

/*
{=============================}
{                             }
{ SHOWSCORES                  }
{ Draws the high score window }
{ Does NOT wait for input, etc}
{                             }
{=============================}
*/
/*
Procedure ShowScores;
var
  s: string;
  i,j:integer;
Begin
  centerwindow (14,14);
  Print (' High scores:]] SCORE LV BY] ----- -- ---]');
  for i:=1 to 5 do
    begin
      str(highscores[i].score:6,s);
      print (s);
      inc (sx);
      if (highscores[i].level=11) then
	begin
	  charout (sx+1,sy,167);
	  sx:=sx+3;
	end
      else
	begin
	  str(highscores[i].level:2,s);
	  print (s);
	  inc (sx);
	end;
      for j:=1 to 3 do
	print (highscores[i].initials[j]);
      print (']]');
    end;
End;
*/

void ShowScores( void )
{
    uint16_t i;

    CenterWindow(14,14);
    Print( " High scores:]] SCORE LV BY] ----- -- ---]" );
    for (i=0; i<HISCORE_MAX; i++)
    {
        Print( "%6d", highscores[i].score );
        sx++;

        if (highscores[i].level==11)
        {
            CharOut(sx+1,sy,167);
            sx += 3;
        }
        else
        {
            Print( "%2d", highscores[i].level );
            sx++;
        }

        Print( "%s]]", highscores[i].initials );
    }
}

/*
{================================}
{                                }
{ GAMEOVER                       }
{ Do a game over bit, then check }
{ for a high score, then return  }
{ to demo.                       }
{                                }
{================================}
*/
/*
Procedure GameOver;
var
  place,i,j:integer;
  st: string;
Begin
  WaitendSound;
  SimpleRefresh;
  while keypressed do
    ch:=readkey;
  for i:=1 to 60 do
    waitVBL;

{                                 }
{ fill in the player's high score }
{                                 }
  If score>=highscores[5].score then
    Begin
      place:=5;
      while (place>1) and (highscores[place-1].score<score) do
	dec(place);
      if place<5 then
	for i:=4 downto place do
	  highscores[i+1]:=highscores[i];	{scroll high scores down}
      with highscores[place] do
	begin
	  level:=catacombs.level;
	  score:=catacombs.score;
	  for i:=1 to 3 do
	    initials[i]:=' ';
	  ShowScores; 		{show the scores with space for your inits}
	  while keypressed do
	    ch:=readkey;
          playsound (highscoresnd);
	  sy:=7+place*2;
	  sx:=15;
	  input (st,3);
	  for i:=1 to length(st) do
	    initials[i]:=st[i];
	end;
      savehighscores;
    end;

{               }
{ no high score }
{               }
  ShowScores;
  while keypressed do
    ch:=readkey;
  i:=0;
  repeat
    waitvbl;
    inc (i);
    PlayerIO (dir,button1,button2);
  until (i>500) or button1 or button2 or keypressed;

  if button1 or button2 then
    playmode:=game
  else
    playmode:=demogame;
End;
*/

void GameOver( void )
{
    int16_t i;
    uint16_t place;
    char st[10];

    WaitEndSound();
    SDL_Delay(2000);

    /*
    {                                 }
    { fill in the player's high score }
    {                                 }
    */
    if (score>=highscores[HISCORE_MAX-1].score)
    {
        place = HISCORE_MAX-1;
        while (place>0 && highscores[place-1].score<score) {
            place--;
        }
        if (place<HISCORE_MAX-1)
        {
            for (i=HISCORE_MAX-2; i>=place; i--)
            {
                highscores[i+1] = highscores[i];	/* {scroll high scores down} */
            }

            highscores[place].level = level;
            highscores[place].score = score;
            strncpy( highscores[place].initials, "   ", 3 );

            ShowScores(); 		/* {show the scores with space for your inits} */
            GetKey( &ch, False );
            PlaySound(highscoresnd);
            sy = 7+(place+1)*2;
            sx = 15;
            Input(st,3);
            strncpy( highscores[place].initials, st, 3 );
        }

        SaveHighScores( filelist[episode][file_score] );
    }

    /*
    {               }
    { no high score }
    {               }
    */
    ShowScores();
    GetKey( &ch, False );

    i = 0;
    do {
        i++;
        PlayerIO( &dir, &button1, &button2 );
    } while (i<500 && !button1 && !button2);

    if (button1 || button2) {
        playmode = game;
    } else {
        playmode = demogame;
    }
}


/*
{================}
{                }
{ exit procedure }
{                }
{================}
*/
/*
{$F+}
procedure Cleanup;
begin;
  exitproc:=exitsave;	{turbo's exit procedure}
  regs.ax:=3;
  intr($10,regs);	{set graphic mode to 80*25 *16 color}
  ShutdownSound;	{remove spkr int 10 handler}
  DisconnectKBD;	{remove KBD int 9 handler}
end;
{$F-}
*/

/*
{=========================}
{                         }
{ M A I N   P R O G R A M }
{                         }
{=========================}
*/
/*
Begin
  initobjects;

  fillchar (priority,sizeof(priority),chr(99));

  priority[blankfloor]:=0;
  for i:=ObjDef[teleporter].firstchar to ObjDef[Teleporter].firstchar+20 do
    priority[i]:=0;
  for clvar:=Dead2 to Dead5 do
    for i:=ObjDef[clvar].firstchar to ObjDef[clvar].firstchar+
    ObjDef[clvar].size*ObjDef[clvar].size do
      priority[i]:=0;		{deadthing}
  for i:=152 to 161 do
    priority[i]:=2;		{shots}
  for i:=ObjDef[bigshot].firstchar to ObjDef[bigshot].firstchar + 31 do
    priority[i]:=2;		{bigshot}
  for i:=0 to tile2s-1 do
    if priority [i]=99 then
      priority[i]:=3;		{most 1*1 tiles are walls, etc}
  for i:=tile2s to maxpics do
    if priority[i]=99 then
      priority[i]:=4;		{most bigger tiles are monsters}
  for i:=ObjDef[player].firstchar to ObjDef[player].firstchar + 63 do
    priority[i]:=5;		{player}


  side:=0;

  for x:=0 to 85 do
    Begin
      for y:=0 to topoff-1 do
        Begin
          view[x,y]:=solidwall;
          view[x,85-y]:=solidwall;
	  background[x,y]:=solidwall;
	  background[x,85-y]:=solidwall;
        end;
      view[86,x]:=solidwall;
    end;
  for y:=11 to 74 do
    for x:=0 to leftoff-1 do
      Begin
        view[x,y]:=solidwall;
        view[85-x,y]:=solidwall;
	background[x,y]:=solidwall;
	background[85-x,y]:=solidwall;
      end;


  mouseok:= true;		{check for a mouse driver}
  GetIntVec ($33,tempp);
  if tempp=NIL then
    mouseok:=false;
  if mem[seg(tempp^):ofs(tempp^)] = $CF then	{does it point to an IRET}
    mouseok:=false;

  LoadDemo;
  LoadSounds;
  LoadHighScores;

  GetGrMode;		{get choice of graphic / sound modes}
  InitGraphics;		{Load the graphics in}
  InitRND (true);	{warm up the random generator}

  exitsave:=exitproc;	{save off turbo's exit handler}
  exitproc:= @cleanup;	{and install ours}

  ConnectKBD;		{set up int 9 handler}
  StartupSound;         {set up int 10 handler}
  soundon:=true;        {sound on until told otherwise}

  xormask:=0;           {draw everything normal until told otherwise}

  inpmode:=kbd;   {keyboard control until told otherwise}


  playmode:=demogame;


{
    Begin
      playmode:=demosave;
      playsound (bigshotsnd);
      waitendsound;
    end;
}

  Repeat
    case playmode of

      game: Begin
	      PlaySetup;
              Playloop;
              If gamexit=killed then
                Begin

                  GameOver;
                end;
              if gamexit=victorious then
                Begin
		  finished;
                  GameOver;
                end;
              playmode:= demogame;
            End;

      demosave:	Begin             	{mode for recording a demo}
		  playsetup;
		  playloop;
		  SaveDemo;
		  playmode:=demogame;
		end;

      demogame: Begin
		  PlaySetup;
		  PlayLoop;
		  if (playmode=demogame) then
		    begin
		      score:=0;	{so demo doersn't get a high score}
		      GameOver;	{if entire demo has cycled, show highs}
		    end;
                End;

{$IFNDEF SAMPLER}

      editor : Begin
                 EditorLoop;
                 playmode:=demogame;
               End;
{$ENDIF}

    end;

  Until false;



End.
*/

int32_t CatacombMain( uint8_t type, const char *path )
{
    playmode = intro;
    episode = type;

    InitObjects();
    SetPriorities();

    LoadData( episode, path );
    LoadDemo( filelist[episode][file_demo] );
    LoadSounds( filelist[episode][file_sound], True );
    LoadHighScores( filelist[episode][file_score] );
    GetGrMode();		/* {get choice of graphic / sound modes} */

    xormask = 0;        /* {draw everything normal until told otherwise} */
    config.soundmode = sound_on;     /* {sound on until told otherwise} */
    config.inputmode = keyboard;      /* {keyboard control until told otherwise} */
    playmode = demogame;

    while (True)
    {
        InputPollEvents();

        switch (playmode)
        {
        case game:
            PlaySetup();
            PlayLoop();
            if (gamexit==killed)
            {
                GameOver();
            }
            if (gamexit==victorious)
            {
                Finished();
                GameOver();
            }
            playmode = demogame;
            break;

        case demosave: /* {mode for recording a demo} */
            PlaySetup();
            PlayLoop();
            SaveDemo( filelist[episode][file_demo] );
            playmode = demogame;
            break;

        case editor:
            EditorLoop();
            playmode = demogame;
            break;

        case demogame:
        default:
            PlaySetup();
            PlayLoop();
            if (playmode==demogame)
            {
                score = 0;	/* {so demo doersn't get a high score} */
                GameOver();	/* {if entire demo has cycled, show highs} */
            }
            break;
        }
    }

    return 0;
}

/*
;{=========================================}
;{                                         }
;{ DRAWOBJ                                 }
;{ Draws the object to TILES in the proper }
;{ direction and state.                    }
;{                                         }
;{=========================================}
;
;Procedure DrawObj;
;var
;  objpri,bsize,tilenum:integer;
;Begin
;  tilenum:=obj.firstchar + obj.size * obj.size
;  *((integer(obj.dir) and obj.dirmask) * obj.stages + obj.stage);
;  obj.oldtile:= tilenum;
;  obj.oldx:=obj.x;
;  obj.oldy:=obj.y;
;  objpri:=priority[tilenum];
;  For y:=obj.y to obj.y+obj.size-1 do
;    for x:=obj.x to obj.x+obj.size-1 do
;      Begin
;	if priority[view[y,x]]<=objpri then
;	  view[y,x]:=tilenum;
;	inc(tilenum);
;      end;
;End;
*/

void DrawObj( void )
{
    int32_t x,y;
    int16_t objpri,tilenum;

    tilenum = obj.firstchar + obj.size * obj.size * ((obj.dir & obj.dirmask) * obj.stages + obj.stage);
    obj.oldtile = tilenum;
    obj.oldx = obj.x;
    obj.oldy = obj.y;
    objpri = priority[tilenum];

    for (y=obj.y; y<=obj.y+obj.size-1; y++)
    {
        for (x=obj.x; x<=obj.x+obj.size-1; x++)
        {
            if (priority[view[y][x]]<=objpri)
            {
                view[y][x]=tilenum;
            }
            tilenum++;
        }
    }
}


/*
;{=======================================}
;{                                       }
;{ ERASEOBJ                              }
;{ Erases the current object by copying  }
;{ the background onto the view where the}
;{ object is standing                    }
;{                                       }
;{=======================================}
;
;Procedure EraseObj;
;var
;  objpri,bsize,tilenum:integer;
;Begin
;  tilenum:=obj.oldtile;
;  For y:=obj.oldy to obj.oldy+obj.size-1 do
;    for x:=obj.oldx to obj.oldx+obj.size-1 do
;      Begin
;	if view[y,x]=tilenum then
;	  view[y,x]:=background[y,x];
;	inc(tilenum);
;      end;
;End;
*/

void EraseObj( void )
{
    int32_t x,y;
    int16_t tilenum;

    tilenum = obj.oldtile;
    for (y=obj.oldy; y<=obj.oldy+obj.size-1; y++)
    {
        for (x=obj.oldx; x<=obj.oldx+obj.size-1; x++)
        {
            if (view[y][x]==tilenum)
            {
                view[y][x]=background[y][x];
                tilenum++;
            }
        }
    }
}

/*
;{====================}
;{                    }
;{ DoAll              }
;{ The main play loop }
;{                    }
;{====================}
;
;Procedure Doall;
;begin
;  Repeat  {until leveldone or playdone}
;    For objecton:=numobj downto 0 do
;      Begin
;	move (o[objecton],obj.active,sizeof(o[objecton]) );
;	if obj.class<>nothing then {class=nothing means it was killed}
;	  Begin
;	    move (ObjDef[obj.class],obj.think,sizeof(objdef[obj.class]) );
;	    if obj.active then
;	      DoActive
;	    else
;	      DoInactive;
;	  end;
;      end;
;   refresh;
;   inc (frameon);
;  until leveldone or playdone;
;end;
*/

void DoAll( void )
{
    do {
        for (objecton=numobj; objecton>=0; objecton--)
        {
            memcpy( &obj.active, &o[objecton], sizeof(o[objecton]) );
            if (obj.type != nothing)    /* {class=nothing means it was killed} */
            {
                memcpy( &obj.think, &ObjDef[obj.type], sizeof(ObjDef[obj.type]) );
                if (obj.active)
                    DoActive();
                else
                    DoInactive();
            }
        }

        Refresh();
        frameon++;

    } while (leveldone == False && playdone == False);
}
