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

![alt text](https://github.com/SA6HBR/F800_Arduino_ctcss_dtmf/blob/main/image/F800_block_scheme.png "block scheme")  
Blockschema. AudioLine är ljud över 300hz och det kopplas direkt mellan RX till TX.  
Från RX AudioData kommer även subtonen och via lp-filter samt op med riktigt hög förstärkning, så bir det en fyrkantsvåg som Arduinon avkodar. 
 
![alt text](https://github.com/SA6HBR/F800_Arduino_ctcss_dtmf/blob/main/image/F800_CTCSS.png "CTCSS")  
Som exempel kan man styra funktioner och kanaler med olika subtoner. Koppla samman repetrar som kör på en viss subton. 
 
![alt text](https://github.com/SA6HBR/F800_Arduino_ctcss_dtmf/blob/main/image/F800_DTMF.png "DTMF")  
Via dtmf kan man också styrafunktioner. Avkoda ton på 130ms om man har 70ms mellan dem. 
Missar ibland första tonen om man kör på så korta toner, så förslagsvis har man första siffra dubbelt och i koden bortser från det.  
 
## Useful Links
* [Circuit Diagram](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/KiCad/CircuitDiagram.pdf)
* [Arduino Nano Pinout](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/Arduino/ArduinoNanoPinout.pdf)
* [Arduino code for CTCSS](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/Arduino/F800_Arduino_ctcss/F800_Arduino_ctcss.ino)
* [Arduino code for DTMF](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/Arduino/F800_Arduino_dtmf/F800_Arduino_dtmf.ino)
* [F800 hardware](http://komradio.com/f800.html)

## License

GNU General Public License v3.0, see [LICENSE](https://github.com/SA6HBR/F800_Arduino_ctcss/blob/main/LICENSE) for details.
