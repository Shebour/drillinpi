/* SPDX-License-Identifier: GPL-2.0 */

#ifndef MFRC522_SPI_H
#define MFRC522_SPI_H

#include <linux/spi/spi.h>
#include <linux/types.h>
#include <linux/spi/spi.h>
#include <linux/compiler.h>

struct address_byte {
	u8 lsb : 1;
	u8 addr : 6;
	u8 rw : 1;
} __packed;

#define MFRC522_SPI_MAX_CLOCK_SPEED 1000000

#define MFRC522_SPI_WRITE 0
#define MFRC522_SPI_READ 1

#define MFRC522_COMMAND_IDLE 0b0000
#define MFRC522_COMMAND_MEM 0b0001
#define MFRC522_COMMAND_GENERATE_RANDOM_ID 0b0010
#define MFRC522_COMMAND_CALC_CRC 0b0011
#define MFRC522_COMMAND_TRANSMIT 0b0100
#define MFRC522_COMMAND_NO_CMD_CHANGE 0b0111
#define MFRC522_COMMAND_RECEIVE 0b1000
#define MFRC522_COMMAND_TRANSCEIVE 0b1100
#define MFRC522_COMMAND_MF_AUTHENT 0b1110
#define MFRC522_COMMAND_SOFT_RESET 0b1111

#define MFRC522_COMMAND_REG 0x1
#define MFRC522_FIFO_DATA_REG 0x9
#define MFRC522_FIFO_LEVEL_REG 0xA
#define MFRC522_VERSION_REG 0x37

#define MFRC522_COMMAND_REG_RCV_ON 0
#define MFRC522_COMMAND_REG_RCV_OFF 1
#define MFRC522_COMMAND_REG_POWER_DOWN_ON 1
#define MFRC522_COMMAND_REG_POWER_DOWN_OFF 0

extern struct spi_device *mfrc522_spi;

struct address_byte address_byte_build(u8 mode, u8 addr);
int mfrc522_get_version(void);
int mfrc522_fifo_level(void);
void mfrc522_fifo_flush(void);
int mfrc522_send_command(u8 rcv_off, u8 power_down, u8 command);
int mfrc522_read_command(void);
int mfrc522_fifo_read(u8 *buf);
int mfrc522_fifo_write(const u8 *buf, size_t len);
int mfrc522_register_read(struct spi_device *client, u8 reg, u8 *read_buff,
			  u8 read_len);
int mfrc522_register_write(struct spi_device *client, u8 reg, u8 value);

#endif /* !MFRC522_SPI_H */
