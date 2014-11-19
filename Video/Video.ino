const int ledPin = 13;

IntervalTimer videoTimer;
const int videoSyncPin = 14;
const int videoRedPin = 15;
const int videoGreenPin = 16;
const int videoBluePin = 17;
const int videoIntensityPin = 18;

uint16_t videoCurrentScanlineNumber = 0;

uint32_t videoBuffer[(320 * 4 / 32) * 200];

inline void videoDelay(uint32_t n) {
  __asm__ volatile(
    "L_%=_videoDelay:"                      "\n\t"
    "subs   %0, #1"                         "\n\t"
    "bne    L_%=_videoDelay"                "\n"
    : "+r" (n) :
  );
}

//TODO: prepsat syncy na digitalWriteFast a opravit videoDelaye
inline void videoDrawScanlineLongPulses() {
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(875); // 27.3us
  digitalWriteFast(videoSyncPin, HIGH);
  videoDelay(150); // 4.7us
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(875); // 27.3us
  digitalWriteFast(videoSyncPin, HIGH);
}

inline void videoDrawScanlineShortPulses() {
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(75); // 2.35us
  digitalWriteFast(videoSyncPin, HIGH);
  videoDelay(950); // 29.65us
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(75); // 2.35us
  digitalWriteFast(videoSyncPin, HIGH);
}

inline void videoDrawScanlineBothPulses() {
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(875); // 27.3us
  digitalWriteFast(videoSyncPin, HIGH);
  videoDelay(150); // 4.7us
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(75); // 2.35us
  digitalWriteFast(videoSyncPin, HIGH);
}

inline void videoDrawScanlineImageEmpty() {
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(150); // 4.7us
  digitalWriteFast(videoSyncPin, HIGH);
}

inline void videoDrawScanlineImage(uint8_t imageLine) {
  // sync
  digitalWriteFast(videoSyncPin, LOW);
  videoDelay(150); // 4.7us
  
  // back porch
  digitalWriteFast(videoSyncPin, HIGH);
  videoDelay(183); // 5.7us
  
  // text-unsafe
  videoDelay(165); // 5.175us
  
  // picture
  uint32_t *pixelPtr;
  pixelPtr = videoBuffer + (imageLine * 10);
  for (uint16_t i = 0; i < /*320*/ 160; i += 8, pixelPtr++) {
    uint32_t pixelValue = *pixelPtr;
    
//    PORTC = 0b00010000;//I
//    PORTC = 0b00001000;//B
//    PORTC = 0b00000100;//G
//    PORTC = 0b00000010;//R
    
/*    digitalWriteFast(videoRedPin, (pixelValue >> 31) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 30) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 29) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 28) & 1);
    //videoDelay(1); // 0.208us
    digitalWriteFast(videoRedPin, (pixelValue >> 27) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 26) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 25) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 24) & 1);
    //videoDelay(1); // 0.208us
    digitalWriteFast(videoRedPin, (pixelValue >> 23) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 22) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 21) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 20) & 1);
    //videoDelay(1); // 0.208us
    digitalWriteFast(videoRedPin, (pixelValue >> 19) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 18) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 17) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 16) & 1);
    //videoDelay(1); // 0.208us
    digitalWriteFast(videoRedPin, (pixelValue >> 15) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 14) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 13) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 12) & 1);
    //videoDelay(1); // 0.208us
    digitalWriteFast(videoRedPin, (pixelValue >> 11) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 10) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 9) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 8) & 1);
    //videoDelay(1); // 0.208us
    digitalWriteFast(videoRedPin, (pixelValue >> 7) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 6) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 5) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 4) & 1);
    //videoDelay(1); // 0.208us
    digitalWriteFast(videoRedPin, (pixelValue >> 3) & 1);
    digitalWriteFast(videoGreenPin, (pixelValue >> 2) & 1);
    digitalWriteFast(videoBluePin, (pixelValue >> 1) & 1);
    digitalWriteFast(videoIntensityPin, (pixelValue >> 0) & 1);
    //videoDelay(1); // 0.208us*/
  }
  
  // text-unsafe, front porch (end of line)
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoGreenPin, LOW);
  digitalWriteFast(videoBluePin, LOW);
  digitalWriteFast(videoIntensityPin, LOW);
}

void videoDrawScanline() {
  noInterrupts();
  
  videoCurrentScanlineNumber++;
  if (videoCurrentScanlineNumber == 313) {
    videoCurrentScanlineNumber = 1;
  }
  
  if (videoCurrentScanlineNumber <= 5) {
    if (videoCurrentScanlineNumber < 3) {
      videoDrawScanlineLongPulses();
    } else if (videoCurrentScanlineNumber > 3) {
      videoDrawScanlineShortPulses();
    } else {
      videoDrawScanlineBothPulses();
    }
  } else if (videoCurrentScanlineNumber >= 310) {
      videoDrawScanlineShortPulses();
  } else {
      if ((videoCurrentScanlineNumber >= 70) && (videoCurrentScanlineNumber < 270)) {
        videoDrawScanlineImage(videoCurrentScanlineNumber - 70);
      } else {
        videoDrawScanlineImageEmpty();
      }
  }  
  
  interrupts();
}

void videoClearScreen() {
  for (uint16_t i = 0; i < 8000; i++) {
    videoBuffer[i] = 0;
  }
}

void videoTestPattern() {
  for (uint16_t i = 0; i < 8000; i++) {
    videoBuffer[i] = 0xE0000000;
    if (((i / 10) % 8) == 0) {
      videoBuffer[i] = 0xEEEEEEEE;
    }
  }
}


void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(videoSyncPin, OUTPUT);
  pinMode(videoRedPin, OUTPUT);
  pinMode(videoGreenPin, OUTPUT);
  pinMode(videoBluePin, OUTPUT);
  pinMode(videoIntensityPin, OUTPUT);
  
  videoClearScreen();
  videoTimer.begin(videoDrawScanline, 64);
  
  videoTestPattern();
//  videoTimer.priority(0);
  
  //Serial.begin(9600);
}

void loop() {
  digitalWriteFast(ledPin, HIGH);
  delay(50);
  digitalWriteFast(ledPin, LOW);
  delay(950);
  //Serial.printf("videoRedPin: %d\n", videoRedPin);
}
