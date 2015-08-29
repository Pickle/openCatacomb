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
#include "cpanel.h"
#include "util.h"
#include "sound.h"
#include "system.h"

soundtype_t channel_playing = nosnd;
soundfile_t *sounddata = NULL;
Mix_Chunk *chunks[soundstotal];

void SoundInit( void )
{
    uint8_t i;
    int32_t result;

    printf( "[AUDIO] Initialize SDL Audio\n" );

    for (i=0; i<soundstotal; i++)
    {
        chunks[i] = NULL;
    }

    result = SDL_InitSubSystem( SDL_INIT_AUDIO );
    if (result != 0)
    {
        printf( "[AUDIO] ERROR InitSound SDL Audio Error: %s\n", SDL_GetError() );
        config.soundmode = sound_off;
        return;
    }

    result = Mix_OpenAudio( SAMPLERATE, MIX_DEFAULT_FORMAT, CHANNELS, 1024 );
    if (result != 0)
    {
        printf( "[AUDIO] ERROR InitSound SDL_Mixer Error: %s\n", Mix_GetError() );
        SDL_QuitSubSystem( SDL_INIT_AUDIO );
        config.soundmode = sound_off;
        return;
    }

    Mix_ChannelFinished( SoundChannelDone );

    sounddata = malloc( sizeof(soundfile_t) );

    printf( "[AUDIO] SDL Audio Completed!\n" );
    config.soundmode = sound_on;
}

void SoundShutDown( void )
{
    printf( "[AUDIO] Shutdown SDL Audio\n" );
    CloseSounds();
    free(sounddata);

    Mix_CloseAudio();
    SDL_QuitSubSystem( SDL_INIT_AUDIO );
}

void CloseSounds( void )
{
    uint8_t i;

    for (i=0; i<soundstotal; i++)
    {
        if (chunks[i] != NULL)
        {
            Mix_FreeChunk( chunks[i] );
            chunks[i] = NULL;
        }
    }
}

void LoadSounds( const char *filename, boolean load )
{
    uint32_t i;
    int32_t dataoffset, srt, end, len;
    char riffname[STRSIZE];
    int16_t *pcm = NULL;
    FILE *wav = NULL;
    riff_header_t riffheader;
    char path[STRSIZE];

    memcpy( riffheader.chunkid, "RIFF", 4 );
    riffheader.chunksize        = 0;
    memcpy( riffheader.format, "WAVE", 4 );
    memcpy( riffheader.subchunk_id1, "fmt ", 4 );
    riffheader.subchuck1_size   = 16;
    riffheader.audioformat      = 1; /* PCM */
    riffheader.channels         = CHANNELS;
    riffheader.samplerate       = SAMPLERATE;
    riffheader.bitspersample    = 16;
    riffheader.byterate         = (riffheader.samplerate * riffheader.bitspersample * riffheader.channels) / 8;
    riffheader.blockalign       = (riffheader.bitspersample * riffheader.channels) / 8;
    memcpy( riffheader.subchunk_id2, "data", 4 );
    riffheader.subchuck2_size   = 0;

    printf( "[SYSTEM] Extracting sounds data to wav\n" );
    memset( sounddata, 0, sizeof(soundfile_t) );
    snprintf( path, STRSIZE, "%s/%s", pathdata, filename );
    LoadDataFromFile( path, (uint8_t**)&sounddata, 0, 0 );

#if defined(DEBUG)
    printf( "Sound ID: %s (%2X%2X%2X%2X)\n", sounddata->id, sounddata->id[0], sounddata->id[1], sounddata->id[2], sounddata->id[3] );
    printf( "File length: %d\n", sounddata->filelength );
#endif

    dataoffset = sounddata->sounds[0].start;

    for (i=0; i<MAXSOUNDS; i++)
    {
        if (strcmp( "->UNUSED<-", sounddata->sounds[i].name ) == 0 ||
            strcmp( "__UnNamed__", sounddata->sounds[i].name ) == 0    )
            continue;

#if defined(DEBUG)
        printf( "  %d start %4X priority: %3d byte: %1X name: %s\n", i,
                sounddata->sounds[i].start,
                sounddata->sounds[i].priority,
                sounddata->sounds[i].byte,
                sounddata->sounds[i].name );
#endif

        snprintf( riffname, STRSIZE, "%s/%s/%d_%s.wav", pathdata, PATH_SOUND, i, sounddata->sounds[i].name );
        CheckPath( riffname );

        if (force_refresh == False)
            wav = fopen( riffname, "rb" );

        if (wav == NULL )
        {
            printf( "[SYSTEM] Extracting sound to %s\n", riffname );

            srt = (sounddata->sounds[i].start-dataoffset)/2;
            end = (sounddata->sounds[i+1].start-dataoffset-4)/2;

            if (srt < 0 || end <= 0 || end < srt)
            {
                printf( "[SYSTEM] Error address srt: %X end: %X\n", srt, end );
                continue;
            }
            if (sounddata->data[end+1] != 0xFFFF)
            {
                printf( "[SYSTEM] Error end of sound file was: %X\n", sounddata->data[end] );
                continue;
            }

            len = end - srt;
            riffheader.subchuck2_size = len * CHANNELS * sizeof(int16_t) * short_while;
            riffheader.chunksize = riffheader.subchuck2_size + 32;

            pcm = malloc( riffheader.subchuck2_size );

            write_freq( pcm, srt, len );

            /* Write to file */
            wav = fopen( riffname, "wb" );
            if (wav != NULL)
            {
                fwrite( &riffheader, sizeof(uint8_t), sizeof(riffheader), wav );
                fwrite( pcm, sizeof(uint8_t), riffheader.subchuck2_size, wav );
                fclose(wav);
                wav = NULL;
            }

            free(pcm);
        }
        else
        {
            printf( "[SYSTEM] Found sound in cache: %s\n", riffname );

            fclose(wav);
            wav = NULL;
        }

        if (load == True)
        {
            if (chunks[i] == NULL)
            {
                chunks[i] = Mix_LoadWAV( riffname );

                if (chunks[i] == NULL)
                {
                    printf( "[AUDIO] ERROR LoadSounds could not open cache wav. Filename %s: Error: %s\n", riffname, Mix_GetError() );
                }
            }
            else
            {
                printf( "[AUDIO] ERROR LoadSounds sound chunk was already loaded. Filename %s:\n", riffname );
            }
        }
    }
}

