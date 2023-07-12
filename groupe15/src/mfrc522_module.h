/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_MODULE_H
#define MFRC522_MODULE_H

#include <linux/types.h>
#include <linux/miscdevice.h>

/**
 * The mfrc522_statistics structure keeps track of the amounts of bytes written and read
 * by the MFRC522 driver
 */

/**
 * Keep global information about the MFRC522 driver. This includes the answer buffer,
 * in which the MFRC522's memory content shall be kept between writes and reads, as well
 * as statistics and information
 */
struct mfrc522 {
	struct miscdevice misc;
	char str[26];
	bool debug;
};

#endif /* ! MFRC522_MODULE_H */
