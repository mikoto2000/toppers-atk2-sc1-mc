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
 *  @(#) $Id: gic_support.S 849 2017-09-29 13:34:39Z ertl-honda $
 */

/*
 *  ターゲット依存モジュール アセンブリ言語部（ARM用）
 */

#define  TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)	(val)		/* ulong_t型の定数を作るマクロ */
#define GIC_REG(base, offset)	((base) + (offset))
#include "kernel_impl.h"
#include "offset.h"
#include "target_asm.inc"
#include "target_ici.h"


/*
 * 割込みハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(irq_handler)
ALABEL(irq_handler)
    /*
     * IRQモードで実行される
     */
    sub   lr, lr, #4 /* 戻り先アドレス  */

    /*
     * lr(戻り先) spsr(cpsr_svc)をスーパーバイザーモードのスタックへ
     * 保存．
     */
    srsdb #CPSR_SVC!

    /*
     *  割込み前のモード(スーパーバイザーモード)へ
     *  移行し，コンテキストを保存する
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_OSLOCK AOR CPSR_ALWAYS_SET)
    stmfd sp!, {r0-r3,ip,lr}
#if defined(SUPPORT_VFPv3_NEON)
    fmrx    r0, fpscr
    push    {r0}
    vpush   {d0-d7} /* スクラッチレジスタの保存 */
#endif /* SUPPORT_VFPv3_NEON */

    /*
     *  CCBの取得
     */
    my_ccb r0, r3
    mov   lr, sp         /* この時点のスタックを復帰のため取得 */

    /*
     *  割り込み要因の判定．
     */
    ldr   r2, =(GICC_IAR)
    ldr   r12, [r2]
    mov   r1, r12, lsl #22  /* 下位10ビット以外をクリア */
    mov   r1, r1,  lsr #22

   /*
    *  spurious割込み(INT_ID=1023(0x03ff))の場合，割込み処理を終了
    */
   ldr   r2, =(0x03ff)
   cmp   r2, r1
   beq   ret_int_2

#ifdef TOPPERS_SAFEG_SECURE
   /*
    *  TrustZoneを用いるときは，INT_ID=1022(0x03fe)も
    *  spurious割込みとなる．
    */
   ldr   r2, =(0x03fe)
   cmp   r2, r1
   beq   ret_int_2
#endif /* TOPPERS_SAFEG_SECURE */

    /*
     *  多重割込みか判定
     */
    ldr   r3, [r0, #CCB_except_nest_cnt]  /* 例外・割込みネスト回数を取得 */
    add   r2, r3, #1                       /* 例外・割込みネスト回数を更新 */
    str   r2, [r0, #CCB_except_nest_cnt]
    cmp   r3, #0

    /*
     *  多重割込みの場合はスタックを8バイト境界に調整
     */
    andne r2, sp, #4
    subne sp, sp, r2

    /*
     * 一段目の例外（割込み）ならスタックを変更する
     */
    ldreq sp, [r0, #CCB_ostkpt]

    stmfd sp!, {lr}     /* 復帰用のスタックポインタの保存 -4 */

    /*
     *  タスクスタックのオーバフローチェック
     *   r0  : ccbへのポインタ
     *   r1  : 割込み番号
     *   r12 : GICC_IARから読み込んだ値
     *   lr  : 割込み元のスタックポインタの値
     */
#ifdef CFG_USE_STACKMONITORING  
    /*
     *  多重割込みの場合はタスクスタックのスタックモニタリングを行わない
     */
    ldr   r2, [r0, #CCB_except_nest_cnt] /* 例外・割込みネスト回数を取得 */
    cmp   r2, #1
    bne   int_nested

    /*
     *  スタックポインタチェック方式
     */
    ldr   r3, [r0, #CCB_p_runtsk]		/* p_runtskを読み込む */
    ldr   r2, [r3, #TCB_p_tinib]		/* タスク初期化ブロック先頭アドレス取得 */
    ldr   r2, [r2, #TINIB_stk]			/* タスクスタック先頭アドレス取得 */
    cmp   lr, r2						/* スタックポインタ(lr) <= スタックの先頭の場合， */
    ble   stack_monitoring_error_isr	/* スタックオーバーフロー発生 */

    /*
     *  マジックナンバーチェック方式
     */
    ldr   r3, [r2]						/* タスクスタックの先頭アドレスからマジックナンバー領域の値取得 */
    ldr   r2, =STACK_MAGIC_NUMBER
    cmp   r3, r2						/* マジックナンバーが破壊されていない場合， */
    beq   stack_monitoring_exit			/* 割込み処理に飛ぶ */
    b     stack_monitoring_error_isr	/* マジックナンバーが破壊されていた場合，スタックオーバーフロー発生 */

    /*
     *  多重割込みの場合
     */
int_nested:

    /*
     *  割込みスタックのオーバフローチェック
     */
    /*
     *  スタック残量チェック方式
     */
    ldr   r3, =isr_p_isrcb_table
    my_core_id r2
    ldr   r3, [r3, r2, lsl #2]					/* 実行するISRの管理ブロックを取得 */ 
    ldr   r3, [r3, r1, lsl #2]
    ldr   r3, [r3, #INTINIB_remain_stksz]		/* 割込み番号に対応したスタックサイズの初期化アドレス取得 */
    ldr   lr, =ostk_table
    ldr   lr, [lr, r2, lsl #2]					/* 割込みスタックの先頭アドレス取得 */
    add   r3, r3, lr							/* 先頭アドレス＋ISRの使用するスタックサイズ */
    cmp   sp, r3								/* SP <= 先頭アドレス＋ISRの使用するスタックサイズ の場合NG  */
    ble   stack_monitoring_error_isr			/* スタック残量が不足している場合，スタックオーバーフロー検出 */

    /*
     *  マジックナンバーチェック方式
     */
    ldr   r2, [lr]						/* タスクスタックの先頭アドレスからマジックナンバー領域の値取得 */
    ldr   lr, =STACK_MAGIC_NUMBER
    cmp   r2, lr						/* マジックナンバーが破壊されていた場合， */
    bne   stack_monitoring_error_isr	/* スタックオーバーフロー発生 */
stack_monitoring_exit:
#endif /* CFG_USE_STACKMONITORING */

    /*
     *  割込み要因の割込み優先度を求め(モデル上の)割込み優先度マスクをセット
     *  する．またその際，ハンドラ実行前の(モデル上の)割込み優先度マスクを
     *  保存する．
     */
    ldr   lr, =(GICC_RPR)
    ldr   r2, [lr]
    ldr   r3, =(GICC_PMR) /* 割込み発生前の割込み優先度マスクを取得 */
    ldr   lr, [r3]
    str   r2, [r3]   /* 新しい割込み優先度マスクをセットする */
    stmfd sp!, {lr}  /* 割込み発生前の割込み優先度マスクをスタックに保存 -8 */

    stmfd sp!,{r12}      /* GICC_IAR から取得した内容を保存  -12 */
    stmfd sp!,{r1}       /* inhno を保存  -16 */

    /*
     * 割り込み許可
     */
    msr   cpsr_cxsf, #(CPSR_SVC AOR CPSR_ALWAYS_SET)

    /*
     *  callevel 保存
     */
    ldrh  r2, [r0, #CCB_callevel_stat]
    stmfd sp!, {r2} 

    /*
     *  callevel 設定
     */
    orr    r2, r2, #TCL_ISR2
    strh   r2, [r0, #CCB_callevel_stat]

    /*
     *  p_currentosap 保存
     */
    ldr   r3, [r0, #CCB_p_currentosap]
    stmfd sp!, {r3}

    /*
     *  p_runisr 保存
     */
    ldr   r3, [r0, #CCB_p_runisr]
    stmfd sp!, {r3}

    /*
     *  p_runisr 設定
     */
    ldr   r3, =isr_p_isrcb_table
    my_core_id r2
    ldr   r3, [r3, r2, lsl #2] /* 実行するISRの管理ブロックを取得 */ 
    ldr   r2, [r3, r1, lsl #2]
    str   r2, [r0, #CCB_p_runisr]

    /*
     *  p_currentosap設定
     */
    cmp   r2, #0	/* p_runisr が NULL でなければ設定 */
    beq   int_handler_call
    ldr   r3, [r2, #ISRCB_p_isrinib]
    ldr   r3, [r3, #ISRINIB_p_osapinib]
    str   r3, [r0, #CCB_p_currentosap]

#ifdef TOPPERS_SAFEG_LOAD
    stmfd sp!,{r0,r1}  /* r1はスタック境界調整のためのダミー */
    bl safeg_load_int
    ldmfd sp!,{r0,r1}  /* r1はスタック境界調整のためのダミー */
#endif /* TOPPERS_SAFEG_LOAD */

ALABEL(int_handler_call)
    /*
     * 割込みハンドラの呼び出し
     */

    /*
     *  割込みハンドラのアドレスを取得
     */
    ldr   r3, [r0, #CCB_p_isr_tbl]   /* 割込みハンドラテーブルの読み込み */
    ldr   r0, [r3, r1, lsl #2]       /* r0<-割込みハンドラ */

    mov   lr,  pc
    bx    r0

ALABEL(target_ret_int)
	/*
	 *  割込みスタックのオーバフローチェック
	 *  割込みから戻った時，スタックポインタも戻ったはずなので，
	 *  マジックナンバーチェック方式のみ実施
	 */
#ifdef CFG_USE_STACKMONITORING
    /*
     *  マジックナンバーチェック方式
     */
    my_core_id r2
    ldr   r1, =ostk_table
    ldr   r1, [r1, r2, lsl #2]			/* 割込みスタックの先頭アドレス取得 */
    ldr   r2, [r1]						/* タスクスタックの先頭アドレスからマジックナンバー領域の値取得 */
    ldr   r1, =STACK_MAGIC_NUMBER
    cmp   r2, r1						/* マジックナンバーが破壊されていた場合， */
    bne   stack_monitoring_error_isr	/* スタックオーバーフロー発生 */
#endif /* CFG_USE_STACKMONITORING */

	my_ccb r0, r3
	ldr    r2, [r0, #CCB_p_runisr]
	cmp   r2, #0
	beq   skip_exit_isr2

    /*
     * C2ISRの不正終了チェック
     */
	bl    exit_isr2

ALABEL(skip_exit_isr2)
    my_ccb r0, r3

    /*
     *  p_runisr 復帰
     */
    ldmfd sp!,{r2}
    str   r2, [r0, #CCB_p_runisr]

    /*
     *  p_currentosap 復帰
     */
    ldmfd sp!,{r2}
    str   r2, [r0, #CCB_p_currentosap]

    /*
     *  callevel_stat の復帰
     */
    ldrh  r3, [r0, #CCB_callevel_stat]  /* ret_int で使用するため */
    ldmfd sp!,{r2}
    strh  r2, [r0, #CCB_callevel_stat]

    ldmfd sp!,{r0}       /* inhno を復帰  +4 */
    ldmfd sp!,{r12}      /* GICC_IAR から取得した内容を取得  +8 */

    ldr   r1,=GICC_EOIR
    str   r12,[r1]               /* EOIレジスタへの書き込み */

    /*
     * 割込み優先度マスクを元に戻す
     */
    ldmfd sp!,{r1}                       /* 元の割込み優先度マスクを取得 +12 */
    ldr   r0, =(GICC_PMR)           /* DICにセット */
    str   r1, [r0]

    /*
     * スタックポインタの復帰
     */
    ldmfd sp!,{r2}              /* 元のスタックポインタを取得 */
    mov   sp, r2

    LOCK_ALL_INT r1

    /*
     * 後の処理はARM依存部で実行
     */
    b     ret_int


/*
 * ターゲット依存の例外入口処理
 * ARMv6/7はARMv4と呼び出し時条件が異なるため注意すること．
 * スーパーバイザーモードで呼び出される．
 */
    ATEXT
    AGLOBAL(target_exc_handler)
ALABEL(target_exc_handler)
    /*
     *  この時点のレジスタの内容は以下の通り
     *    r1 : spsr
     *    r2 : 例外番号
     */
#if defined(SUPPORT_VFPv3_NEON)
    fmrx    r0, fpscr
    push    {r0}
    vpush   {d0-d7} /* スクラッチレジスタの保存 */
#endif /* SUPPORT_VFPv3_NEON */

    mov   lr, sp         /* この時点のスタックを復帰のため取得 */

    ldr   r0, =(GICC_PMR) /* 例外発生前の割込み優先度マスクを取得 */
    ldr   r3, [r0]
    asr   r3, r3, #GIC_PRI_SHIFT /* 外部表現に変更 */
    sub   r3, r3, #(GIC_PRI_LEVEL - 1)
    stmfd sp!, {r3}  /* 例外発生前の割込み優先度マスクをスタックに保存 */

    /*
     *  コンテキスト判定のため，except_nest_cnt をスタックに保存．
     *  スタックに保存せず，現在のexcept_nest_cntを-1すると取得できるが，
     *  スタックに積んでおいた方がデバッグ等が行いやすいので，スタックに
     *  保存する．
     */
    my_ccb r0, r3
    ldr   r3, [r0, #CCB_except_nest_cnt]
    stmfd sp!, {r3}

    mov   r3,  sp         /* 例外フレーム番地を保存 */

    /*
     * カーネル管理外のCPU例外か判定する
     *
     * カーネル管理外のCPU例外は，カーネル実行中，全割込みロック状態，
     * CPUロック状態，カーネル管理外の割込みハンドラ実行中に発生した
     * CPU例外である．ARMの場合は，SPSRのI/Fフラグのいずれかが'1'なら，
     * 該当する．
     */
    tst   r1, #(CPSR_FIQ_BIT AOR CPSR_IRQ_BIT)
    bne   target_kernel_unc_exc_handler /* カーネル管理外のCPU例外の処理へ */

    /*
     *  例外発生時のコンテキストを判定
     */
    ldr   r1, [r0, #CCB_except_nest_cnt]
    add   r1, r1, #1
    str   r1, [r0, #CCB_except_nest_cnt]
    cmp   r1, #1

    /*
     *  多重割込みの場合はスタックを8バイト境界に調整
     */
    andne r2, sp, #4
    subne sp, sp, r2

    /*
     * タスクコンテキストでCPU例外が発生した場合
     * スタックを非タスクコンテキストに切り替える．
     */
    ldreq sp, [r0, #CCB_ostkpt]

ALABEL(target_exc_handler_1)
    stmfd sp!, {lr}     /* 復帰用のスタックポインタの保存 +4 */

    /*
     * 割込み発生前の割込み優先度マスクをスタックに保存
     * 割込みハンドラと出口ルーチンを共有するために保存
     */
    ldr   r12, =(GICC_PMR) /* 例外発生前の割込み優先度マスクを取得 */
    ldr   r1, [r12]
    stmfd sp!, {r1}  /* 例外発生前の割込み優先度マスクをスタックに保存 +8 */

    /* 
     * プロテクションフックはOS割込み禁止状態で実行する
     */
    OUTER_LOCK_OS_INT  r1, r0
    UNLOCK_ALL_INT r1

    /*
     *  カーネル起動していない場合に起きたCPU例外は，無限ループへ
     */
    ldr   r3, [r0, #CCB_kerflg]
    cmp   r3, #0
    beq   infinity_loop

    /*
     * CPU例外が発生した場合，OSはE_OS_PROTECTION_EXCEPTIONをパラメータとして
     * プロテクションフックを呼び出す
     */
    mov   r0, #E_OS_PROTECTION_EXCEPTION 	/* call_protectionhk_mainの引数設定 */
    bl    call_protectionhk_main		/* プロテクションフックの呼び出し */

    /*
     * 割込み優先度マスクを元に戻す
     */
    ldmfd sp!,{r1}                  /* 元の割込み優先度マスクを取得 */
    ldr   r0, =(GICC_PMR)           /* DICにセット */
    str   r1, [r0]

    /*
     * スタックポインタの復帰
     */
    ldmfd sp!,{r2}              /* 元のスタックポインタを取得 */
    mov   sp, r2

    LOCK_ALL_INT r1
    my_ccb r0, r3
    OUTER_UNLOCK_OS_INT r1, r0

    /*
     *  後の処理はARM依存部で実行
     *  r3はret_intで参照する
     */
    ldrh  r3, [r0, #CCB_callevel_stat]
    b     ret_int

/*
 * カーネル管理外のCPU例外の出入口処理
 */
ALABEL(target_kernel_unc_exc_handler)
    stmfd sp!, {r1, lr}     /* 復帰用のスタックポインタの保存 +8(r1はダミー) */

    /*
     * 割込み発生時のコンテキストを判定
     */
    ldr   r1, [r0, #CCB_except_nest_cnt]
    add   r1, r1, #1
    str   r1, [r0, #CCB_except_nest_cnt]
    cmp   r1, #1

    /*
     *  多重割込みの場合はスタックを8バイト境界に調整
     */
    andne r2, sp, #4
    subne sp, sp, r2

    /*
     * タスクコンテキストでCPU例外が発生した場合
     * スタックを非タスクコンテキストに切り替える．
     */
    ldreq sp, [r0, #CCB_ostkpt]

   /*
    * システム状態（コンテキストは除く）を，CPU例外発生時の状態へ
    */
    and   r1, r1, #(CPSR_IRQ_BIT AOR CPSR_FIQ_BIT)
    orr   r1, r1, #(CPSR_SVC AOR CPSR_ALWAYS_SET)
    msr   cpsr_cxsf, r1

    /* 
     * プロテクションフックはOS割込み禁止状態で実行する
     */
    OUTER_LOCK_OS_INT  r1, r0
    UNLOCK_ALL_INT r1

    /*
     *  カーネル起動していない場合に起きたCPU例外は，無限ループへ
     */
    ldr   r3, [r0, #CCB_kerflg]
    cmp   r3, #0
    beq   infinity_loop

    /*
     * CPU例外が発生した場合，OSはE_OS_PROTECTION_EXCEPTIONをパラメータとして
     * プロテクションフックを呼び出す
     */
    mov   r0, #E_OS_PROTECTION_EXCEPTION 	/* call_protectionhk_mainの引数設定 */
    bl    call_protectionhk_main			/* プロテクションフックの呼び出し */

    LOCK_ALL_INT r1
    my_ccb r0, r3
    OUTER_UNLOCK_OS_INT r1, r0

    /*
     *  例外・割込みのネストカウント（except_nest_cnt)のデクリメント
     */
    my_ccb r0, r3
    ldr   r1, [r0, #CCB_except_nest_cnt]
    sub   r1, r1, #1
    str   r1, [r0, #CCB_except_nest_cnt]

    /*
     * スタックポインタの復帰
     */
    ldmfd sp!, {r1,r2}    /* 元のスタックポインタ(r2)を取得，r1はスタック境界調整のためのダミー +8 */
    mov   sp, r2

    /*
     * CPU例外からの復帰
     */
#if defined(SUPPORT_VFPv3_NEON)
    vpop    {d0-d7}             /* スクラッチレジスタの復帰 */
    pop     {r0}                /* FPSCRレジスタの復帰      */
    vmsr    fpscr, r0
#endif /* SUPPORT_VFPv3_NEON */
    ldmfd sp!,{r0-r3,ip,lr}
    rfeia sp!

/*
 *  無限ループ処理
 */
infinity_loop:
	b infinity_loop
