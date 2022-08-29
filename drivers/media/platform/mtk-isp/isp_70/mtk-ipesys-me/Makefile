# SPDX-License-Identifier: GPL-2.0
#
# Copyright (C) 2018 MediaTek Inc.
#
export MTK_IMGSYS_VER := 70
ccflags-y += -I$(top)/drivers/media/platform/mtk-isp/isp_70/mtk-hcp
ccflags-$(CONFIG_MTK_CMDQ_MBOX_EXT) += -I$(srctree)/drivers/misc/mediatek/cmdq/mailbox/
ccflags-y += -I$(top)/drivers/media/platform/mtk-isp/isp_70/imgsys/
ccflags-y += -I$(top)/drivers/media/platform/mtk-isp/isp_70/imgsys/platforms/
ccflags-y += -I$(top)/drivers/media/platform/mtk-isp/isp_70/imgsys/platforms/isp_70/

# Utilities to provide frame-based streaming model
# with v4l2 user interfaces and alloc context managing
# memory shared between ISP and coprocessor

ifneq ($(wildcard $(srctree)/include/dt-bindings/memory/mt6983-larb-port.h),)
	subdir-ccflags-y += -DTF_DUMP_71_1
else ifneq ($(wildcard $(srctree)/include/dt-bindings/memory/mt6879-larb-port.h),)
	subdir-ccflags-y += -DTF_DUMP_71_2
endif
ifeq ($(MTK_IMGSYS_VER), 71)
  subdir-ccflags-y += -DIMGSYS_VER_ISP71
else ifeq ($(MTK_IMGSYS_VER), 70)
  subdir-ccflags-y += -DIMGSYS_VER_ISP70
endif
$(info "ipesys, subdir-ccflags-y = $(subdir-ccflags-y)")

obj-$(CONFIG_VIDEO_MTK_ISP_7_IMGSYS) += mtk-ipesys-me.o

