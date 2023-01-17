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

#include "mtk_camera_drv_base.h"
#include "mtk_camera_drv.h"
#include "mtk_camera_util.h"
#include "mtk_vcu_camif.h"

static void handle_init_ack_msg(struct cam_vcu_ipi_ack *m)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)(unsigned long)m->ap_inst_addr;
	struct mtk_camera_ctx *ctx = inst->ctx;

	dev_dbg(ctx->dev, "+ ap_inst_addr = 0x%llx\n", (uint64_t)m->ap_inst_addr);
	inst->inst_addr = m->vcu_inst_addr;
	dev_dbg(ctx->dev, "- vcu_inst_addr = 0x%llx\n", (uint64_t)inst->inst_addr);
}

static void handle_capture_ack_msg(struct cam_vcu_ipi_ack *m)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)(unsigned long)m->ap_inst_addr;
	struct mtk_camera_ctx *ctx = inst->ctx;
	struct fb_info_out *info = &m->info;
	struct mtk_camera_mem *mem = NULL;

	dev_dbg(ctx->dev, "+ ap_inst_addr = 0x%llx, handle 0x%llx\n",
		(uint64_t)m->ap_inst_addr, info->dma_addr);

	list_for_each_entry(mem, &inst->queue, list) {
		if (mem->planes[0].dma_addr == info->dma_addr) {
			if (info->status == 0)
				mem->status = BUFFER_FILLED;
			else
				mem->status = BUFFER_ERROR;
			ctx->callback(mem);
			dev_dbg(ctx->dev, "- vcu_inst_addr: 0x%llx, buffer handle 0x%llx\n",
				inst->inst_addr, info->dma_addr);
			return;
		}
	}
	dev_err(ctx->dev, "invalid buffer handle %llx\n", info->dma_addr);
}

static void handle_get_param_ack_msg(struct cam_vcu_ipi_ack *m)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)(unsigned long)m->ap_inst_addr;
	struct mtk_camera_ctx *ctx = inst->ctx;

	switch (m->param_id) {
	case GET_PARAM_SUPPORTED_FORMATS:
	{
		struct fmt_info *fmt = (struct fmt_info *)m->data_addr;

		fmt->v4l2_format = m->data.fmt.v4l2_format;
		fmt->valid = m->data.fmt.valid;
		break;
	}
	case GET_PARAM_FRAME_SIZES:
	{
		struct res_info *res = (struct res_info *)m->data_addr;

		res->width = m->data.res.width;
		res->height = m->data.res.height;
		res->valid = m->data.res.valid;
		break;
	}
	default:
		dev_err(ctx->dev, "Unknown get_param id=%d", m->param_id);
		break;
	}
}

/*
 * This function runs in interrupt context and it means there's a IPI MSG
 * from VCU.
 */
int vcu_ipi_handler(void *data, unsigned int len, void *priv)
{
	struct cam_vcu_ipi_ack *m = data;
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)((unsigned long)m->ap_inst_addr);
	struct mtk_camera_ctx *ctx = inst->ctx;
	int ret = 0;

	dev_dbg(ctx->dev, "+ id=%x status = %d\n", m->msg_id, m->status);

	inst->failure = m->status;

	if (m->status == 0) {
		switch (m->msg_id) {
		case VCU_IPIMSG_CAM_INIT_ACK:
			handle_init_ack_msg(data);
			break;
		case VCU_IPIMSG_CAM_START_STREAM_ACK:
		case VCU_IPIMSG_CAM_STOP_STREAM_ACK:
		case VCU_IPIMSG_CAM_INIT_BUFFER_ACK:
		case VCU_IPIMSG_CAM_DEINIT_BUFFER_ACK:
		case VCU_IPIMSG_CAM_START_ACK:
		case VCU_IPIMSG_CAM_DEINIT_ACK:
		case VCU_IPIMSG_CAM_SET_PARAM_ACK:
			break;
		case VCU_IPIMSG_CAM_GET_PARAM_ACK:
			handle_get_param_ack_msg(data);
			break;
		case VCU_IPIMSG_CAM_END_ACK:
			handle_capture_ack_msg(data);
			ret = 1;
			break;
		default:
			dev_err(ctx->dev, "invalid msg=%x\n", m->msg_id);
			ret = 1;
			break;
		}
	}

	dev_dbg(ctx->dev, "- id=%x\n", m->msg_id);
	inst->signaled = 1;

	return ret;
}

