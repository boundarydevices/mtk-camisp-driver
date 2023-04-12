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

#ifndef _MTK_CAMERA_FORMAT_H_
#define _MTK_CAMERA_FORMAT_H_

#include "mtk_camera_drv.h"

static struct mtk_camera_fmt mtk_camera_formats[] = {
	{
		.name	= "GREY",
		.fourcc = V4L2_PIX_FMT_GREY,
		.depth		= { 8 },
		.row_depth	= { 8 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "RGBR",
		.fourcc = V4L2_PIX_FMT_RGB565X,
		.depth		= { 16 },
		.row_depth	= { 16 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SRGB,
	}, {
		.name	= "RGBP",
		.fourcc = V4L2_PIX_FMT_RGB565,
		.depth		= { 16 },
		.row_depth	= { 16 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SRGB,
	}, {
		.name	= "RGB3",
		.fourcc = V4L2_PIX_FMT_RGB24,
		.depth		= { 24 },
		.row_depth	= { 24 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SRGB,
	}, {
		.name	= "BGR3",
		.fourcc = V4L2_PIX_FMT_BGR24,
		.depth		= { 24 },
		.row_depth	= { 24 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "AR24",
		.fourcc = V4L2_PIX_FMT_ABGR32,
		.depth		= { 32 },
		.row_depth	= { 32 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "BA24",
		.fourcc = V4L2_PIX_FMT_ARGB32,
		.depth		= { 32 },
		.row_depth	= { 32 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SRGB,
	}, {
		.name	= "UYVY",
		.fourcc = V4L2_PIX_FMT_UYVY,
		.depth		= { 16 },
		.row_depth	= { 16 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "VYUY",
		.fourcc = V4L2_PIX_FMT_VYUY,
		.depth		= { 16 },
		.row_depth	= { 16 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YUYV",
		.fourcc = V4L2_PIX_FMT_YUYV,
		.depth		= { 16 },
		.row_depth	= { 16 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YVYU",
		.fourcc = V4L2_PIX_FMT_YVYU,
		.depth		= { 16 },
		.row_depth	= { 16 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YU12",
		.fourcc = V4L2_PIX_FMT_YUV420,
		.depth		= { 12 },
		.row_depth	= { 8 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YV12",
		.fourcc = V4L2_PIX_FMT_YVU420,
		.depth		= { 12 },
		.row_depth	= { 8 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NV12",
		.fourcc = V4L2_PIX_FMT_NV12,
		.depth		= { 12 },
		.row_depth	= { 8 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NV21",
		.fourcc = V4L2_PIX_FMT_NV21,
		.depth		= { 12 },
		.row_depth	= { 8 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NV16",
		.fourcc = V4L2_PIX_FMT_NV16,
		.depth		= { 16 },
		.row_depth	= { 8 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NV61",
		.fourcc = V4L2_PIX_FMT_NV61,
		.depth		= { 16 },
		.row_depth	= { 8 },
		.mplane	= false,
		.num_planes	= 1,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NM12",
		.fourcc = V4L2_PIX_FMT_NV12M,
		.depth		= { 8, 4 },
		.row_depth	= { 8, 8 },
		.mplane	= true,
		.num_planes	= 2,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NM21",
		.fourcc = V4L2_PIX_FMT_NV21M,
		.depth		= { 8, 4 },
		.row_depth	= { 8, 8 },
		.mplane	= true,
		.num_planes	= 2,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NM16",
		.fourcc = V4L2_PIX_FMT_NV16M,
		.depth		= { 8, 8 },
		.row_depth	= { 8, 8 },
		.mplane	= true,
		.num_planes	= 2,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "NM61",
		.fourcc = V4L2_PIX_FMT_NV61M,
		.depth		= { 8, 8 },
		.row_depth	= { 8, 8 },
		.mplane	= true,
		.num_planes	= 2,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YM12",
		.fourcc = V4L2_PIX_FMT_YUV420M,
		.depth		= { 8, 2, 2 },
		.row_depth	= { 8, 4, 4 },
		.mplane	= true,
		.num_planes	= 3,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YM21",
		.fourcc = V4L2_PIX_FMT_YVU420M,
		.depth		= { 8, 2, 2 },
		.row_depth	= { 8, 4, 4 },
		.mplane	= true,
		.num_planes	= 3,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YM16",
		.fourcc = V4L2_PIX_FMT_YUV422M,
		.depth		= { 8, 4, 4 },
		.row_depth	= { 8, 4, 4 },
		.mplane	= true,
		.num_planes	= 3,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "YM61",
		.fourcc = V4L2_PIX_FMT_YVU422M,
		.depth		= { 8, 4, 4 },
		.row_depth	= { 8, 4, 4 },
		.mplane	= true,
		.num_planes	= 3,
		.colorspace	= V4L2_COLORSPACE_SMPTE170M,
	}, {
		.name	= "JPEG",
		.fourcc = V4L2_PIX_FMT_JPEG,
		.mplane	= false,
		.num_planes = 1,
		.colorspace	= V4L2_COLORSPACE_JPEG,
	}
};

#endif /*_MTK_CAMERA_FORMAT_H_*/

