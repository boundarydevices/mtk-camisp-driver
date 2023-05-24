// SPDX-License-Identifier: GPL-2.0-only
/*
 * ar0830ap1302d2l.c - driver for AR0830AP1302D2L mezzanine
 *
 * Copyright (C) 2023, Witekio, Inc.
 *
 * This driver can only provide limited feature on AR0830AP1302D2L.
 * Still need enhancement
 */

#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/kernel.h>
#include <linux/media.h>
#include <media/media-entity.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-fwnode.h>
#include "adaptor.h"
#include "adaptor-hw.h"
#include "ar0830ap1302d2l_control.h"

#define DRIVER_NAME "ar0830ap1302d2l"

#define AR0830AP1302D2L_FW_WINDOW_SIZE		0x2000
#define AR0830AP1302D2L_FW_WINDOW_OFFSET	0x8000
#define AR0830AP1302D2L_MIN_WIDTH			24U
#define AR0830AP1302D2L_MIN_HEIGHT			16U
#define AR0830AP1302D2L_MAX_WIDTH			4224U
#define AR0830AP1302D2L_MAX_HEIGHT			4092U
#define AR0830AP1302D2L_OUT_WIDTH			3840U // 2316U
#define AR0830AP1302D2L_OUT_HEIGHT			2160U // 1746U

#define ENABLE_DEBUGFS 0
#define ENABLE_DEV_DEBUG_REG 0
/*
 * Register address macros encode the size and address of registers as 32-bit
 * integers formatted as follows.
 *
 * AR0S SSRR RRRR RRRR RRRR RRRR RRRR RRRR
 *
 * A: Advanced (1) or base (0) register
 * S: Size (1: 8-bit, 2: 16-bit, 4: 32-bit)
 * R: Register address
 *
 * For base registers, the address is limited to the 16 low order bits. For
 * advanced registers, the address is a 32-bit value.
 */
#define AR0830AP1302D2L_REG_ADV				BIT(31)
#define AR0830AP1302D2L_REG_8BIT(n)			((1 << 26) | (n))
#define AR0830AP1302D2L_REG_16BIT(n)			((2 << 26) | (n))
#define AR0830AP1302D2L_REG_32BIT(n)			((4 << 26) | (n))
#define AR0830AP1302D2L_REG_ADV_8BIT(n)			(AR0830AP1302D2L_REG_ADV | (1 << 26) | (n))
#define AR0830AP1302D2L_REG_ADV_16BIT(n)			(AR0830AP1302D2L_REG_ADV | (2 << 26) | (n))
#define AR0830AP1302D2L_REG_ADV_32BIT(n)			(AR0830AP1302D2L_REG_ADV | (4 << 26) | (n))
#define AR0830AP1302D2L_REG_SIZE(n)			(((n) >> 26) & 0x7)
#define AR0830AP1302D2L_REG_ADDR(n)			((n) & 0x43ffffff)
#define AR0830AP1302D2L_REG_PAGE_MASK			0x43fff000
#define AR0830AP1302D2L_REG_PAGE(n)			((n) & 0x43fff000)

/* Info Registers */
#define AR0830AP1302D2L_CHIP_VERSION			AR0830AP1302D2L_REG_16BIT(0x0000)
#define AR0830AP1302D2L_CHIP_ID				0x0265
#define AR0830AP1302D2L_FRAME_CNT			AR0830AP1302D2L_REG_8BIT(0x0002)
#define AR0830AP1302D2L_ERROR				AR0830AP1302D2L_REG_16BIT(0x0006)
#define AR0830AP1302D2L_ERR_FILE				AR0830AP1302D2L_REG_32BIT(0x0008)
#define AR0830AP1302D2L_ERR_LINE				AR0830AP1302D2L_REG_16BIT(0x000c)
#define AR0830AP1302D2L_SIPM_ERR_0			AR0830AP1302D2L_REG_16BIT(0x0014)
#define AR0830AP1302D2L_SIPM_ERR_1			AR0830AP1302D2L_REG_16BIT(0x0016)
#define AR0830AP1302D2L_CHIP_REV				AR0830AP1302D2L_REG_16BIT(0x0050)
#define AR0830AP1302D2L_CON_BUF				AR0830AP1302D2L_REG_8BIT(0x0a2c)
#define AR0830AP1302D2L_CON_BUF_SIZE			512

/* Control Registers */
#define AR0830AP1302D2L_DZ_TGT_FCT			AR0830AP1302D2L_REG_16BIT(0x1010)
#define AR0830AP1302D2L_SFX_MODE				AR0830AP1302D2L_REG_16BIT(0x1016)
#define AR0830AP1302D2L_SFX_MODE_SFX_NORMAL		(0U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_ALIEN		(1U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_ANTIQUE		(2U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_BW			(3U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_EMBOSS		(4U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_EMBOSS_COLORED	(5U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_GRAYSCALE		(6U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_NEGATIVE		(7U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_BLUISH		(8U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_GREENISH		(9U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_REDISH		(10U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_POSTERIZE1		(11U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_POSTERIZE2		(12U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_SEPIA1		(13U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_SEPIA2		(14U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_SKETCH		(15U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_SOLARIZE		(16U << 0)
#define AR0830AP1302D2L_SFX_MODE_SFX_FOGGY		(17U << 0)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT			AR0830AP1302D2L_REG_16BIT(0x1164)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FT_YUV		(3U << 4)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FT_RGB		(4U << 4)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FT_YUV_JFIF	(5U << 4)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FST_RGB_888	(0U << 0)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FST_RGB_565	(1U << 0)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FST_RGB_555M	(2U << 0)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FST_RGB_555L	(3U << 0)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FST_YUV_422	(0U << 0)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FST_YUV_420	(1U << 0)
#define AR0830AP1302D2L_BUBBLE_OUT_FMT_FST_YUV_400	(2U << 0)
#define AR0830AP1302D2L_ATOMIC				AR0830AP1302D2L_REG_16BIT(0x1184)
#define AR0830AP1302D2L_ATOMIC_MODE			BIT(2)
#define AR0830AP1302D2L_ATOMIC_FINISH			BIT(1)
#define AR0830AP1302D2L_ATOMIC_RECORD			BIT(0)

/*
 * Preview Context Registers (preview_*). AR0830AP1302D2L supports 3 "contexts"
 * (Preview, Snapshot, Video). These can be programmed for different size,
 * format, FPS, etc. There is no functional difference between the contexts,
 * so the only potential benefit of using them is reduced number of register
 * writes when switching output modes (if your concern is atomicity, see
 * "atomic" register).
 * So there's virtually no benefit in using contexts for this driver and it
 * would significantly increase complexity. Let's use preview context only.
 */
