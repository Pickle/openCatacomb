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
#include "system.h"
#include "video.h"
#include "sound.h"
#include "input.h"
#include "opengl.h"
#include "unlzexe.h"

config_t config;
boolean force_refresh = False;
uint8_t     episode;
SDL_Rect    picdims[graphmode_total][pic_total];
char pathdata[STRSIZE];

const char *filelist[episode_total][file_total] = {
{
    "CAT",
    "CATACOMB.EXE",
    "SOUNDS.CAT",
    "DEMO.CAT",
    "TOPSCORS.CAT",
    "none",
    "none",
    "none",
    "none"
},
{
    "CA2",
    "CATACOMB.EXE",
    "SOUNDS.CA2",
    "DEMO1.CA2",
    "TOPSCORS.CA2",
    "CGACHARS.CA2",
    "CGAPICS.CA2",
    "EGACHARS.CA2",
    "EGAPICS.CA2"
} };


void ParseArgs( int32_t argc, char *argv[] )
{
    int32_t i;

    for (i=1; i<argc; i++)
    {
        if ((strstr( argv[i], "--ver" ) != NULL) || (strstr( argv[i], "-V") != NULL))
        {
            printf( "openCatacomb\n" );
            printf( "Copyright 2014 Scott Smith\n" );
            printf( "Version %f\n", VERSION );
            printf( "\n" );
            printf( "based on\n" );
            printf( "\n" );
            printf( "The Catacomb\n" );
            printf( "Copyright 1990-93 Softdisk Publishing\n" );
            printf( "Version 1.02\n" );
            SystemShutdown();
            exit(0);
        }
        else if ((strstr( argv[i], "--help" ) != NULL) || (strstr( argv[i], "-H") != NULL))
        {
            printf( "  openCatacomb\n" );
            printf( "  -V,--ver    : version and copyright information\n" );
            printf( "  -G,--game   : select which game to run 1 or 2\n" );
            printf( "  -F,--force  : Force extraction of game resources\n" );
            SystemShutdown();
            exit(0);
        }

        if ((strstr( argv[i], "--force" ) != NULL) || (strstr( argv[i], "-F") != NULL))
        {
            force_refresh = True;
        }

        if ((strstr( argv[i], "--game" ) != NULL) || (strstr( argv[i], "-G") != NULL))
        {
            episode = LIM( 1, 2, atoi( argv[++i] ) );
        }
    }
}

void SystemInit( void )
{
    boolean result;

    /* Default Config */
    LoadConfig( "config.bin" );

    result = VideoInit( SCREEN_W*SCALE, SCREEN_H*SCALE );

    if (result == True) {
        SoundInit();
    }

    if (result == True) {
        InputInit();
    }

    if (result != True)
    {
        SystemShutdown();
        exit(-1);
    }
}

void SystemShutdown( void )
{
    printf( "[SYSTEM] Quitting...\n" );

    SaveConfig( "config.bin" );
    CloseData();
    OGL_Close();
    InputShutDown();
    SoundShutDown();
    VideoShutDown();
    SDL_Quit();
}

void LoadConfig( const char *path )
{
    uint8_t i;
    int32_t value;
    FILE *file = fopen( path, "rb" );

    if (file != NULL)
    {
        fread( &config, 1, sizeof(config_t), file );
        for (i=0; i<input_total; i++)
        {
            fread( &value, 1, sizeof(SDLKey), file );
            SetKey( i, value );
        }
        for (i=0; i<input_total; i++)
        {
            fread( &value, 1, sizeof(uint8_t), file );
            SetButton( i, value );
        }

        fclose( file );
    }
    else
    {
        printf( "[SYSTEM] ERROR LoadConfig could not open file: %s\n", path );

        config.graphmode = EGAgr;
        config.soundmode = sound_on;
        config.inputmode = keyboard;
        config.mousesensitivity = 10;
        config.filtermode = GL_NEAREST;
    }

    if (config.graphmode >= graphmode_total) {
        config.graphmode = EGAgr;
        printf( "[SYSTEM] ERROR LoadConfig graphmode was a corrupted value\n" );
    }

    if (config.soundmode >= soundmode_total) {
        config.soundmode = sound_on;
        printf( "[SYSTEM] ERROR LoadConfig soundmode was a corrupted value\n" );
    }

    if (config.inputmode >= inputmode_total) {
        config.inputmode = keyboard;
        printf( "[SYSTEM] ERROR LoadConfig inputmode was a corrupted value\n" );
    }

    if ( config.mousesensitivity >= 20) {
        config.mousesensitivity = 10;
        printf( "[SYSTEM] ERROR LoadConfig mousesensitivity was a corrupted value\n" );
    }

    if (config.filtermode != GL_NEAREST && config.filtermode != GL_LINEAR) {
        config.filtermode = GL_NEAREST;
        printf( "[SYSTEM] ERROR LoadConfig filtermode was a corrupted value\n" );
    }
}