static int camera_vcu_send_msg(struct cam_vcu_inst *inst, void *m, int l)
{
	uint32_t msg_id = *(uint32_t *)m;
	struct mtk_camera_ctx *ctx = inst->ctx;
	int err = 0;

	dev_dbg(ctx->dev, "id=%x\n", msg_id);

	inst->failure  = 0;
	inst->signaled = 0;

	err = vcu_ipi_send(inst->dev, inst->id, m, l, NULL);
	if (err) {
		dev_err(ctx->dev, "send fail vcu_id=%d msg_id=%x status=%d\n",
			       inst->id, msg_id, err);
		return err;
	}

	return inst->failure;
}

static int camera_send_ap_ipi(struct cam_vcu_inst *inst,
		unsigned int id, struct fb_info_in *n)
{
	struct cam_ap_ipi_cmd m;

	memset(&m, 0, sizeof(m));
	m.msg_id = id;
	m.ipi_id = inst->id;
	m.vcu_inst_addr = inst->inst_addr;
	m.ap_inst_addr = (uint64_t)(unsigned long)inst;
	m.stream_id = (uint32_t)inst->ctx->stream_id;
	if (n != NULL)
		m.info = *n;

	return camera_vcu_send_msg(inst, &m, sizeof(m));
}

static int camera_vcu_set_param(struct cam_vcu_inst *inst,
		unsigned int id, void *param, unsigned int size)
{
	struct cam_ap_ipi_set_param msg;
	uint32_t *param_ptr = (uint32_t *)param;
	struct mtk_camera_ctx *ctx = inst->ctx;
	int i = 0;

	dev_dbg(ctx->dev, "+ id=%x\n", AP_IPIMSG_CAM_SET_PARAM);

	memset(&msg, 0, sizeof(msg));
	msg.msg_id = AP_IPIMSG_CAM_SET_PARAM;
	msg.ipi_id = inst->id;
	msg.id = id;
	msg.vcu_inst_addr = inst->inst_addr;
	msg.ap_inst_addr = (uint64_t)(unsigned long)inst;
	msg.stream_id = (uint32_t)inst->ctx->stream_id;

	for (i = 0; i < size; ++i)
		msg.data[i] = *(param_ptr + i);

	return camera_vcu_send_msg(inst, &msg, sizeof(msg));
}

static int camera_vcu_get_fmt(struct cam_vcu_inst *inst,
		unsigned int id, void *param)
{
	struct cam_ap_ipi_get_param msg;
	struct mtk_camera_ctx *ctx = inst->ctx;

	dev_dbg(ctx->dev, "+ id=%x\n", AP_IPIMSG_CAM_GET_PARAM);

	memset(&msg, 0, sizeof(msg));
	msg.msg_id = AP_IPIMSG_CAM_GET_PARAM;
	msg.ipi_id = inst->id;
	msg.id = id;
	msg.vcu_inst_addr = inst->inst_addr;
	msg.ap_inst_addr = (uint64_t)(unsigned long)inst;

	switch (id) {
	case GET_PARAM_SUPPORTED_FORMATS:
	{
		struct camera_fmt_info *fmt = (struct camera_fmt_info *)param;

		msg.data.fmt.index = fmt->index;
		break;
	}
	case GET_PARAM_FRAME_SIZES:
	{
		struct camera_res_info *res = (struct camera_res_info *)param;

		msg.data.res.index = res->index;
		msg.data.res.v4l2_format = res->v4l2_format;
		break;
	}
	default:
		dev_err(ctx->dev, "Unknown get_param id=%d", id);
		break;
	}

	msg.data_addr = (uint64_t)(unsigned long)param;
	msg.stream_id = (uint32_t)inst->ctx->stream_id;

	return camera_vcu_send_msg(inst, &msg, sizeof(msg));
}

