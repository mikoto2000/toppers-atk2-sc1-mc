/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2016 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: target_support.S 780 2017-03-07 01:32:17Z ertl-honda $
 */

/*
 *  ターゲット依存モジュール アセンブリ言語部（ARM用）
 */

#define  TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)	(val)		/* ulong_t型の定数を作るマクロ */
#include "kernel_impl.h"
#include "arm.h"
#include "target_asm.inc"

    ATEXT
    AALIGN(2)
    AGLOBAL(_init_reg)
ALABEL(_init_reg)
	/* 
	 * Initialization for Lock-step 
	 */
	mov	r0,#0
	mov	r1,#0
	mov	r2,#0
	mov	r3,#0
	mov	r4,#0
	mov	r5,#0
	mov	r6,#0
	mov	r7,#0
	mov	r8,#0
	mov	r9,#0
	mov	r10,#0
	mov	r11,#0
	mov	r12,#0

	msr cpsr_cxsf, #(CPSR_IRQ AOR CPSR_ALLLOCK)
	mov r13,#0
	mov r14,#0

	msr cpsr_cxsf, #(CPSR_SVC AOR CPSR_ALLLOCK)
	mov r13,#0
	mov r14,#0

	msr cpsr_cxsf, #(CPSR_ABT AOR CPSR_ALLLOCK)
	mov r13,#0
	mov r14,#0

	msr cpsr_cxsf, #(CPSR_FIQ AOR CPSR_ALLLOCK)
	mov r8, #0
	mov r9, #0
	mov r10,#0
	mov r11,#0
	mov r12,#0
	mov r13,#0
	mov r14,#0

	msr cpsr_cxsf, #(CPSR_UND AOR CPSR_ALLLOCK)
	mov r13,#0
	mov r14,#0

	msr cpsr_cxsf, #(CPSR_SYS AOR CPSR_ALLLOCK)
	mov r13,#0
	mov r14,#0

	/* Enable access to CP10, CP11 */
	ldr r0, =(0xf << 20)
	mcr p15, 0, r0, c1, c0, 2 /* Write CPACR cp11/cp10*/
	isb sy

	/* Enable FPU */
	mov r0, #0x40000000
	vmsr FPEXC, r0
	isb sy

	mov  r1,#0
	vmov d0,r1,r1
	vmov d1,r1,r1
	vmov d2,r1,r1
	vmov d3,r1,r1
	vmov d4,r1,r1
	vmov d5,r1,r1
	vmov d6,r1,r1
	vmov d7,r1,r1
	vmov d8,r1,r1
	vmov d9,r1,r1
	vmov d10,r1,r1
	vmov d11,r1,r1
	vmov d12,r1,r1
	vmov d13,r1,r1
	vmov d14,r1,r1
	vmov d15,r1,r1

    b _startup

#if 0
/* Disable MPU and caches */
        mrc     p15, 0, r0, c1, c0, 0       	/* Read CP15 Control Register*/
        bic     r0, r0, #0x05               	/* Disable MPU (M bit) and data cache (C bit) */
        bic     r0, r0, #0x1000             	/* Disable instruction cache (I bit) */
        dsb                                 	/* Ensure all previous loads/stores have completed */
        mcr     p15, 0, r0, c1, c0, 0       	/* Write CP15 Control Register */
        isb                                 	/* Ensure subsequent insts execute wrt new MPU settings */

/* Disable Branch prediction, TCM ECC checks */
        mrc     p15, 0, r0, c1, c0, 1       	/* Read ACTLR */
        orr     r0, r0, #(0x1 << 17)        	/* Enable RSDIS bit 17 to disable the return stack */
        orr     r0, r0, #(0x1 << 16)        	/* Clear BP bit 15 and set BP bit 16:*/
        bic     r0, r0, #(0x1 << 15)        	/* Branch always not taken and history table updates disabled*/
        bic     r0, r0, #(0x1 << 27)		/* Disable B1TCM ECC check */
        bic     r0, r0, #(0x1 << 26)		/* Disable B0TCM ECC check */
        bic     r0, r0, #(0x1 << 25)		/* Disable ATCM ECC check */
	orr	r0, r0, #(0x1 << 5)		/* Enable ECC with no forced write through with [5:3]=b'101*/
	bic 	r0, r0, #(0x1 << 4)
	orr	r0, r0, #(0x1 << 3)
        mcr     p15, 0, r0, c1, c0, 1       	/* Write ACTLR*/
	dsb				    	/* Complete all outstanding explicit memory operations*/

/* Invalidate caches */
	mov	r0,#0				/* r0 = 0  */
	dsb
	mcr	p15, 0, r0, c7, c5, 0		/* invalidate icache */
	mcr 	p15, 0, r0, c15, c5, 0      	/* Invalidate entire data cache*/
	isb

/* enable fault log for lock step */
	ldr	r0,=RPU_GLBL_CNTL
	ldr	r1, [r0]
	ands	r1, r1, #0x8
/* branch to initialization if split mode*/
	bne 	init
/* reset the debug logic */
	ldr	r0,=RST_LPD_DBG
	ldr	r1, [r0]
	orr	r1, r1, #(0x1 << 1)
	orr	r1, r1, #(0x1 << 4)
	orr	r1, r1, #(0x1 << 5)
	str	r1, [r0]
/* enable fault log */
	ldr	r0,=RPU_ERR_INJ
	ldr	r1,=fault_log_enable
	ldr	r2, [r0]
	orr	r2, r2, r1
	str	r2, [r0]
	nop
	nop

init:
	bl 	Init_MPU		/* Initialize MPU */

/* Enable Branch prediction */
	mrc     p15, 0, r0, c1, c0, 1       /* Read ACTLR*/
        bic     r0, r0, #(0x1 << 17)        /* Clear RSDIS bit 17 to enable return stack*/
        bic     r0, r0, #(0x1 << 16)        /* Clear BP bit 15 and BP bit 16:*/
        bic     r0, r0, #(0x1 << 15)        /* Normal operation, BP is taken from the global history table.*/
        orr	r0, r0, #(0x1 << 14)	    /* Disable DBWR for errata 780125 */
	mcr     p15, 0, r0, c1, c0, 1       /* Write ACTLR*/

/* Enable icahce and dcache */
	mrc 	p15,0,r1,c1,c0,0
	ldr	r0, =0x1005
	orr 	r1,r1,r0
	dsb
	mcr	p15,0,r1,c1,c0,0		/* Enable cache  */
	isb					/* isb	flush prefetch buffer */

/* Warning message to be removed after 2016.1 */
/* USEAMP was introduced in 2015.4 with ZynqMP and caused confusion with USE_AMP */
#ifdef USEAMP
#warning "-DUSEAMP=1 is deprecated, use -DVEC_TABLE_IN_OCM instead to set vector table in OCM"
#endif

/* Set vector table in TCM/LOVEC */
#ifndef VEC_TABLE_IN_OCM
	mrc	p15, 0, r0, c1, c0, 0
	mvn	r1, #0x2000
	and	r0, r0, r1
	mcr	p15, 0, r0, c1, c0, 0
#endif

/* enable asynchronous abort exception */
	mrs	r0, cpsr
	bic	r0, r0, #0x100
	msr	cpsr_xsf, r0
#endif
