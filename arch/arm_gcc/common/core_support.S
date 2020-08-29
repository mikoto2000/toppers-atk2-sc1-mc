/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_support.S 804 2017-08-10 11:54:37Z ertl-honda $
 */

/*
 *        コア依存モジュール アセンブリ言語部（ARM用）
 */

#define TOPPERS_MACRO_ONLY
#define TOPPERS_ASM_MACRO
#define UINT_C(val)		(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)	(val)		/* ulong_t型の定数を作るマクロ */

#include "kernel_impl.h"
#include "offset.h"
#include "target_asm.inc"

/*
 * 例外ベクタ
 */
    ASECTION(vector)
    AGLOBAL(vector_table)
ALABEL(vector_table)
    ldr pc, reset_vector       /* リセット             */
    ldr pc, undef_vector       /* 未定義命令           */
    ldr pc, swi_vector         /* ソフトウェア割込み   */
    ldr pc, prefech_vector     /* プリフェッチアボート */
    ldr pc, data_abort_vector  /* データアボート       */
    ldr pc, reset_vector
    ldr pc, irq_vector         /* IRQ                  */
    ldr pc, fiq_vector         /* FIQ                  */

/*
 *  例外ベクタの命令から参照される
 *  ジャンプ先アドレス
 */
    AGLOBAL(vector_ref_tbl)
    AGLOBAL(vector_ref_tbl_end)
ALABEL(vector_ref_tbl)
ALABEL(reset_vector)
    ALONG   start
ALABEL(undef_vector)
    ALONG   undef_handler
ALABEL(swi_vector)
    ALONG   swi_handler
ALABEL(prefech_vector)
    ALONG   prefetch_handler
ALABEL(data_abort_vector)
    ALONG   data_abort_handler
ALABEL(irq_vector)
    ALONG   irq_handler
ALABEL(fiq_vector)
    ALONG   fiq_handler
ALABEL(vector_ref_tbl_end)

#include "target_asm.inc"

/*
 *  タスクディスパッチャ
 *
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(dispatch)
ALABEL(dispatch)
    /*
     *  このルーチンは，タスクコンテキスト・CPUロック状態・ディスパッチ
     *  許可状態・（モデル上の）割込み優先度マスク全解除状態で呼び出さ
     *  れる．
     */
    stmfd  sp!, {r4-r12,lr}    /* スクラッチレジスタ以外のレジスタの保存,r12はスタック境界調整のためのダミー -(10*4) */
#if defined(SUPPORT_VFPv3_NEON)
    vpush   {d8-d15}              
    vpush   {d16-d31}             
