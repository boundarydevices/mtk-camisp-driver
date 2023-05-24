// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2023 MediaTek Inc.

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/types.h>
#include <linux/firmware.h>

#include "kd_camera_typedef.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define_v4l2.h"
#include "kd_imgsensor_errcode.h"

#include "ar0830ap1302mipiyuv_Sensor.h"
#include "ar0830ap1302_control.h"

#include "adaptor-subdrv.h"
#include "adaptor-i2c.h"

#define read_cmos_sensor_8(...) subdrv_i2c_rd_u8(__VA_ARGS__)
#define read_cmos_sensor_16(...) subdrv_i2c_rd_u16(__VA_ARGS__)
#define write_cmos_sensor_8(...) subdrv_i2c_wr_u8(__VA_ARGS__)
#define write_cmos_sensor_16(...) subdrv_i2c_wr_u16(__VA_ARGS__)

#define PFX "AR0830AP1302_camera_sensor"
#define LOG_ERR(format, args...)\
	pr_err(PFX "[%s] " format, __func__, ##args)
#define LOG_WARN(format, args...)\
	pr_warn(PFX "[%s] " format, __func__, ##args)
#define LOG_INFO(format, args...)\
	pr_info(PFX "[%s] " format, __func__, ##args)
#define LOG_DBG(format, args...)\
	pr_debug(PFX "[%s] " format, __func__, ##args)

#define AR0830AP1302_SUPPORTED_SENSOR_MODE_NUMBER 5
#define AR0830AP1302_LOG_STATUS_EN 0

static struct imgsensor_info_struct imgsensor_info = {
	.sensor_id = AR0830AP1302_CHIP_ID,
	.checksum_value = 0xffffffff,  // TODO: Need modify

	.pre = {
		.pclk = 610000000, // record different mode's pclk
		.linelength = 4800, // record different mode's linelength
		.framelength = 3180, // record different mode's framelength
		.startx = 0, // record different mode's startx of grabwindow
		.starty = 0, // record different mode's starty of grabwindow
		.grabwindow_width = 3200, // record different mode's width of grabwindow
		.grabwindow_height = 1800, // record different mode's height of grabwindow
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 610000000,
		.max_framerate = 300, // max_fps * 10
	},
	.cap = {
		.pclk = 610000000,
		.linelength = 4800,
		.framelength = 3180,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 3840,
		.grabwindow_height = 2160,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 610000000,
		.max_framerate = 250,
	},
	.cap1 = {
		.pclk = 610000000,
		.linelength = 4800,
		.framelength = 3180,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1920,
		.grabwindow_height = 1080,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 484000000,
		.max_framerate = 300,
	},
	.normal_video = {
		.pclk = 610000000,
		.linelength = 4800,
		.framelength = 3180,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1920,
		.grabwindow_height = 1080,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 610000000,
		.max_framerate = 300,
	},
	.hs_video = {
		.pclk = 610000000,
		.linelength = 4800,
		.framelength = 3180,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1920,
		.grabwindow_height = 1080,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 320000000,
		.max_framerate = 600,
	},
	.slim_video = {
		.pclk = 610000000,
		.linelength = 4800,
		.framelength = 3180,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1920,
		.grabwindow_height = 1080,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 160000000,
		.max_framerate = 300,
	},
	.margin = 6,
	.min_shutter = 6,
	.min_gain = BASEGAIN,
	.max_gain = BASEGAIN * 16,
	.min_gain_iso = 100,
	.exp_step = 2,
	.gain_step = 1,
	.gain_type = 0,
	.max_frame_length = 0x90f7,
	.ae_shut_delay_frame = 0,
	.ae_sensor_gain_delay_frame = 0,
	.ae_ispGain_delay_frame = 2,
	.ihdr_support = 0,    // 1, support; 0,not support
	.ihdr_le_firstline = 0,  // 1,le first ; 0, se first
	.sensor_mode_num = AR0830AP1302_SUPPORTED_SENSOR_MODE_NUMBER,    // support sensor mode num

	.cap_delay_frame = 4,
	.pre_delay_frame = 4,
	.video_delay_frame = 4,
	.hs_video_delay_frame = 4,
	.slim_video_delay_frame = 4,

	.isp_driving_current = ISP_DRIVING_2MA,
	.sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,
	.mipi_sensor_type = MIPI_OPHY_NCSI2, // 0,MIPI_OPHY_NCSI2; 1,MIPI_OPHY_CSI2
	.mipi_settle_delay_mode = MIPI_SETTLEDELAY_MANUAL, // 0,MIPI_SETTLEDELAY_AUTO; 1,MIPI_SETTLEDELAY_MANUAL
	.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_UYVY,
	.mclk = 48,
	.mipi_lane_num = SENSOR_MIPI_4_LANE,
	.i2c_addr_table = {0x78, 0x7a, 0xff},
	.i2c_addr_slave_sensor = {0x6c},
	.ar0830ap1302 = {
		.width_factor = AR0830AP1302_SLAVE_SENSOR_COUNT,
	},
};
/* Sensor output window information */
static struct SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[AR0830AP1302_SUPPORTED_SENSOR_MODE_NUMBER] = {
	{3840, 2160, 0, 0, 3200, 1800, 3200, 1800, 0, 0, 3200, 1800, 0, 0, 3200, 1800}, /* Preview */
	{3840, 2160, 0, 0, 3840, 2160, 3840, 2160, 0, 0, 3840, 2160, 0, 0, 3840, 2160}, /* capture */
	{3840, 2160, 0, 0, 1920, 1080, 1920, 1080, 0, 0, 1920, 1080, 0, 0, 1920, 1080}, /* video */
	{3840, 2160, 0, 0, 1920, 1080, 1920, 1080, 0, 0, 1920, 1080, 0, 0, 1920, 1080}, /*hight speed video */
	{3840, 2160, 0, 0, 1920, 1080, 1920, 1080, 0, 0, 1920, 1080, 0, 0, 1920, 1080} /* slim video  */
};

static struct SENSOR_VC_INFO_STRUCT SENSOR_VC_INFO[3] = {/* Preview mode setting */
	{0x01, 0x08, 0x00,   0x08, 0x40, 0x00,
	 0x00, 0x1e, 3200,   1800,
	 0x01, 0x1e, 3200,   1800,
	 0x02, 0x1e, 3200,   1800,
	 0x03, 0x1e, 3200,   1800 },
	/* Capture mode setting */
	{0x01, 0x08, 0x00,   0x08, 0x40, 0x00,
	 0x00, 0x1e, 3840,   2160,
	 0x01, 0x1e, 3840,   2160,
	 0x02, 0x1e, 3840,   2160,
	 0x03, 0x1e, 3840,   2160 },
	/* Video mode setting */
	{0x01, 0x08, 0x00,   0x08, 0x40, 0x00,
	 0x00, 0x1e, 1920,   1080,
	 0x01, 0x1e, 1920,   1080,
	 0x02, 0x1e, 1920,   1080,
	 0x03, 0x1e, 1920,   1080 }
};

static void sensor_init(struct subdrv_ctx *ctx)
{
	LOG_INFO("AR0830AP1302 init()");
	// ar0830ap1302_load_firmware
} /* sensor_init */

static void preview_setting(struct subdrv_ctx *ctx)
{
	struct ar0830ap1302_mode_info info = {0};

	LOG_INFO("%s E\n", __func__);

	info.width = imgsensor_info.pre.grabwindow_width;
	info.height = imgsensor_info.pre.grabwindow_height;
	info.target_fps = imgsensor_info.pre.max_framerate;
	ar0830ap1302_config_mode(&imgsensor_info.ar0830ap1302, &info);
}   /*  preview_setting  */

static void capture_setting(struct subdrv_ctx *ctx, kal_uint16 currefps)
{
	struct ar0830ap1302_mode_info info = {0};

	LOG_INFO("%s E\n", __func__);

	info.width = imgsensor_info.cap.grabwindow_width;
	info.height = imgsensor_info.cap.grabwindow_height;
	info.target_fps = imgsensor_info.cap.max_framerate;
	ar0830ap1302_config_mode(&imgsensor_info.ar0830ap1302, &info);
}

static void normal_video_setting(struct subdrv_ctx *ctx, kal_uint16 currefps)
{
	struct ar0830ap1302_mode_info info = {0};

	LOG_INFO("%s E\n", __func__);

	info.width = imgsensor_info.normal_video.grabwindow_width;
	info.height = imgsensor_info.normal_video.grabwindow_height;
	info.target_fps = imgsensor_info.normal_video.max_framerate;
	ar0830ap1302_config_mode(&imgsensor_info.ar0830ap1302, &info);
}

static void hs_video_setting(struct subdrv_ctx *ctx)
{
	LOG_INFO("E\n");
}

static void slim_video_setting(struct subdrv_ctx *ctx)
{
	LOG_INFO("E\n");
}

/*************************************************************************
 * FUNCTION
 * get_imgsensor_id
 *
 * DESCRIPTION
 * This function get the sensor ID
 *
 * PARAMETERS
 * *sensorID : return the sensor ID
 *
 * RETURNS
 * None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static int get_imgsensor_id(struct subdrv_ctx *ctx, UINT32 *sensor_id)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	//sensor have two i2c address 0x6c 0x6d & 0x21 0x20,
	//we should detect the module used i2c address
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		ctx->i2c_write_id = imgsensor_info.i2c_addr_table[i];
		do {
			*sensor_id = read_cmos_sensor_16(ctx, AR0830AP1302_REG_CHIP_ID);
			if (*sensor_id == imgsensor_info.sensor_id) {
				LOG_INFO("AR0830AP1302 i2c write id: 0x%x, sensor id: 0x%x\n",
					ctx->i2c_write_id, *sensor_id);
				return ERROR_NONE;
			}
			LOG_DBG("AR0830AP1302 Read sensor id fail, write id:0x%x id: 0x%x\n",
				ctx->i2c_write_id, *sensor_id);
			retry--;
		} while (retry > 0);
		i++;
		if (*sensor_id == imgsensor_info.sensor_id)
			break;
		retry = 2;
	}
	if (*sensor_id != imgsensor_info.sensor_id) {
		// if Sensor ID is not correct,
		// Must set *sensor_id to 0xFFFFFFFF
		LOG_ERR("Get AR0830AP1302 sensor id fail.");
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;
}

/*************************************************************************
 * FUNCTION
 * open
 *
 * DESCRIPTION
 * This function initialize the registers of CMOS sensor
 *
 * PARAMETERS
 * None
 *
 * RETURNS
 * None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static int open(struct subdrv_ctx *ctx)
{
	kal_uint32 sensor_id = 0;
	int ret;
	unsigned int retries;

	LOG_INFO("E\n");
	//sensor have two i2c address 0x6c 0x6d & 0x21 0x20,
	//we should detect the module used i2c address
	// LOG_INFO("AR0830AP1302 open().");
	if (get_imgsensor_id(ctx, &sensor_id) != ERROR_NONE) {
		LOG_ERR("AR0830AP1302 get_imgsensor_id() return error.");
		return ERROR_SENSOR_CONNECT_FAIL;
	}

	ar0830ap1302_sensor_init(&imgsensor_info.ar0830ap1302);
	// request firmware
	LOG_INFO("AR0830AP1302 request fw.");
	ret = ar0830ap1302_request_firmware(&imgsensor_info.ar0830ap1302);
	if (ret) {
		LOG_ERR("AR0830AP1302 Request Firmware Failed.");
		release_firmware(imgsensor_info.ar0830ap1302.fw);
		return ret;
	}
	for (retries = 0; retries < 5; ++retries) {
		LOG_DBG("Try to load f/w. AR0830AP1302 LINE = %d.", __LINE__);
		ret = ar0830ap1302_load_firmware(&imgsensor_info.ar0830ap1302);
		if (!ret)
			break;
		//ar0830ap1302_log_status(&imgsensor_info.ar0830ap1302);
		ar0830ap1302_reset(&imgsensor_info.ar0830ap1302);
		LOG_DBG("Retry. AR0830AP1302 LINE = %d.", __LINE__);
	}
	if (retries == 5) {
		LOG_DBG("Firmware load retries exceeded, aborting\n");
		release_firmware(imgsensor_info.ar0830ap1302.fw);
		ret = ERROR_DRIVER_INIT_FAIL;
		return ret;
	}
	/* initail sequence write in  */
	sensor_init(ctx);
	LOG_DBG("SensorInit. AR0830AP1302 LINE = %d.", __LINE__);

	ctx->autoflicker_en = KAL_FALSE;
	ctx->sensor_mode = IMGSENSOR_MODE_INIT;
	ctx->shutter = 0x3D0;
	ctx->gain = 0x100;
	ctx->pclk = imgsensor_info.pre.pclk;
	ctx->frame_length = imgsensor_info.pre.framelength;
	ctx->line_length = imgsensor_info.pre.linelength;
	ctx->min_frame_length = imgsensor_info.pre.framelength;
	ctx->dummy_pixel = 0;
	ctx->dummy_line = 0;
	ctx->ihdr_mode = 0;
	ctx->test_pattern = KAL_FALSE;
	ctx->current_fps = imgsensor_info.pre.max_framerate;
	release_firmware(imgsensor_info.ar0830ap1302.fw);
	LOG_DBG("Open() Done. AR0830AP1302 LINE = %d.", __LINE__);
	return ERROR_NONE;
} /* open  */

/*************************************************************************
 * FUNCTION
 * close
 *
 * DESCRIPTION
 *
 *
 * PARAMETERS
 * None
 *
 * RETURNS
 * None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static int close(struct subdrv_ctx *ctx)
{
	LOG_INFO("E\n");

	/*No Need to implement this function*/

	// Add for AR0830AP1302
	//release_firmware(imgsensor_info.ar0830ap1302.fw);
	ar0830ap1302_remove(&imgsensor_info.ar0830ap1302);
	return ERROR_NONE;
} /* close  */

/*************************************************************************
 * FUNCTION
 * preview
 *
 * DESCRIPTION
 * This function start the sensor preview.
 *
 * PARAMETERS
 * *image_window : address pointer of pixel numbers in one period of HSYNC
 *  *sensor_config_data : address pointer of line numbers in one period of VSYNC
 *
 * RETURNS
 * None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 preview(struct subdrv_ctx *ctx, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INFO("E\n");

	ctx->sensor_mode = IMGSENSOR_MODE_PREVIEW;
	ctx->pclk = imgsensor_info.pre.pclk;
	ctx->line_length = imgsensor_info.pre.linelength;
	ctx->frame_length = imgsensor_info.pre.framelength;
	ctx->min_frame_length = imgsensor_info.pre.framelength;
	ctx->autoflicker_en = KAL_TRUE;
	preview_setting(ctx);
	return ERROR_NONE;
} /* preview */

/*************************************************************************
 * FUNCTION
 * capture
 *
 * DESCRIPTION
 * This function setup the CMOS sensor in capture MY_OUTPUT mode
 *
 * PARAMETERS
 *
 * RETURNS
 * None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 capture(struct subdrv_ctx *ctx, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INFO("E\n");
	ctx->sensor_mode = IMGSENSOR_MODE_CAPTURE;
	if (ctx->current_fps == imgsensor_info.cap1.max_framerate) {
		ctx->pclk = imgsensor_info.cap1.pclk;
		ctx->line_length = imgsensor_info.cap1.linelength;
		ctx->frame_length = imgsensor_info.cap1.framelength;
		ctx->min_frame_length = imgsensor_info.cap1.framelength;
		ctx->autoflicker_en = KAL_TRUE;
	} else {
		if (ctx->current_fps != imgsensor_info.cap.max_framerate)
			LOG_WARN(
				"Warning: current_fps %u fps is not support, use: %u fps!\n",
				ctx->current_fps,
				imgsensor_info.cap.max_framerate / 10);
		ctx->pclk = imgsensor_info.cap.pclk;
		ctx->line_length = imgsensor_info.cap.linelength;
		ctx->frame_length = imgsensor_info.cap.framelength;
		ctx->min_frame_length = imgsensor_info.cap.framelength;
		ctx->autoflicker_en = KAL_TRUE;
	}
	capture_setting(ctx, ctx->current_fps);
	return ERROR_NONE;
}	/* capture(ctx) */

static kal_uint32 normal_video(struct subdrv_ctx *ctx,
		MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INFO("E\n");

	ctx->sensor_mode = IMGSENSOR_MODE_VIDEO;
	ctx->pclk = imgsensor_info.normal_video.pclk;
	ctx->line_length = imgsensor_info.normal_video.linelength;
	ctx->frame_length = imgsensor_info.normal_video.framelength;
	ctx->min_frame_length = imgsensor_info.normal_video.framelength;
	/* ctx->current_fps = 300; */
	ctx->autoflicker_en = KAL_TRUE;

	normal_video_setting(ctx, ctx->current_fps);
	return ERROR_NONE;
} /* normal_video   */

static kal_uint32 hs_video(struct subdrv_ctx *ctx, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INFO("E\n");

	ctx->sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
	ctx->pclk = imgsensor_info.hs_video.pclk;
	/* ctx->video_mode = KAL_TRUE; */
	ctx->line_length = imgsensor_info.hs_video.linelength;
	ctx->frame_length = imgsensor_info.hs_video.framelength;
	ctx->min_frame_length = imgsensor_info.hs_video.framelength;
	ctx->dummy_line = 0;
	ctx->dummy_pixel = 0;
	ctx->autoflicker_en = KAL_TRUE;
	hs_video_setting(ctx);

	return ERROR_NONE;
} /* hs_video   */

static kal_uint32 slim_video(struct subdrv_ctx *ctx,
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INFO("E\n");

	ctx->sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
	ctx->pclk = imgsensor_info.slim_video.pclk;
	ctx->line_length = imgsensor_info.slim_video.linelength;
	ctx->frame_length = imgsensor_info.slim_video.framelength;
	ctx->min_frame_length = imgsensor_info.slim_video.framelength;
	ctx->dummy_line = 0;
	ctx->dummy_pixel = 0;
	ctx->autoflicker_en = KAL_TRUE;
	slim_video_setting(ctx);

	return ERROR_NONE;
} /* slim_video  */
static int get_resolution(struct subdrv_ctx *ctx,
		MSDK_SENSOR_RESOLUTION_INFO_STRUCT *sensor_resolution)
{
	int i = 0;

	for (i = SENSOR_SCENARIO_ID_MIN; i < SENSOR_SCENARIO_ID_MAX; i++) {
		if (i < ARRAY_SIZE(imgsensor_winsize_info)) {
			sensor_resolution->SensorWidth[i] = imgsensor_winsize_info[i].w2_tg_size;
			sensor_resolution->SensorHeight[i] = imgsensor_winsize_info[i].h2_tg_size;
		} else {
			sensor_resolution->SensorWidth[i] = 0;
			sensor_resolution->SensorHeight[i] = 0;
		}
	}

	return ERROR_NONE;
} /* get_resolution */

static int get_info(struct subdrv_ctx *ctx, enum MSDK_SCENARIO_ID_ENUM scenario_id,
	MSDK_SENSOR_INFO_STRUCT *sensor_info,
	MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_DBG("scenario_id = %d\n", scenario_id);

	sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
	 /* not use */
	sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorHsyncPolarity =
		SENSOR_CLOCK_POLARITY_LOW; // inverse with datasheet
	sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorInterruptDelayLines = 4; /* not use */
	sensor_info->SensorResetActiveHigh = FALSE; /* not use */
	sensor_info->SensorResetDelayCount = 5; /* not use */

	sensor_info->SensroInterfaceType = imgsensor_info.sensor_interface_type;
	sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
	//sensor_info->SettleDelayMode = imgsensor_info.mipi_settle_delay_mode;
	sensor_info->SensorOutputDataFormat =
		imgsensor_info.sensor_output_dataformat;

	sensor_info->DelayFrame[SENSOR_SCENARIO_ID_NORMAL_PREVIEW] =
		imgsensor_info.pre_delay_frame;
	sensor_info->DelayFrame[SENSOR_SCENARIO_ID_NORMAL_CAPTURE] =
		imgsensor_info.cap_delay_frame;
	sensor_info->DelayFrame[SENSOR_SCENARIO_ID_NORMAL_VIDEO] =
		imgsensor_info.video_delay_frame;
	sensor_info->DelayFrame[SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO] =
		imgsensor_info.hs_video_delay_frame;
	sensor_info->DelayFrame[SENSOR_SCENARIO_ID_SLIM_VIDEO] =
		imgsensor_info.slim_video_delay_frame;

	sensor_info->SensorMasterClockSwitch = 0; /* not use */
	sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;

	/* The frame of setting shutter default 0 for TG int */
	sensor_info->AEShutDelayFrame =
		imgsensor_info.ae_shut_delay_frame;
	/* The frame of setting sensor gain */
	sensor_info->AESensorGainDelayFrame =
		imgsensor_info.ae_sensor_gain_delay_frame;
	sensor_info->AEISPGainDelayFrame =
		imgsensor_info.ae_ispGain_delay_frame;
	sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
	sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
	sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;

	sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num;
	sensor_info->SensorClockFreq = imgsensor_info.mclk;
	sensor_info->SensorClockDividCount = 3; /* not use */
	sensor_info->SensorClockRisingCount = 0;
	sensor_info->SensorClockFallingCount = 2; /* not use */
	sensor_info->SensorPixelClockCount = 3; /* not use */
	sensor_info->SensorDataLatchCount = 2; /* not use */

	sensor_info->SensorWidthSampling = 0;  // 0 is default 1x
	sensor_info->SensorHightSampling = 0; // 0 is default 1x
	sensor_info->SensorPacketECCOrder = 1;

	return ERROR_NONE;
}	/*	get_info  */

static int control(struct subdrv_ctx *ctx, enum MSDK_SCENARIO_ID_ENUM scenario_id,
	MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
	MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_DBG("scenario_id = %d\n", scenario_id);
	ctx->current_scenario_id = scenario_id;
	switch (scenario_id) {
	case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		preview(ctx, image_window, sensor_config_data);
		break;
	case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
		capture(ctx, image_window, sensor_config_data);
		break;
	case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
		normal_video(ctx, image_window, sensor_config_data);
		break;
	case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
		hs_video(ctx, image_window, sensor_config_data);
		break;
	case SENSOR_SCENARIO_ID_SLIM_VIDEO:
		slim_video(ctx, image_window, sensor_config_data);
		break;
	default:
		LOG_WARN("Error ScenarioId setting (id = %d)\n", scenario_id);
		preview(ctx, image_window, sensor_config_data);
		return ERROR_INVALID_SCENARIO_ID;
	}
	return ERROR_NONE;
} /* control(ctx) */

static kal_uint32 set_video_mode(struct subdrv_ctx *ctx, UINT16 framerate)
{
	LOG_DBG("framerate = %u\n ", framerate);
	return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(struct subdrv_ctx *ctx, kal_bool enable, UINT16 framerate)
{
	LOG_DBG("enable = %d, framerate = %u\n", enable, framerate);
	if (enable) //enable auto flicker
		ctx->autoflicker_en = KAL_TRUE;
	else //Cancel Auto flick
		ctx->autoflicker_en = KAL_FALSE;
	return ERROR_NONE;
}

static kal_uint32 set_max_framerate_by_scenario(struct subdrv_ctx *ctx,
		enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 framerate)
{
	kal_uint32 frame_length;

	LOG_DBG("scenario_id = %d, framerate = %u\n",
		scenario_id, framerate);

	switch (scenario_id) {
	case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		frame_length =
			imgsensor_info.pre.pclk /
			framerate * 10 /
			imgsensor_info.pre.linelength;
		ctx->dummy_line =
			(frame_length > imgsensor_info.pre.framelength) ?
			(frame_length - imgsensor_info.pre.framelength) :
			0;
		ctx->frame_length =
			imgsensor_info.pre.framelength + ctx->dummy_line;
		ctx->min_frame_length = ctx->frame_length;
		break;
	case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
		if (framerate == 0)
			return ERROR_NONE;
		frame_length =
			imgsensor_info.normal_video.pclk /
			framerate * 10 /
			imgsensor_info.normal_video.linelength;
		ctx->dummy_line =
			(frame_length > imgsensor_info.normal_video.framelength) ?
			(frame_length - imgsensor_info.normal_video.framelength) :
			0;
		ctx->frame_length =
			imgsensor_info.normal_video.framelength + ctx->dummy_line;
		ctx->min_frame_length = ctx->frame_length;
		break;
	case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
		if (ctx->current_fps ==
				imgsensor_info.cap1.max_framerate) {
			frame_length =
				imgsensor_info.cap1.pclk /
				framerate * 10 /
				imgsensor_info.cap1.linelength;
			ctx->dummy_line =
				(frame_length > imgsensor_info.cap1.framelength) ?
				(frame_length - imgsensor_info.cap1.framelength) :
				0;
			ctx->frame_length =
				imgsensor_info.cap1.framelength + ctx->dummy_line;
			ctx->min_frame_length = ctx->frame_length;
		} else {
			if (ctx->current_fps !=
				imgsensor_info.cap.max_framerate)
				LOG_WARN(
					"Warning: current_fps %u fps is not support,so use cap's setting: %u fps!\n",
					framerate,
					imgsensor_info.cap.max_framerate / 10);
				frame_length =
					imgsensor_info.cap.pclk /
					framerate * 10 /
					imgsensor_info.cap.linelength;
				ctx->dummy_line =
					(frame_length > imgsensor_info.cap.framelength) ?
					(frame_length - imgsensor_info.cap.framelength) :
					0;
				ctx->frame_length =
					imgsensor_info.cap.framelength + ctx->dummy_line;
				ctx->min_frame_length = ctx->frame_length;
		}
		break;
	case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
		frame_length =
			imgsensor_info.hs_video.pclk /
			framerate * 10 /
			imgsensor_info.hs_video.linelength;
		ctx->dummy_line =
			(frame_length > imgsensor_info.hs_video.framelength) ?
			(frame_length - imgsensor_info.hs_video.framelength) :
			0;
		ctx->frame_length =
			imgsensor_info.hs_video.framelength + ctx->dummy_line;
		ctx->min_frame_length = ctx->frame_length;
		break;
	case SENSOR_SCENARIO_ID_SLIM_VIDEO:
		frame_length =
			imgsensor_info.slim_video.pclk /
			framerate * 10 /
			imgsensor_info.slim_video.linelength;
		ctx->dummy_line =
			(frame_length > imgsensor_info.slim_video.framelength) ?
			(frame_length - imgsensor_info.slim_video.framelength) :
			0;
		ctx->frame_length =
			imgsensor_info.slim_video.framelength + ctx->dummy_line;
		ctx->min_frame_length = ctx->frame_length;
		break;
	default:  //coding with  preview scenario by default
		frame_length =
			imgsensor_info.pre.pclk /
			framerate * 10 /
			imgsensor_info.pre.linelength;
		ctx->dummy_line =
			(frame_length > imgsensor_info.pre.framelength) ?
			(frame_length - imgsensor_info.pre.framelength) :
			0;
		ctx->frame_length =
			imgsensor_info.pre.framelength + ctx->dummy_line;
		ctx->min_frame_length = ctx->frame_length;
		LOG_WARN("error scenario_id = %d, we use preview scenario\n",
			scenario_id);
		break;
	}
	return ERROR_NONE;
}

static kal_uint32 get_default_framerate_by_scenario(struct subdrv_ctx *ctx,
		enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 *framerate)
{
	LOG_DBG("scenario_id = %d\n", scenario_id);

	switch (scenario_id) {
	case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		*framerate = imgsensor_info.pre.max_framerate;
		break;
	case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
		*framerate = imgsensor_info.normal_video.max_framerate;
		break;
	case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
		*framerate = imgsensor_info.cap.max_framerate;
		break;
	case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
		*framerate = imgsensor_info.hs_video.max_framerate;
		break;
	case SENSOR_SCENARIO_ID_SLIM_VIDEO:
		*framerate = imgsensor_info.slim_video.max_framerate;
		break;
	default:
		break;
	}

	return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(struct subdrv_ctx *ctx, kal_bool enable)
{
	return ERROR_NONE;
}

static kal_uint32 streaming_control(struct subdrv_ctx *ctx, kal_bool enable)
{
	LOG_INFO("streaming_enable(0=Sw Standby,1=streaming): %d\n", enable);
	if (enable) {
		ar0830ap1302_stream(&imgsensor_info.ar0830ap1302, enable);
#if AR0830AP1302_LOG_STATUS_EN
		ar0830ap1302_log_status(&imgsensor_info.ar0830ap1302);
		ar0830ap1302_log_status(&imgsensor_info.ar0830ap1302);
		ar0830ap1302_log_status(&imgsensor_info.ar0830ap1302);
		ar0830ap1302_log_status(&imgsensor_info.ar0830ap1302);
#endif
	} else {
		ar0830ap1302_stream(&imgsensor_info.ar0830ap1302, enable);
	}
	mdelay(10);
	return ERROR_NONE;
}

static int feature_control(struct subdrv_ctx *ctx, MSDK_SENSOR_FEATURE_ENUM feature_id,
			UINT8 *feature_para, UINT32 *feature_para_len)
{
	UINT16 *feature_return_para_16 = (UINT16 *) feature_para;
	UINT16 *feature_data_16 = (UINT16 *) feature_para;
	UINT32 *feature_return_para_32 = (UINT32 *) feature_para;
	UINT32 *feature_data_32 = (UINT32 *) feature_para;
	unsigned long long *feature_data = (unsigned long long *) feature_para;
	kal_uint32 rate;

	struct SENSOR_WINSIZE_INFO_STRUCT *wininfo;
	struct SENSOR_VC_INFO_STRUCT *pvcinfo;
	 MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data =
		 (MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;

	switch (feature_id) {
	case SENSOR_FEATURE_GET_OUTPUT_FORMAT_BY_SCENARIO:
		switch (*feature_data) {
		case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
		case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
		case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
		case SENSOR_SCENARIO_ID_SLIM_VIDEO:
		case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
			*(feature_data + 1)
			= (enum ACDK_SENSOR_OUTPUT_DATA_FORMAT_ENUM)
				imgsensor_info.sensor_output_dataformat;
			break;
		}
	break;
	case SENSOR_FEATURE_GET_ANA_GAIN_TABLE:
		break;
	case SENSOR_FEATURE_GET_GAIN_RANGE_BY_SCENARIO:
		*(feature_data + 1) = imgsensor_info.min_gain;
		*(feature_data + 2) = imgsensor_info.max_gain;
		break;
	case SENSOR_FEATURE_GET_BASE_GAIN_ISO_AND_STEP:
		*(feature_data + 0) = imgsensor_info.min_gain_iso;
		*(feature_data + 1) = imgsensor_info.gain_step;
		*(feature_data + 2) = imgsensor_info.gain_type;
		break;
	case SENSOR_FEATURE_GET_MIN_SHUTTER_BY_SCENARIO:
		*(feature_data + 1) = imgsensor_info.min_shutter;
		*(feature_data + 2) = imgsensor_info.exp_step;
		break;
	case SENSOR_FEATURE_GET_PERIOD:
		*feature_return_para_16++ = ctx->line_length;
		*feature_return_para_16 = ctx->frame_length;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
		*feature_return_para_32 = ctx->pclk;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_ESHUTTER:
		break;
	case SENSOR_FEATURE_SET_NIGHTMODE:
		break;
	case SENSOR_FEATURE_SET_GAIN:
		break;
	case SENSOR_FEATURE_SET_FLASHLIGHT:
		break;
	case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
		break;
	case SENSOR_FEATURE_SET_REGISTER:
		break;
	case SENSOR_FEATURE_GET_REGISTER:
		break;
	case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
		// get the lens driver ID from EEPROM or just
		// return LENS_DRIVER_ID_DO_NOT_CARE
		// if EEPROM does not exist in camera module.
		*feature_return_para_32 = LENS_DRIVER_ID_DO_NOT_CARE;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_VIDEO_MODE:
		break;
	case SENSOR_FEATURE_CHECK_SENSOR_ID:
		get_imgsensor_id(ctx, feature_return_para_32);
		break;
	case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
		set_auto_flicker_mode(ctx, (BOOL)*feature_data_16,
			*(feature_data_16+1));
		break;
	case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
		set_max_framerate_by_scenario(ctx,
			(enum MSDK_SCENARIO_ID_ENUM)*feature_data,
			*(feature_data+1));
		break;
	case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
		get_default_framerate_by_scenario(ctx,
			(enum MSDK_SCENARIO_ID_ENUM)*(feature_data),
			(MUINT32 *)(uintptr_t)(*(feature_data+1)));
		break;
	case SENSOR_FEATURE_SET_TEST_PATTERN:
		set_test_pattern_mode(ctx, (BOOL)*feature_data);
		break;
	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:
		//for factory mode auto testing
		*feature_return_para_32 = imgsensor_info.checksum_value;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_FRAMERATE:
		LOG_DBG("current fps :%u\n", (UINT32)*feature_data);
		ctx->current_fps = *feature_data;
		break;
	case SENSOR_FEATURE_SET_HDR:
		LOG_DBG("ihdr enable :%u\n", (BOOL)*feature_data);
		ctx->ihdr_mode = *feature_data;
		break;
	case SENSOR_FEATURE_GET_CROP_INFO:
		LOG_DBG("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%u\n",
			(UINT32)*feature_data);
		wininfo = (struct SENSOR_WINSIZE_INFO_STRUCT *)
			(uintptr_t)(*(feature_data+1));

		switch (*feature_data_32) {
		case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[1],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[2],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[3],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case SENSOR_SCENARIO_ID_SLIM_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[4],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		default:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[0],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		}
		break;
	case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:
		break;
	case SENSOR_FEATURE_GET_VC_INFO:
		LOG_DBG("SENSOR_FEATURE_GET_VC_INFO 0x%x\n",
			(UINT16)*feature_data);
		pvcinfo = (struct SENSOR_VC_INFO_STRUCT *)
			(uintptr_t)(*(feature_data+1));
		switch (*feature_data_32) {
		case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
			memcpy((void *)pvcinfo,
				(void *)&SENSOR_VC_INFO[1],
				sizeof(struct SENSOR_VC_INFO_STRUCT));
			break;
		case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
			memcpy((void *)pvcinfo,
				(void *)&SENSOR_VC_INFO[2],
				sizeof(struct SENSOR_VC_INFO_STRUCT));
			break;
		case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		default:
			memcpy((void *)pvcinfo,
				(void *)&SENSOR_VC_INFO[0],
				sizeof(struct SENSOR_VC_INFO_STRUCT));
			break;
		}
		break;
	case SENSOR_FEATURE_SET_HDR_SHUTTER:
		break;
	case SENSOR_FEATURE_GET_MIPI_PIXEL_RATE:
		switch (*feature_data) {
		case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
			rate = imgsensor_info.cap.mipi_pixel_rate;
			break;
		case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
			rate = imgsensor_info.normal_video.mipi_pixel_rate;
			break;
		case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
			rate = imgsensor_info.hs_video.mipi_pixel_rate;
			break;
		case SENSOR_SCENARIO_ID_SLIM_VIDEO:
			rate = imgsensor_info.slim_video.mipi_pixel_rate;
			break;
		case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		default:
			rate = imgsensor_info.pre.mipi_pixel_rate;
			break;
		}
		*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) = rate;
		break;
	case SENSOR_FEATURE_SET_STREAMING_SUSPEND:
		LOG_DBG("SENSOR_FEATURE_SET_STREAMING_SUSPEND\n");
		streaming_control(ctx, KAL_FALSE);
		break;
	case SENSOR_FEATURE_SET_STREAMING_RESUME:
		LOG_DBG("SENSOR_FEATURE_SET_STREAMING_RESUME\n");
		streaming_control(ctx, KAL_TRUE);
		break;
	case SENSOR_FEATURE_GET_PERIOD_BY_SCENARIO:
		switch (*feature_data) {
		case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.cap.framelength << 16)
				+ imgsensor_info.cap.linelength;
			break;
		case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.normal_video.framelength << 16)
				+ imgsensor_info.normal_video.linelength;
			break;
		case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.hs_video.framelength << 16)
				+ imgsensor_info.hs_video.linelength;
			break;
		case SENSOR_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.slim_video.framelength << 16)
				+ imgsensor_info.slim_video.linelength;
			break;
		case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.pre.framelength << 16)
				+ imgsensor_info.pre.linelength;
			break;
		}
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ_BY_SCENARIO:
		switch (*feature_data) {
		case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.cap.pclk;
			break;
		case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.normal_video.pclk;
			break;
		case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.hs_video.pclk;
			break;
		case SENSOR_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.slim_video.pclk;
			break;
		case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.pre.pclk;
			break;
		}
		break;
	case SENSOR_FEATURE_GET_BINNING_TYPE:
		switch (*(feature_data + 1)) {
		case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		case SENSOR_SCENARIO_ID_HIGHSPEED_VIDEO:
		case SENSOR_SCENARIO_ID_SLIM_VIDEO:
		case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
		case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
		default:
			*feature_return_para_32 = 1; /*BINNING_AVERAGE*/
			break;
		}
		pr_debug("SENSOR_FEATURE_GET_BINNING_TYPE AE_binning_type:%u,\n",
			*feature_return_para_32);
		*feature_para_len = 4;

		break;
	default:
		break;
	}

	return ERROR_NONE;
} /* feature_control(ctx)  */

