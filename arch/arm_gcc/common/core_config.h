/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_config.h 780 2017-03-07 01:32:17Z ertl-honda $
 */


/*
 *      コア依存モジュール（ARM用）
 *
 *  このインクルードファイルは，target_config.h（または，そこからインク
 *  ルードされるファイル）のみからインクルードされる．他のファイルから
 *  直接インクルードしてはならない．
 */

#ifndef TOPPERS_CORE_CONFIG_H
#define TOPPERS_CORE_CONFIG_H

/*
 *  ARM依存の定義
 */
#include "arm.h"

/*
 *  エラーチェック方法の指定
 */
#define CHECK_STKSZ_ALIGN	8	/* スタックサイズのアライン単位 */
#define CHECK_FUNC_ALIGN	4	/* 関数のアライン単位 */
#define CHECK_FUNC_NONNULL		/* 関数の非NULLチェック */
#define CHECK_STACK_ALIGN	4	/* スタック領域のアライン単位 */
#define CHECK_STACK_NONNULL		/* スタック領域の非NULLチェック */
#define CHECK_MPF_ALIGN		4	/* 固定長メモリプール領域のアライン単位 */
#define CHECK_MPF_NONNULL		/* 固定長メモリプール領域の非NULLチェック */
#define CHECK_MB_ALIGN		4	/* 管理領域のアライン単位 */

/*
 *  CPUロックとするCPSRのパターン
 */
#ifdef TOPPERS_MACRO_ONLY

#ifdef TOPPERS_SAFEG_SECURE
#define CPSR_OSLOCK     (CPSR_FIQ_BIT AOR CPSR_IRQ_BIT)
#else  /* !TOPPERS_SAFEG_SECURE */
#define CPSR_OSLOCK     (CPSR_IRQ_BIT)
#endif /* TOPPERS_SAFEG_SECURE */

#else /* !TOPPERS_MACRO_ONLY */

#ifdef TOPPERS_SAFEG_SECURE
#define CPSR_OSLOCK     (CPSR_FIQ_BIT|CPSR_IRQ_BIT)
#else  /* !TOPPERS_SAFEG_SECURE */
#define CPSR_OSLOCK     (CPSR_IRQ_BIT)
#endif /* TOPPERS_SAFEG_SECURE */

#endif /* TOPPERS_MACRO_ONLY */


/*
 *  割込みロックとするCPSRのパターン
 */
#ifdef TOPPERS_MACRO_ONLY
#define CPSR_ALLLOCK     (CPSR_FIQ_BIT AOR CPSR_IRQ_BIT)
#else /* !TOPPERS_MACRO_ONLY */
#define CPSR_ALLLOCK     (CPSR_FIQ_BIT|CPSR_IRQ_BIT)
#endif /* TOPPERS_MACRO_ONLY */


/*
 *  CPSRに常にセットするパターン
 */
#ifdef TOPPERS_SAFEG_SECURE
#define CPSR_ALWAYS_SET  CPSR_IRQ_BIT
#else  /* !TOPPERS_SAFEG_SECURE */
#define CPSR_ALWAYS_SET  0x00
#endif /* TOPPERS_SAFEG_SECURE */

/*
 *  例外の番号
 */
#define EXCH_NO_RESET     0
#define EXCH_NO_UNDEF     1
#define EXCH_NO_SWI       2
#define EXCH_NO_PABORT    3
#define EXCH_NO_DABORT    4
#define EXCH_NO_IRQ       5
#define EXCH_NO_FIQ       6

/*
 *  例外の個数
 */
#define TNUM_EXCH   7

#define call_errorhook(ercd, svcid)	stack_change_and_call_func_2((void *) internal_call_errorhook, (ercd), (svcid))
#define call_shutdownhook(ercd)	stack_change_and_call_func_1((void *) internal_call_shtdwnhk, (ercd))

#ifndef TOPPERS_MACRO_ONLY

/*
 *  非タスクコンテキスト用のスタック初期値
 */
#define TOPPERS_OSTKPT(stk, stksz)	((StackType *) ((sint8 *) (stk) + (stksz)))

/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#include <core_insn.h>

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
    void  *sp;       /* スタックポインタ */
    FP    pc;        /* プログラムカウンタ */
} TSKCTXB;

/*
 * マスタプロセッサの判定
 */
Inline bool_t
x_sense_mcore(void)
{
	return(x_core_id() == OS_CORE_ID_MASTER);
}

/*
 *  TOPPERS標準割込み処理モデルの実現
 *
 *  IRQをカーネル管理内，FIQをカーネル管理外の割込みとして扱う．
 *
 *  ARM依存部では，TOPPERS標準割込み処理モデルのうち，CPUロック状態
 *  のみを取り扱う．割込み優先度マスク，割込み要求禁止フラグに関して
 *  は，各ターゲット依存部で取り扱う
 */

/*
 *  全割込み禁止状態への移行
 */
LOCAL_INLINE void
x_lock_all_int(void)
{
	set_sr(current_sr() | CPSR_ALLLOCK | CPSR_ALWAYS_SET);
	ARM_MEMORY_CHANGED;
}

