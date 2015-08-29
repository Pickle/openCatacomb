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

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ODD(X) (((X % 2) != 0) ? True : False)

void MakeDir( const char *path );
uint32_t LoadDataFromFile( const char *filename, uint8_t **buffer, uint32_t offset, uint32_t length );
void RLEexpand( uint8_t *dst, uint8_t **src, uint16_t src_size );
void SwapColors( uint8_t *pixel, uint32_t lines, uint32_t pitch );
uint32_t Random( uint32_t min, uint32_t max );
void CheckPath( char *path );

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H_INCLUDED */
