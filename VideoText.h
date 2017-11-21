#ifndef __VideoText_h
#define __VideoText_h

enum VideoTextAttribute {
  VideoTextAttribute_none = 0,
  VideoTextAttribute_transparentBackground = 1
};

class VideoText {
  public:
    static void Init();
    static void SetColor(uint16_t foreground, uint16_t background);
    static void SetAttributes(uint8_t attributes);
    static void SetPos(uint16_t x, uint8_t y);
    static void DrawChar(const char ch);
    static void WriteChar(const char ch);
    static void WriteString(const char* ch);

  private:
    static const unsigned char font[][6];
    static const uint8_t fontWidth = 6;
    static const uint8_t fontHeight = 8;

    static uint16_t posX;
    static uint8_t posY;
    static uint16_t colorForeground;
    static uint16_t colorBackground;
    static uint8_t attributes;
  
    static void NextLine();
    static void NextChar();
};

#endif
