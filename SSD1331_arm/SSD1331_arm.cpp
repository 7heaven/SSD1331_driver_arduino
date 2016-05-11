#include "SSD1331_arm.h"

SSD1331::SSD1331(uint8_t cs, uint8_t rs, uint8_t sdin, uint8_t sclk, uint8_t rst){
	_cs = cs;
	_rs = rs;
	_sdin = sdin;
	_sclk = sclk;
	_rst = rst;

	_mode = SSD1331_DRAW_STROKE;
}

void SSD1331::oled_init(){

	//方向控制寄存器切换为输出模式
	pinMode(_cs, OUTPUT);
	pinMode(_rs, OUTPUT);
	pinMode(_sclk, OUTPUT);
	pinMode(_sdin, OUTPUT);
	pinMode(_rst, OUTPUT);

	digitalWrite(_rst, HIGH);
	delay(500);
	digitalWrite(_rst, LOW);
	delay(500);
	digitalWrite(_rst, HIGH);
	delay(500);

	//initialization sequence
	oled_command(SSD1331_CMD_DISPLAYOFF);
	oled_command(SSD1331_CMD_SETREMAP);
#if defined SSD1331_COLORORDER_RGB
	oled_command(0x72);
#else
	oled_command(0x76);
#endif
    oled_command(SSD1331_CMD_STARTLINE); 	// 0xA1
    oled_command(0x0);
    oled_command(SSD1331_CMD_DISPLAYOFFSET); 	// 0xA2
    oled_command(0x0);
    oled_command(SSD1331_CMD_NORMALDISPLAY);  	// 0xA4
    oled_command(SSD1331_CMD_SETMULTIPLEX); 	// 0xA8
    oled_command(0x3F);  			// 0x3F 1/64 duty
    oled_command(SSD1331_CMD_SETMASTER);  	// 0xAD
    oled_command(0x8E);
    oled_command(SSD1331_CMD_POWERMODE);  	// 0xB0
    oled_command(0x0B);
    oled_command(SSD1331_CMD_PRECHARGE);  	// 0xB1
    oled_command(0x31);
    oled_command(SSD1331_CMD_CLOCKDIV);  	// 0xB3
    oled_command(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    oled_command(SSD1331_CMD_PRECHARGEA);  	// 0x8A
    oled_command(0x64);
    oled_command(SSD1331_CMD_PRECHARGEB);  	// 0x8B
    oled_command(0x78);
    oled_command(SSD1331_CMD_PRECHARGEA);  	// 0x8C
    oled_command(0x64);
    oled_command(SSD1331_CMD_PRECHARGELEVEL);  	// 0xBB
    oled_command(0x3A);
    oled_command(SSD1331_CMD_VCOMH);  		// 0xBE
    oled_command(0x3E);
    oled_command(SSD1331_CMD_MASTERCURRENT);  	// 0x87
    oled_command(0x06);
    oled_command(SSD1331_CMD_CONTRASTA);  	// 0x81
    oled_command(0x91);
    oled_command(SSD1331_CMD_CONTRASTB);  	// 0x82
    oled_command(0x50);
    oled_command(SSD1331_CMD_CONTRASTC);  	// 0x83
    oled_command(0x7D);
    oled_command(SSD1331_CMD_DISPLAYON);	//--turn on oled panel   

}

void SSD1331::oled_clear(uint16_t color){
	unsigned char _pre_mode = _mode;
	_mode = SSD1331_DRAW_FILL;
	oled_rectangle(0, 0, TFTWIDTH, TFTHEIGHT, color);
	_mode = _pre_mode;
}

uint16_t SSD1331::color565(uint8_t r, uint8_t g, uint8_t b){
	uint16_t c;
    c = r >> 3;
    c <<= 6;
    c |= g >> 2;
    c <<= 5;
    c |= b >> 3;

    return c;
}

uint16_t SSD1331::color8882565(uint32_t rgb){
    uint8_t r = rgb >> 16 & 0xFF;
    uint8_t g = rgb >> 8 & 0xFF;
    uint8_t b = rgb & 0xFF;

    return color565(r, g, b);
}

void SSD1331::oled_drawmode(unsigned char mode){
	_mode = mode;
}

void SSD1331::oled_setposition(uint8_t x, uint8_t y){

	if(x >= TFTWIDTH || y >= TFTHEIGHT) return;

	oled_command(SSD1331_CMD_SETCOLUMN);
	oled_command(x);
	oled_command(TFTWIDTH - 1);

	oled_command(SSD1331_CMD_SETROW);
	oled_command(y);
	oled_command(TFTHEIGHT - 1);
}

void SSD1331::oled_pixel(uint8_t x, uint8_t y, uint16_t color){

	if(x < 0 || x >= TFTWIDTH || y < 0 || y >= TFTHEIGHT) return;

	oled_setposition(x, y);

	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);

	oled_data(color >> 8);
	oled_data(color);

	digitalWrite(_cs, HIGH);
}

