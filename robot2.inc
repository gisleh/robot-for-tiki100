{ Initialization ------------------------------------------------------------}


procedure initink;
var
  ii: integer;
begin; { initink }
   ink(BLACK);  defink(  0,  0,  0); { Black     }
   if configbyte = 1 then
   begin
      ink(REDCL);  defink(100,  0,  0); { Red       }
      ink(BLUEC);  defink( 57, 71,100); { Blue      }
      ink(GREEN);  defink(  0,100,  0); { Green     }
   end
   else for ii := 1 to 3 do begin ink(ii); defink(100,100,100); end;
end;  { initink }


procedure initfont(var myfont: fontarray);
{
| Des: Add one to lheit, lwidd to get air between symbols.
| Sef: bboxh, lheit, bboxw, lwidd = height and width of symbols.
}
begin { initfont }
   fontptr := addr(myfont);
   font(fontptr);
   bboxh := myfont[FRES];
   bboxw := myfont[myfont[FTAB]];
   lheit := bboxh+1;
   lwidd := bboxw+1;
end;  { initfont }


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


function wprot: boolean;
{
| Abs: Check if write protect tag is present on logged disk.
| Src: Otto Milvang.
| Ret: TRUE if it is, else FALSE.
| Des: Must be called while disk is rotating.
| Env: Tiki-100, Z80, Turbo-Pascal.
}
const
   MASK = $40;
var
   bb: byte;
begin
   inline($F3); port[$10] := $D0; delay(56); bb := port[$10]; inline($FB);
   wprot := (bb and MASK) <> 0;
end;

(* ENDINC *)
