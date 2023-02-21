// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2022 MediaTek Inc.

#include <linux/pm_opp.h>
#include <linux/regulator/consumer.h>

#ifdef CONFIG_MTK_INTERCONNECT
#include "mtk-interconnect.h"
#endif
#include "mtk_cam.h"
#include "mtk_cam-feature.h"
#include "mtk_cam-dvfs_qos.h"
#include "mtk_cam-meta.h"
#ifdef MMDVFS_SUPPORT
#include "soc/mediatek/mmqos.h"
#endif
static unsigned int debug_mmqos;
module_param(debug_mmqos, uint, 0644);
MODULE_PARM_DESC(debug_mmqos, "activates debug mmqos");

enum raw_qos_port_id {
	imgo_r1 = 0,
	cqi_r1,
	cqi_r2,
	bpci_r1,
	lsci_r1,
	rawi_r2,/* 5 */
	rawi_r3,
	ufdi_r2,
	ufdi_r3,
	rawi_r4,
	rawi_r5,/* 10 */
	aai_r1,
#ifdef ISP7_1
	ufdi_r5,
#endif
	fho_r1,
	aao_r1,
	tsfso_r1,
	flko_r1,/* 15 */
	yuvo_r1,
	yuvo_r3,
	yuvco_r1,
	yuvo_r2,
	rzh1n2to_r1,/* 20 */
	drzs4no_r1,
	tncso_r1,
	raw_qos_port_num
};

enum sv_qos_port_id {
	sv_imgo = 0,
	sv_qos_port_num
};

struct raw_mmqos {
	char *port[raw_qos_port_num];
};

struct sv_mmqos {
	char *port[sv_qos_port_num];
};

#ifdef MMDVFS_SUPPORT
static struct raw_mmqos raw_qos[] = {
	[RAW_A] = {
		.port = {
			"l16_imgo_r1",
			"l16_cqi_r1",
			"l16_cqi_r2",
			"l16_bpci_r1",
			"l16_lsci_r1",
			"l16_rawi_r2",
			"l16_rawi_r3",
			"l16_ufdi_r2",
			"l16_ufdi_r3",
			"l16_rawi_r4",
			"l16_rawi_r5",
			"l16_aai_r1",
#ifdef ISP7_1
			"l16_ufdi_r5",
#endif
			"l16_fho_r1",
			"l16_aao_r1",
			"l16_tsfso_r1",
			"l16_flko_r1",
			"l17_yuvo_r1",
			"l17_yuvo_r3",
			"l17_yuvco_r1",
			"l17_yuvo_r2",
			"l17_rzh1n2to_r1",
			"l17_drzs4no_r1",
			"l17_tncso_r1"
		},
	},
	[RAW_B] = {
		.port = {
			"l27_imgo_r1",
			"l27_cqi_r1",
			"l27_cqi_r2",
			"l27_bpci_r1",
			"l27_lsci_r1",
			"l27_rawi_r2",
			"l27_rawi_r3",
			"l27_ufdi_r2",
			"l27_ufdi_r3",
			"l27_rawi_r4",
			"l27_rawi_r5",
			"l27_aai_r1",
#ifdef ISP7_1
			"l27_ufdi_r5",
#endif
			"l27_fho_r1",
			"l27_aao_r1",
			"l27_tsfso_r1",
			"l27_flko_r1",
#ifdef ISP7_1
			"l29_yuvo_r1",
			"l29_yuvo_r3",
			"l29_yuvco_r1",
			"l29_yuvo_r2",
			"l29_rzh1n2to_r1",
			"l29_drzs4no_r1",
			"l29_tncso_r1"
#else //MT8195
			"l28_yuvo_r1",
			"l28_yuvo_r3",
			"l28_yuvco_r1",
			"l28_yuvo_r2",
			"l28_rzh1n2to_r1",
			"l28_drzs4no_r1",
			"l28_tncso_r1"
#endif
		},
	},

#ifdef ISP7_1
	[RAW_C] = {
		.port = {
			"l28_imgo_r1",
			"l28_cqi_r1",
			"l28_cqi_r2",
			"l28_bpci_r1",
			"l28_lsci_r1",
			"l28_rawi_r2",
			"l28_rawi_r3",
			"l28_ufdi_r2",
			"l28_ufdi_r3",
			"l28_rawi_r4",
			"l28_rawi_r5",
			"l28_aai_r1",
			"l28_ufdi_r5",
			"l28_fho_r1",
			"l28_aao_r1",
			"l28_tsfso_r1",
			"l28_flko_r1",
			"l30_yuvo_r1",
			"l30_yuvo_r3",
			"l30_yuvco_r1",
			"l30_yuvo_r2",
			"l30_rzh1n2to_r1",
			"l30_drzs4no_r1",
			"l30_tncso_r1"
		},
	},
#endif
};

