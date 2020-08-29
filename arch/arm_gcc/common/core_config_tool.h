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
 *  @(#) $Id: core_config_tool.h 780 2017-03-07 01:32:17Z ertl-honda $
 */

/*
 *  チップ依存モジュールのツール依存部（ARM用）
 *
 *  カーネルのチップ依存部のインクルードファイルのツール依存部．
 *  chip_config.h からインクルードされる．
 */

#ifndef TOPPERS_CORE_CONFIG_TOOL_H
#define TOPPERS_CORE_CONFIG_TOOL_H

#ifndef TOPPERS_MACRO_ONLY

#if __TARGET_ARCH_ARM >= 6

/*
 *  スピンロック関連のコード
 */
/*
 *  ロック取得解放関数
 */

/*
 *  1段目のロック取得（タスク・非タスクコンテキスト共用）
 */
LOCAL_INLINE void
x_acquire_lock(LockType *p_lock)
{
	int locked;

	while(true) {
		Asm("   mov      r2, #0x01   \n"
			"\t ldrex    r1, [%1]    \n"
			"\t cmp      r1, #0x00   \n"
			"\t msrne    cpsr_c, %2  \n" /* 割込みの許可(bit0-7のみ書き換え) */
			#ifndef TOPPERS_OMIT_USE_WFE
			"\t wfene                \n"
			#endif /* TOPPERS_OMIT_USE_WFE */
			"\t msrne    cpsr_c, %3  \n" /* 割込みの禁止(bit0-7のみ書き換え) */
			"\t strexeq  r1, r2,[%1] \n"
			"\t mov      %0, r1      \n"
			:"=r"(locked)
			:"r"(p_lock),"I"(CPSR_SVC|CPSR_ALWAYS_SET),"I"(CPSR_SVC|CPSR_OSLOCK|CPSR_ALWAYS_SET)
			:"r1","r2", "cc");

		if(locked == 0){
			/* ロック取得成功 */
			CP15_DATA_MEMORY_BARRIER();
			Asm("":::"memory");
			return;
		}
	}
}


/*
 *  ロックの解放（タスク・非タスクコンテキスト共用）
 */
LOCAL_INLINE void
x_release_lock(LockType *p_lock)
{
	Asm("":::"memory");
	CP15_DATA_MEMORY_BARRIER();
	*p_lock = 0;
	CP15_DATA_MEMORY_BARRIER();
	Asm("sev");
}

/*
 *  ロックの取得試行（タスク・非タスクコンテキスト共用）
 */
LOCAL_INLINE boolean
x_try_lock(LockType *p_lock)
{
	int locked;

	Asm("   mov      r2, #0x01   \n"
		"\t ldrex    r1, [%1]    \n"
		"\t cmp      r1, #0x00   \n"
		"\t strexeq  r1, r2,[%1] \n"
		"\t mov      %0, r1      \n"
		:"=r"(locked):"r"(p_lock):"r1","r2", "cc");

	if (locked == 0) {
		/* 成功した場合 */
		CP15_DATA_MEMORY_BARRIER();
		Asm("":::"memory");
		return(true);
	}

	return(false);

}


#if TTYPE_SPN == NATIVE_SPN

/*
 *  スピンロックの初期化
 */
LOCAL_INLINE void
x_initialize_spin(SpinlockIdType i, LockType *p_spn_lock)
{
	*p_spn_lock = 0;
}

#else /* TTYPE_SPN != NATIVE_SPN */

#error TTYPE_SPN can be NATIVE_SPN.

#endif /* TTYPE_SPN == NATIVE_SPN */

#endif /* __TARGET_ARCH_ARM >= 6 */
#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_CONFIG_TOOL_H */
