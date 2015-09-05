/*
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

#include "frontend.h"

#include "system.h"
#include "util.h"
#include "video.h"
#include "input.h"
#include "opengl.h"
#include "md5.h"
#include "catacomb.h"

int32_t main( int32_t argc, char *argv[] )
{
    int32_t result;

    ParseArgs( argc, argv );
    SystemInit();

    result = RunFrontend();

    SystemShutdown();
    return result;
}

uint8_t ScanPath( const string& path, uint32_t types, vector<string>& list )
{
    DIR *dp;
    char fullpath[STRSIZE];
    struct dirent *dirp;
    struct stat sb;

    if ((dp = opendir( path.c_str() )) == NULL)
    {
        printf( "Failed to open dir path '%s' code: '%d'\n", path.c_str(), errno );
        return 1;
    }

    list.clear();

    while ((dirp = readdir(dp)) != NULL)
    {
        if (strlen(dirp->d_name) > 0)
        {
            /* Skip . and .. */
            if (strcmp( dirp->d_name, ".") == 0 || strcmp( dirp->d_name, "..") == 0) {
                continue;
            }

            snprintf( fullpath, STRSIZE, "%s/%s", path.c_str(), dirp->d_name );

            /* Check if dir */
            if (stat( fullpath, &sb) == -1) {
                perror("stat");
                continue;
            }

            if (((sb.st_mode & S_IFMT) & types) == types)
            {
                list.push_back( string(dirp->d_name) );
            }
        }
    }
    closedir(dp);

    return 0;
}

int32_t RunFrontend( void )
{
    #define LOADING "Loading ..."
    bool quit = false;
    int8_t i, sel = 0;
    uint8_t keys;
    uint32_t y;
    int32_t result = 0;
#if defined(GCW)
    string scanpath = "/media/sdcard/catacomb/";
#else
    string scanpath = "./";
#endif

    vector< md5list_t > md5list;
    vector< string > dirs;
    vector< int8_t > episodes;
    vector< vector< string > > files;

    OGL_DrawString( &font, 2.0f, (BASE_W-strlen(TITLE)*font.w*2)/2, font.h, TITLE );
    OGL_DrawString( &font, 1.0f, (BASE_W-strlen(LOADING)*font.w)/2, BASE_H/2, LOADING );
    VideoSwapBuffers ();

    LoadChecksums( "md5checksums.txt", md5list );

    /* Read directories */
    ScanPath( scanpath, S_IFDIR, dirs );

    /* Read files */
    files.resize( dirs.size() );
    episodes.resize( dirs.size() );

    for (i=dirs.size()-1; i>=0; i--)
    {
        ScanPath( scanpath+dirs.at(i), S_IFREG, files[i] );

        /* Check for good data files */
        episodes[i] = VerifyChecksums( md5list, scanpath+dirs.at(i), files.at(i) );
        if (episodes[i] < 0)
        {
            dirs.erase( dirs.begin()+i );
            files.erase( files.begin()+i );
            episodes.erase( episodes.begin()+i );
        }
        else
        {
            /* Convert and cache the data */
            result = LoadData( episodes[i], string(scanpath+dirs.at(i)).c_str() );

            CloseData();
        }
    }
    dirs.push_back( "Quit" );

    while (quit == false)
    {
        keys = InputSelector();

        if (ACTIVE_KEY(SEL_UP))
        {
            if (--sel<0)
            {
                sel = (int8_t)dirs.size()-1;
            }
        }
        else if (ACTIVE_KEY(SEL_DOWN))
        {
            if (++sel>=(int8_t)dirs.size())
            {
                sel = 0;
            }
        }
        else if (ACTIVE_KEY(SEL_SELECT))
        {
            if (sel == (int8_t)dirs.size()-1)
            {
                quit = true;
            }
            else
            {
                CatacombMain( episodes[sel], string(scanpath+dirs.at(sel)).c_str() );
            }
        }

        OGL_DrawString( &font, 2.0f, (BASE_W-strlen(TITLE)*font.w*2)/2, font.h, TITLE );
        OGL_DrawString( &font, 1.0f,  font.w*4, font.h*4, "Folder               Game Dectected" );

        y = font.h*5;
        for (i=0; i<(int8_t)dirs.size(); i++ )
        {
            y += font.h;
            if (i == (int8_t)dirs.size()-1)
            {
                y += font.h;
                OGL_DrawString( &font, 1.0f, font.w*4, y, "%s", dirs.at(i).c_str() );
            }
            else
            {
                OGL_DrawString( &font, 1.0f, font.w*4, y, "%-20s %s", dirs.at(i).c_str(), md5list.at(episodes[i]).name.c_str() );
            }
            if (sel == i) {
                OGL_DrawString( &font, 1.0f, font.w, y, SELECTOR );
            }
        }
        OGL_DrawString( &font, 1.0f, (BASE_W-strlen(OC_COPYRIGHT)*font.w)/2, BASE_H-font.h*2, "%s ", OC_COPYRIGHT );
        OGL_DrawString( &font, 1.0f, (BASE_W-strlen(CATA_COPYRIGHT)*font.w)/2, BASE_H-font.h*1, "%s ", CATA_COPYRIGHT );

        VideoSwapBuffers();
        VideoClearBuffer();
    }

    return result;
}

