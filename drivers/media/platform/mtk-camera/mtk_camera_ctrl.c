// SPDX-License-Identifier: GPL-2.0
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

#include "mtk_camera_drv.h"
#include "mtk_camera_ctrl.h"

static int camera_set_ctrl(struct mtk_camera_ctx *ctx,
			  enum camera_set_param_type type, uint32_t value)
{
	int ret = 0;

	if (!ctx->cam_if_rdy) {
		ret = camera_if_init(ctx);
		if (ret) {
			dev_err(ctx->dev, "cam%d:[%d]: camera_if_init() fail ret=%d\n",
				ctx->camera_id, ctx->id, ret);
			return -EINVAL;
		}
	}
	camera_if_set_param(ctx, type, (void *)&value);
	dev_dbg(ctx->dev, "type:%d,value:%d cam_if_rdy:%d\n", type, value, ctx->cam_if_rdy);

	return ret;
}

static int mtk_camera_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct mtk_camera_ctx *ctx = ctrl_to_ctx(ctrl);
	int ret = 0;

	if (ctrl->flags & V4L2_CTRL_FLAG_INACTIVE)
		return 0;

	switch (ctrl->id) {

	case V4L2_CID_BRIGHTNESS:
		ret = camera_set_ctrl(ctx, SET_PARAM_BRIGHTNESS, ctrl->val);
		break;

	case V4L2_CID_CONTRAST:
		ret = camera_set_ctrl(ctx, SET_PARAM_CONTRAST, ctrl->val);
		break;

	case V4L2_CID_SATURATION:
		ret = camera_set_ctrl(ctx, SET_PARAM_SATURATION, ctrl->val);
		break;

	case V4L2_CID_SHARPNESS:
		ret = camera_set_ctrl(ctx, SET_PARAM_SHARPNESS, ctrl->val);
		break;

	case V4L2_CID_HUE:
		ret = camera_set_ctrl(ctx, SET_PARAM_HUE, ctrl->val);
		break;

	case V4L2_CID_GAMMA:
		ret = camera_set_ctrl(ctx, SET_PARAM_GAMMA, ctrl->val);
		break;

	case V4L2_CID_AUTO_WHITE_BALANCE:
		ret = camera_set_ctrl(ctx, SET_PARAM_AUTO_WHITE_BALANCE, ctrl->val);
		break;

	case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
		ret = camera_set_ctrl(ctx, SET_PARAM_WHITE_BALANCE_TEMP, ctrl->val);
		break;

	case V4L2_CID_EXPOSURE:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE, ctrl->val);
		break;

	case V4L2_CID_POWER_LINE_FREQUENCY:
		ret = camera_set_ctrl(ctx, SET_PARAM_POWER_LINE_FREQ, ctrl->val);
		break;

	case V4L2_CID_CAMERA_MIN_FPS:
		ret = camera_set_ctrl(ctx, SET_PARAM_MIN_FPS, ctrl->val);
		break;

	case V4L2_CID_CAMERA_MAX_FPS:
		ret = camera_set_ctrl(ctx, SET_PARAM_MAX_FPS, ctrl->val);
		break;

	case V4L2_CID_EXPOSURE_AUTO:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_AUTO, ctrl->val);
		break;

	case V4L2_CID_EXPOSURE_ABSOLUTE:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_ABSOLUTE, ctrl->val);
		break;

	case V4L2_CID_FOCUS_AUTO:
		ret = camera_set_ctrl(ctx, SET_PARAM_FOCUS_AUTO, ctrl->val);
		break;

	case V4L2_CID_FOCUS_ABSOLUTE:
		ret = camera_set_ctrl(ctx, SET_PARAM_FOCUS_ABSOLUTE, ctrl->val);
		break;

	case V4L2_CID_CAMERA_SENSOR_ORIENTATION:
		ret = camera_set_ctrl(ctx, SET_PARAM_SENSOR_ORIENTATION, ctrl->val);
		break;

	case V4L2_CID_CAMERA_EXPOSURE_REGION_LEFT:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_REGION_LEFT, ctrl->val);
		break;

	case V4L2_CID_CAMERA_EXPOSURE_REGION_TOP:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_REGION_TOP, ctrl->val);
		break;

	case V4L2_CID_CAMERA_EXPOSURE_REGION_RIGHT:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_REGION_RIGHT, ctrl->val);
		break;

	case V4L2_CID_CAMERA_EXPOSURE_REGION_BOTTOM:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_REGION_BOTTOM, ctrl->val);
		break;

	case V4L2_CID_CAMERA_EXPOSURE_WEIGHT:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_WEIGHT, ctrl->val);
		break;

	case V4L2_CID_CAMERA_EXPOSURE_REGION_TRIGGER:
		ret = camera_set_ctrl(ctx, SET_PARAM_EXPOSURE_REGION_TRIGGER, ctrl->val);
		break;

	case V4L2_CID_CAMERA_ISO:
		ret = camera_set_ctrl(ctx, SET_PARAM_ISO, ctrl->val);
		break;

	case V4L2_CID_CAMERA_FRAME_DURATION:
		ret = camera_set_ctrl(ctx, SET_PARAM_FRAME_DURATION, ctrl->val);
		break;

	case V4L2_CID_CAMERA_HDR:
		ret = camera_set_ctrl(ctx, SET_PARAM_HDR, ctrl->val);
		break;

	default:
		break;
	}

	return ret;
}