#define AR0830AP1302D2L_PREVIEW_WIDTH			AR0830AP1302D2L_REG_16BIT(0x2000)
#define AR0830AP1302D2L_PREVIEW_HEIGHT			AR0830AP1302D2L_REG_16BIT(0x2002)
#define AR0830AP1302D2L_PREVIEW_ROI_X0			AR0830AP1302D2L_REG_16BIT(0x2004)
#define AR0830AP1302D2L_PREVIEW_ROI_Y0			AR0830AP1302D2L_REG_16BIT(0x2006)
#define AR0830AP1302D2L_PREVIEW_ROI_X1			AR0830AP1302D2L_REG_16BIT(0x2008)
#define AR0830AP1302D2L_PREVIEW_ROI_Y1			AR0830AP1302D2L_REG_16BIT(0x200a)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT			AR0830AP1302D2L_REG_16BIT(0x2012)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_IPIPE_BYPASS	BIT(13)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_SS		BIT(12)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FAKE_EN		BIT(11)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_ST_EN		BIT(10)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_IIS_NONE		(0U << 8)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_IIS_POST_VIEW	(1U << 8)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_IIS_VIDEO	(2U << 8)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_IIS_BUBBLE	(3U << 8)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_JPEG_422	(0U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_JPEG_420	(1U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_YUV		(3U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_RGB		(4U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_YUV_JFIF	(5U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_RAW8		(8U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_RAW10		(9U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_RAW12		(10U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_RAW16		(11U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_DNG8		(12U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_DNG10		(13U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_DNG12		(14U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_DNG16		(15U << 4)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_JPEG_ROTATE	BIT(2)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_JPEG_SCAN	(0U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_JPEG_JFIF	(1U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_JPEG_EXIF	(2U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RGB_888	(0U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RGB_565	(1U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RGB_555M	(2U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RGB_555L	(3U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_YUV_422	(0U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_YUV_420	(1U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_YUV_400	(2U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_SENSOR	(0U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_CAPTURE	(1U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_CP	(2U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_BPC	(3U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_IHDR	(4U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_PP	(5U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_DENSH	(6U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_PM	(7U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_GC	(8U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_CURVE	(9U << 0)
#define AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_RAW_CCONV	(10U << 0)
#define AR0830AP1302D2L_PREVIEW_S1_SENSOR_MODE		AR0830AP1302D2L_REG_16BIT(0x202e)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL		AR0830AP1302D2L_REG_16BIT(0x2030)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_BT656_LE	BIT(15)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_BT656_16BIT	BIT(14)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_MUX_DELAY(n)	((n) << 8)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_LV_POL		BIT(7)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_FV_POL		BIT(6)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_MIPI_CONT_CLK	BIT(5)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_SPOOF		BIT(4)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_MIPI_MODE	BIT(3)
#define AR0830AP1302D2L_PREVIEW_HINF_CTRL_MIPI_LANES(n)	((n) << 0)

/* IQ Registers */
#define AR0830AP1302D2L_AE_BV_OFF			AR0830AP1302D2L_REG_16BIT(0x5014)
#define AR0830AP1302D2L_AWB_CTRL				AR0830AP1302D2L_REG_16BIT(0x5100)
#define AR0830AP1302D2L_AWB_CTRL_RECALC			BIT(13)
#define AR0830AP1302D2L_AWB_CTRL_POSTGAIN		BIT(12)
#define AR0830AP1302D2L_AWB_CTRL_UNGAIN			BIT(11)
#define AR0830AP1302D2L_AWB_CTRL_CLIP			BIT(10)
#define AR0830AP1302D2L_AWB_CTRL_SKY			BIT(9)
#define AR0830AP1302D2L_AWB_CTRL_FLASH			BIT(8)
#define AR0830AP1302D2L_AWB_CTRL_FACE_OFF		(0U << 6)
#define AR0830AP1302D2L_AWB_CTRL_FACE_IGNORE		(1U << 6)
#define AR0830AP1302D2L_AWB_CTRL_FACE_CONSTRAINED	(2U << 6)
#define AR0830AP1302D2L_AWB_CTRL_FACE_ONLY		(3U << 6)
#define AR0830AP1302D2L_AWB_CTRL_IMM			BIT(5)
#define AR0830AP1302D2L_AWB_CTRL_IMM1			BIT(4)
#define AR0830AP1302D2L_AWB_CTRL_MODE_OFF		(0U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_HORIZON		(1U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_A			(2U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_CWF		(3U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_D50		(4U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_D65		(5U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_D75		(6U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_MANUAL		(7U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_MEASURE		(8U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_AUTO		(15U << 0)
#define AR0830AP1302D2L_AWB_CTRL_MODE_MASK		0x000f
#define AR0830AP1302D2L_FLICK_CTRL			AR0830AP1302D2L_REG_16BIT(0x5440)
#define AR0830AP1302D2L_FLICK_CTRL_FREQ(n)		((n) << 8)
#define AR0830AP1302D2L_FLICK_CTRL_ETC_IHDR_UP		BIT(6)
#define AR0830AP1302D2L_FLICK_CTRL_ETC_DIS		BIT(5)
#define AR0830AP1302D2L_FLICK_CTRL_FRC_OVERRIDE_MAX_ET	BIT(4)
#define AR0830AP1302D2L_FLICK_CTRL_FRC_OVERRIDE_UPPER_ET	BIT(3)
#define AR0830AP1302D2L_FLICK_CTRL_FRC_EN		BIT(2)
#define AR0830AP1302D2L_FLICK_CTRL_MODE_DISABLED		(0U << 0)
#define AR0830AP1302D2L_FLICK_CTRL_MODE_MANUAL		(1U << 0)
#define AR0830AP1302D2L_FLICK_CTRL_MODE_AUTO		(2U << 0)
#define AR0830AP1302D2L_SCENE_CTRL			AR0830AP1302D2L_REG_16BIT(0x5454)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_NORMAL		(0U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_PORTRAIT		(1U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_LANDSCAPE	(2U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_SPORT		(3U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_CLOSE_UP		(4U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_NIGHT		(5U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_TWILIGHT		(6U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_BACKLIGHT	(7U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_HIGH_SENSITIVE	(8U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_NIGHT_PORTRAIT	(9U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_BEACH		(10U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_DOCUMENT		(11U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_PARTY		(12U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_FIREWORKS	(13U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_SUNSET		(14U << 0)
#define AR0830AP1302D2L_SCENE_CTRL_MODE_AUTO		(0xffU << 0)

/* System Registers */
#define AR0830AP1302D2L_BOOTDATA_STAGE			AR0830AP1302D2L_REG_16BIT(0x6002)
#define AR0830AP1302D2L_WARNING(n)			AR0830AP1302D2L_REG_16BIT(0x6004 + (n) * 2)
#define AR0830AP1302D2L_SENSOR_SELECT			AR0830AP1302D2L_REG_16BIT(0x600c)
#define AR0830AP1302D2L_SENSOR_SELECT_TP_MODE(n)		((n) << 8)
#define AR0830AP1302D2L_SENSOR_SELECT_PATTERN_ON		BIT(7)
#define AR0830AP1302D2L_SENSOR_SELECT_MODE_3D_ON		BIT(6)
#define AR0830AP1302D2L_SENSOR_SELECT_CLOCK		BIT(5)
#define AR0830AP1302D2L_SENSOR_SELECT_SINF_MIPI		BIT(4)
#define AR0830AP1302D2L_SENSOR_SELECT_YUV		BIT(2)
#define AR0830AP1302D2L_SENSOR_SELECT_SENSOR_TP		(0U << 0)
#define AR0830AP1302D2L_SENSOR_SELECT_SENSOR(n)		(((n) + 1) << 0)
#define AR0830AP1302D2L_SYS_START			AR0830AP1302D2L_REG_16BIT(0x601a)
#define AR0830AP1302D2L_SYS_START_PLL_LOCK		BIT(15)
#define AR0830AP1302D2L_SYS_START_LOAD_OTP		BIT(12)
#define AR0830AP1302D2L_SYS_START_RESTART_ERROR		BIT(11)
#define AR0830AP1302D2L_SYS_START_STALL_STATUS		BIT(9)
#define AR0830AP1302D2L_SYS_START_STALL_EN		BIT(8)
#define AR0830AP1302D2L_SYS_START_STALL_MODE_FRAME	(0U << 6)
#define AR0830AP1302D2L_SYS_START_STALL_MODE_DISABLED	(1U << 6)
#define AR0830AP1302D2L_SYS_START_STALL_MODE_POWER_DOWN	(2U << 6)
#define AR0830AP1302D2L_SYS_START_GO			BIT(4)
#define AR0830AP1302D2L_SYS_START_PATCH_FUN		BIT(1)
#define AR0830AP1302D2L_SYS_START_PLL_INIT		BIT(0)
#define AR0830AP1302D2L_DMA_SRC				AR0830AP1302D2L_REG_32BIT(0x60a0)
#define AR0830AP1302D2L_DMA_DST				AR0830AP1302D2L_REG_32BIT(0x60a4)
#define AR0830AP1302D2L_DMA_SIP_SIPM(n)			((n) << 26)
#define AR0830AP1302D2L_DMA_SIP_DATA_16_BIT		BIT(25)
#define AR0830AP1302D2L_DMA_SIP_ADDR_16_BIT		BIT(24)
#define AR0830AP1302D2L_DMA_SIP_ID(n)			((n) << 17)
#define AR0830AP1302D2L_DMA_SIP_REG(n)			((n) << 0)
#define AR0830AP1302D2L_DMA_SIZE				AR0830AP1302D2L_REG_32BIT(0x60a8)
#define AR0830AP1302D2L_DMA_CTRL				AR0830AP1302D2L_REG_16BIT(0x60ac)
#define AR0830AP1302D2L_DMA_CTRL_SCH_NORMAL		(0 << 12)
#define AR0830AP1302D2L_DMA_CTRL_SCH_NEXT		(1 << 12)
#define AR0830AP1302D2L_DMA_CTRL_SCH_NOW			(2 << 12)
#define AR0830AP1302D2L_DMA_CTRL_DST_REG			(0 << 8)
#define AR0830AP1302D2L_DMA_CTRL_DST_SRAM		(1 << 8)
#define AR0830AP1302D2L_DMA_CTRL_DST_SPI			(2 << 8)
#define AR0830AP1302D2L_DMA_CTRL_DST_SIP			(3 << 8)
#define AR0830AP1302D2L_DMA_CTRL_SRC_REG			(0 << 4)
#define AR0830AP1302D2L_DMA_CTRL_SRC_SRAM		(1 << 4)
#define AR0830AP1302D2L_DMA_CTRL_SRC_SPI			(2 << 4)
#define AR0830AP1302D2L_DMA_CTRL_SRC_SIP			(3 << 4)
#define AR0830AP1302D2L_DMA_CTRL_MODE_32_BIT		BIT(3)
#define AR0830AP1302D2L_DMA_CTRL_MODE_MASK		(7 << 0)
#define AR0830AP1302D2L_DMA_CTRL_MODE_IDLE		(0 << 0)
#define AR0830AP1302D2L_DMA_CTRL_MODE_SET		(1 << 0)
#define AR0830AP1302D2L_DMA_CTRL_MODE_COPY		(2 << 0)
#define AR0830AP1302D2L_DMA_CTRL_MODE_MAP		(3 << 0)
#define AR0830AP1302D2L_DMA_CTRL_MODE_UNPACK		(4 << 0)
#define AR0830AP1302D2L_DMA_CTRL_MODE_OTP_READ		(5 << 0)
#define AR0830AP1302D2L_DMA_CTRL_MODE_SIP_PROBE		(6 << 0)

