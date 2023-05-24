/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#ifndef _AR0430AP1302MIPI_SENSOR_CONTROL_H
#define _AR0430AP1302MIPI_SENSOR_CONTROL_H

#include <linux/firmware.h>
#include "adaptor-subdrv.h"

struct ar0430ap1302_sensor {
	struct ar0430ap1302_device *ar0430ap1302;
	// unsigned int index;

	// struct device_node *of_node;
	// struct device *dev;
	// unsigned int num_supplies;
	// struct regulator_bulk_data *supplies;

	// struct v4l2_subdev sd;
	// struct media_pad pad;
};

struct ar0430ap1302_device {
	struct device *dev;
	struct i2c_client *client;

	// struct gpio_desc *reset_gpio;
	// struct gpio_desc *standby_gpio;
	// struct clk *clock;
	u32 reg_page;

	const struct firmware *fw;

	// struct v4l2_fwnode_endpoint bus_cfg;

	// struct mutex lock;	/* Protects formats */

	// struct v4l2_subdev sd;
	// struct media_pad pads[AR0430AP1302_PAD_MAX];
	// struct ar0430ap1302_format formats[3];
	unsigned int width_factor;

	// struct v4l2_ctrl_handler ctrls;

	// const struct ar0430ap1302_sensor_info *sensor_info;
	struct ar0430ap1302_sensor sensors[2];

	struct {
		struct dentry *dir;
		struct mutex lock;
		u32 sipm_addr;
	} debugfs;

	// struct regulator *supply;

	struct subdrv_ctx *ctx;
	struct adaptor_ctx *ad_ctx;
};


int ar0430ap1302_request_firmware(struct ar0430ap1302_device *ar0430ap1302);
int ar0430ap1302_load_firmware(struct ar0430ap1302_device *ar0430ap1302);
int ar0430ap1302_stream(struct ar0430ap1302_device *ar0430ap1302, int enable);
int ar0430ap1302_reset(struct ar0430ap1302_device *ar0430ap1302);
int ar0430ap1302_log_status(struct ar0430ap1302_device *ar0430ap1302);
int ar0430ap1302_sensor_init(struct ar0430ap1302_device *ar0430ap1302);
int ar0430ap1302_remove(struct ar0430ap1302_device *ar0430ap1302);
#endif
