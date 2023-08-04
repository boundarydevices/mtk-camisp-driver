/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 MediaTek Inc.
 *
 * Author: Daniel Huang <daniel.huang@mediatek.com>
 *
 */

#ifndef _MTK_IMGSYS_CMDQ_PLAT_H_
#define _MTK_IMGSYS_CMDQ_PLAT_H_

#define IMGSYS_DVFS_ENABLE     (1)
#define IMGSYS_QOS_ENABLE      (1)
#define IMGSYS_QOS_SET_REAL    (0)
#define IMGSYS_QOS_SET_BY_SCEN (1)

#define IMGSYS_SECURE_ENABLE   (0)

/* Record info definitions */
#define GCE_REC_MAX_FRAME_BLOCK     (40)
#define GCE_REC_MAX_TILE_BLOCK      (40)
#define GCE_REC_MAX_LABEL_COUNT     (2048)

#define IMGSYS_ENG_MAX 10
#define IMGSYS_REG_START	0x15000000
#define IMGSYS_REG_END		0x1570FFFF

#define IMGSYS_CMD_MAX_SZ_N	0x9000
#define IMGSYS_CMD_MAX_SZ_V	0x50000

#define IMGSYS_QOS_MAX 56
#define IMGSYS_SEC_THD 2
#define IMGSYS_MAX_FPS 60

#define IMGSYS_VSS_FREQ_FLOOR	660000000
#define IMGSYS_SMVR_FREQ_FLOOR	393000000

#define IMGSYS_CMDQ_HW_EVENT_BEGIN	0
#define IMGSYS_CMDQ_HW_EVENT_END	(IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_START - 1)
#define IMGSYS_CMDQ_SW_EVENT_BEGIN	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_START
#define IMGSYS_CMDQ_SW_EVENT_END	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_END
#define IMGSYS_CMDQ_GPR_EVENT_BEGIN	999
#define IMGSYS_CMDQ_GPR_EVENT_END	1008
#define IMGSYS_CMDQ_SYNC_POOL_NUM	60

#define WPE_SMI_PORT_NUM	5
#define ME_SMI_PORT_NUM	2
#define PQ_DIP_SMI_PORT_NUM	4
#define TRAW_SMI_PORT_NUM	13
#define LTRAW_SMI_PORT_NUM	7
#define DIP_SMI_PORT_NUM	16

#define IMGSYS_QOS_UPDATE_FREQ	1000
#define IMGSYS_QOS_BLANK_INT	100
#define IMGSYS_QOS_FACTOR		13
#define IMGSYS_QOS_FHD_SIZE		(1920*1080/2)
#define IMGSYS_QOS_4K_SIZE		(4000*2000/2)
#define IMGSYS_QOS_FHD_30_BW_0	3320 //1328
#define IMGSYS_QOS_FHD_30_BW_1	2262 //905
#define IMGSYS_QOS_FHD_60_BW_0	6145 //2458
#define IMGSYS_QOS_FHD_60_BW_1	3387 //1355
#define IMGSYS_QOS_4K_30_BW_0	5875 //2350
#define IMGSYS_QOS_4K_30_BW_1	3145 //1258
#define IMGSYS_QOS_4K_60_BW_0	5610 //2244
#define IMGSYS_QOS_4K_60_BW_1	5910 //2360