/* Misc Registers */
#define AR0830AP1302D2L_REG_ADV_START			0xe000
#define AR0830AP1302D2L_ADVANCED_BASE			AR0830AP1302D2L_REG_32BIT(0xf038)
#define AR0830AP1302D2L_SIP_CRC				AR0830AP1302D2L_REG_16BIT(0xf052)
#define AR0830AP1302D2L_BOOTDATA_CHECKSUM		AR0830AP1302D2L_REG_16BIT(0x6134)

/* Advanced System Registers */
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE			AR0830AP1302D2L_REG_ADV_32BIT(0x00230000)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_TEST_COUNT	BIT(25)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_HINF_1		BIT(24)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_HINF_0		BIT(23)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SINF_B_MIPI_L	(7U << 20)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SINF_B_MIPI	BIT(19)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SINF_A_MIPI_L	(15U << 14)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SINF_A_MIPI	BIT(13)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SINF		BIT(12)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_IPIPE_S		BIT(11)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_IPIPE_B		BIT(10)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_IPIPE_A		BIT(9)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_IP		BIT(8)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_TIMER		BIT(7)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SIPM		(3U << 6)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SIPS_ADR_RANGE	BIT(5)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SIPS_DIRECT_WRITE	BIT(4)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SIPS_FIFO_WRITE	BIT(3)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SPI		BIT(2)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_GPIO_CNT	BIT(1)
#define AR0830AP1302D2L_ADV_IRQ_SYS_INTE_GPIO_PIN	BIT(0)

/* Advanced Slave MIPI Registers */
#define AR0830AP1302D2L_ADV_SINF_MIPI_INTERNAL_p_LANE_n_STAT(p, n) \
	AR0830AP1302D2L_REG_ADV_32BIT(0x00420008 + (p) * 0x50000 + (n) * 0x20)
#define AR0830AP1302D2L_LANE_ERR_LP_VAL(n)		(((n) >> 30) & 3)
#define AR0830AP1302D2L_LANE_ERR_STATE(n)		(((n) >> 24) & 0xf)
#define AR0830AP1302D2L_LANE_ERR				BIT(18)
#define AR0830AP1302D2L_LANE_ABORT			BIT(17)
#define AR0830AP1302D2L_LANE_LP_VAL(n)			(((n) >> 6) & 3)
#define AR0830AP1302D2L_LANE_STATE(n)			((n) & 0xf)
#define AR0830AP1302D2L_LANE_STATE_STOP_S		0x0
#define AR0830AP1302D2L_LANE_STATE_HS_REQ_S		0x1
#define AR0830AP1302D2L_LANE_STATE_LP_REQ_S		0x2
#define AR0830AP1302D2L_LANE_STATE_HS_S			0x3
#define AR0830AP1302D2L_LANE_STATE_LP_S			0x4
#define AR0830AP1302D2L_LANE_STATE_ESC_REQ_S		0x5
#define AR0830AP1302D2L_LANE_STATE_TURN_REQ_S		0x6
#define AR0830AP1302D2L_LANE_STATE_ESC_S			0x7
#define AR0830AP1302D2L_LANE_STATE_ESC_0			0x8
#define AR0830AP1302D2L_LANE_STATE_ESC_1			0x9
#define AR0830AP1302D2L_LANE_STATE_TURN_S		0xa
#define AR0830AP1302D2L_LANE_STATE_TURN_MARK		0xb
#define AR0830AP1302D2L_LANE_STATE_ERROR_S		0xc

#define AR0830AP1302D2L_ADV_CAPTURE_A_FV_CNT		AR0830AP1302D2L_REG_ADV_16BIT(0x00490042)

struct ar0830ap1302d2l_device;

enum {
	AR0830AP1302D2L_PAD_SINK_0,
	AR0830AP1302D2L_PAD_SINK_1,
	AR0830AP1302D2L_PAD_SOURCE,
	AR0830AP1302D2L_PAD_MAX,
};

struct ar0830ap1302d2l_format_info {
	unsigned int code;
	u16 out_fmt;
};

struct ar0830ap1302d2l_size {
	unsigned int width;
	unsigned int height;
};

struct ar0830ap1302d2l_sensor_supply {
	const char *name;
	unsigned int post_delay_us;
};

struct ar0830ap1302d2l_firmware_header {
	u16 pll_init_size;
	u16 check;
} __packed;

#define MAX_FW_LOAD_RETRIES		5
#define MAX_CHIP_DETECT_RETRIES		5
#define MAX_CHECK_RETRIES		5

static const struct ar0830ap1302d2l_format_info supported_video_formats[] = {
	{
		.code = MEDIA_BUS_FMT_UYVY8_1X16,
		.out_fmt = AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_YUV_JFIF
			 | AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_YUV_422,
	}, {
		.code = MEDIA_BUS_FMT_UYYVYY8_0_5X24,
		.out_fmt = AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_YUV_JFIF
			 | AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_YUV_420,
	},
};

/* -----------------------------------------------------------------------------
 * Register Configuration
 */

static int __ar0830ap1302d2l_write(struct ar0830ap1302d2l_device *ar0830ap1302d2l, u32 reg, u32 val)
{
	unsigned int size = AR0830AP1302D2L_REG_SIZE(reg);
	u16 addr = AR0830AP1302D2L_REG_ADDR(reg);
	unsigned int i;
	u8 buf[6];
	int ret;

	buf[0] = (addr >> 8) & 0xff;
	buf[1] = (addr >> 0) & 0xff;

	for (i = 0; i < size; ++i) {
		buf[2 + size - i - 1] = val & 0xff;
		val >>= 8;
	}

	ret = i2c_master_send(ar0830ap1302d2l->client, buf, size + 2);
	if (ret != size + 2) {
		dev_err(ar0830ap1302d2l->dev, "%s: register 0x%04x %s failed: %d\n",
			__func__, addr, "write", ret);
		return ret;
	}

	return 0;
}

