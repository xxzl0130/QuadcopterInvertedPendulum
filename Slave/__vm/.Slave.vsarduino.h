/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Due (Programming Port), Platform=sam, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define printf iprintf
#define F_CPU 84000000L
#define ARDUINO 158
#define ARDUINO_SAM_DUE
#define ARDUINO_ARCH_SAM
#define __SAM3X8E__
#define USB_VID 0x2341
#define USB_PID 0x003e
#define USBCON
#define __cplusplus 201103L
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __ICCARM__
#define __ASM
#define __INLINE
//#define __GNUC__ 0
#define __GNUC__ 4
#define __GNUC_MINOR__ 100


#define __ICCARM__
#define __ARMCC_VERSION 400678
#define __attribute__(noinline)

#define prog_void
#define PGM_VOID_P int
            

extern "C" void __cxa_pure_virtual() {;}

typedef unsigned char byte;
typedef int __builtin_va_list;

//arduino.org
typedef long Pwm;
typedef long Adc;
typedef long Efc;
typedef long Pio;
typedef long Rstc;
typedef long Rtc;
typedef long Rtt;
typedef long Spi;
typedef long Ssc;
typedef long Tc;
typedef long Twi;
typedef long Usart;
typedef long Wdf;
typedef long Wdt;
typedef long Pdc;
typedef long caddr_t;


#define ADC_MR_TRGSEL_Pos 0
#define ADC_MR_TRGSEL_Msk 0

#define ADC_MR_TRGSEL_ADC_G0 0
#define ADC_MR_TRGSEL_ADC_G1 0
#define ADC_MR_TRGSEL_ADC_G2 0
#define ADC_MR_TRGSEL_ADC_G3 0

#define ADC_MR_TRGEN 0
#define ADC_MR_TRGEN_DIS 0
#define ADC_MR_TRGSEL_ADC_TRIG0 0
#define ADC_MR_TRGSEL_ADC_TRIG1 0
#define ADC_MR_TRGSEL_ADC_TRIG2 0
#define ADC_MR_TRGSEL_ADC_TRIG3 0




#include <arduino.h>
#include <pins_arduino.h> 
#include <variant.h> 
#undef F
#define F(string_literal) ((const PROGMEM char *)(string_literal))
#undef PSTR
#define PSTR(string_literal) ((const PROGMEM char *)(string_literal))
#undef cli
#define cli()
#define pgm_read_byte(address_short)
#define pgm_read_word(address_short)
#define pgm_read_word2(address_short)
#define digitalPinToPort(P)
#define digitalPinToBitMask(P) 
#define digitalPinToTimer(P)
#define analogInPinToBit(P)
#define portOutputRegister(P)
#define portInputRegister(P)
#define portModeRegister(P)
#include <..\Slave\Slave.ino>
#endif
