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
   ohp, orb, fontptr, irobs, level: integer;
   oii, redscore, grnscore: real;
   hroll: honorroll;
   robots: board;
   plcxx, plcyy: array[0..BSIZE] of integer;
   herox, heroy, bboxh, lheit, bboxw, lwidd: byte;
   rollshown, newscore: boolean;


{ Initialization ------------------------------------------------------------}


function lebensraum(st: state): integer;
{
| Abs: Looks for some uncluttered space to put object.
| Ret: Position to put object (exploited in computing hero position).
| Sef: Object is put into robot array.
}
var
   jj: integer;
begin { lebensraum }
   repeat jj  := random(BSIZE) until robots.t[jj] = EMTY;
   robots.t[jj] := st;
   lebensraum := jj;
end;  { lebensraum }


procedure placeinitial(no: integer);
{
| Abs: Initial robot placement.
| Sef: * robots array initialized.
|      * herox, heroy computed.
}
var
   ii, jj: integer;
begin { placeinitial }
   for ii := 0 to BSIZE do robots.t[ii] := EMTY;
   jj := INCYY * DECXX;
   for ii := 0 to DECYY do
   begin
      robots.t[ii]    := EDGE;
      robots.t[ii+jj] := EDGE;
   end;
   jj := 0;
   for ii := 0 to DECXX do
   begin
      robots.t[jj]       := EDGE;
      robots.t[jj+DECYY] := EDGE;
      jj := jj + INCYY;
   end;
   for ii := 1 to no do jj := lebensraum(ROBO);
   jj := lebensraum(HERO);
   herox := jj div INCYY;
   heroy := jj mod INCYY;
end;  { placeinitial }


procedure computinitxy;
var
  ii: integer;
begin
   for ii := 0 to BSIZE do
   begin
      plcxx[ii] := (ii div INCYY - 1)*lwidd+1;
      plcyy[ii] := (ii mod INCYY - 1)*lheit+1;
   end;
end;

(* ENDINC *)


{ Compute delta -------------------------------------------------------------}


function computdelta(movpl: integer): integer;
{
| Des: Expedience is  rewarded.  The formula  used rewards clearing a level
|      in 11 moves with 10 initial robots, and in 50 moves with 244 initial
|      robots.  A linear function from  zero to normal increment is used to
|      compute the reward for expedience, so that clearing it halway before
|      the critical point yields half a reward.  An example:  Say  we faced
|      100 initial robots on previous level.  The normal increment would be
|      100/2 = 50 robots, and this sets the critical point at 100/6+10 = 26
|      moves.  Say we used 15 moves; our increment becomes  (50 * 15) / 26,
|      which gives an increment of 28 instead.
| Var: cp = critical point.
|      da = delta (normal increment)
}
var
   cp, da: integer;
begin
   cp := (irobs div 6) + 10;
   da :=  irobs div 2;
   if movpl < cp then computdelta := (da * movpl) div cp
   else computdelta := da;
end;


{ Text string output --------------------------------------------------------}


procedure message(no: byte);
{
| Des: Wipe out 6 * slength.
}
begin
   apos(0,PRLIN); ink(BLACK); block(180,9);
   apos(0,PRLIN); font(0);    ink(REDCL);
   case no of
      0: txt(IBMWON);
      1: txt(YOUWON);
      2: txt(NEWBRD);
      3: txt(CONFIG);
      4: txt('.');
      5: txt(RETIRE);
      6: txt(NICDAY);
   end; { case }
   if no <= 2 then
   begin
      if soundobyte = 0 then delay(1200)
   end
   else if no <= 4 then delay(900);
   font(fontptr);
end;


procedure dohelp;
var
   mm: moves;
begin
   apos(10, 48); ink(GREEN); txt(TXTMOV);
   apos(10, 63); ink(GREEN); txt(KEYRST);    ink(REDCL); txt(TXTRST);
   apos(10, 73); ink(GREEN); txt('F1:    '); ink(REDCL); txt(TXTSTP);
   apos(10, 83); ink(GREEN); txt('F2:    '); ink(REDCL); txt(TXTOTP);
   apos(10, 93); ink(GREEN); txt('F3:    '); ink(REDCL); txt(TXTZAP);
   apos(10,103); ink(GREEN); txt('F10:   '); ink(REDCL); txt(TXTRFR);
   apos(10,113); ink(GREEN); txt('F11:   '); ink(REDCL); txt(TXTCHI);
   apos(10,123); ink(GREEN); txt('F12:   '); ink(REDCL); txt(TXTSTD);
   apos(10,133); ink(GREEN); txt(KEYPNT);    ink(REDCL); txt(TXTPNT);
   apos(10,143); ink(GREEN); txt(KEYSND);    ink(REDCL); txt(TXTSND);
   apos(10,153); ink(GREEN); txt(KEYCHT);    ink(REDCL); txt(TXTCHT);
   apos(10,163); ink(GREEN); txt(KEYBRK);    ink(REDCL); txt(TXTBRK);
   apos(10,173); ink(GREEN); txt(KEYHLP);    ink(REDCL); txt(TXTHLP);