static int ar0830ap1302d2l_write(struct ar0830ap1302d2l_device *ar0830ap1302d2l, u32 reg, u32 val,
			int *err)
{
	int ret;

	if (err && *err)
		return *err;

	if (reg & AR0830AP1302D2L_REG_ADV) {
		u32 page = AR0830AP1302D2L_REG_PAGE(reg);

		if (ar0830ap1302d2l->reg_page != page) {
			ret = __ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_ADVANCED_BASE,
					     page);
			if (ret < 0)
				goto done;

			ar0830ap1302d2l->reg_page = page;
		}

		reg &= ~AR0830AP1302D2L_REG_ADV;
		reg &= ~AR0830AP1302D2L_REG_PAGE_MASK;
		reg += AR0830AP1302D2L_REG_ADV_START;
	}

	ret = __ar0830ap1302d2l_write(ar0830ap1302d2l, reg, val);

done:
	if (err && ret)
		*err = ret;

	return ret;
}

static int __ar0830ap1302d2l_read_raw(struct ar0830ap1302d2l_device *ar0830ap1302d2l, u16 addr,
			     unsigned int size, u8 *data)
{
	u8 buf[2];
	int ret;

	struct i2c_msg msgs[2] = {
		{
			.addr = ar0830ap1302d2l->client->addr,
			.flags = 0,
			.len = 2,
			.buf = buf,
		}, {
			.addr = ar0830ap1302d2l->client->addr,
			.flags = I2C_M_RD,
			.len = size,
			.buf = data,
		}
	};

	buf[0] = (addr >> 8) & 0xff;
	buf[1] = (addr >> 0) & 0xff;

	ret = i2c_transfer(ar0830ap1302d2l->client->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret != ARRAY_SIZE(msgs)) {
		dev_err(ar0830ap1302d2l->dev, "%s: register 0x%04x %s failed: %d\n",
			__func__, addr, "read", ret);
		return ret;
	}

	return 0;

}

static int __ar0830ap1302d2l_read(struct ar0830ap1302d2l_device *ar0830ap1302d2l, u32 reg, u32 *val)
{
	unsigned int size = AR0830AP1302D2L_REG_SIZE(reg);
	u16 addr = AR0830AP1302D2L_REG_ADDR(reg);
	unsigned int i;
	u32 value = 0;
	u8 data[4];
	int ret;

	ret = __ar0830ap1302d2l_read_raw(ar0830ap1302d2l, addr, size, data);
	if (ret < 0)
		return ret;

	for (i = 0; i < size; ++i) {
		value <<= 8;
		value |= data[i];
	}

	// dev_dbg(ar0830ap1302d2l->dev, "%s: R0x%04x = 0x%0*x\n", __func__,
	//   addr, size * 2, value);

	*val = value;

	return 0;
}

static int ar0830ap1302d2l_read(struct ar0830ap1302d2l_device *ar0830ap1302d2l, u32 reg, u32 *val)
{
	int ret;

	if (reg & AR0830AP1302D2L_REG_ADV) {
		u32 page = AR0830AP1302D2L_REG_PAGE(reg);

		if (ar0830ap1302d2l->reg_page != page) {
			ret = __ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_ADVANCED_BASE,
					     page);
			if (ret < 0)
				return ret;

			ar0830ap1302d2l->reg_page = page;
		}

		reg &= ~AR0830AP1302D2L_REG_ADV;
		reg &= ~AR0830AP1302D2L_REG_PAGE_MASK;
		reg += AR0830AP1302D2L_REG_ADV_START;
	}

	return __ar0830ap1302d2l_read(ar0830ap1302d2l, reg, val);
}

#if ENABLE_DEBUGFS
/* -----------------------------------------------------------------------------
 * Sensor Registers Access
 *
 * Read and write sensor registers through the AR0830AP1302D2L DMA interface.
 */

static int ar0830ap1302d2l_dma_wait_idle(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	unsigned int i;
	u32 ctrl;
	int ret;

	for (i = 50; i > 0; i--) {
		ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_DMA_CTRL, &ctrl);
		if (ret < 0)
			return ret;

		if ((ctrl & AR0830AP1302D2L_DMA_CTRL_MODE_MASK) ==
		    AR0830AP1302D2L_DMA_CTRL_MODE_IDLE)
			break;

		usleep_range(1000, 1500);
	}

	if (!i) {
		dev_err(ar0830ap1302d2l->dev, "DMA timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int ar0830ap1302d2l_sipm_read(struct ar0830ap1302d2l_device *ar0830ap1302d2l, unsigned int port,
			    unsigned int size, u16 reg, u32 *val)
{
	u32 src;
	int ret;

	if (size > 2)
		return -EINVAL;

	ret = ar0830ap1302d2l_dma_wait_idle(ar0830ap1302d2l);
	if (ret < 0)
		return ret;

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_SIZE, size, &ret);
	src = AR0830AP1302D2L_DMA_SIP_SIPM(port)
	    | (size == 2 ? AR0830AP1302D2L_DMA_SIP_DATA_16_BIT : 0)
	    | AR0830AP1302D2L_DMA_SIP_ADDR_16_BIT
	    // | AR0830AP1302D2L_DMA_SIP_ID(ar0830ap1302d2l->sensor_info->i2c_addr) // TODO
		| AR0830AP1302D2L_DMA_SIP_ID(0x36)
	    | AR0830AP1302D2L_DMA_SIP_REG(reg);
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_SRC, src, &ret);

	/*
	 * Use the AR0830AP1302D2L_DMA_DST register as both the destination address, and
	 * the scratch pad to store the read value.
	 */
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_DST, AR0830AP1302D2L_REG_ADDR(AR0830AP1302D2L_DMA_DST),
		     &ret);

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_CTRL,
		     AR0830AP1302D2L_DMA_CTRL_SCH_NORMAL |
		     AR0830AP1302D2L_DMA_CTRL_DST_REG |
		     AR0830AP1302D2L_DMA_CTRL_SRC_SIP |
		     AR0830AP1302D2L_DMA_CTRL_MODE_COPY, &ret);
	if (ret < 0)
		return ret;

	ret = ar0830ap1302d2l_dma_wait_idle(ar0830ap1302d2l);
	if (ret < 0)
		return ret;

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_DMA_DST, val);
	if (ret < 0)
		return ret;

	/*
	 * The value is stored in big-endian at the DMA_DST address. The regmap
	 * uses big-endian, so 8-bit values are stored in bits 31:24 and 16-bit
	 * values in bits 23:16.
	 */
	*val >>= 32 - size * 8;

	return 0;
}

