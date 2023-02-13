# SPDX-License-Identifier: GPL-2.0
#
# Copyright (c) 2022 MediaTek Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

ccflags-y += -I$(src)/external_include/mtk-vcu-mt8395/
ccflags-y += -I$(src)/external_include/uapi/
ccflags-y += -I$(top)/include/

mtk_camera-objs += mtk_camera_drv.o \
		 mtk_camera_util.o \
		 mtk_camera_if.o \
		 mtk_camera_ctrl.o \
		 mtk_vcu_camif.o

obj-$(CONFIG_VIDEO_MEDIATEK_CAMERA) += mtk_camera.o

#subdir-ccflags-y += -Werror
