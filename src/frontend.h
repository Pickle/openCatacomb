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

#ifndef FRONTEND_H_INCLUDED
#define FRONTEND_H_INCLUDED

#include <stdint.h>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

#define LIST_MAX_DIRS       50
#define LIST_MAX_FILES      10
#define MD5LENGTH           32

typedef pair< string,string > md5file_t; /* name, checksum */

typedef struct MD5LIST_T {
    string name;
    vector< md5file_t > files;
} md5list_t;

uint8_t ScanPath( const string& path, uint32_t types, vector<string>& list );
int32_t RunFrontend( void );
uint8_t LoadChecksums( const string& path, vector<md5list_t>& list );
int8_t VerifyChecksums( const vector<md5list_t>& md5list, const string& dir, const vector<string>& files );

#endif /* FRONTEND_H_INCLUDED */