void SaveConfig( const char *path )
{
    uint8_t i;
    FILE *file = fopen( path, "wb" );

    if (file != NULL)
    {
        fwrite( &config, 1, sizeof(config_t), file );
        for (i=0; i<input_total; i++)
        {
            fwrite( &inputmaps[i].key, 1, sizeof(SDLKey), file );
        }
        for (i=0; i<input_total; i++)
        {
            fwrite( &inputmaps[i].joy, 1, sizeof(uint8_t), file );
        }

        fclose( file );
    }
    else
    {
        printf( "[SYSTEM] ERROR LoadConfig could not open file: %s\n", path );
    }
}

boolean LoadData( uint8_t type, const char *path )
{
    char pathfile[STRSIZE];
    char pathcache[STRSIZE];
    char pathexe[STRSIZE];
    char pathsnd[STRSIZE];
    char pathunlz[STRSIZE];
    char pathcachesnd[STRSIZE];

    boolean result = True;
    uint32_t length = 0;
    uint8_t *data = NULL;
    FILE *file = NULL;

    /* Setup paths */
    snprintf( pathdata, STRSIZE, "%s", path );
    snprintf( pathcache, STRSIZE, "%s/%s", pathdata, PATH_CACHE );
    snprintf( pathexe, STRSIZE, "%s/%s", pathdata, filelist[type][file_exe] );
    snprintf( pathsnd, STRSIZE, "%s/%s", pathdata, filelist[type][file_sound] );
    snprintf( pathunlz, STRSIZE, "%s/%s", pathcache, CATACOMB_UNLZ );
    snprintf( pathcachesnd, STRSIZE, "%s/%s", pathdata, PATH_SOUND );

    /* Create folders to cache data files in useable formats */
    MakeDir( pathcache );
    MakeDir( pathcachesnd );

    if (force_refresh == False)
        file = fopen( pathunlz, "r" );

    if (file == NULL) {
        printf( "[SYSTEM] Running unlzexe on %s to %s\n", pathexe, pathunlz );
        result = unlzexe( pathexe, pathunlz );
    } else {
        fclose(file);
    }

    if (type == catacomb1)
    {
        /* CGA Tile data */
        LoadDataFromFile( pathunlz, &data, CGAPICS_OFFSET, CGAPICS_LENGTH );
        if (data == NULL)
            return False;

        snprintf( pathfile, STRSIZE, "%s/cgatiles.bmp", pathcache );
        result = LoadCGA_Tiles( &textiles[CGAgr], data, CGAPICS_LENGTH, pathfile, TILE_W, TILE_H );
        free( data );
        data = NULL;

        if (result == False)
            return False;

        /* EGA Tile data */
        LoadDataFromFile( pathunlz, &data, EGAPICS_OFFSET, EGAPICS_LENGTH );
        if (data == NULL)
            return False;

        snprintf( pathfile, STRSIZE, "%s/egatiles.bmp", pathcache );
        result = LoadEGA_Tiles( &textiles[EGAgr], data, EGAPICS_LENGTH, pathfile, TILE_W, TILE_H );
        free( data );
        data = NULL;

        if (result == False)
            return False;

        /* CGA Image data */
        LoadDataFromFile( pathunlz, &data, TITLE_OFFSET, TITLE_LENGTH );
        if (data == NULL)
            return False;

        snprintf( pathfile, STRSIZE, "%s/cgatitle.bmp", pathcache );
        result = LoadCGA_PICFormat( &screen_title, data, pathfile, BASE_W, BASE_H );
        free( data );
        data = NULL;

        if (result == False)
            return False;

        /* EGA Image data */
        LoadDataFromFile( pathunlz, &data, END_OFFSET, END_LENGTH );
        if (data == NULL)
            return False;

        snprintf( pathfile, STRSIZE, "%s/cgaend.bmp", pathcache );
        result = LoadCGA_PICFormat( &screen_end, data, pathfile, BASE_W, BASE_H );
        free( data );
        data = NULL;

        if (result == False)
            return False;
    }
    else if (type == catacomb2)
    {
        snprintf( pathfile, STRSIZE, "%s/%s", pathdata, filelist[catacomb2][file_cgachars] );
        length  = LoadDataFromFile( pathfile, &data, 0, 0 );
        if (data == NULL)
            return False;
        snprintf( pathfile, STRSIZE, "%s/cgatiles.bmp", pathcache );
        result = LoadCGA_Tiles( &textiles[CGAgr], data, length, pathfile, TILE_W, TILE_H );
        free( data );
        data = NULL;

        snprintf( pathfile, STRSIZE, "%s/%s", pathdata, filelist[catacomb2][file_egachars] );
        length = LoadDataFromFile( pathfile, &data, 0, 0 );
        if (data == NULL)
            return False;
        snprintf( pathfile, STRSIZE, "%s/egatiles.bmp", pathcache );
        result = LoadEGA_Tiles( &textiles[EGAgr], data, length, pathfile, TILE_W, TILE_H );
        free( data );
        data = NULL;

        snprintf( pathfile, STRSIZE, "%s/%s", pathdata, filelist[catacomb2][file_cgapics] );
        length = LoadDataFromFile( pathfile, &data, 0, 0 );
        if (data == NULL)
            return False;
        snprintf( pathfile, STRSIZE, "%s/cgapics", pathcache );
        result = LoadCGA_Pics( &texpics[CGAgr], data, pathfile, TILE_W, TILE_H );
        free( data );
        data = NULL;

        snprintf( pathfile, STRSIZE, "%s/%s", pathdata, filelist[catacomb2][file_egapics] );
        length = LoadDataFromFile( pathfile, &data, 0, 0 );
        if (data == NULL)
            return False;
        snprintf( pathfile, STRSIZE, "%s/egapics", pathcache );
        result = LoadEGA_Pics( &texpics[EGAgr], data, pathfile, TILE_W, TILE_H );
        free( data );
        data = NULL;
    }
    else
    {
        printf( "[SYSTEM] Unknown data set\n" );
        result = True;
    }

    /* Create sound wavs */
    LoadSounds( filelist[type][file_sound], False );
    CloseSounds();

    return result;
}

