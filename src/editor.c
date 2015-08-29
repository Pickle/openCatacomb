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

#include "editor.h"
#include "catacomb.h"
#include "input.h"

/*
{====================}
{                    }
{ EDITORLOOP         }
{ The editor mode... }
{                    }
{====================}
*/
/*
Procedure EditorLoop;

Label
  cmdover;

const
  samplepics : array[1..12] of string[13] =
    (#128#128#128#128#128#128#128#128#128#128#128#128#128,
     #128#128#129#129#135#128#128#128#171#171#177#128#128,
     #128#129#129#129#129#135#128#171#171#171#171#177#128,
     #128#129#129#129#129#130#128#171#178#179#180#172#128,
     #128#134#129#129#133#132#128#176#171#171#175#174#128,
     #128#128#134#131#132#128#128#128#176#173#174#128#128,
     #128#128#128#128#128#128#128#128#128#128#128#128#128,
     #128#162#128#163#128#164#128#167#128#165#128#166#128,
     #128#128#128#128#128#128#128#128#128#128#128#128#128,
     #128#230#231#232#233#234#235#236#237#238#239#240#128,
     #128#128#128#128#128#128#128#128#128#128#128#128#128,
     #128#128#128#128#128#128#128#128#128#128#128#128#128);

var
  drawtile:integer;
  ltx,lty,ltt,x,y,i:integer;
  dor: dirtype;
  b1,b2: boolean;
{$i-}

{                              }
{                              }
{ LOADLEVEL                    }
{ Loads map LEVEL into memory, }
{ nothing more                 }
{                              }
{                              }

Procedure LoadLevel;
label
  tryopen,fileread;
Var
  filename : string;
  st: string[3];
  x,y,xx,yy,recs, btile : Integer;
  iofile: file;
  tile: byte;
  sm : array [0..4095] of byte;

Begin
  str(level:1,st);
  filename:=concat ('LEVEL',st,'.CAT');

tryopen:

  Assign (iofile,filename);
  Reset (iofile,1);
  If ioresult<>0 then
{create a blank level for the editor}
      Begin
        for x:=0 to 63 do
          for y:=0 to 63 do
            background[y+topoff,x+leftoff]:=blankfloor;
        for x:=0 to 63 do
          Begin
            background[topoff,x]:=131;     {perspective north wall}
            background[topoff+63,x]:=129;  {solid south wall}
            background[x,leftoff]:=130;    {perspective west wall}
            background[x,leftoff+63]:=129; {solid east wall}
          end;
        background [topoff,leftoff]:=133;  {perspective NW corner}
        goto fileread;
      End

    else

  BlockRead (iofile,sm,4096,recs);
  Close (iofile);

  numobj:=0;

  for yy:=0 to 63 do
    for xx:=0 to 63 do
      begin
        tile:=sm[yy*64+xx];

{if tile is an exploding block, change it to a special icon for editor}

        if (tile>=136) and (tile<=145) then
          tile:=tile+35;
        background[yy+topoff,xx+leftoff]:=tile;
      end;

fileread:

  for y:=topoff to 63+topoff do
    for x:=leftoff to 63+leftoff do
      view[y,x]:=background[y,x];
  sx:=33;                  {print the new level number on the right window}
  sy:=1;
  shortnum (level);
  Print (' ');          {in case it went from double to single digit}
  restore;
End;
*/

const uint8_t samplepics[12][13] = {
    { 128,128,128,128,128,128,128,128,128,128,128,128,128 },
    { 128,128,129,129,135,128,128,128,171,171,177,128,128 },
    { 128,129,129,129,129,135,128,171,171,171,171,177,128 },
    { 128,129,129,129,129,130,128,171,178,179,180,172,128 },
    { 128,134,129,129,133,132,128,176,171,171,175,174,128 },
    { 128,128,134,131,132,128,128,128,176,173,174,128,128 },
    { 128,128,128,128,128,128,128,128,128,128,128,128,128 },
    { 128,162,128,163,128,164,128,167,128,165,128,166,128 },
    { 128,128,128,128,128,128,128,128,128,128,128,128,128 },
    { 128,230,231,232,233,234,235,236,237,238,239,240,128 },
    { 128,128,128,128,128,128,128,128,128,128,128,128,128 },
    { 128,128,128,128,128,128,128,128,128,128,128,128,128 }
};

/*
{                              }
{                              }
{ LOADLEVEL                    }
{ Loads map LEVEL into memory, }
{ nothing more                 }
{                              }
{                              }
*/