#ifdef IMGSENSOR_VC_ROUTING
static struct mtk_mbus_frame_desc_entry frame_desc_prev[] = {
	{
		.bus.csi2 = {
			.channel = 0,
			.data_type = 0x1e,
			.hsize = 3200,
			.vsize = 1800,
		},
	},
};

static struct mtk_mbus_frame_desc_entry frame_desc_cap[] = {
	{
		.bus.csi2 = {
			.channel = 0,
			.data_type = 0x1e,
			.hsize = 3840,
			.vsize = 2160,
		},
	},
};

static struct mtk_mbus_frame_desc_entry frame_desc_video[] = {
	{
		.bus.csi2 = {
			.channel = 0,
			.data_type = 0x1e,
			.hsize = 1920,
			.vsize = 1080,
		},
	},
};

static int get_frame_desc(struct subdrv_ctx *ctx,
		int scenario_id, struct mtk_mbus_frame_desc *fd)
{
	switch (scenario_id) {
	case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
		fd->type = MTK_MBUS_FRAME_DESC_TYPE_CSI2;
		fd->num_entries = ARRAY_SIZE(frame_desc_prev);
		memcpy(fd->entry, frame_desc_prev, sizeof(frame_desc_prev));
		break;
	case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
		fd->type = MTK_MBUS_FRAME_DESC_TYPE_CSI2;
		fd->num_entries = ARRAY_SIZE(frame_desc_cap);
		memcpy(fd->entry, frame_desc_cap, sizeof(frame_desc_cap));
		break;
	case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
		fd->type = MTK_MBUS_FRAME_DESC_TYPE_CSI2;
		fd->num_entries = ARRAY_SIZE(frame_desc_video);
		memcpy(fd->entry, frame_desc_video, sizeof(frame_desc_video));
		break;

	default:
		return -1;
	}

	return 0;
}
#endif
static int vsync_notify(struct subdrv_ctx *ctx, unsigned int sof_cnt)
{
	return 0;
};

