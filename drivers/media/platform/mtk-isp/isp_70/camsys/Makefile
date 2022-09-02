# SPDX-License-Identifier: GPL-2.0

ifneq ($(CONFIG_VIDEO_MTK_ISP_7_CAMSYS_UT),m)
obj-y += src/
else
# enable with unit_test
KBUILD_CFLAGS += -Wframe-larger-than=26384
obj-y += unit_test/
endif
