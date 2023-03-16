/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _CAMERA_IF_H_
#define _CAMERA_IF_H_

enum camera_get_param_type {
	GET_PARAM_SUPPORTED_FORMATS,
	GET_PARAM_FRAME_SIZES,
	GET_PARAM_SENSOR_INFO,
};

enum camera_set_param_type {
	SET_PARAM_FRAME_SIZE,
	SET_PARAM_BRIGHTNESS,
	SET_PARAM_CONTRAST,
	SET_PARAM_SATURATION,
	SET_PARAM_SHARPNESS,
	SET_PARAM_HUE,
	SET_PARAM_GAMMA,
	SET_PARAM_AUTO_WHITE_BALANCE,
	SET_PARAM_WHITE_BALANCE_TEMP,
	SET_PARAM_EXPOSURE,
	SET_PARAM_POWER_LINE_FREQ,
	SET_PARAM_MIN_FPS,
	SET_PARAM_MAX_FPS,
	SET_PARAM_EXPOSURE_AUTO,
	SET_PARAM_EXPOSURE_ABSOLUTE,
	SET_PARAM_FOCUS_AUTO,
	SET_PARAM_FOCUS_ABSOLUTE,
	SET_PARAM_SENSOR_ORIENTATION,
	SET_PARAM_EXPOSURE_REGION_LEFT,
	SET_PARAM_EXPOSURE_REGION_TOP,
	SET_PARAM_EXPOSURE_REGION_RIGHT,
	SET_PARAM_EXPOSURE_REGION_BOTTOM,
	SET_PARAM_EXPOSURE_WEIGHT,
	SET_PARAM_EXPOSURE_REGION_TRIGGER,
	SET_PARAM_ISO,
	SET_PARAM_FRAME_DURATION,
	SET_PARAM_HDR,
};

/**
 * struct camera_fmt_info - Used for GET_PARAM_SUPPORTED_FORMATS
 * @index: index of the v4l2 format, set by the driver
 * @v4l2_format: v4l2 format fourcc, set by the daemon
 * @valid: whether the information is valid, set by the daemon
 */
struct camera_fmt_info {
	unsigned int index;
	unsigned int v4l2_format;
	unsigned int valid;
};

/**
 * struct camera_res_info - Used for GET_PARAM_FRAME_SIZES
 * @index: index of the v4l2 format, set by the driver
 * @v4l2_format: v4l2 format fourcc, set by the driver
 * @width: resolution width, set by the daemon
 * @height: resolution height, set by the daemon
 * @valid: whether the information is valid, set by the daemon
 */
struct camera_res_info {
	unsigned int index;
	unsigned int v4l2_format;
	unsigned int width;
	unsigned int height;
	unsigned int valid;
};

/**
 * struct camera_sensor_info - Used for GET_PARAM_SENSOR_INFO
 * @type: sensor type (e.g. RAW, YUV), set by the daemon
 * @valid: whether the information is valid, set by the daemon
 */
struct camera_sensor_info {
	unsigned int type;
	unsigned int valid;
};

int  camera_if_init(void *ctx);

void camera_if_deinit(void *ctx);

int  camera_if_start_stream(void *ctx);

int  camera_if_stop_stream(void *ctx);

int  camera_if_init_buffer(void *ctx, void *fb);

int  camera_if_deinit_buffer(void *ctx, void *fb);
int  camera_if_capture(void *ctx, void *fb);

int  camera_if_get_param(void *ctx,
			enum camera_get_param_type type,
			void *out);

int  camera_if_set_param(void *ctx,
			enum camera_set_param_type type,
			void *in);

#endif /*_CAMERA_IF_H_*/