enum mtk_imgsys_event {
	/* HW event */
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_0 = 0,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_1,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_2,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_3,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_4,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_5,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_6,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_7,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_8,
	IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_9,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_0,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_1,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_2,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_3,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_4,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_5,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_6,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_7,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_8,
	IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_9,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_0,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_1,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_2,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_3,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_4,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_5,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_6,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_7,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_8,
	IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_9,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_0,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_1,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_2,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_3,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_4,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_5,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_6,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_7,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_8,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_9,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_0,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_1,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_2,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_3,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_4,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_5,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_6,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_7,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_8,
	IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_9,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_0,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_1,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_2,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_3,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_4,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_5,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_6,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_7,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_8,
	IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_9,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_0,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_1,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_2,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_3,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_4,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_5,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_6,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_7,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_8,
	IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_9,
	IMGSYS_CMDQ_EVENT_IMG_IMGSYS_IPE_ME_DONE,
	/* SW event */
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_START,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_WPE_EIS = IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_START,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_WPE_TNR,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_TRAW,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_LTRAW,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_DIP,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_PQDIP_A,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_PQDIP_B,
	IMGSYS_CMDQ_SYNC_TOKEN_IPESYS_ME,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_VSS_TRAW,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_VSS_LTRAW,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_VSS_DIP,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_START,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_1 = IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_START,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_2,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_3,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_4,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_5,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_6,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_7,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_8,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_9,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_10,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_11,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_12,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_13,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_14,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_15,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_16,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_17,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_18,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_19,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_20,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_21,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_22,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_23,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_24,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_25,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_26,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_27,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_28,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_29,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_30,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_31,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_32,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_33,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_34,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_35,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_36,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_37,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_38,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_39,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_40,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_41,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_42,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_43,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_44,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_45,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_46,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_47,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_48,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_49,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_50,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_51,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_52,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_53,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_54,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_55,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_56,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_57,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_58,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_59,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_60,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_END = IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_60,
#ifdef support_event /* 0 */
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_61,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_62,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_63,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_64,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_65,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_66,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_67,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_68,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_69,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_70,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_71,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_72,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_73,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_74,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_75,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_76,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_77,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_78,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_79,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_80,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_81,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_82,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_83,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_84,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_85,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_86,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_87,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_88,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_89,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_90,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_91,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_92,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_93,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_94,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_95,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_96,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_97,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_98,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_99,
	IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_100,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_1,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_2,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_3,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_4,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_5,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_6,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_7,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_8,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_9,
	IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_10,
#endif
	IMGSYS_CMDQ_SYNC_TOKEN_TZMP_ISP_WAIT,
	IMGSYS_CMDQ_SYNC_TOKEN_TZMP_ISP_SET,
	IMGSYS_CMDQ_EVENT_MAX
};

