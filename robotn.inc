{
| ROBOTN.INC
| Abs: Norwegian ROBOT strings.
| Imp: Collection of strings, for translating ROBOT to foreign languages.
| Aut: Copyright (c) 1985 Gisle Hannemyr.
| Des: All Yes/No questions assumes negative response character  is  "N",  but
|      the character constant  POSTIV  should be set equal to the *upper case*
|      initial letter of the positive response  of  the  appropriate  language
|      (Yes, Ja, Oui, Si), which also is the default.  This hack exploits  the
|      fact that most  (all?)  indo-european languages share  the same initial
|      letter for a negative response, while the symbol for a postive response
|      are different between languages.
}

const
   (* Some vanilla prompts *)
   PRTWRN = '*** Disken kan ikke v{re skrivebeskyttet.';
   PRSKEY = 'Trykk p} hvilken som helst tast';
   HELLO1 = 'Hallo igjen! Du har faktisk gjort det';
   HELLO2 = 'bedre f|r.';


   (* Bottomline prompts. Do not create any wider than the present IBMWON. *)
   IBMWON = 'Service koster, som du vet...';
   YOUWON = 'Bra! N} kommer neste niv}...';
   NEWBRD = 'Setter opp nytt brett . . .';
   CONFIG = 'Konfigurerer ROBOT . . .';
   RETIRE = 'Avskjed i n}de innvilget.';
   NICDAY = 'Ha det!  Og ha en fin dag.';
   TGLSND = 'Sl}r lyden ';
   TGLONN = 'p}.';
   TGLOFF = 'av.';

   (* Strings in highscore display.  Try to maintain the columnar format;
   ** i.e.: keep the pairs RESIGN/SERVIC and EARNED/LEFFFT of equal size.
   ** Also, watch the total width of this display. *********************)
   RESIGN = 'trakk seg {refullt ut';
   SERVIC = 'tegnet service-avtale';
   LEVVEL = ' p} niv} ';
   EARNED = 'tjent';
   LEFFFT = 'igjen';
   DOLLAR = ' kr.';
   CONGR1 = 'Gratulerer! Ditt navn skal bevares for';
   CONGR2 = 'ettertiden. Hva heter du? ';

   (* The positive response char; J = JA (norway). *)
   POSTIV = 'J';

   (* Initialization strings. *)
   QSTRGB = 'Har du RGB fargemonitor';
   QSTSND = '\nsker  du  lydeffekter';
   QSTUNQ = 'Unike navn i scorelista';
   QSTTHD = 'Skal "typeahead" kastes';
   QSTSTD = 'Standardutgave av ROBOT';

   (* Helptexts -- watch the field widths. *)
   EDIHLP = 'UTVID = slett til venstre.  SLETT = slett til h|yre.  Avslutt med ';
   KEYRST = 'HJEM:  '; (* Do not change total width of this field. *)
   KEYPNT = 'UTVID: '; (*                  - " -                   *)
   KEYSND = 'SLETT: '; (*                  - " -                   *)
   KEYCHT = 'ANGRE: '; (*                  - " -                   *)
   KEYBRK = 'BRYT:  '; (*                  - " -                   *)
   KEYHLP = 'HJELP: '; (*                  - " -                   *)
   TXTMOV = 'Bruk en av piltastene for } flytte.';
   TXTRST = 'Bli sittende p} samme sted.';
   TXTSTP = 'Sikker teleport.';
   TXTOTP = 'Vanlig teleport.';
   TXTZAP = 'Super-zap.';
   TXTRFR = 'Tegn skjermen p} nytt';
   TXTCHI = 'Avslutt med de poengene du har.';
   TXTSTD = 'St} stille resten av omgangen.';
   TXTPNT = 'Vis beste poengsum.';
   TXTSND = 'Vipp lydeffekter av/p}.';
   TXTCHT = 'Stopp og start med nytt brett.';
   TXTBRK = 'Avbryt og g} tilbake til TIKO.';
   TXTHLP = 'Skriv ut dette.';

   (* Merge feature prompts. *)
   INSDSK = 'Sett inn en ny disk.  Trykk '#141' for } lese,  <BRYT>  for } skrive og avslutte.';
   INSDSA: array[0..4] of string[76] = (
            'Samsortering av  "ROBOT.HSC"-filer.  Putt disketter inn i aktiv platestasjon',
            'n}r du f}r beskjed om det.  Trykk p} '#141' for } lese "ROBOT.HSC" fra disketten.',
            'Navn og poengsummer blir m} sortert inn i tabell i RAM.  N}r alle diskettene',
            'er lest,  setter du inn disketten hvor du |nsker den samsorterte "ROBOT.HSC"',
            'lagret.  Trykk <BRYT> for } avslutte samsortering og skrive resultat p} fil.');


(* ENDINC *)
