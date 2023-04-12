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

#ifndef _MTK_CAMERA_CTRL_H_
#define _MTK_CAMERA_CTRL_H_

#include <linux/v4l2-controls.h>
#include "mtk_camera_drv.h"

#define V4L2_CID_USER_MTK_CAMERA_BASE	 (V4L2_CID_USER_BASE + 0x10a0)
#define V4L2_CID_CAMERA_MIN_FPS          (V4L2_CID_USER_MTK_CAMERA_BASE + 0)
#define V4L2_CID_CAMERA_MAX_FPS          (V4L2_CID_USER_MTK_CAMERA_BASE + 1)
#define V4L2_CID_CAMERA_SENSOR_ORIENTATION            (V4L2_CID_USER_MTK_CAMERA_BASE + 2)
#define V4L2_CID_CAMERA_EXPOSURE_REGION_LEFT          (V4L2_CID_USER_MTK_CAMERA_BASE + 3)
#define V4L2_CID_CAMERA_EXPOSURE_REGION_TOP           (V4L2_CID_USER_MTK_CAMERA_BASE + 4)
#define V4L2_CID_CAMERA_EXPOSURE_REGION_RIGHT         (V4L2_CID_USER_MTK_CAMERA_BASE + 5)
#define V4L2_CID_CAMERA_EXPOSURE_REGION_BOTTOM        (V4L2_CID_USER_MTK_CAMERA_BASE + 6)
#define V4L2_CID_CAMERA_EXPOSURE_WEIGHT               (V4L2_CID_USER_MTK_CAMERA_BASE + 7)
#define V4L2_CID_CAMERA_EXPOSURE_REGION_TRIGGER       (V4L2_CID_USER_MTK_CAMERA_BASE + 8)
#define V4L2_CID_CAMERA_ISO                           (V4L2_CID_USER_MTK_CAMERA_BASE + 9)
#define V4L2_CID_CAMERA_FRAME_DURATION                (V4L2_CID_USER_MTK_CAMERA_BASE + 10)
#define V4L2_CID_CAMERA_HDR                           (V4L2_CID_USER_MTK_CAMERA_BASE + 11)
#define V4L2_CID_CAMERA_MULTI_OUTPUT_NUM              (V4L2_CID_USER_MTK_CAMERA_BASE + 12)

int mtk_camera_ctrls_setup(struct mtk_camera_ctx *ctx);

#endif /* _MTK_CAMERA_CTRL_H_ */
