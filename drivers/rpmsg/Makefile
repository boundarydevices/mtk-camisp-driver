# SPDX-License-Identifier: GPL-2.0

ccflags-y += -I$(top)/include
ccflags-y += -I$(srctree)/drivers/rpmsg

obj-$(CONFIG_RPMSG_MTK_CCD) += mtk-ccd-rpmsg.o

# mtk-ccd-rpmsg.ko
mtk-ccd-rpmsg-objs += mtk_ccd_rpmsg.o
mtk-ccd-rpmsg-objs += mtk_ccd_rpmsg_ipi.o
