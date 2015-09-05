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

#include "input.h"
#include "system.h"

const char *inputnames[] = {
    "Up",
    "Down",
    "Left",
    "Right",
    "Select",
    "Back",
    "Shoot",
    "Strafe",
    "Use Potion",
    "Use Bolt",
    "Use Nuke",
    "Cheat",
    "Help",
    "Setup Sound",
    "Control Panel",
    "Reset Game",
    "Save Game",
    "Load Game",
    "Pause Game",
    "Quit Game"
};

inputmap_t inputmaps[input_total];
boolean *keydown = NULL;
SDL_Joystick *joystick[2] = { NULL, NULL };
uint8_t key_count = 0;
boolean key_press = False;

void InputInit( void )
{
    int32_t result, joycount;

    printf( "[INPUT] Initialize SDL Input\n" );
    result = SDL_InitSubSystem( SDL_INIT_JOYSTICK );
    if (result != 0)
    {
        printf( "[INPUT] ERROR InitInput SDL Input Error: %s\n", SDL_GetError() );
    }

    keydown = SDL_GetKeyState(NULL);
    if (keydown == NULL)
    {
        printf( "[INPUT] ERROR InitInput SDL Input could not get keystate: %s\n", SDL_GetError() );
    }

    joycount = SDL_NumJoysticks();
    printf( "[INPUT] %d joysticks detected by SDL\n", joycount );

    if (joycount >= 1) {
        joystick[0] = OpenJoystick( 0 );
    }
    if (joycount >= 2) {
        joystick[1] = OpenJoystick( 1 );
    }

    SDL_EnableUNICODE( SDL_ENABLE );

    DefaultMaps();

    printf( "[INPUT] SDL Input Completed!\n" );
}

void InputShutDown( void )
{
    printf( "[INPUT] Shutdown SDL Input\n" );
    if (joystick[0] != NULL) {
        SDL_JoystickClose( joystick[0] );
    }
    if (joystick[1] != NULL) {
        SDL_JoystickClose( joystick[1] );
    }
}

SDL_Joystick* OpenJoystick( uint8_t index )
{
    SDL_Joystick *joy = NULL;

    joy = SDL_JoystickOpen( index );

    if (joy != NULL) {
        printf( "[INPUT] Opened Joystick %d\n", index );
        printf( "[INPUT] Number of Axes: %d\n", SDL_JoystickNumAxes(joy) );
        printf( "[INPUT] Number of Buttons: %d\n", SDL_JoystickNumButtons(joy) );
        printf( "[INPUT] Number of Balls: %d\n", SDL_JoystickNumBalls(joy) );
    }
    else
    {
        printf( "[INPUT] Couldn't open Joystick %d\n", index );
    }

    return joy;
}

void InputPollEvents( void )
{
    uint16_t i;

    SDL_PumpEvents();

    for (i=0; i<input_total; i++)
    {
        if (keydown[inputmaps[i].key] >= 1)
        {
            inputmaps[i].pressed = True;
        }
        else
        {
            inputmaps[i].pressed = False;
        }
    }
}

uint8_t InputSelector( void )
{
    uint8_t keys = 0;

    InputPollEvents();

    if (key_press == False)
    {
        if (inputmaps[input_up].pressed == True)
        {
            keys |= SEL_UP;
        }
        else if (inputmaps[input_down].pressed == True)
        {
            keys |= SEL_DOWN;
        }
        else if (inputmaps[input_left].pressed == True)
        {
            keys |= SEL_LEFT;
        }
        else if (inputmaps[input_right].pressed == True)
        {
            keys |= SEL_RIGHT;
        }
        else if (inputmaps[input_select].pressed == True)
        {
            keys |= SEL_SELECT;
        }
        else if (inputmaps[input_back].pressed == True)
        {
            keys |= SEL_BACK;
        }

        if (keys != 0)
        {
            key_press = True;
        }
    }
    else
    {
        key_count++;
        if (key_count >= 10)
        {
            key_count = 0;
            key_press = False;
        }
    }

    return keys;
}