void LoadLevelEditor( void )
{
#if 0
    char filename[STRSIZE];
    char st[3][STRSIZE];
    int16_t x,y,xx,yy,recs, btile;
    FILE *file;
    uint8_t tile;
    uint8_t sm[4096];

    snprintf( filename, STRSIZE, "LEVEL%d.CAT", level );
    file = fopen( filename, "rb" );

    if (file != NULL)
    {
        for (x=0; x<=63; i++)
        {
            for (y=0; y<=63; y++)
            {
                background[y+TOPOFF][x+LEFTOFF] = BLANKFLOOR;
            }
        }

        for (x=0; x<=63; i++)
        {
            background[TOPOFF][x]=131;     /* {perspective north wall} */
            background[TOPOFF+63][x]=129;  /* {solid south wall} */
            background[x][LEFTOFF]=130;    /* {perspective west wall} */
            background[x][LEFTOFF+63]=129; /* {solid east wall} */
        }
        background[TOPOFF][LEFTOFF]=133;  /* {perspective NW corner} */
    }
    else
    {

    }

tryopen:

  Assign (iofile,filename);
  Reset (iofile,1);
  If ioresult<>0 then
    /* {create a blank level for the editor} */
      Begin
        for x:=0 to 63 do
          for y:=0 to 63 do
            background[y+topoff,x+leftoff]:=blankfloor;
        for x:=0 to 63 do
          Begin
            background[topoff,x]:=131;     {perspective north wall}
            background[topoff+63,x]:=129;  {solid south wall}
            background[x,leftoff]:=130;    {perspective west wall}
            background[x,leftoff+63]:=129; {solid east wall}
          end;
        background [topoff,leftoff]:=133;  {perspective NW corner}
        goto fileread;
      End

    else

  BlockRead (iofile,sm,4096,recs);
    Close (iofile);

    numobj=0;

    for (yy=0; yy<=63; yy++)
    {
        for (xx=0; xx<=63; xx++)
        {
            tile = sm[yy*64+xx];

            /* {if tile is an exploding block, change it to a special icon for editor} */
            if ((tile>=136) && (tile<=145))
                tile = tile+35;

            background[yy+TOPOFF][xx+LEFTOFF] = tile;
        }
    }

fileread:

  for (y=TOPOFF; y<=63+TOPOFF; y++)
    for (x:=LEFTOFF; x<=63+LEFTOFF; x++)
      view[y][x] = background[y][x];

  sx=33;                  /* {print the new level number on the right window} */
  sy=1;
  Print("%d ", level);   /* {in case it went from double to single digit} */
  Restore();
  #endif
}

/*
{            }
{ Save Level }
{            }
*/
/*
Procedure Saveit;
Var
  iofile : file;
  filename : string;
  x,y,recs : Integer;
  tile: byte;
  st: string[3];
  sm : array [0..4095] of byte;
Begin
  CenterWindow (9,1);
  Print ('Saving...');
  For y:=0 to 63 do
    for x:=0 to 63 do
      begin
        tile:=background[y+topoff,x+leftoff] and $00FF;

{if the tile was an exploding block, change back to undetectable}

        if (tile>=171) and (tile<=180) then
          tile:=tile-35;
        sm[y*64+x]:=tile;
      end;
  str(level:1,st);
  filename:=concat ('LEVEL',st,'.CAT');
  Assign (iofile,filename);
  Rewrite (iofile,1);
  BlockWrite (iofile,sm,4096,recs);
  Close (iofile);
  restore;
End;
*/

void SaveLevel( void )
{

}

/*
{              }
{ Select Level }
{              }
*/
/*
function SelectLevel:boolean;
Var
  err:integer;
  lv:string;
Begin
  selectlevel:=false;              {editor won't reload a level if false}
  CenterWindow (16,2);
  Print ('Edit which level](1-99):');
  input (lv,2);
  if lv[1]=chr(27) then               {allow ESC to quit editor mode}
    Begin
      leveldone:=true;
      playdone:=true;
    end;
  val (lv,level,err);
  If level>=1 then
    selectlevel:=true;
  restore;
End;
*/

boolean SelectLevel( void )
{
    boolean result;
    /* int16_t err; */
    char lv[STRSIZE];

    result = False;         /* {editor won't reload a level if false} */
    CenterWindow(16,2);
    Print("Edit which level](1-99):");
 /*   input(lv,2); */
    if (lv[1]==inputmaps[input_back].key)       /* {allow ESC to quit editor mode} */
    {
        leveldone = True;
        playdone = True;
    }
    /* val(lv,level,err); */
    if (level>=1)
        result = True;

    Restore();

    return result;
}

