{ Honor-roll, File I/O ------------------------------------------------------}

function cptcheck(var croll: honorroll): integer;
type
   trick = record case boolean of
      TRUE:  (t: real);
      FALSE: (f: array[0..2] of integer);
   end;
var
   ii, jj, cs: integer;
   hack: trick;
begin
   cs := 0;
   for ii := 1 to 20 do with croll[ii] do
   begin
      cs := cs xor level;
      hack.t := score;
      for jj := 0 to  2 do cs := cs xor hack.f[jj];
      for jj := 0 to 10 do cs := cs xor ord(fname[jj]);
   end;
   cptcheck := cs;
end;


function cabbageroll(var lroll: honorroll): boolean;
{
| Ret: TRUE if OK score file, FALSE if anything fishy.
}
var
   ii: integer;
   okfil: boolean;
   entr: entrytype;
   honor: file of entrytype;
begin { cabbageroll }
   assign(honor,FILNAM);
   {$I-} reset(honor); {$I+}
   okfil := ioresult = 0;
   if okfil then
   begin
      read(honor,entr);
      if entr.fname = MAGICH then
      begin
         for ii := 1 to 20 do read(honor, lroll[ii]);
         okfil  := cptcheck(lroll) = round(entr.score);
      end
      else okfil := FALSE;
      close(honor);
   end;
   cabbageroll := okfil;
end;  { cabbageroll }


procedure initroll;
var
   ii, tt: integer;
begin { initroll }
   if cabbageroll(hroll) then
   begin
      if stdgambyte = 1 then
      begin
         stdgambyte := 0;
         tt :=  0;                           { No bogus records.         }
      end else tt := 10;                     { Fast robot records bogus. }
   end else tt := 20;                        { All records bogus.        }
   for ii := 1 to tt do with hroll[ii] do    { Bogify it.                }
   begin
      fname := 'Bogus';
      quitt := TRUE;
      level := 0;
      score := 0.0;
   end;
   redscore := hroll[ 1+stdgambyte].score;
   grnscore := hroll[10+stdgambyte].score;
end; { initroll }


procedure writroll;
var
   entr: entrytype;
   honor: file of entrytype;
   ii: integer;
begin
   assign(honor,FILNAM);
   rewrite(honor);
   with entr do
   begin
      fname := MAGICH;
      level := 0;
      score := cptcheck(hroll);
   end;
   write(honor,entr);
   for ii := 1 to 20 do write(honor,hroll[ii]);
   close(honor);
end;


procedure commshow(wid, top, bot, col: byte);
{
| Abs: Paint a block of colour col from pixel top or above, and down to  pixel
|      bot or below.
| Imp: Size of block is rounded to be a multiple  of robot dimensions.
}
var
   dd: byte;
begin
   dd := (top div lheit) * lheit;
   ink(col);
   apos(1,dd);
   block(lwidd * wid, (((bot-1) div lheit) + 1) * lheit - dd);
end;


procedure intoshow(xx, yy: integer);
{
| Par: xx = Width of area to clear expressed in units of 1 robot-width.
|      yy = How far down to clear, expressed in pixels.
}
begin
   commshow(xx,29,yy,BLACK); (* REDCL *)
   font(0);
   ink(BLUEC);
   apos(10,30);  txt('ROBOT Ver. '+VERS+' ');
   if stdgambyte = 0 then ch('R') else ch('S');
   txt(' '+CRIGHT+' 1986 Gisle Hannemyr.');
end;


function exitshow(xx, yy: integer): moves;
var
   cc: char;
begin
   apos(13,yy); ink(BLUEC);
   txt(PRSKEY); read(kbd,cc);
   if cc = BRYT then exitshow := ABRYT else exitshow := AHELP;
   font(fontptr);
   commshow(xx,29,yy+9,BLACK);
end;


function updtroll(var ne: entrytype; dounique: boolean): byte;
{
| Imp: Tries to avoid that the same person  appear  twice  on  the  list.  The
|      algorithm  checks  for  duplicate  strings of characters after  folding
|      case.  E.,g: "trinelise2" are identical to "Trine Lise".
| Var: oh = Set TRUE first time we insert something.  Is there  to ensure that
|      oldest highscore remain when last score is a tie.
| Ret: Which entry is new, or 0 if none.
}
label
   99;
