#ifndef __h
#define __h

class Video {
  public:
    static void DrawScanline();
    static void Init();
    static void ClearScreen();
    static void TestPattern();
    static void inline SetPixel(uint16_t x, uint8_t y, uint16_t color); 

  private:
    static IntervalTimer timer;
  
    static const uint16_t width = 320;
    static const uint8_t height = 200;
    static uint16_t buffer[width * height];
    
    static const uint16_t dmaBufferLineSize = width;
    static const uint8_t dmaBufferLineSizeStart = 55; // for time stabilization??
    static const uint8_t dmaBufferLineSizeStop = 1; // for reseting output back to zero
    
    static uint32_t dmaBuffer[dmaBufferLineSizeStart + dmaBufferLineSize + dmaBufferLineSizeStop];
    
    static const uint16_t scanlines = 449;
    static uint16_t currentScanlineNumber;
    static uint32_t startScanlineCyccnt;
    static uint32_t startSyncCyccnt;
    static uint32_t startBPorchCyccnt;
    static uint32_t startImageCyccnt;
    
    static const int vSyncPin = 3;
    static const int hSyncPin = 4;
};

#endif
