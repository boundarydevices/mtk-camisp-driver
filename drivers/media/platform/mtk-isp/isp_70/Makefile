# SPDX-License-Identifier: GPL-2.0

subdir-ccflags-y += -Werror

subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_70_CAMSYS) += -DISP7_0
subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_71_CAMSYS) += -DISP7_1

obj-$(CONFIG_VIDEO_MTK_ISP_70_CAMSYS) += camsys/
obj-$(CONFIG_VIDEO_MTK_ISP_71_CAMSYS) += camsys/

subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_70_IMGSYS) += -DIMGSYS_VER_ISP70
subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_70_IMGSYS) += -DIMGSYS_WPE_NUM=2
subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_70_IMGSYS) += -DIMGSYS_TRAW_NUM=2

subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_71_IMGSYS) += -DIMGSYS_VER_ISP70
subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_71_IMGSYS) += -DIMGSYS_WPE_NUM=2
subdir-ccflags-$(CONFIG_VIDEO_MTK_ISP_71_IMGSYS) += -DIMGSYS_TRAW_NUM=2

obj-$(CONFIG_VIDEO_MTK_ISP_70_IMGSYS) += imgsys/
obj-$(CONFIG_VIDEO_MTK_ISP_70_IMGSYS) += mtk-hcp/
obj-$(CONFIG_VIDEO_MTK_ISP_70_IMGSYS) += mtk-ipesys-me/

obj-$(CONFIG_VIDEO_MTK_ISP_71_IMGSYS) += imgsys/
obj-$(CONFIG_VIDEO_MTK_ISP_71_IMGSYS) += mtk-hcp/
obj-$(CONFIG_VIDEO_MTK_ISP_71_IMGSYS) += mtk-ipesys-me/