static const struct subdrv_ctx defctx = {
	.ana_gain_def = BASEGAIN * 4,
	.ana_gain_max = BASEGAIN * 16,
	.ana_gain_min = BASEGAIN,
	.ana_gain_step = 1,
	.exposure_def = 0x3D0,
	.exposure_max = 0xffff - 10,
	.exposure_min = 1,
	.exposure_step = 1,
	.max_frame_length = 0xffff,

	.mirror = IMAGE_NORMAL,//mirrorflip information
	//IMGSENSOR_MODE enum value,record current sensor mode,
	//such as: INIT, Preview, Capture, Video,High Speed Video, Slim Video
	.sensor_mode = IMGSENSOR_MODE_INIT,
	.shutter = 0x4C00,//current shutter
	.gain = BASEGAIN * 4,//current gain
	.dummy_pixel = 0,//current dummypixel
	.dummy_line = 0,  //current dummyline
	//full size current fps : 24fps for PIP, 30fps for Normal or ZSD
	.current_fps = 300,
	//auto flicker enable: KAL_FALSE for disable auto flicker,
	//KAL_TRUE for enable auto flicker
	.autoflicker_en = KAL_FALSE,
	//test pattern mode or not. KAL_FALSE for in test pattern mode,
	//KAL_TRUE for normal output
	.test_pattern = KAL_FALSE,
	//current scenario id
	.current_scenario_id = SENSOR_SCENARIO_ID_NORMAL_PREVIEW,
	.ihdr_mode = 0, //sensor need support LE, SE with HDR feature
	.i2c_write_id = 0x78,
};

