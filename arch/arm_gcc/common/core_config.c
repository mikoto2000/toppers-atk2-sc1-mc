/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_config.c 849 2017-09-29 13:34:39Z ertl-honda $
 */

/*
 *        コア依存モジュール（ARM用）
 */
#include "kernel_impl.h"
#include "check.h"
#include "task.h"

/*
 *  start.S での同期用変数
 */
volatile uint_t start_sync[TotalNumberOfCores];

#if __TARGET_ARCH_ARM >= 6
/*
 *  SILのスピンロック用変数
 */
uint32_t TOPPERS_spn_var;
#endif /* __TARGET_ARCH_ARM >= 6 */

/*
 *  プロセッサ依存の初期化
 */
void
core_initialize(void)
{
	ACCB *p_accb;
	uint32_t id = x_core_id();

#ifdef USE_THREAD_ID_CCB
	/*
	 *  Thread ID レジスタへのCCBへのポインタの設定
	 */
	CPU15_PONLY_THREAD_ID_WRITE((uint32_t)p_ccb_table[id]);
#endif /* USE_THREAD_ID_CCB */

	p_accb = &((get_my_p_ccb())->target_ccb.arm_ccb);

	/*
	 *  カーネル起動時は非タスクコンテキストとして動作させるため1に
	 */
	p_accb->except_nest_cnt = 1;

	/*
	 *  非タスクコンテキスト用のスタックの初期値
	 */
	p_accb->ostkpt = ostkpt_table[id];

	/*
	 *  CPU例外ハンドラテーブルへのポインタの初期化
	 */
	p_accb->p_exch_tbl = p_exch_table[id];

	p_accb->trusted_hook_savedsp = 0U;

	p_accb->nested_lock_os_int_cnt = 0U;
	
}

/*
 *  プロセッサ依存の終了処理
 */
void
core_terminate(void)
{
	/* ToDo */
}

/*
 * CPU例外ハンドラの初期化
 */
void
initialize_exception(void)
{
}

/*
 *  CPU例外の発生状況のログ出力
 *
 *  CPU例外ハンドラの中から，CPU例外情報ポインタ（p_excinf）を引数とし
 *  て呼び出すことで，CPU例外の発生状況をシステムログに出力する．
 */
#ifdef SUPPORT_XLOG_SYS

void
xlog_sys(void *p_excinf)
{
}

#endif /* SUPPORT_XLOG_SYS */

#ifndef OMIT_DEFAULT_INT_HANDLER

/*
 *  未定義の割込みが入った場合の処理
 */
void
default_int_handler(void){
	while(1);
}

#endif /* OMIT_DEFAULT_INT_HANDLER */

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 * 未定義の例外が入った場合の処理
 */

void
dump_excframe(exc_frame_t *p_exc_frame)
{
	while(1);
#if 0
	ID prc_id = ID_PRC(x_core_id());

	syslog_3(LOG_EMERG, "Processor %d : nest_count = %d, ipm = %d.",
			 prc_id, p_exc_frame->nest_count, p_exc_frame->ipm);
	syslog_4(LOG_EMERG, "Processor %d : r0 = 0x%08x, r1 = 0x%08x, r2 = 0x%08x",
			 prc_id, p_exc_frame->r0, p_exc_frame->r1, p_exc_frame->r2);
	syslog_3(LOG_EMERG, "Processor %d : r3 = 0x%08x, r12 = 0x%08x",
			 prc_id, p_exc_frame->r3, p_exc_frame->r12);
	syslog_4(LOG_EMERG, "Processor %d : lr = 0x%08x, pc = 0x%08x, cpsr = 0x%08x",
			 prc_id, p_exc_frame->lr, p_exc_frame->pc, p_exc_frame->cpsr);
#endif
}

void
default_exc_handler(void *p_excinf){
	while(1);
#if 0
	ID prc_id = ID_PRC(x_core_id());

	syslog_1(LOG_EMERG, "\nProcessor %d : Unregistered Exception occurs.", prc_id);
	dump_excframe((exc_frame_t*)p_excinf);
	target_exit();
#endif
}

void
default_undef_handler(void *p_excinf){
	while(1);
#if 0
	ID prc_id = ID_PRC(x_core_id());

	syslog_1(LOG_EMERG, "\nProcessor %d : Undef Exception occurs.", prc_id);
	dump_excframe((exc_frame_t*)p_excinf);
	target_exit();
#endif
}

void
default_swi_handler(void *p_excinf){
	while(1);
#if 0
	ID prc_id = ID_PRC(x_core_id());

	syslog_1(LOG_EMERG, "\nProcessor %d : SWI Exception occurs.", prc_id);
	dump_excframe((exc_frame_t*)p_excinf);
	target_exit();
#endif
}

void
default_prefetchabort_handler(void *p_excinf){
	while(1);
#if 0
	ID prc_id = ID_PRC(x_core_id());

	syslog_1(LOG_EMERG, "\nProcessor %d : Prefetch Abort Exception occurs.", prc_id);
	dump_excframe((exc_frame_t*)p_excinf);
	target_exit();
#endif
}

void
default_dataabort_handler(void *p_excinf){
	while(1);
#if 0
	ID prc_id = ID_PRC(x_core_id());

	syslog_1(LOG_EMERG, "\nProcessor %d : Data Abort Exception occurs.", prc_id);
	dump_excframe((exc_frame_t*)p_excinf);
	target_exit();
#endif
}

#endif /* OMIT_DEFAULT_EXC_HANDLER */
