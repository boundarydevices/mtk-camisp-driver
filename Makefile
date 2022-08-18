ifneq ($(KERNELRELEASE),)

CONFIG_RPMSG_MTK_CCD=m
CONFIG_MTKCCD_REMOTEPROC=m
CONFIG_VIDEO_MTK_ISP_7_CAMSYS=m
CONFIG_VIDEO_MTK_ISP_7_IMGSYS=n

export CONFIG_RPMSG_MTK_CCD
export CONFIG_MTKCCD_REMOTEPROC
export CONFIG_VIDEO_MTK_ISP_7_CAMSYS
export CONFIG_VIDEO_MTK_ISP_7_IMGSYS

top := $(src)
export top

obj-y += drivers/rpmsg/
obj-y += drivers/remoteproc/
obj-y += drivers/media/platform/mtk-isp/isp_70/

else

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean

endif
