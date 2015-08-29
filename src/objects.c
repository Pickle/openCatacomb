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

#include "objects.h"
#include "catacomb.h"

/*
{                        }
{ initialize object info }
{                        }
*/

void InitObjects( void )
{
    ObjDef[player].think	    = playercmd;
    ObjDef[player].contact	    = benign;
    ObjDef[player].solid	    = True;
    ObjDef[player].firstchar	= TILE2S;
    ObjDef[player].size	        = 2;
    ObjDef[player].stages	    = 4;
    ObjDef[player].dirmask	    = 3;
    ObjDef[player].speed	    = 256;
    ObjDef[player].hitpoints    = 12;
    ObjDef[player].damage	    = 0;
    ObjDef[player].points	    = 0;

    ObjDef[goblin].think	    = ramstraight;
    ObjDef[goblin].contact	    = monster;
    ObjDef[goblin].solid	    = True;
    ObjDef[goblin].firstchar	= TILE2S+64;
    ObjDef[goblin].size     	= 2;
    ObjDef[goblin].stages	    = 4;
    ObjDef[goblin].dirmask	    = 3;
    ObjDef[goblin].speed	    = 75;
    ObjDef[goblin].hitpoints	= 1;
    ObjDef[goblin].damage	    = 1;
    ObjDef[goblin].points	    = 50;

    ObjDef[skeleton].think 	    = ramdiag;
    ObjDef[skeleton].contact	= monster;
    ObjDef[skeleton].solid	    = True;
    ObjDef[skeleton].firstchar	= TILE2S+128;
    ObjDef[skeleton].size	    = 2;
    ObjDef[skeleton].stages	    = 4;
    ObjDef[skeleton].dirmask	= 3;
    ObjDef[skeleton].speed 	    = 130;
    ObjDef[skeleton].hitpoints	= 1;
    ObjDef[skeleton].damage	    = 1;
    ObjDef[skeleton].points	    = 150;

    ObjDef[ogre].think	        = ramstraight;
    ObjDef[ogre].contact	    = monster;
    ObjDef[ogre].solid	        = True;
    ObjDef[ogre].firstchar	    = TILE3S;
    ObjDef[ogre].size	        = 3;
    ObjDef[ogre].stages	        = 4;
    ObjDef[ogre].dirmask	    = 3;
    ObjDef[ogre].speed	        = 75;
    ObjDef[ogre].hitpoints	    = 5;
    ObjDef[ogre].damage	        = 2;
    ObjDef[ogre].points	        = 250;

    ObjDef[gargoyle].think	    = gargcmd;
    ObjDef[gargoyle].contact	= monster;
    ObjDef[gargoyle].solid	    = True;
    ObjDef[gargoyle].firstchar	= TILE4S;
    ObjDef[gargoyle].size	    = 4;
    ObjDef[gargoyle].stages	    = 4;
    ObjDef[gargoyle].dirmask	= 3;
    ObjDef[gargoyle].speed	    = 150;
    ObjDef[gargoyle].hitpoints	= 10;
    ObjDef[gargoyle].damage	    = 3;
    ObjDef[gargoyle].points	    = 500;

    ObjDef[dragon].think	    = dragoncmd;
    ObjDef[dragon].contact	    = monster;
    ObjDef[dragon].solid	    = True;
    ObjDef[dragon].firstchar	= TILE5S;
    ObjDef[dragon].size	        = 5;
    ObjDef[dragon].stages	    = 4;
    ObjDef[dragon].dirmask	    = 3;
    ObjDef[dragon].speed	    = 100;
    ObjDef[dragon].hitpoints	= 100;
    ObjDef[dragon].damage	    = 5;
    ObjDef[dragon].points	    = 1000;

    /* {an explosion on a wall that didn't blow up} */
    ObjDef[wallhit].think	    = fade;
    ObjDef[wallhit].contact	    = benign;
    ObjDef[wallhit].solid	    = True;
    ObjDef[wallhit].firstchar	= 26;
    ObjDef[wallhit].size	    = 1;
    ObjDef[wallhit].stages	    = 3;
    ObjDef[wallhit].dirmask	    = 0;
    ObjDef[wallhit].speed	    = 80;
    ObjDef[wallhit].hitpoints	= 0;
    ObjDef[wallhit].damage 	    = 0;
    ObjDef[wallhit].points	    = 0;


    ObjDef[dead1].think	        = fade;
    ObjDef[dead1].contact	    = benign;
    ObjDef[dead1].solid	        = False;
    ObjDef[dead1].firstchar	    = 29;
    ObjDef[dead1].size	        = 1;
    ObjDef[dead1].stages	    = 3;
    ObjDef[dead1].dirmask	    = 0;
    ObjDef[dead1].speed	        = 80;
    ObjDef[dead1].hitpoints	    = 0;
    ObjDef[dead1].damage	    = 0;
    ObjDef[dead1].points	    = 0;

    ObjDef[dead2].think	        = fade;
    ObjDef[dead2].contact	    = benign;
    ObjDef[dead2].solid	        = False;
    ObjDef[dead2].firstchar	    = TILE2S+224;
    ObjDef[dead2].size	        = 2;
    ObjDef[dead2].stages	    = 3;
    ObjDef[dead2].dirmask	    = 0;
    ObjDef[dead2].speed	        = 80;
    ObjDef[dead2].hitpoints	    = 0;
    ObjDef[dead2].damage	    = 0;
    ObjDef[dead2].points	    = 0;

    ObjDef[dead3].think	        = fade;
    ObjDef[dead3].contact	    = benign;
    ObjDef[dead3].solid	        = False;
    ObjDef[dead3].firstchar	    = TILE3S + 9*16;
    ObjDef[dead3].size	        = 3;
    ObjDef[dead3].stages	    = 3;
    ObjDef[dead3].dirmask	    = 0;
    ObjDef[dead3].speed	        = 80;
    ObjDef[dead3].hitpoints	    = 0;
    ObjDef[dead3].damage	    = 0;
    ObjDef[dead3].points	    = 0;

    ObjDef[dead4].think	        = fade;
    ObjDef[dead4].contact	    = benign;
    ObjDef[dead4].solid	        = False;
    ObjDef[dead4].firstchar	    = TILE4S + 16*16;
    ObjDef[dead4].size	        = 4;
    ObjDef[dead4].stages	    = 3;
    ObjDef[dead4].dirmask	    = 0;
    ObjDef[dead4].speed	        = 80;
    ObjDef[dead4].hitpoints	    = 0;
    ObjDef[dead4].damage	    = 0;
    ObjDef[dead4].points	    = 0;

    ObjDef[dead5].think	        = fade;
    ObjDef[dead5].contact	    = benign;
    ObjDef[dead5].solid	        = False;
    ObjDef[dead5].firstchar	    = TILE5S + 25*16;
    ObjDef[dead5].size	        = 5;
    ObjDef[dead5].stages	    = 3;
    ObjDef[dead5].dirmask	    = 0;
    ObjDef[dead5].speed	        = 80;
    ObjDef[dead5].hitpoints	    = 0;
    ObjDef[dead5].damage	    = 0;
    ObjDef[dead5].points	    = 0;

    ObjDef[shot].think	        = straight;
    ObjDef[shot].contact	    = pshot;
    ObjDef[shot].solid	        = False;
    ObjDef[shot].firstchar	    = 154;
    ObjDef[shot].size	        = 1;
    ObjDef[shot].stages	        = 2;
    ObjDef[shot].dirmask	    = 3;
    ObjDef[shot].speed	        = 256;
    ObjDef[shot].hitpoints	    = 0;
    ObjDef[shot].damage	        = 1;
    ObjDef[shot].points	        = 0;

    ObjDef[rock].think	        = straight;
    ObjDef[rock].contact	    = mshot;
    ObjDef[rock].solid	        = False;
    ObjDef[rock].firstchar	    = 152;
    ObjDef[rock].size	        = 1;
    ObjDef[rock].stages	        = 2;
    ObjDef[rock].dirmask	    = 0;
    ObjDef[rock].speed	        = 256;
    ObjDef[rock].hitpoints	    = 0;
    ObjDef[rock].damage	        = 1;
    ObjDef[rock].points	        = 0;

    ObjDef[bigshot].think	    = straight;
    ObjDef[bigshot].contact	    = nukeshot;
    ObjDef[bigshot].solid	    = False;
    ObjDef[bigshot].firstchar   = TILE2S+192;
    ObjDef[bigshot].size	    = 2;
    ObjDef[bigshot].stages	    = 2;
    ObjDef[bigshot].dirmask	    = 3;
    ObjDef[bigshot].speed	    = 256;
    ObjDef[bigshot].hitpoints	= 0;
    ObjDef[bigshot].damage	    = 1;
    ObjDef[bigshot].points	    = 0;

    ObjDef[teleporter].think	    = idle;
    ObjDef[teleporter].contact	    = benign;
    ObjDef[teleporter].solid	    = False;
    ObjDef[teleporter].firstchar    = TILE2S+236;
    ObjDef[teleporter].size	        = 2;
    ObjDef[teleporter].stages	    = 5;
    ObjDef[teleporter].dirmask	    = 0;
    ObjDef[teleporter].speed	    = 200;
    ObjDef[teleporter].hitpoints	= 0;
    ObjDef[teleporter].damage	    = 0;
    ObjDef[teleporter].points	    = 0;
}