end;


procedure showtport(carry, tport: integer);
var
   cc: char;
   ss: string[5];
begin
   apos(133,PRLIN); ink(BLACK); block(24,9);
   apos(133,PRLIN); font(0);    ink(GREEN);
   cc := chr(tport+ord('0'));   ch(cc);
   str(carry:1,ss);        txt('+'+ss);
   font(fontptr);
end;


procedure showrob(xx,robs: integer);
var
   ss: string[3];
begin
   apos(xx,PRLIN); ink(BLACK); block(24,9);
   apos(xx,PRLIN); ink(REDCL); str(robs:1,ss); txt(ss+')');
end;


procedure showlevel(carry, tport, pprob: integer);
{
| Abs: Displays level, irobs, pprob, carry, tport.
}
var
   ss: string[5];
begin
   apos(2,PRLIN); ink(BLACK); block(42,9);
   apos(2,PRLIN); font(0);    ink(BLUEC);
   str(level:1,ss); txt(ss);  ink(REDCL);
   apos(IROBX-24,PRLIN);
   str(irobs:3,ss); txt(ss+'(');
   str(irobs:1,ss); txt(ss+')');
   apos(PROBX-24,PRLIN);
   str(pprob:3,ss); txt(ss+'(');
   txt('0'+')');
   showtport(carry,tport);
end;


procedure showscore(ii: real; movpl: integer);
var
   hp, rb: integer;
   col: byte;
   ss: string[14];
begin
   font(0);
   hp := 1;
   for rb := 0 to BSIZE do if robots.t[rb] = HEAP then hp := hp + 1;
   hp := hp shr 1;
   rb := irobs + computdelta(movpl);
   if rb > MAXROBS then rb := MAXROBS;
   if ii <> oii then
   begin
      if   level  = 1        then col := GREEN
      else if ii >= redscore then col := REDCL
      else if ii <= grnscore then col := GREEN
      else col := BLUEC;
      str(ii:14:0,ss);
      apos(396,PRLIN); ink(BLACK); block(84,9);
      apos(396,PRLIN); ink(col);   txt(ss);
      oii := ii;
   end;
   if movpl > 0 then
   begin
      if hp <> ohp then begin showrob(PROBX,hp); ohp := hp; end;
      if rb <> orb then begin showrob(IROBX,rb); orb := rb; end;
   end;
   font(fontptr);
end;


{ Graphics output -----------------------------------------------------------}


procedure displayfield;
{
| Abs: Redisplay playing field.
}
var
   ii, cc, xx, yy: integer;
begin
   for ii := 0 to BSIZE do
   begin
      if robots.t[ii] < EMTY then
      begin
         cc := ord(robots.t[ii]);
         ink(cc); apos(plcxx[ii],plcyy[ii]); ch(chr(cc));
      end;
   end;
end;


procedure background;
begin
   clrscr; ink(REDCL); apos(0,0);
   aline(0,lheit*(INCYY-2)); aline(lwidd*(DECXX-1)+1,lheit*(INCYY-2));
   aline(lwidd*(DECXX-1)+1,0); aline(0,0);
   displayfield;
end;


procedure clearbox(ii: integer);
begin
   apos(plcxx[ii],plcyy[ii]); ink(BLACK); block(bboxw,bboxh);
end;


{ Move generation utilities -------------------------------------------------}


function safespot(ii: integer): boolean;
{
| Abs: Check if the spot will be safe to stay at.
| Ret: TRUE if it is.
}
var
  jj: integer;
  bb: boolean;
begin
   bb := (robots.t[ii] = EMTY);
   for jj := 1 to 8 do bb := bb and (robots.t[ii+NEIGHBR[jj]] <> ROBO);
   safespot := bb;
end;


