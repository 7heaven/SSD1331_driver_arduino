
#ifndef SSD1331_h
#define SSD1331_h

#if defined (__arm__) && defined (__SAM3X8E__)
    #include "SSD1331_arm.h"
#else
    #include "SSD1331_avr.h"
#endif

#endif