static int ar0830ap1302d2l_sipm_write(struct ar0830ap1302d2l_device *ar0830ap1302d2l, unsigned int port,
			     unsigned int size, u16 reg, u32 val)
{
	u32 dst;
	int ret;

	if (size > 2)
		return -EINVAL;

	ret = ar0830ap1302d2l_dma_wait_idle(ar0830ap1302d2l);
	if (ret < 0)
		return ret;

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_SIZE, size, &ret);

	/*
	 * Use the AR0830AP1302D2L_DMA_SRC register as both the source address, and the
	 * scratch pad to store the write value.
	 *
	 * As the AR0830AP1302D2L uses big endian, to store the value at address DMA_SRC
	 * it must be written in the high order bits of the registers. However,
	 * 8-bit values seem to be incorrectly handled by the AR0830AP1302D2L, which
	 * expects them to be stored at DMA_SRC + 1 instead of DMA_SRC. The
	 * value is thus unconditionally shifted by 16 bits, unlike for DMA
	 * reads.
	 */
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_SRC,
		     (val << 16) | AR0830AP1302D2L_REG_ADDR(AR0830AP1302D2L_DMA_SRC), &ret);
	if (ret < 0)
		return ret;

	dst = AR0830AP1302D2L_DMA_SIP_SIPM(port)
	    | (size == 2 ? AR0830AP1302D2L_DMA_SIP_DATA_16_BIT : 0)
	    | AR0830AP1302D2L_DMA_SIP_ADDR_16_BIT
		// | AR0830AP1302D2L_DMA_SIP_ID(ar0830ap1302d2l->sensor_info->i2c_addr) // TODO
	    | AR0830AP1302D2L_DMA_SIP_ID(0x36)
	    | AR0830AP1302D2L_DMA_SIP_REG(reg);
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_DST, dst, &ret);

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_DMA_CTRL,
		     AR0830AP1302D2L_DMA_CTRL_SCH_NORMAL |
		     AR0830AP1302D2L_DMA_CTRL_DST_SIP |
		     AR0830AP1302D2L_DMA_CTRL_SRC_REG |
		     AR0830AP1302D2L_DMA_CTRL_MODE_COPY, &ret);
	if (ret < 0)
		return ret;

	ret = ar0830ap1302d2l_dma_wait_idle(ar0830ap1302d2l);
	if (ret < 0)
		return ret;

	return 0;
}

/* -----------------------------------------------------------------------------
 * Debugfs
 */

static int ar0830ap1302d2l_sipm_addr_get(void *arg, u64 *val)
{
	struct ar0830ap1302d2l_device *ar0830ap1302d2l = arg;

	mutex_lock(&ar0830ap1302d2l->debugfs.lock);
	*val = ar0830ap1302d2l->debugfs.sipm_addr;
	mutex_unlock(&ar0830ap1302d2l->debugfs.lock);

	return 0;
}

static int ar0830ap1302d2l_sipm_addr_set(void *arg, u64 val)
{
	struct ar0830ap1302d2l_device *ar0830ap1302d2l = arg;

	if (val & ~0x8700ffff)
		return -EINVAL;

	switch ((val >> 24) & 7) {
	case 1:
	case 2:
		break;
	default:
		return -EINVAL;
	}

	mutex_lock(&ar0830ap1302d2l->debugfs.lock);
	ar0830ap1302d2l->debugfs.sipm_addr = val;
	mutex_unlock(&ar0830ap1302d2l->debugfs.lock);

	return 0;
}

static int ar0830ap1302d2l_sipm_data_get(void *arg, u64 *val)
{
	struct ar0830ap1302d2l_device *ar0830ap1302d2l = arg;
	u32 value;
	u32 addr;
	int ret;

	mutex_lock(&ar0830ap1302d2l->debugfs.lock);

	addr = ar0830ap1302d2l->debugfs.sipm_addr;
	if (!addr) {
		ret = -EINVAL;
		goto unlock;
	}

	ret = ar0830ap1302d2l_sipm_read(ar0830ap1302d2l, addr >> 31, (addr >> 24) & 7,
			       addr & 0xffff, &value);
	if (!ret)
		*val = value;

unlock:
	mutex_unlock(&ar0830ap1302d2l->debugfs.lock);

	return ret;
}

static int ar0830ap1302d2l_sipm_data_set(void *arg, u64 val)
{
	struct ar0830ap1302d2l_device *ar0830ap1302d2l = arg;
	u32 addr;
	int ret;

	mutex_lock(&ar0830ap1302d2l->debugfs.lock);

	addr = ar0830ap1302d2l->debugfs.sipm_addr;
	if (!addr) {
		ret = -EINVAL;
		goto unlock;
	}

	ret = ar0830ap1302d2l_sipm_write(ar0830ap1302d2l, addr >> 31, (addr >> 24) & 7,
				addr & 0xffff, val);

unlock:
	mutex_unlock(&ar0830ap1302d2l->debugfs.lock);

	return ret;
}

/*
 * The sipm_addr and sipm_data attributes expose access to the sensor I2C bus.
 *
 * To read or write a register, sipm_addr has to first be written with the
 * register address. The address is a 32-bit integer formatted as follows.
 *
 * I000 0SSS 0000 0000 RRRR RRRR RRRR RRRR
 *
 * I: SIPM index (0 or 1)
 * S: Size (1: 8-bit, 2: 16-bit)
 * R: Register address (16-bit)
 *
 * The sipm_data attribute can then be read to read the register value, or
 * written to write it.
 */

DEFINE_DEBUGFS_ATTRIBUTE(ar0830ap1302d2l_sipm_addr_fops, ar0830ap1302d2l_sipm_addr_get,
			 ar0830ap1302d2l_sipm_addr_set, "0x%08llx\n");
DEFINE_DEBUGFS_ATTRIBUTE(ar0830ap1302d2l_sipm_data_fops, ar0830ap1302d2l_sipm_data_get,
			 ar0830ap1302d2l_sipm_data_set, "0x%08llx\n");

static void ar0830ap1302d2l_debugfs_init(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	struct dentry *dir;
	char name[16];

	mutex_init(&ar0830ap1302d2l->debugfs.lock);

	snprintf(name, sizeof(name), "ar0830ap1302d2l.%s", dev_name(ar0830ap1302d2l->dev));

	dir = debugfs_create_dir(name, NULL);
	if (IS_ERR(dir))
		return;

	ar0830ap1302d2l->debugfs.dir = dir;

	debugfs_create_file_unsafe("sipm_addr", 0600, ar0830ap1302d2l->debugfs.dir,
				   ar0830ap1302d2l, &ar0830ap1302d2l_sipm_addr_fops);
	debugfs_create_file_unsafe("sipm_data", 0600, ar0830ap1302d2l->debugfs.dir,
				   ar0830ap1302d2l, &ar0830ap1302d2l_sipm_data_fops);
}

static void ar0830ap1302d2l_debugfs_cleanup(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	debugfs_remove_recursive(ar0830ap1302d2l->debugfs.dir);
	mutex_destroy(&ar0830ap1302d2l->debugfs.lock);
}
#endif

/* -----------------------------------------------------------------------------
 * Hardware Configuration
 */

static int ar0830ap1302d2l_dump_console(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	u8 *buffer;
	u8 *endp;
	u8 *p;
	int ret;

	buffer = kmalloc(AR0830AP1302D2L_CON_BUF_SIZE + 1, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	ret = __ar0830ap1302d2l_read_raw(ar0830ap1302d2l, AR0830AP1302D2L_REG_ADDR(AR0830AP1302D2L_CON_BUF),
				AR0830AP1302D2L_CON_BUF_SIZE, buffer);
	if (ret < 0) {
		dev_err(ar0830ap1302d2l->dev, "Failed to read console buffer: %d\n",
			ret);
		goto done;
	}

	print_hex_dump(KERN_INFO, "console ", DUMP_PREFIX_OFFSET, 16, 1, buffer,
		       AR0830AP1302D2L_CON_BUF_SIZE, true);

	buffer[AR0830AP1302D2L_CON_BUF_SIZE] = '\0';

	for (p = buffer; p < buffer + AR0830AP1302D2L_CON_BUF_SIZE && *p; p = endp + 1) {
		endp = strchrnul(p, '\n');
		*endp = '\0';

		dev_dbg(ar0830ap1302d2l->dev, "console %s\n", p);
	}

	ret = 0;

done:
	kfree(buffer);
	return ret;
}

static int ar0830ap1302d2l_configure(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	// const struct ar0830ap1302d2l_format *format = &ar0830ap1302d2l->formats[AR0830AP1302D2L_PAD_SOURCE];
	// unsigned int data_lanes = ar0830ap1302d2l->bus_cfg.bus.mipi_csi2.num_data_lanes;
	struct v4l2_subdev *sd = i2c_get_clientdata(ar0830ap1302d2l->client);
	struct adaptor_ctx *ad_ctx = container_of(sd, struct adaptor_ctx, sd);
	unsigned int data_lanes = ad_ctx->ep.bus.mipi_csi2.num_data_lanes;
	int ret = 0;
	int val = 0;

	dev_info(ar0830ap1302d2l->dev, "ap1320 ad_ctx num-data-lane: %u", ad_ctx->ep.bus.mipi_csi2.num_data_lanes);

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_PREVIEW_HINF_CTRL,
		     AR0830AP1302D2L_PREVIEW_HINF_CTRL_SPOOF |
		     AR0830AP1302D2L_PREVIEW_HINF_CTRL_MIPI_LANES(data_lanes), &ret);

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_PREVIEW_OUT_FMT,
		     AR0830AP1302D2L_PREVIEW_OUT_FMT_FT_YUV_JFIF | AR0830AP1302D2L_PREVIEW_OUT_FMT_FST_YUV_422, &ret);

	// set minimum mipi_t_hs_exit (us, u12.4)
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x201a), 0x20, &ret);
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x301a), 0x20, &ret);
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x401a), 0x20, &ret);

	// target mipi data rate
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_REG_32BIT(0x6034), 0x4ae0000, &ret);
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_32BIT(0x6034), &val);
	dev_dbg(ar0830ap1302d2l->dev, "R6034 = 0x%x", val);

	if (ret < 0)
		return ret;

	// __v4l2_ctrl_handler_setup(&ar0830ap1302d2l->ctrls);

	return 0;
}