void CloseData( void )
{
    uint8_t i;

    for (i=0; i<graphmode_total; i++)
    {
        if (textiles[i].id != 0) {
            glDeleteTextures( 1, &textiles[i].id );
            textiles[i].id = 0;
        }

        if (texpics[i].id != 0) {
            glDeleteTextures( 1, &texpics[i].id );
            texpics[i].id = 0;
        }
    }

    if (screen_title.id != 0) {
        glDeleteTextures( 1, &screen_title.id );
        screen_title.id = 0;
    }

    if (screen_end.id != 0) {
        glDeleteTextures( 1, &screen_end.id );
        screen_end.id = 0;
    }
}

boolean LoadBMP( const char *path, tex_t *texture )
{
    SDL_Surface *image;

    image = SDL_LoadBMP( path );

    if (image != NULL)
    {
        texture->w = image->w;
        texture->h = image->h;
        texture->id = OGL_OpenTexture( image->w, image->h, image->format->BytesPerPixel, image->pixels );
        SDL_FreeSurface( image );
        return True;
    }
    else
    {
        printf( "[SYSTEM] ERROR LoadBMP could not open bmp: %s\n", SDL_GetError() );
        return False;
    }
}

boolean SaveBMP( SDL_Surface *image, const char *path )
{
    uint32_t result;

    result = SDL_SaveBMP_RW( image, SDL_RWFromFile( path, "wb"), 1 );
    if (result !=  0)
    {
        printf( "[SYSTEM] ERROR SaveBMP could not write bmp '%s': %s\n", path, SDL_GetError() );
        return False;
    }
    return True;
}

