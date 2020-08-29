/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2007-2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  @(#) $Id: zynqmp_r5.h 780 2017-03-07 01:32:17Z ertl-honda $
 */

#ifndef TOPPERS_ZYNQMP_R5_H
#define TOPPERS_ZYNQMP_R5_H

#include "core_sil.h"

/*
 *  コア数
 */
#define TNUM_HWCORE				UINT_C(2)           /* コア数 */

/*
 *  メモリマップ
 */
#define DDR_TEXT_START		TEXT_START_ADDRESS
#define DDR_TEXT_SIZE		RSAE_8M
#define DDR_DATA_START		DATA_START_ADDRESS
#define DDR_DATA_SIZE		RSAE_8M

/*
 *  タイマ関連
 */
#define TTC2_BASE		UINT_C(0xFF130000)
#define TTC3_BASE		UINT_C(0xFF140000)
#define TTC_FREQUENCY_HZ	UINT_C(100000000)

#define RST_LPD_IOU2 					0xFF5E0238U
#define RST_LPD_IOU2_TTC2_RESET_MASK	0x00002000U
#define RST_LPD_IOU2_TTC3_RESET_MASK	0x00004000U

#define TTC_CLK_CNTRL_OFFSET 		0x00000000U
#define TTC_CNTR_CNTRL_OFFSET		0x0000000CU
#define TTC_CNTR_VAL_OFFSET			0x00000018U
#define TTC_INTER_CNTR_OFFSET		0x00000024U
#define TTC_INT_MATCH1_OFFSET		0x00000030U
#define TTC_INT_REG_OFFSET			0x00000054U
#define TTC_INT_ENA_OFFSET			0x00000060U

#define TTC_CNTR_CNTRL_DIS_MASK    0x00000001U
#define TTC_CNTR_CNTRL_INT_MASK    0x00000002U
#define TTC_CNTR_CNTRL_MATCH_MASK  0x00000008U
#define TTC_CNTR_CNTRL_RST_MASK    0x00000010U
#define TTC_CLK_CNTRL_PS_EN_MASK   0x00000001U
#define TTC_INT_REG_IV             0x00000001U
#define TTC_INT_REG_M1             0x00000002U

/*
 *  GIC関連
 */
#define GICC_BASE		UINT_C(0xF9001000)
#define GICD_BASE		UINT_C(0xF9000000)
#define GIC_TNUM_INTNO	UINT_C(156+32)

#define TNUM_INTPRI     32

#define IRQ_XUART0		UINT_C(21 + 32)
#define IRQ_XUART1		UINT_C(22 + 32)
#define IRQ_TCC2_0		UINT_C(42 + 32)
#define IRQ_TCC2_1		UINT_C(43 + 32) 
#define IRQ_TCC3_0		UINT_C(45 + 32)
#define IRQ_TCC3_1		UINT_C(46 + 32) 

#define ICINO_DIS		UINT_C(0)
#define ICINO_EXT		UINT_C(1)

#define XUART0_BASE		0xFF000000U
#define XUART1_BASE		0xFF010000U


/*
 *  各種周辺回路
 */
#define RPU_BASEADDR		0xFF9A0000U

#define RPU_RPU_GLBL_CNTL				((RPU_BASEADDR) + 0X00000000U)
#define RPU_RPU_GLBL_CNTL_SLSPLIT_MASK	0X00000008U
#define RPU_RPU_GLBL_CNTL_TCM_COMB_MASK	0X00000040U
#define RPU_RPU_GLBL_CNTL_SLCLAMP_MASK	0X00000010U

#define RPU_RPU_1_CFG					((RPU_BASEADDR) + 0X00000200U)
#define RPU_RPU_1_CFG_NCPUHALT_MASK		0X00000001U
#define RPU_RPU_1_CFG_VINITHI_MASK		0X00000004U

#define RPU_RPU_ERR_INJ					((RPU_BASEADDR) + 0X00000020U)
#define RPU_RPU_ERR_INJ_FAULTLOGENABLE	0x101

#define CRL_APB_BASEADDR				0XFF5E0000U

#define CRL_APB_CPU_R5_CTRL				((CRL_APB_BASEADDR) + 0X00000090U)
#define CRL_APB_CPU_R5_CTRL_CLKACT_MASK	0X01000000U

#define CRL_APB_RST_LPD_TOP				((CRL_APB_BASEADDR)+ 0X0000023CU)
#define CRL_APB_RST_LPD_TOP_RPU_R50_RESET_MASK	0X00000001U
#define CRL_APB_RST_LPD_TOP_RPU_AMBA_RESET_MASK	0X00000004U
#define CRL_APB_RST_LPD_TOP_RPU_R51_RESET_MASK	0X00000002U

#define CRL_APB_RST_LPD_DBG						((CRL_APB_BASEADDR) + 0X00000240U)
#define CRL_APB_RST_LPD_DBG_DBG_LPD_RESET		(1U << 1U)
#define CRL_APB_RST_LPD_DBG_RPU_DBG0_RESET		(1U << 4U)
#define CRL_APB_RST_LPD_DBG_RPU_DBG1_RESET		(1U << 5U)

#define XPAR_CSU_BASEADDR		0xFFCA0000U
#define XPAR_CSU_VER_OFFSET		0x00000044U

#define XPLAT_ZYNQ_ULTRA_MP_SILICON	0x0
#define XPLAT_ZYNQ_ULTRA_MP			0x1
#define XPLAT_ZYNQ_ULTRA_MPVEL		0x2
#define XPLAT_ZYNQ_ULTRA_MPQEMU		0x3
#define XPLAT_ZYNQ					0x4
#define XPLAT_MICROBLAZE			0x5
#define XPLAT_INFO_MASK				(0xF)

#define PSU_R5_1_ATCM_S_AXI_BASEADDR	0xFFE90000


#endif /* TOPPERS_ZYNQMP_R5_H */
