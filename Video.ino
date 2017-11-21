/**
 * Generating VGA video
 * VGA Signal 640 x 400 @ 70 Hz
 * Usable picture is 320x200 x 16bpp
 *
 * Screen refresh rate  60 Hz
 * Vertical refresh  31.46875 kHz
 * Pixel freq. 25.175 MHz
 *
 * Scanline part  Pixels  Time [μs]
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

#include "Video.h"
 
IntervalTimer Video::timer;

uint16_t Video::buffer[width * height];
uint32_t Video::dmaBuffer[dmaBufferLineSizeStart + dmaBufferLineSize + dmaBufferLineSizeStop];

uint16_t Video::currentScanlineNumber = 1;
uint32_t Video::startScanlineCyccnt = ARM_DWT_CYCCNT;
uint32_t Video::startSyncCyccnt = 0;
uint32_t Video::startBPorchCyccnt = 0;
uint32_t Video::startImageCyccnt = 0;

void Video::DrawScanline() {
  timer.begin(Video::DrawScanline, 30.2); // less than 31.777557100298 us
  noInterrupts();

  int16_t imageLine = (currentScanlineNumber - 47) / 2;
  if (imageLine < 0) {
    imageLine = height;
  }
  // front porch
  while (startScanlineCyccnt > ARM_DWT_CYCCNT) {};

  startSyncCyccnt = startScanlineCyccnt + 152; // front porch
  startBPorchCyccnt = startSyncCyccnt + 915; // sync
  startImageCyccnt = startBPorchCyccnt + 458; // back porch
  startScanlineCyccnt = startScanlineCyccnt + 7627; // 31.777557100298 us - whole line

  // VSync + HSync
  while (startSyncCyccnt > ARM_DWT_CYCCNT) {};
  digitalWriteFast(vSyncPin, (currentScanlineNumber < 2) ? HIGH : LOW);
  digitalWriteFast(hSyncPin, LOW);

  // back porch
  while (startBPorchCyccnt > ARM_DWT_CYCCNT) {};
  digitalWriteFast(hSyncPin, HIGH);

  if (imageLine < height) {
    // image data
    while (startImageCyccnt > ARM_DWT_CYCCNT) {};
  
    // send from DMA buffer
    DMA_TCD1_SADDR = dmaBuffer;
    DMA_TCD1_SOFF = 4;
    DMA_TCD1_ATTR = DMA_TCD_ATTR_SSIZE(2) | DMA_TCD_ATTR_DSIZE(2);
    DMA_TCD1_NBYTES_MLNO = (dmaBufferLineSizeStart + dmaBufferLineSize + dmaBufferLineSizeStop) * 4;
    DMA_TCD1_SLAST = 0;
    DMA_TCD1_DADDR = &GPIOB_PDOR;
    DMA_TCD1_DOFF = 0;
    DMA_TCD1_CITER_ELINKNO = 1;
    DMA_TCD1_DLASTSGA = 0;
    DMA_TCD1_BITER_ELINKNO = 1;
    DMA_TCD1_CSR = DMA_TCD_CSR_START | DMA_TCD_CSR_BWC(3);
  
    // prepare next line into DMA buffer
    uint16_t *pixelPtr;
    uint16_t *endPtr;
    uint32_t *targetPtr;
    pixelPtr = buffer + (imageLine * width);
    endPtr = pixelPtr + width;
    targetPtr = dmaBuffer + dmaBufferLineSizeStart;
    pixelPtr--;
    while (++pixelPtr < endPtr) {
      uint16_t val = (*pixelPtr);
      (*(targetPtr++)) = (val & 0b111111) + ((val & 0b11000000) << 4) + ((val & 0b1111111100000000) << 8);
    }
  }
  
  currentScanlineNumber++;
  if (currentScanlineNumber > scanlines) {
    currentScanlineNumber = 1;
  }
  
  interrupts();
}

void Video::Init() {
  // initialize the digital pin as an output.
  pinMode(hSyncPin, OUTPUT);
  pinMode(vSyncPin, OUTPUT);

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
  Video::ClearScreen();

  // clear port;
  GPIOB_PDOR = 0x00000000;

  // initialize DMA buffer
  for (uint16_t i = 0; i < dmaBufferLineSizeStart + dmaBufferLineSize + dmaBufferLineSizeStop; i++) {
    dmaBuffer[i] = 0;
  }

  // initialize CPU cycle counter
  ARM_DEMCR |= ARM_DEMCR_TRCENA;
  ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;

  // set backdoor (DMA) vs CPU priority to DMA
  MCM_CR |= (1 << 29) | (1 << 28) | (1 << 25) | (1 << 24);

  // disable unwanted interrupts
  NVIC_ICER0 = 0xffffffff;
  NVIC_ICER1 = 0xffffffff;
  NVIC_ICER2 = 0xffffffff;
  NVIC_ICER3 = 0xffffffff;

  // initialize video timer and start loop
  timer.priority(0);
  Video::DrawScanline();
}

void Video::ClearScreen() {
  for (uint32_t i = 0; i < width * height; i++) {
    buffer[i] = 0;
  }
}

void Video::TestPattern() {
  for (uint32_t i = 0; i < width * height; i++) {
    buffer[i] = i % 65536;
  }
}

void inline Video::SetPixel(uint16_t x, uint8_t y, uint16_t color) {
  buffer[x + y * width] = color;
}