procedure dojump(var herox, heroy: byte; freebies: byte);
{
| Imp: A  rather  sticky  situation may arise on level 8 and above if user has
|      the right to a free teleport,  but  no legal  free teleport exists.  In
|      such cases, a random search is performed 1000 times  (the  figure  1000
|      was found through tests),  then a systematic search is  performed,  and
|      if this  yields  no result,  we cheat:  killing off the required no. of
|      robots to make a free teleport legal.
}
var
   ii, jj: integer;
begin
   if freebies > 0 then
   begin
      jj := 0;
      repeat
         ii := random(BSIZE);
         jj := succ(jj);
      until safespot(ii) or (jj = 1000);
      if not safespot(ii) then { search systematic }
      begin
         ii := INCYY;
         repeat ii := succ(ii) until safespot(ii) or (ii = BSIZE);
         if not safespot(ii) then { cheat }
         begin
            repeat ii := random(BSIZE) until robots.t[ii] = EMTY;
            for jj := 1 to 8 do if robots.t[ii+NEIGHBR[jj]] = ROBO then
               robots.t[ii+NEIGHBR[jj]] := EMTY;
         end; { cheat }
      end; { search systematic }
   end
   else
   begin
      repeat ii := random(BSIZE) until robots.t[ii] = EMTY;
   end;
   herox := ii div INCYY;
   heroy := ii mod INCYY;
end;


(* ENDINC *)

{ Playing the game ----------------------------------------------------------}


function domove(var freb, bies, prob, nrob, movp: integer; score: real): moves;
{
| Par: freb + bies is the total number of "free" teleports.
|      freb = carry (teleports carried from last round)
|      bies = tport (teleports gained this round)
}
var
   ii: integer;
   cc: char;
   oldhx, oldhy: byte;
   justteleport: boolean;
   dm: moves;
   newstate: state;

   procedure restore;
   begin
      herox := oldhx;
      heroy := oldhy;
      write(BELL);
      dm := AHELP;
   end;

   procedure decrfree;
   begin
      if freb > 0 then freb := freb - 1 else bies := bies - 1;
      showtport(freb,bies);
   end;

   procedure tglsound;
   begin
      apos(0,PRLIN); ink(BLACK); block(180,9);
      apos(0,PRLIN); font(0);    ink(REDCL);
      txt(TGLSND);
      if soundobyte = 0 then
      begin
         txt(TGLONN);
         soundobyte := 1;
      end
      else
      begin
         txt(TGLOFF);
         soundobyte := 0;
      end;
      delay(1000);
      apos(0,PRLIN); ink(BLACK); block(180,9);
   end;

   procedure badmove;
   begin
      if stdgambyte = 0 then cc := CURHOM else dm := AHELP;
      write(BELL);
   end;

   procedure safeport;
   begin
      if (freb + bies) > 0 then
      begin
         teleprt;
         dojump(herox,heroy,freb+bies);
         decrfree;
      end
      else badmove;
   end;

   procedure pancport;
   begin
      if (level >= 5) or ((freb + bies) = 0) then
      begin
         teleprt;
         dojump(herox,heroy,0);
         justteleport := TRUE;
      end
      else badmove;
   end;

   procedure superzap;
   var
      jj, ii: integer;
   begin
      if (freb + bies) > 0 then
      begin
         sdriver;
         ii := herox*INCYY+heroy;
         for jj := 1 to 8 do
         begin
            if robots.t[ii+NEIGHBR[jj]] < EDGE then
            begin
               robots.t[ii+NEIGHBR[jj]] := EMTY;
               clearbox(ii+NEIGHBR[jj]);
            end;
         end;
         decrfree;
      end
      else badmove;
   end;

   function suicide: boolean;
   {
   | Ret: TRUE if move is suicide, else FALSE -- unless we just teleported,
   |      or it's not a move, in which case will always return FALSE.
   }
   var
      jj, ii: integer;
      su: boolean;
   begin { suicide }
      if justteleport or (dm <> AMOVE) then
         begin suicide := FALSE; exit; end;
      ii := herox*INCYY+heroy;
      if not (robots.t[ii] in [EMTY,HERO,ZZAP]) then
         begin suicide := TRUE;  exit; end;
      su := FALSE;
      if stdgambyte <> 0 then
         for jj := 1 to 8 do if robots.t[ii+NEIGHBR[jj]]=ROBO then su := TRUE;
      suicide := su;
   end;  { suicide }

   function getcommand(seconds: integer): char;
   {
   | Abs: Get command from keyboard, or time out.
   | Ret: Key pressed, or "sitting duck" code if timed out.
   }
   var
      cc: char;
      bb, bo, noalarm: boolean;
      lastsecnd, timelimit: integer;
   begin
      fastcl := 0;
      cc := CURHOM;
      lastsecnd := (seconds-4)*25;
      timelimit := seconds*25;
      noalarm := TRUE;
      repeat
         bo := keypressed;
         bb := bo;
         while bb do
         begin
            read(kbd,cc);
            bb := keypressed and (flushtbyte <> 0);
         end;
         if (fastcl > lastsecnd) and noalarm then
         begin
            write(BELL);
            noalarm := FALSE;
         end;
      until (fastcl > timelimit) or bo;
      getcommand   := cc;
   end;

   procedure dorefres;
   begin
      orb := 0;
      ohp := 0;
      oii := -1.0;
      background;
      showlevel(freb,bies,prob);
      oii := -1.0;
      showscore(score+(irobs-nrob)*prob,movp);
      dm := AHELP;
   end;

   procedure dohoooot;
   begin
      sound(0,0,0);
      sound(1,EE1,15);
      read(kbd,cc);
      sound(0,0,0);
      if stdgambyte <> 0 then dm := AHELP;
   end;

