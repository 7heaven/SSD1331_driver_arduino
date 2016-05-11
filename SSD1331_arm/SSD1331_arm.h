#ifndef SSD1331_arm_h
#define SSD1331_arm_h

#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

#include "SSD1331_cmd.h"

class SSD1331{
	typedef unsigned char byte;

public:
	SSD1331(uint8_t cs, uint8_t rs, uint8_t sdin, uint8_t sclk, uint8_t rst);

    //oled初始化，使用oled前必须初始化才能使用
	void oled_init();

    //用特定颜色清屏
	void oled_clear(uint16_t color);

	//定位到坐标
	void oled_setposition(uint8_t x, uint8_t y);

    //绘制模式 分为填充或者边缘绘制
    void oled_drawmode(unsigned char mode);

    //绘制点
	void oled_pixel(uint8_t x, uint8_t y, uint16_t color);

	//绘制线
	void oled_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

	//绘制抗锯齿
	void oled_antialias_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

	//绘制圆
	void oled_circle(uint8_t cx, uint8_t cy, uint8_t radius, uint16_t color);

	//绘制矩形
	void oled_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);

	//
	void oled_polygon(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t count, uint16_t color);

	void oled_command(byte data);
	void oled_data(byte data);

	uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
	uint16_t color8882565(uint32_t rgb);

	static const int16_t TFTWIDTH = 96;
    static const int16_t TFTHEIGHT = 64;

private:
	int8_t _cs, _rs, _sclk, _sdin, _rst;
	unsigned char _mode;
	
	volatile uint8_t *csport, *rsport, *sclkport, *sdinport, *rstport;
    uint8_t cspinmask, rspinmask, sclkpinmask, sdinpinmask, rstpinmask;
};

#endif