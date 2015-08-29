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

#include "util.h"

#if defined(WIN32)
    #define MKDIR(X)    mkdir(X)
#else
    #define MKDIR(X)    mkdir(X,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH)
#endif

void MakeDir( const char *path )
{
    if (MKDIR( path ) != 0)
    {
        printf( "[SYSTEM] Error: could not create directory: %s code %X\n", path, errno );
    }
}

uint32_t LoadDataFromFile( const char *filename, uint8_t **buffer, uint32_t offset, uint32_t length )
{
    uint32_t total;
    FILE *file;

    file = fopen( filename, "rb" );

    if (file != NULL)
    {
        /* Read entire file if no specific size is given */
        if (length == 0)
        {
            fseek( file, 0, SEEK_END );
            length = ftell( file );
            fseek( file, 0, SEEK_SET );
        }

        /* Allocate memory if none was given */
        if (*buffer == NULL)
        {
            *buffer = malloc( length+1 );
            if (*buffer == NULL)
            {
                printf( "[UTIL] ERROR LoadDataFromFile could not create buffer for file %s\n", filename );
                fclose( file );
                return False;
            }
        }

        fseek( file, offset, SEEK_SET );
        total = fread( *buffer, sizeof(uint8_t), length, file );
        (*buffer)[total] = '\0';
        fclose( file );

        if (total != length)
        {
            printf( "[UTIL] LoadDataFromFile: WARNING Only read %d of %d bytes\n", total, length );
        }
    }
    else
    {
        printf( "[UTIL] ERROR LoadDataFromFile could not open file %s\n", filename );
        return 0;
    }

    return total;
}

void RLEexpand( uint8_t *src, uint8_t **dst, uint16_t src_size )
{
    uint8_t inc;
    uint16_t i, j, k;
    uint32_t dst_size;

    dst_size = ((src[3]<<24) + (src[2]<<16)+ (src[1]<<8) + src[0]);

    *dst = (uint8_t*)malloc( dst_size+1 );

    if (*dst == NULL)
    {
        printf( "[UTIL] RLEexpand: Could not allocate bytes %d\n", dst_size );
    }
    else
    {
        k = 0;
        for (i=4; i<src_size-1; /* nop */)
        {
            if (src[i] >= 0x80)
            {
                /* Insert the number of following bytes */
                inc = src[i]-0x7F;

                for (j=1; j<=inc; j++)
                {
                    if (k < dst_size)
                        (*dst)[k++] = src[i+j];
                    else
                        printf( "[UTIL] RLEexpand: source data is exceeding given size %d\n", dst_size );
                }
                i += (inc+1);
            }
            else if (src[i] < 0x80)
            {
                /* Repeat the next byte for the given count */
                inc = src[i]+3;

                for (j=0; j<inc; j++)
                {
                    if (k < dst_size)
                        (*dst)[k++] = src[i+1];
                    else
                        printf( "[UTIL] RLEexpand: source data is exceeding given size %d\n", dst_size );
                }
                i += 2;
            }
        }

        (*dst)[k] = 0;
    }
}

uint32_t Random( uint32_t min, uint32_t max )
{
    int32_t base_random, range, rem, bucket;

    base_random = rand(); /* in [0, RAND_MAX] */
    if (RAND_MAX == base_random) return Random(min, max);
    /* now guaranteed to be in [0, RAND_MAX) */
    range       = max - min;
    rem         = RAND_MAX % range;
    bucket      = RAND_MAX / range;
    /* There are range buckets, plus one smaller interval
    within remainder of RAND_MAX */
    if (base_random < RAND_MAX - rem) {
        return min + base_random/bucket;
    } else {
        return Random(min, max);
    }
}

void SwapColors( uint8_t *pixel, uint32_t lines, uint32_t pitch )
{
    uint32_t x, y;

    for (y=0; y<lines; y++)
    {
        for (x=0; x<pitch; x+=3)
        {
            uint8_t b = pixel[(y*pitch)+x];
            uint8_t r = pixel[(y*pitch)+x+2];
            pixel[(y*pitch)+x]   = r;
            pixel[(y*pitch)+x+2] = b;
        }
    }
}

void CheckPath( char *path )
{
    uint16_t i,j;

    const char *invalid = "[:\"*?<>|]+\0";

    for (i=0; i<strlen(path); i++)
    {
        for (j=0; j<strlen(invalid); j++)
        {
            if (path[i] == invalid[j]) {
                path[i] = 'x';
                break;
            }
        }
    }
}
