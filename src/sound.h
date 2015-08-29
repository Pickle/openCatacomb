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

#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLERATE  44100
#define CHANNELS    2
#define MAXSOUNDS   63

#define FREQDIV     1193180.0
#define short_while (SAMPLERATE/100)

#define StopSound()     Mix_Pause( -1 );
#define ResumeSound()   Mix_Resume( -1 );

typedef struct SOUND_T {
    uint16_t start;             /* start: Word; */
    uint8_t priority;           /* priority: byte; */
    uint8_t byte;               /* samples: byte; */
    char name[12];              /* name: string[11] */
} sound_t;

typedef struct SOUNDFILE_T {
    char id[4];                 /* {'SOUN' if good file} */
    uint16_t filelength;        /* filelength: word; */
    uint8_t filler[10];         /* filler: array [7..16] of byte; {should be 00} */
    sound_t sounds[MAXSOUNDS];  /* sounds: array[1..63] of soundtype; */
    uint16_t data[0x7BC0];      /* data: array[0..$7BBF] of integer; */
} soundfile_t;

typedef struct RIFF_HEADER_T {
    char chunkid[4];            /* Always "RIFF" */
    uint32_t chunksize;         /* size following this section (size-8) */
    char format[4];             /* Always "WAVE" */
    char subchunk_id1[4];       /* Always "fmt " */
    uint32_t subchuck1_size;    /* Always 16 bytes */
    uint16_t audioformat;       /* 1 for PCM */
    uint16_t channels;          /* 2 channels */
    uint32_t samplerate;        /* 44100 hz */
    uint32_t byterate;          /* (Sample Rate * BitsPerSample * Channels) / 8 */
    uint16_t blockalign;        /* (BitsPerSample * Channels) / 8 */
    uint16_t bitspersample;     /* 16 bit */
    char subchunk_id2[4];       /* Always "data" */
    uint32_t subchuck2_size;    /* size of data section */
} riff_header_t;

extern soundfile_t *sounddata;

void SoundInit( void );
void SoundShutDown( void );
void CloseSounds( void );
void LoadSounds( const char *filename, boolean load );
void PlaySound( soundtype_t soundnum );
void SoundChannelDone( int channel );
void WaitEndSound( void );
void write_freq( int16_t *data, uint32_t start, uint32_t count );

#ifdef __cplusplus
}
#endif

#endif /* SOUND_H_INCLUDED */
