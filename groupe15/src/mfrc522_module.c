// SPDX-License-Identifier: GPL-2.0

#include <linux/errno.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>

#include "mfrc522_module.h"
#include "mfrc522_spi.h"

#define MFRC522_ID_SIZE 10

static struct mfrc522 *device;
static int mfrc522_spi_probe(struct spi_device *spi);
static int buffer_full;

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("DaemonOnUnix & Shebour");
MODULE_DESCRIPTION("Driver MFRC522 DRIL");

int isnumber(char c)
{
	return c >= '0' && c <= '9';
}

int startswith(const char *src, const char *substr)
{
	int i = 0;

	while (src[i] && substr[i]) {
		if (src[i] != substr[i])
			return 0;
		i++;
	}
	return substr[i] == '\0';
}

int parse_cmd_write(const char *cmd, int *len, const char **data)
{
	// <len>:<data> with len <= 25 else truncate
	char current_char;
	int pos = 0;
	int len_complete = 0;
	int charCount = 0; // protects from integer overflow

	*len = 0;

	for (; cmd[pos]; pos++) {
		current_char = cmd[pos];
		if (current_char == ':') {
			len_complete = 1;
			if (cmd[pos + 1] == '\0')
				return -EINVAL;
			*data = cmd + pos + 1;
			break;
		}
		if (!len_complete) {
			if (isnumber(current_char)) {
				if (charCount <= 2) {
					*len = *len * 10 + (current_char - '0');
					charCount++;
				}
			} else
				return -EINVAL;
		}
	}
	if (*len > 25)
		*len = 25;
	return 0;
}

void mem_write(const char *data)
{
	if (mfrc522_fifo_write(data, 25) < 0)
		pr_err("[MFRC522] Couldn't write to FIFO\n");

	mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON,
			MFRC522_COMMAND_REG_POWER_DOWN_OFF,
			MFRC522_COMMAND_MEM);

	pr_info("[MFRC522] Wrote data to memory\n");
}

int mem_read(char *answer)
{
	int byte_amount = 0;

	mfrc522_fifo_flush();
	if (mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON,
				MFRC522_COMMAND_REG_POWER_DOWN_OFF,
				MFRC522_COMMAND_MEM) < 0)
		return -1;

	byte_amount = mfrc522_fifo_read(answer);
	if (byte_amount < 0) {
		pr_err("[MFRC522] An error happened when reading MFRC522's internal memory\n");
		return -1;
	}

	return byte_amount;
}

int generate_random(void)
{
	u8 buffer[25] = { 0 };
	char char_buffer[MFRC522_ID_SIZE * 2 + 1] = { 0 };
	int i = 0;

	mem_write(buffer);

	if (mfrc522_send_command(MFRC522_COMMAND_REG_RCV_ON,
				MFRC522_COMMAND_REG_POWER_DOWN_OFF,
				MFRC522_COMMAND_GENERATE_RANDOM_ID) < 0)
		return -1;

	mem_read(buffer);

	for (i = 0; i < MFRC522_ID_SIZE; i++)
		sprintf(char_buffer + i * 2, "%02X", buffer[i]);

	pr_info("[MFRC522] Rand ID: %s\n", char_buffer);

	return 0;
}

void read_cmd(struct mfrc522 *dev, const char *cmd)
{
	int len;
	const char buffer[25] = { 0 };
	const char *data;
	int ret;
	size_t i = 0;
	char *dbg;

	if (startswith(cmd, "mem_write:")) {
		ret = parse_cmd_write(cmd + 10, &len, &data);
		if (ret < 0) {
			pr_err("[MFRC522] Invalid write command\n");
			return;
		}
		memcpy((void *)buffer, data, len);
		mem_write(buffer);
		if (device->debug) {
			pr_info("[MFRC522] WR");
			dbg = kmalloc(26, GFP_KERNEL);
			memset(dbg, 0, 26);
			memcpy(dbg, data, len);
			for (; i < 25; i += 5)
				pr_info("[MFRC522] %02x %02x %02x %02x %02x\n", dbg[i], dbg[i+1], dbg[i+2], dbg[i+3], dbg[i+4]);
			kfree(dbg);
		}
		return;
	}

	if (startswith(cmd, "mem_read")) {
		ret = mem_read(dev->str);
		pr_info("[MFRC522] \"%s\"\n", dev->str);
		return;
	}

	if (startswith(cmd, "gen_rand_id")) {
		ret = generate_random();
		return;
	}

	if (startswith(cmd, "debug:")) {
		cmd += 6;
		if (!strcmp(cmd, "on")) {
			device->debug = 1;
			pr_info("[MFRC522] Debug on\n");
		} else if (!strcmp(cmd, "off")) {
			device->debug = 0;
			pr_info("[MFRC522] Debug off\n");
		}
		return;
	}
	if (startswith(cmd, "version")) {
		u8 version = mfrc522_get_version();

		if (version == 0x91 || version == 0x92)
			pr_info("[MFRC522] version %d\n", version - 0x90);
		return;
	}
	pr_err("[MFRC522] Unknown command !\n");
}


