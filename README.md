# MAX7219 Display and Text Scroller
This is a simple project using MAX7219 modules driving 8x8 LED matrix display to show a scrolling text. The sketch uses a ESP8266 (NodeMCU, or Wemos D1 mini), to drive the display. The ESP hosts a website where the text, the scrolling speed and the brightness can be controlled. But you can change any of these using simple HTTP commands:<br/>
Changing text: http://ip/set?text=My new text<br/>
Changing speed: http://ip/set?delay=40 where the delay is the miliseconds between steps. Use smaller value for faster scroll<br/>
Changing brightness: http://ip/set?intensity=4 where the brightness value can be between 0 and 15.<br/>
<br/><br/>
<b>Wiring</b><br/>
<table>
<tr><th>MAX7219 display</th><th>Cable color</th><th>ESP Pin</tr>
  <tr><td>VCC</td><td>Blue</td><td>Vin on the NodeMCU, 5V on Wemos D1 mini</td></tr>
<tr><td>GND</td><td>Purple</td><td>GND</td></tr>
<tr><td>DIN</td><td>Grey</td><td>D7</td></tr>
<tr><td>CS</td><td>White</td><td>D4 (this can be a different pin too)</td></tr>
<tr><td>CLK</td><td>Black</td><td>D5</td></tr>
</table>
<br/><br/>
You can watch a detailed video on this project at: https://youtu.be/ndeLOQo68IA