static int ar0830ap1302d2l_stall(struct ar0830ap1302d2l_device *ar0830ap1302d2l, bool stall)
{
	int ret = 0;
	int i;
	u32 value;

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START, &value);
	if (ret < 0)
		return ret;

	if ((value & AR0830AP1302D2L_SYS_START_STALL_STATUS) == stall) {
		dev_dbg(ar0830ap1302d2l->dev, "Stall status = 0x%x, not need to set it again", value);
		return 0;
	}

	if (stall) {
		dev_dbg(ar0830ap1302d2l->dev, "Stall ...");
		ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START,
			     AR0830AP1302D2L_SYS_START_PLL_LOCK |
			     AR0830AP1302D2L_SYS_START_STALL_MODE_DISABLED, &ret);
		ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START,
			     AR0830AP1302D2L_SYS_START_PLL_LOCK |
			     AR0830AP1302D2L_SYS_START_STALL_EN |
			     AR0830AP1302D2L_SYS_START_STALL_MODE_DISABLED, &ret);

		if (ret < 0)
			return ret;

		msleep(200);

		ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_ADV_IRQ_SYS_INTE,
			     AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SIPM |
			     AR0830AP1302D2L_ADV_IRQ_SYS_INTE_SIPS_FIFO_WRITE, &ret);
		if (ret < 0)
			return ret;
	} else {
		dev_dbg(ar0830ap1302d2l->dev, "Un-stall ...", value);
		return ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START,
				    AR0830AP1302D2L_SYS_START_PLL_LOCK |
				    AR0830AP1302D2L_SYS_START_STALL_STATUS |
				    AR0830AP1302D2L_SYS_START_STALL_EN |
				    AR0830AP1302D2L_SYS_START_STALL_MODE_DISABLED, NULL);

	}

	return 0;
}

/* -----------------------------------------------------------------------------
 * Boot & Firmware Handling
 */

int ar0830ap1302d2l_request_firmware(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	// static const char * const suffixes[] = {
	//  "",
	//  "_single",
	//  "_dual",
	// };

	const struct ar0830ap1302d2l_firmware_header *fw_hdr;
	// unsigned int num_sensors;
	unsigned int fw_size;
	// unsigned int i;
	// char name[64];
	char name[] = "ap1302_ar0830_single_fw.bin";
	int ret;

	ret = request_firmware(&ar0830ap1302d2l->fw, name, ar0830ap1302d2l->dev);
	if (ret) {
		dev_err(ar0830ap1302d2l->dev, "Failed to request firmware: %d\n", ret);
		return ret;
	}

	/*
	 * The firmware binary contains a header defined by the
	 * ar0830ap1302d2l_firmware_header structure. The firmware itself (also referred
	 * to as bootdata) follows the header. Perform sanity checks to ensure
	 * the firmware is valid.
	 */
	fw_hdr = (const struct ar0830ap1302d2l_firmware_header *)ar0830ap1302d2l->fw->data;
	fw_size = ar0830ap1302d2l->fw->size - sizeof(*fw_hdr);

	if (fw_hdr->pll_init_size > fw_size) {
		dev_err(ar0830ap1302d2l->dev,
			"Invalid firmware: PLL init size too large\n");
		return -EINVAL;
	}

	return 0;
}

/*
 * ar0830ap1302d2l_write_fw_window() - Write a piece of firmware to the AR0830AP1302D2L
 * @win_pos: Firmware load window current position
 * @data: Firmware data buffer
 * @len: Firmware data length
 * @buf: Write buffer
 *
 * The firmware is loaded through a window in the registers space. Writes are
 * sequential starting at address 0x8000, and must wrap around when reaching
 * 0x9fff. This function writes the firmware data stored in @data to the AR0830AP1302D2L,
 * keeping track of the window position in the @win_pos argument.
 *
 * The write buffer is used to format the data to be written to the device
 * through I2C. It must be allocated by the caller, be suitable for DMA, be at
 * least AR0830AP1302D2L_FW_WINDOW_OFFSET + 2 bytes long, and may be reused across
 * multiple calls to this function.
 */
static int ar0830ap1302d2l_write_fw_window(struct ar0830ap1302d2l_device *ar0830ap1302d2l, const u8 *data,
				  u32 len, unsigned int *win_pos, u8 *buf)
{
	while (len > 0) {
		unsigned int write_addr;
		unsigned int write_size;
		int ret;

		/*
		 * Write at most len bytes, from the current position to the
		 * end of the window.
		 */
		write_addr = *win_pos + AR0830AP1302D2L_FW_WINDOW_OFFSET;
		write_size = min(len, AR0830AP1302D2L_FW_WINDOW_SIZE - *win_pos);

		buf[0] = (write_addr >> 8) & 0xff;
		buf[1] = (write_addr >> 0) & 0xff;

		memcpy(&buf[2], data, write_size);

		ret = i2c_master_send_dmasafe(ar0830ap1302d2l->client, buf,
					      write_size + 2);
		if (ret != write_size + 2) {
			dev_err(ar0830ap1302d2l->dev,
				"%s: firmware write @0x%04x failed: %d\n",
				__func__, write_addr, ret);
			return ret;
		}

		data += write_size;
		len -= write_size;

		*win_pos += write_size;
		if (*win_pos >= AR0830AP1302D2L_FW_WINDOW_SIZE)
			*win_pos = 0;
	}

	return 0;
}

