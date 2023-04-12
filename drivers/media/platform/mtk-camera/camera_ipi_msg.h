/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
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

#ifndef _UAPI_CAMERA_IPI_MSG_H_
#define _UAPI_CAMERA_IPI_MSG_H_

#include <linux/dma-buf.h>

#define MTK_VIDEO_MAX_PLANES 3

enum camera_ipi_msg_status {
	CAM_IPI_MSG_STATUS_OK   = 0,
	CAM_IPI_MSG_STATUS_FAIL = -1,
	CAM_IPI_MSG_TIMEOUT     = -2,
};

enum camera_ipi_msgid {
	AP_IPIMSG_CAM_INIT               = 0xC000,
	AP_IPIMSG_CAM_START_STREAM       = 0xC001,
	AP_IPIMSG_CAM_INIT_BUFFER        = 0xC002,
	AP_IPIMSG_CAM_DEINIT_BUFFER      = 0xC003,
	AP_IPIMSG_CAM_START              = 0xC004,
	AP_IPIMSG_CAM_END                = 0xC005,
	AP_IPIMSG_CAM_STOP_STREAM        = 0xC006,
	AP_IPIMSG_CAM_DEINIT             = 0xC007,
	AP_IPIMSG_CAM_SET_PARAM          = 0xC008,
	AP_IPIMSG_CAM_GET_PARAM          = 0xC009,
	VCU_IPIMSG_CAM_INIT_ACK          = 0xD000,
	VCU_IPIMSG_CAM_START_STREAM_ACK  = 0xD001,
	VCU_IPIMSG_CAM_INIT_BUFFER_ACK   = 0xD002,
	VCU_IPIMSG_CAM_DEINIT_BUFFER_ACK = 0xD003,
	VCU_IPIMSG_CAM_START_ACK         = 0xD004,
	VCU_IPIMSG_CAM_END_ACK           = 0xD005,
	VCU_IPIMSG_CAM_STOP_STREAM_ACK   = 0xD006,
	VCU_IPIMSG_CAM_DEINIT_ACK        = 0xD007,
	VCU_IPIMSG_CAM_SET_PARAM_ACK     = 0xD008,
	VCU_IPIMSG_CAM_GET_PARAM_ACK     = 0xD009,
};

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

enum mtk_instance_state {
	MTK_STATE_FREE  = 0,
	MTK_STATE_INIT  = 1,
	MTK_STATE_START = 2,
	MTK_STATE_FLUSH = 3,
	MTK_STATE_ABORT = 4,
};

enum camera_buffer_status {
	BUFFER_FILLED = 0,
	BUFFER_EMPTY  = 1,
	BUFFER_ERROR  = 2,
};

enum camera_id {
	CAMERA_0   = 0,
	CAMERA_1   = 1,
	CAMERA_2   = 2,
	CAMERA_3   = 3,
	CAMERA_NUM = 4,
};

enum camera_stream_id {
	STREAM_PREVIEW = 0,
	STREAM_VIDEO   = 1,
	STREAM_CAPTURE = 2,
	STREAM_NUM     = 3,
};

enum sensor_type {
	SENSOR_TYPE_UNKNOWN = 0,
	SENSOR_TYPE_RAW     = 1,
	SENSOR_TYPE_YUV     = 2,
};

#pragma pack(push, 4)

struct fb_info_in {
	uint32_t index;
	uint64_t dma_addr[MTK_VIDEO_MAX_PLANES];
	uint32_t dma_size[MTK_VIDEO_MAX_PLANES];
	uint32_t num_planes;
	union {
		uint64_t ptr[MTK_VIDEO_MAX_PLANES];
		int fd[MTK_VIDEO_MAX_PLANES];
	} dbuf;
};

struct fb_info_out {
	uint32_t index;
	uint64_t dma_addr;
	uint32_t dma_size;
	uint32_t status;
	uint64_t timestamp;
};

/**
 * struct fmt_info - Used for GET_PARAM_SUPPORTED_FORMATS
 * @index: index of the v4l2 format, set by the driver
 * @v4l2_format: v4l2 format fourcc, set by the daemon
 * @valid: whether the information is valid, set by the daemon
 */
