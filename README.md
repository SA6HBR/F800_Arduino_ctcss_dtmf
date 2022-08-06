# Ericsson F800 with arduino ctcss encoder / decoder and dtmf decoder

This is a Beta and we are testing it now.

Only text in Swedish. 

Arduino som avkodar och kodar ctcss, samt avkodar dtmf.

Delar av kod har jag bland annat plockat från:
YO3HJV, ON4ADI & ON7EQ samt Tim Eckel

Funktionsbeskrivning av elektroniken: (Komponenterna är mer valda av vad som ligger på hyllan än 100% rätt.)
En LM317-modul för stabilisera drivspänningen från F800 till 8v. Behövde ett par extra kondensatorer för att inte Arduinon skulle störas av sändning.
En LM324 operationsförstärkare som lågpassfiler för ingående subton. Mitt val av komponenter får en bryt frekvens 153hz. fc= 1/(2πRC) 47nF 22k
Låpassfilter för utgående subton. Detta för att gå från fyrkantsvåg till sinus. Mitt val av komponenter får en bryt frekvens 159hz. fc= 1/(2πRC) 100R 10uF

## Useful Links
* [CircuitDiagram](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/KiCad/CircuitDiagram.pdf)
* [ArduinoNanoPinout](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/Arduino/ArduinoNanoPinout.pdf)
* [ArduinoCodeCTCSS](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/Arduino/F800_Arduino_ctcss/F800_Arduino_ctcss.ino)
* [ArduinoCodeDTMF](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/Arduino/F800_Arduino_dtmf/F800_Arduino_dtmf.ino)

* [F800 hardware](http://komradio.com/f800.html)

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/LICENSE) for details.




