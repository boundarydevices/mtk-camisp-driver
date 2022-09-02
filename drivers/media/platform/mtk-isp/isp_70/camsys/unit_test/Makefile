
ccflags-y += -I$(top)/drivers/media/platform/mtk-isp \
	     -I$(top)/drivers/media/platform/mtk-isp/isp_70/camsys/ \
	     -I$(top)/include \
	     -I$(top)/include/uapi

cam-ut-objs := mtk_cam_ut-cam.o \
	       mtk_cam_ut-seninf.o \
	       mtk_cam_ut.o \
	       mtk_cam_ut-camsv.o

obj-$(CONFIG_VIDEO_MTK_ISP_7_CAMSYS_UT) += cam-ut.o