void SSD1331::oled_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color){
  if ((y0 >= TFTHEIGHT) && (y1 >= TFTHEIGHT))
	return;
  if ((x0 >= TFTWIDTH) && (x1 >= TFTWIDTH))
	return;	
  if (x0 >= TFTWIDTH)
    x0 = TFTWIDTH - 1;
  if (y0 >= TFTHEIGHT)
    y0 = TFTHEIGHT - 1;
  if (x1 >= TFTWIDTH)
    x1 = TFTWIDTH - 1;
  if (y1 >= TFTHEIGHT)
    y1 = TFTHEIGHT - 1;
  
  oled_command(SSD1331_CMD_DRAWLINE);
  oled_command(x0);
  oled_command(y0);
  oled_command(x1);
  oled_command(y1);
  delay(SSD1331_DELAYS_HWLINE);  
  oled_command((uint8_t)((color >> 11) << 1));
  oled_command((uint8_t)((color >> 5) & 0x3F));
  oled_command((uint8_t)((color << 1) & 0x3F));
  delay(SSD1331_DELAYS_HWLINE); 
}

void SSD1331::oled_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color){
  // Bounds check
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT))
	return;
  // Y bounds check
  if (y+height > TFTHEIGHT)
  {
    height = TFTHEIGHT - y;
  }
  // X bounds check
  if (x+width > TFTWIDTH)
  {
    width = TFTWIDTH - x;
  }
  
  switch(_mode){
  	case SSD1331_DRAW_STROKE:{
  		oled_line(x, y, x + width, y, color);
  		oled_line(x, y, x, y + height, color);
  		oled_line(x + width, y, x + width, y + height, color);
  		oled_line(x, y + height, x + width, y + height, color);
  	}
  	    break;
  	case SSD1331_DRAW_FILL:{
  		// fill!
        oled_command(SSD1331_CMD_FILL);
        oled_command(0x01);
        oled_command(SSD1331_CMD_DRAWRECT);
        oled_command(x & 0xFF);							// Starting column
        oled_command(y & 0xFF);							// Starting row
        oled_command((x+width-1) & 0xFF);	// End column
        oled_command((y+height-1) & 0xFF);	// End row
  
        // Outline color
        oled_command((uint8_t)((color >> 11) << 1));
        oled_command((uint8_t)((color >> 5) & 0x3F));
        oled_command((uint8_t)((color << 1) & 0x3F));
        // Fill color
        oled_command((uint8_t)((color >> 11) << 1));
        oled_command((uint8_t)((color >> 5) & 0x3F));
        oled_command((uint8_t)((color << 1) & 0x3F));
 
        // Delay while the fill completes
        delay(SSD1331_DELAYS_HWFILL); 
  	}
  	    break;
  }
}

void SSD1331::oled_polygon(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t count, uint16_t color){
	uint8_t centerX = width / 2;
	uint8_t centerY = height / 2;
	uint8_t radius = centerX > centerY ? centerY : centerX;

	centerX += x;
	centerY += y;

	uint8_t i;
	float angleStep = 6.28 / (float) count;
	float currentAngle = 0;
	uint8_t latestX = centerX + (uint8_t) (cos(currentAngle) * radius);
	uint8_t latestY = centerY + (uint8_t) (sin(currentAngle) * radius);

	uint8_t firstX = latestX;
	uint8_t firstY = latestY;

	for(i = 0; i < count; i++){
		currentAngle += angleStep;

		uint8_t currX = i == count - 1 ? firstX : centerX + (uint8_t) (cos(currentAngle) * radius);
		uint8_t currY = i == count - 1 ? firstY : centerY + (uint8_t) (sin(currentAngle) * radius);

		oled_line(latestX, latestY, currX, currY, color);

		latestX = currX; 
		latestY = currY;
	}
}

void SSD1331::oled_command(byte data){
	byte i;

	digitalWrite(_rs, LOW);
	digitalWrite(_cs, LOW);

	for(i = 0; i < 8; i++){
		digitalWrite(_sclk, LOW);
		if(data & 0x80){
			digitalWrite(_sdin, HIGH);
		}else{
			digitalWrite(_sdin, LOW);
		}
		digitalWrite(_sclk, HIGH);
		data <<= 1;
	}

	digitalWrite(_cs, HIGH);
	digitalWrite(_rs, HIGH);

}

void SSD1331::oled_data(byte data){
	byte i;

	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);

	for(i = 0; i < 8; i++){
		digitalWrite(_sclk, LOW);
		if(data & 0x80){
			digitalWrite(_sdin, HIGH);
		}else{
			digitalWrite(_sdin, LOW);
		}
		digitalWrite(_sclk, HIGH);
		data <<= 1;
	}

	digitalWrite(_cs, HIGH);
	digitalWrite(_rs, HIGH);

}


