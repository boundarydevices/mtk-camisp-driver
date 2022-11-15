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

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-dma-contig.h>

#include "mtk_camera_drv.h"
#include "mtk_camera_util.h"
#include "mtk_camera_if.h"
#include "mtk_camera_ctrl.h"
#include "mtk_vcu.h"

#define MTK_VIDEO_CAPTURE_DEF_WIDTH         1920U
#define MTK_VIDEO_CAPTURE_DEF_HEIGHT        1080U
#define MTK_VIDEO_CAPTURE_MIN_WIDTH         2U
#define MTK_VIDEO_CAPTURE_MAX_WIDTH         8192U
#define MTK_VIDEO_CAPTURE_MIN_HEIGHT        2U
#define MTK_VIDEO_CAPTURE_MAX_HEIGHT        8192U

static struct v4l2_frmsize_discrete mtk_camera_sizes_yuyv[] = {
	{ 640, 360   },
	{ 576, 432   },
	{ 640, 480   },
	{ 960, 540   },
	{ 928, 696   },
	{ 1024, 768  },
	{ 1280, 720  },
	{ 1440, 1080 },
	{ 1920, 1080 },
	{ 1856, 1392 },
	{ 2048, 1536 },
	{ 2104, 1560 },
	{ 3840, 2160 },
	{ 4000, 3000 },
};

static struct v4l2_frmsize_discrete mtk_camera_sizes_ym21[] = {
	{ 640, 360   },
	{ 576, 432   },
	{ 640, 480   },
	{ 960, 540   },
	{ 928, 696   },
	{ 1024, 768  },
	{ 1280, 720  },
	{ 1440, 1080 },
	{ 1920, 1080 },
	{ 1856, 1392 },
	{ 2048, 1536 },
	{ 2104, 1560 },
	{ 3840, 2160 },
	{ 4000, 3000 },
};

static struct v4l2_frmsize_discrete mtk_camera_sizes_nm12[] = {
	{ 640, 360   },
	{ 576, 432   },
	{ 640, 480   },
	{ 960, 540   },
	{ 928, 696   },
	{ 1024, 768  },
	{ 1280, 720  },
	{ 1440, 1080 },
	{ 1920, 1080 },
	{ 1856, 1392 },
	{ 2048, 1536 },
	{ 2104, 1560 },
	{ 3840, 2160 },
	{ 4000, 3000 },
};

static struct v4l2_frmsize_discrete mtk_camera_sizes_rgb3[] = {
	{ 640, 360   },
	{ 576, 432   },
	{ 640, 480   },
	{ 960, 540   },
	{ 928, 696   },
	{ 1024, 768  },
	{ 1280, 720  },
	{ 1440, 1080 },
	{ 1920, 1080 },
	{ 1856, 1392 },
	{ 2048, 1536 },
	{ 2104, 1560 },
	{ 3840, 2160 },
	{ 4000, 3000 },
};

static struct v4l2_frmsize_discrete mtk_camera_sizes_jpeg[] = {
	{ 320, 240   },
	{ 640, 480   },
	{ 1280, 720  },
	{ 1280, 960  },
	{ 1440, 1088 },
	{ 1920, 1080 },
	{ 1920, 1088 },
	{ 1920, 1440 },
	{ 2560, 1920 },
	{ 3840, 2160 },
	{ 4096, 2304 },
	{ 4000, 3000 },
};

static struct mtk_camera_fmt mtk_camera_formats_video[] = {
	{
		.name   = "YUYV",
		.fourcc = V4L2_PIX_FMT_YUYV,
		.bpp	= {16, 0, 0},
		.mplane = false,
		.num_planes = 1,
		.sizes  = mtk_camera_sizes_yuyv,
		.num_sizes = ARRAY_SIZE(mtk_camera_sizes_yuyv),
		.colorspace = V4L2_COLORSPACE_SMPTE170M,
	},
	{
		.name   = "YM21",
		.fourcc = V4L2_PIX_FMT_YVU420M,
		.bpp	= {8, 2, 2},
		.mplane = true,
		.num_planes = 3,
		.sizes	= mtk_camera_sizes_ym21,
		.num_sizes = ARRAY_SIZE(mtk_camera_sizes_ym21),
		.colorspace = V4L2_COLORSPACE_SMPTE170M,
	},
	{
		.name	= "NM12",
		.fourcc = V4L2_PIX_FMT_NV12M,
		.bpp	= {8, 4, 0},
		.mplane = true,
		.num_planes = 2,
		.sizes	= mtk_camera_sizes_nm12,
		.num_sizes = ARRAY_SIZE(mtk_camera_sizes_nm12),
		.colorspace = V4L2_COLORSPACE_SMPTE170M,
	},
	{
		.name	= "RGB3",
		.fourcc = V4L2_PIX_FMT_RGB24,
		.bpp	= {24, 0, 0},
		.mplane = false,
		.num_planes = 1,
		.sizes	= mtk_camera_sizes_rgb3,
		.num_sizes = ARRAY_SIZE(mtk_camera_sizes_rgb3),
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
};

static struct mtk_camera_fmt mtk_camera_formats_capture[] = {
	{
		.name	= "JPEG",
		.fourcc = V4L2_PIX_FMT_JPEG,
		.mplane = false,
		.num_planes = 1,
		.sizes  = mtk_camera_sizes_jpeg,
		.num_sizes = ARRAY_SIZE(mtk_camera_sizes_jpeg),
		.colorspace = V4L2_COLORSPACE_JPEG,
	},
};

static const char * const stream_type[] = {"Preview", "Video", "Capture"};

/* ------------------------------------------------------------------------
 * Privilege management
 */
/*
 * Privilege management is the multiple-open implementation basis. The current
 * implementation is completely transparent for the end-user and doesn't
 * require explicit use of the VIDIOC_G_PRIORITY and VIDIOC_S_PRIORITY ioctls.
 * Those ioctls enable finer control on the device (by making possible for a
 * user to request exclusive access to a device), but are not mature yet.
 * Switching to the V4L2 priority mechanism might be considered in the future
 * if this situation changes.
 *
 * Each open instance of a mtk-camera device can either be in a privileged or
 * unprivileged state. Only a single instance can be in a privileged state at
 * a given time. Trying to perform an operation that requires privileges will
 * automatically acquire the required privileges if possible, or return -EBUSY
 * otherwise. Privileges are dismissed when closing the instance or when
 * freeing the video buffers using VIDIOC_REQBUFS.
 *
 * Operations that require privileges are:
 *
 * - VIDIOC_S_FMT
 * - VIDIOC_REQBUFS
 * - VIDIOC_QUERYBUF
 * - VIDIOC_EXPBUF
 * - VIDIOC_QBUF
 * - VIDIOC_DQBUF
 * - VIDIOC_STREAMON
 * - VIDIOC_STREAMOFF
 */
static int mtk_camera_acquire_privileges(struct mtk_camera_fh *handle)
{
	/* Always succeed if the handle is already privileged. */
	if (handle->state == MTK_HANDLE_ACTIVE)
		return 0;

	/* Check if the device already has a privileged handle. */
	if (atomic_inc_return(&handle->stream->active) != 1) {
		atomic_dec(&handle->stream->active);
		return -EBUSY;
	}

	handle->state = MTK_HANDLE_ACTIVE;
	return 0;
}

static void mtk_camera_dismiss_privileges(struct mtk_camera_fh *handle)
{
	if (handle->state == MTK_HANDLE_ACTIVE)
		atomic_dec(&handle->stream->active);

	handle->state = MTK_HANDLE_PASSIVE;
}

static int mtk_camera_has_privileges(struct mtk_camera_fh *handle)
{
	return handle->state == MTK_HANDLE_ACTIVE;
}

/* -----------------------------------------------------------------------------
 * Video queue operations
 */

static int vb2ops_camera_queue_setup(struct vb2_queue *vq,
				unsigned int *nbuffers,
				unsigned int *nplanes,
				unsigned int sizes[],
				struct device *alloc_ctxs[])
{
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(vq);
	struct mtk_q_data *q_data = &ctx->q_data;
	int i;