static struct imgsys_event_table imgsys_event[] = {
	/*TRAW frame done*/
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_0, "traw_cq_thread0_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_1, "traw_cq_thread1_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_2, "traw_cq_thread2_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_3, "traw_cq_thread3_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_4, "traw_cq_thread4_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_5, "traw_cq_thread5_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_6, "traw_cq_thread6_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_7, "traw_cq_thread7_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_8, "traw_cq_thread8_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW0_CQ_THR_DONE_TRAW0_9, "traw_cq_thread9_frame_done"},
	/*LTRAW frame done*/
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_0, "ltraw_cq_thread0_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_1, "ltraw_cq_thread1_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_2, "ltraw_cq_thread2_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_3, "ltraw_cq_thread3_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_4, "ltraw_cq_thread4_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_5, "ltraw_cq_thread5_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_6, "ltraw_cq_thread6_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_7, "ltraw_cq_thread7_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_8, "ltraw_cq_thread8_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_TRAW1_CQ_THR_DONE_TRAW0_9, "ltraw_cq_thread9_frame_done"},
	/*DIP frame done*/
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_0, "dip_cq_thread0_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_1, "dip_cq_thread1_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_2, "dip_cq_thread2_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_3, "dip_cq_thread3_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_4, "dip_cq_thread4_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_5, "dip_cq_thread5_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_6, "dip_cq_thread6_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_7, "dip_cq_thread7_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_8, "dip_cq_thread8_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_DIP_CQ_THR_DONE_P2_9, "dip_cq_thread9_frame_done"},
	/*PQ_DIP_A frame done*/
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_0, "pqa_cq_thread0_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_1, "pqa_cq_thread1_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_2, "pqa_cq_thread2_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_3, "pqa_cq_thread3_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_4, "pqa_cq_thread4_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_5, "pqa_cq_thread5_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_6, "pqa_cq_thread6_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_7, "pqa_cq_thread7_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_8, "pqa_cq_thread8_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_A_CQ_THR_DONE_P2_9, "pqa_cq_thread9_frame_done"},
	/*PQ_DIP_B frame done*/
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_0, "pqb_cq_thread0_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_1, "pqb_cq_thread1_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_2, "pqb_cq_thread2_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_3, "pqb_cq_thread3_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_4, "pqb_cq_thread4_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_5, "pqb_cq_thread5_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_6, "pqb_cq_thread6_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_7, "pqb_cq_thread7_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_8, "pqb_cq_thread8_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_PQDIP_B_CQ_THR_DONE_P2_9, "pqb_cq_thread9_frame_done"},
	/*WPE_EIS frame done*/
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_0, "wpe_eis_cq_thread0_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_1, "wpe_eis_cq_thread1_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_2, "wpe_eis_cq_thread2_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_3, "wpe_eis_cq_thread3_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_4, "wpe_eis_cq_thread4_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_5, "wpe_eis_cq_thread5_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_6, "wpe_eis_cq_thread6_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_7, "wpe_eis_cq_thread7_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_8, "wpe_eis_cq_thread8_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_EIS_CQ_THR_DONE_P2_9, "wpe_eis_cq_thread9_frame_done"},
	/*WPE_TNR frame done*/
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_0, "wpe_tnr_cq_thread0_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_1, "wpe_tnr_cq_thread1_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_2, "wpe_tnr_cq_thread2_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_3, "wpe_tnr_cq_thread3_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_4, "wpe_tnr_cq_thread4_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_5, "wpe_tnr_cq_thread5_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_6, "wpe_tnr_cq_thread6_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_7, "wpe_tnr_cq_thread7_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_8, "wpe_tnr_cq_thread8_frame_done"},
	{IMGSYS_CMDQ_EVENT_IMG_WPE_TNR_CQ_THR_DONE_P2_9, "wpe_tnr_cq_thread9_frame_done"},
	/*ME done*/
	{IMGSYS_CMDQ_EVENT_IMG_IMGSYS_IPE_ME_DONE, "me_done"},
	/* SW event */
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_WPE_EIS, "wpe_eis_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_WPE_TNR, "wpe_tnr_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_TRAW, "traw_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_LTRAW, "ltraw_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_DIP, "dip_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_PQDIP_A, "pqdip_a_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_PQDIP_B, "pqdip_b_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IPESYS_ME, "me_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_VSS_TRAW, "vss_traw_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_VSS_LTRAW, "vss_ltraw_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_VSS_DIP, "vss_dip_sync_token"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_1, "sw_sync_token_pool_1"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_2, "sw_sync_token_pool_2"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_3, "sw_sync_token_pool_3"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_4, "sw_sync_token_pool_4"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_5, "sw_sync_token_pool_5"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_6, "sw_sync_token_pool_6"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_7, "sw_sync_token_pool_7"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_8, "sw_sync_token_pool_8"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_9, "sw_sync_token_pool_9"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_10, "sw_sync_token_pool_10"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_11, "sw_sync_token_pool_11"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_12, "sw_sync_token_pool_12"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_13, "sw_sync_token_pool_13"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_14, "sw_sync_token_pool_14"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_15, "sw_sync_token_pool_15"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_16, "sw_sync_token_pool_16"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_17, "sw_sync_token_pool_17"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_18, "sw_sync_token_pool_18"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_19, "sw_sync_token_pool_19"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_20, "sw_sync_token_pool_20"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_21, "sw_sync_token_pool_21"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_22, "sw_sync_token_pool_22"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_23, "sw_sync_token_pool_23"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_24, "sw_sync_token_pool_24"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_25, "sw_sync_token_pool_25"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_26, "sw_sync_token_pool_26"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_27, "sw_sync_token_pool_27"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_28, "sw_sync_token_pool_28"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_29, "sw_sync_token_pool_29"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_30, "sw_sync_token_pool_30"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_31, "sw_sync_token_pool_31"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_32, "sw_sync_token_pool_32"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_33, "sw_sync_token_pool_33"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_34, "sw_sync_token_pool_34"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_35, "sw_sync_token_pool_35"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_36, "sw_sync_token_pool_36"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_37, "sw_sync_token_pool_37"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_38, "sw_sync_token_pool_38"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_39, "sw_sync_token_pool_39"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_40, "sw_sync_token_pool_40"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_41, "sw_sync_token_pool_41"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_42, "sw_sync_token_pool_42"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_43, "sw_sync_token_pool_43"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_44, "sw_sync_token_pool_44"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_45, "sw_sync_token_pool_45"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_46, "sw_sync_token_pool_46"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_47, "sw_sync_token_pool_47"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_48, "sw_sync_token_pool_48"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_49, "sw_sync_token_pool_49"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_50, "sw_sync_token_pool_50"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_51, "sw_sync_token_pool_51"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_52, "sw_sync_token_pool_52"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_53, "sw_sync_token_pool_53"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_54, "sw_sync_token_pool_54"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_55, "sw_sync_token_pool_55"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_56, "sw_sync_token_pool_56"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_57, "sw_sync_token_pool_57"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_58, "sw_sync_token_pool_58"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_59, "sw_sync_token_pool_59"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_60, "sw_sync_token_pool_60"},
#ifdef support_event /* 0 */
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_61, "sw_sync_token_pool_61"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_62, "sw_sync_token_pool_62"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_63, "sw_sync_token_pool_63"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_64, "sw_sync_token_pool_64"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_65, "sw_sync_token_pool_65"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_66, "sw_sync_token_pool_66"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_67, "sw_sync_token_pool_67"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_68, "sw_sync_token_pool_68"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_69, "sw_sync_token_pool_69"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_70, "sw_sync_token_pool_70"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_71, "sw_sync_token_pool_71"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_72, "sw_sync_token_pool_72"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_73, "sw_sync_token_pool_73"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_74, "sw_sync_token_pool_74"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_75, "sw_sync_token_pool_75"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_76, "sw_sync_token_pool_76"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_77, "sw_sync_token_pool_77"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_78, "sw_sync_token_pool_78"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_79, "sw_sync_token_pool_79"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_80, "sw_sync_token_pool_80"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_81, "sw_sync_token_pool_81"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_82, "sw_sync_token_pool_82"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_83, "sw_sync_token_pool_83"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_84, "sw_sync_token_pool_84"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_85, "sw_sync_token_pool_85"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_86, "sw_sync_token_pool_86"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_87, "sw_sync_token_pool_87"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_88, "sw_sync_token_pool_88"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_89, "sw_sync_token_pool_89"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_90, "sw_sync_token_pool_90"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_91, "sw_sync_token_pool_91"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_92, "sw_sync_token_pool_92"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_93, "sw_sync_token_pool_93"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_94, "sw_sync_token_pool_94"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_95, "sw_sync_token_pool_95"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_96, "sw_sync_token_pool_96"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_97, "sw_sync_token_pool_97"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_98, "sw_sync_token_pool_98"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_99, "sw_sync_token_pool_99"},
	{IMGSYS_CMDQ_SYNC_TOKEN_IMGSYS_POOL_100, "sw_sync_token_pool_100"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_1, "sw_sync_token_camsys_pool_1"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_2, "sw_sync_token_camsys_pool_2"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_3, "sw_sync_token_camsys_pool_3"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_4, "sw_sync_token_camsys_pool_4"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_5, "sw_sync_token_camsys_pool_5"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_6, "sw_sync_token_camsys_pool_6"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_7, "sw_sync_token_camsys_pool_7"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_8, "sw_sync_token_camsys_pool_8"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_9, "sw_sync_token_camsys_pool_9"},
	{IMGSYS_CMDQ_SYNC_TOKEN_CAMSYS_POOL_10, "sw_sync_token_camsys_pool_10"},
