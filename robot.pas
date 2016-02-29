{$U-,C-}
program robot;

{
|  ROBOT
|
|  Abs: Program to play "Robot" on Tiki-100.
|  His: 19 may 86  2.8  [gh]  Fixed small bug in getline.
|       25 jan 86  2.7  [gh]  Added HSC-file merge command.
|        7 sep 85  2.6  [gh]  Check for write protected disk.
|       18 aug 85  2.5  [gh]  Fixed fast Robot bug.
|       12 aug 85  2.4  [gh]  Fixed refresh and colour bugs.
|        9 aug 85  2.3  [gh]  Willy's suggestions
|       27 jul 85  2.2  [gh]  Fixed bug with <CR> response on name.
|       18 jul 85  2.1  [gh]  Changed crediting of expedience.
|       16 jul 85  2.0  [gh]  Super, duper feature-laden version.
|       12 jul 85  1.3  [gh]  Fixed bug in score colour coding. Added sound.
|        4 jul 85  1.2  [gh]  Two levels of difficulty.
|       18 jun 85  1.1  [gh]  Fixed bug in line editor character deletion.
|       17 jun 85  1.0  [gh]  Started.
|  Des: * Change the string MAGICH when incompatible score file is created.
|       * '?' in response to monitor will return  ROBOT to its pristine state.
|         I.e. set the configuration to '2',  which  causes  questions  to  be
|         asked on startup.
|       * '?' in response to standard game will make it fast, and save highsc.
|       * A lot of  necessary  parameters  are extracted from font definition,
|         and  major  parts  of  the  program  should  work  unaltered on font
|         matrices with different symbols.  The embedded font dependencies are
|         tied to the dimensioning of the board.  This is isolated to a number
|         of constants, which need to be recomputed on font change.  Character
|         enumeration may not be changed.
|       * Some dirty tricks are performed.  In particular,  inks  outside  the
|         valid range are used where this bums the code.
|  Imp: Font is compiled into code as a TURBO  "typed constant"  matrix.  This
|       matrix is genererated from ROBOT.FNT with the program MAKINC.COM.
|  Env: Tiki-100, TIKO, Libg and Turbo-Pascal.
|  Bug: Present scroring strategy will create overflow  at  level 140.  It  is
|       assumed that nobody will *ever* get that far.
}

{$I A:STDCONS.INC}
{$I ROBOTN.INC   }

const
   VERS    =  '2.8';
   FILNAM  =  'ROBOT.HSC';
   MAGICH  =  '09August1985';
   DECXX   =   41; { 25 Line index: 0..DECXX.       }
   INCYY   =   26; { 19 No of robots in one column  }
   DECYY   =   25; { 18 INCYY  -  1                 }
   BSIZE   = 1091; {493 (DECXX + 1) * INCYY - 1     }
   MAXROBS =  288; { (DECXX-1) * (DECYY-1) * 0.3 }
   PRLIN   =  244; { Prompt line.                }
   IROBX   =   60;
   PROBX   =  364;
   WRITMOD =    0; { Overwrite mode              }
   BLACK   =    0;
   REDCL   =    1;
   BLUEC   =    2;
   GREEN   =    3;
   (* GHBR: array[1..8] of integer = (-20,  -1,  18,
                                      -19,       19,
                                      -18,   1,  20); *)
   NEIGHBR: array[1..8] of integer = (-27,  -1,  25,
                                      -26,       26,
                                      -25,   1,  27);
type
   state = (ZERO, HEAP, ROBO, HERO, HACK ,ZZAP, EMTY, EDGE);
                                            { All below "EMTY" is }
   board = record case boolean of           {  stuff that may  be }
      TRUE:  (t: array[0..BSIZE] of state); {  displayed.         }
      FALSE: (f: array[0..DECXX,0..DECYY] of state)
   end;

   namstring = string[12];

   entrytype = record
      fname: namstring;
      quitt: boolean;
      level: integer;
      score: real;
   end; { honortype }
   honorroll = array[1..20] of entrytype;

   moves = (AMOVE, ACALE, AHELP, ACHET, ACHIK, ACHLV, ABRYT);

var
   configbyte: byte absolute $0103; { 1FC9 }
   soundobyte: byte absolute $0104; { 1FCA }
   uniquebyte: byte absolute $0105;
   flushtbyte: byte absolute $0106;
   stdgambyte: byte absolute $0107; { 1FCB }
   ohp, orb, fontptr, irobs, level: integer;
   oii, redscore, grnscore: real;
   hroll: honorroll;
   robots: board;
   plcxx, plcyy: array[0..BSIZE] of integer;
   herox, heroy, bboxh, lheit, bboxw, lwidd: byte;
   rollshown, newscore: boolean;


{$I A:LIBG.INC - Standard grafikk-bibliotek        }
{$I ROBOT1.INC - Robot font matrix, sound effects. }
{$I ROBOT2.INC - Initialisation.                   }
{$I ROBOT3.INC - Honor-roll, file I/O, install.    }
{$I ROBOT4.INC - Text and graphics, move generator }
{$I ROBOT5.INC - Move handling                     }

begin
   if wprot then begin writeln(PRTWRN); halt; end;
   ginit;
   write(CUROFF);
   wrtm(WRITMOD);
   randomize;
   initroll;
   main;
   if newscore then writroll;
   write(NORMAL);
   clrscr;
   ink( 1); defink(  0,  0,100);
   ink( 2); defink(100,100,  0);
   ink(15); defink(100,100,100); { Works in all modes. }
end.