static const struct v4l2_ctrl_ops mtk_camera_ctrl_ops = {
	.s_ctrl = mtk_camera_s_ctrl,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_min_fps = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_MIN_FPS,
	.name = "Min Fps",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 5,
	.max = 30,
	.step = 1,
	.def = 30,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_max_fps = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_MAX_FPS,
	.name = "Max Fps",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 5,
	.max = 30,
	.step = 1,
	.def = 30,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_sensor_orientation = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_SENSOR_ORIENTATION,
	.name = "Sensor Orientation",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 3,
	.step = 1,
	.def = 0,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_exposure_region_bottom = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_EXPOSURE_REGION_BOTTOM,
	.name = "Exposure Region Bottom",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 2048,
	.step = 1,
	.def = 0,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_exposure_region_left = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_EXPOSURE_REGION_LEFT,
	.name = "Exposure Region Left",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 2048,
	.step = 1,
	.def = 0,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_exposure_region_top = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_EXPOSURE_REGION_TOP,
	.name = "Exposure Region Top",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 2048,
	.step = 1,
	.def = 0,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_exposure_region_right = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_EXPOSURE_REGION_RIGHT,
	.name = "Exposure Region Right",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 2048,
	.step = 1,
	.def = 0,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_exposure_weight = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_EXPOSURE_WEIGHT,
	.name = "Exposure Weight",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 1,
	.step = 1,
	.def = 0,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_exposure_region_trigger = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_EXPOSURE_REGION_TRIGGER,
	.name = "Exposure Region Trigger",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 1,
	.step = 1,
	.def = 0,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_iso = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_ISO,
	.name = "ISO",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 100,
	.max = 6400,
	.step = 100,
	.def = 100,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_frame_duration = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_FRAME_DURATION,
	.name = "Frame Duration",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 100000,
	.max = 100000000,
	.step = 1,
	.def = 33000000,
};

static const struct v4l2_ctrl_config mtk_camera_ctrl_hdr = {
	.ops = &mtk_camera_ctrl_ops,
	.id = V4L2_CID_CAMERA_HDR,
	.name = "HDR",
	.type = V4L2_CTRL_TYPE_INTEGER,
	.min = 0,
	.max = 4,
	.step = 1,
	.def = 3,
};