int ar0830ap1302d2l_load_firmware(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	const struct ar0830ap1302d2l_firmware_header *fw_hdr;
	unsigned int fw_size;
	const u8 *fw_data;
	unsigned int win_pos = 0;
	unsigned int checksum;
	unsigned int crc;
	unsigned int stat;
	unsigned int retries;
	u8 *buf;
	int ret;

	/* dbg start */
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_CHIP_VERSION, &stat);
	dev_dbg(ar0830ap1302d2l->dev, "Check chip ver = 0x%x", stat);
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_CHIP_REV, &stat);
	dev_dbg(ar0830ap1302d2l->dev, "Check chip revision = 0x%x", stat);
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x004e), &stat);
	dev_dbg(ar0830ap1302d2l->dev, "Check f/w revision = 0x%x", stat);
	/* dbg end */

	buf = kmalloc(AR0830AP1302D2L_FW_WINDOW_SIZE + 2, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	fw_hdr = (const struct ar0830ap1302d2l_firmware_header *)ar0830ap1302d2l->fw->data;
	fw_data = (u8 *)&fw_hdr[1];
	fw_size = ar0830ap1302d2l->fw->size - sizeof(*fw_hdr);
	dev_dbg(ar0830ap1302d2l->dev, "fw_size = 0x%x", fw_size);

	/* dbg start */
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START, &stat);
	dev_dbg(ar0830ap1302d2l->dev, "before1 , SYS_START = 0x%x", stat);
	/* dbg end */

	/* Clear the CHECKSUM register. */
	ret = ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_BOOTDATA_CHECKSUM, 0x0000, NULL);
	if (ret)
		goto done;

	/* Clear the CRC register. */
	ret = ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_SIP_CRC, 0xffff, NULL);
	if (ret)
		goto done;

	/*
	 * Load the PLL initialization settings, set the bootdata stage to 2 to
	 * apply the basic_init_hp settings, and wait 1ms for the PLL to lock.
	 */
	/* dbg start */
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START, &stat);
	dev_dbg(ar0830ap1302d2l->dev, "before2 , SYS_START = 0x%x", stat);
	/* dbg end */

	ret = ar0830ap1302d2l_write_fw_window(ar0830ap1302d2l, fw_data, fw_hdr->pll_init_size,
				     &win_pos, buf);
	if (ret)
		goto done;

	ret = ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_BOOTDATA_STAGE, 0x0002, NULL);
	if (ret)
		goto done;

	usleep_range(1000, 2000);
	/* dbg start */
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START, &stat);
	dev_dbg(ar0830ap1302d2l->dev, "PLL Init , SYS_START = 0x%x", stat);
	/* dbg end */

	/* Load the rest of the bootdata content and verify the CHECKSUM or the CRC. */
	ret = ar0830ap1302d2l_write_fw_window(ar0830ap1302d2l, fw_data + fw_hdr->pll_init_size,
				     fw_size - fw_hdr->pll_init_size, &win_pos,
				     buf);
	if (ret)
		goto done;

	msleep(40);

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SIP_CRC, &crc);
	if (ret)
		goto done;

	/*
	 * Write 0xffff to the bootdata_stage register to indicate to the
	 * AR0830AP1302D2L that the whole bootdata content has been loaded.
	 */
	ret = ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_BOOTDATA_STAGE, 0xffff, NULL);
	if (ret)
		goto done;

	for (retries = 0; retries < MAX_CHECK_RETRIES; ++retries) {
		ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_BOOTDATA_CHECKSUM, &checksum);
		if (ret)
			goto done;

		if ((checksum != 0 && checksum == fw_hdr->check) || crc == fw_hdr->check)
			break;
		ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START, &stat);
		dev_dbg(ar0830ap1302d2l->dev, "wait bootstage , SYS_START = 0x%x", stat);
		msleep(100);
	}

	if (retries == MAX_CHECK_RETRIES) {
		dev_err(ar0830ap1302d2l->dev,
				 "CHECK mismatch: expected 0x%04x, got CHECKSUM 0x%04x CRC 0x%04x\n",
				 fw_hdr->check, checksum, crc);
		ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_ERROR, &stat);
		dev_err(ar0830ap1302d2l->dev, "AR0830AP1302D2L_ERROR = 0x%x", stat);
		ret = -EAGAIN;
		goto done;
	}
	msleep(300);

	/* The AR0830AP1302D2L starts outputting frames right after boot, stop it. */
	ret = ar0830ap1302d2l_stall(ar0830ap1302d2l, true);

done:
	kfree(buf);
	return ret;
}
static const char * const ar0830ap1302d2l_lane_states[] = {
	"stop_s",
	"hs_req_s",
	"lp_req_s",
	"hs_s",
	"lp_s",
	"esc_req_s",
	"turn_req_s",
	"esc_s",
	"esc_0",
	"esc_1",
	"turn_s",
	"turn_mark",
	"error_s",
};
static const char * const ar0830ap1302d2l_warnings[] = {
	"HINF_BANDWIDTH",
	"FLICKER_DETECTION",
	"FACED_NE",
	"SMILED_NE",
	"HINF_OVERRUN",
	NULL,
	"FRAME_TOO_SMALL",
	"MISSING_PHASES",
	"SPOOF_UNDERRUN",
	"JPEG_NOLAST",
	"NO_IN_FREQ_SPEC",
	"SINF0",
	"SINF1",
	"CAPTURE0",
	"CAPTURE1",
	"ISR_UNHANDLED",
	"INTERLEAVE_SPOOF",
	"INTERLEAVE_BUF",
	"COORD_OUT_OF_RANGE",
	"ICP_CLOCKING",
	"SENSOR_CLOCKING",
	"SENSOR_NO_IHDR",
	"DIVIDE_BY_ZERO",
	"INT0_UNDERRUN",
	"INT1_UNDERRUN",
	"SCRATCHPAD_TOO_BIG",
	"OTP_RECORD_READ",
	"NO_LSC_IN_OTP",
	"GPIO_INT_LOST",
	"NO_PDAF_DATA",
	"FAR_PDAF_ACCESS_SKIP",
	"PDAF_ERROR",
	"ATM_TVI_BOUNDS",
	"SIPM_0_RTY",
	"SIPM_1_TRY",
	"SIPM_0_NO_ACK",
	"SIPM_1_NO_ACK",
	"SMILE_DIS",
	"DVS_DIS",
	"TEST_DIS",
	"SENSOR_LV2LV",
	"SENSOR_FV2FV",
	"FRAME_LOST",
};
static void ar0830ap1302d2l_log_lane_state(struct ar0830ap1302d2l_sensor *sensor,
				  unsigned int index)
{
	static const char * const lp_states[] = {
		"00", "10", "01", "11",
	};

	unsigned int counts[4][ARRAY_SIZE(ar0830ap1302d2l_lane_states)];
	unsigned int samples = 0;
	unsigned int lane;
	unsigned int i;
	u32 first[4] = { 0, };
	u32 last[4] = { 0, };
	int ret;

	memset(counts, 0, sizeof(counts));

	for (i = 0; i < 1000; ++i) {
		u32 values[4];

		/*
		 * Read the state of all lanes and skip read errors and invalid
		 * values.
		 */
		for (lane = 0; lane < 4; ++lane) {
			ret = ar0830ap1302d2l_read(sensor->ar0830ap1302d2l,
					  AR0830AP1302D2L_ADV_SINF_MIPI_INTERNAL_p_LANE_n_STAT(index, lane),
					  &values[lane]);
			if (ret < 0)
				break;

			if (AR0830AP1302D2L_LANE_STATE(values[lane]) >=
			    ARRAY_SIZE(ar0830ap1302d2l_lane_states)) {
				ret = -EINVAL;
				break;
			}
		}

		if (ret < 0)
			continue;

		/* Accumulate the samples and save the first and last states. */
		for (lane = 0; lane < 4; ++lane)
			counts[lane][AR0830AP1302D2L_LANE_STATE(values[lane])]++;

		if (!samples)
			memcpy(first, values, sizeof(first));
		memcpy(last, values, sizeof(last));

		samples++;
	}

	if (!samples)
		return;

	/*
	 * Print the LP state from the first sample, the error state from the
	 * last sample, and the states accumulators for each lane.
	 */
	for (lane = 0; lane < 4; ++lane) {
		u32 state = last[lane];
		char error_msg[25] = "";

		if (state & (AR0830AP1302D2L_LANE_ERR | AR0830AP1302D2L_LANE_ABORT)) {
			unsigned int err = AR0830AP1302D2L_LANE_ERR_STATE(state);
			const char *err_state = NULL;

			err_state = err < ARRAY_SIZE(ar0830ap1302d2l_lane_states)
				  ? ar0830ap1302d2l_lane_states[err] : "INVALID";

			snprintf(error_msg, sizeof(error_msg), "ERR (%s%s) %s LP%s",
				 state & AR0830AP1302D2L_LANE_ERR ? "E" : "",
				 state & AR0830AP1302D2L_LANE_ABORT ? "A" : "",
				 err_state,
				 lp_states[AR0830AP1302D2L_LANE_ERR_LP_VAL(state)]);
		}

		dev_dbg(sensor->ar0830ap1302d2l->dev, "SINF%u L%u state: LP%s %s",
			 index, lane, lp_states[AR0830AP1302D2L_LANE_LP_VAL(first[lane])],
			 error_msg);

		for (i = 0; i < ARRAY_SIZE(ar0830ap1302d2l_lane_states); ++i) {
			if (counts[lane][i])
				dev_dbg(sensor->ar0830ap1302d2l->dev, " %s:%u",
				       ar0830ap1302d2l_lane_states[i],
				       counts[lane][i]);
		}
		dev_dbg(sensor->ar0830ap1302d2l->dev, "\n");
	}

	/* Reset the error flags. */
	for (lane = 0; lane < 4; ++lane)
		ar0830ap1302d2l_write(sensor->ar0830ap1302d2l,
			     AR0830AP1302D2L_ADV_SINF_MIPI_INTERNAL_p_LANE_n_STAT(index, lane),
			     AR0830AP1302D2L_LANE_ERR | AR0830AP1302D2L_LANE_ABORT, NULL);
}
// Add for AR0830AP1302D2L
int ar0830ap1302d2l_stream(struct ar0830ap1302d2l_device *ar0830ap1302d2l, int enable)
{
	int ret;

	// mutex_lock(&ar0830ap1302d2l->lock);

	if (enable) {
		ret = ar0830ap1302d2l_configure(ar0830ap1302d2l);
		if (ret < 0)
			goto done;

		ret = ar0830ap1302d2l_stall(ar0830ap1302d2l, false);
	} else {
		ret = ar0830ap1302d2l_stall(ar0830ap1302d2l, true);
	}

done:
	// mutex_unlock(&ar0830ap1302d2l->lock);

	if (ret < 0)
		dev_err(ar0830ap1302d2l->dev, "Failed to %s stream: %d\n",
			enable ? "start" : "stop", ret);

	return ret;
}
int ar0830ap1302d2l_reset(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	unsigned int stat, ret;
	struct v4l2_subdev *sd = i2c_get_clientdata(ar0830ap1302d2l->client);
	struct adaptor_ctx *ad_ctx = container_of(sd, struct adaptor_ctx, sd);

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START, 0, NULL);
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SYS_START, &stat);
	dev_dbg(ar0830ap1302d2l->dev, "do ar0830ap1302d2l reset , SYS_START = 0x%x", stat);
	ret = adaptor_hw_power_off(ad_ctx);
	dev_dbg(ar0830ap1302d2l->dev, "ar0830ap1302d2l: hw_power_off return 0x%x", ret);
	msleep(200);
	ret = adaptor_hw_power_on(ad_ctx);
	dev_dbg(ar0830ap1302d2l->dev, "ar0830ap1302d2l: hw_power_on return 0x%x", ret);
	msleep(200);
	return 0;
}
int ar0830ap1302d2l_config_mode(struct ar0830ap1302d2l_device *ar0830ap1302d2l, struct ar0830ap1302d2l_mode_info *info)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(ar0830ap1302d2l->client);
	struct adaptor_ctx *ad_ctx = container_of(sd, struct adaptor_ctx, sd);
	int ret = 0;
	u32 value;

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_PREVIEW_WIDTH, info->width, &ret);
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_PREVIEW_HEIGHT, info->height, &ret);
	dev_dbg(ar0830ap1302d2l->dev, "Config ar0830ap1302d2l size = %u x %u", info->width, info->height);

	value = 10000000 / info->target_fps;
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_REG_32BIT(0x2024), value, &ret);
	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_REG_32BIT(0x2028), value, &ret);
	dev_dbg(ar0830ap1302d2l->dev, "Config max expo time = %u (0x%x)", value, value);

	ar0830ap1302d2l_write(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x6124), 1, &ret);

	if (ret < 0)
		return ret;

	return 0;
}