void SetPriorities( void )
{
    int16_t i, x, y;
    classtype_t clvar;

    memset( priority, 99, sizeof(priority) );

    priority[BLANKFLOOR] = 0;

    for (i=ObjDef[teleporter].firstchar; i<=ObjDef[teleporter].firstchar+20; i++) {
        priority[i] = 0;
    }

    for (clvar=dead2; clvar<=dead5; clvar++) {
        for (i=ObjDef[clvar].firstchar; i<=ObjDef[clvar].firstchar+ObjDef[clvar].size*ObjDef[clvar].size; i++) {
            priority[i] = 0;    /* {deadthing} */
        }
    }

    for (i=152; i<=161; i++) {
        priority[i] = 2;		/* {shots} */
    }

    for (i=ObjDef[bigshot].firstchar; i<=ObjDef[bigshot].firstchar + 31; i++) {
        priority[i] = 2;		/* {bigshot} */
    }

    for (i=0; i<=TILE2S-1; i++) {
        if (priority[i]==99) {
            priority[i] = 3;    /* {most 1*1 tiles are walls, etc} */
        }
    }

    for (i=TILE2S; i<MAXPICS; i++) {
        if (priority[i]==99) {
            priority[i] = 4;		/* {most bigger tiles are monsters} */
        }
    }

    for (i=ObjDef[player].firstchar; i<=ObjDef[player].firstchar + 63; i++) {
        priority[i] = 5;            /* {player} */
    }

    side = 0;
    for (x=0; x<=85; x++)
    {
        for (y=0; y<=TOPOFF-1; y++)
        {
            view[x][y] = SOLIDWALL;
            view[x][85-y] = SOLIDWALL;
            background[x][y] = SOLIDWALL;
            background[x][85-y] = SOLIDWALL;
        }
        view[86][x] = SOLIDWALL;
    }
    for (y=11; y<=74; y++)
    {
        for (x=0; x<=LEFTOFF-1; x++)
        {
            view[x][y] = SOLIDWALL;
            view[85-x][y] = SOLIDWALL;
            background[x][y] = SOLIDWALL;
            background[85-x][y] = SOLIDWALL;
        }
    }
}
