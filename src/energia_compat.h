/* 
 * Compatibility header for Energia framework on TM4C123
 * Provides missing C standard library functions and Arduino API functions
 */

#ifndef _ENERGIA_COMPAT_H_
#define _ENERGIA_COMPAT_H_

#include <ctype.h>

/* Define missing character classification functions for WCharacter.h */
#ifndef isascii
#define isascii(c) (((unsigned char)(c)) < 128)
#endif

#ifndef toascii
#define toascii(c) (((unsigned char)(c)) & 0x7f)
#endif

/* 
 * Define missing digitalPinToInterrupt macro for TM4C123/Stellaris
 * For this platform, all GPIO pins support external interrupts
 * Map pin to interrupt number - for Energia on TM4C123, pin IS the interrupt
 */
#ifndef digitalPinToInterrupt
#define digitalPinToInterrupt(pin) (pin)
#endif

#endif /* _ENERGIA_COMPAT_H_ */

