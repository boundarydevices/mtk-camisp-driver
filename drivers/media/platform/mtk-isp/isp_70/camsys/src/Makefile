# SPDX-License-Identifier: GPL-2.0-only

ccflags-y += -I$(srctree)/drivers/media/platform/mtk-isp/isp_70 \
	     -I$(srctree)/drivers/media/platform/mtk-isp \
	     -I$(srctree)/drivers/misc/mediatek/imgsensor/src-v4l2/frame-sync/ \
	     -I$(srctree)/drivers/misc/mediatek/imgsensor/inc/

mtk-cam-isp-objs := mtk_cam.o mtk_cam-raw.o mtk_cam-pool.o mtk_cam_pm.o \
		    mtk_cam-video.o mtk_cam-smem.o mtk_cam_vb2-dma-contig.o \
		    mtk_cam-ctrl.o \
		    mtk_cam-seninf-route.o mtk_cam-seninf-drv.o \
		    mtk_cam-dvfs_qos.o \
		    mtk_cam-debug.o \
		    mtk_cam-sv.o \
		    mtk_cam-raw_debug.o \
		    mtk_cam-tg-flash.o \
		    mtk_cam-feature.o mtk_cam-timesync.o

mtk-cam-plat-util-objs :=  mtk_cam-plat-util.o
mtk-cam-isp-objs +=  mtk_cam-hsf.o

include $(src)/mtk_csi_phy_2_0/sub_drv.mk

mtk-cam-plat-mt8195-objs :=  mtk_cam-plat-mt8195.o
obj-y += mtk-cam-plat-mt8195.o

obj-y += mtk-cam-plat-util.o
obj-y += mtk-cam-isp.o
