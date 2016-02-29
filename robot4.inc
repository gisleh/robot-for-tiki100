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
   intoshow(21,193);
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
   mm := exitshow(21,189);
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