uint8_t LoadChecksums( const string& path, vector<md5list_t>& list )
{
    size_t pos1, pos2, pos3;
    string line;
    ifstream file;

    file.open( path.c_str(), ifstream::in );

    if (file.is_open() == true)
    {
        while (getline( file, line ))
        {
            /* Look for sections */
            pos1 = line.find_first_of( '[', 0);
            pos2 = line.find_last_of( ']' );

            if ((pos1 != string::npos) && (pos2 != string::npos) && (pos2 > pos1))
            {
                list.push_back( md5list_t() );
                list.back().name = line.substr( pos1+1, pos2-pos1-1 );

#if defined(DEBUG)
                printf( "[DEBUG] MD5 added section %s\n", list.back().name.c_str() );
#endif

                continue;
            }

            /* Look for files */
            pos1 = line.find_first_of( '"', 0 );
            pos2 = line.find_last_of( '"' );
            pos3 = line.find_last_of( '=' );

            if ((pos1 != string::npos) && (pos2 != string::npos) && (pos3 != string::npos) && (pos2 > pos1))
            {
                list.back().files.push_back( make_pair( line.substr( pos1+1, pos2-pos1-1 ),
                                                        line.substr( pos3+2, line.length()-pos3-1 ) ) );

#if defined(DEBUG)
                printf( "[DEBUG]   MD5 list added file %16s MD5 %s\n", list.back().files.back().first.c_str()
                                                                     , list.back().files.back().second.c_str() );
#endif
                continue;
            }
        }

        file.close();
    }
    else
    {
        printf ("ERROR: Could not load %s\n", path.c_str() );
        return 1;
    }

    return 0;
}


int8_t VerifyChecksums( const vector<md5list_t>& md5list, const string& dir, const vector<string>& files )
{
    bool valid = false;
    int8_t result = -1;
    char path[STRSIZE];
    uint8_t value[16];
    char checksum[33];
    uint8_t *data = NULL;
    uint32_t size = 0;
    uint32_t i,j,k,x;
    MD5_CTX hash;

#if defined(DEBUG)
    printf( "[DEBUG] Checking directory for valid data: %s\n", dir.c_str() );
#endif

    if (files.size() > 0)
    {
        for (i=0; i<md5list.size(); i++)
        {
            for (j=0; j<md5list.at(i).files.size(); j++)
            {
                valid = true;

                for (k=0; k<files.size(); k++)
                {
                    if (md5list.at(i).files.at(j).first.compare( files.at(k) ) == 0)
                    {
                        snprintf( path, STRSIZE, "%s/%s", dir.c_str(), files.at(k).c_str() );

                        size = LoadDataFromFile( path, &data, 0, 0 );

                        if (data != NULL)
                        {
                            /* Calculate MD5 sum and convert to string */
                            MD5_Init( &hash );
                            MD5_Update( &hash, data, size );
                            MD5_Final( &value[0], &hash );

                            free( data );
                            data = NULL;

                            int ptr = 0;
                            for( x = 0; x < sizeof(value); x++)
                            {
                                ptr += snprintf( checksum + ptr, sizeof(checksum) - ptr, "%02X", value[x] );
                            }

                            if (md5list.at(i).files.at(j).second.compare( 0, strlen(checksum), checksum ) == 0)
                            {
#if defined(DEBUG)
                                printf( "[DEBUG]    MD5 match [%s] %s\n", md5list.at(i).name.c_str(), md5list.at(i).files.at(j).first.c_str() );
#endif
                                valid = true;
                                break;
                            }
                            else
                            {
#if defined(DEBUG)
                                printf( "[DEBUG]    MD5 did not match %s\n", md5list.at(i).files.at(j).first.c_str() );
#endif
                                valid = false;
                                break;
                            }
                        }
                        else
                        {
                            valid = false;
                            break;
                        }
                    }

                    if (valid == false) {
                        break;
                    }
                }

                if (k >= files.size() || valid == false)
                {
                    valid = false;
#if defined(DEBUG)
                    printf( "[DEBUG]    did not find file [%s] %s\n", md5list.at(i).name.c_str(), md5list.at(i).files.at(j).first.c_str() );
#endif
                    break;
                }
            }

            if (j >= md5list.at(i).files.size() && valid == true)
            {
                result = i;
                break;
            }
        }
    }
    else
    {
#if defined(DEBUG)
        printf( "[DEBUG]    no files\n" );
#endif
    }

#if defined(DEBUG)
    printf( "[DEBUG] Scan result: " );
    if (result >= 0)
        printf( "%s (%d)\n", md5list.at(result).name.c_str(), result );
    else
        printf( "not valid\n" );
#endif

    return result;
}
