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
 *  $Id: target_hw_counter.c 780 2017-03-07 01:32:17Z ertl-honda $
 */

/*
 *		以下ハードウェアカウンタプログラムのターゲット依存定義（NIOS2_DEV用）
 *
 *  使用するタイマ：
 *    差分タイマ：目的の時間を設定する時の現在時間(現在値タイマ)
 *              と次の満了時間との相対時間をカウントすることで
 *              目的の絶対時間に満了したこととする
 *              count mode:count down once
 *
 *    現在値タイマ：カウンタ周期分のベースタイマを実現
 *              (絶対時間をカウント)
 *              count mode:continuous count down
 *
 *    また上記のタイマは32bitのアップカウンタタイマである
 *
 *  制御方針：
 *
 *   現在値タイマはユーザ定義カウンタ最大値2倍+1までカウントし，
 *   周期タイマとして連続カウントダウンして，常に現在時刻を
 *   取得する．割込み発生する必要がないため，割込みなしモード
 *
 *   差分タイマは，満了処理を行うため，割込みありモードで動く
 *   アラームなどの満了点とタイマ1で示した現在時刻の差を
 *   現在値タイマに設定する
 *
 *  ポイント：
 *   満了処理は，現在時刻を影響しないため，現在値タイマを設けている
 *
 */
#include "Os.h"

#include "arm_gcc/common/core_sil.h"
#include "target_hw_counter.h"

/* 内部関数のプロトタイプ宣言 */
static TickType get_curr_time(uint32 coreid, TickType maxval);
static void init_hwcounter(uint32 coreid, TickType maxval, TimeType nspertick, TickRefType cycle);
static void start_hwcounter(uint32 coreid);
static void stop_hwcounter(uint32 coreid);
static void set_hwcounter(uint32 coreid, TickType exprtick, TickType maxval);
static TickType get_hwcounter(uint32 coreid, TickType maxval);
static void cancel_hwcounter(uint32 coreid);
static void trigger_hwcounter(uint32 coreid);
static void int_clear_hwcounter(uint32 coreid);
static void int_cancel_hwcounter(uint32 coreid);
static void increment_hwcounter(uint32 coreid);

const uint32_t hwc_dtim_base[] = {
	(HWC_BASE_CORE0 + (HWC_DTIM_CORE0_ID*4)),
	(HWC_BASE_CORE1 + (HWC_DTIM_CORE1_ID*4))
};

const uint32_t hwc_ctim_base[] = {
	(HWC_BASE_CORE0 + (HWC_CTIM_CORE0_ID*4)),
	(HWC_BASE_CORE1 + (HWC_CTIM_CORE1_ID*4))
};

const uint32_t hwc_reset_mask[] = {
	(HWC_RESET_MASK_CORE0),
	(HWC_RESET_MASK_CORE1)
};

/*
 *  ハードウェアカウンタ現在ティック値取得
 */
static TickType
get_curr_time(uint32 coreid, TickType maxval)
{
	TickType curr_time = 0U;
	uint32 ctime_base = hwc_ctim_base[coreid];

	curr_time = sil_rew_mem((void *)(ctime_base + TTC_CNTR_VAL_OFFSET));

	return(curr_time);
}

/*
 *  ハードウェアカウンタの初期化
 */
static void
init_hwcounter(uint32 coreid, TickType maxval, TimeType nspertick, TickRefType cycle)
{
	uint32 dtime_base = hwc_dtim_base[coreid];
	uint32 ctime_base = hwc_ctim_base[coreid];
	uint32 reset_mask = hwc_reset_mask[coreid];

	*cycle = maxval;

	/*
	 *  Release Reset
	 */
	sil_wrw_mem((void *)RST_LPD_IOU2, 
				sil_rew_mem((void *)RST_LPD_IOU2) & (~reset_mask));

	/*
	 *  差分タイマ初期化
	 */
	/* Disable Timer */
	sil_wrw_mem((void *)(dtime_base + TTC_CNTR_CNTRL_OFFSET), 
				TTC_CNTR_CNTRL_DIS_MASK);

	/* Disable Priscaler */
	sil_wrw_mem((void *)(dtime_base + TTC_CLK_CNTRL_OFFSET), 
				sil_rew_mem((void *)(dtime_base + TTC_CLK_CNTRL_OFFSET))
				& (~TTC_CLK_CNTRL_PS_EN_MASK));

	/*
	 *  現在値タイマ初期化
	 */
	/* Disable Timer */
	sil_wrw_mem((void *)(ctime_base + TTC_CNTR_CNTRL_OFFSET), 
				TTC_CNTR_CNTRL_DIS_MASK);

	/* Disable Priscaler */
	sil_wrw_mem((void *)(ctime_base + TTC_CLK_CNTRL_OFFSET), 
				sil_rew_mem((void *)(ctime_base + TTC_CLK_CNTRL_OFFSET))
				& (~TTC_CLK_CNTRL_PS_EN_MASK));

	/* インターバル値をセット */
	sil_wrw_mem((void *)(ctime_base + TTC_INTER_CNTR_OFFSET), maxval);
}