var
   ii: integer;
   te,oe: entrytype;
   oh: boolean;
   place: byte;

   procedure squeeze(var a: namstring);
   var
      ii,jj: byte;
   begin
      jj := 0;
      for ii := 1 to length(a) do
      begin
         if a[ii] > 'A' then
         begin
            jj := jj + 1;
            if a[ii] > 'a' then a[jj] := chr(ord(a[ii])-32)
            else a[jj] := a[ii];
         end;
      end;
      a[0] := chr(jj);
   end;

   function samestring(a, b: namstring): boolean;
   begin
      if dounique then
      begin
         squeeze(a);
         squeeze(b);
         samestring := a = b;
      end else samestring := FALSE;
   end;

begin { updtroll }
   place := 0;
   updtroll := 0;
   oh := FALSE;
   oe := ne;
   for ii := 1+stdgambyte to 10+stdgambyte do
   begin
      if samestring(oe.fname,hroll[ii].fname) then
      begin
         if (oe.score <= hroll[ii].score) and (not oh) then exit;
         hroll[ii] := ne;
         if not oh then place := ii;
         goto 99;
      end;

      if (ne.score > hroll[ii].score) or
         (ne.score = hroll[ii].score) and oh then
      begin
         te := hroll[ii];
         hroll[ii] := ne;
         ne := te;
         if not oh then
         begin
            oh := TRUE;
            place := ii;
         end;
      end;

   end;
99:
   updtroll := place - stdgambyte;
   redscore := hroll[ 1+stdgambyte].score;
   grnscore := hroll[10+stdgambyte].score;
   newscore := TRUE;
end;  { updtroll }


function springroll(ne: byte; palett: boolean): integer;
{
| Des: Caller must establish context (e.g. by calling intoshow).
| Ret: How far down the page we've got.
}
var
   ii,yy: integer;
   ss: string[14];
begin
   intoshow(40,163);
   ink(REDCL);
   yy := 53;
   for ii := 1 to 10 do with hroll[ii+stdgambyte] do
   begin
      if score > 0 then
      begin
         if palett then
         begin
            if ii = 2 then ink(BLUEC) else if ii = 10 then ink(GREEN);
         end;
         yy := 10*ii+43;
         apos(20,yy);
         str(ii:2,ss); txt(ss+') '+fname);
         apos(122,yy);
         if quitt then       txt(RESIGN)
         else                txt(SERVIC);
         str(level:2,ss);    txt(LEVVEL+ss+', ');
         if quitt then       txt(EARNED)
         else                txt(LEFFFT);
         str(score:14:0,ss); txt(': '+ss+DOLLAR);
         if ii = ne then
         begin
            ink(REDCL);
            yy := yy + 9;
            apos(20,yy); aline(476,yy);
            ink(BLUEC);
         end;
      end;
   end;  { for }
   springroll := yy;
end;


function showroll(ne: byte): moves;
{
| Des: Caller must establish context (e.g. by calling intoshow).
| Sef: Rollshown = Set true when we display honor roll.
}
var
   yy: integer;
begin
   rollshown := ne <> 0;
   if ne = 0 then
   begin
      ink(REDCL);
      apos(12,60); txt(HELLO1);
      apos(12,70); txt(HELLO2);
      showroll := exitshow(21,85);
   end
   else
   begin
      yy := springroll(ne, TRUE);
      showroll := exitshow(40,yy+14);
   end;
end;


procedure getline(var s: namstring; cno, lno: byte; onechar: boolean);
{
| Abs: Procedure getline will let the user type and/or edit a string  of  max.
|      length max.  The string will start  at  cursor  position  cno, lno.
|      Exit is through CR.
| Par: s        = String to edit.
|      cno, lno = Where start line (column and line no.).
|      onechar  = TRUE if immediate return.
| Ret: s        = Result of line edit.
| Cby: getformula, gettext and a number of other places.
}
const
   MAX = 12;
   WID =  6;