static struct sv_mmqos sv_qos[] = {
	[CAMSV_0] = {
		.port = {
			"l13_imgo_a0"
		},
	},
	[CAMSV_1] = {
		.port = {
#ifdef ISP7_1
			"l13_imgo_a1"
#else //MT8195
			"l14_imgo_a1"
#endif
		},
	},
	[CAMSV_2] = {
		.port = {
#ifdef ISP7_1
			"l14_imgo_b0"
#else //MT8195
			"l13_imgo_b0"
#endif
		},
	},
	[CAMSV_3] = {
		.port = {
#ifdef ISP7_1
			"l14_imgo_b1"
#else //MT8195
			"l13_imgo_b1"
#endif
		},
	},
	[CAMSV_4] = {
		.port = {
#ifdef ISP7_1
			"l13_imgo_c0"
#else //MT8195
			"l14_imgo_c0"
#endif
		},
	},
	[CAMSV_5] = {
		.port = {
#ifdef ISP7_1
			"l13_imgo_c1"
#else //MT8195
			"l14_imgo_c1"
#endif
		},
	},

#ifdef ISP7_1
	[CAMSV_6] = {
		.port = {
			"l14_imgo_d0"
		},
	},
	[CAMSV_7] = {
		.port = {
			"l14_imgo_d1"
		},
	},
	[CAMSV_8] = {
		.port = {
			"l13_imgo_e0"
		},
	},
	[CAMSV_9] = {
		.port = {
			"l13_imgo_e1"
		},
	},
	[CAMSV_10] = {
		.port = {
			"l14_imgo_f0"
		},
	},
	[CAMSV_11] = {
		.port = {
			"l14_imgo_f1"
		},
	},
	[CAMSV_12] = {
		.port = {
			"l13_imgo_g0"
		},
	},
	[CAMSV_13] = {
		.port = {
			"l13_imgo_g1"
		},
	},
	[CAMSV_14] = {
		.port = {
			"l14_imgo_h0"
		},
	},
	[CAMSV_15] = {
		.port = {
			"l14_imgo_h1"
		},
	},
#endif
};

#endif //MMDVFS_SUPPORT

#ifdef MMDVFS_SUPPORT
static void mtk_cam_dvfs_enumget_clktarget(struct mtk_cam_device *cam)
{
	struct mtk_camsys_dvfs *dvfs = &cam->camsys_ctrl.dvfs_info;
	int clk_streaming_max = dvfs->clklv[0];
	int i;

	for (i = MTKCAM_SUBDEV_RAW_START;  i < MTKCAM_SUBDEV_RAW_END; i++) {
		if (cam->ctxs[i].streaming && cam->ctxs[i].pipe) {
			struct mtk_cam_resource_config *res =
				&cam->ctxs[i].pipe->res_config;
			if (clk_streaming_max < res->clk_target)
				clk_streaming_max = res->clk_target;
			dev_dbg(cam->dev, "on ctx:%d clk needed:%d", i, res->clk_target);
		} else {
			dev_dbg(cam->dev, "on ctx:%d not streaming or pipe null", i);
		}
	}
	dvfs->clklv_target = clk_streaming_max;
	dev_info(cam->dev, "[%s] dvfs->clk=%d", __func__, dvfs->clklv_target);
}
#endif

#ifdef MMDVFS_SUPPORT
static void mtk_cam_dvfs_get_clkidx(struct mtk_cam_device *cam)
{
	struct mtk_camsys_dvfs *dvfs = &cam->camsys_ctrl.dvfs_info;

	u64 freq_cur = 0;
	int i;

	freq_cur = dvfs->clklv_target;
	for (i = 0; i < dvfs->clklv_num; i++) {
		if (freq_cur == dvfs->clklv[i])
			dvfs->clklv_idx = i;
	}
	dev_dbg(cam->dev, "[%s] get clk=%d, idx=%d",
		 __func__, dvfs->clklv_target, dvfs->clklv_idx);
}

#ifdef ISP7_1
static int set_clk_src(struct mtk_camsys_dvfs *dvfs, u32 opp_level)
{
	struct clk *mux, *clk_src;
	s32 err;

	if (opp_level >= MAX_CAM_OPP_STEP) {
		dev_info(dvfs->dev, "opp level(%d) is out of bound\n", opp_level);
		return -EINVAL;
	}

	mux = dvfs->mux;
	clk_src = dvfs->clk_src[opp_level];

	err = clk_prepare_enable(mux);
	if (err) {
		dev_info(dvfs->dev, "prepare cam mux fail:%d opp_level:%d\n",
				err, opp_level);
		return err;
	}
	err = clk_set_parent(mux, clk_src);
	if (err)
		dev_info(dvfs->dev, "set cam mux parent fail:%d opp_level:%d\n",
				err, opp_level);
	clk_disable_unprepare(mux);

	return err;
}
#endif //ISP7_1
#endif //MMDVFS_SUPPORT