	if (q_data == NULL) {
		dev_err(ctx->dev, "q_data is NULL\n");
		return -EINVAL;
	}

	if (*nplanes) {
		for (i = 0; i < *nplanes; i++) {
			if (sizes[i] < q_data->bytesperline[i] * q_data->height)
				return -EINVAL;
		}
	} else {
		if (q_data->fmt->mplane) {
			*nplanes = q_data->fmt->num_planes;
			for (i = 0; i < *nplanes; i++)
				sizes[i] = q_data->sizeimage[i];
		} else {
			*nplanes = 1;
			sizes[0] = q_data->sizeimage[0];
		}
	}

	if (q_data->fmt->fourcc == V4L2_PIX_FMT_JPEG)
		*nbuffers = max_t(unsigned int, *nbuffers, MTK_CAMERA_CAPTURE_MIN_BUFFERS);
	else
		*nbuffers = max_t(unsigned int, *nbuffers, MTK_CAMERA_PREVIEW_MIN_BUFFERS);

	vq->min_buffers_needed = *nbuffers;

	dev_dbg(ctx->dev, "cam%d:[%d]\t type = %d, get %d plane(s), %d buffer(s) of size %d %d\n",
		ctx->camera_id, ctx->id, vq->type, *nplanes, *nbuffers,
		sizes[0], sizes[1]);
	return 0;
}

static int vb2ops_camera_buf_prepare(struct vb2_buffer *vb)
{
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);

	dev_dbg(ctx->dev, "cam%d:[%d] (%d) id=%d, state=%d, vb=%p\n",
		ctx->camera_id, ctx->id, vb->vb2_queue->type,
		vb->index, vb->state, vb);

	return 0;
}

static void vb2ops_camera_buf_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vb2_v4l2 = container_of(vb,
				struct vb2_v4l2_buffer, vb2_buf);
	struct camera_buffer *buffer = container_of(vb2_v4l2,
				struct camera_buffer, vb);
	struct mtk_camera_mem *fb = &buffer->mem;
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	int i, ret = 0;

	dev_dbg(ctx->dev, "cam%d:[%d] (%d) id=%d, state=%d, vb=%p\n",
		ctx->camera_id, ctx->id, vb->vb2_queue->type,
		vb->index, vb->state, vb);

	fb->size = 0;
	fb->index = vb->index;
	fb->num_planes = vb->num_planes;
	for (i = 0; i < vb->num_planes; i++) {
		fb->planes[i].dma_addr = vb2_dma_contig_plane_dma_addr(vb, i);
		fb->planes[i].size = vb2_plane_size(vb, i);
		fb->size += fb->planes[i].size;
		dev_dbg(ctx->dev, "plane %d, dma_addr 0x%llx, size %lu\n",
			i, (uint64_t)fb->planes[i].dma_addr, fb->planes[i].size);
	}
	dev_dbg(ctx->dev, "total size %lu\n", fb->size);

	ret = camera_if_capture(ctx, fb);
	if (ret) {
		dev_err(ctx->dev, "cam%d:[%d]: camera_if_capture() fail ret=%d\n",
			ctx->camera_id, ctx->id, ret);
	}
}

static int vb2ops_camera_buf_init(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vb2_v4l2 = container_of(vb,
				struct vb2_v4l2_buffer, vb2_buf);
	struct vb2_plane *vb_plane;
	struct vb2_queue *vb_queue = vb->vb2_queue;
	struct camera_buffer *buffer = container_of(vb2_v4l2,
				struct camera_buffer, vb);
	struct mtk_camera_mem *fb = &buffer->mem;
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	struct mtk_q_data *q_data = &ctx->q_data;
	int i, ret = 0;

	dev_dbg(ctx->dev, "cam%d:[%d] (%d) id=%d, state=%d, vb=%p vb->num_planes:%d\n",
		ctx->camera_id, ctx->id, vb->vb2_queue->type,
		vb->index, vb->state, vb, vb->num_planes);

	fb->size = 0;
	fb->index = vb->index;
	fb->format = q_data->fmt->fourcc;
	fb->num_planes = vb->num_planes;
	for (i = 0; i < vb->num_planes; i++) {
		fb->planes[i].dma_addr = vb2_dma_contig_plane_dma_addr(vb, i);
		fb->planes[i].size = vb2_plane_size(vb, i);

		vb_plane = &vb->planes[i];
		fb->planes[i].dbuf = vb_queue->mem_ops->get_dmabuf(vb,
					vb_plane->mem_priv, O_CLOEXEC | O_RDWR);
		if (!fb->planes[i].dbuf) {
			dev_err(ctx->dev, "cam%d:[%d]: get_dmabuf() fail, buffer index %d\n",
				ctx->camera_id, ctx->id, fb->index);
			return -EINVAL;
		}

		fb->size += fb->planes[i].size;
		dev_dbg(ctx->dev, "plane %d, dma_addr 0x%llx, size %lu format:0x%x\n",
				i, (uint64_t)fb->planes[i].dma_addr,
				fb->planes[i].size, fb->format);
	}
	dev_dbg(ctx->dev, "total size %lu fb->num_planes:%d\n", fb->size, fb->num_planes);

	if (ctx->state == MTK_STATE_INIT) {
		ret = camera_if_init_buffer(ctx, fb);
		if (ret) {
			dev_err(ctx->dev, "cam%d:[%d]: camera_if_use_buffer() fail ret=%d\n",
				ctx->camera_id, ctx->id, ret);
			return -EINVAL;
		}
	} else
		dev_err(ctx->dev, "cam%d:[%d] state=(%x) invalid call\n",
			ctx->camera_id, ctx->id, ctx->state);

	return 0;
}

