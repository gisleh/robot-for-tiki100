{----------------+
|     robot9.fnt |
| Font size:  96 |
| First    :   1 |
| Number   :   5 |
| Baseline :   0 |
| Height   :   9 |
+----------------}
type
   fontarray = array[0..95] of byte;
const
   FSIZ = 0;
   F1ST = 2;
   FLEN = 3;
   FPRO = 4;
   FRES = 5;
   FTAB = 6;
   MYFONT: fontarray = (
      $60, $00, 1, 5, 0, 9, $10, $00, $24, $00, $38, $00, $4C, $00, $4C, $00,
      11, 9, $04, $00, $0E, $00, $1B, $00, $35, $00, $2B, $80, $76, $C0, $7B,
        $40, $FD, $A0, $FF, $E0,
      11, 9, $9F, $20, $91, $20, $D1, $62, $6A, $C0, $3F, $80, $1F, $02, $3B,
        $82, $31, $80, $31, $80,
      11, 9, $1F, $00, $7F, $C0, $80, $20, $B1, $A0, $8E, $20, $A0, $A0, $9F,
        $20, $80, $20, $7F, $C0,
      11, 9, $40, $40, $E0, $E0, $44, $40, $04, $00, $0E, $00, $15, $00, $00,
        $00, $3F, $80, $40, $40);

{
| Abstract:
|    Lydeffekter for ROBOT.
|
| Contents:
|    fanfare
|    trudelt
|    brasvar
|    loselyd
|    zapsond
|    collide
|    teleprt
|    sdriver
}

procedure fanfare;
{
| Abs: Spiller en fanfare i kanal 1.
}
begin
   sound(0,  0, 0);
   sound(1,CC1,10); delay(250);
   sound(1,EE1,13); delay(250);
   sound(1,GG1,14); delay(250);
   sound(1,CC2,15); delay(999);
   sound(0,0,0);
end;


procedure trudelt;
{
| Abs: Spiller en liten trudelutt.
}
const
   Z = 2500;
begin { trudelutt }
sound(0, 0, 0);
sound(5,Z,  6); sound(1,CC1,16);                                   delay( 115);
sound(5,Z,  6); sound(1,EE1,16);                                   delay( 115);
sound(5,Z*4,6); sound(1,GG1,16);                                   delay( 450);
sound(5,Z*2,6); sound(1,GG1,16); sound(2,EE1,16);                  delay( 225);
sound(5,Z*4,6); sound(1,AA1,16); sound(2,FF1,16);                  delay( 450);
sound(5,Z*2,6); sound(1,DD1,16); sound(2,FF1,16); sound(4,HH1,16); delay( 225);
sound(5,Z*7,6); sound(1,CC1,16); sound(2,EE1,16); sound(4,CC2,16); delay(1260);
sound(0, 0, 0);
end;  { trudelutt }


procedure brasvar;
begin { brasvar }
   sound(0,   0,0);
   sound(5,2500,6); sound(1,CC1,16); delay(225);
   sound(5,2500,6); sound(1,CC2,16); delay(225);
   sound(5,7500,6); sound(1,CC3,16); delay(450);
   sound(0,   0,0);
end;  { brasvar }


procedure loselyd;
begin
   delay(600);      sound(0,  0, 0); {To allow zapsond to finish.}
   sound(1,AA1,10); sound(2,EE1,10); sound(4,CC1,10); delay(1500);
   sound(1,AA1,10); sound(2,FF1,10); sound(4,DD1,10); delay(1500);
   sound(1,Gx1,10); sound(2,EE1,10); sound(4,HH0,10); delay(1500);
   sound(1,AA1,10); sound(2,EE1,10); sound(4,CC1,10); delay(3000);
   sound(0,0,0);
end;


procedure zapsond;
begin
   if soundobyte = 0 then exit;
   sound(1,1955,16);
   sound(2,900,16);
   sound(4,319,16);
   sound(5,9000,6);
   sound(3,16,4);
end;


procedure collide;
{
| Abs: Lyden av en kollisjon.
| Imp: Verdien av PER gir mye av lydens karakter.  Pr|v 9000 og 1000.
}
const
   PER = 2250;
begin
   if soundobyte = 0 then exit;
   sound(1, 428,16);
   sound(2, 500,16);
   sound(4, 300,16);
   sound(3,  31, 7);
   sound(5, PER, 6);
end;


procedure teleprt;
begin
   if soundobyte = 0 then exit;
   sound(0,  0, 0);
   sound(1,119,16);
   sound(2,239,16);
   sound(4,319,16);
   sound(5,1200,6);
end;


procedure sdriver;
begin
   if soundobyte = 0 then exit;
   sound(1,955,16);
   sound(2,478,16);
   sound(4,319,16);
   sound(5,9000,0);
   sound(3,1,6);
end;


(* ENDINC *)
