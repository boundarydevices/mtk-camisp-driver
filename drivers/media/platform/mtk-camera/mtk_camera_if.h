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
	GET_PARAM = 0
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