void PlaySound( soundtype_t soundnum )
{
    int32_t result;

    if ((config.soundmode = sound_on) && (playmode != demogame) && (chunks[soundnum] != NULL))
    {
        if (channel_playing != nosnd)
        {
            if (sounddata->sounds[soundnum].priority < sounddata->sounds[channel_playing].priority)
            {
                Mix_HaltChannel( 1 );
            }
        }

        result = Mix_PlayChannel( 1, chunks[soundnum], 0 );
        if(result == -1)
        {
            printf("[AUDIO] SDL_Mixer Audio Mix_PlayChannel: %s\n",Mix_GetError());
        }

        channel_playing = soundnum;
    }
}

void SoundChannelDone( int channel )
{
    if (channel == 1)
        channel_playing = nosnd;
}

void WaitEndSound( void )
{
    uint16_t limit;

    limit = 0;
    while (channel_playing != nosnd)
    {
        SDL_Delay(10);
        limit++;
        if (limit > 1000)
            break;
    }
}

/* Adapted on source code from */
/* http://web.archive.org/web/20000818033701/http://www.dd.chalmers.se/~f98anga/projects/keen/sounds2raw.c */

void write_freq( int16_t *data, uint32_t start, uint32_t count )
{
    /*  write a short while of 'freq':  */
    /*  y = Amplitude * sin (omega * t)  where omega = 2*PI*freq  */
    uint32_t i,j;
    int16_t inverse;
    int16_t v;
    double freq;
    double t;
    double dt = 1.0/SAMPLERATE;
    double y;
    double pi = 4*atan(1);

    for (i=start; i<start+count; i++)
    {
        inverse = sounddata->data[i];
        freq    = FREQDIV/(double)inverse;

        if ((uint16_t)inverse == 0xFFFF)
            break;

        t = 0;

        for (j=0; j<short_while; j++)
        {
            /* Generate the frequency sample */
            if (inverse > 0)
                y = 10000.0 * sin( 2.0*pi*freq*t );
            else
                y = 0.0;

            /* square wave it */
            if (y > 1.0)
                v = 10000;
            if (y < -1.0)
                v = -10000;
            else
                v = 0;

            t += dt;

            *(data+0) = v;    /* right */
            *(data+1) = v;    /* left */
            data += 2;
        }
    }
}
