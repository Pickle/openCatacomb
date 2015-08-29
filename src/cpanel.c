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

#include "cpanel.h"

#include "catacomb.h"
#include "opengl.h"
#include "video.h"
#include "sound.h"
#include "input.h"
#include "system.h"

#define PANEL_COLS 5
#define PANEL_ROWS 3

int8_t rowy[4] = {4,9,14,19};
int8_t colx[4] = {14,20,26,32};
boolean spotok[PANEL_ROWS][PANEL_COLS];
int8_t row,col;
boolean joy1ok,joy2ok;

/*
////////////////////
//
// getconfig
// Checks video cards, mouse, and joysticks
//
////////////////////
*/

void GetConfig( void )
{
    spotok[0][0] = True;    /* CGA */
    spotok[0][1] = True;    /* EGA */
    spotok[0][2] = False;   /* VGA */
    spotok[0][3] = False;
    spotok[0][4] = False;

    spotok[1][0] = True;    /* Use Sound */
    spotok[1][1] = True;    /* No Sound */
    spotok[1][2] = False;
    spotok[1][3] = False;
    spotok[1][4] = False;

    joy1ok = (joystick[0] != NULL) ? True : False;
    joy2ok = (joystick[1] != NULL) ? True : False;

    spotok[2][0] = True;    /* Keyboard */
    spotok[2][1] = True;    /* Mouse */
    spotok[2][2] = joy1ok;
    spotok[2][3] = joy2ok;
    spotok[2][4] = False;
}

/*
////////////////
//
// controlpanel
//
////////////////
*/

void ControlPanel( void )
{
    uint8_t keys;
    int oldcenterx,oldcentery;
    uint16_t prevtiles[TILEVIEW_W][TILEVIEW_H];

    /* Save previous screen contents */
    memcpy( prevtiles, tiles, sizeof(prevtiles) );

    StopSound();		/* pause any sound that is playing */

    GetConfig();

    oldcenterx=screencenterx;
    oldcentery=screencentery;

    screencenterx = 19;
    screencentery = 11;

    row = 0;
    col = config.graphmode;

    do
    {
        InputPollEvents();
        keys = InputSelector();

        DrawWindow( 0,0,39,24 );

        sx=colx[col]+2;
        sy=rowy[row]+3;

        if (ACTIVE_KEY(SEL_UP))
        {
            /* Up */
            if (--row<0)
            {
                row = PANEL_ROWS-1;
            }
        }
        else if (ACTIVE_KEY(SEL_DOWN))
        {
            /* Down */
            if (++row>=PANEL_ROWS)
            {
                row = 0;
            }
        }
        else if (ACTIVE_KEY(SEL_LEFT))
        {
            /* Left */
            if (--col<0)
            {
                col = PANEL_COLS-1;
            }
        }
        else if (ACTIVE_KEY(SEL_RIGHT))
        {
            /* Right */
            if (++col>=PANEL_COLS)
            {
                col = 0;
            }
        }

        /* see if up or down took you to a bad spot */
        if (spotok[row][col] == False) {
            row = 0;
        }

        /* see if up or down took you to a bad spot */
        while (spotok[row][col] == False) {
            col--;
            if (col <= 0) {
                col = 0;
                break;
            }
        }

        if (ACTIVE_KEY(SEL_SELECT))		/* return */
        {
            switch (row)
            {
                case 0:
                    config.graphmode = col;
                    break;
                case 1:
                    config.soundmode = col;
                    break;
                case 2:
                    config.inputmode = col;

                    if (config.inputmode == keyboard)
                        SetupKeys();
                    else if (config.inputmode == mouse)
                        SetupMouse();
                    else if (config.inputmode == joystick1)
                        SetupJoy();
                    else if (config.inputmode == joystick2)
                        SetupJoy();
                    break;
                default:
                    col = 0;
                    row = 0;
            }
        }

        /* Selector */
        CharOut( colx[col]+1,rowy[row]+3,15 );

        DrawPanel();

        VideoSwapBuffers();

    } while (!ACTIVE_KEY(SEL_BACK));	/* ESC to quit */

    /*
    // done, so return to game
    */
    screencenterx = oldcenterx;
    screencentery = oldcentery;

    ResumeSound ();		/* continue any sound that was playing */

    /* Restore previous screen contents */
    memcpy( tiles, prevtiles, sizeof(prevtiles) );
}


/*
////////////////
//
// drawpanel
// Draws everything inside the control panel window.
// Used to refresh under dialogs.
//
////////////////
*/

