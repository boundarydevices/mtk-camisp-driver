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

#ifndef _MTK_CAMERA_UTIL_H_
#define _MTK_CAMERA_UTIL_H_

enum camera_buffer_type {
	BUFFER_STEREO = 0,
	BUFFER_MAIN1  = 1,
	BUFFER_MAIN2  = 2,
};

enum camera_buffer_status {
	BUFFER_FILLED = 0,
	BUFFER_EMPTY  = 1,
	BUFFER_ERROR  = 2,
};

enum camera_stream_id {
	STREAM_PREVIEW = 0,
	STREAM_VIDEO   = 1,
	STREAM_CAPTURE = 2,
	STREAM_NUM     = 3,
};

enum camera_id {
	CAMERA_0   = 0,
	CAMERA_1   = 1,
	CAMERA_2   = 2,
	CAMERA_3   = 3,
	CAMERA_NUM = 4,
};

struct plane_buffer {
	unsigned long size;
	unsigned long payload;
	dma_addr_t dma_addr;
	unsigned int bytesperline;
	struct dma_buf *dbuf;
	int dbuf_fd;
};

struct mtk_camera_mem {
	unsigned int status;
	unsigned int index;
	unsigned int format;
	unsigned int num_planes;
	unsigned long size;
	struct list_head list;
	enum camera_buffer_type type;
	struct plane_buffer planes[VIDEO_MAX_PLANES];
};

#define DEBUG	1

#define ALIGN_CEIL(value, align)     (((value) + (align) - 1L) & ~((align) - 1L))

#endif /*_MTK_CAMERA_UTIL_H_*/