boolean LoadLocs( const char* path, SDL_Rect *locs )
{
    uint8_t i;
    int32_t value, index;
    FILE *file = fopen( path, "r" );

    if (file != NULL)
    {
        for (i=0; i<pic_total; i++)
        {
            fscanf( file, "%5d", &index );
            if (index < pic_total)
            {
                fscanf( file, "%5d", &value );
                locs[index].x = value;
                fscanf( file, "%5d", &value );
                locs[index].y = value;
                fscanf( file, "%5d", &value );
                locs[index].w = value;
                fscanf( file, "%5d", &value );
                locs[index].h = value;
            }
            else
            {
                printf( "[SYSTEM] ERROR LoadLocs index out of range: %s\n", path );
            }
        }

        fclose( file );
    }
    else
    {
        printf( "[SYSTEM] ERROR LoadLocs could not open file: %s\n", path );
        return False;
    }

    return True;
}

SDL_Surface* ReadImageCGA( uint8_t *data, uint32_t w, uint32_t h )
{
    uint32_t i;
    uint32_t *pixels = NULL;
    uint32_t tile_bytes = (w*h)/TILE_CGA_BPP;
    SDL_Surface* image;

    image = SDL_CreateRGBSurface( 0, w, h, 32, RMASK, GMASK, BMASK, AMASK );
    SDL_FillRect( image, NULL, 0xFFFF00FF );

    pixels = (uint32_t*)image->pixels;

    for (i=0; i<tile_bytes; i++)
    {
        *(pixels+0) = GET_CGA_COLOR( ((data[i] >> 6) & 0x3) );
        *(pixels+1) = GET_CGA_COLOR( ((data[i] >> 4) & 0x3) );
        *(pixels+2) = GET_CGA_COLOR( ((data[i] >> 2) & 0x3) );
        *(pixels+3) = GET_CGA_COLOR( ((data[i] >> 0) & 0x3) );
        pixels += TILE_CGA_BPP;
    }

    return image;
}

SDL_Surface* ReadImageEGA( uint8_t *data, uint32_t offset, uint32_t *planes, uint32_t w, uint32_t h )
{
    uint32_t x,y;
    uint8_t r,g,b,i,color;
    uint32_t *pixels = NULL;
    uint32_t bytesperplane;
    uint32_t offsets[4];
    SDL_Surface* image;

    bytesperplane = (w*h)/8;

    if (planes != NULL)
    {
        offsets[0] = planes[0]+offset;
        offsets[1] = planes[1]+offset;
        offsets[2] = planes[2]+offset;
        offsets[3] = planes[3]+offset;
    }
    else
    {
        offsets[0] = 0;
        offsets[1] = bytesperplane;
        offsets[2] = bytesperplane*2;
        offsets[3] = bytesperplane*3;
    }

    image = SDL_CreateRGBSurface( 0, w, h, 32, RMASK, GMASK, BMASK, AMASK );
    SDL_FillRect( image, NULL, 0xFFFF00FF );
    pixels = (uint32_t*)image->pixels;

    for (x=0; x<bytesperplane; x++)
    {
        /* grab a byte from each color plane */
        b = data[x+offsets[0]];
        g = data[x+offsets[1]];
        r = data[x+offsets[2]];
        i = data[x+offsets[3]];

        /* sample each bit to form pixel color (8 components per byte) */
        for (y=0; y<8; y++)
        {
            uint8_t m = 0x80 >> y;
            color = ( (((i&m)==m)?8:0) | (((r&m)==m)?4:0) | (((g&m)==m)?2:0) | (((b&m)==m)?1:0) );
            *pixels = GET_EGA_COLOR( color );
            pixels++;
        }
    }

    return image;
}

