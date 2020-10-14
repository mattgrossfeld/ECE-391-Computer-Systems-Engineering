#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define RTC_IRQ_NUM			0x08
#define MASK_LOW			0x0F
#define MASK_HIGH			0xF0

#define RTC_COMMAND_PORT 	0x70
#define RTC_DATA_PORT		0x71

#define STATUS_REG_A		0x8A
#define STATUS_REG_B		0x8B
#define STATUS_REG_C		0x0C

// Initialize RTC
extern void rtc_init(void);
// Call the Interrupt Handler for RTC
extern void rtc_irq_handler(void);


//Open file for RTC driver.
extern int32_t rtc_open(const uint8_t* filename);

//Close file for RTC driver.
extern int32_t rtc_close(int32_t fd);

//Read file for RTC driver.
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

//Write file for RTC driver.
extern int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes);

//Sets the frequency for writing.
extern int32_t rtc_set_freq(int32_t frequency);

#endif
