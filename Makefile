ifneq ($(KERNELRELEASE),)

export MTK_PLATFORM=mt8195
export CONFIG_RPMSG_MTK_CCD=m
export CONFIG_MTKCCD_REMOTEPROC=m
export CONFIG_VIDEO_MTK_ISP_7_CAMSYS=m
export CONFIG_VIDEO_MTK_ISP_7_IMGSYS=m
export CONFIG_VIDEO_MTK_ISP_7_CAMSYS_UT=n
export CONFIG_VIDEO_MEDIATEK_CAMERA=m
export CONFIG_MTK_V4L2_IMGSENSOR=m
export CONFIG_CUSTOM_KERNEL_IMGSENSOR="imx214_mipi_raw ap1302_mipi_yuv"

export top := $(src)

obj-y += drivers/rpmsg/
obj-y += drivers/remoteproc/
obj-y += drivers/media/platform/mtk-isp/isp_70/
obj-y += drivers/media/platform/mtk-camera/
obj-y += drivers/misc/mediatek/imgsensor/src-v4l2/

else

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) KBUILD_EXTRA_SYMBOLS=`pwd`/mtk-vcu-mt8395/Module.symvers

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean

endif