boolean LoadCGA_Tiles( tex_t *texture, uint8_t *data, uint32_t length, const char *path, uint32_t tile_w, uint32_t tile_h )
{
    uint32_t i;
    uint16_t tile_bytes, tile_count;
    FILE *file          = NULL;
    SDL_Surface *tile   = NULL;
    SDL_Surface *image  = NULL;
    SDL_Rect dstrect;

    dstrect.x  = dstrect.y = 0;
    dstrect.w = tile_w;
    dstrect.h = tile_h;

    if (force_refresh == False)
        file = fopen( path, "rb" );

    if (file == NULL)
    {
        printf( "[SYSTEM] Extracting CGA tile images to bmp\n" );

        tile_bytes = (tile_w*tile_h)/TILE_CGA_BPP;
        tile_count = length/tile_bytes;

        image = SDL_CreateRGBSurface( 0, tile_w*TILE_COL, tile_h*TILE_ROW, 32, RMASK, GMASK, BMASK, AMASK ); /* 2048 tiles */

        SDL_FillRect( image, NULL, 0xFFFF00FF );

        for (i=0; i<tile_count; i++)
        {
            tile = ReadImageCGA( data+(i*tile_bytes), TILE_W, TILE_H );
            SDL_BlitSurface( tile, NULL, image, &dstrect );
            SDL_FreeSurface( tile );
            tile  = NULL;

            dstrect.x += tile_w;
            if ((dstrect.x + tile_w) > (uint32_t)image->w)
            {
                dstrect.x = 0;
                dstrect.y += tile_h;
            }
        }

        SaveBMP( image, path );

        SDL_FreeSurface( image );
        image = NULL;
    }
    else
    {
        printf( "[SYSTEM] Found CGA tile data in cache: %s\n", path );
        fclose( file );
        file = NULL;
    }

    return LoadBMP( path, texture );
}

boolean LoadEGA_Tiles( tex_t *texture, uint8_t *data, uint32_t length, const char *path, uint32_t tile_w, uint32_t tile_h )
{
    uint32_t i;
    uint16_t count;
    FILE *file          = NULL;
    SDL_Surface *tile   = NULL;
    SDL_Surface *image  = NULL;
    SDL_Rect dstrect;

    dstrect.x = dstrect.y = 0;
    dstrect.w = tile_w;
    dstrect.h = tile_h;

    if (force_refresh == False)
        file = fopen( path, "rb" );

    if (file == NULL)
    {
        printf( "[SYSTEM] Extracting EGA tile images to bmp\n" );

        image = SDL_CreateRGBSurface( 0, tile_w*TILE_COL, tile_h*TILE_ROW, 32, RMASK, GMASK, BMASK, AMASK ); /* 2048 tiles */
        SDL_FillRect( image, NULL, 0xFFFF00FF );

        count = length/TILE_EGA_BYTES;

        for (i=0; i<count; i++)
        {
            tile = ReadImageEGA( data+(i*TILE_EGA_BYTES), 0, NULL, TILE_W, TILE_H );
            SDL_BlitSurface( tile, NULL, image, &dstrect );
            SDL_FreeSurface( tile );
            tile  = NULL;

            dstrect.x += tile_w;
            if ((dstrect.x + tile_w) > (uint32_t)image->w)
            {
                dstrect.x = 0;
                dstrect.y += tile_h;
            }
        }

        SaveBMP( image, path );

        SDL_FreeSurface( image );
        image = NULL;
    }
    else
    {
        printf( "[SYSTEM] Found EGA tile data in cache: %s\n", path );
        fclose( file );
        file = NULL;
    }

    return LoadBMP( path, texture );
}

