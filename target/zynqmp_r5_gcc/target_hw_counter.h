/*
 *  TOPPERS ATK2
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Automotive Kernel Version 2
 *
 *  Copyright (C) 2016 by Center for Embedded Computing Systems
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
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
 *  本ソフトウェアは，AUTOSAR（AUTomotive Open System ARchitecture）仕
 *  様に基づいている．上記の許諾は，AUTOSARの知的財産権を許諾するもので
 *  はない．AUTOSARは，AUTOSAR仕様に基づいたソフトウェアを商用目的で利
 *  用する者に対して，AUTOSARパートナーになることを求めている．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  $Id: target_hw_counter.h 780 2017-03-07 01:32:17Z ertl-honda $
 */

/*
 *		テストプログラムのターゲット依存定義（NIOS2_DEV用）
 */

#ifndef TOPPERS_TARGET_HW_COUNTER_H
#define TOPPERS_TARGET_HW_COUNTER_H

#include "zynqmp_r5.h"

/*
 *  使用するタイマーのユニット番号
 */
#define HWC_BASE_CORE0 TTC2_BASE
#define HWC_BASE_CORE1 TTC3_BASE

#define HWC_RESET_MASK_CORE0 RST_LPD_IOU2_TTC2_RESET_MASK
#define HWC_RESET_MASK_CORE1 RST_LPD_IOU2_TTC3_RESET_MASK

/*
 *  使用するタイマーの差分タイマと現在値タイマのチャネル
 */
#define HWC_DTIM_CORE0_ID	0
#define HWC_CTIM_CORE0_ID	1

#define HWC_DTIM_CORE1_ID	0
#define HWC_CTIM_CORE1_ID	1

/*
 *  割込み番号
 */
#define HWC_DTIM_CORE0_INTNO	(0xffff0000 | (IRQ_TCC2_0 + HWC_DTIM_CORE0_ID))
#define HWC_DTIM_CORE1_INTNO	(0xffff0000 | (IRQ_TCC3_0 + HWC_DTIM_CORE1_ID))

/*
 *  割込み優先度
 */
#define HWC_DTIM_CORE0_INTPRI	1
#define HWC_DTIM_CORE1_INTPRI	1

/*
 *  カウンタ最大値
 */
#define MAX_TIMER_CNT		((uint32) 0xFFFFFFFF)

/*
 *  カウンタ周期最大値
 */
#define MAX_CNT_CYCLE		((uint32) 0x7FFFFFFF)

/*
 *  タイマクロック周波数（Hz）
 */
#define TIMER_CLOCK_HZ		TTC_FREQUENCY_HZ

/*
 *  NIOS2_DEV_DE2_115 用の定義割込みBASE ADDRESS
 */
#define INTNO_BASE_CORE0	UINT_C(0x10000)
#define INTNO_BASE_CORE1	UINT_C(0x20000)

/*
 *  10msと一致するティック値(サンプルプログラム用)
 */
#define TICK_FOR_10MS	(TIMER_CLOCK_HZ / 100)

#endif /* TOPPERS_TARGET_HW_COUNTER_H */
