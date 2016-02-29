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
      F13:    begin mergefiles;                            dm:=ACHET; end;
      F16:    begin configur(TRUE);                        dm:=ACHLV; end;
      F17:    dohoooot;
      HJELP:  begin dohelp;             displayfield;      dm:=AHELP; end;
      UTVID:  begin dm := showroll(11); displayfield;      dm:=AHELP; end;
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
   initink;
   initfont(myfont);
   computinitxy;
   newscore := FALSE;
   if configbyte >= 2 then configur(FALSE);

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
      if (score > hroll[10+stdgambyte].score) and (level > 2) then
         lm := gnamroll(lm,score,level-1);
   until lm = ABRYT;
   if not rollshown then lm := showroll(11);

   ii := gmode(oldmode);
end;  { Main }