#endif
	{IMGSYS_CMDQ_SYNC_TOKEN_TZMP_ISP_WAIT, "sw_sync_token_tzmp_isp_wait"},
	{IMGSYS_CMDQ_SYNC_TOKEN_TZMP_ISP_SET, "sw_sync_token_tzmp_isp_set"},
	{IMGSYS_CMDQ_EVENT_MAX, "imgsys_event_max"},
};

static struct cmdq_client *imgsys_clt[IMGSYS_ENG_MAX];
#if IMGSYS_SECURE_ENABLE
static struct cmdq_client *imgsys_sec_clt[IMGSYS_SEC_THD];
#endif

enum mtk_imgsys_m4u_port {
	/* TRAW */
	IMGSYS_M4U_PORT_TRAW_START,
	IMGSYS_M4U_PORT_L9_IMG_IMGI_T1_A = IMGSYS_M4U_PORT_TRAW_START,
	IMGSYS_M4U_PORT_L9_IMG_IMGBI_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_IMGCI_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_SMTI_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_TNCSTI_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_TNCSTI_T4_A,
	IMGSYS_M4U_PORT_L9_IMG_YUVO_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_TIMGO_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_YUVO_T2_A,
	IMGSYS_M4U_PORT_L9_IMG_YUVO_T5_A,
	IMGSYS_M4U_PORT_L9_IMG_TNCSO_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_SMTO_T1_A,
	IMGSYS_M4U_PORT_L9_IMG_TNCSTO_T1_A,