void mtk_cam_dvfs_update_clk(struct mtk_cam_device *cam)
{
#ifdef MMDVFS_SUPPORT
	struct mtk_camsys_dvfs *dvfs = &cam->camsys_ctrl.dvfs_info;
#ifdef ISP7_1
	int current_volt;
	s32 err;
#endif

	if (dvfs->clklv_num) {
		mtk_cam_dvfs_enumget_clktarget(cam);
		mtk_cam_dvfs_get_clkidx(cam);
		dev_dbg(cam->dev, "[%s] update idx:%d clk:%d volt:%d", __func__,
			dvfs->clklv_idx, dvfs->clklv_target, dvfs->voltlv[dvfs->clklv_idx]);
#ifdef ISP7_1
		if (dvfs->reg_vmm) {
			current_volt = regulator_get_voltage(dvfs->reg_vmm);
			if (dvfs->voltlv[dvfs->clklv_idx] < current_volt) {
				err = set_clk_src(dvfs, dvfs->clklv_idx);
				if (err) {
					dev_info(cam->dev, "[%s] adjust clk fail\n", __func__);
					return;
				}
				regulator_set_voltage(dvfs->reg_vmm,
						dvfs->voltlv[dvfs->clklv_idx], INT_MAX);
			} else {
				err = regulator_set_voltage(dvfs->reg_vmm,
						dvfs->voltlv[dvfs->clklv_idx], INT_MAX);
				if (err) {
					dev_info(cam->dev, "[%s] adjust voltage fail\n", __func__);
					return;
				}
				set_clk_src(dvfs, dvfs->clklv_idx);
			}
		}
#else //MT8195
		regulator_set_voltage(dvfs->reg_vmm, dvfs->voltlv[dvfs->clklv_idx], INT_MAX);
#endif //ISP7_1
	}
#endif //MMDVFS_SUPPORT
}

void mtk_cam_dvfs_uninit(struct mtk_cam_device *cam)
{
#ifdef MMDVFS_SUPPORT
	struct mtk_camsys_dvfs *dvfs_info = &cam->camsys_ctrl.dvfs_info;

	if (dvfs_info->clklv_num)
		dev_pm_opp_of_remove_table(dvfs_info->dev);
	dev_info(cam->dev, "[%s]\n", __func__);
#endif
}

void mtk_cam_dvfs_init(struct mtk_cam_device *cam)
{
#ifdef MMDVFS_SUPPORT
	struct mtk_camsys_dvfs *dvfs_info = &cam->camsys_ctrl.dvfs_info;
	struct dev_pm_opp *opp;
	unsigned long freq = 0;
	int ret = 0, clk_num = 0, i = 0;
	struct device *dev = cam->dev;
	const char *mux_name, *clksrc_name;
	struct property *clksrc_prop;
	u32 num_clksrc = 0;


	memset((void *)dvfs_info, 0x0, sizeof(struct mtk_camsys_dvfs));
	dvfs_info->dev = cam->dev;
	ret = dev_pm_opp_of_add_table(dvfs_info->dev);
	if (ret < 0) {
		dev_info(dvfs_info->dev, "fail to init opp table: %d\n", ret);
		goto opp_default_table;
	}

#ifdef ISP7_1
	dvfs_info->reg_vmm = devm_regulator_get_optional(dvfs_info->dev, "dvfs-vmm");
#else //MT8195
	dvfs_info->reg_vmm = devm_regulator_get_optional(dvfs_info->dev, "dvfsrc-vcore");
#endif //ISP7_1
	if (IS_ERR(dvfs_info->reg_vmm)) {
		dev_info(dvfs_info->dev, "can't get dvfsrc-vcore\n");
		goto opp_default_table;
	}

	clk_num = dev_pm_opp_get_opp_count(dvfs_info->dev);
	while (!IS_ERR(opp = dev_pm_opp_find_freq_ceil(dvfs_info->dev, &freq))) {
		dvfs_info->clklv[i] = freq;
		dvfs_info->voltlv[i] = dev_pm_opp_get_voltage(opp);
		freq++;
		i++;
		dev_pm_opp_put(opp);
	}
	dvfs_info->clklv_num = clk_num;
	dvfs_info->clklv_target = dvfs_info->clklv[0];
	dvfs_info->clklv_idx = 0;
	for (i = 0; i < dvfs_info->clklv_num; i++) {
		dev_info(cam->dev, "[%s] idx=%d, clk=%d volt=%d\n",
			 __func__, i, dvfs_info->clklv[i], dvfs_info->voltlv[i]);
	}
	mtk_cam_qos_init(cam);

	/* Get CLK handles */
	ret = of_property_read_string(dev->of_node, "mux_name", &mux_name);
	dev_info(dev, "mux name(%s)\n", mux_name);
	dvfs_info->mux = devm_clk_get(dev, mux_name);

	/* Get CLK source */
	of_property_for_each_string(
		dev->of_node, "clk_src", clksrc_prop, clksrc_name) {
		if (num_clksrc >= MAX_CAM_OPP_STEP) {
			dev_info(dev, "Too many items in clk_src array\n");
			return;
		}
		dev_info(dev, "clksrc name(%s)\n", clksrc_name);
		dvfs_info->clk_src[num_clksrc] =
			devm_clk_get(dev, clksrc_name);
		num_clksrc++;
	}

	return;

opp_default_table:
	/* TODO: need to get from CCF or others api */
	dvfs_info->voltlv[0] = 650000;
	dvfs_info->clklv[0] = 546000000;
	dvfs_info->clklv_target = 546000000;
	dvfs_info->clklv_num = 1;
#endif

}