boolean LoadCGA_Pics( tex_t *texture, uint8_t *data, const char *path, uint32_t image_w, uint32_t image_h )
{
    uint32_t i;
    int16_t last_h, max_h;
    char pathbmp[STRSIZE];
    char pathloc[STRSIZE];
    FILE *filebmp       = NULL;
    FILE *fileloc       = NULL;
    SDL_Surface *tile   = NULL;
    SDL_Surface *image  = NULL;
    picheader_t header;
    pic_header_t picheaders[pic_total];
    SDL_Surface *picsurfs[pic_total];
    SDL_Rect dstrect;
    uint32_t tile_bytes = (TILE_W*TILE_H)/TILE_CGA_BPP;

    dstrect.x  = dstrect.y = 0;
    dstrect.w = image_w;
    dstrect.h = image_h;

    snprintf( pathbmp, STRSIZE, "%s.bmp", path );
    snprintf( pathloc, STRSIZE, "%s.txt", path );

    if (force_refresh == False) {
        filebmp = fopen( pathbmp, "rb" );
        fileloc = fopen( pathloc, "r" );
    }

    if (filebmp == NULL && fileloc == NULL)
    {
        printf( "[SYSTEM] Extracting CGA image to bmp\n" );
        memcpy( &header, data, sizeof(header) );

        header.offset_char /= 0x1000;
        header.offset_tile /= 0x1000;
        header.offset_pic /= 0x1000;
        header.offset_sprite /= 0x1000;
        header.offset_picheaders /= 0x1000;
        header.offset_spriteheaders /= 0x1000;
        header.offset_plane[0] /= 0x1000;
        header.offset_plane[1] /= 0x1000;
        header.offset_plane[2] /= 0x1000;
        header.offset_plane[3] /= 0x1000;

        image = SDL_CreateRGBSurface( 0, 512, 512, 32, RMASK, GMASK, BMASK, AMASK );
        SDL_FillRect( image, NULL, 0xFFFF00FF );

        /* Save Chars */
        for (i=0; i<header.count_char; i++)
        {
            tile = ReadImageCGA( data+header.offset_char+(i*tile_bytes), TILE_W, TILE_H );
            SDL_BlitSurface( tile, NULL, image, &dstrect );
            SDL_FreeSurface( tile );
            tile  = NULL;

            dstrect.x += TILE_W;
            if ((dstrect.x + TILE_W) > image->w)
            {
                dstrect.x = 0;
                dstrect.y += TILE_H;
            }
        }

        /* Save Pics */
        if (header.count_pic != pic_total)
        {
            printf( "[SYSTEM] ERROR: pic total read was not as expected: %d actual: %d\n", pic_total, header.count_pic );
            return False;
        }

        /* Load the pics into surfaces */
        memcpy( picheaders, data+header.offset_picheaders, sizeof(pic_header_t) * header.count_pic );
        for (i=0; i<header.count_pic; i++)
        {
            picheaders[i].width *= 4; /* width is in text columns, not pixels */

#if defined(DEBUG)
            printf( "[DEBUG] Loading %s %dx%d +%d\n", picheaders[i].name, picheaders[i].width, picheaders[i].height, picheaders[i].offset );
#endif
            picsurfs[i] = ReadImageCGA( data+header.offset_pic+picheaders[i].offset, picheaders[i].width, picheaders[i].height );
        }

        /* Copy and sort the pics to the tile surface and store the coordinates */
        fileloc = fopen( pathloc, "w" );

        dstrect.x = 0;
        last_h = max_h = 0;

        while (max_h >= 0)
        {
            uint8_t index;

            max_h = -1;

            for (i=0; i<header.count_pic; i++)
            {
                if (picsurfs[i] != NULL && picsurfs[i]->h > max_h)
                {
                    max_h = picsurfs[i]->h;
                    index = i;
                }
            }

            if (max_h > 0)
            {
                if (dstrect.x + picsurfs[index]->w > image->w)
                {
                    dstrect.x = 0;
                    dstrect.y += last_h;
                    last_h = 0;
                }
                dstrect.w = picsurfs[index]->w;
                dstrect.h = picsurfs[index]->h;

                SDL_BlitSurface( picsurfs[index], NULL, image, &dstrect );

                picdims[CGAgr][index] = dstrect;
                fprintf( fileloc, "%d %d %d %d %d\n", index,
                                                      picdims[CGAgr][index].x,
                                                      picdims[CGAgr][index].y,
                                                      picdims[CGAgr][index].w,
                                                      picdims[CGAgr][index].h );

                dstrect.x += picsurfs[index]->w;
                last_h = MAX( last_h, max_h );

                SDL_FreeSurface( picsurfs[index] );
                picsurfs[index] = NULL;
            }
        }

        fclose( fileloc );
        fileloc = NULL;

        SaveBMP( image, pathbmp );
        SDL_FreeSurface( image );
        image = NULL;
    }
    else
    {
        printf( "[SYSTEM] Found CGA image data in cache: %s\n", pathbmp );
        fclose( filebmp );
        fclose( fileloc );
        filebmp = NULL;
        fileloc = NULL;
    }

    if (LoadLocs( pathloc, picdims[CGAgr] ) == False)
        return False;

    return LoadBMP( pathbmp, texture );
}