ssize_t driver_write(struct file *file, const char *buffer, size_t len,
		loff_t *offset)
{
	struct mfrc522 *dev;
	char k_buf[26] = { 0 };

	if (copy_from_user(k_buf, buffer, len))
		pr_err("[MFRC522] Copy from user fail\n");
	dev = container_of(file->private_data, struct mfrc522, misc);
	read_cmd(dev, k_buf);
	buffer_full = 1;
	return len;
}

static ssize_t driver_read(struct file *file, char *buffer, size_t len,
		loff_t *offset)
{
	struct mfrc522 *dev;

	dev = container_of(file->private_data, struct mfrc522, misc);

	if (len > 25)
		len = 25;

	if (!buffer_full)
		return 0;

	if (copy_to_user(buffer, dev->str, len))
		pr_err("[MFRC522] Copy to user fail\n");
	buffer_full = 0;

	return len;
}

static const struct file_operations mfrc522_fops = {
	.owner = THIS_MODULE,
	.write = driver_write,
	.read = driver_read,
};

static const struct of_device_id mfrc522_match_table[] = {
	{ .compatible = "mfrc522" },
	{}
};


static struct spi_driver mfrc522_spi_driver = {
	.driver = {
		.name = "mfrc522",
		.owner = THIS_MODULE,
		.of_match_table = mfrc522_match_table,
	},
	.probe = mfrc522_spi_probe,
};

static int mfrc522_detect(struct spi_device *client)
{
	u8 version = mfrc522_get_version();

	if (version == 0x91 || version == 0x92) {
		version = version - 0x90;
		pr_info("[MFRC522] version %d\n", version);
		return version;
	}

	pr_err("[MFRC522] Bad version: 0x%x\n", version);
	return -1;
}

static int mfrc522_spi_probe(struct spi_device *client)
{
	pr_info("[MFRC522] SPI found\n");

	if (client->max_speed_hz > MFRC522_SPI_MAX_CLOCK_SPEED)
		client->max_speed_hz = MFRC522_SPI_MAX_CLOCK_SPEED;

	mfrc522_spi = client;

	return mfrc522_detect(client) < 0;
}

static int __init mfrc522_init(void)
{
	int ret;

	buffer_full = 0;

	pr_info("[MFRC522] init\n");

	device = kcalloc(1, sizeof(*device), GFP_KERNEL);

	if (!device)
		return -ENOMEM;

	device->misc = (struct miscdevice){
		.minor = MISC_DYNAMIC_MINOR,
			.name = "mfrc522",
			.fops = &mfrc522_fops,
	};

	device->debug = false;

	ret = misc_register(&device->misc);
	if (ret) {
		pr_err("[MFRC522] Misc device initialization failed\n");
		return ret;
	}

	ret = spi_register_driver(&mfrc522_spi_driver);
	if (ret) {
		pr_err("[MFRC522] SPI Register failed\r\n");
		return ret;
	}

	dev_set_drvdata(device->misc.this_device, device);

	return 0;
}

static void __exit mfrc522_exit(void)
{
	misc_deregister(&device->misc);
	spi_unregister_driver(&mfrc522_spi_driver);

	kfree(device);

	pr_info("[MFRC522] exit\n");
}

module_init(mfrc522_init);
module_exit(mfrc522_exit);
