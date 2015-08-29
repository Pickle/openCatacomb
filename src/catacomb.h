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

#ifndef CATACOMB_H_INCLUDED
#define CATACOMB_H_INCLUDED

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HISCORE_MAX 5

extern graphmode_t graphmode;
extern playmode_t  playmode;
extern objdef_t ObjDef[lastclass];
extern int16_t numobj, objecton;
extern activeobj_t o[MAXOBJ], saveo;
extern int32_t     score, savescore;
extern int16_t     sx, sy, leftedge;
extern highscores_t highscores[HISCORE_MAX];
extern int16_t     shotpower;
extern boolean  playdone, leveldone;
extern int16_t     background[87][86];
extern objdesc_t   obj, altobj;
extern int16_t     level;
extern gamexit_t   gamexit;
extern int16_t     items[items_total], saveitems[items_total];
extern int16_t     boltsleft;
extern int16_t chkx,chky,chkspot;
extern int16_t     view[87][86];
extern int16_t     originx, originy;
extern int16_t     altnum;
extern int16_t     side;
extern uint8_t democmds[3000];
extern uint16_t frameon;
extern boolean altkey;
extern uint8_t priority[MAXPICS];

extern const uint8_t altmeters[14][14];
extern const uint8_t meters[14][14];
extern const dirtype_t opposite[ dirtype_total ];
extern uint8_t screencenterx, screencentery;

int32_t CatacombMain( uint8_t type, const char *path );
void GetGrMode( void );
void Print( const char *s, ... );
void PrintArray( const uint8_t *s, uint32_t len  );
void Refresh( void );
void ClearOld( void );
void Restore( void );
void DrawWindow( int16_t left, int16_t top, int16_t right, int16_t bottom );
void CenterWindow( int16_t width, int16_t height);
#if !defined(CATACOMB1)
void DrawWindowExpand( uint8_t width, int height );
void DrawWinExpHorz( uint8_t width, int height );
void DrawWinExpVert( uint8_t width, int height );
#endif
uint16_t GetKey( SDLKey *key, boolean cursor );
void GetButton( uint8_t *joy, boolean cursor );
void Input( char *s, int16_t length);
boolean Wantmore(void );
void DrawPic( int16_t x, int16_t y, classtype_t c, dirtype_t direc, int16_t stage );
void CheckKeys( void );
void Help( void );
void SoundChange( void );
void InputChange( void );
void Reset( void );
void Pause( void );
void QuitGame( void );
void SaveGame( void );
void LoadGame( void );
void PlayerIO( dirtype_t *direc, boolean *b1, boolean *b2 );

void LoadLevel( const char *ext  );
void LoadHighScores( const char *filename );
void SaveHighScores( const char *filename );
void LoadDemo( const char *filename );
void SaveDemo( const char *filename );
void Finished( void );
void PlaySetup( void );
void ShowSidePanel( void );
void ShowScores( void );
void GameOver( void );
void Cleanup( void );
void EraseObj( void );
void DrawObj( void );
void DoAll( void );

#ifdef __cplusplus
}
#endif

#endif /* CATACOMB_H_INCLUDED */