/*
 *  全割込み禁止状態の解除
 */
LOCAL_INLINE void
x_unlock_all_int(void)
{
	ARM_MEMORY_CHANGED;
	set_sr((current_sr() & (~CPSR_ALLLOCK)) | CPSR_ALWAYS_SET);
}

/*
 *  OS割込み禁止
 *  API実行時に呼び出される
 *  割込み優先度マスクがC2ISRの優先度マスクの範囲より高い状態で呼び出される
 *  ことはない(C1ISRから呼び出されることはない)
 */
LOCAL_INLINE void
x_nested_lock_os_int(void)
{
	CCB		*p_ccb = get_my_p_ccb();

	set_sr(current_sr() | CPSR_OSLOCK | CPSR_ALWAYS_SET);
	p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt++;

	Asm("" ::: "memory");
}

/*
 *  OS割込み解除
 *  API実行時に呼び出される
 *  割込み優先度マスクがC2ISRの優先度マスクの範囲より高い状態で呼び出される
 *  ことはない(C1ISRから呼び出されることはない)
 */
LOCAL_INLINE void
x_nested_unlock_os_int(void)
{
	CCB *p_ccb = get_my_p_ccb();

	Asm("" ::: "memory");

	ASSERT(p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt > 0U);
	p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt--;

	/*
	 *  一番外側ならOS割込み許可
	 */
	if (p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt == 0U) {
		set_sr((current_sr() & (~CPSR_OSLOCK)) | CPSR_ALWAYS_SET);
	}
}


/*
 *  SuspendOSInterrupts()から呼び出すOS割込み禁止状態への移行
 *  x_nested_lock_os_int()との違いは，C1ISRから呼び出される可能性がある．
 */
LOCAL_INLINE void
x_suspend_lock_os_int(void)
{
	CCB		*p_ccb = get_my_p_ccb();
	uint32	sr = current_sr();

	/*
	 *  C1ISRから呼ばれた場合は何もしない
	 */
	if ((sr & CPSR_ALLLOCK) != CPSR_ALLLOCK) {
		set_sr(current_sr() | CPSR_OSLOCK | CPSR_ALWAYS_SET);
		p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt++;

		Asm("" ::: "memory");
	}
}

/*
 *  ResumeOSInterrupts()から呼び出すOS割込み禁止状態の解除
 *  x_nested_unlock_os_int()との違いは，C1ISRから呼び出される可能性がある．
 */
LOCAL_INLINE void
x_resume_unlock_os_int(void)
{
	CCB		*p_ccb = get_my_p_ccb();
	uint32	sr = current_sr();

	Asm("" ::: "memory");

	/*
	 * C1ISRから呼ばれた場合は何もしない
	 */
	if ((sr & CPSR_ALLLOCK) != CPSR_ALLLOCK) {

		ASSERT(p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt > 0U);
		p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt--;

		/*
		 *  一番外側ならOS割込みを許可
		 */
		if (p_ccb->target_ccb.arm_ccb.nested_lock_os_int_cnt == 0U) {
			set_sr((current_sr() & (~CPSR_OSLOCK)) | CPSR_ALWAYS_SET);
		}
	}
}

/*
 *  タスクディスパッチャ
 */

/*
 *  最高優先順位タスクへのディスパッチ（core_support.S）
 *
 *  dispatchは，タスクコンテキストから呼び出されたサービスコール処理か
 *  ら呼び出すべきもので，タスクコンテキスト・CPUロック状態・ディスパッ
 *  チ許可状態・（モデル上の）割込み優先度マスク全解除状態で呼び出さな
 *  ければならない．
 */
extern void dispatch(void);

/*
 *  自タスクのマイグレーション（core_support.S）
 */
extern void dispatch_and_migrate(ID prcid);

/*
 *  現在のコンテキストを捨てマイグレーション（core_support.S）
 */
extern void exit_and_migrate(ID prcid);

/*
 *  ディスパッチャの動作開始（core_support.S）
 *
 *  start_dispatchは，カーネル起動時に呼び出すべきもので，すべての割込
 *  みを禁止した状態（割込みロック状態と同等の状態）で呼び出さなければ
 *  ならない．
 */
extern void start_dispatch(void) NoReturn;

/*
 *  現在のコンテキストを捨ててディスパッチ（core_support.S）
 *
 *  exit_and_dispatchは，ext_tskから呼び出すべきもので，タスクコンテキ
 *  スト・CPUロック状態・ディスパッチ許可状態・（モデル上の）割込み優先
 *  度マスク全解除状態で呼び出さなければならない．
 */
extern void exit_and_dispatch(void) NoReturn;

/*
 *  カーネルの終了処理の呼出し（core_support.S）
 *
 *  call_exit_kernelは，カーネルの終了時に呼び出すべきもので，非タスク
 *  コンテキストに切り換えて，カーネルの終了処理（exit_kernel）を呼び出
 *  す．
 */
extern void call_exit_kernel(void) NoReturn;