static void vb2ops_camera_buf_deinit(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vb2_v4l2 = container_of(vb,
				struct vb2_v4l2_buffer, vb2_buf);
	struct camera_buffer *buffer = container_of(vb2_v4l2,
				struct camera_buffer, vb);
	struct mtk_camera_mem *fb = &buffer->mem;
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	int ret = 0;

	dev_dbg(ctx->dev, "cam%d:[%d] (%d) id=%d, state=%d, vb=%p\n",
			ctx->camera_id, ctx->id, vb->vb2_queue->type,
			vb->index, vb->state, vb);

	if (ctx->state == MTK_STATE_INIT) {
		ret = camera_if_deinit_buffer(ctx, fb);
		if (ret) {
			dev_err(ctx->dev, "cam%d:[%d]: camera_if_use_buffer() fail ret=%d\n",
				ctx->camera_id, ctx->id, ret);
			return;
		}
	} else
		dev_err(ctx->dev, "cam%d:[%d] state=(%x) invalid call\n",
			ctx->camera_id, ctx->id, ctx->state);
}

static int vb2ops_camera_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(q);
	int ret = 0;

	dev_dbg(ctx->dev, "cam%d:[%d] (%d) state=(%x)\n",
			ctx->camera_id, ctx->id, q->type, ctx->state);

	if (ctx->state == MTK_STATE_INIT) {
		ret = camera_if_start_stream(ctx);
		if (ret) {
			dev_err(ctx->dev, "cam%d:[%d]: camera_if_start_stream() fail ret=%d\n",
				ctx->camera_id, ctx->id, ret);
			return -EINVAL;
		}
		ctx->state = MTK_STATE_START;
	} else
		dev_err(ctx->dev, "cam%d:[%d] state=(%x) invalid call\n",
			ctx->camera_id, ctx->id, ctx->state);

	return 0;
}

static void vb2ops_camera_stop_streaming(struct vb2_queue *q)
{
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(q);
	int ret = 0;

	dev_dbg(ctx->dev, "cam%d:[%d] (%d) state=(%x)\n",
		ctx->camera_id, ctx->id, q->type, ctx->state);

	if (ctx->state == MTK_STATE_START) {
		ret = camera_if_stop_stream(ctx);
		if (ret) {
			dev_err(ctx->dev, "cam%d:[%d]: camera_if_stop_stream() fail ret=%d\n",
				ctx->camera_id, ctx->id, ret);
			return;
		}
		ctx->state = MTK_STATE_FLUSH;

		vb2_wait_for_all_buffers(q);
		ctx->state = MTK_STATE_INIT;

		dev_dbg(ctx->dev, "cam%d:[%d] wait buffer done\n", ctx->camera_id, ctx->id);
	} else
		dev_err(ctx->dev, "cam%d:[%d] state=(%x) invalid call\n",
			ctx->camera_id, ctx->id, ctx->state);
}

static const struct vb2_ops mtk_camera_vb2_ops = {
	.queue_setup		= vb2ops_camera_queue_setup,
	.buf_prepare		= vb2ops_camera_buf_prepare,
	.buf_queue			= vb2ops_camera_buf_queue,
	.wait_prepare		= vb2_ops_wait_prepare,
	.wait_finish		= vb2_ops_wait_finish,
	.buf_init			= vb2ops_camera_buf_init,
	.buf_cleanup		= vb2ops_camera_buf_deinit,
	.start_streaming	= vb2ops_camera_start_streaming,
	.stop_streaming		= vb2ops_camera_stop_streaming,
};

static struct mtk_camera_fmt *mtk_camera_find_format(struct v4l2_format *f,
			struct mtk_camera_fmt *formats, int num_formats)
{
	struct mtk_camera_fmt *fmt;
	unsigned int i;

	for (i = 0; i < num_formats; i++) {
		fmt = &formats[i];
		if (f->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
			if (fmt->fourcc == f->fmt.pix_mp.pixelformat) {
				pr_debug("matched mplane format %s\n", fmt->name);
				return fmt;
			}
		} else {
			if (fmt->fourcc == f->fmt.pix.pixelformat) {
				pr_debug("matched format %s\n", fmt->name);
				return fmt;
			}
		}
	}

	return NULL;
}

static struct v4l2_frmsize_discrete *mtk_camera_find_sizes(u32 width, u32 height,
			struct v4l2_frmsize_discrete *sizes, int num_sizes)
{
	unsigned int i;

	for (i = 0; i < num_sizes; i++) {
		if (sizes[i].height == height && sizes[i].width == width)
			return &sizes[i];
	}

	return NULL;
}