static int camera_init(void *ctx, unsigned long *handle)
{
	struct cam_vcu_inst *inst = NULL;
	struct mtk_camera_ctx *contex = (struct mtk_camera_ctx *)ctx;
	struct cam_ap_ipi_init msg;
	int ret = 0;

	if (!ctx || !handle)
		return -ENOMEM;

	inst = kzalloc(sizeof(*inst), GFP_KERNEL);
	if (!inst)
		return -ENOMEM;

	inst->ctx = contex;
	inst->failure  = 0;
	inst->signaled = 0;

	if (contex->camera_id >= 0 && contex->camera_id < CAMERA_NUM)
		inst->id = IPI_CAMERA_1 + contex->camera_id;
	else {
		dev_err(contex->dev, "Invalid camera_id:%d\n", contex->camera_id);
		kfree(inst);
		return -EINVAL;
	}

	dev_dbg(contex->dev, "camera_id:%d ipi:%d\n", contex->camera_id, inst->id);

	INIT_LIST_HEAD(&inst->queue);
	inst->dev = vcu_get_plat_device(contex->stream->plat_dev);
	if (inst->dev == NULL) {
		kfree(inst);
		return -EINVAL;
	}

	ret = vcu_ipi_register(inst->dev, inst->id, vcu_ipi_handler, NULL, NULL);
	if (ret != 0) {
		dev_err(contex->dev, "camera_vcu_ipi_register fail status=%d\n", ret);
		kfree(inst);
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.msg_id = AP_IPIMSG_CAM_INIT;
	msg.ipi_id = inst->id;
	msg.ap_inst_addr = (uint64_t)(unsigned long)inst;
	msg.stream_id = contex->stream_id;

	ret = camera_vcu_send_msg(inst, (void *)&msg, sizeof(msg));
	if (ret != 0) {
		dev_err(contex->dev, "camera_vcu_send_msg %s fail status=%d\n", __func__, ret);
		kfree(inst);
		return ret;
	}

	*handle = (unsigned long)inst;

	return 0;
}

int camera_init_buffer(unsigned long handle, void *fb)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;
	struct mtk_camera_mem *mem = (struct mtk_camera_mem *)fb;
	struct mtk_camera_ctx *ctx = inst->ctx;
	struct fb_info_in info;
	int i;

	info.index = (uint32_t)(mem->index);
	info.num_planes = (uint32_t)(mem->num_planes);

	for (i = 0; i < info.num_planes; i++) {
		info.dma_addr[i] = (uint64_t)(mem->planes[i].dma_addr);
		info.dma_size[i] = (uint32_t)(mem->planes[i].size);
		info.dbuf.ptr[i] = (uint64_t)(mem->planes[i].dbuf);
		dev_dbg(ctx->dev, "inst %p, dma_addr[i] 0x%llx\n",
			inst, mem->planes[i].dma_addr);

	}

	list_add_tail(&mem->list, &inst->queue);

	return camera_send_ap_ipi(inst, AP_IPIMSG_CAM_INIT_BUFFER, &info);
}

int camera_deinit_buffer(unsigned long handle, void *fb)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;
	struct mtk_camera_ctx *ctx = inst->ctx;
	struct fb_info_in info = {0};
	int i;

	struct mtk_camera_mem *mem = (struct mtk_camera_mem *)fb;
	struct mtk_camera_mem *child, *t;

	dev_dbg(ctx->dev, "inst %p, fb 0x%llx\n",
		inst, mem->planes[0].dma_addr);

	info.num_planes = (uint32_t)(mem->num_planes);
	for (i = 0; i < info.num_planes; i++) {
		info.dma_addr[i] = (uint64_t)(mem->planes[i].dma_addr);
		info.dma_size[i] = (uint32_t)(mem->planes[i].size);
	}
	list_for_each_entry_safe(child, t,
				&inst->queue, list) {
		if (child == mem) {
			list_del(&child->list);
			dev_dbg(ctx->dev, "remove fb 0x%llx\n", child->planes[0].dma_addr);
		}
	}

	return camera_send_ap_ipi(inst, AP_IPIMSG_CAM_DEINIT_BUFFER, &info);
}

int camera_start_stream(unsigned long handle)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;

	return camera_send_ap_ipi(inst, AP_IPIMSG_CAM_START_STREAM, NULL);
}