/*
 *  タスクコンテキストの初期化
 *
 *  タスクが休止状態から実行できる状態に移行する時に呼ばれる．この時点
 *  でスタック領域を使ってはならない．
 *
 *  activate_contextを，インライン関数ではなくマクロ定義としているのは，
 *  この時点ではTCBが定義されていないためである．
 */
extern void    start_r(void);

#define activate_context(p_tcb)                                         \
{                                                                       \
    (p_tcb)->tskctxb.sp = (void *)((char *)((p_tcb)->p_tinib->stk)    \
                                        + (p_tcb)->p_tinib->stksz);     \
    (p_tcb)->tskctxb.pc = (FP)start_r;                                  \
}

extern void call_trusted_hook(void *hook, StatusType arg);
extern void exit_trusted_shutdown_hook(void) NoReturn;

/* 引数の型はハードウェアが扱えるサイズ（uint32）と同サイズを使用 */
extern void stack_change_and_call_func_1(void (*func)(StatusType ercd), uint32 arg1);

extern void stack_change_and_call_func_2(void (*func)(StatusType ercd, OSServiceIdType svcid), uint8 arg1, uint8 arg2);

/* スタックモニタリングエラー処理 */
extern void stack_monitoring_error_isr(void);

/*
 *  calltexは使用しない
 */
#define OMIT_CALLTEX

/*
 * ターゲット非依存部に含まれる標準の例外管理機能の初期化処理を用いない
 */
#define OMIT_INITIALIZE_EXCEPTION

/*
 *  CPU例外ハンドラの登録用テーブル（kernel_cfg.c）
 */
extern const FP* const p_exch_table[];

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のCPSRの取得
 */
Inline uint32_t
exc_get_sr(void *p_excinf)
{
	return(((exc_frame_t *)(p_excinf))->cpsr);
}

/*
 * CPUロック状態か
 */
Inline bool_t
exc_sense_lock(void *p_excinf)
{
	return((exc_get_sr(p_excinf) & CPSR_OSLOCK) == CPSR_OSLOCK);
}

/*
 * 割込みロック状態か
 */
Inline bool_t
exc_sense_int_lock(void *p_excinf)
{
	return((exc_get_sr(p_excinf) & CPSR_ALLLOCK) == CPSR_ALLLOCK);
}

#if __TARGET_ARCH_ARM >= 6
/*
 *  ロック関係
 */

/*
 *  スピンロック関連のコード
 */
#if TTYPE_KLOCK == G_KLOCK

/*
 *  ジャイアントロックの初期化（G_KLOCKの場合）
 */
LOCAL_INLINE void
x_initialize_giant_lock(LockType *p_giant_lock)
{
	*p_giant_lock = 0;
}

#endif /* TTYPE_KLOCK == G_KLOCK */

#if TTYPE_KLOCK == C_KLOCK

/*
 *  タスクロックの初期化（C_KLOCKの場合）
 */
LOCAL_INLINE void
x_initialize_tsk_lock(LockType *p_tsk_lock)
{
	*p_tsk_lock = 0;
}

/*
 *  カウンタロックの初期化
 */
LOCAL_INLINE void
x_initialize_cnt_lock(LockType *p_cnt_lock)
{
	*p_cnt_lock = 0;
}

/*
 *  スピンロックロックの初期化
 */
LOCAL_INLINE void
x_initialize_spn_lock(LockType *p_spn_lock)
{
	*p_spn_lock = 0;
}

/*
 *  IOCロックの初期化
 */
LOCAL_INLINE void
x_initialize_ioc_lock(LockType *p_ioc_lock)
{
	*p_ioc_lock = 0;
}

#endif /* TTYPE_KLOCK == C_KLOCK */

#endif /* __TARGET_ARCH_ARM >= 6 */

/*
 *  プロセッサ依存の初期化
 */
extern void core_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void core_terminate(void);

/*
 *  未定義の割込みが入った場合の処理
 */
extern void default_int_handler(void);

/*
 *  ベクターテーブルの命令から参照されるジャンプ先アドレス
 * （start.S）
 */
extern void *vector_ref_tbl;

/*
 *  例外ハンドラ
 */

/*
 *  未定義命令 例外ハンドラ（core_support.S）
 */
extern void undef_handler(void);

/*
 *  SWI 例外ハンドラ（core_support.S）
 */
extern void swi_handler(void);

/*
 *  プリフェッチアボード 例外ハンドラ（core_support.S）
 */
extern void prefetch_handler(void);

/*
 *  データアボード 例外ハンドラ（core_support.S）
 */
extern void data_abort_handler(void);

/*
 *  FIQ 例外ハンドラ（core_support.S）
 */
extern void fiq_handler(void);

/*
 *  未定義の例外が入った場合の処理
 */
extern void default_exc_handler(void *p_excinf);
extern void default_undef_handler(void *p_excinf);
extern void default_swi_handler(void *p_excinf);
extern void default_prefetchabort_handler(void *p_excinf);
extern void default_dataabort_handler(void *p_excinf);
#endif /* TOPPERS_MACRO_ONLY */

/*
 *  コア依存モジュールのツール依存部
 */
#include <core_config_tool.h>
#endif /* TOPPERS_CORE_CONFIG_H */