static int mtk_camera_try_fmt_mplane(struct v4l2_format *f, struct mtk_camera_fmt *fmt)
{
	struct v4l2_pix_format_mplane *pix_fmt_mp = &f->fmt.pix_mp;
	unsigned int bytesperline = 0;
	int i;
	u32 memory_type = 0;
	u32 org_w, org_h;

	pix_fmt_mp->field = V4L2_FIELD_NONE;
	/* Clamp the width and height. */
	pix_fmt_mp->width = clamp(pix_fmt_mp->width,
				MTK_VIDEO_CAPTURE_MIN_WIDTH,
				MTK_VIDEO_CAPTURE_MAX_WIDTH);
	pix_fmt_mp->height = clamp(pix_fmt_mp->height,
				MTK_VIDEO_CAPTURE_MIN_HEIGHT,
				MTK_VIDEO_CAPTURE_MAX_HEIGHT);
	pix_fmt_mp->num_planes = fmt->num_planes;

	org_w = pix_fmt_mp->width;
	org_h = pix_fmt_mp->height;

	if ((pix_fmt_mp->pixelformat == V4L2_PIX_FMT_YUV420M) ||
	    (pix_fmt_mp->pixelformat == V4L2_PIX_FMT_YUV420M) ||
	    (pix_fmt_mp->pixelformat == V4L2_PIX_FMT_NV12) ||
	    (pix_fmt_mp->pixelformat == V4L2_PIX_FMT_NV12M) ||
	    (pix_fmt_mp->pixelformat == V4L2_PIX_FMT_NV21M))
		memory_type = pix_fmt_mp->field;
	else
		memory_type = 0;

	pr_debug("mplane format %s, width:%u, height:%u, memory_type: %d\n",
		fmt->name, pix_fmt_mp->width, pix_fmt_mp->height, memory_type);

	if (memory_type) {
		/* for dma buffer handle */
		pix_fmt_mp->width = ALIGN_CEIL(pix_fmt_mp->width, 16);
		pix_fmt_mp->height = ALIGN_CEIL(pix_fmt_mp->height, 32);
	}

	for (i = 0; i < pix_fmt_mp->num_planes; i++) {
		memset(pix_fmt_mp->plane_fmt[i].reserved, 0,
			   sizeof(pix_fmt_mp->plane_fmt[i].reserved));
	}

	if (fmt->fourcc == V4L2_PIX_FMT_JPEG) {
		pix_fmt_mp->plane_fmt[0].bytesperline = 0;
		pix_fmt_mp->plane_fmt[0].sizeimage =
			round_up(pix_fmt_mp->plane_fmt[0].sizeimage, 128);
		if (pix_fmt_mp->plane_fmt[0].sizeimage == 0)
			pix_fmt_mp->plane_fmt[0].sizeimage = MTK_CAMERA_JPEG_DEFAULT_SIZEIMAGE;
	} else {
		for (i = 0; i < pix_fmt_mp->num_planes; i++) {
			bytesperline = pix_fmt_mp->width * fmt->bpp[i] / 8;
			pix_fmt_mp->plane_fmt[i].bytesperline = bytesperline;
			pix_fmt_mp->plane_fmt[i].sizeimage = pix_fmt_mp->height * bytesperline;
			pr_debug("mplane format %s, width:%u, height:%u, sizeimage: %u\n",
				fmt->name, pix_fmt_mp->width, pix_fmt_mp->height,
				pix_fmt_mp->plane_fmt[i].sizeimage);
		}
	}

	pix_fmt_mp->flags = 0;

	if (memory_type) {
		pix_fmt_mp->width = org_w;
		pix_fmt_mp->height = org_h;
	}

	pix_fmt_mp->colorspace = fmt->colorspace;
	pix_fmt_mp->ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
	pix_fmt_mp->quantization = V4L2_QUANTIZATION_DEFAULT;
	pix_fmt_mp->xfer_func = V4L2_XFER_FUNC_DEFAULT;

	memset(pix_fmt_mp->reserved, 0, sizeof(pix_fmt_mp->reserved));
	pix_fmt_mp->request_fd = 0;

	return 0;
}


/* -----------------------------------------------------------------------------
 * V4L2 ioctls
 */
static int
camera_querycap(struct file *file, void *fh, struct v4l2_capability *cap)
{
	struct mtk_camera_stream *stream = video_drvdata(file);
	struct v4l2_device *v4l2_dev = stream->video.v4l2_dev;
	struct mtk_camera_dev *dev = v4l2_dev_to_dev(v4l2_dev);

	strncpy(cap->driver, MTK_CAMERA_DEVICE, sizeof(cap->driver) - 1);
	strncpy(cap->bus_info, dev->platform, sizeof(cap->bus_info) - 1);
	strncpy(cap->card, MTK_CAMERA_NAME, sizeof(cap->card) - 1);

	return 0;
}

static int
camera_enum_format(struct file *file, void *fh,
					struct v4l2_fmtdesc *f)
{
	struct mtk_camera_fh *handle = fh;
	const struct mtk_camera_fmt *fmt;
	struct mtk_camera_fmt *formats;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int num_formats;

	formats = ctx->q_data.formats;
	num_formats = ctx->q_data.num_formats;

	if (f->index >= num_formats)
		return -EINVAL;

	fmt = &formats[f->index];

	f->pixelformat = fmt->fourcc;
	memset(f->reserved, 0, sizeof(f->reserved));

	return 0;
}

static int
camera_get_format_mplane(struct file *file, void *fh,
					struct v4l2_format *format)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	struct v4l2_pix_format_mplane *pix_fmt_mp = &format->fmt.pix_mp;
	struct mtk_q_data *q_data = &ctx->q_data;
	int i;

	dev_dbg(ctx->dev, "cam%d:%s [%d]\n", ctx->camera_id, __func__, ctx->id);

	pix_fmt_mp->field = V4L2_FIELD_NONE;
	pix_fmt_mp->colorspace = ctx->colorspace;
	pix_fmt_mp->ycbcr_enc = ctx->ycbcr_enc;
	pix_fmt_mp->quantization = ctx->quantization;
	pix_fmt_mp->xfer_func = ctx->xfer_func;
	pix_fmt_mp->width  = q_data->width;
	pix_fmt_mp->height = q_data->height;
	pix_fmt_mp->pixelformat = q_data->fmt->fourcc;
	pix_fmt_mp->num_planes = q_data->fmt->num_planes;

	for (i = 0; i < pix_fmt_mp->num_planes; i++) {
		pix_fmt_mp->plane_fmt[i].bytesperline = q_data->bytesperline[i];
		pix_fmt_mp->plane_fmt[i].sizeimage = q_data->sizeimage[i];
	}

	memset(pix_fmt_mp->reserved, 0, sizeof(pix_fmt_mp->reserved));
	pix_fmt_mp->request_fd = 0;

	return 0;
}

