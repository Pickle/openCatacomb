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

#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACTIVE_KEY(X)  ((keys & X) == X)
#define SEL_UP          (1<<input_up)
#define SEL_DOWN        (1<<input_down)
#define SEL_LEFT        (1<<input_left)
#define SEL_RIGHT       (1<<input_right)
#define SEL_SELECT      (1<<input_select)
#define SEL_BACK        (1<<input_back)

enum {
    input_up,
    input_down,
    input_left,
    input_right,
    input_select,
    input_back,
    input_shoot,
    input_strafe,
    input_potion,
    input_bolt,
    input_nuke,
    input_cheat,
    input_help,
    input_sound,
    input_control,
    input_reset,
    input_save,
    input_load,
    input_pause,
    input_quit,
    input_total
};

typedef struct INPUTMAP_T {
    uint8_t support;
    SDLKey key;
    char keyname[50];
    uint8_t joy;
    boolean pressed;
} inputmap_t;

extern inputmap_t inputmaps[input_total];
extern const char *inputnames[];
extern boolean *keydown;
extern SDL_Joystick *joystick[2];
extern uint8_t mousesensitivity;

void InputInit( void );
void InputShutDown( void );
SDL_Joystick* OpenJoystick( uint8_t index );
void InputPollEvents( void );
uint8_t InputSelector( void );
void Rd_Mouse( dirtype_t *dir, boolean *b1, boolean *b2 );
void Rd_Joystick( uint8_t index, dirtype_t *dir, boolean *b1, boolean *b2 );
void Rd_Keyboard( dirtype_t *dir, boolean *b1, boolean *b2 );
void DefaultMaps( void );
void SetKey( uint8_t index, SDLKey sym );
void SetButton( uint8_t index, uint8_t button );

#ifdef __cplusplus
}
#endif

#endif /* INPUT_H_INCLUDED */
