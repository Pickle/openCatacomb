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

#ifndef CAT_PLAY_H_INCLUDED
#define CAT_PLAY_H_INCLUDED

#include "types.h"

int16_t NewObject( void );
void PrintScore( void );
void PrintHighScore( void );
void PrintShotPower( void );
void PrintBody( void );
void LevelCleared( void );
void GiveKey( void );
void GivePotion( void );
void GiveBolt( void );
void GiveNuke( void );
boolean TakeKey( void );
void TakePotion ( void );
void CastBolt( void );
void CastNuke( void );
void PlayShoot( void );
void PlayBigShoot( void );
void GiveScroll( void );
void Opendoor( void );
void TagObject( void );
boolean IntoMonster( void );
boolean Walkthrough( void );
boolean Walk( void );
void playerCMDThink( void );
void chaseThink( boolean diagonal );
void gargThink( void );
void dragonThink( void );
void gunThink( dirtype_t dir );
void shooterThink( void );
void idleThink( void );
void fadeThink( void );
void KillNear( int16_t chkx, int16_t chky );
void explodeThink( void );
void Think( void );
void DoActive( void );
void DoInactive( void );
void PlayLoop( void );

#endif /* CAT_PLAY_H_INCLUDED */