boolean LoadEGA_Pics( tex_t *texture, uint8_t *data, const char *path, uint32_t image_w, uint32_t image_h )
{
    uint32_t i;
    int16_t last_h, max_h;
    char pathbmp[STRSIZE];
    char pathloc[STRSIZE];
    FILE *filebmp       = NULL;
    FILE *fileloc       = NULL;
    SDL_Surface *tile   = NULL;
    SDL_Surface *image  = NULL;
    picheader_t header;
    pic_header_t picheaders[pic_total];
    SDL_Surface *picsurfs[pic_total];
    SDL_Rect dstrect;

    dstrect.x  = dstrect.y = 0;
    dstrect.w = image_w;
    dstrect.h = image_h;

    snprintf( pathbmp, STRSIZE, "%s.bmp", path );
    snprintf( pathloc, STRSIZE, "%s.txt", path );

    if (force_refresh == False) {
        filebmp = fopen( pathbmp, "rb" );
        fileloc = fopen( pathloc, "r" );
    }

    if (filebmp == NULL && fileloc == NULL)
    {
        printf( "[SYSTEM] Extracting EGA image to bmp\n" );
        memcpy( &header, data, sizeof(header) );

        header.offset_char /= 0x1000;
        header.offset_tile /= 0x1000;
        header.offset_pic /= 0x1000;
        header.offset_sprite /= 0x1000;
        header.offset_picheaders /= 0x1000;
        header.offset_spriteheaders /= 0x1000;
        header.offset_plane[0] /= 0x1000;
        header.offset_plane[1] /= 0x1000;
        header.offset_plane[2] /= 0x1000;
        header.offset_plane[3] /= 0x1000;

#if defined(DEBUG)
        printf( "offset_char   0x%X\n", header.offset_char );
        printf( "offset_tile   0x%X\n", header.offset_tile );
        printf( "offset_pic    0x%X\n", header.offset_pic );
        printf( "offset_sprite 0x%X\n", header.offset_sprite );
        printf( "offset_picheaders    0x%X\n", header.offset_picheaders );
        printf( "offset_spriteheaders 0x%X\n", header.offset_spriteheaders );
        printf( "offset_plane[0] 0x%X\n", header.offset_plane[0] );
        printf( "offset_plane[1] 0x%X\n", header.offset_plane[1] );
        printf( "offset_plane[2] 0x%X\n", header.offset_plane[2] );
        printf( "offset_plane[3] 0x%X\n", header.offset_plane[3] );
        printf( "char %d tile %d pic %d sprite %d\n", header.count_char, header.count_tile, header.count_pic, header.count_sprite );
#endif

        image = SDL_CreateRGBSurface( 0, 512, 512, 32, RMASK, GMASK, BMASK, AMASK );
        SDL_FillRect( image, NULL, 0xFFFF00FF );

        /* Save Chars */
        for (i=0; i<header.count_char; i++)
        {
            tile = ReadImageEGA( data, (header.offset_char-header.offset_plane[0])+(i*8), header.offset_plane, TILE_W, TILE_H );
            SDL_BlitSurface( tile, NULL, image, &dstrect );
            SDL_FreeSurface( tile );
            tile  = NULL;

            dstrect.x += TILE_W;
            if ((dstrect.x + TILE_W) > image->w)
            {
                dstrect.x = 0;
                dstrect.y += TILE_H;
            }
        }

        /* Save Pics */
        if (header.count_pic != pic_total)
        {
            printf( "[SYSTEM] ERROR: pic total read was not as expected: %d actual: %d\n", pic_total, header.count_pic );
            return False;
        }

        /* Load the pics into surfaces */
        memcpy( picheaders, data+header.offset_picheaders, sizeof(pic_header_t) * header.count_pic );
        for (i=0; i<header.count_pic; i++)
        {
            picheaders[i].width *= 8; /* width is in text columns, not pixels */

#if defined(DEBUG)
            printf( "[DEBUG] Loading %s %dx%d +%d\n", picheaders[i].name, picheaders[i].width, picheaders[i].height, picheaders[i].offset );
#endif
            picsurfs[i] = ReadImageEGA( data, (header.offset_pic-header.offset_plane[0])+picheaders[i].offset, header.offset_plane, picheaders[i].width, picheaders[i].height );
        }

        /* Copy and sort the pics to the tile surface and store the coordinates */
        fileloc = fopen( pathloc, "w" );

        dstrect.x = 0;
        last_h = max_h = 0;

        while (max_h >= 0)
        {
            uint8_t index;

            max_h = -1;

            for (i=0; i<header.count_pic; i++)
            {
                if (picsurfs[i] != NULL && picsurfs[i]->h > max_h)
                {
                    max_h = picsurfs[i]->h;
                    index = i;
                }
            }

            if (max_h > 0)
            {
                if (dstrect.x + picsurfs[index]->w > image->w)
                {
                    dstrect.x = 0;
                    dstrect.y += last_h;
                    last_h = 0;
                }
                dstrect.w = picsurfs[index]->w;
                dstrect.h = picsurfs[index]->h;

                SDL_BlitSurface( picsurfs[index], NULL, image, &dstrect );

                picdims[EGAgr][index] = dstrect;
                fprintf( fileloc, "%d %d %d %d %d\n", index,
                                                      picdims[EGAgr][index].x,
                                                      picdims[EGAgr][index].y,
                                                      picdims[EGAgr][index].w,
                                                      picdims[EGAgr][index].h );

                dstrect.x += picsurfs[index]->w;
                last_h = MAX( last_h, max_h );

                SDL_FreeSurface( picsurfs[index] );
                picsurfs[index] = NULL;
            }
        }

        fclose( fileloc );
        fileloc = NULL;

        SaveBMP( image, pathbmp );
        SDL_FreeSurface( image );
        image = NULL;
    }
    else
    {
        printf( "[SYSTEM] Found EGA image data in cache: %s\n", pathbmp );
        fclose( filebmp );
        fclose( fileloc );
        filebmp = NULL;
        fileloc = NULL;
    }

    if (LoadLocs( pathloc, picdims[EGAgr] ) == False)
        return False;

    return LoadBMP( pathbmp, texture );
}