/*
{              }
{ Toggle Block }
{              }
*/
/*
Procedure ToggleBlock;
Var
  x,y,block:integer;
Begin
  x:=originx+topoff;
  y:=originy+leftoff;
  block:=background [y,x];

  If block=blankfloor then
    block:=solidwall
  else
    block:=blankfloor;

  background [y,x]:=block;
  view [y,x]:=block;
end;
*/

void ToggleBlock( void )
{
    int16_t x,y,block;

    x = originx+TOPOFF;
    y = originy+LEFTOFF;
    block = background[y][x];

    if (block==BLANKFLOOR)
        block = SOLIDWALL;
    else
        block = BLANKFLOOR;

    background[y][x] = block;
    view[y][x] = block;
}

/*
{           }
{ Print map }
{           }
*/
/*
Procedure PrintMap;
var
  x,y,block:integer;
  ch: char;
Begin
  writeln (lst);
  Writeln (lst,'CATACOMB level ',level);
  for y:=0 to 65 do
    Begin
      for x:=0 to 65 do
	begin
	  block:=background[topoff-1+y,leftoff-1+x];
	  case block of
	    0..127: ch:=chr(block);	{ASCII}
	    128: ch:=' ';		{floor}
	    129..135: ch:='#';		{walls}
	    171..177: ch:='*';		{exploding}
	    178..180: ch:='!';		{hidden stuff}
	    162: ch:='p';		{potion}
	    163: ch:='s';		{scroll}
	    164: ch:='k';		{key}
	    165: ch:='|';		{door}
	    166: ch:='-';		{door}
	    167: ch:='$';		{treasure}
	    230..238: ch:=chr(ord('0')+block-229); {tokens}
	    else ch:='?';
	  end;
	  write (lst,ch);
      end;
    writeln (lst);
  end;
  writeln (lst,chr(12));
end;
*/

void PrintMap( void )
{
    int16_t x,y,block;
    char tile;
    FILE *file = NULL;
    char filename[STRSIZE];

    snprintf( filename, STRSIZE, "CATACOMB level %d.txt", level );
    file = fopen( filename, "w" );

    if (file != NULL)
    {
        for (y=0; y<=65; y++)
        {
            for (x=0; x<=65; x++)
            {
                block = background[TOPOFF-1+y][LEFTOFF-1+x];

                if (block >= 0 && block <= 127)         /* {ASCII} */
                    tile = block;
                else if (block == 128)                  /* {floor} */
                    tile = ' ';
                else if (block >= 129 && block <= 135)  /* {walls} */
                    tile = '#';
                else if (block >= 171 && block <= 177)  /* {exploding} */
                    tile = '*';
                else if (block >= 178 && block <= 180)  /* {hidden stuff} */
                    tile = '!';
                else if (block == 162)                  /* {potion} */
                    tile = 'p';
                else if (block == 163)                  /* {scroll} */
                    tile = 's';
                else if (block == 164)                  /* {key} */
                    tile = 'k';
                else if (block == 165)                  /* {door} */
                    tile = '|';
                else if (block == 166)                  /* {door} */
                    tile = '-';
                else if (block == 167)                  /* {treasure} */
                    tile = '$';
                else if (block >= 209 && block <= 218)  /* {tokens} */
                    tile = '0'+(block-209);
                else
                    tile = '?';

                fputc( tile, file );
            }

            fputc( '\n', file );
        }

        fclose( file );
    }
}