static int
camera_set_format_mplane(struct file *file, void *fh,
					struct v4l2_format *format)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	struct v4l2_pix_format_mplane *pix_mp;
	struct mtk_q_data *q_data = &ctx->q_data;
	struct mtk_camera_fmt *fmt;
	struct v4l2_frmsize_discrete *frmsize;
	uint32_t size[3];
	int i, ret = 0;

	ret = mtk_camera_acquire_privileges(handle);
	if (ret < 0)
		return ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d]\n", ctx->camera_id, __func__, ctx->id);

	if (vb2_is_busy(&ctx->queue)) {
		dev_err(ctx->dev, "capture buffers already requested\n");
		return -EBUSY;
	}

	if (!q_data) {
		dev_err(ctx->dev, "cam%d:[%d]: q_data is NULL\n", ctx->camera_id, ctx->id);
		return -EINVAL;
	}
	pix_mp = &format->fmt.pix_mp;

	fmt = mtk_camera_find_format(format, q_data->formats, q_data->num_formats);
	if (fmt == NULL) {
		format->fmt.pix_mp.pixelformat = q_data->formats->fourcc;
		fmt = mtk_camera_find_format(format, q_data->formats, q_data->num_formats);
	}
	q_data->fmt = fmt;

	mtk_camera_try_fmt_mplane(format, q_data->fmt);

	for (i = 0; i < fmt->num_planes; i++) {
		q_data->bytesperline[i] = pix_mp->plane_fmt[i].bytesperline;
		q_data->sizeimage[i] = pix_mp->plane_fmt[i].sizeimage;
		dev_dbg(ctx->dev, "format %s, bytesperline:%u, sizeimage: %u\n",
			fmt->name, q_data->bytesperline[i], q_data->sizeimage[i]);
	}

	frmsize = mtk_camera_find_sizes(pix_mp->width, pix_mp->height,
				    fmt->sizes, fmt->num_sizes);
	if (frmsize == NULL) {
		q_data->width = pix_mp->width = fmt->sizes[0].width;
		q_data->height = pix_mp->height = fmt->sizes[0].height;
	} else {
		q_data->width = pix_mp->width;
		q_data->height = pix_mp->height;
	}

	size[0] = pix_mp->width;
	size[1] = pix_mp->height;
	size[2] = fmt->fourcc;

	ctx->colorspace = format->fmt.pix_mp.colorspace;
	ctx->ycbcr_enc = format->fmt.pix_mp.ycbcr_enc;
	ctx->quantization = format->fmt.pix_mp.quantization;
	ctx->xfer_func = format->fmt.pix_mp.xfer_func;

	ctx->queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	ctx->queue.is_multiplanar = 1;

	if (!ctx->cam_if_rdy) {
		ret = camera_if_init(ctx);
		if (ret) {
			dev_err(ctx->dev, "cam%d:[%d]: camera_if_init() fail ret=%d\n",
				ctx->camera_id, ctx->id, ret);
			return -EINVAL;
		}
	}

	if (ctx->state == MTK_STATE_FREE || ctx->state == MTK_STATE_INIT) {
		dev_dbg(ctx->dev, "%s , SET_PARAM_FRAME_SIZE,format:%d\n", __func__, size[2]);
		camera_if_set_param(ctx, SET_PARAM_FRAME_SIZE, (void *)size);
		ctx->state = MTK_STATE_INIT;
	} else {
		dev_err(ctx->dev, "cam%d:[%d] state=(%x) invalid call\n",
			ctx->camera_id, ctx->id, ctx->state);
	}

	dev_dbg(ctx->dev, "ret %d\n", ret);

	return 0;
}

static int
camera_try_format_mplane(struct file *file, void *fh,
					struct v4l2_format *format)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_fmt *fmt;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	struct mtk_camera_fmt *formats;
	int num_formats;

	formats = ctx->q_data.formats;
	num_formats = ctx->q_data.num_formats;

	fmt = mtk_camera_find_format(format, formats, num_formats);
	if (fmt == NULL) {
		format->fmt.pix_mp.pixelformat = formats->fourcc;
		fmt = mtk_camera_find_format(format, formats, num_formats);
	}

	return mtk_camera_try_fmt_mplane(format, fmt);
}

static int
camera_get_param(struct file *file, void *fh,
					struct v4l2_streamparm *parm)
{
	parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = 30;
	return 0;
}

static int
camera_set_param(struct file *file, void *fh,
					struct v4l2_streamparm *parm)
{
	parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = 30;
	return 0;
}

static int
camera_enum_framesizes(struct file *file, void *fh,
					struct v4l2_frmsizeenum *fsize)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	struct mtk_camera_fmt *formats;
	int num_formats;
	struct v4l2_frmsize_discrete *sizes;
	int i;

	formats = ctx->q_data.formats;
	num_formats = ctx->q_data.num_formats;

	for (i = 0; i < num_formats; i++) {
		if (formats[i].fourcc == fsize->pixel_format) {
			sizes = formats[i].sizes;
			break;
		}
	}

	if (i >= num_formats)
		return -EINVAL;

	if (fsize->index >= formats[i].num_sizes)
		return -EINVAL;

	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete = sizes[fsize->index];
	return 0;
}

/* timeperframe is arbitrary and continuous */
static int
camera_enum_frameintervals(struct file *file, void *fh,
					struct v4l2_frmivalenum *fival)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	struct mtk_camera_fmt *formats;
	struct mtk_camera_fmt *fmt;
	int num_formats;
	int i;

	formats = ctx->q_data.formats;
	num_formats = ctx->q_data.num_formats;

	for (i = 0; i < num_formats; i++) {
		if (formats[i].fourcc == fival->pixel_format)
			break;
	}

	if (i >= num_formats)
		return -EINVAL;

	fmt = &formats[i];

	for (i = 0; i < fmt->num_sizes; i++) {
		if (fmt->sizes[i].height == fival->height &&
		   fmt->sizes[i].width == fival->width)
			break;
	}

	if (i >= fmt->num_sizes)
		return -EINVAL;

	if (fival->index != 0)
		return -EINVAL;

	fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	fival->discrete.numerator = 1;
	fival->discrete.denominator = 30;
	return 0;
}