boolean LoadCGA_PICFormat( tex_t *texture, uint8_t *data, const char *path, uint32_t image_w, uint32_t image_h )
{
    uint8_t even, odd;
    uint32_t i,j;
    uint16_t image_bytes, image_pitch, image_plane;
    uint32_t *pixels    = NULL;
    FILE *file          = NULL;
    SDL_Surface *image  = NULL;

    if (force_refresh == False)
        file = fopen( path, "rb" );

    if (file == NULL)
    {
        printf( "[SYSTEM] Extracting CGA image to bmp\n" );

        image_bytes = (image_w*image_h)/TILE_CGA_BPP;
        image_pitch = image_w/TILE_CGA_BPP;
        image_plane = (image_bytes/2)+192;

        image = SDL_CreateRGBSurface( 0, image_w, image_h, 32, RMASK, GMASK, BMASK, AMASK );

        SDL_FillRect( image, NULL, 0xFFFF00FF );

        pixels = (uint32_t*)image->pixels;

        for (i=0; i<(image_h/2); i++)
        {
            for (j=0; j<image_pitch; j++)
            {
                even = data[(i*image_pitch)+j];
                *(pixels+0) = GET_CGA_COLOR( ((even >> 6) & 0x3) );
                *(pixels+1) = GET_CGA_COLOR( ((even >> 4) & 0x3) );
                *(pixels+2) = GET_CGA_COLOR( ((even >> 2) & 0x3) );
                *(pixels+3) = GET_CGA_COLOR( ((even >> 0) & 0x3) );

                odd  = data[image_plane+(i*image_pitch)+j];
                *(pixels+image_w+0) = GET_CGA_COLOR( ((odd >> 6) & 0x3) );
                *(pixels+image_w+1) = GET_CGA_COLOR( ((odd >> 4) & 0x3) );
                *(pixels+image_w+2) = GET_CGA_COLOR( ((odd >> 2) & 0x3) );
                *(pixels+image_w+3) = GET_CGA_COLOR( ((odd >> 0) & 0x3) );

                pixels += 4;
            }
            pixels += image_w;
        }

        SaveBMP( image, path );

        SDL_FreeSurface( image );
        image = NULL;
    }
    else
    {
        printf( "[SYSTEM] Found CGA image data in cache: %s\n", path );
        fclose( file );
        file = NULL;
    }

    return LoadBMP( path, texture );
}
