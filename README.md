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
to do partial refreshes and grayscale and all the other features that we want. 
Since it's the same board layout that also reads the LCD from the Newton. 
I have - at least on the hardware side - everything to transfer LCD data 
to the ePaper.

ePaper TODO
-----------

- [ ] try to control the hires ePaper
  - [x] black screen, white screen, refresh
  - [ ] full image send
  - [ ] partial refresh
  - [ ] 4 levels of gray
- [ ] improve reading LCD data
  - [ ] get reading LCD data to work again
  - [ ] copy to TFT for easy verification
  - [ ] find sequences for different gray values
  - [ ] use DMA to get pixel data into RAM
- [ ] move LCD data to ePaper
  - [ ] implement image store and copy LCD lines into gray scale
  - [ ] find changes in image and generate refresh rectangle
  - [ ] partial refresh of ePaper
  - [ ] simulate 16 grays with 4 pixels of 4 grays
  - [ ] find maths to scale original res into ePaper res (not just *2)
  - [ ] find a good point for full refreshes
  - [ ] more timing optimiztion
- [ ] get touch data back to Newton
  - [ ] order resistive panel overlay
  - [ ] read touch data from display (no documentation yet!)
  - [ ] generate analog signal for Newton controller
  - [ ] align touch in and pen out
- [ ] extra goodies:
  - [ ] backlight/frontlight
  - [ ] "magic" display data to tune controller
- [ ] hardware
  - [ ] testing testing testing
  - [ ] flex pcb for LCD out to ePaper in
  - [ ] wiring of touch signal
  - [ ] 3d printed adapter to make display fit

ePaper log book
---------------

- adapt existing TFT driver code to control an ePaper display
- new files layout, refactoring existing code
- lo res display renders full screen
- hi res display renders full screen

AI Comment
----------

A few things I think are missing (or worth making explicit):

Definition of done per milestone
Add one measurable success line for each section, like refresh time target, ghosting limit, and acceptable touch latency.

Performance and memory budgets
Add targets for frame capture throughput, end-to-end latency, RAM usage, and CPU load so DMA/gray work has clear constraints.

Power behavior
For ePaper projects this is easy to forget: idle/sleep states, wake timing, and expected current draw in each mode.

Failure handling and recovery
Include watchdog/reset strategy for SPI stalls, bad frame sync, and display controller lockups.

Calibration and persistence
Touch alignment and grayscale tuning likely need stored calibration values; add non-volatile config storage and a calibration routine.

Visual validation tooling
Add a small test mode that shows diagnostic patterns, changed-region overlays, and timing stats over serial to debug partial refresh logic.

Test matrix
Create a quick checklist: different ambient temps, long-run stability, repeated partial updates (ghosting), and motion-heavy Newton screens.

Integration hygiene
Add one item for build/release housekeeping: known-good firmware tag per milestone and short bring-up notes for reproducibility.