void Rd_Mouse( dirtype_t *dir, boolean *b1, boolean *b2 )
{
    int8_t xmove = 0, ymove = 0;
    uint8_t buttons;
    int32_t x,y;

    buttons = SDL_GetMouseState( &x, &y );

    *b1 = ((buttons&SDL_BUTTON(1)) == SDL_BUTTON(1)) ? True : False;
    *b2 = ((buttons&SDL_BUTTON(2)) == SDL_BUTTON(2)) ? True : False;

    if ((x-320)/2>config.mousesensitivity)
    {
        xmove = 1;
    }
    else if ((x-320)/2<-config.mousesensitivity)
    {
        xmove = -1;
    }
    if ((y-100)>config.mousesensitivity)
    {
        ymove = 1;
    }
    else if ((y-100)<-config.mousesensitivity)
    {
        ymove = -1;
    }

    switch (ymove*3+xmove)
    {
        case -4: *dir = northwest; break;
        case -3: *dir = north; break;
        case -2: *dir = northeast; break;
        case -1: *dir = west; break;
        case  0: *dir = nodir; break;
        case  1: *dir = east; break;
        case  2: *dir = southwest; break;
        case  3: *dir = south; break;
        case  4: *dir = southeast; break;
        default: *dir = nodir;
    }
}

void Rd_Joystick( uint8_t index, dirtype_t *dir, boolean *b1, boolean *b2 )
{
    uint16_t i;
    uint8_t hat;

    if (joystick[index] != NULL)
    {
        hat = SDL_JoystickGetHat( joystick[index], 0 );

        if ((hat&SDL_HAT_UP) == SDL_HAT_UP)
            *dir = south;

        if ((hat&SDL_HAT_RIGHT) == SDL_HAT_RIGHT)
            *dir = west;

        if ((hat&SDL_HAT_DOWN) == SDL_HAT_DOWN)
            *dir = north;

        if ((hat&SDL_HAT_LEFT) == SDL_HAT_LEFT)
            *dir = east;

        if ((hat&SDL_HAT_RIGHTUP) == SDL_HAT_RIGHTUP)
            *dir = southwest;

        if ((hat&SDL_HAT_RIGHTDOWN) == SDL_HAT_RIGHTDOWN)
            *dir = northwest;

        if ((hat&SDL_HAT_LEFTUP) == SDL_HAT_LEFTUP)
            *dir = southeast;

        if ((hat&SDL_HAT_LEFTDOWN) == SDL_HAT_LEFTDOWN)
            *dir = northeast;

        if ((hat&SDL_HAT_CENTERED) == SDL_HAT_CENTERED)
            *dir = nodir;

        *b1 = SDL_JoystickGetButton( joystick[index], 0 );
        *b2 = SDL_JoystickGetButton( joystick[index], 1 );

        for (i=0; i<MIN(input_total, SDL_JoystickNumButtons(joystick[index])); i++)
        {
            inputmaps[i].pressed = SDL_JoystickGetButton( joystick[index], inputmaps[i].joy );
        }
    }
}

void Rd_Keyboard( dirtype_t *dir, boolean *b1, boolean *b2 )
{
    int8_t x = 0;
    int8_t y = 0;

    *b1 = (inputmaps[input_shoot].pressed) ? True : False;
    *b2 = (inputmaps[input_strafe].pressed) ? True : False;

    if (inputmaps[input_up].pressed)
    {
        y--;
    }
    if (inputmaps[input_down].pressed)
    {
        y++;
    }
    if (inputmaps[input_right].pressed)
    {
        x++;
    }
    if (inputmaps[input_left].pressed)
    {
        x--;
    }

    switch (y*3+x)
    {
        case -4: *dir = northwest; break;
        case -3: *dir = north; break;
        case -2: *dir = northeast;  break;
        case -1: *dir = west; break;
        case  0: *dir = nodir; break;
        case  1: *dir = east; break;
        case  2: *dir = southwest; break;
        case  3: *dir = south; break;
        case  4: *dir = southeast; break;
        default: *dir = nodir; break;
    }
}

void DefaultMaps( void )
{
    inputmaps[input_up].support       = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_down].support     = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_left].support     = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_right].support    = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_select].support   = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_back].support     = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_shoot].support    = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_strafe].support   = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_potion].support   = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_bolt].support     = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_nuke].support     = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_cheat].support    = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_help].support     = (1<<catacomb1)|(1<<catacomb2);
#if defined(CATACOMB1)
    inputmaps[input_sound].support    = (1<<catacomb1);
    inputmaps[input_control].support  = (1<<catacomb2);
    inputmaps[input_reset].support    = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_save].support     = (1<<catacomb2);
    inputmaps[input_load].support     = (1<<catacomb2);
#else
    inputmaps[input_sound].support    = (1<<catacomb1);
    inputmaps[input_control].support  = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_reset].support    = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_save].support     = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_load].support     = (1<<catacomb1)|(1<<catacomb2);
