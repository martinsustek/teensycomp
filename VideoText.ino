/**
 * TODO:
 * complete font
 * transparent background
 * effects (shadows, rotations,...)
 * printf-style methods
 * PROGMEM string printing
 */

#include "VideoTextFont.h"

const uint8_t VideoText_fontWidth = 6;
const uint8_t VideoText_fontHeight = 8;

uint16_t VideoText_posX;
uint8_t VideoText_posY;
uint16_t VideoText_colorForeground;
uint16_t VideoText_colorBackground;

void VideoText_Init() {
  VideoText_SetPos(0,0);
  VideoText_SetColor(0xFFFF, 0x0000);
}

void VideoText_SetColor(uint16_t foreground, uint16_t background) {
  VideoText_colorForeground = foreground;
  VideoText_colorBackground = background;
}

void VideoText_SetPos(uint16_t x, uint8_t y) {
  VideoText_posX = x;
  VideoText_posY = y;
}

void VideoText_NextLine() {
  VideoText_posX = 0;
  VideoText_posY += VideoText_fontHeight;
  if (VideoText_posY >= 200) {
    VideoText_posY = 0; // TODO: scroll instead?
  }
}

void VideoText_NextChar() {
  VideoText_posX += VideoText_fontWidth;
  if (VideoText_posX >= 318) {
    VideoText_NextLine();
  }
}

void VideoText_DrawChar(const char ch) {
  for (uint8_t column = 0; column < 6; column++) {
    uint8_t data = pgm_read_byte(&(VideoText_font[(int)ch][column]));
    for (uint8_t row = 0; row < 8; row++) {
      uint16_t color = VideoText_colorBackground;
      if ((data >> row) & 1) {
        color = VideoText_colorForeground;
      }
      Video_SetPixel(VideoText_posX + column, VideoText_posY + row, color);
    }
  }
}

void VideoText_WriteChar(const char ch) {
  if (ch == '\n') {
    VideoText_NextLine();
  } else {
    VideoText_DrawChar(ch);
    VideoText_NextChar();
  }
}

void VideoText_WriteString(const char* ch) {
  while (*ch != 0) {
    VideoText_WriteChar(*ch);
    ch++;
  }  
}