	/* LTRAW */
	IMGSYS_M4U_PORT_LTRAW_START,
	IMGSYS_M4U_PORT_L9_IMG_IMGI_T1_B = IMGSYS_M4U_PORT_LTRAW_START,
	IMGSYS_M4U_PORT_L9_IMG_IMGBI_T1_B,
	IMGSYS_M4U_PORT_L9_IMG_IMGCI_T1_B,
	IMGSYS_M4U_PORT_L9_IMG_SMTI_T1_B,
	IMGSYS_M4U_PORT_L9_IMG_YUVO_T2_B,
	IMGSYS_M4U_PORT_L9_IMG_YUVO_T5_B,
	IMGSYS_M4U_PORT_L9_IMG_SMTO_T1_B,

	/* DIP */
	IMGSYS_M4U_PORT_DIP_START,
	IMGSYS_M4U_PORT_L10_IMG_IMGI_D1_A = IMGSYS_M4U_PORT_DIP_START,
	IMGSYS_M4U_PORT_L10_IMG_IMGCI_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_DEPI_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_DMGI_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_VIPI_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_TNRWI_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_RECI_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_SMTI_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_SMTI_D6_A,
	IMGSYS_M4U_PORT_L10_IMG_IMG3O_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_IMG4O_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_IMG3CO_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_FEO_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_IMG2O_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_TNRWO_D1_A,
	IMGSYS_M4U_PORT_L10_IMG_SMTO_D1_A,

	/* PQDIP_A */
	IMGSYS_M4U_PORT_PQDIP_A_START,
	IMGSYS_M4U_PORT_L10_IMG_PIMGI_P1_A = IMGSYS_M4U_PORT_PQDIP_A_START,
	IMGSYS_M4U_PORT_L10_IMG_PIMGBI_P1_A,
	IMGSYS_M4U_PORT_L10_IMG_PIMGCI_P1_A,
	IMGSYS_M4U_PORT_L10_IMG_WROT_P1_A,

	/* PQDIP_B */
	IMGSYS_M4U_PORT_PQDIP_B_START,
	IMGSYS_M4U_PORT_L10_IMG_PIMGI_P1_B = IMGSYS_M4U_PORT_PQDIP_B_START,
	IMGSYS_M4U_PORT_L10_IMG_PIMGBI_P1_B,
	IMGSYS_M4U_PORT_L10_IMG_PIMGCI_P1_B,
	IMGSYS_M4U_PORT_L10_IMG_WROT_P1_B,

	/* WPE_EIS */
	IMGSYS_M4U_PORT_WPE_EIS_START,
	IMGSYS_M4U_PORT_L11_IMG_WPE_EIS_RDMA0_A = IMGSYS_M4U_PORT_WPE_EIS_START,
	IMGSYS_M4U_PORT_L11_IMG_WPE_EIS_RDMA1_A,
	IMGSYS_M4U_PORT_L11_IMG_WPE_EIS_WDMA0_A,
	IMGSYS_M4U_PORT_L11_IMG_WPE_EIS_CQ0_A,
	IMGSYS_M4U_PORT_L11_IMG_WPE_EIS_CQ1_A,

	/* WPE_TNR */
	IMGSYS_M4U_PORT_WPE_TNR_START,
	IMGSYS_M4U_PORT_L11_IMG_WPE_TNR_RDMA0_A = IMGSYS_M4U_PORT_WPE_TNR_START,
	IMGSYS_M4U_PORT_L11_IMG_WPE_TNR_RDMA1_A,
	IMGSYS_M4U_PORT_L11_IMG_WPE_TNR_WDMA0_A,
	IMGSYS_M4U_PORT_L11_IMG_WPE_TNR_CQ0_A,
	IMGSYS_M4U_PORT_L11_IMG_WPE_TNR_CQ1_A,

	/* ME */
	IMGSYS_M4U_PORT_ME_START,
	IMGSYS_M4U_PORT_L12_IMG_ME_RDMA = IMGSYS_M4U_PORT_ME_START,
	IMGSYS_M4U_PORT_L12_IMG_ME_WDMA,