static int mtk_camera_ctrls_create(struct mtk_camera_ctx *ctx)
{
	int ret = 0;

	ret = v4l2_ctrl_handler_init(&ctx->ctrl_hdl, MTK_CAMERA_MAX_CTRLS_HINT);

	// ctx->ctrls.brightness = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_BRIGHTNESS,
	//					-1023, 1023, 1, 0);

	// ctx->ctrls.contrast = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_CONTRAST,
	//					0, 2000, 1, 100);

	// ctx->ctrls.saturation = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_SATURATION,
	//					0, 2, 1, 1);

	ctx->ctrls.sharpness = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_SHARPNESS,
						-10, 50, 1, 0);

	// ctx->ctrls.hue = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_HUE,
	//					0, 2, 1, 1);

	// ctx->ctrls.gamma = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_GAMMA,
	//					0, 255, 1, 0);

	ctx->ctrls.auto_white_balance = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_AUTO_WHITE_BALANCE,
						0, 1, 1, 1);

	// ctx->ctrls.do_white_balance = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_DO_WHITE_BALANCE,
	//					0, 255, 1, 0);

	ctx->ctrls.white_balance_temperature = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_WHITE_BALANCE_TEMPERATURE,
						2700, 6500, 1, 6500);

	ctx->ctrls.exposure = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_EXPOSURE,
						-40, 40, 1, 0);

	// ctx->ctrls.autogain = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_AUTOGAIN,
	//					0x0, 0xff, 1, 0);

	// ctx->ctrls.gain = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_GAIN,
	//					0, 255, 1, 32);

	ctx->ctrls.power_line_frequency = v4l2_ctrl_new_std_menu(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_POWER_LINE_FREQUENCY,
						V4L2_CID_POWER_LINE_FREQUENCY_AUTO, 0,
						V4L2_CID_POWER_LINE_FREQUENCY_AUTO);

	// ctx->ctrls.backlight_compensation = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_BACKLIGHT_COMPENSATION,
	//					0, 4, 1, 1);

	ctx->ctrls.exposure_auto = v4l2_ctrl_new_std_menu(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_EXPOSURE_AUTO,
						V4L2_EXPOSURE_MANUAL, 0,
						V4L2_EXPOSURE_AUTO);

	ctx->ctrls.exposure_absolute = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_EXPOSURE_ABSOLUTE,
						100000, 100000000, 1, 33000000);

	ctx->ctrls.focus_absolute = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_FOCUS_ABSOLUTE,
						0, 255, 1, 0);

	// ctx->ctrls.focus_relative = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_ops,
	//					V4L2_CID_FOCUS_RELATIVE,
	//					0, 255, 1, 0);

	ctx->ctrls.focus_auto = v4l2_ctrl_new_std(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_ops,
						V4L2_CID_FOCUS_AUTO,
						0, 1, 1, 0);

	// ctx->ctrls.sensor_orientation = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_sensor_orientation,
	//					NULL);

	ctx->ctrls.min_fps = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_min_fps,
						NULL);

	ctx->ctrls.max_fps = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_max_fps,
						NULL);

	ctx->ctrls.exposure_region_left = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_exposure_region_left,
						NULL);

	ctx->ctrls.exposure_region_top = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_exposure_region_top,
						NULL);

	ctx->ctrls.exposure_region_right = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_exposure_region_right,
						NULL);

	ctx->ctrls.exposure_region_bottom = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_exposure_region_bottom,
						NULL);

	ctx->ctrls.exposure_weight = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_exposure_weight,
						NULL);

	ctx->ctrls.exposure_region_trigger = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_exposure_region_trigger,
						NULL);

	if (ctx->ctrls.exposure_region_trigger == NULL)
		dev_err(ctx->dev, "Register ctrl exposure_region_trigger failed ret=%d\n",
			ctx->ctrl_hdl.error);
	else
		ctx->ctrls.exposure_region_trigger->flags |= V4L2_CTRL_FLAG_EXECUTE_ON_WRITE;


	ctx->ctrls.iso = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_iso,
						NULL);

	ctx->ctrls.frame_duration = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
						&mtk_camera_ctrl_frame_duration,
						NULL);

	// ctx->ctrls.hdr = v4l2_ctrl_new_custom(&ctx->ctrl_hdl,
	//					&mtk_camera_ctrl_hdr,
	//					NULL);

	ctx->ctrls_rdy = ctx->ctrl_hdl.error == 0;

	return 0;
}

int mtk_camera_ctrls_setup(struct mtk_camera_ctx *ctx)
{
	mtk_camera_ctrls_create(ctx);

	if (ctx->ctrl_hdl.error) {
		dev_err(ctx->dev, "adding control failed %d\n",
			ctx->ctrl_hdl.error);
		v4l2_ctrl_handler_free(&ctx->ctrl_hdl);
		dev_err(&ctx->stream->plat_dev->dev,
			"Failed to create control handlers\n");
		return ctx->ctrl_hdl.error;
	}

	ctx->stream->video.ctrl_handler = &ctx->ctrl_hdl;

	v4l2_ctrl_handler_setup(&ctx->ctrl_hdl);

	return 0;
}