#endif /* CATACOMB1 */
    inputmaps[input_pause].support    = (1<<catacomb1)|(1<<catacomb2);
    inputmaps[input_quit].support     = (1<<catacomb1)|(1<<catacomb2);

    /* Default Mapping */
    SetKey( input_up,       SDLK_UP );
    SetKey( input_down,     SDLK_DOWN );
    SetKey( input_left,     SDLK_LEFT );
    SetKey( input_right,    SDLK_RIGHT );
    SetKey( input_select,   SDLK_RETURN );
    SetKey( input_back,     SDLK_ESCAPE );
    SetKey( input_shoot,    SDLK_LCTRL );
    SetKey( input_strafe,   SDLK_LALT );
    SetKey( input_potion,   SDLK_p );
    SetKey( input_bolt,     SDLK_b );
    SetKey( input_nuke,     SDLK_n );
    SetKey( input_cheat,    SDLK_q );
    SetKey( input_help,     SDLK_F1 );
#if defined(CATACOMB1)
    SetKey( input_sound,    SDLK_F2 );
    SetKey( input_control,  SDLK_FIRST );
    SetKey( input_reset,    SDLK_F3 );
    SetKey( input_save,     SDLK_FIRST );
    SetKey( input_load,     SDLK_FIRST );
#else
    SetKey( input_sound,    SDLK_FIRST );
    SetKey( input_control,  SDLK_F2 );
    SetKey( input_reset,    SDLK_F3 );
    SetKey( input_save,     SDLK_F4 );
    SetKey( input_load,     SDLK_F5 );
#endif /* CATACOMB1 */
    SetKey( input_pause,    SDLK_F9 );
    SetKey( input_quit,     SDLK_F10 );

    /* Custom Overides */
#if defined(PANDORA)
    SetKey( input_up,       SDLK_UP );
    SetKey( input_down,     SDLK_DOWN );
    SetKey( input_left,     SDLK_LEFT );
    SetKey( input_right,    SDLK_RIGHT );
    SetKey( input_select,   SDLK_RETURN );
    SetKey( input_back,     SDLK_ESCAPE );
    SetKey( input_shoot,    SDLK_HOME );
    SetKey( input_strafe,   SDLK_PAGEDOWN );
    SetKey( input_potion,   SDLK_END );
    SetKey( input_bolt,     SDLK_PAGEUP );
    SetKey( input_nuke,     SDLK_RCTRL );
    SetKey( input_cheat,    SDLK_q );
    SetKey( input_help,     SDLK_1 );
#if defined(CATACOMB1)
    SetKey( input_sound,    SDLK_2 );
    SetKey( input_reset,    SDLK_3 );
#else
    SetKey( input_control,  SDLK_2 );
    SetKey( input_reset,    SDLK_3 );
    SetKey( input_save,     SDLK_4 );
    SetKey( input_load,     SDLK_5 );
#endif /* CATACOMB1 */
    SetKey( input_pause,    SDLK_9 );
    SetKey( input_quit,     SDLK_0 );

#elif defined(GCW)
    SetKey( input_up,       SDLK_UP );
    SetKey( input_down,     SDLK_DOWN );
    SetKey( input_left,     SDLK_LEFT );
    SetKey( input_right,    SDLK_RIGHT );
    SetKey( input_select,   SDLK_RETURN );
    SetKey( input_back,     SDLK_ESCAPE );
    SetKey( input_shoot,    SDLK_LCTRL );
    SetKey( input_strafe,   SDLK_FIRST );
    SetKey( input_potion,   SDLK_SPACE );
    SetKey( input_bolt,     SDLK_LSHIFT );
    SetKey( input_nuke,     SDLK_LALT );
    SetKey( input_cheat,    SDLK_q );
    SetKey( input_help,     SDLK_1 );
    SetKey( input_pause,    SDLK_9 );
    SetKey( input_quit,     SDLK_ESCAPE );

#if !defined(CATACOMB1)
    SetKey( input_save,     SDLK_TAB );
    SetKey( input_load,     SDLK_BACKSPACE );
#endif
#endif
}

void SetKey( uint8_t index, SDLKey sym )
{
    inputmaps[index].key = sym;
    strncpy( inputmaps[index].keyname, SDL_GetKeyName( inputmaps[index].key ), sizeof(inputmaps[index].keyname) );
}

void SetButton( uint8_t index, uint8_t button )
{
    inputmaps[index].joy = button;
}