int camera_capture(unsigned long handle, void *fb)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;
	struct mtk_camera_ctx *ctx = inst->ctx;
	struct mtk_camera_mem *mem = (struct mtk_camera_mem *)fb;
	struct fb_info_in info = {0};
	int i;

	dev_dbg(ctx->dev, "inst %p, fb 0x%llx\n", inst, mem->planes[0].dma_addr);

	info.index = (uint32_t)(mem->index);
	info.num_planes = (uint32_t)(mem->num_planes);

	for (i = 0; i < info.num_planes; i++) {
		info.dma_addr[i] = (uint64_t)(mem->planes[i].dma_addr);
		info.dma_size[i] = (uint32_t)mem->planes[i].size;
	}

	return camera_send_ap_ipi(inst, AP_IPIMSG_CAM_START, &info);
}

int camera_get_param(unsigned long handle,
		 enum camera_get_param_type type, void *out)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;
	struct mtk_camera_ctx *ctx = inst->ctx;
	int ret = 0;

	switch (type) {
	case GET_PARAM_SUPPORTED_FORMATS:
	case GET_PARAM_FRAME_SIZES:
		camera_vcu_get_fmt(inst, (unsigned int)type, out);
		break;
	default:
		dev_err(ctx->dev, "invalid get parameter type=%d\n", (int)type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

int camera_set_param(unsigned long handle,
		 enum camera_set_param_type type, void *in)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;
	struct mtk_camera_ctx *ctx = inst->ctx;
	int ret = 0;

	switch (type) {
	case SET_PARAM_FRAME_SIZE:
		camera_vcu_set_param(inst, (unsigned int)type, in, 3U);
		break;
	case SET_PARAM_BRIGHTNESS:
	case SET_PARAM_CONTRAST:
	case SET_PARAM_SATURATION:
	case SET_PARAM_SHARPNESS:
	case SET_PARAM_HUE:
	case SET_PARAM_GAMMA:
	case SET_PARAM_AUTO_WHITE_BALANCE:
	case SET_PARAM_WHITE_BALANCE_TEMP:
	case SET_PARAM_EXPOSURE:
	case SET_PARAM_POWER_LINE_FREQ:
	case SET_PARAM_MIN_FPS:
	case SET_PARAM_MAX_FPS:
	case SET_PARAM_EXPOSURE_AUTO:
	case SET_PARAM_EXPOSURE_ABSOLUTE:
	case SET_PARAM_FOCUS_AUTO:
	case SET_PARAM_FOCUS_ABSOLUTE:
	case SET_PARAM_SENSOR_ORIENTATION:
	case SET_PARAM_EXPOSURE_REGION_LEFT:
	case SET_PARAM_EXPOSURE_REGION_TOP:
	case SET_PARAM_EXPOSURE_REGION_RIGHT:
	case SET_PARAM_EXPOSURE_REGION_BOTTOM:
	case SET_PARAM_EXPOSURE_WEIGHT:
	case SET_PARAM_EXPOSURE_REGION_TRIGGER:
	case SET_PARAM_FRAME_DURATION:
	case SET_PARAM_ISO:
	case SET_PARAM_HDR:

		camera_vcu_set_param(inst, (unsigned int)type, in, 1U);
		break;

	default:
		dev_err(ctx->dev, "invalid set parameter type=%d\n", (int)type);
		ret = -EINVAL;
		break;
	}
	return ret;
}

int camera_stop_stream(unsigned long handle)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;

	return camera_send_ap_ipi(inst, AP_IPIMSG_CAM_STOP_STREAM, NULL);
}

void camera_deinit(unsigned long handle)
{
	struct cam_vcu_inst *inst = (struct cam_vcu_inst *)handle;

	if (inst != NULL) {
		camera_send_ap_ipi(inst, AP_IPIMSG_CAM_DEINIT, NULL);
		kfree(inst);
	}
}

static struct mtk_camera_if sdk = {
	camera_init,
	camera_capture,
	camera_start_stream,
	camera_init_buffer,
	camera_deinit_buffer,
	camera_get_param,
	camera_set_param,
	camera_stop_stream,
	camera_deinit,
};

struct mtk_camera_if *get_camera_if(void)
{
	return &sdk;
}
