

const int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);

  Video_Init();
  Video_TestPattern();
  
  VideoText_Init();
  VideoText_WriteString("hello everybody!\nthis is amazing!\n");
  
//  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < 100; i++) {
    Video_SetPixel(i, i, i);
  }
}