static int
camera_reqbufs(struct file *file, void *fh, struct v4l2_requestbuffers *rb)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d], cnt[%d] mem[%d] type[%d, %d] point[0x%p]\n",
		ctx->camera_id, __func__, ctx->id, rb->count, rb->memory, rb->type,
		ctx->queue.type, &ctx->queue);
	ret = mtk_camera_acquire_privileges(handle);
	if (ret < 0)
		return ret;

	ret = vb2_reqbufs(&ctx->queue, rb);
	if (ret < 0 || rb->count == 0)
		mtk_camera_dismiss_privileges(handle);

	dev_dbg(ctx->dev, "ret %d cnt[%d]\n", ret, rb->count);

	return ret;
}

static int
camera_querybuf(struct file *file, void *fh, struct v4l2_buffer *b)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d], queue %p\n",
		ctx->camera_id, __func__, ctx->id, &ctx->queue);

	if (!mtk_camera_has_privileges(handle))
		return -EBUSY;

	ret = vb2_querybuf(&ctx->queue, b);

	dev_dbg(ctx->dev, "ret %d\n", ret);

	return ret;
}

static int
camera_expbuf(struct file *file, void *fh, struct v4l2_exportbuffer *p)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d], fd[%d], flag[%d], idx[%d], plane[%d]\n",
		ctx->camera_id, __func__, ctx->id, p->fd, p->flags, p->index, p->plane);

	if (!mtk_camera_has_privileges(handle))
		return -EBUSY;

	ret = vb2_expbuf(&ctx->queue, p);

	dev_dbg(ctx->dev, "ret %d\n", ret);

	return ret;
}

static int
camera_qbuf(struct file *file, void *fh, struct v4l2_buffer *b)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d], length[%d], bytes[%d]\n",
		ctx->camera_id, __func__, ctx->id, b->length, b->bytesused);

	if (ctx->state == MTK_STATE_FREE || ctx->state == MTK_STATE_FLUSH) {
		dev_err(ctx->dev, "cam%d:[%d] state=(%x) invalid call\n",
			ctx->camera_id, ctx->id, ctx->state);
		return -EIO;
	}

	if (!mtk_camera_has_privileges(handle))
		return -EBUSY;

	ret = vb2_qbuf(&ctx->queue, NULL, b);

	dev_dbg(ctx->dev, "ret %d\n", ret);

	return ret;
}

static int
camera_dqbuf(struct file *file, void *fh, struct v4l2_buffer *b)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d], length[%d], bytes[%d]\n",
		ctx->camera_id, __func__, ctx->id, b->length, b->bytesused);

	if (!mtk_camera_has_privileges(handle))
		return -EBUSY;

	ret = vb2_dqbuf(&ctx->queue, b, file->f_flags & O_NONBLOCK);

	dev_dbg(ctx->dev, "ret %d\n", ret);

	return ret;
}

int camera_streamon(struct file *file, void *fh, enum v4l2_buf_type type)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d], queue %p\n",
		ctx->camera_id, __func__, ctx->id, &ctx->queue);

	if (!mtk_camera_has_privileges(handle))
		return -EBUSY;

	ret = vb2_streamon(&ctx->queue, type);

	dev_dbg(ctx->dev, "ret %d\n", ret);

	return ret;
}

int camera_streamoff(struct file *file, void *fh, enum v4l2_buf_type type)
{
	struct mtk_camera_fh *handle = fh;
	struct mtk_camera_ctx *ctx = handle->stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:%s [%d], queue %p\n",
		ctx->camera_id, __func__, ctx->id, &ctx->queue);

	if (!mtk_camera_has_privileges(handle))
		return -EBUSY;

	ret = vb2_streamoff(&ctx->queue, type);

	dev_dbg(ctx->dev, "ret %d\n", ret);

	return ret;
}

static int
camera_enum_input(struct file *file, void *fh, struct v4l2_input *input)
{
	if (input->index > 0)
		return -EINVAL;

	strncpy(input->name, "camera", sizeof(input->name) - 1);
	input->type = V4L2_INPUT_TYPE_CAMERA;

	return 0;
}

static int
camera_g_input(struct file *file, void *fh, unsigned int *input)
{
	*input = 0;

	return 0;
}

static int
camera_s_input(struct file *file, void *fh, unsigned int input)
{
	return input == 0 ? 0 : -EINVAL;
}

static const struct v4l2_ioctl_ops mtk_camera_ioctl_ops = {
	.vidioc_querycap		= camera_querycap,
	.vidioc_enum_framesizes		= camera_enum_framesizes,
	.vidioc_enum_frameintervals	= camera_enum_frameintervals,
	.vidioc_enum_fmt_vid_cap	= camera_enum_format,
	.vidioc_try_fmt_vid_cap_mplane	= camera_try_format_mplane,
	.vidioc_s_fmt_vid_cap_mplane	= camera_set_format_mplane,
	.vidioc_g_fmt_vid_cap_mplane	= camera_get_format_mplane,
	.vidioc_reqbufs			= camera_reqbufs,
	.vidioc_querybuf		= camera_querybuf,
	.vidioc_expbuf			= camera_expbuf,
	.vidioc_qbuf			= camera_qbuf,
	.vidioc_dqbuf			= camera_dqbuf,
	.vidioc_streamon		= camera_streamon,
	.vidioc_streamoff		= camera_streamoff,
	.vidioc_enum_input		= camera_enum_input,
	.vidioc_g_input			= camera_g_input,
	.vidioc_s_input			= camera_s_input,
	.vidioc_g_parm			= camera_get_param,
	.vidioc_s_parm			= camera_set_param,
	.vidioc_subscribe_event		= v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event	= v4l2_event_unsubscribe,
};

static void mtk_handle_buffer(struct mtk_camera_mem *fb)
{
	struct camera_buffer *buffer = container_of(fb,
		struct camera_buffer, mem);
	struct vb2_v4l2_buffer *vb2_v4l2 = &buffer->vb;
	struct vb2_buffer *vb = &vb2_v4l2->vb2_buf;
	struct mtk_camera_ctx *ctx = vb2_get_drv_priv(vb->vb2_queue);
	int i = 0;

	dev_dbg(ctx->dev, "cam%d:[%d] (%d) id=%d, state=%d, vb=%p\n",
		ctx->camera_id, ctx->id, vb->vb2_queue->type,
		vb->index, vb->state, vb);

	/*should be replaced by payload */
	for (i = 0; i < fb->num_planes; i++)
		vb2_set_plane_payload(vb, i, fb->planes[i].size);

	if (fb->status == BUFFER_FILLED)
		vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
	else
		vb2_buffer_done(vb, VB2_BUF_STATE_ERROR);
}

