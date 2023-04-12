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

#include <linux/videodev2.h>

enum camera_buffer_type {
	BUFFER_STEREO = 0,
	BUFFER_MAIN1  = 1,
	BUFFER_MAIN2  = 2,
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
	unsigned long long timestamp;
};

#define ALIGN_CEIL(value, align)     (((value) + (align) - 1L) & ~((align) - 1L))

#endif /*_MTK_CAMERA_UTIL_H_*/