	/* SMI COMMON */
	IMGSYS_L9_COMMON_0,
	IMGSYS_L12_COMMON_1,
	IMGSYS_M4U_PORT_MAX
};

#if DVFS_QOS_READY
static struct mtk_imgsys_qos_path imgsys_qos_path[IMGSYS_M4U_PORT_MAX] = {
	{NULL, "l9_imgi_t1_a", 0},
	{NULL, "l9_imgbi_t1_a", 0},
	{NULL, "l9_imgci_t1_a", 0},
	{NULL, "l9_smti_t1_a", 0},
	{NULL, "l9_tncsti_t1_a", 0},
	{NULL, "l9_tncsti_t4_a", 0},
	{NULL, "l9_yuvo_t1_a", 0},
	{NULL, "l9_timgo_t1_a", 0},
	{NULL, "l9_yuvo_t2_a", 0},
	{NULL, "l9_yuvo_t5_a", 0},
	{NULL, "l9_tncso_t1_a", 0},
	{NULL, "l9_smto_t1_a", 0},
	{NULL, "l9_tncsto_t1_a", 0},
	{NULL, "l9_imgi_t1_b", 0},
	{NULL, "l9_imgbi_t1_b", 0},
	{NULL, "l9_imgci_t1_b", 0},
	{NULL, "l9_smti_t1_b", 0},
	{NULL, "l9_yuvo_t2_b", 0},
	{NULL, "l9_yuvo_t5_b", 0},
	{NULL, "l9_smto_t1_b", 0},
	{NULL, "l10_imgi_d1_a", 0},
	{NULL, "l10_imgci_d1_a", 0},
	{NULL, "l10_depi_d1_a", 0},
	{NULL, "l10_dmgi_d1_a", 0},
	{NULL, "l10_vipi_d1_a", 0},
	{NULL, "l10_tnrwi_d1_a", 0},
	{NULL, "l10_reci_d1_a", 0},
	{NULL, "l10_smti_d1_a", 0},
	{NULL, "l10_smti_d6_a", 0},
	{NULL, "l10_img3o_d1_a", 0},
	{NULL, "l10_img4o_d1_a", 0},
	{NULL, "l10_img3co_d1_a", 0},
	{NULL, "l10_feo_d1_a", 0},
	{NULL, "l10_img2o_d1_a", 0},
	{NULL, "l10_tnrwo_d1_a", 0},
	{NULL, "l10_smto_d1_a", 0},
	{NULL, "l10_pimgi_p1_a", 0},
	{NULL, "l10_pimgbi_p1_a", 0},
	{NULL, "l10_pimgci_p1_a", 0},
	{NULL, "l10_wrot_p1_a", 0},
	{NULL, "l10_pimgi_p1_b", 0},
	{NULL, "l10_pimgbi_p1_b", 0},
	{NULL, "l10_pimgci_p1_b", 0},
	{NULL, "l10_wrot_p1_b", 0},
	{NULL, "l11_wpe_eis_rdma0_a", 0},
	{NULL, "l11_wpe_eis_rdma1_a", 0},
	{NULL, "l11_wpe_eis_wdma0_a", 0},
	{NULL, "l11_wpe_eis_cq0_a", 0},
	{NULL, "l11_wpe_eis_cq1_a", 0},
	{NULL, "l11_wpe_tnr_rdma0_a", 0},
	{NULL, "l11_wpe_tnr_rdma1_a", 0},
	{NULL, "l11_wpe_tnr_wdma0_a", 0},
	{NULL, "l11_wpe_tnr_cq0_a", 0},
	{NULL, "l11_wpe_tnr_cq1_a", 0},
	{NULL, "l12_me_rdma", 0},
	{NULL, "l12_me_wdma", 0},
	{NULL, "l9_common_0", 0},
	{NULL, "l12_common_1", 0}
};
#endif

struct BlockRecord {
	uint32_t            label_min;
	uint32_t            label_max;
	uint32_t            label_count;
	uint32_t            cmd_offset;
	uint32_t            cmd_length;
};

enum GCE_REC_BLOCK_ENUM {
	GCE_REC_NONE_BLOCK = -1,
	GCE_REC_FRAME_BLOCK = 0,
	GCE_REC_TILE_BLOCK
};