void mtk_camera_unlock(struct mtk_camera_ctx *ctx)
{
	mutex_unlock(&ctx->stream->capture_mutex);
}

void mtk_camera_lock(struct mtk_camera_ctx *ctx)
{
	mutex_lock(&ctx->stream->capture_mutex);
}

static void mtk_camera_release(struct mtk_camera_ctx *ctx)
{
	camera_if_deinit(ctx);
	ctx->state = MTK_STATE_FREE;
}

void mtk_camera_set_default_params(struct mtk_camera_ctx *ctx)
{
	struct mtk_q_data *q_data = &ctx->q_data;
	unsigned int i;

	memset(q_data, 0, sizeof(struct mtk_q_data));
	q_data->width  = MTK_VIDEO_CAPTURE_DEF_WIDTH;
	q_data->height = MTK_VIDEO_CAPTURE_DEF_HEIGHT;

	/* TODO: Use VCU to query supported format instead of fixed */
	if (ctx->stream_id == STREAM_PREVIEW || ctx->stream_id == STREAM_VIDEO) {
		q_data->fmt = mtk_camera_formats_video;
		q_data->formats = mtk_camera_formats_video;
		q_data->num_formats = ARRAY_SIZE(mtk_camera_formats_video);
		for (i = 0; i < q_data->fmt->num_planes; ++i) {
			q_data->bytesperline[i] = q_data->width * q_data->fmt->bpp[i] / 8;
			q_data->sizeimage[i] = q_data->bytesperline[i] * q_data->height;
		}
	} else if (ctx->stream_id == STREAM_CAPTURE) {
		q_data->fmt = mtk_camera_formats_capture;
		q_data->formats = mtk_camera_formats_capture;
		q_data->num_formats = ARRAY_SIZE(mtk_camera_formats_capture);
		q_data->bytesperline[0] = 0;
		q_data->sizeimage[0] = MTK_CAMERA_JPEG_DEFAULT_SIZEIMAGE;
	}
	q_data->field = V4L2_FIELD_NONE;

	ctx->fh.ctrl_handler = &ctx->ctrl_hdl;
	ctx->colorspace = q_data->fmt->colorspace;
	ctx->ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
	ctx->quantization = V4L2_QUANTIZATION_DEFAULT;
	ctx->xfer_func = V4L2_XFER_FUNC_DEFAULT;
}

/* -----------------------------------------------------------------------------
 * V4L2 file operations
 */

static int fops_camera_open(struct file *file)
{
	struct mtk_camera_stream *stream = video_drvdata(file);
	struct mtk_camera_fh *handle;
	int ret = 0;

	/* Create the device handle. */
	handle = kzalloc(sizeof(*handle), GFP_KERNEL);
	if (handle == NULL)
		return -ENOMEM;

	mutex_lock(&stream->dev_mutex);
	stream->users++;
	mutex_unlock(&stream->dev_mutex);

	v4l2_fh_init(&handle->vfh, &stream->video);
	v4l2_fh_add(&handle->vfh);
	handle->state = MTK_HANDLE_PASSIVE;
	handle->stream = stream;
	file->private_data = handle;
	return 0;
}

static int fops_camera_release(struct file *file)
{
	struct mtk_camera_stream *stream = video_drvdata(file);
	struct mtk_camera_fh *handle = file->private_data;

	mutex_lock(&stream->dev_mutex);

	/* Only free resources if this is a privileged handle. */
	if (mtk_camera_has_privileges(handle))
		vb2_queue_release(stream->queue);

	/* Release the file handle. */
	mtk_camera_dismiss_privileges(handle);

	v4l2_fh_del(&handle->vfh);
	v4l2_fh_exit(&handle->vfh);
	kfree(handle);
	file->private_data = NULL;

	if (--stream->users == 0)
		mtk_camera_release(stream->curr_ctx);

	mutex_unlock(&stream->dev_mutex);

	return 0;
}

static unsigned int fops_camera_poll(struct file *file, poll_table *wait)
{
	struct mtk_camera_stream *stream = video_drvdata(file);
	struct mtk_camera_ctx *ctx = stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d,[%d] video capture\n", ctx->camera_id, ctx->id);

	mutex_lock(&stream->dev_mutex);
	ret = vb2_poll(&ctx->queue, file, wait);
	mutex_unlock(&stream->dev_mutex);

	dev_dbg(ctx->dev, "ret %d\n", ret);
	return ret;
}

static int fops_camera_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct mtk_camera_stream *stream = video_drvdata(file);
	struct mtk_camera_ctx *ctx = stream->curr_ctx;
	int ret;

	dev_dbg(ctx->dev, "cam%d:[%d] video capture\n", ctx->camera_id, ctx->id);

	ret = vb2_mmap(&ctx->queue, vma);

	dev_dbg(ctx->dev, "ret %d\n", ret);
	return ret;
}

static const struct v4l2_file_operations mtk_camera_fops = {
	.owner		= THIS_MODULE,
	.open		= fops_camera_open,
	.release	= fops_camera_release,
	.poll		= fops_camera_poll,
	.unlocked_ioctl	= video_ioctl2,
	.mmap		= fops_camera_mmap,
};