/*
Begin

  regs.ax:=0;
  intr($33,regs);    {show the mouse cursor}

  DrawWindow (24,0,38,23);  {draw the right side window}
  Print ('  Level]] Map editor]]F4=exit]F7=Load]F8=Save]^P=Print');

  sx:=25;
  leftedge:=sx;
  sy:=10;
  for i:=1 to 12 do
    Print (samplepics[i]+']');

  drawtile:=solidwall;
  ltx:=28;
  lty:=13;
  ltt:=solidwall;
  xormask:=$FFFF;
  charout (ltx,lty,ltt);         {highlight the new block}
  xormask:=0;

  level:=1;
  playdone:=false;

  Repeat
    leveldone:=false;
    originx:=0;
    originy:=0;

    If selectlevel then {let them choose which level to edit}
      loadlevel
    else
      goto cmdover;     {so if they pressed ESC, they can leave}

    repeat
      SimpleRefresh;

      regs.ax:=1;
      intr($33,regs);    {show the mouse cursor}
      waitvbl;           {make sure it gets seen}
      waitvbl;

      Repeat
        regs.ax:=3;
        intr($33,regs);  {mouse status}
      Until keypressed or (regs.bx and 3>0);

      sx:=regs.cx div 16;   {tile on screen mouse is over}
      sy:=regs.dx div 8;

      regs.ax:=2;
      intr($33,regs);    {hide the mouse cursor}

      Checkkeys;       {handles F keys and returns a keypress}

      ch:=chr(0);
      altkey:=false;
      if keypressed then
	Begin
	  ch:=upcase(readkey);
	  if ch=chr(0) then
	    Begin
	      altkey:=true;
	      ch:=readkey;
	    end
	end;

      if (sx<24) and (sy<24) then
{buttons pressed in tile map}
        Begin
          x:=originx+sx;
          y:=originy+sy;
          if (x>=leftoff) and (x<leftoff+64) and
             (y>=topoff) and (y<topoff+64) then
            Begin
              if (regs.bx and 1>0) then

{left button places/deletes a DRAWTILE}

                Begin
                  background[y,x]:=drawtile;
                  view[y,x]:=drawtile;
                end;

              if (regs.bx and 2>0) then  {right button places a BLANKFLOOR}
                Begin
                  background[y,x]:=blankfloor;
                  view[y,x]:=blankfloor;
                end;

	      if (not altkey) and ((ch>='A') and (ch<='Z')
	      or ((ch>='0') and (ch<='9') ) ) then
		Begin
		  if (ch>='0') and (ch<='9') then
		    background[y,x]:=ord(ch)+161   {map numbers are later}
		  else
		    background[y,x]:=ord(ch)+32; {map letters are lowercase}
		  view[y,x]:=background[y,x];
                  regs.ax:=4;
                  regs.cx:=regs.cx+16;
                  intr ($33,regs);        {move the mouse over}
                end;

              if (not altkey) and (ch=' ') then  {space makes a solidwall}
                Begin
                  background[y,x]:=solidwall;
                  view[y,x]:=solidwall;
                  regs.ax:=4;
                  regs.cx:=regs.cx+16;
                  intr ($33,regs);        {move the mouse over}
                end;

            end;
        end;


      x:=sx-24;
      y:=sy-9;
      if  (regs.bx and 1>0) and (x>0) and (x<14) and (y>0) and (y<13)and
       (samplepics[y][x]<>#128)then
{button pressed in samplepics}
        Begin
          charout (ltx,lty,ltt);         {unhighlight the old DRAWTILE}
          drawtile:=ord(samplepics[y][x]);
          ltx:=sx;
          lty:=sy;
          ltt:=drawtile;
          xormask:=$FFFF;
          charout (ltx,lty,ltt);         {highlight the new block}
          xormask:=0;
        end;


      Rd_Keyboard (dir,b1,b2);
      case dir of
	north: if originy>0 then
                originy:=originy-1
              else
		playsound (blockedsnd);
	west: if originx>0
           then
                originx:=originx-1
              else
		playsound(blockedsnd);
	east: if originx<51+leftoff then
                originx:=originx+1
              else
		playsound(blockedsnd);
	south: if originy<51+topoff then
                originy:=originy+1
              else
		playsound(blockedsnd);
      end;


      If keydown[$19] and keydown[$1d] then {control-P}
	PrintMap;

      If keydown[$42] then
	Begin
	  keydown[$42]:=false;
	  SaveIt;
	end;

      If keydown[$41] then
	Begin
	  keydown[$41]:=false;
	  leveldone:=true;        {so SELECTLEVEL will be called}
        end;

cmdover:

    Until leveldone or playdone;
  Until playdone;

End;
*/