int ar0830ap1302d2l_log_status(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	//struct ar0830ap1302d2l_device *ar0830ap1302d2l = to_ar0830ap1302d2l(sd);
	u16 frame_count_in;
	u16 frame_count_out;
	u32 warning[4];
	u32 error[3];
	unsigned int i;
	u32 value;
	int ret;

	/* Dump the console buffer. */
	ret = ar0830ap1302d2l_dump_console(ar0830ap1302d2l);
	if (ret < 0)
		return ret;

	/* Print errors. */
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_ERROR, &error[0]);
	if (ret < 0)
		return ret;

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_ERR_FILE, &error[1]);
	if (ret < 0)
		return ret;

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_ERR_LINE, &error[2]);
	if (ret < 0)
		return ret;

	dev_info(ar0830ap1302d2l->dev, "ERROR: 0x%04x (file 0x%08x:%u)\n",
		 error[0], error[1], error[2]);

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SIPM_ERR_0, &error[0]);
	if (ret < 0)
		return ret;

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_SIPM_ERR_1, &error[1]);
	if (ret < 0)
		return ret;

	dev_info(ar0830ap1302d2l->dev, "SIPM_ERR [0] 0x%04x [1] 0x%04x\n",
		 error[0], error[1]);

	/* Print warnings. */
	for (i = 0; i < ARRAY_SIZE(warning); ++i) {
		ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_WARNING(i), &warning[i]);
		if (ret < 0)
			return ret;
	}

	dev_info(ar0830ap1302d2l->dev,
		 "WARNING [0] 0x%04x [1] 0x%04x [2] 0x%04x [3] 0x%04x\n",
		 warning[0], warning[1], warning[2], warning[3]);

	for (i = 0; i < ARRAY_SIZE(ar0830ap1302d2l_warnings); ++i) {
		if ((warning[i / 16] & BIT(i % 16)) &&
		    ar0830ap1302d2l_warnings[i])
			dev_info(ar0830ap1302d2l->dev, "- WARN_%s\n",
				 ar0830ap1302d2l_warnings[i]);
	}

	/* Print the frame counter. */
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_FRAME_CNT, &value);
	if (ret < 0)
		return ret;

	frame_count_out = value;

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_ADV_CAPTURE_A_FV_CNT, &value);
	if (ret < 0)
		return ret;

	frame_count_in = value & 0xffff;

	dev_info(ar0830ap1302d2l->dev, "Frame counters: IN %u, OUT %u\n",
		 frame_count_in, frame_count_out);


	/* Sample the lane state. */
	for (i = 0; i < ARRAY_SIZE(ar0830ap1302d2l->sensors); ++i) {
		struct ar0830ap1302d2l_sensor *sensor = &ar0830ap1302d2l->sensors[i];

		if (!sensor->ar0830ap1302d2l)
			continue;

		ar0830ap1302d2l_log_lane_state(sensor, i);
	}

#if ENABLE_DEV_DEBUG_REG
	/* Add debug log */
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x2030), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x2030: 0x%04x\n", value);

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x302e), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x302e: 0x%04x\n", value);

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x4030), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x4030: 0x%04x\n", value);

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x2000), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x2000: 0x%04x\n", value);

	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x2002), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x2002: 0x%04x\n", value);

	// mipi_hs_exit
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x201a), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x0x201a: 0x%04x\n", value);
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x301a), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x0x301a: 0x%04x\n", value);
	ret = ar0830ap1302d2l_read(ar0830ap1302d2l, AR0830AP1302D2L_REG_16BIT(0x401a), &value);
	if (ret < 0)
		return ret;
	dev_info(ar0830ap1302d2l->dev, "R0x0x401a: 0x%04x\n", value);
#endif
	return 0;
}
int ar0830ap1302d2l_sensor_init(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(ar0830ap1302d2l->sensors); ++i) {
		struct ar0830ap1302d2l_sensor *sensor = &ar0830ap1302d2l->sensors[i];

		sensor->ar0830ap1302d2l = ar0830ap1302d2l;
	}
#if ENABLE_DEBUGFS
	ar0830ap1302d2l_debugfs_init(ar0830ap1302d2l);
#endif
	return 0;
}
int ar0830ap1302d2l_remove(struct ar0830ap1302d2l_device *ar0830ap1302d2l)
{
#if ENABLE_DEBUGFS
	ar0830ap1302d2l_debugfs_cleanup(ar0830ap1302d2l);
#endif
	return 0;
}
