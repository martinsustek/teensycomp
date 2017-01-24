/*
 * Generating VGA video
 * VGA Signal 640 x 480 @ 60 Hz Industry standard timing
 *
 * Screen refresh rate  60 Hz
 * Vertical refresh  31.46875 kHz
 * Pixel freq. 25.175 MHz
 *
 * Polarity of horizontal sync pulse is negative.
 * Scanline part  Pixels Time [µs]
 * Visible area      640 25.422045680238
 * Front porch        16  0.63555114200596
 * Sync pulse         96  3.8133068520357
 * Back porch         48  1.9066534260179
 * Whole line        800 31.777557100298
 *
 * Polarity of vertical sync pulse is negative.
 * Frame part  Lines Time [ms]
 * Visible area  480 15.253227408143
 * Front porch    10  0.31777557100298
 * Sync pulse      2  0.063555114200596
 * Back porch     33  1.0486593843098
 * Whole frame   525 16.683217477656
 * 
 */

IntervalTimer Video_timer;

const uint16_t Video_width = 320;
const uint8_t Video_height = 240;
uint16_t Video_buffer[Video_width * (Video_height + 1)];

const uint16_t Video_scanlines = 525;
uint16_t Video_currentScanlineNumber = 1;
uint32_t Video_nextScanlineCyccnt = 0;

const int Video_vSyncPin = 3;
const int Video_hSyncPin = 4;

inline void Video_Delay(uint32_t n) {
  __asm__ volatile(
    "L_%=_videoDelay:"                      "\n\t"
    "subs   %0, #1"                         "\n\t"
    "bne    L_%=_videoDelay"                "\n"
    : "+r" (n) :
  );
}

inline void Video_DrawImageLine(uint16_t imageLine, uint8_t vsyncValue) {
  // Count cycles to sync (TODO: could be written more precise in ASM?)
  uint32_t cyccnt;
  do {
    cyccnt = ARM_DWT_CYCCNT;
  } while (Video_nextScanlineCyccnt > cyccnt);
  Video_nextScanlineCyccnt = cyccnt + 5710; // 31.777557100298 us

  // VSync
  digitalWriteFast(Video_vSyncPin, vsyncValue);
  
  // HSync
  digitalWriteFast(Video_hSyncPin, LOW);
  Video_Delay(227); // 3.8133068520357 us
  
  // back porch
  digitalWriteFast(Video_hSyncPin, HIGH);
  if (imageLine == Video_height) {
    GPIOB_PDOR = 0x00000000;
    return;
  }
  Video_Delay(113); // 1.9066534260179 us
  
  // picture
  uint16_t *pixelPtr;
  uint16_t *endPtr;
  pixelPtr = Video_buffer + (imageLine * Video_width);
  endPtr = pixelPtr + Video_width;
  pixelPtr--;
  while (++pixelPtr < endPtr) {
    uint16_t val = (*pixelPtr);
    GPIOB_PDOR = (val & 0b111111) + ((val & 0b11000000) << 4) + ((val & 0b1111111100000000) << 8);
    asm("nop\n nop\n nop"); // 1px = 25.175 MHz (39.564787339268 ns)
  }

  // front porch
  GPIOB_PDOR = 0x00000000;  // (till the end of line)
}

void Video_DrawScanline() {
  noInterrupts();

  int16_t imageLine = (Video_currentScanlineNumber - 35) / 2;
  if (imageLine < 0) {
    imageLine = Video_height;
  }
  if (imageLine > Video_height) {
    imageLine = Video_height;
  }
  Video_DrawImageLine(imageLine, (Video_currentScanlineNumber < 2) ? LOW : HIGH);
  
  Video_currentScanlineNumber++;
  if (Video_currentScanlineNumber > Video_scanlines) {
    Video_currentScanlineNumber = 1;
  }
  
  interrupts();
}

void Video_Init() {
  // initialize the digital pin as an output.
  pinMode(Video_hSyncPin, OUTPUT);
  pinMode(Video_vSyncPin, OUTPUT);

  pinMode(16, OUTPUT); // PTB0
  pinMode(17, OUTPUT); // PTB1
  pinMode(19, OUTPUT); // PTB2
  pinMode(18, OUTPUT); // PTB3
  pinMode(49, OUTPUT); // PTB4
  pinMode(50, OUTPUT); // PTB5
  pinMode(31, OUTPUT); // PTB10
  pinMode(32, OUTPUT); // PTB11
  pinMode( 0, OUTPUT); // PTB16
  pinMode( 1, OUTPUT); // PTB17  
  pinMode(29, OUTPUT); // PTB18
  pinMode(30, OUTPUT); // PTB19
  pinMode(43, OUTPUT); // PTB20
  pinMode(46, OUTPUT); // PTB21
  pinMode(44, OUTPUT); // PTB22
  pinMode(45, OUTPUT); // PTB23

  // initialize CPU cycle counter and timer
  ARM_DEMCR |= ARM_DEMCR_TRCENA;
  ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
  Video_timer.begin(Video_DrawScanline, 31); // less than 31.777557100298 us
  Video_timer.priority(0);

  // clear screen
  Video_ClearScreen();
}

void Video_ClearScreen() {
  for (uint32_t i = 0; i < Video_width * Video_height; i++) {
    Video_buffer[i] = 0;
  }
}

void Video_TestPattern() {
  for (uint32_t i = 0; i < Video_width * Video_height; i++) {
    Video_buffer[i] = i % 65536;
  }
}

void Video_SetPixel(uint16_t x, uint8_t y, uint16_t color) {
  Video_buffer[x + y * Video_width] = color;
}
