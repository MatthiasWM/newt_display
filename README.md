# newt_display

Convert Newton MP2x00 LCD data stream into TFT and ePaper output.

This is a continuation of the newt_lcd project in 
https://github.com/MatthiasWM/newt_lcd/

After a long slow winter, I was finally able to put time into the ePaper 
display (GDEH058). I was able to communicate with a very similar display 
(GDEY0583T81) using the LCD reader setup that I showed on video a while ago. 
I will try to control the ePaper display that you sent next. It has amuch 
better resolution.

Now that I was able to find most of the documentation online, I should be able 
to do partial refreshs and grayscale and all the other features that we want. 
Since it's the same board layout that also reads the LCD from the Newton. 
I have - at least on the hardware side - everything to transfer LCD data 
to the ePaper.

ePaper done:
- adapt existing TFT driver code to control an ePaper display

ePaper TODO:
- try to control the hires ePaper
  - full image send
  - partial refresh
  - 4 levels of gray
- improve reading LCD data
  - use DMA to get pixel data into RAM
  - analyse grayscale sequences
  - accumulate frames to grayscale image
- move LCD data to ePaper
  - find changes in iimage and generate refresh rectangle
  - partial refresh of ePaper
  - simulate 16 grays with 4 pixels of 4 grays
  - find maths to scale original res into ePaper res (not just *2)
  - find a good point for full refreshes
- get touch data back to Newton
  - read touch data from display (no documentation yet!)
  - generate analog signal for Newton controller
  - align touch in and pen out
- extra goodies:
  - backlight/frontlight
  - "magic" display data to tune controller
- hardware
  - testing testing testing
  - flex pcb for LCD out to ePaper in
  - wiring of touch signal
  - 3d printed adapter to make display fit