var
   x, o:  integer;
   cc: char;
   finis: boolean;

   procedure gotox;
   var
      xx, oo: integer;
   begin
      xx := cno + x * WID;    oo := cno + o * WID;
      apos(oo,lno-1); ink(BLACK); aline(oo,lno+8);
      apos(xx,lno-1); ink(BLUEC); aline(xx,lno+8);
   end;

   procedure printit(ss: namstring);
   var
      xx: integer;
   begin
      xx := cno + x * WID + 1;
      apos(xx,lno); ink(BLACK); block((lwidd*(DECXX-1) shr 1) - xx, 9);
      apos(xx,lno); ink(GREEN); txt(ss);
   end;

begin { getline }
   s := '';
   finis := FALSE;
   x := 1;
   o := 1;
   repeat
      gotox;
      o := x;
      read(kbd,cc);
      cc := chr(ord(cc) and $7F);
      case cc of
        CR:     finis := TRUE;
        CURRGT: if (x <= length(s)) and (x <= max) then x := x + 1;
        CURLFT: if                       x >   1   then x := x - 1;
        TAB:    x := length(s) + 1;
        ICRLFT: x := 1;
        HJELP:
           begin
              apos(0,PRLIN); ink(BLACK); block(406,9);
              apos(0,PRLIN); ink(GREEN);
              txt(EDIHLP+RETURN+'.');
           end;
        UTVID:
           begin { Delete left char }
              if (length(s) >= 1) and (x > 1)  then
              begin
                 x := x - 1;
                 delete(s,x,1);
                 printit(copy(s,x,length(s)-x+1)+' ');
              end;
           end;  { dellft }
        SLETT:
           begin { Delete right char }
              if x <= length(s) then
              begin
                 delete(s,x,1);
                 printit(copy(s,x,length(s)-x+1)+' ');
              end;
           end;  { delrgt }
        else
          if (cc >= ' ') and (cc <= #127) then
          begin
             if length(s) < max then
             begin
                insert(cc,s,x);
                printit(copy(s,x,length(s)-x+1));
                x := x + 1;
             end
             else write(BELL); { line to long }
          end;
      end; { case }
   until finis or onechar;
   o := cno + o * WID;
   apos(o,lno-1); ink(BLACK); aline(o,lno+8);
end;  { getline }


function gnamroll(lm: moves; sc: real; lv: integer): moves;
var
   entr: entrytype;
   bb: boolean;
   ii: byte;
begin
   font(0);
   commshow(21,29,94,BLACK);
   ink(REDCL);
   apos(6,30);  txt(CONGR1);
   apos(6,41);  txt(CONGR2);
   getline(entr.fname,155,41,FALSE);
   entr.level := lv;
   entr.quitt := lm = ACHIK;
   entr.score := sc;
   bb := FALSE;
   gnamroll := lm;
   for ii := 1 to length(entr.fname) do
      if entr.fname[ii] > ' ' then bb := TRUE;
   if bb then gnamroll := showroll(updtroll(entr,uniquebyte <> 0))
   else       font(fontptr);
end;


procedure mergefiles;
{
| Sef: hroll changed.
}
var
   ii, jj, col: integer;
   cc: char;
   lroll: honorroll;
   fast :integer absolute $FF8C;


begin
   clrscr;
   if configbyte = 1 then
   begin
      ink(REDCL);  defink(100,100,  0); { Red       }
      ink(BLUEC);  defink( 86, 86,100); { Blue      }
      ink(GREEN);  defink(  0,  0,  0); { Green     }
   end
   else for ii := 1 to 3 do begin ink(ii); defink(100,100,100); end;

   ii := springroll(11,FALSE);
   ink(BLUEC);
   for ii := 0 to 4 do
   begin
      apos(20,189+(9*ii)); txt(INSDSA[ii]);
   end;
   ink(GREEN); apos(20,170); txt(INSDSK); col := 0;
   repeat
      fast := 100;
      repeat
         if fast > 80 then
         begin
            if col = 0 then col := 100 else col := 0;
            defink(col,col,col);
            fast := 0;
         end;
      until keypressed;
      read(kbd,cc);
      if (cc = CR) or (cc = BRYT) then
      begin
         col := 0; defink(col,col,col);
         ii := bdos(GETDISK); bdos(RSETDSK); bdos(SELDISK,ii);
         if cabbageroll(lroll) then for jj := 1+stdgambyte to 10+stdgambyte do
            ii := updtroll(lroll[jj],TRUE);
         if cc = BRYT then writroll;
         ii := springroll(11,FALSE);
         ink(GREEN);
      end;
   until cc = BRYT;
   clrscr;
   initink;
end;  { mergefiles }


{ Install -------------------------------------------------------------------}

procedure install(config, soundo, unique, flusht, stdgam: byte);
const
   fname    = 'ROBOT.COM';
var
   src: file;
   buffer: array[0..127] of byte;
   bas, blk, byt: integer;
begin
   assign(src,fname);
   {$I-} reset(src); {$I+}
   if ioresult = 0 then
   begin
      bas := addr(configbyte) - $100; { Determine location in file. }
      blk := bas div 128;             { Logical sector number.      }
      byt := bas mod 128;             { Byte in sector.             }
      seek(src,blk);
      blockread(src,buffer,1);        { Read the block into buffer. }
      bas := addr(buffer) + byt;      { Calculate table base.       }

      mem[bas]   := config;           { Patch entry in buffer       }
      mem[bas+1] := soundo;           { Patch entry in buffer       }
      mem[bas+2] := unique;           { Patch entry in buffer       }
      mem[bas+3] := flusht;           { Patch entry in buffer       }
      mem[bas+4] := stdgam;           { Patch entry in buffer       }

      seek(src,blk);
      blockwrite(src,buffer,1);
      close(src);
   end;
   configbyte := config;              { Patch entry in TPA          }
   soundobyte := soundo;              { Patch entry in TPA          }
   uniquebyte := unique;              { Patch entry in TPA          }
   flushtbyte := flusht;              { Patch entry in TPA          }
   stdgambyte := stdgam;              { Patch entry in TPA          }
end;


procedure configur(reinst: boolean);
{
| Imp: Undocumented features:
|      '?' in response to monitor will return ROBOT to its pristine state.
|      '?' in resposne to standard game will make it fast, and save highscore.
| Des: Always: "Y" = default. "N" is what we test against.
}
type
   reply = (NONE,YES,NO,FEATURE);
var
   ss: namstring;
   config, soundo, unique, flusht, stdgam: byte;
   pyy: integer;
   rep: reply;

   function getynf(prompt: integer; featon: boolean): reply;
   {
   | Abs: Keeps asking the same question until a valid response is entered.
   | Ret: Some valid reply.
   }
   var
      rr: reply;
   begin
      ink(GREEN); apos(6,pyy);
      case prompt of
        1: txt(QSTRGB);
        2: txt(QSTSND);
        3: txt(QSTUNQ);
        4: txt(QSTTHD);
        5: txt(QSTSTD);
      end; { case }
      apos(146,pyy); txt('('+POSTIV+'/n) ?');
      rr := NONE;
      repeat
         getline(ss,187,pyy,TRUE);
         if      length(ss) =  0          then rr := YES
         else if featon and (ss[1] = '?') then rr := FEATURE
         else
         begin
            ss[1] := upcase(ss[1]);
            if      ss[1] = POSTIV then rr := YES
            else if ss[1] =   'N'  then rr := NO;
         end;
         if rr = NONE then begin write(BELL); apos(187,pyy); ink(BLACK); block(30,9); end;
      until rr <> NONE;
      getynf := rr;
      pyy := pyy + 10;
   end;

begin
   font(0); commshow(21,29,80,BLACK); pyy := 30;
   rep := getynf(1,TRUE);  if rep = NO then config := 0 else if rep = YES then config := 1 else config := 2;
   rep := getynf(2,FALSE); if rep = NO then soundo := 0 else soundo := 1;
   rep := getynf(3,FALSE); if rep = NO then unique := 0 else unique := 1;
   rep := getynf(4,FALSE); if rep = NO then flusht := 0 else flusht := 1;
   if reinst then
   begin
      rep := getynf(5,TRUE);
      if      rep = NO  then stdgam :=  0
      else if rep = YES then stdgam := 10
      else                   stdgam :=  1;
   end else stdgam := 10;
   install(config, soundo, unique, flusht, stdgam);
   if stdgambyte = 1 then stdgambyte := 0;
   initink;
   redscore := hroll[ 1+stdgambyte].score;
   grnscore := hroll[10+stdgambyte].score;
   font(fontptr);
end;


(* ENDINC *)


