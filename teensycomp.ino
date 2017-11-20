/*
 * Todo:
 * Graphics library (2D/3D)
 */

const int ledPin = 13;


uint32_t benchmark_counter = 0;
elapsedMillis sinceStart;
void benchmark() {
  VideoText_SetPos(100, 20);
  char text[20];
  sprintf(text, "%.0f ops/s", 1000 * (float)benchmark_counter / sinceStart);
  VideoText_WriteString(text);
}


void setup() {
  pinMode(ledPin, OUTPUT);

  Video_Init();
  Video_TestPattern();
  
  VideoText_Init();
  VideoText_WriteString("hello everybody!\nthis is amazing!\n");
  
  //benchmark_timer.begin(benchmark, 1000000);

  for (;;) {
    benchmark_counter++;
    benchmark();
  }
}


//uint16_t color = 0;
void loop() {
//  //for (int i = 0; i < 1000000; i++) {
//    benchmark_counter++;
//  //}
//
//  /*for (uint8_t y = 0; y < 50; y++) {
//    for (uint8_t int x = 0; x < 50; x++) {
//      Video_SetPixel((320-50)/2 + x, (200-50)/2 + y, color++);
//    }
//  }*/
}