#define MTK_CAM_QOS_LSCI_TABLE_MAX_SIZE (32768)
#define MTK_CAM_QOS_CACI_TABLE_MAX_SIZE (32768)
#define BW_B2KB(value) ((value) / 1024)
#define BW_B2KB_WITH_RATIO(value) ((value) * 4 / 3 / 1024)

void mtk_cam_qos_bw_calc(struct mtk_cam_ctx *ctx, unsigned long raw_dmas)
{
#ifdef MMDVFS_SUPPORT
	struct mtk_cam_device *cam = ctx->cam;
	struct mtk_camsys_dvfs *dvfs_info = &cam->camsys_ctrl.dvfs_info;
	struct mtk_cam_video_device *vdev;
	struct mtk_raw_pipeline *pipe = ctx->pipe;
	struct mtk_raw_device *raw_dev = get_master_raw_dev(cam, pipe);
	struct v4l2_subdev_frame_interval fi;
	struct v4l2_subdev_format sd_fmt;
	struct v4l2_ctrl *ctrl;
	struct raw_mmqos *raw_mmqos;
	struct sv_mmqos *sv_mmqos;
	unsigned int ipi_video_id;
	int engine_id = raw_dev->id;
	unsigned int qos_port_id;
	unsigned int ipi_fmt;
	int i, pixel_bits, plane_factor;
	unsigned long vblank, fps, height, PBW_MB_s, ABW_MB_s;

	raw_mmqos = raw_qos + engine_id;

	raw_dmas |= dvfs_info->updated_raw_dmas[engine_id];

	/* mstream may no settings */
	if (raw_dmas == 0 || dvfs_info->updated_raw_dmas[engine_id] == raw_dmas)
		return;

	dev_info(cam->dev, "[%s] engine_id(%d) enable_dmas(0x%lx) raw_dmas(0x%lx), updated_raw_dmas(0x%lx)\n",
		__func__, engine_id, pipe->enabled_dmas, raw_dmas,
		dvfs_info->updated_raw_dmas[engine_id]);

	dvfs_info->updated_raw_dmas[engine_id] = raw_dmas;

	if (ctx->sensor) {
		fi.pad = 0;
		fi.which = V4L2_SUBDEV_FORMAT_ACTIVE;
		v4l2_subdev_call(ctx->sensor, video, g_frame_interval, &fi);
		fps = fi.interval.denominator / fi.interval.numerator;
		pipe->res_config.interval.denominator = fi.interval.denominator;
		pipe->res_config.interval.numerator = fi.interval.numerator;
		ctrl = v4l2_ctrl_find(ctx->sensor->ctrl_handler, V4L2_CID_VBLANK);
		vblank = v4l2_ctrl_g_ctrl(ctrl);
		sd_fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
		sd_fmt.pad = PAD_SRC_RAW0;
		v4l2_subdev_call(ctx->seninf, pad, get_fmt, NULL, &sd_fmt);
		height = sd_fmt.format.height;
		dev_info(cam->dev, "[%s] FPS:%lu/%lu:%lu, H:%lu, VB:%lu\n",
			 __func__, fi.interval.denominator, fi.interval.numerator,
			 fps, height, vblank);
	}

	/* clear raw qos */
	for (i = 0; i < raw_qos_port_num; i++) {
		qos_port_id = engine_id * raw_qos_port_num + i;
		dvfs_info->qos_bw_avg[qos_port_id] = 0;
		dvfs_info->qos_bw_peak[qos_port_id] = 0;
	}

	for (i = 0; i < MTKCAM_IPI_RAW_ID_MAX; i++) {
		if (raw_dmas & 1ULL<<i)
			ipi_video_id = i;
		else
			continue;
		/* update buffer internal address */
		switch (ipi_video_id) {
		case MTKCAM_IPI_RAW_IMGO:
			qos_port_id = engine_id * raw_qos_port_num + imgo_r1;
			vdev = &pipe->vdev_nodes[MTK_RAW_MAIN_STREAM_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] = ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_YUVO_1:
			qos_port_id = engine_id * raw_qos_port_num + yuvo_r1;
			vdev = &pipe->vdev_nodes[MTK_RAW_YUVO_1_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] = ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_YUVO_3:
			qos_port_id = engine_id * raw_qos_port_num + yuvo_r3;
			vdev = &pipe->vdev_nodes[MTK_RAW_YUVO_3_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] = ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_YUVO_2:
		case MTKCAM_IPI_RAW_YUVO_4:
		case MTKCAM_IPI_RAW_YUVO_5:
			qos_port_id = engine_id * raw_qos_port_num + yuvo_r2;
			if (ipi_video_id == MTKCAM_IPI_RAW_YUVO_2)
				vdev = &pipe->vdev_nodes[MTK_RAW_YUVO_2_OUT - MTK_RAW_SINK_NUM];
			else if (ipi_video_id == MTKCAM_IPI_RAW_YUVO_4)
				vdev = &pipe->vdev_nodes[MTK_RAW_YUVO_4_OUT - MTK_RAW_SINK_NUM];
			else if (ipi_video_id == MTKCAM_IPI_RAW_YUVO_5)
				vdev = &pipe->vdev_nodes[MTK_RAW_YUVO_5_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] += PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] += ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_RZH1N2TO_1:
		case MTKCAM_IPI_RAW_RZH1N2TO_2:
		case MTKCAM_IPI_RAW_RZH1N2TO_3:
			qos_port_id = engine_id * raw_qos_port_num + rzh1n2to_r1;
			if (ipi_video_id == MTKCAM_IPI_RAW_RZH1N2TO_1)
				vdev = &pipe->vdev_nodes[MTK_RAW_RZH1N2TO_1_OUT - MTK_RAW_SINK_NUM];
			else if (ipi_video_id == MTKCAM_IPI_RAW_RZH1N2TO_2)
				vdev = &pipe->vdev_nodes[MTK_RAW_RZH1N2TO_2_OUT - MTK_RAW_SINK_NUM];
			else if (ipi_video_id == MTKCAM_IPI_RAW_RZH1N2TO_3)
				vdev = &pipe->vdev_nodes[MTK_RAW_RZH1N2TO_3_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] += PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] += ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_DRZS4NO_1:
		case MTKCAM_IPI_RAW_DRZS4NO_2:
		case MTKCAM_IPI_RAW_DRZS4NO_3:
			qos_port_id = engine_id * raw_qos_port_num + drzs4no_r1;
			if (ipi_video_id == MTKCAM_IPI_RAW_DRZS4NO_1)
				vdev = &pipe->vdev_nodes[MTK_RAW_DRZS4NO_1_OUT - MTK_RAW_SINK_NUM];
			else if (ipi_video_id == MTKCAM_IPI_RAW_DRZS4NO_2)
				vdev = &pipe->vdev_nodes[MTK_RAW_DRZS4NO_2_OUT - MTK_RAW_SINK_NUM];
			else if (ipi_video_id == MTKCAM_IPI_RAW_DRZS4NO_3)
				vdev = &pipe->vdev_nodes[MTK_RAW_DRZS4NO_3_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] += PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] += ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_META_STATS_CFG:
			/* cq_r1 = main+sub cq descriptor size */
			qos_port_id = engine_id * raw_qos_port_num + cqi_r1;
			PBW_MB_s = CQ_BUF_SIZE * fps / 10;
			dvfs_info->qos_bw_avg[qos_port_id] =
				dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d BW(B/s):%lu\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, PBW_MB_s);
			/* cq_r2 = main+sub cq virtual address size */
			qos_port_id = engine_id * raw_qos_port_num + cqi_r2;
			PBW_MB_s = CQ_BUF_SIZE * fps * 9 / 10;
			dvfs_info->qos_bw_avg[qos_port_id] =
				dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d BW(B/s):%lu\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, PBW_MB_s);
			/* lsci_r1 = lsci */
			// lsci = MTK_CAM_QOS_LSCI_TABLE_MAX_SIZE
			qos_port_id = engine_id * raw_qos_port_num + lsci_r1;
			PBW_MB_s = MTK_CAM_QOS_LSCI_TABLE_MAX_SIZE * fps;
			dvfs_info->qos_bw_avg[qos_port_id] =
				dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d BW(B/s):%lu\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, PBW_MB_s);
			/* fho_r1 = aaho + fho + pdo */
			// aaho = MTK_CAM_UAPI_AAHO_HIST_SIZE
			// fho = almost zero
			// pdo = implement later
			qos_port_id = engine_id * raw_qos_port_num + fho_r1;
			PBW_MB_s = mtk_cam_get_port_bw(AAHO, height, fps);
			dvfs_info->qos_bw_avg[qos_port_id] =
				dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d BW(B/s):%lu\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, PBW_MB_s);
			/* aao_r1 = aao + afo */
			// aao = MTK_CAM_UAPI_AAO_MAX_BUF_SIZE (twin = /2)
			// afo = MTK_CAM_UAPI_AFO_MAX_BUF_SIZE (twin = /2)
			qos_port_id = engine_id * raw_qos_port_num + aao_r1;
			PBW_MB_s = mtk_cam_get_port_bw(AAO, height, fps);
			dvfs_info->qos_bw_avg[qos_port_id] =
				dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d BW(B/s):%lu\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, PBW_MB_s);
			/* tsfso_r1 = tsfso + tsfso + ltmso */
			// tsfso x 2 = MTK_CAM_UAPI_TSFSO_SIZE * 2
			// ltmso = MTK_CAM_UAPI_LTMSO_SIZE
			qos_port_id = engine_id * raw_qos_port_num + tsfso_r1;
			PBW_MB_s = mtk_cam_get_port_bw(TSFSO, height, fps);
			dvfs_info->qos_bw_avg[qos_port_id] =
				dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d BW(B/s):%lu\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, PBW_MB_s);
			/* flko_r1 = flko + ufeo + bpco */
			// flko = MTK_CAM_UAPI_FLK_BLK_SIZE * MTK_CAM_UAPI_FLK_MAX_STAT_BLK_NUM
			//						* sensor height
			// ufeo = implement later
			// bpco = implement later
			qos_port_id = engine_id * raw_qos_port_num + flko_r1;
			PBW_MB_s = mtk_cam_get_port_bw(FLKO, height, fps);
			dvfs_info->qos_bw_avg[qos_port_id] =
				dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d BW(B/s):%lu\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_RAWI_2:
			qos_port_id = engine_id * raw_qos_port_num + rawi_r2;
			vdev = &pipe->vdev_nodes[MTK_RAW_MAIN_STREAM_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] = ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_RAWI_3:
			qos_port_id = engine_id * raw_qos_port_num + rawi_r3;
			vdev = &pipe->vdev_nodes[MTK_RAW_MAIN_STREAM_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] = ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_RAWI_5:
			qos_port_id = engine_id * raw_qos_port_num + rawi_r5;
			vdev = &pipe->vdev_nodes[MTK_RAW_MAIN_STREAM_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] = PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] = ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_RAWI_6:
			qos_port_id = engine_id * raw_qos_port_num + aai_r1;
			vdev = &pipe->vdev_nodes[MTK_RAW_MAIN_STREAM_OUT - MTK_RAW_SINK_NUM];
			ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
			pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
			plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
			PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						(vblank + height) * pixel_bits *
						plane_factor / 8 / 100;
			ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
						vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
						plane_factor / 8 / 100;
			dvfs_info->qos_bw_peak[qos_port_id] += PBW_MB_s;
			dvfs_info->qos_bw_avg[qos_port_id] += ABW_MB_s;
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
				  raw_mmqos->port[qos_port_id % raw_qos_port_num],
				  qos_port_id, ipi_fmt,
				  pixel_bits, plane_factor,
				  vdev->active_fmt.fmt.pix_mp.width,
				  vdev->active_fmt.fmt.pix_mp.height,
				  ABW_MB_s, PBW_MB_s);
			break;
		case MTKCAM_IPI_RAW_META_STATS_0:
			break;
		case MTKCAM_IPI_RAW_META_STATS_1:
			break;
		case MTKCAM_IPI_RAW_META_STATS_2:
			break;
		default:
			break;
		}
	}

	if (mtk_cam_is_stagger(ctx)) {
		vdev = &pipe->vdev_nodes[MTK_RAW_MAIN_STREAM_OUT - MTK_RAW_SINK_NUM];
		ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
		pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
		plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
		PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
					(vblank + height) * pixel_bits *
					plane_factor / 8 / 100;
		ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
					vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
					plane_factor / 8 / 100;
		for (i = MTKCAM_SUBDEV_CAMSV_START ; i < MTKCAM_SUBDEV_CAMSV_END ; i++) {
			if (ctx->pipe->enabled_raw & (1 << i)) {
				qos_port_id =
					((i - MTKCAM_SUBDEV_CAMSV_START) * sv_qos_port_num) +
					sv_imgo;
				dvfs_info->sv_qos_bw_peak[qos_port_id] = PBW_MB_s;
				dvfs_info->sv_qos_bw_avg[qos_port_id] = ABW_MB_s;
				if (unlikely(debug_mmqos))
					dev_info(cam->dev,
					  "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
					  sv_qos[i - MTKCAM_SUBDEV_CAMSV_START].port[
					  qos_port_id % sv_qos_port_num],
					  qos_port_id, ipi_fmt, pixel_bits, plane_factor,
					  vdev->active_fmt.fmt.pix_mp.width,
					  vdev->active_fmt.fmt.pix_mp.height,
					  ABW_MB_s, PBW_MB_s);
			}
		}
	}

	for (i = 0; i < ctx->used_sv_num; i++) {
		qos_port_id = ((ctx->sv_pipe[i]->id - MTKCAM_SUBDEV_CAMSV_START)
						* sv_qos_port_num)
						+ sv_imgo;
		sv_mmqos = &sv_qos[ctx->sv_pipe[i]->id - MTKCAM_SUBDEV_CAMSV_START];
		vdev = &ctx->sv_pipe[i]->vdev_nodes[MTK_CAMSV_MAIN_STREAM_OUT - MTK_CAMSV_SINK_NUM];
		ipi_fmt = mtk_cam_get_img_fmt(vdev->active_fmt.fmt.pix_mp.pixelformat);
		pixel_bits = mtk_cam_get_pixel_bits(ipi_fmt);
		plane_factor = mtk_cam_get_fmt_size_factor(ipi_fmt);
		PBW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
					(vblank + height) * pixel_bits *
					plane_factor / 8 / 100;
		ABW_MB_s = vdev->active_fmt.fmt.pix_mp.width * fps *
					vdev->active_fmt.fmt.pix_mp.height * pixel_bits *
					plane_factor / 8 / 100;
		dvfs_info->sv_qos_bw_peak[qos_port_id] = PBW_MB_s;
		dvfs_info->sv_qos_bw_avg[qos_port_id] = ABW_MB_s;
		if (unlikely(debug_mmqos))
			dev_info(cam->dev, "[%16s] qos_idx:%2d ipifmt/bits/plane/w/h : %2d/%2d/%d/%5d/%5d BW(B/s)(avg:%lu,peak:%lu)\n",
			  sv_mmqos->port[qos_port_id % sv_qos_port_num], qos_port_id, ipi_fmt,
			  pixel_bits, plane_factor,
			  vdev->active_fmt.fmt.pix_mp.width,
			  vdev->active_fmt.fmt.pix_mp.height,
			  ABW_MB_s, PBW_MB_s);
	}
	/* by engine update */
	for (i = 0; i < raw_qos_port_num; i++) {
		qos_port_id = engine_id * raw_qos_port_num + i;
		if (unlikely(debug_mmqos))
			dev_info(cam->dev, "[%s] port idx/name:%2d/%16s BW(kB/s)(avg:%lu,peak:%lu)\n",
			  __func__, qos_port_id, raw_mmqos->port[qos_port_id % raw_qos_port_num],
			  BW_B2KB_WITH_RATIO(dvfs_info->qos_bw_avg[qos_port_id]),
			  BW_B2KB(dvfs_info->qos_bw_peak[qos_port_id]));

		if (dvfs_info->qos_req[qos_port_id])
			mtk_icc_set_bw(dvfs_info->qos_req[qos_port_id],
				kBps_to_icc(BW_B2KB_WITH_RATIO(
					dvfs_info->qos_bw_avg[qos_port_id])),
				kBps_to_icc(BW_B2KB(
					dvfs_info->qos_bw_peak[qos_port_id])));
	}
	for (i = 0; i < MTK_CAM_SV_PORT_NUM; i++) {
		if (dvfs_info->sv_qos_bw_avg[i] != 0) {
			if (unlikely(debug_mmqos))
				dev_info(cam->dev, "[%s] port idx:%2d BW(kB/s)(avg:%lu,peak:%lu)\n",
				  __func__, i,
				  BW_B2KB_WITH_RATIO(dvfs_info->sv_qos_bw_avg[i]),
				  BW_B2KB(dvfs_info->sv_qos_bw_peak[i]));

			if (dvfs_info->sv_qos_req[i])
				mtk_icc_set_bw(dvfs_info->sv_qos_req[i],
					kBps_to_icc(BW_B2KB_WITH_RATIO(
						dvfs_info->sv_qos_bw_avg[i])),
					kBps_to_icc(BW_B2KB(
						dvfs_info->sv_qos_bw_peak[i])));
		}
	}
