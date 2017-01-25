/*
 * Todo:
 * Graphics library (2D/3D)
 */

const int ledPin = 13;


uint32_t benchmark_counter = 0;
IntervalTimer benchmark_timer;
void benchmark() {
  VideoText_SetPos(100, 20);
  char text[20];
  sprintf(text, "%d ops", benchmark_counter);
  VideoText_WriteString(text);

  benchmark_counter = 0;
}


void setup() {
  pinMode(ledPin, OUTPUT);

  Video_Init();
  Video_TestPattern();
  
  VideoText_Init();
  VideoText_WriteString("hello everybody!\nthis is amazing!\n");
  
  benchmark_timer.begin(benchmark, 1000000);
}


uint16_t color = 0;
void loop() {
  benchmark_counter++;

  /*for (int y = 0; y < 50; y++) {
    for (int x = 0; x < 50; x++) {
      Video_SetPixel((320-50)/2 + x, (200-50)/2 + y, color++);
    }
  }*/
}