/*
 *  ハードウェアカウンタの開始
 */
static void
start_hwcounter(uint32 coreid)
{
	uint32 ctime_base = hwc_ctim_base[coreid];

	/*
	 *  現在値タイマ開始(Interval Mode)
	 *  割込み必要がないため，割込みなしモード
	 */
	sil_wrw_mem((void *)(ctime_base + TTC_CNTR_CNTRL_OFFSET),
				TTC_CNTR_CNTRL_RST_MASK|TTC_CNTR_CNTRL_INT_MASK);
}

/*
 *  ハードウェアカウンタの停止
 */
static void
stop_hwcounter(uint32 coreid)
{
	uint32 dtime_base = hwc_dtim_base[coreid];
	uint32 ctime_base = hwc_ctim_base[coreid];

	/* 差分タイマ停止 */
	sil_wrw_mem((void *)(dtime_base + TTC_CNTR_CNTRL_OFFSET),
				TTC_CNTR_CNTRL_DIS_MASK);

	/* 差分タイマの割込み要求のクリア */
	(void)sil_rew_mem((void*)(dtime_base + TTC_INT_REG_OFFSET));

	/* 現在値タイマ停止 */
	sil_wrw_mem((void *)(ctime_base + TTC_CNTR_CNTRL_OFFSET),
				TTC_CNTR_CNTRL_DIS_MASK);
}

/*
 *  ハードウェアカウンタへの満了時間の設定
 */
static void
set_hwcounter(uint32 coreid, TickType exprtick, TickType maxval)
{
	uint32 dtime_base = hwc_dtim_base[coreid];
	TickType	curr_time;
	TickType	value;

	/* 差分タイマの割込み要求のクリア */
	(void)sil_rew_mem((void*)(dtime_base + TTC_INT_REG_OFFSET));

	/* 現在時刻の取得 */
	curr_time = get_curr_time(coreid, maxval);

	/* タイマに設定する値を算出	*/
	if (exprtick >= curr_time) {
		value = exprtick - curr_time;
	}
	else {
		value = ((maxval - curr_time) + exprtick) + 1U;
	}

	/*
	 *  タイマに0x00を設定し，割込み発生後，再度0を設定した場合，2回目の
	 *  0x00設定後の割込みは発生しないので，0x00設定値を0x01に直して設定
	 *  タイマにセットする値は，期待する周期のカウント値から-1する必要がある
	 */
	if (value <= 0x01U) {
		value = 0x01U;
	}
	else {
		value -= 1U;
	}

	/*
	 *  差分タイママッチ値セット
	 */
	sil_wrw_mem((void *)(dtime_base + TTC_INT_MATCH1_OFFSET), value);

	/* Enable Match1 Interrupt */
	sil_wrw_mem((void *)(dtime_base + TTC_INT_ENA_OFFSET),
				TTC_INT_REG_M1);

	/*
	 *  差分タイマ開始(count down once mode)
	 *  割込み必要のため，割込みモードあり
	 */
	/* Enable Timer : Match, upcount, Counter Reset */
	sil_wrw_mem((void *)(dtime_base + TTC_CNTR_CNTRL_OFFSET),
				TTC_CNTR_CNTRL_RST_MASK|TTC_CNTR_CNTRL_MATCH_MASK);
}

/*
 *  ハードウェアカウンタの現在時間の取得
 */
static TickType
get_hwcounter(uint32 coreid, TickType maxval)
{
	return(get_curr_time(coreid, maxval));
}

/*
 *  ハードウェアカウンタの設定された満了時間の取消
 */
static void
cancel_hwcounter(uint32 coreid)
{
	uint32 dtime_base = hwc_dtim_base[coreid];

	/* 差分タイマ停止 */
	sil_wrw_mem((void *)(dtime_base + TTC_CNTR_CNTRL_OFFSET),
				TTC_CNTR_CNTRL_DIS_MASK);
}

/*
 *  ハードウェアカウンタの強制割込み要求
 */
static void
trigger_hwcounter(uint32 coreid)
{
	uint32 dtime_base = hwc_dtim_base[coreid];

	/* 差分タイマ停止 */
	sil_wrw_mem((void *)(dtime_base + TTC_CNTR_CNTRL_OFFSET),
				TTC_CNTR_CNTRL_DIS_MASK);

	/* 差分タイマの割込み要求のクリア */
	(void)sil_rew_mem((void*)(dtime_base + TTC_INT_REG_OFFSET));

	/* 差分タイママッチ値に0x01をセットすることで，すぐ満了 */
	sil_wrw_mem((void *)(dtime_base + TTC_INT_MATCH1_OFFSET), 0x01U);

	/* Enable Match1 Interrupt */
	sil_wrw_mem((void *)(dtime_base + TTC_INT_ENA_OFFSET),
				TTC_INT_REG_M1);

	/* Enable Timer : Match, upcount, Counter Reset */
	sil_wrw_mem((void *)(dtime_base + TTC_CNTR_CNTRL_OFFSET),
				TTC_CNTR_CNTRL_RST_MASK|TTC_CNTR_CNTRL_MATCH_MASK);
}