void DrawPanel( void )
{
    SDL_Rect dest = { 0, 0, 0, 0 };
    leftedge=1;

    sx=8;
    sy=2;
    Print( "       Control Panel      ]" );

    sy=rowy[0]+2;
    sx=2;
    Print( "VIDEO:" );

    sy=rowy[1]+2;
    sx=2;
    Print( "SOUND:" );

    sy=rowy[2]+2;
    sx=2;
    Print( "CONTROL:" );

    CharOut( colx[config.graphmode],rowy[0]+3,15 );
    CharOut( colx[config.soundmode],rowy[1]+3,15 );
    CharOut( colx[config.inputmode],rowy[2]+3,15 );

    sy=21;
    sx=1;
    Print( "  Move the cursor with the arrow keys ]" );
    Print( "   Make decisions with the ENTER key  ]" );
    Print( "       ESC to return to your game     ]" );

    RefreshTiles( &textiles[config.graphmode] );

    if (episode == catacomb2)
    {
        /* CGA */
        dest.x = colx[0]*8;
        dest.y = rowy[0]*8;
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_cga] );

        /* EGA */
        dest.x = colx[1]*8;
        dest.y = rowy[0]*8;
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_ega] );

        /* Sound */
        dest.x = colx[0]*8;
        dest.y = rowy[1]*8;
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_nosound] );

        dest.x = colx[1]*8;
        dest.y = rowy[1]*8;
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_speaker] );

        /* Keyboard */
        dest.x = colx[0]*8;
        dest.y = rowy[2]*8;
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_keyboard] );

        /* Mouse */
        dest.x = colx[1]*8;
        OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_mouse] );

        /* Joy1 */
        dest.x = colx[2]*8;
        if (joy1ok == True)
            OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_joy1] );
        else
            OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_nojoy] );

        /* Joy2 */
        dest.x = colx[3]*8;
        if (joy2ok == True)
            OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_joy2] );
        else
            OGL_DrawImage( &texpics[config.graphmode], False, &dest, &picdims[config.graphmode][pic_nojoy] );
    }
}

void SetupKeys( void )
{
    boolean modify = False;
    uint8_t i, keys;
    int8_t sel = 0;
    SDLKey newkey;

    do
    {
        keys = InputSelector();

        if (ACTIVE_KEY(SEL_UP))
        {
            if (--sel<0)
            {
                sel = input_total-1;
            }

            while (sel > 0 && (inputmaps[sel].support & (1<<episode)) != (1<<episode)) {
                sel--;
            }
        }
        else if (ACTIVE_KEY(SEL_DOWN))
        {
            if (++sel>input_total)
            {
                sel = 0;
            }

            while (sel < input_total && (inputmaps[sel].support & (1<<episode)) != (1<<episode)) {
                sel++;
            }
        }
        else if (ACTIVE_KEY(SEL_SELECT))
        {
            modify = True;
        }

        DrawWindow( 0,0,39,24 );

        sx=9; sy=2;
        Print( "Keyboard Configuration]" );

        sx=2; sy=4;
        leftedge=2;
        for (i=0; i<input_total; i++)
        {
            if ((inputmaps[i].support & (1<<episode)) == (1<<episode))
            {
                if ((sel == i) && (modify == True))
                {
                    Print( "%-15s : <press new key>]", inputnames[i] );
                }
                else
                {
                    Print( "%-15s : %-15s]", inputnames[i], inputmaps[i].keyname );
                }
            }

            if (sel == i)
            {
                CharOut( 35, sy-1, 15 );
            }
        }

        if (modify == True)
        {
            GetKey( &newkey, False );
            SetKey( sel, newkey );
            modify = False;
        }
        else
        {
            RefreshTiles( &textiles[config.graphmode] );
            VideoSwapBuffers();
        }

    } while (!ACTIVE_KEY(SEL_BACK));
}

void SetupMouse( void )
{
    boolean quit = False;
    char num;

    do
    {
        InputPollEvents();

        DrawWindow( 0,0,39,24 );

        sx=10; sy=2;
        Print( "Mouse Configuration]" );

        sx=9; sy=4;
        leftedge=9;
        Print( "Choose the sensitivity]");
        Print( "Currently set to: %d]", config.mousesensitivity );
        Print( "of the mouse, 1 being ]");
        Print( " slow, 9 being fast:  ");

        Input( &num,1 );

        DrawWindow( 0,0,39,24 );
        VideoSwapBuffers();

        if (num >= '1' && num <= '9') {
            config.mousesensitivity = atoi( &num );
            quit = True;
        }

    } while (quit == False);
}

void SetupJoy( void )
{
    boolean modify = False;
    uint8_t i, keys;
    int8_t sel = 0;
    uint8_t newjoy;

    do
    {
        InputPollEvents();
        keys = InputSelector();

        if (ACTIVE_KEY(SEL_UP))
        {
            if (--sel<0)
            {
                sel = input_total-1;
            }

            while (sel > 0 && (inputmaps[sel].support & (1<<episode)) != (1<<episode)) {
                sel--;
            }
        }
        else if (ACTIVE_KEY(SEL_DOWN))
        {
            if (++sel>input_total)
            {
                sel = 0;
            }

            while (sel < input_total && (inputmaps[sel].support & (1<<episode)) != (1<<episode)) {
                sel++;
            }
        }
        else if (ACTIVE_KEY(SEL_SELECT))
        {
            modify = True;
        }

        DrawWindow( 0,0,39,24 );

        sx=9; sy=2;
        Print( "Joystick Configuration]" );

        sx=2; sy=4;
        leftedge=2;
        for (i=0; i<input_total; i++)
        {
            if ((inputmaps[i].support & (1<<episode)) == (1<<episode))
            {
                if ((sel == i) && (modify == True))
                {
                    Print( "%-15s : <press new button>]", inputnames[i] );
                }
                else
                {
                    Print( "%-15s : %d]", inputnames[i], inputmaps[i].joy );
                }
            }

            if (sel == i)
            {
                CharOut( 35, sy-1, 15 );
            }
        }

        if (modify == True)
        {
            GetButton( &newjoy, False );
            SetButton( sel, newjoy );
            modify = False;
        }
        else
        {
            RefreshTiles( &textiles[config.graphmode] );
            VideoSwapBuffers();
        }

    } while (!ACTIVE_KEY(SEL_BACK));
}
