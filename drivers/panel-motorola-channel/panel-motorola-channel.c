// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 Nicholas Velten
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct channel_djn569 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
};

static inline struct channel_djn569 *to_channel_djn569(struct drm_panel *panel)
{
	return container_of_const(panel, struct channel_djn569, panel);
}

static void channel_djn569_reset(struct channel_djn569 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
}

static int channel_djn569_on(struct channel_djn569 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x23);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x07, 0x20);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x08, 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x09, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x24);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x5e, 0x11);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x3b, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x5b, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x93, 0x08);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0a, 0x1c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x20, 0x1c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x86, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x80, 0x09);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x81, 0x08);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x82, 0x09);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x83, 0x08);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x84, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x25);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x4c, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x5e, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x20, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x27, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x34, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x43, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x50, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x62, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0a, 0x82);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0b, 0x9e);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0c, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x26);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x64, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x83, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x68, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x74, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x7b, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x87, 0x8c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x10);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x51, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x53, 0x24);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x55, 0x01);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 101);

	return dsi_ctx.accum_err;
}

static int channel_djn569_off(struct channel_djn569 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 100);

	return dsi_ctx.accum_err;
}

static int channel_djn569_prepare(struct drm_panel *panel)
{
	struct channel_djn569 *ctx = to_channel_djn569(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	channel_djn569_reset(ctx);

	ret = channel_djn569_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	return 0;
}

static int channel_djn569_unprepare(struct drm_panel *panel)
{
	struct channel_djn569 *ctx = to_channel_djn569(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = channel_djn569_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	return 0;
}

static const struct drm_display_mode channel_djn569_mode = {
	.clock = (720 + 220 + 12 + 72) * (1512 + 8 + 2 + 8) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 220,
	.hsync_end = 720 + 220 + 12,
	.htotal = 720 + 220 + 12 + 72,
	.vdisplay = 1512,
	.vsync_start = 1512 + 8,
	.vsync_end = 1512 + 8 + 2,
	.vtotal = 1512 + 8 + 2 + 8,
	.width_mm = 64,
	.height_mm = 128,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int channel_djn569_get_modes(struct drm_panel *panel,
				struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &channel_djn569_mode);
}

static const struct drm_panel_funcs channel_djn569_panel_funcs = {
	.prepare = channel_djn569_prepare,
	.unprepare = channel_djn569_unprepare,
	.get_modes = channel_djn569_get_modes,
};

static int channel_djn569_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

// TODO: Check if /sys/class/backlight/.../actual_brightness actually returns
// correct values. If not, remove this function.
static int channel_djn569_bl_get_brightness(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return brightness & 0xff;
}

static const struct backlight_ops channel_djn569_bl_ops = {
	.update_status = channel_djn569_bl_update_status,
	.get_brightness = channel_djn569_bl_get_brightness,
};

static struct backlight_device *
channel_djn569_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &channel_djn569_bl_ops, &props);
}

static int channel_djn569_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct channel_djn569 *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct channel_djn569, panel,
				   &channel_djn569_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = channel_djn569_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void channel_djn569_remove(struct mipi_dsi_device *dsi)
{
	struct channel_djn569 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id channel_djn569_of_match[] = {
	{ .compatible = "motorola,channel-panel" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, channel_djn569_of_match);

static struct mipi_dsi_driver channel_djn569_driver = {
	.probe = channel_djn569_probe,
	.remove = channel_djn569_remove,
	.driver = {
		.name = "panel-motorola-channel",
		.of_match_table = channel_djn569_of_match,
	},
};
module_mipi_dsi_driver(channel_djn569_driver);

MODULE_AUTHOR("Nicholas Velten <nikvelten@gmail.com>");
MODULE_DESCRIPTION("DRM driver for mipi_mot_vid_djn_hd_569");
MODULE_LICENSE("GPL");
