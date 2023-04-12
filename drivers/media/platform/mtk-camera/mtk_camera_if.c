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

#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include "mtk_camera_if.h"
#include "mtk_camera_drv.h"
#include "mtk_vcu_camif.h"

int  camera_if_init(void *ctx)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	if (!contex->cam_if_rdy) {
		contex->cam_if = get_camera_if();
		ret = contex->cam_if->init((void *)contex, &contex->drv_handle);
		if (ret == 0)
			contex->cam_if_rdy = true;
	}
	mtk_camera_unlock(contex);

	return ret;
}

void camera_if_deinit(void *ctx)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;

	if (contex->drv_handle == 0)
		return;

	mtk_camera_lock(contex);
	contex->cam_if->deinit(contex->drv_handle);
	contex->cam_if_rdy = false;
	mtk_camera_unlock(contex);

	contex->drv_handle = 0;
}

int  camera_if_start_stream(void *ctx)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	ret = contex->cam_if->start_stream(contex->drv_handle);
	mtk_camera_unlock(contex);

	return ret;
}

int  camera_if_stop_stream(void *ctx)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	ret = contex->cam_if->stop_stream(contex->drv_handle);
	mtk_camera_unlock(contex);

	return ret;
}

int  camera_if_init_buffer(void *ctx, void *fb)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	ret = contex->cam_if->init_buffer(contex->drv_handle, fb);
	mtk_camera_unlock(contex);

	return ret;
}

int  camera_if_deinit_buffer(void *ctx, void *fb)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	ret = contex->cam_if->deinit_buffer(contex->drv_handle, fb);
	mtk_camera_unlock(contex);

	return ret;
}

int  camera_if_capture(void *ctx, void *fb)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	ret = contex->cam_if->capture(contex->drv_handle, fb);
	mtk_camera_unlock(contex);

	return ret;
}

int  camera_if_get_param(void *ctx,
			enum camera_get_param_type type,
			void *out)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	ret = contex->cam_if->get_param(contex->drv_handle, type, out);
	mtk_camera_unlock(contex);

	return ret;
}

int  camera_if_set_param(void *ctx,
			enum camera_set_param_type type,
			void *in)
{
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	int ret = 0;

	mtk_camera_lock(contex);
	ret = contex->cam_if->set_param(contex->drv_handle, type, in);
	mtk_camera_unlock(contex);

	return ret;
}
