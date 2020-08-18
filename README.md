# SK6812-RGBW-ESP32
Quick little class to drive SK6812 RGBW leds reliably on a ESP32

All libraries I have tried on ESP32 resulted in flashing, weird colours. When using the Arduino RMT code,
it caused artefacts on the SPI display I used in the same project.

I made this small class to succesfully ( i.e. no flicker and no artefacts on my SPI display) drive my SK6812 RGBW strip. 
Use at your own peril, for WS2812s you must change the timing and the number of bytes per pixel, but it's a way forward maybe.