static int init_ctx(struct subdrv_ctx *ctx,
		struct i2c_client *i2c_client, u8 i2c_write_id)
{
	memcpy(ctx, &defctx, sizeof(*ctx));
	ctx->i2c_client = i2c_client;
	ctx->i2c_write_id = i2c_write_id;

	// add for AR0830AP1302
	imgsensor_info.ar0830ap1302.dev = &i2c_client->dev;
	imgsensor_info.ar0830ap1302.client = i2c_client;
	imgsensor_info.ar0830ap1302.ctx = ctx;
	return 0;
}

static struct subdrv_ops ops = {
	.get_id = get_imgsensor_id,
	.init_ctx = init_ctx,
	.open = open,
	.get_info = get_info,
	.get_resolution = get_resolution,
	.control = control,
	.feature_control = feature_control,
	.close = close,
#ifdef IMGSENSOR_VC_ROUTING
	.get_frame_desc = get_frame_desc,
#endif
	.vsync_notify = vsync_notify,
};

static struct subdrv_pw_seq_entry pw_seq[] = {
	{HW_ID_PDN, 0, 0},
	{HW_ID_RST, 0, 0},
	{HW_ID_AVDD, 2800000, 0},
	{HW_ID_DVDD, 1000000, 0},
	{HW_ID_AFVDD, 1800000, 0},
	{HW_ID_DOVDD, 1800000, 1},
	{HW_ID_MCLK, 48, 1},
	{HW_ID_MCLK_DRIVING_CURRENT, 2, 5},
	{HW_ID_PDN, 1, 0},
	{HW_ID_RST, 1, 10},
};

const struct subdrv_entry ar0830ap1302_mipi_yuv_entry = {
	.name = "ar0830ap1302_mipi_yuv",
	.id = AR0830AP1302_SENSOR_ID,
	.pw_seq = pw_seq,
	.pw_seq_cnt = ARRAY_SIZE(pw_seq),
	.ops = &ops,
};