void EditorLoop( void )
{
#if 0
    /*
    int16_t drawtile;
    int16_t ltx,lty,ltt,x,y,i;
    dirtype_t dor;
    boolean b1,b2;
    */


  //regs.ax:=0;
  //intr($33,regs);    /* {show the mouse cursor} */

    DrawWindow( 24,0,38,23 );  /* {draw the right side window} */
    Print( '  Level]] Map editor]]F4=exit]F7=Load]F8=Save]^P=Print' );

    sx=25;
    leftedge=sx;
    sy=10;
    for (i=1; i<=12; i++)
    {
        Print( samplepics[i]+']' );
    }

    drawtile=solidwall;
    ltx=28;
    lty=13;
    ltt=solidwall;
    //xormask:=$FFFF;
    CharOut( ltx,lty,ltt );         /* {highlight the new block} */
    //xormask:=0;

    level=1;
    playdone=False;

    while (playdone == false)
    {
        leveldone=False;
        originx=0;
        originy=0;

        if (selectlevel) {  /* {let them choose which level to edit} */
            LoadLevel( todo )
        } else {
            goto cmdover;     /* {so if they pressed ESC, they can leave} */
        }

        while (leveldone == false && playdone == false)
        {
            SimpleRefresh();

      //regs.ax:=1;
      //intr($33,regs);    {show the mouse cursor}
      //waitvbl;           {make sure it gets seen}
    //waitvbl;

      Repeat
        regs.ax:=3;
        intr($33,regs);  {mouse status}
      Until keypressed or (regs.bx and 3>0);

      sx:=regs.cx div 16;   {tile on screen mouse is over}
      sy:=regs.dx div 8;

      regs.ax:=2;
      intr($33,regs);    {hide the mouse cursor}

      Checkkeys;       {handles F keys and returns a keypress}

      ch:=chr(0);
      altkey:=false;
      if keypressed then
	Begin
	  ch:=upcase(readkey);
	  if ch=chr(0) then
	    Begin
	      altkey:=true;
	      ch:=readkey;
	    end
	end;

      if ((sx<24) && (sy<24)) /* {buttons pressed in tile map} */
    {
          x=originx+sx;
          y=originy+sy;
          if (x>=leftoff) && (x<leftoff+64) &&
             (y>=topoff) && (y<topoff+64) then
            Begin
              if (regs.bx && 1>0) then

{left button places/deletes a DRAWTILE}

                Begin
                  background[y,x]:=drawtile;
                  view[y,x]:=drawtile;
                end;

              if (regs.bx && 2>0) then  {right button places a BLANKFLOOR}
                Begin
                  background[y,x]:=blankfloor;
                  view[y,x]:=blankfloor;
                end;

	      if (not altkey) && ((ch>='A') && (ch<='Z')
	      or ((ch>='0') && (ch<='9') ) ) then
		Begin
		  if (ch>='0') && (ch<='9') then
		    background[y,x]:=ord(ch)+161   {map numbers are later}
		  else
		    background[y,x]:=ord(ch)+32; {map letters are lowercase}
		  view[y,x]:=background[y,x];
                  regs.ax:=4;
                  regs.cx:=regs.cx+16;
                  intr ($33,regs);        {move the mouse over}
                end;

              if (not altkey) && (ch=' ') then  {space makes a solidwall}
                Begin
                  background[y,x]:=solidwall;
                  view[y,x]:=solidwall;
                  regs.ax:=4;
                  regs.cx:=regs.cx+16;
                  intr ($33,regs);        {move the mouse over}
                end;

            end;
        end;


      x:=sx-24;
      y:=sy-9;
      if  (regs.bx && 1>0) && (x>0) && (x<14) && (y>0) && (y<13) &&
       (samplepics[y][x]<>#128)then
{button pressed in samplepics}
    {
          charout (ltx,lty,ltt);         {unhighlight the old DRAWTILE}
          drawtile:=ord(samplepics[y][x]);
          ltx:=sx;
          lty:=sy;
          ltt:=drawtile;
          xormask:=$FFFF;
          charout (ltx,lty,ltt);         {highlight the new block}
          xormask:=0;
        }


      Rd_Keyboard (dir,b1,b2);
      case dir of
	north: if originy>0 then
                originy:=originy-1
              else
		playsound (blockedsnd);
	west: if originx>0
           then
                originx:=originx-1
              else
		playsound(blockedsnd);
	east: if originx<51+leftoff then
                originx:=originx+1
              else
		playsound(blockedsnd);
	south: if originy<51+topoff then
                originy:=originy+1
              else
		playsound(blockedsnd);
      end;


      If keydown[$19] and keydown[$1d] then {control-P}
	PrintMap;

      If keydown[$42] then
	Begin
	  keydown[$42]:=false;
	  SaveIt;
	end;

      If keydown[$41] then
	Begin
	  keydown[$41]:=false;
	  leveldone:=true;        {so SELECTLEVEL will be called}
        end;

cmdover:

    //Until leveldone or playdone;
  //Until playdone;

End;
#endif
}
