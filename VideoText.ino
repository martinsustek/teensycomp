/**
 * TODO:
 * complete font
 * effects (shadows, rotations,...)
 * printf-style methods
 */

#include "VideoText.h"
#include "VideoTextFont.h"

uint16_t VideoText::posX;
uint8_t VideoText::posY;
uint16_t VideoText::colorForeground;
uint16_t VideoText::colorBackground;
uint8_t VideoText::attributes;

void VideoText::Init() {
  SetPos(0,0);
  SetColor(0xFFFF, 0x0000);
  SetAttributes(VideoTextAttribute_none);
}

void VideoText::SetColor(uint16_t foreground, uint16_t background) {
  colorForeground = foreground;
  colorBackground = background;
}

void VideoText::SetAttributes(uint8_t attributes) {
  VideoText::attributes = attributes;
}

void VideoText::SetPos(uint16_t x, uint8_t y) {
  posX = x;
  posY = y;
}

void VideoText::NextLine() {
  posX = 0;
  posY += fontHeight;
  if (posY >= 200) {
    posY = 0; // TODO: scroll instead?
  }
}

void VideoText::NextChar() {
  posX += fontWidth;
  if (posX >= 318) {
    NextLine();
  }
}

void VideoText::DrawChar(const char ch) {
  for (uint8_t column = 0; column < 6; column++) {
    uint8_t data = pgm_read_byte(&(font[(int)ch][column]));
    for (uint8_t row = 0; row < 8; row++) {
      if ((data >> row) & 1) {
        Video::SetPixel(posX + column, posY + row, colorForeground);
      } else {
        if (!(attributes & VideoTextAttribute_transparentBackground)) {
          Video::SetPixel(posX + column, posY + row, colorBackground);
        }
      }
    }
  }
}

void VideoText::WriteChar(const char ch) {
  if (ch == '\n') {
    NextLine();
  } else {
    DrawChar(ch);
    NextChar();
  }
}

void VideoText::WriteString(const char* ch) {
  while (*ch != 0) {
    WriteChar(*ch);
    ch++;
  }  
}

