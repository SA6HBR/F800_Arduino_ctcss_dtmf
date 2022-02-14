# Ericsson F800 with arduino ctcss encoder and decoder

This is a Beta and has not been tested yet.

Only text in Swedish.

Arduino som avkodar och kodar ctcss.
Fungerar hemma på bänken, men har inte povkört som repeater ännu.

Delar av kod har jag bland annat fått från:
YO3HJV, ON4ADI & ON7EQ samt Tim Eckel

Funktionsbeskrivning av elektroniken: (Komponenterna är mer valda av vad som ligger på hyllan än 100% rätt.)

En LM317-modul för stabilisera drivspänningen från F800 till 8v. Behövde ett par extra kondensatorer för att inte Arduinon skulle störas av sändning.
En LM324 operationsförstärkare som lågpassfiler för ingående subton. Mitt val av komponenter får en bryt frekvens 153hz. fc= 1/(2πRC) 47nF 22k
Låpassfilter för utgående subton. Detta för att gå från fyrkantsvåg till sinus. Mitt val av komponenter får en bryt frekvens 159hz. fc= 1/(2πRC) 100R 10uF
Möjligt behövs det en operationsförstärkare för att höja utgående subton.
"Sound bypass" 150nF spärrar inkommande subton att gå igenom radion.

Funktionsbeskrivning av koden:
Man ställer in vilka kanaler som skall skannas.
Första kanalen används enbart för att styra starta och stoppa skanning.
Den kanalen som lyssnas på när skanning är stoppad.
Resterande kanaler används för repeaterfunktion. Samma kanal kan användas flera gånger om den skall användas med flera subtoner.

char *reciverChannelText[] = {"CH 1", "CH 2", "CH 3"};
byte      reciverChannel[] = {     1,      2,      3}; //F802 Channel


Val av ingående och utgående subtoner. Första tonen används inte för den ställs in på annat ställe.
int           ctcssForRX[] = {     0,      7,      9}; //ctcss for RX
int           ctcssForTX[] = {     0,      9,      7}; //ctcss for TX

Val av subton för starta och stoppa skanning
int scanOff    = 0; //ctcss 67.0 for stop scanning  
int scanOn     = 5; //ctcss 77.0 for starting scanning

## Useful Links
* [CircuitDiagram](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/doc/F800_Arduino_ctcss/KiCad/CircuitDiagram.pdf)
* [ArduinoNanoPinout](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/doc/F800_Arduino_ctcss/Arduino/ArduinoNanoPinout.pdf)
* [ArduinoCode](https://github.com/SA6HBR/F800_Arduino_ctcss/Arduino/F800_Arduino_ctcss/F800_Arduino_ctcss.ino)

* [ctcss-encoder](https://bitbucket.org/teckel12/arduino-new-tone/wiki/Home)
* [ctcss-decoder](https://github.com/yo3hjv/Arduino/blob/master/CTCSS%20fast%20decoder)
* [F800 hardware](http://komradio.com/f800.html)

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/LICENSE) for details.




