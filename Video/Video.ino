const int ledPin = 13;

IntervalTimer videoTimer;
const int videoSyncPin = 14;
const int videoRedPin = 15;
const int videoGreenPin = 16;
const int videoBluePin = 17;
const int videoIntensityPin = 18;

int16_t videoCurrentScanlineNumber = 0;

inline void videoDelay(uint32_t n) {
  __asm__ volatile(
    "L_%=_videoDelay:"                      "\n\t"
    "subs   %0, #1"                         "\n\t"
    "bne    L_%=_videoDelay"                "\n"
    : "+r" (n) :
  );
}

inline void videoDrawScanlineLongPulses() {
  digitalWrite(videoSyncPin, LOW);
  videoDelay(864); // 27.3us
  digitalWrite(videoSyncPin, HIGH);
  videoDelay(140); // 4.7us
  digitalWrite(videoSyncPin, LOW);
  videoDelay(864); // 27.3us
  digitalWrite(videoSyncPin, HIGH);
}

inline void videoDrawScanlineShortPulses() {
  digitalWrite(videoSyncPin, LOW);
  videoDelay(63); // 2.35us
  digitalWrite(videoSyncPin, HIGH);
  videoDelay(940); // 29.65us
  digitalWrite(videoSyncPin, LOW);
  videoDelay(63); // 2.35us
  digitalWrite(videoSyncPin, HIGH);
}

inline void videoDrawScanlineBothPulses() {
  digitalWrite(videoSyncPin, LOW);
  videoDelay(864); // 27.3us
  digitalWrite(videoSyncPin, HIGH);
  videoDelay(140); // 4.7us
  digitalWrite(videoSyncPin, LOW);
  videoDelay(63); // 2.35us
  digitalWrite(videoSyncPin, HIGH);
}

inline void videoDrawScanlineImage() {
  digitalWrite(videoSyncPin, LOW);
  videoDelay(140); // 4.7us
  digitalWrite(videoSyncPin, HIGH);

    //picture (with front and back porch)
  digitalWriteFast(videoRedPin, HIGH); // 16ns
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, LOW);
  digitalWriteFast(videoRedPin, HIGH);
  digitalWriteFast(videoRedPin, LOW);

  
  asm("nop");
  digitalWrite(videoRedPin, HIGH);
  digitalWrite(videoRedPin, LOW);
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
      videoDrawScanlineImage();
  }
  
  
  interrupts();
}

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(videoSyncPin, OUTPUT);
  pinMode(videoRedPin, OUTPUT);
  pinMode(videoGreenPin, OUTPUT);
  pinMode(videoBluePin, OUTPUT);
  pinMode(videoIntensityPin, OUTPUT);
  
  videoTimer.begin(videoDrawScanline, 64);
  
  Serial.begin(9600);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
  delay(950);
  Serial.printf("videoRedPin: %d\n", videoRedPin);
}