struct fmt_info {
	uint32_t index;
	uint32_t v4l2_format;
	uint32_t valid;
};

/**
 * struct res_info - Used for GET_PARAM_FRAME_SIZES
 * @index: index of the v4l2 format, set by the driver
 * @v4l2_format: v4l2 format fourcc, set by the driver
 * @width: resolution width, set by the daemon
 * @height: resolution height, set by the daemon
 * @valid: whether the information is valid, set by the daemon
 */
struct res_info {
	uint32_t index;
	uint32_t v4l2_format;
	uint32_t width;
	uint32_t height;
	uint32_t valid;
};

/**
 * struct sensor_info - Used for GET_PARAM_SENSOR_INFO
 * @type: sensor type (e.g. RAW, YUV), set by the daemon
 * @valid: whether the information is valid, set by the daemon
 */
struct sensor_info {
	uint32_t type;
	uint32_t valid;
};

/**
 * struct cam_ap_ipi_cmd - generic AP to VCU ipi command format
 * @msg_id		: camera_ipi_msgid
 * @vcu_inst_addr : VCU camera instance address
 */
struct cam_ap_ipi_cmd {
	uint32_t msg_id;
	uint32_t ipi_id;
	uint64_t vcu_inst_addr;
	uint64_t ap_inst_addr;
	struct fb_info_in info;
	uint32_t stream_id;
};

/**
 * struct cam_vcu_ipi_ack - generic VPU to AP ipi command format
 * @msg_id		: camera_ipi_msgid
 * @status		: VPU execution result
 * @ap_inst_addr	: AP camera instance address
 */
struct cam_vcu_ipi_ack {
	uint32_t msg_id;
	uint32_t ipi_id;
	uint32_t param_id;
	uint64_t vcu_inst_addr;
	uint64_t ap_inst_addr;
	struct fb_info_out info;
	uint64_t data_addr;
	int32_t  status;
	uint32_t stream_id;
	union {
		struct fmt_info fmt;
		struct res_info res;
		struct sensor_info sensor;
	} data;
};

/**
 * struct cam_ap_ipi_init - for AP_IPIMSG_CAM_INIT
 * @msg_id		: AP_IPIMSG_CAM_INIT
 * @reserved		: Reserved field
 * @ap_inst_addr	: AP camera instance address
 */
struct cam_ap_ipi_init {
	uint32_t msg_id;
	uint32_t ipi_id;
	uint64_t ap_inst_addr;
	uint32_t stream_id;
};

/**
 * struct cam_ap_ipi_set_param - for AP_IPIMSG_CAM_SET_PARAM
 * @msg_id		: AP_IPIMSG_CAM_SET_PARAM
 * @vcu_inst_addr	: VCU decoder instance address
 * @id			: set param type
 * @data		: param data
 * @stream_id		: stream id
 */
struct cam_ap_ipi_set_param {
	uint32_t msg_id;
	uint32_t ipi_id;
	uint32_t id;
	uint64_t vcu_inst_addr;
	uint64_t ap_inst_addr;
	uint32_t data[4];
	uint32_t stream_id;
};

/**
 * struct cam_ap_ipi_get_param - for AP_IPIMSG_CAM_GET_PARAM
 * @msg_id		: AP_IPIMSG_CAM_GET_PARAM
 * @vcu_inst_addr	: VCU decoder instance address
 * @id			: get param type
 * @data		: param data
 * @data_addr		: data address to return
 * @stream_id		: stream id
 */
struct cam_ap_ipi_get_param {
	uint32_t msg_id;
	uint32_t ipi_id;
	uint32_t id;
	uint64_t vcu_inst_addr;
	uint64_t ap_inst_addr;
	uint64_t data_addr;
	uint32_t stream_id;
	union {
		struct fmt_info fmt;
		struct res_info res;
		struct sensor_info sensor;
	} data;
};

#pragma pack(pop)

#endif /* _UAPI_CAMERA_IPI_MSG_H_ */
