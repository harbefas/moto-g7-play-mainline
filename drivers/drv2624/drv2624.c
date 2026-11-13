// SPDX-License-Identifier: GPL-2.0-only
/*
 * TI DRV2624 haptics driver (RTP mode, LRA)
 *
 * Register layout from Motorola's downstream drv2624_mmi driver.
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regmap.h>

#define DRV2624_ID		0x00
#define DRV2624_MODE		0x07
#define DRV2624_GO		0x0c
#define DRV2624_RTP_INPUT	0x0e
#define DRV2624_RATED_VOLTAGE	0x1f
#define DRV2624_OD_CLAMP	0x20

#define DRV2624_ID_MASK		0xf0
#define DRV2624_MODE_RTP	0x00
#define DRV2624_TRIG_INT	(0x02 << 2)

/* Moto G7 Play LRA (downstream ti,rated_voltage / ti,overdrive_voltage) */
#define DRV2624_RATED_DEFAULT	0x49
#define DRV2624_OD_DEFAULT	0x9a

struct drv2624_data {
	struct input_dev *input_dev;
	struct i2c_client *client;
	struct regmap *regmap;
	struct work_struct work;
	u8 level;
};

static void drv2624_worker(struct work_struct *work)
{
	struct drv2624_data *haptics =
				container_of(work, struct drv2624_data, work);
	int error;

	if (haptics->level) {
		error = regmap_write(haptics->regmap, DRV2624_RTP_INPUT,
				     haptics->level);
		if (!error)
			error = regmap_write(haptics->regmap, DRV2624_GO, 1);
	} else {
		error = regmap_write(haptics->regmap, DRV2624_GO, 0);
	}

	if (error)
		dev_err(&haptics->client->dev,
			"Failed to set vibration: %d\n", error);
}

static int drv2624_haptics_play(struct input_dev *input, void *data,
				struct ff_effect *effect)
{
	struct drv2624_data *haptics = input_get_drvdata(input);
	u16 magnitude = effect->u.rumble.strong_magnitude ?:
			effect->u.rumble.weak_magnitude;

	/* RTP input is signed 8-bit; map 0..0xffff to 0..0x7f */
	haptics->level = magnitude >> 9;
	schedule_work(&haptics->work);

	return 0;
}

static void drv2624_close(struct input_dev *input)
{
	struct drv2624_data *haptics = input_get_drvdata(input);

	cancel_work_sync(&haptics->work);
	regmap_write(haptics->regmap, DRV2624_GO, 0);
}

static const struct reg_sequence drv2624_init_regs[] = {
	{ DRV2624_MODE, DRV2624_TRIG_INT | DRV2624_MODE_RTP },
	{ DRV2624_RATED_VOLTAGE, DRV2624_RATED_DEFAULT },
	{ DRV2624_OD_CLAMP, DRV2624_OD_DEFAULT },
};

static const struct regmap_config drv2624_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0x30,
	.cache_type = REGCACHE_NONE,
};

static int drv2624_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct drv2624_data *haptics;
	struct gpio_desc *reset;
	unsigned int id;
	int error;

	haptics = devm_kzalloc(dev, sizeof(*haptics), GFP_KERNEL);
	if (!haptics)
		return -ENOMEM;

	reset = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(reset))
		return dev_err_probe(dev, PTR_ERR(reset),
				     "Failed to get reset gpio\n");
	if (reset) {
		usleep_range(1000, 2000);
		gpiod_set_value_cansleep(reset, 0);
		usleep_range(2000, 3000);
	}

	haptics->client = client;
	haptics->regmap = devm_regmap_init_i2c(client, &drv2624_regmap_config);
	if (IS_ERR(haptics->regmap))
		return dev_err_probe(dev, PTR_ERR(haptics->regmap),
				     "Failed to allocate register map\n");

	error = regmap_read(haptics->regmap, DRV2624_ID, &id);
	if (error)
		return dev_err_probe(dev, error, "Failed to read chip id\n");
	if (id & DRV2624_ID_MASK)
		return dev_err_probe(dev, -ENODEV,
				     "Unexpected chip id 0x%02x\n", id);

	error = regmap_multi_reg_write(haptics->regmap, drv2624_init_regs,
				       ARRAY_SIZE(drv2624_init_regs));
	if (error)
		return dev_err_probe(dev, error,
				     "Failed to write init registers\n");

	haptics->input_dev = devm_input_allocate_device(dev);
	if (!haptics->input_dev)
		return -ENOMEM;

	haptics->input_dev->name = "drv2624:haptics";
	haptics->input_dev->close = drv2624_close;
	input_set_drvdata(haptics->input_dev, haptics);
	input_set_capability(haptics->input_dev, EV_FF, FF_RUMBLE);

	error = input_ff_create_memless(haptics->input_dev, NULL,
					drv2624_haptics_play);
	if (error)
		return dev_err_probe(dev, error, "input_ff_create() failed\n");

	INIT_WORK(&haptics->work, drv2624_worker);
	i2c_set_clientdata(client, haptics);

	error = input_register_device(haptics->input_dev);
	if (error)
		return dev_err_probe(dev, error,
				     "Failed to register input device\n");

	dev_info(dev, "DRV2624 rev %u\n", id & 0x0f);

	return 0;
}

static const struct i2c_device_id drv2624_id[] = {
	{ "drv2624" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, drv2624_id);

static const struct of_device_id drv2624_of_match[] = {
	{ .compatible = "ti,drv2624", },
	{ }
};
MODULE_DEVICE_TABLE(of, drv2624_of_match);

static struct i2c_driver drv2624_driver = {
	.probe		= drv2624_probe,
	.driver		= {
		.name	= "drv2624-haptics",
		.of_match_table = drv2624_of_match,
	},
	.id_table = drv2624_id,
};
module_i2c_driver(drv2624_driver);

MODULE_DESCRIPTION("TI DRV2624 haptics driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nicholas Velten <nikvelten@gmail.com>");