enum GCE_REC_MODE_ENUM {
	GCE_REC_APPEND_MODE = 0,
	GCE_REC_REPLACE_MODE
};

struct GCERecoder {
	uint32_t            header_code;

	// Record command offset
	uint32_t            cmd_offset;

	uint32_t            check_pre;

	// Reocrd command buffer info
	uint32_t            *pOutput;

	uint32_t            check_post;

	uint32_t            *pBuffer;
	uint32_t            max_length;
	uint32_t            curr_length;

	// Each frame block info
	struct BlockRecord  frame_record[GCE_REC_MAX_FRAME_BLOCK];
	uint32_t            frame_block;
	uint32_t            curr_frame;

	// Each tile block info
	struct BlockRecord  tile_record[GCE_REC_MAX_TILE_BLOCK];
	uint32_t            tile_block;
	uint32_t            curr_tile;

	// Record current block type
	enum GCE_REC_BLOCK_ENUM  curr_block;

	// Current mode setting
	enum GCE_REC_MODE_ENUM   curr_mode;

	// Append/Replace mode switch
	uint32_t            orig_index;
	uint32_t            *pOrig_out;
	uint32_t            curr_label;

	// Current label mode setting
	enum GCE_REC_BLOCK_ENUM  label_block;

	// Each frame command label
	uint32_t            frame_label[GCE_REC_MAX_LABEL_COUNT];
	uint32_t            frame_count;

	// Each tile command label
	uint32_t            tile_label[GCE_REC_MAX_LABEL_COUNT];
	uint32_t            tile_count;

	uint32_t            footer_code;
};

#if DVFS_QOS_READY
static struct imgsys_dvfs_group  dvfs_group[MTK_IMGSYS_DVFS_GROUP] = {
	{0, (IMGSYS_ENG_TRAW
			|IMGSYS_ENG_LTR
			|IMGSYS_ENG_ME)},
	{1, (IMGSYS_ENG_WPE_TNR
			|IMGSYS_ENG_DIP)},
	{2, (IMGSYS_ENG_WPE_EIS
			|IMGSYS_ENG_PQDIP_A
			|IMGSYS_ENG_PQDIP_B)}
};

static struct imgsys_dvfs_group  qos_group[MTK_IMGSYS_QOS_GROUP] = {
	{0, (IMGSYS_ENG_WPE_EIS
			|IMGSYS_ENG_TRAW
			|IMGSYS_ENG_LTR
			|IMGSYS_ENG_DIP
			|IMGSYS_ENG_PQDIP_A)},
	{1, (IMGSYS_ENG_WPE_TNR
			|IMGSYS_ENG_DIP
			|IMGSYS_ENG_PQDIP_B
			|IMGSYS_ENG_ME)}
};
#endif

struct smi_port_t {
	uint32_t portenum;
	uint32_t portbw;
} __attribute__((__packed__));

struct wpe_bw_t {
	uint32_t totalbw;
	struct smi_port_t smiport[WPE_SMI_PORT_NUM];
} __attribute__((__packed__));

struct me_bw_t {
	uint32_t totalbw;
	struct smi_port_t smiport[ME_SMI_PORT_NUM];
} __attribute__((__packed__));

struct pqdip_bw_t {
	uint32_t totalbw;
	struct smi_port_t smiport[PQ_DIP_SMI_PORT_NUM];
} __attribute__((__packed__));

struct traw_bw_t {
	uint32_t totalbw;
	struct smi_port_t smiport[TRAW_SMI_PORT_NUM];
} __attribute__((__packed__));

struct ltraw_bw_t {
	uint32_t totalbw;
	struct smi_port_t smiport[LTRAW_SMI_PORT_NUM];
} __attribute__((__packed__));

struct dip_bw_t {
	uint32_t totalbw;
	struct smi_port_t smiport[DIP_SMI_PORT_NUM];
} __attribute__((__packed__));

struct frame_bw_t {
	struct wpe_bw_t wpe_eis;
	struct wpe_bw_t wpe_tnr;
	struct me_bw_t me;
	struct pqdip_bw_t pqdip_a;
	struct pqdip_bw_t pqdip_b;
	struct traw_bw_t traw;
	struct ltraw_bw_t ltraw;
	struct dip_bw_t dip;
} __attribute__((__packed__));

#endif /* _MTK_IMGSYS_CMDQ_PLAT_H_ */