/*
 *  割込み要求のクリア
 */
static void
int_clear_hwcounter(uint32 coreid)
{
	uint32 dtime_base = hwc_dtim_base[coreid];

	/* 差分タイマの割込み要求のクリア */
	(void)sil_rew_mem((void*)(dtime_base + TTC_INT_REG_OFFSET));
}

/*
 *  割込み要求のキャンセル
 *  ペンディングされている割込み要求をキャンセル
 */
static void
int_cancel_hwcounter(uint32 coreid)
{
	uint32 dtime_base = hwc_dtim_base[coreid];

	/* 差分タイマの割込み要求のクリア */
	(void)sil_rew_mem((void*)(dtime_base + TTC_INT_REG_OFFSET));
}

/*
 *  ハードウェアカウンタのインクリメント
 */
static void
increment_hwcounter(uint32 coreid)
{
	/* Nios2ターゲットは未サポート */
	return;
}

/*
 *  MAIN_HW_COUNTER_CORE0の定義
 */
/* カウンタの最大値の2倍+1 */
static TickType MAIN_HW_COUNTER_CORE0_maxval;

/*
 *  ハードウェアカウンタの初期化
 */
void
init_hwcounter_MAIN_HW_COUNTER_CORE0(TickType maxval, TimeType nspertick)
{
	init_hwcounter(0, maxval, nspertick, &MAIN_HW_COUNTER_CORE0_maxval);
}

/*
 *  ハードウェアカウンタの開始
 */
void
start_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	start_hwcounter(0);
}

/*
 *  ハードウェアカウンタの停止
 */
void
stop_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	stop_hwcounter(0);
}

/*
 *  ハードウェアカウンタへの満了時間の設定
 */
void
set_hwcounter_MAIN_HW_COUNTER_CORE0(TickType exprtick)
{
	set_hwcounter(0, exprtick, MAIN_HW_COUNTER_CORE0_maxval);
}

/*
 *  ハードウェアカウンタの現在時間の取得
 */
TickType
get_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	return(get_hwcounter(0, MAIN_HW_COUNTER_CORE0_maxval));
}

/*
 *  ハードウェアカウンタの設定された満了時間の取消
 */
void
cancel_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	cancel_hwcounter(0);
}

/*
 *  ハードウェアカウンタの強制割込み要求
 */
void
trigger_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	trigger_hwcounter(0);
}

/*
 *  ハードウェアカウンタの設定された満了時間の取消
 */
void
int_clear_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	int_clear_hwcounter(0);
}

/*
 *  ハードウェアカウンタの設定された満了時間の取消
 */
void
int_cancel_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	int_cancel_hwcounter(0);
}

/*
 *  ハードウェアカウンタのインクリメント
 */
void
increment_hwcounter_MAIN_HW_COUNTER_CORE0(void)
{
	increment_hwcounter(0);
}

/*
 *  MAIN_HW_COUNTER_CORE1の定義
 */
/* カウンタの最大値の2倍+1 */
static TickType MAIN_HW_COUNTER_CORE1_maxval;

/*
 *  ハードウェアカウンタの初期化
 */
void
init_hwcounter_MAIN_HW_COUNTER_CORE1(TickType maxval, TimeType nspertick)
{
	init_hwcounter(1, maxval, nspertick, &MAIN_HW_COUNTER_CORE1_maxval);
}

/*
 *  ハードウェアカウンタの開始
 */
void
start_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	start_hwcounter(1);
}

/*
 *  ハードウェアカウンタの停止
 */
void
stop_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	stop_hwcounter(1);
}

/*
 *  ハードウェアカウンタへの満了時間の設定
 */
void
set_hwcounter_MAIN_HW_COUNTER_CORE1(TickType exprtick)
{
	set_hwcounter(1, exprtick, MAIN_HW_COUNTER_CORE1_maxval);
}

/*
 *  ハードウェアカウンタの現在時間の取得
 */
TickType
get_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	return(get_hwcounter(1, MAIN_HW_COUNTER_CORE1_maxval));
}

/*
 *  ハードウェアカウンタの設定された満了時間の取消
 */
void
cancel_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	cancel_hwcounter(1);
}

/*
 *  ハードウェアカウンタの強制割込み要求
 */
void
trigger_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	trigger_hwcounter(1);
}

/*
 *  ハードウェアカウンタの設定された満了時間の取消
 */
void
int_clear_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	int_clear_hwcounter(1);
}

/*
 *  ハードウェアカウンタの設定された満了時間の取消
 */
void
int_cancel_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	int_cancel_hwcounter(1);
}

/*
 *  ハードウェアカウンタのインクリメント
 */
void
increment_hwcounter_MAIN_HW_COUNTER_CORE1(void)
{
	increment_hwcounter(1);
}
