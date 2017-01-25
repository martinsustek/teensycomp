/**
 * Generating VGA video
 * VGA Signal 640 x 400 @ 70 Hz
 * Usable picture is 320x200 x 16bpp
 *
 * Screen refresh rate  60 Hz
 * Vertical refresh  31.46875 kHz
 * Pixel freq. 25.175 MHz
 *
 * Scanline part  Pixels  Time [µs]
 * Visible area      640 25.422045680238
 * Front porch        16  0.63555114200596
 * Sync pulse         96  3.8133068520357
 * Back porch         48  1.9066534260179
 * Whole line        800 31.777557100298
 *
 * Frame part  Lines  Time [ms]
 * Visible area  400  12.711022840119
 * Front porch    12   0.38133068520357
 * Sync pulse      2   0.063555114200596
 * Back porch     35   1.1122144985104
 * Whole frame   449  14.268123138034
 * 
 */

#define VIDEO_USE_DMA 1

/**
 * TODO:
 * tick-precise Video_timer - to keep it in sync with cpucounter
 * asm routine for wait to cpucounter
 */
IntervalTimer Video_timer;

const uint16_t Video_width = 320;
const uint8_t Video_height = 200;
uint16_t Video_buffer[Video_width * Video_height];

#if VIDEO_USE_DMA
const uint16_t Video_dmaBufferLineSize = Video_width;
const uint8_t Video_dmaBufferLineSizeStart = 0; // for time stabilization??
const uint8_t Video_dmaBufferLineSizeStop = 1; // for reseting output back to zero
uint32_t Video_dmaBuffer[2][Video_dmaBufferLineSizeStart + Video_dmaBufferLineSize + Video_dmaBufferLineSizeStop];
#endif

const uint16_t Video_scanlines = 449;
uint16_t Video_currentScanlineNumber = 1;
uint32_t Video_startScanlineCyccnt = ARM_DWT_CYCCNT;

const int Video_vSyncPin = 3;
const int Video_hSyncPin = 4;

inline void Video_DrawImageLine(uint16_t imageLine, uint8_t oddScanLine, uint8_t vsyncValue) {
  // front porch
  while (Video_startScanlineCyccnt > ARM_DWT_CYCCNT) {};
  GPIOB_PDOR = 0x00000000;
  uint32_t syncCyccnt = Video_startScanlineCyccnt + 114; // front porch
  uint32_t bPorchCyccnt = syncCyccnt + 686; // sync
  uint32_t imageCyccnt = bPorchCyccnt + 343 + 50; // back porch (+ left offset)
  Video_startScanlineCyccnt = Video_startScanlineCyccnt + 5720; // 31.777557100298 us - whole line

  // VSync + HSync
  while (syncCyccnt > ARM_DWT_CYCCNT) {};
  digitalWriteFast(Video_vSyncPin, vsyncValue);
  digitalWriteFast(Video_hSyncPin, LOW);
  
  // back porch
  while (bPorchCyccnt > ARM_DWT_CYCCNT) {};
  digitalWriteFast(Video_hSyncPin, HIGH);

 
  // image data
  if (imageLine < Video_height) {
    while (imageCyccnt > ARM_DWT_CYCCNT) {};
  
#if VIDEO_USE_DMA
    // send from DMA buffer
    DMA_TCD1_SADDR = Video_dmaBuffer[imageLine % 2];
    DMA_TCD1_SOFF = 4;
    DMA_TCD1_ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2);
    DMA_TCD1_NBYTES_MLNO = (Video_dmaBufferLineSizeStart + Video_dmaBufferLineSize + Video_dmaBufferLineSizeStop) * 4;
    DMA_TCD1_SLAST = 0;
    DMA_TCD1_DADDR = &GPIOB_PDOR;
    DMA_TCD1_DOFF = 0;
    DMA_TCD1_CITER_ELINKNO = 1;
    DMA_TCD1_DLASTSGA = 0;
    DMA_TCD1_BITER_ELINKNO = 1;
    DMA_TCD1_CSR = DMA_TCD_CSR_START | DMA_TCD_CSR_BWC(3);
    //while (!(DMA_TCD1_CSR & DMA_TCD_CSR_DONE)) /* wait */ ;
  
    // prepare next line into DMA buffer
    if (!oddScanLine) {
      uint16_t *pixelPtr;
      uint16_t *endPtr;
      uint32_t *targetPtr;
      pixelPtr = Video_buffer + (((imageLine + 1) % Video_height) * Video_width);
      endPtr = pixelPtr + Video_width;
      targetPtr = Video_dmaBuffer[((imageLine + 1) % 2)] + Video_dmaBufferLineSizeStart;
      pixelPtr--;
      while (++pixelPtr < endPtr) {
        uint16_t val = (*pixelPtr);
        (*(targetPtr++)) = (val & 0b111111) + ((val & 0b11000000) << 4) + ((val & 0b1111111100000000) << 8);
      }
    }
#else
    uint16_t *pixelPtr;
    uint16_t *endPtr;
    pixelPtr = Video_buffer + (imageLine * Video_width);
    endPtr = pixelPtr + Video_width;
    pixelPtr--;
    while (++pixelPtr < endPtr) {
      uint16_t val = (*pixelPtr);
      GPIOB_PDOR = (val & 0b111111) + ((val & 0b11000000) << 4) + ((val & 0b1111111100000000) << 8);
      asm("nop\n nop\n nop\n");
    }
    GPIOB_PDOR = 0x00000000;
#endif
  }
}

void Video_DrawScanline() {
  noInterrupts();

  int16_t imageLine = (Video_currentScanlineNumber - 47) / 2;
  if (imageLine < 0) {
    imageLine = Video_height;
  }
  Video_DrawImageLine(imageLine, Video_currentScanlineNumber % 2, (Video_currentScanlineNumber < 2) ? HIGH : LOW);
  
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

  // clear screen
  Video_ClearScreen();

  // initialize CPU cycle counter and timer
  ARM_DEMCR |= ARM_DEMCR_TRCENA;
  ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
  Video_timer.priority(0);
  Video_timer.begin(Video_DrawScanline, 31); // less than 31.777557100298 us

#if VIDEO_USE_DMA
  // initialize DMA buffer
  for (uint16_t i = 0; i < Video_dmaBufferLineSizeStart + Video_dmaBufferLineSize + Video_dmaBufferLineSizeStop; i++) {
    Video_dmaBuffer[0][i] = 0;
    Video_dmaBuffer[1][i] = 0;
  }
#endif
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