#endif //MMDVFS_SUPPORT
}

void mtk_cam_qos_init(struct mtk_cam_device *cam)
{
#ifdef MMDVFS_SUPPORT
	struct mtk_camsys_dvfs *dvfs_info = &cam->camsys_ctrl.dvfs_info;
	struct raw_mmqos *raw_mmqos;
	struct sv_mmqos *sv_mmqos;
	struct mtk_raw_device *raw_dev;
	struct mtk_yuv_device *yuv_dev;
	struct mtk_camsv_device *sv_dev;
	struct device *engineraw_dev;
	struct device *engineyuv_dev;
	struct device *enginesv_dev;
	int i, raw_num, port_id, engine_id;

	for (i = 0; i < MTK_CAM_RAW_PORT_NUM; i++) {
		dvfs_info->qos_req[i] = 0;
		dvfs_info->qos_bw_avg[i] = 0;
		dvfs_info->qos_bw_peak[i] = 0;
	}
	for (i = 0; i < MTK_CAM_SV_PORT_NUM; i++) {
		dvfs_info->sv_qos_req[i] = 0;
		dvfs_info->sv_qos_bw_avg[i] = 0;
		dvfs_info->sv_qos_bw_peak[i] = 0;
	}
	i = 0;
	for (raw_num = 0; raw_num < cam->num_raw_drivers; raw_num++) {
		engine_id = RAW_A + raw_num;
		raw_mmqos = raw_qos + engine_id;
		engineraw_dev = cam->raw.devs[engine_id];
		engineyuv_dev = cam->raw.yuvs[engine_id];
		for (port_id = 0; port_id < raw_qos_port_num; port_id++) {
			if (port_id < yuvo_r1) {
				raw_dev = dev_get_drvdata(engineraw_dev);
				dvfs_info->qos_req[i] =
					of_mtk_icc_get(raw_dev->dev, raw_mmqos->port[port_id]);
			} else {
				yuv_dev = dev_get_drvdata(engineyuv_dev);
				dvfs_info->qos_req[i] =
					of_mtk_icc_get(yuv_dev->dev, raw_mmqos->port[port_id]);
			}
			i++;
		}
		dvfs_info->updated_raw_dmas[engine_id] = 0;
		dev_info(raw_dev->dev, "[%s] raw_engine_id=%d, port_num=%d\n",
			 __func__, engine_id, i);
	}
	i = 0;
	for (engine_id = CAMSV_START; engine_id < CAMSV_END; engine_id++) {
		sv_mmqos = sv_qos + engine_id;
		enginesv_dev = cam->sv.devs[engine_id];
		if (enginesv_dev == NULL)
			continue;
		for (port_id = 0; port_id < sv_qos_port_num; port_id++) {
			sv_dev = dev_get_drvdata(enginesv_dev);
			dvfs_info->sv_qos_req[i] =
				of_mtk_icc_get(sv_dev->dev, sv_mmqos->port[port_id]);
			i++;
		}
		dev_info(sv_dev->dev, "[%s] sv_engine_id=%d, port_num=%d\n",
			 __func__, engine_id, i);
	}
#endif //MMDVFS_SUPPORT
}