#endif /* SUPPORT_VFPv3_NEON */
    my_ccb r0, r1                       /* p_runtskを読み込む */
    ldr    r1, [r0,#CCB_p_runtsk]
    str    sp, [r1,#TCB_sp]             /* タスクスタックを保存 */
    adr    r2, dispatch_r
    str    r2, [r1,#TCB_pc]             /* 実行再開番地を保存 */
    b      dispatcher

    /*
     *  実行中タスクのタスクスタックのオーバフローチェック
     */
#ifdef CFG_USE_STACKMONITORING
    /*
     *  スタックポインタチェック方式
     */
    ldr   r3, [r1, #TCB_p_tinib]		/* タスク初期化ブロック先頭アドレス取得 */
    ldr   r3, [r3, #TINIB_stk]			/* タスクスタック先頭アドレス取得 */
    cmp   sp, r3						/* スタックポインタ <= スタックの先頭の場合， */
    ble   stack_monitoring_error		/* スタックオーバーフロー発生 */

    /*
     * マジックナンバーチェック方式
     */
    ldr   r4, [r3]						/* タスクスタックの先頭アドレスからマジックナンバー領域の値取得 */
    ldr   r3, =STACK_MAGIC_NUMBER
    cmp   r4, r3						/* マジックナンバーが破壊されていた場合， */
    bne   stack_monitoring_error		/* スタックオーバーフロー発生 */
#endif /* CFG_USE_STACKMONITORING */

ALABEL(dispatch_r)
#if defined(SUPPORT_VFPv3_NEON)
    vpop    {d16-d31}
    vpop    {d8-d15}
#endif /* SUPPORT_VFPv3_NEON */
    ldmfd sp!,{r4-r12,lr}     /* スクラッチレジスタ以外のレジスタの復帰,r12はスタック境界調整のためのダミー +(10*4) */
    bx    lr

/*
 *  ディスパッチャの動作開始（cpu_support.S）
 */
    AGLOBAL(start_dispatch)
ALABEL(start_dispatch)
    /*
     *  このルーチンは，カーネル起動時に，すべての割込みを禁止した状態
     *  （割込みロック状態と同等）で呼び出される．また，非タスクコンテキ
     *  ストから呼び出されることを想定している．
     *
     *  dispatcherは，CPUロック状態，（モデル上の）割込み優先度マスク全
     *  解除状態，例外（割込み/CPU例外）のネスト回数0で呼び出す．
     *  target_initializeでは，（モデル上の）割込み優先度マスク全解除
     *  とし，カーネル管理外の割込みであるFIQを許可することで，
     *  CPUロック状態・（モデル上の）割込み優先度マスク全解除状態になる．
     *  また，initialize_taskでdisdspをFALSEに初期化しているため，ディ
     *  スパッチ許可状態になっている．
     *
     */
    my_ccb r0, r1                          
    OUTER_LOCK_OS_INT  r1, r0
    UNLOCK_ALL_INT r1
    mov   r1, #0                           /* 例外（割込み/CPU例外）のネスト回数 */
    str   r1, [r0, #CCB_except_nest_cnt]  /* を0に                              */
    b     dispatcher_0

/*
 *  現在のコンテキストを捨ててディスパッチ
 */
    AGLOBAL(exit_and_dispatch)
ALABEL(exit_and_dispatch)
#ifdef CFG_USE_STACKMONITORING
	/*
	 *  実行中タスクのタスクスタックのオーバフローチェック
	 */
	/*
	 *  スタックポインタチェック方式
	 */
    my_ccb r0, r1                       /* p_runtskを読み込む */
    ldr    r1, [r0,#CCB_p_runtsk]
    ldr    r2, [r1, #TCB_p_tinib]		/* タスク初期化ブロック先頭アドレス取得 */
    ldr    r2, [r2, #TINIB_stk]			/* タスクスタック先頭アドレス取得 */
    cmp    sp, r2						/* スタックポインタ <= スタックの先頭の場合， */
    ble    stack_monitoring_error		/* スタックオーバーフロー発生 */

	/*
	 *  マジックナンバーチェック方式
	 */
    ldr   r3, [r2]						/* タスクスタックの先頭アドレスからマジックナンバー領域の値取得 */
    ldr   r2, =STACK_MAGIC_NUMBER
    cmp   r3, r2						/* マジックナンバーが破壊されていた場合， */
    bne   stack_monitoring_error		/* スタックオーバーフロー発生 */
#endif /* CFG_USE_STACKMONITORING */
    /* ディスパッチャ本体（dispatcher）へ */
    my_ccb r0, r1                       /* my_ccbを読み込む */

/*
 *  ディスパッチャ本体
 */
ALABEL(dispatcher)
    /*
     *  PostTaskHookの呼び出し
     */
#ifdef CFG_USE_POSTTASKHOOK
posttaskhook_call:
    ldr   r0, =call_posttaskhook
    bl    stack_change_and_call_func_1
    my_ccb r0, r1
#endif /* CFG_USE_POSTTASKHOOK */

ALABEL(dispatcher_0)
    /*
     *  このルーチンは，タスクコンテキスト・CPUロック状態・ディスパッチ
     *  許可状態・（モデル上の）割込み優先度マスク全解除状態で呼び出さ
     *  れる．実行再開番地へもこの状態のまま分岐する．
     *
     *  すなわち，スーパーバイザーモード，IRQ禁止となっている．
     *  r0には，my_ccb へのポインタを入れる
     */
#ifdef LOG_DSP_ENTER
    stmfd sp!, {r0}
    ldr   r1, [r0, #CCB_p_runtsk]
    mov   r0, r1
    bl    log_dsp_enter
    ldmfd sp!, {r0}
#endif /* LOG_DSP_ENTER */

    ldrh  r1, [r0, #CCB_callevel_stat]
    and   r1, r1, #(~TSYS_DISALLINT)
    strh  r1, [r0, #CCB_callevel_stat]

    UNLOCK_ALL_INT r1

    ldr   r1, [r0, #CCB_p_schedtsk] /* p_schedtskをp_runtskに */
    str   r1, [r0, #CCB_p_runtsk]
    cmp   r1, #0            /* p_runtskがNULLならdispatcher_1へ */
    beq   dispatcher_1
    ldr   sp, [r1,#TCB_sp]  /* タスクスタックを復帰 */
#ifdef LOG_DSP_LEAVE
    mov   r0, r1            /* p_runtskをパラメータに */
    mov   r4, r1            /* r1はスクラッチレジスタなので保存 */
    bl    log_dsp_leave
    mov   r1, r4
#endif /* LOG_DSP_LEAVE */
    /*
     *  PreTaskHookの呼び出し
     */
#ifdef CFG_USE_PRETASKHOOK
pretaskhook_call:
    mov   r4, r0            /* r0はスクラッチレジスタなので保存 */
    mov   r5, r1            /* r1はスクラッチレジスタなので保存 */
    ldr   r0, =call_pretaskhook
    bl    stack_change_and_call_func_1
    mov   r1, r5
    mov   r0, r4
#endif /* USE_PRETASKHOOK */

ALABEL(dispatcher_3)
    ldr   pc, [r1,#TCB_pc]  /* 実行再開番地を復帰 */

ALABEL(dispatcher_1)
    /*
     * CPUロック状態の解除と，非タスクコンテキスト実行状態への
     * 準備をする
     */
    ldr   sp, [r0,#CCB_ostkpt]         /* 非タスクコンテキストのスタックへ */
    add   r7, r0, #CCB_except_nest_cnt
    mov   r4, #0
    mov   r5, #1
ALABEL(dispatcher_2)
    /*
     *  割込みを許可し，非タスクコンテキスト実行状態とし割込みを待つ．
     *
     *  ここで非タスクコンテキスト実行状態に切り換えるのは，ここで発生
     *  する割込み処理にどのスタックを使うかという問題の解決と，割込み
     *  ハンドラ内でのタスクディスパッチの防止という2つの意味がある．
     *
     *  プロセッサを割込み待ちに移行させる処理と，割込み許可とは，不可
     *  分に行なう必要がある．
     *  これを不可分に行なわない場合，割込みを許可した直後に割込
     *  みが入り，その中でタスクが実行可能状態になると，実行すべきタス
     *  クがあるにもかかわらずプロセッサが割込み待ちになってしまう．
     *
     *  割込み待ちの間は，p_runtskをNULL（＝0）に設定しなければならな
     *  い．このように設定しないと，割込みハンドラからiget_tidを呼び出
     *  した際の動作が仕様に合致しなくなる．
     *
     *  ターゲットによっては，省電力モード等に移行するため，標準の方法と
     *  異なる手順が必要な場合がある．
     *  そのようなターゲットでは，ターゲット依存において，TOPPERS_CUSTOM_IDLE
     *  を定義し，アセンブラマクロとして，toppers_asm_custom_idle を用意
     *  すればよい．
     *
     *  なお，toppers_asm_custom_idle の記述にあたっては，次のレジスタは
     *  toppers_asm_custom_idleの前後で使用するため，
     *  toppers_asm_custom_idle 内で使用する場合は，前後で保存復帰すること．
     *  これらのレジスタは Calee saved レジスタであるため，
     *  toppers_asm_custom_idle として関数呼び出しをした場合は，呼び出した
     *  関数で自動的に保存復帰されるため，アセンブラレベルでの保存復帰は必
     *  要ない．
     *
     *  レジスタ : 内容
     *   r4      : '0'
     *   r5      : '1'
     *   r6      : reqflgのアドレス
     *   r7      : except_nest_cntのアドレス
     *   sp      : 非タスクコンテキスト用のスタックの先頭アドレス．
     */
    str   r5, [r7]                          /* except_nest_cnt = 1 */

#ifdef TOPPERS_CUSTOM_IDLE
    toppers_asm_custom_idle
#else
    OUTER_UNLOCK_OS_INT r6, r0
    nop
    OUTER_LOCK_OS_INT  r6, r0
#endif /* TOPPERS_CUSTOM_IDLE */
    my_ccb r0, r1         /* my_ccbを読み込む */
    ldr   r8, [r0,#CCB_p_runtsk]
    ldr   r6, [r0,#CCB_p_schedtsk]
    cmp   r8, r6
    beq   dispatcher_2    /* p_runtskとp_schedtskが同じなら */
    str   r4, [r7]        /* except_nest_cnt = 0 */

    b     dispatcher_0

    /*
     *  スタックオーバフロー時の処理
     */
#ifdef CFG_USE_STACKMONITORING
    /*
     *  スタックオーバフロー時プロテクションフックを呼び出し
     *  スタックを更に壊さないため，割込みスタックの初期値を使用する
     */
    .global stack_monitoring_error_isr
stack_monitoring_error_isr:
    my_ccb r0, r1
    OUTER_LOCK_OS_INT  r1, r0 			/* ISRから来た場合はOS割込み禁止になっていない */   

    .global stack_monitoring_error
stack_monitoring_error:
    my_ccb r0, r1
    str   r3, [r0, #CCB_except_nest_cnt] /* _ostkptを使う場合はexcept_nest_cntをインクリメントする */
    add   r3, r3, #1
    str   r3, [r0, #CCB_except_nest_cnt]
    ldr   sp, [r0,#CCB_ostkpt]            /* スタックを更に壊さないため，割込みスタックの初期値を使用する */

    mov   r0, #E_OS_STACKFAULT		 	/* プロテクションフックの引数を設定 */
    b     call_protectionhk_main		/* プロテクションフックを呼び出し */
    /*
     *  プロテクションフックからは戻ってこない
     */
#endif /* CFG_USE_STACKMONITORING */


/*
 *  タスク開始時処理
 *
 *  dispatcherから呼び出されるため，TCBのアドレスはr1に入っている
 */
    ATEXT
    AGLOBAL(start_r)
ALABEL(start_r)
    my_ccb r0, r2
    OUTER_UNLOCK_OS_INT r2, r0
    ldr   lr, = exit_task                    /* 戻り番地設定 */
    ldr   r2, [r1, #TCB_p_tinib]            /* p_runtsk->p_tinibをr2に  */
    ldrb  r4, [r2, #TINIB_exepri]           /* 実行開始時の優先度取得  */
    strb  r4, [r1, #TCB_curpri]             /* 現在の優先度に設定  */
    ldr   r1, [r2, #TINIB_task]             /* タスク起動番地にジャンプ */
    bx    r1

/*
 *  ディスパッチ要求用のプロセッサ間割込みのバイパス処理
 *
 *  共通部で用意されている ipi_handler を呼び出さずにディスパッチャを
 *  呼び出す．最も外側のルーチンでない場合は，reqflgをセットする．
 *  自CCBのアドレスがr0に入っていることを想定している．
 */
#ifdef USE_IPI_DIS_HANDER_BYPASS
    AGLOBAL(ret_int_ipi_dispatch)
ALABEL(ret_int_ipi_dispatch)
    ldr   r2, [r0,#CCB_except_nest_cnt] /* 戻り先がタスクコンテキストなら */
    cmp   r2, #0                         /* ret_int_4へ */
    beq   ret_int_4
    mov   r1, #1                         /* 戻り先が非タスクコンテキストなら */
    str   r1, [r0, #CCB_reqflg]          /* reqflgをtrueにしてリターン       */
    b     ret_int_2
#endif /* USE_IPI_DIS_HANDER_BYPASS */

/*
 *  CPU例外ハンドラ出口処理
 *
 *  ret_excは，CPU例外ハンドラから戻った直後に実行するルーチンで，
 *  CPU例外ハンドラ終了後，ターゲット依存の処理を実行した後，
 *  カーネル管理の割込みを禁止，スタックをCPU例外前のスタックに，
 *  r0にCPU例外発生時に取得しているスピンロックの値を入れた状態
 *  で呼び出される．
 */
    ATEXT
    AGLOBAL(ret_exc)
ALABEL(ret_exc)
    /*
     * CPU例外発生時にスピンロックを取得していれば，スピンロック
     * の解放は行わない．
     */
    cmp  r0, #0
    bne  ret_int

/*
 *  割込みハンドラ/CPU例外ハンドラ出口処理
 *
 *  ret_intは，割込みハンドラから戻った直後に実行するルーチンで，
 *  割込みハンドラ終了後，ターゲット依存の処理を実行した後，
 *  OS割込みを禁止，スタックを割込み前のスタックにした
 *  状態で呼び出される
 *  r3には復帰前のcallevel_statが入っている
 */
    AGLOBAL(ret_int)
ALABEL(ret_int)
    /*
     *  例外・割込みのネストカウント（except_nest_cnt)のデクリメント
     */
    my_ccb r0, r1
    ldr   r1, [r0,#CCB_except_nest_cnt] /* r1 <-except_nest_cnt */
    sub   r2, r1, #1
    str   r2, [r0,#CCB_except_nest_cnt]
    cmp   r2, #0                  /* 戻り先が非タスクコンテキストなら */
    bne   ret_int_2               /* すぐにリターン                   */

	/*
	 * C1ISR, CPU例外の後にタスクディスパッチはしない
	 */
	and   r3, r3, #TCL_ISR2
	cmp   r3, #TCL_ISR2
    bne   ret_int_2

    ldr   r1, [r0,#CCB_p_runtsk]
    ldr   r2, [r0,#CCB_p_schedtsk]
    cmp   r1, r2              /* p_runtskとp_schedtskが異なるならret_int_3へ */
    bne   ret_int_3

    AGLOBAL(ret_int_2)
ALABEL(ret_int_2)
    /*
     *  割込み処理からのリターンにより，CPUロック解除状態に移行しなければ
     *  ならない．
     *  ARMでは，CPSRのIRQビットでCPUロック解除状態とするため，単にリターン
     *  すればよい．
     */
#if (__TARGET_ARCH_ARM == 4) || (__TARGET_ARCH_ARM == 5)
    ldmfd sp!,{r1}              /* CPSRの復帰処理 */
    msr   spsr_cxsf, r1              /* 戻り先のcpsrをspsrに設定 */
    ldmfd sp!,{r0-r3,ip,lr,pc}^ /* コンテキストの復帰，^付きなので、cpsr <- spsr */
#else /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */
#if defined(SUPPORT_VFPv3_NEON)
    vpop    {d0-d7}             /* スクラッチレジスタの復帰 */
    pop     {R0}                /* FPSCRレジスタの復帰      */
    vmsr    fpscr, R0
#endif /* SUPPORT_VFPv3_NEON */
    ldmfd sp!,{r0-r3,ip,lr}
    rfeia sp!
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */

ALABEL(ret_int_3)
    /*
     *  ここでは，戻り先がタスクであり，スタックは，タスクスタックの上
     *  にスクラッチレジスタのみが保存された状態になっている．また，
     *  プロセッサは，スーパーバイザーモード・カーネル管理の割込みを禁止
     *  した状態となっている．
     */

    /*
     *  OS割込み禁止状態に移行する
     *
     *  この時点でOS割込み禁止状態とするのは，dispatcherへ分岐する時に，
     *  OS割込み禁止状態になっている必要があるためである
     */
    OUTER_LOCK_OS_INT  r1, r0
    UNLOCK_ALL_INT r1

ALABEL(ret_int_4)
    /*
     *  ディスパッチの必要性をチェック
     */
    ldr   r1, [r0,#CCB_p_runtsk]
    ldr   r2, [r0,#CCB_p_schedtsk]
    cmp   r1, r2              /* p_runtskとp_schedtskが同じなら */
    beq   ret_int_r_1         /*                    ret_int_r_1へ */
    /* 
     * スタックを8バイト境界に調整 
     */
    and   r2, sp, #4             
    sub   sp, sp, r2
    stmfd sp!, {r2, r3}   /* r3はスタック境界の調整用のダミー */

    stmfd sp!, {r4-r11}       /* 残りのレジスタを保存 -(8*4) */
#if defined(SUPPORT_VFPv3_NEON)
    /* 残りの浮動小数点/NEONレジスタを保存  */
    vpush   {d8-d15}
    vpush   {d16-d31}
#endif /* SUPPORT_VFPv3_NEON */
    str   sp, [r1,#TCB_sp]    /* タスクスタックを保存 */
    adr   r2, ret_int_r       /* 実行再開番地を保存   */
    str   r2, [r1,#TCB_pc]
    b     dispatcher

ALABEL(ret_int_r)
#if defined(SUPPORT_VFPv3_NEON)
    vpop    {d16-d31}
    vpop    {d8-d15}
#endif /* SUPPORT_VFPv3_NEON */
    ldmfd sp!, {r4-r11}       /* レジスタの復帰 +(8*4) */
    /* 
     *  8バイト境界に調整したスタックを元に戻す
     */
    ldmfd sp!, {r2, r3}   /* r3はスタック境界の調整用のダミー */
    add   sp, sp, r2
ALABEL(ret_int_r_1)
ALABEL(ret_int_r_2)
    /*
     *  割込み処理からのリターンにより，CPUロック解除状態に移行するよ
     *  うにする．
     */
    LOCK_ALL_INT r1
    OUTER_UNLOCK_OS_INT r1, r0
#if (__TARGET_ARCH_ARM == 4) || (__TARGET_ARCH_ARM == 5)
    ldmfd sp!, {r0}             /* spsr を復帰 */
    msr   spsr_cxsf,r0          /* 戻り先のcpsrをspsrに設定 */
    ldmfd sp!,{r0-r3,ip,lr,pc}^ /* タスクに復帰 ^付きなので、cpsr <- spsr */
#else /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */
#if defined(SUPPORT_VFPv3_NEON)
    vpop    {d0-d7}        /* スクラッチレジスタの復帰 */
    pop     {R0}           /* FPSCRレジスタの復帰 */
    vmsr    fpscr, R0
#endif /* SUPPORT_VFPv3_NEON */
    ldmfd sp!,{r0-r3,ip,lr}
    rfeia sp!
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */

/*
 * CPU例外ハンドラ
 *
 * CPU例外ハンドラは，非タスクコンテキストで実行する．
 *
 */
#if (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 7)
#define EXC_SPSR_OFFSET (7*4)
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 7) */

/*
 *  未定義命令 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(undef_handler)
ALABEL(undef_handler)
#if (__TARGET_ARCH_ARM == 4) || (__TARGET_ARCH_ARM == 5)
    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr,pc} /* pcはダミー */

    /*
     * spsrと戻り番地を取得するために未定義モードへ
     */
    msr   cpsr_cxsf, #(CPSR_UND AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr
#else /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */
    /*
     * lr(戻り先) spsr(cpsr_svc)をスーパーバイザーモードのスタックへ
     * 保存．
     */
    srsdb #CPSR_SVC!

    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr}
    ldr   r1, [sp, #EXC_SPSR_OFFSET]     /* spsr(例外発生時のcpsr)を取得 */
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */

    mov   r2, #EXCH_NO_UNDEF
    b     target_exc_handler

/*
 *  SWI 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(swi_handler)
ALABEL(swi_handler)
#if (__TARGET_ARCH_ARM == 4) || (__TARGET_ARCH_ARM == 5)
    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     *  元々スーパーバイザーモードだが，CPUロック状態とする
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr,pc} /* pcはダミー */
    mov   r0, lr
    mrs   r1, spsr
#else /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */
    /*
     * lr(戻り先) spsr(cpsr_svc)をスーパーバイザーモードのスタックへ
     * 保存．
     */
    srsdb #CPSR_SVC!

    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr}
    ldr   r1, [sp, #EXC_SPSR_OFFSET]     /* spsr(例外発生時のcpsr)を取得 */
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */

    mov   r2, #EXCH_NO_SWI
    b     target_exc_handler

/*
 *  プリフェッチアボード 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(prefetch_handler)
ALABEL(prefetch_handler)
#if (__TARGET_ARCH_ARM == 4) || (__TARGET_ARCH_ARM == 5)
    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr,pc} /* pcはダミー */

    /*
     * spsrと戻り番地を取得するためにアボートモードへ
     */
    msr   cpsr_cxsf, #(CPSR_ABT AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr
#else /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */
    /*
     * lr(戻り先) spsr(cpsr_svc)をスーパーバイザーモードのスタックへ
     * 保存．
     */
    srsdb #CPSR_SVC!

    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr}
    ldr   r1, [sp, #EXC_SPSR_OFFSET]     /* spsr(例外発生時のcpsr)を取得 */
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */

    mov   r2, #EXCH_NO_PABORT
    b     target_exc_handler

/*
 *  データアボード 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(data_abort_handler)
ALABEL(data_abort_handler)
#if (__TARGET_ARCH_ARM == 4) || (__TARGET_ARCH_ARM == 5)
    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr,pc} /* pcはダミー */

    /*
     * spsrと戻り番地を取得するためにアボートモードへ
     */
    msr   cpsr_cxsf, #(CPSR_ABT AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr
#else /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */
    /*
     * lr(戻り先) spsr(cpsr_svc)をスーパーバイザーモードのスタックへ
     * 保存．
     */
    srsdb #CPSR_SVC!

    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr}
    ldr   r1, [sp, #EXC_SPSR_OFFSET]     /* spsr(例外発生時のcpsr)を取得 */
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */

    mov   r2, #EXCH_NO_DABORT
    b     target_exc_handler


#ifndef TARGET_FIQ_HANDLER
/*
 *  FIQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(fiq_handler)
ALABEL(fiq_handler)
#if (__TARGET_ARCH_ARM == 4) || (__TARGET_ARCH_ARM == 5)
    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_FIQ_BIT AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr,pc} /* pcはダミー */

    /*
     * spsrと戻り番地を取得するためにFIQモードへ
     */
    msr   cpsr_cxsf, #(CPSR_FIQ AOR CPSR_FIQ_BIT AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    mov   r0, lr
    mrs   r1, spsr
#else /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */
    /*
     * lr(戻り先) spsr(cpsr_svc)をスーパーバイザーモードのスタックへ
     * 保存．
     */
    srsdb #CPSR_SVC!

    /*
     *  例外発生前の動作時モード(スーパーバイザーモード)へ
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr}
    ldr   r1, [sp, #EXC_SPSR_OFFSET]     /* spsr(例外発生時のcpsr)を取得 */
#endif /* (__TARGET_ARCH_ARM == 6) || (__TARGET_ARCH_ARM == 7) || (__TARGET_ARCH_ARM == 8) */

    mov   r2, #EXCH_NO_FIQ
    b     target_exc_handler
#endif /* TARGET_FIQ_HANDLER */

#ifdef __thumb__
    ATEXT
    AALIGN(2)
    AGLOBAL(current_sr)
ALABEL(current_sr)
    mrs   r0, cpsr
    bx    lr

   AGLOBAL(set_sr)
ALABEL(set_sr)
    msr   cpsr_cxsf, r0
    bx    lr
#endif /* __thumb__ */

    .text
    .align 2
    .global stack_change_and_call_func_1
    .global stack_change_and_call_func_2
stack_change_and_call_func_1:
stack_change_and_call_func_2:
    /*
     * 割込み発生時のコンテキストを判定
     */
	push  {r4, lr}
    my_ccb r3, r4
    ldr   r4, [r3, #CCB_except_nest_cnt]
    add   r4, r4, #1
    str   r4, [r3, #CCB_except_nest_cnt]
    cmp   r4, #1
    bne   1f

	mov   r4, sp
	ldr   sp, [r3,#CCB_ostkpt]
	push  {r4}
1:    
	mov   r3, r0
	mov   r0, r1
	mov   r1, r2
	mov   lr, pc
	bx    r3

    my_ccb r3, r4
    ldr   r4, [r3, #CCB_except_nest_cnt]
    sub   r4, r4, #1
    str   r4, [r3, #CCB_except_nest_cnt]
    cmp   r4, #0
    bne   1f

	pop   {r3}
	mov   sp, r3

1:
	pop   {r4, lr}
	bx    lr

    /*
     *  フックの呼び出し
     *  フックの強制終了を備える
     */
    ATEXT
    AALIGN(2)
    AGLOBAL(call_trusted_hook)
ALABEL(call_trusted_hook)
    stmfd  sp!, {r4-r12,lr}    /* スクラッチレジスタ以外のレジスタの保存  -(9*4) */

    my_ccb r2, r3
    ldrh  r3, [r2,#CCB_callevel_stat]
    ldr   r4, [r2,#CCB_except_nest_cnt]
    ldr   r5, [r2,#CCB_trusted_hook_savedsp]
    stmfd  sp!, {r3-r5}       /* -(3*4) */

    mov   r2, r0 /* 引数の積み直し */
    mov   r0, r1

    mov   lr,  pc
    bx    r2

    ldmfd sp!, {r3-r5}      /* スタックを捨てる */


    /*
     *  フックの強制終了
     */
    ATEXT
    AALIGN(2)
    AGLOBAL(exit_trusted_shutdown_hook)
ALABEL(exit_trusted_shutdown_hook)
    my_ccb r2, r3
    ldr   sp, [r2,#CCB_trusted_hook_savedsp]
    ldmfd sp!, {r3-r5}      /* +(3*4) */
    strh  r3, [r2,#CCB_callevel_stat]
    str   r4, [r2,#CCB_except_nest_cnt]
    str   r5, [r2,#CCB_trusted_hook_savedsp]

    ldmfd  sp!, {r4-r12,lr}    /* スクラッチレジスタ以外のレジスタの復帰  +(9*4) */

    bx    lr

