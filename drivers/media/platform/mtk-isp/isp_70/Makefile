# SPDX-License-Identifier: GPL-2.0

subdir-ccflags-y += -Werror

subdir-ccflags-y += -DISP7_0
obj-$(CONFIG_VIDEO_MTK_ISP_7_CAMSYS) += camsys/

subdir-ccflags-y += -DIMGSYS_VER_ISP70
subdir-ccflags-y += -DIMGSYS_WPE_NUM=2
subdir-ccflags-y += -DIMGSYS_TRAW_NUM=2
obj-$(CONFIG_VIDEO_MTK_ISP_7_IMGSYS) += imgsys/
obj-$(CONFIG_VIDEO_MTK_ISP_7_IMGSYS) += mtk-hcp/
obj-$(CONFIG_VIDEO_MTK_ISP_7_IMGSYS) += mtk-ipesys-me/
