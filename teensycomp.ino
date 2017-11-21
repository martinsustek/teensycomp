#include "Core.h"

const int ledPin = 13;
const char helloString[] = "hello everybody!\nthis is amazing!\n";

uint32_t benchmark_counter = 0;
elapsedMillis sinceStart;
void benchmark() {
  Video::TestPattern();
  VideoText::SetAttributes(VideoTextAttribute_none);
  VideoText::WriteString(helloString);

  for (;;) {
    benchmark_counter++;

    VideoText::SetPos(100, 20);
    char text[20];
    sprintf(text, "%.0f ops/s", 1000 * (float)benchmark_counter / sinceStart);
    VideoText::WriteString(text);
  }
}


void setup() {
  Core::Init();

  benchmark();
}


void loop() {}