int mtk_camera_stream_create_context(struct mtk_camera_stream *stream)
{
	struct mtk_camera_ctx *ctx = NULL;
	struct vb2_queue *queue;
	int ret = 0;

	ctx = devm_kzalloc(&stream->plat_dev->dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	mutex_lock(&stream->dev_mutex);
	ctx->dev = &stream->plat_dev->dev;
	ctx->camera_id = stream->camera_id;
	ctx->stream_id = stream->stream_id;
	INIT_LIST_HEAD(&ctx->list);
	ctx->stream = stream;
	ctx->cam_if_rdy = false;

	ret = mtk_camera_ctrls_setup(ctx);
	if (ret) {
		dev_err(ctx->dev, "Failed to setup video capture controls\n");
		goto err_ctrls_setup;
	}

	queue = &ctx->queue;
	queue->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	queue->io_modes	= VB2_DMABUF | VB2_MMAP;
	queue->drv_priv	= ctx;
	queue->buf_struct_size = sizeof(struct camera_buffer);
	queue->ops		= &mtk_camera_vb2_ops;
	queue->mem_ops		= &vb2_dma_contig_memops;
	queue->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	queue->lock		= &ctx->stream->dev_mutex;
	queue->allow_zero_bytesused = 1;
	queue->dev = &ctx->stream->plat_dev->dev;

	ret = vb2_queue_init(&ctx->queue);
	if (ret < 0) {
		dev_err(ctx->dev, "Failed to initialize videobuf2 queue\n");
		goto err_vb2_init;
	}

	stream->queue = &ctx->queue;
	stream->curr_ctx = ctx;

	mtk_camera_set_default_params(ctx);

	ctx->callback = mtk_handle_buffer;

	mutex_unlock(&stream->dev_mutex);
	dev_dbg(ctx->dev, "cam%d:%s capture [%d]\n", ctx->camera_id,
		dev_name(&stream->plat_dev->dev), ctx->id);
	return ret;

err_vb2_init:
	v4l2_ctrl_handler_free(&ctx->ctrl_hdl);
err_ctrls_setup:
	mutex_unlock(&stream->dev_mutex);

	return ret;
}

int mtk_camera_register_video_device(struct mtk_camera_dev *dev,
				    struct mtk_camera_stream *stream,
				    struct video_device *video)
{
	int ret;

	video->fops = &mtk_camera_fops;
	video->ioctl_ops = &mtk_camera_ioctl_ops;
	video->release = video_device_release;
	video->lock = &stream->dev_mutex;
	video->v4l2_dev  = &dev->v4l2_dev;
	video->vfl_type = VFL_TYPE_VIDEO;
	video->device_caps = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE_MPLANE;

	snprintf(video->name, sizeof(video->name), "%s@%d-%s",
		MTK_CAMERA_NAME, stream->camera_id, stream_type[stream->stream_id]);

	video_set_drvdata(video, stream);

	ret = video_register_device(video, VFL_TYPE_VIDEO, -1);
	if (ret) {
		dev_err(stream->curr_ctx->dev, "Failed to register video device\n");
		video_device_release(video);
		return ret;
	}

	dev_dbg(stream->curr_ctx->dev, "video capture registered as /dev/video%d\n",
			video->num);

	return 0;
}

int mtk_camera_create_stream(struct mtk_camera_dev *dev)
{
	int stream_id;
	int ret;
	struct device *device;
	struct platform_device *pdev;

	pdev = dev->plat_dev;
	device = &pdev->dev;

	for (stream_id = STREAM_PREVIEW; stream_id < STREAM_NUM; ++stream_id) {
		struct mtk_camera_stream *stream;

		stream = devm_kzalloc(device, sizeof(*stream), GFP_KERNEL);
		if (!stream)
			return -ENOMEM;

		stream->plat_dev = pdev;
		stream->vcu_plat_dev = vcu_get_plat_device(pdev);
		if (stream->vcu_plat_dev == NULL) {
			dev_err(device, "[VCU] vcu device in not ready\n");
			return -EPROBE_DEFER;
		}

		mutex_init(&stream->capture_mutex);
		mutex_init(&stream->dev_mutex);

		stream->stream_id = stream_id;
		stream->camera_id = dev->camera_id;

		ret = mtk_camera_stream_create_context(stream);
		if (ret) {
			dev_err(device, "failed to create context stream_id=%d ret=%d\n",
				stream_id, ret);
			return ret;
		}

		ret = mtk_camera_register_video_device(dev, stream, &stream->video);
		if (ret) {
			dev_err(device, "failed to register video device stream_id=%d ret=%d\n",
				stream_id, ret);
			return ret;
		}

		list_add_tail(&stream->list, &dev->streams);
	}
	return 0;
}

static int mtk_camera_probe(struct platform_device *pdev)
{
	struct mtk_camera_dev *dev;
	struct device *device;
	int camera_id;
	int ret;

	device = &pdev->dev;
	dev = devm_kzalloc(device, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->plat_dev = pdev;

	snprintf(dev->v4l2_dev.name, sizeof(dev->v4l2_dev.name), "mtk-camera");

	ret = of_property_read_u32(device->of_node, "mediatek,cameraid", &camera_id);
	if (ret) {
		dev_err(device, "failed to find mediatek,camera-id\n");
		return ret;
	}
	dev->camera_id = camera_id;

	ret = of_property_read_string(device->of_node, "mediatek,platform", &dev->platform);
	if (ret) {
		dev_err(device, "failed to find mediatek,platform\n");
		return ret;
	}

	ret = v4l2_device_register(device, &dev->v4l2_dev);
	if (ret) {
		dev_err(device, "v4l2_device_register err=%d\n", ret);
		return ret;
	}

	INIT_LIST_HEAD(&dev->streams);
	ret = mtk_camera_create_stream(dev);
	if (ret) {
		dev_err(device, "mtk_camera_create_stream err=%d\n", ret);
		v4l2_device_unregister(&dev->v4l2_dev);
		return ret;
	}

	platform_set_drvdata(pdev, dev);
	return 0;
}

static const struct of_device_id mtk_camera_match[] = {
	{.compatible = "mediatek,camera",},
	{},
};

MODULE_DEVICE_TABLE(of, mtk_camera_match);

static int mtk_camera_remove(struct platform_device *pdev)
{
	struct mtk_camera_dev *dev;
	struct mtk_camera_stream *stream;

	dev = platform_get_drvdata(pdev);

	dev_dbg(&pdev->dev, "%s dev %p\n", __func__, dev);
	list_for_each_entry(stream, &dev->streams, list) {
		if (!video_is_registered(&stream->video))
			continue;

		video_unregister_device(&stream->video);

		mutex_destroy(&stream->capture_mutex);
		mutex_destroy(&stream->dev_mutex);
	}

	v4l2_device_unregister(&dev->v4l2_dev);

	return 0;
}

static struct platform_driver mtk_camera_driver = {
	.probe	= mtk_camera_probe,
	.remove	= mtk_camera_remove,
	.driver	= {
		.name	= MTK_CAMERA_NAME,
		.of_match_table = mtk_camera_match,
	},
};

static int __init mtk_camera_init(void)
{
	return platform_driver_register(&mtk_camera_driver);
}

static void __exit mtk_camera_exit(void)
{
	platform_driver_unregister(&mtk_camera_driver);
}

module_init(mtk_camera_init);
module_exit(mtk_camera_exit);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("MediaTek V4L2 video capture driver");