void mtk_cam_qos_bw_reset(struct mtk_cam_ctx *ctx, unsigned int enabled_sv)
{
#ifdef MMDVFS_SUPPORT
	struct mtk_cam_device *cam = ctx->cam;
	struct mtk_raw_pipeline *pipe = ctx->pipe;
	struct mtk_raw_device *raw_dev = get_master_raw_dev(cam, pipe);
	struct mtk_camsys_dvfs *dvfs_info = &cam->camsys_ctrl.dvfs_info;
	unsigned int qos_port_id;
	int engine_id = raw_dev->id;
	int i, j;

	dev_info(cam->dev, "[%s] enabled_raw(%d),used_sv_num(%d)\n",
		__func__, ctx->pipe->enabled_raw, ctx->used_sv_num);

	dvfs_info->updated_raw_dmas[engine_id] = 0;

	for (i = 0; i < raw_qos_port_num; i++) {
		qos_port_id = engine_id * raw_qos_port_num + i;
		dvfs_info->qos_bw_avg[qos_port_id] = 0;
		dvfs_info->qos_bw_peak[qos_port_id] = 0;
		if (dvfs_info->qos_req[qos_port_id])
			mtk_icc_set_bw(dvfs_info->qos_req[qos_port_id], 0, 0);
	}

	if (mtk_cam_is_stagger(ctx) || mtk_cam_is_time_shared(ctx)
		|| mtk_cam_is_with_w_channel(ctx)) {
		for (i = MTKCAM_SUBDEV_CAMSV_START ; i < MTKCAM_SUBDEV_CAMSV_END ; i++) {
			if (enabled_sv & (1 << i)) {
				qos_port_id =
					((i - MTKCAM_SUBDEV_CAMSV_START) * sv_qos_port_num) +
					sv_imgo;
				dvfs_info->sv_qos_bw_avg[qos_port_id] = 0;
				dvfs_info->sv_qos_bw_peak[qos_port_id] = 0;
				if (dvfs_info->sv_qos_req[qos_port_id])
					mtk_icc_set_bw(dvfs_info->sv_qos_req[qos_port_id], 0, 0);
			}
		}
	}

	for (i = 0; i < ctx->used_sv_num; i++) {
		for (j = 0; j < sv_qos_port_num; j++) {
			qos_port_id = ((ctx->sv_pipe[i]->id - MTKCAM_SUBDEV_CAMSV_START)
						* sv_qos_port_num) + j;
			dvfs_info->sv_qos_bw_avg[qos_port_id] = 0;
			dvfs_info->sv_qos_bw_peak[qos_port_id] = 0;
			if (dvfs_info->sv_qos_req[qos_port_id])
				mtk_icc_set_bw(dvfs_info->sv_qos_req[qos_port_id], 0, 0);
		}
	}

#endif //MMDVFS_SUPPORT
}