begin { domove }
   dm := AMOVE;
   oldhx := herox;
   oldhy := heroy;
   justteleport := FALSE;
   if stdgambyte = 0 then cc := getcommand(10+random(15)) else
   repeat read(kbd,cc) until (not keypressed) or (flushtbyte = 0);
   cc := chr(ord(cc) AND $7F);

   case cc of
      CURLFT: herox := herox - 1;
      CURRGT: herox := herox + 1;
      CURUP:  heroy := heroy - 1;
      CURDN:  heroy := heroy + 1;
      ROLLUP: begin herox := herox - 1; heroy := heroy - 1; end;
      ROLLDN: begin herox := herox + 1; heroy := heroy - 1; end;
      ICRLFT: begin herox := herox - 1; heroy := heroy + 1; end;
      TAB:    begin herox := herox + 1; heroy := heroy + 1; end;
      CURHOM: begin end; { Don't move }
      F1:     safeport;
      F2:     pancport;
      F3:     superzap;
      F10:    dorefres;
      F11:                                                 dm:=ACHIK;
      F12:                                                 dm:=ACALE;
      F17:    dohoooot;
      HJELP:  begin dohelp;             displayfield;      dm:=AHELP; end;
      SLETT:  begin tglsound;   showlevel(freb,bies,prob); dm:=AHELP; end;
      ANGRE:                                               dm:=ACHET;
      BRYT:                                                dm:=ABRYT;
      else    badmove;
   end; { case }
   ii := herox*INCYY+heroy;
   if (stdgambyte=0) and (robots.f[herox,heroy]=ROBO) then
   begin
      zapsond;
      newstate := ZZAP;
      clearbox(ii);
   end
   else
   begin
      if suicide then restore;
      newstate := HERO;
   end;
   if (herox <> oldhx) or (heroy <> oldhy) then
   begin
      robots.f[oldhx,oldhy] := EMTY;
      robots.f[herox,heroy] := newstate;
      clearbox(oldhx*INCYY+oldhy);
      ink(ord(newstate)); apos(plcxx[ii],plcyy[ii]); ch(chr(ord(newstate)));
      oldhx := herox; oldhy := heroy;
   end;
   domove := dm;
end;  { domove }


procedure moverobots(var alive, rleft: boolean; var nrobs: integer);
var
   xx, xx2, yy, yy2: integer;
   newpos: board;
begin { moverobots }
   for yy := 0 to BSIZE do
      if (robots.t[yy] <> ROBO) then newpos.t[yy] := robots.t[yy]
      else newpos.t[yy] := EMTY;
   for xx := 0 to DECXX do for yy := 0 to DECYY do
   begin
      if robots.f[xx,yy] = ROBO then
      begin
         if      xx < herox then xx2 := xx+1
         else if xx > herox then xx2 := xx-1 else xx2 := xx;
         if      yy < heroy then yy2 := yy+1
         else if yy > heroy then yy2 := yy-1 else yy2 := yy;
         if       newpos.f[xx2,yy2] = EMTY  then newpos.f[xx2,yy2] := ROBO
         else if (newpos.f[xx2,yy2] = ZZAP)
              or (newpos.f[xx2,yy2] = HACK) then begin end
         else if  newpos.f[xx2,yy2] = HERO  then newpos.f[xx2,yy2] := ZZAP
         else if (newpos.f[xx2,yy2] = HEAP)
              and (soundobyte = 1)          then newpos.f[xx2,yy2] := HACK
         else newpos.f[xx2,yy2] := HEAP;
      end;
   end;
   for xx := 0 to BSIZE do if robots.t[xx] <> newpos.t[xx] then
   begin
      clearbox(xx);
      if newpos.t[xx] < EMTY then
      begin
         if (newpos.t[xx] = HEAP) or (newpos.t[xx] = HACK) then
         begin
            collide;
            newpos.t[xx] := HEAP;
         end
         else if newpos.t[xx] = ZZAP then zapsond;
         yy := ord(newpos.t[xx]);
         ink(yy); apos(plcxx[xx],plcyy[xx]); ch(chr(yy));
      end;
   end;
   nrobs :=     0; { No of robots remaining.         }
   alive := FALSE; { A pessimistic assumption.       }
   rleft := FALSE; { But at the end of the tunnel... }
   for xx := 0 to BSIZE do
   begin
      robots.t[xx] := newpos.t[xx];
      if robots.t[xx] = ROBO then
      begin
         rleft := TRUE;
         nrobs := nrobs + 1;
      end
      else if robots.t[xx] = HERO then alive := TRUE;
   end;
end;  { moverobots }


procedure main;
{
| Des: * The initial irobs of 7 is not pulled out of a hat.
|      * The method for generating carry batteries is deliberately obscure, to
|        discourage compulsive patchers from cheating.
| Var: mcary = No of teleports given at start of round, minus 1.
|      carry = Teleports carried from last round
|      tport = New teleports given this round (mcary plus 1).
|      pprob = Points per robot.
}
var
   cc: char;
   ii, oldmode, nrobs, mcary, carry, tport, pprob, movpl: integer;
   lm: moves;
   alive, rleft: boolean;
   score: real;

begin { main }
   clrscr;
   oldmode := gmode(2);
   computinitxy;
   newscore := FALSE;

   repeat       { one session }
      alive := TRUE;
      irobs := 7;
      ohp   := 0;
      orb   := 0;
      score := 0.0;
      mcary := 0;
      carry := 0;
      pprob := 1;
      level := 1;
      movpl := 100;
      repeat    { one game    }
         oii   := -1.0;
         rleft := TRUE;
         irobs := irobs + computdelta(movpl);
         if irobs > MAXROBS then irobs := MAXROBS;
         nrobs := irobs;
         placeinitial(irobs);
         background;
         tport := mcary + 1;
         showlevel(carry,tport,pprob);
         showscore(score,0);
         movpl := 0;
         while keypressed do read(kbd,cc); { Flush typeahead }
         repeat { one level   }
            lm := domove(carry,tport,pprob,nrobs,movpl,score);
            if lm = AMOVE then
            begin
               movpl := succ(movpl);
               moverobots(alive,rleft,nrobs);
               showscore(score+(irobs-nrobs)*pprob,movpl);
            end
            else
            while (lm=ACALE) and alive and rleft do moverobots(alive,rleft,nrobs);
         until (lm = ACHET) or (lm = ACHIK) or (lm = ACHLV)
            or (lm = ABRYT) or (not (alive and rleft));

         if (lm <> ACHET) and (lm <> ACHLV) and (lm <> ABRYT) then
         begin
            score := score + (irobs - nrobs) * pprob;
            if not alive then score := score * 0.75;
            showscore(score,movpl);
         end
         else score := 0.0;
         if      lm = ACHET then message(2)
         else if lm = ACHLV then message(3)
         else if lm = ACHIK then message(5)
         else if lm = ABRYT then message(6)
         else message(ord(alive));
         if (score > 0.0) and (soundobyte <> 0) then
         begin
            if      not alive        then loselyd
            else if score > redscore then fanfare
            else if score > grnscore then trudelt
            else brasvar;
         end;

         pprob := 1;
         for ii := 0 to BSIZE do if robots.t[ii] = HEAP then pprob:=pprob+1;
         pprob := pprob shr 1;
         if cc = BRYT then lm := ABRYT;
         level := succ(level);
         carry := carry + tport;
         if mcary < carry then
         begin
            if carry <= 2 then
            begin
               if stdgambyte = 0 then mcary := 1 else mcary := carry;
            end
            else mcary := 2;
         end;
      until (lm = ACHET) or (lm = ACHIK) or (lm = ACHLV) or (lm = ABRYT) or (not alive);
      { end of one game }
      rollshown := FALSE;
   until lm = ABRYT;

   ii := gmode(oldmode);
end;  { Main }



begin
   randomize;
   main;
end.
