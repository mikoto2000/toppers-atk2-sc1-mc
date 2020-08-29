$
$  TOPPERS ATK2
$      Toyohashi Open Platform for Embedded Real-Time Systems
$      Automotive Kernel Version 2
$
$  Copyright (C) 2016 by Center for Embedded Computing Systems
$              Graduate School of Information Science, Nagoya Univ., JAPAN
$
$  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
$  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
$  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
$  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
$      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
$      スコード中に含まれていること．
$  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
$      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
$      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
$      の無保証規定を掲載すること．
$  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
$      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
$      と．
$    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
$        作権表示，この利用条件および下記の無保証規定を掲載すること．
$    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
$        報告すること．
$  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
$      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
$      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
$      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
$      免責すること．
$
$  本ソフトウェアは，AUTOSAR（AUTomotive Open System ARchitecture）仕
$  様に基づいている．上記の許諾は，AUTOSARの知的財産権を許諾するもので
$  はない．AUTOSARは，AUTOSAR仕様に基づいたソフトウェアを商用目的で利
$  用する者に対して，AUTOSARパートナーになることを求めている．
$
$  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
$  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
$  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
$  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
$  の責任を負わない．
$
$  $Id: target.tf 849 2017-09-29 13:34:39Z ertl-honda $
$

$
$     パス2のターゲット依存テンプレート（ZYNQMP_R5用）
$

$ 
$  有効な割込み番号の定義
$ 
$INTNO_VALID = { 
	0x10000,0x10001,...,0x1001f;
	0x20000,0x20001,...,0x2001f;
	0xFFFF0020,0xFFFF0021,...,0xFFFF00bb
}$

$ 
$  制御可能な割込み番号
$ 
$INTNO_CONTROLLABLE  = {
	0x10000,0x10001,...,0x1000f;0x1001b,0x1001c,...,0x1001f;
	0x20000,0x20001,...,0x2000f;0x2001b,0x2001c,...,0x2001f;
	0xFFFF0020,0xFFFF0021,...,0xFFFF00bb
}$

$
$  CRE_ISR2で使用できる割込み番号
$
$INTNO_CREISR2_VALID = INTNO_VALID$

$ 
$  コア間割込み番号の定義
$ 
$	//コア0,コア1
$INTNO_ICI_LIST = {0x10000,0x20000}$

$ 
$  コア間割込み優先度の定義
$ 
$	//コア0,コア1
$INTPRI_ICI_LIST = {INTPRI_ICI0,INTPRI_ICI1}$

$ 
$ 
$ コア0に関する指定
$ 

$  オブジェクトロック
$ P_LOCKの場合に使用するオブジェクトロックを持つコアのIDを指定
$CORE_OBJ_LOCK[0] = 0$

$  CCBのセクション指定
$CORE_SECTION_CCB[0]  = "__attribute__((section(\"._kernel_core0s_ccb\"), nocommon))"$

$  非タスクコンテキスト用のスタックのセクション指定
$CORE_SECTION_OSTK[0] = "__attribute__((section(\"._kernel_core0p_osstack\"), nocommon))"$

$  タスクスタックのセクション指定
$CORE_SECTION_TSKSTK[0] = "__attribute__((section(\"._kernel_core0p_tstack\"), nocommon))"$

$  タスクコントロールブロックのセクション指定
$CORE_SECTION_TSKCB[0] = "__attribute__((section(\"._kernel_core0s_tskcb\"), nocommon))"$

$  オブジェクトコントロールブロックのセクション指定
$CORE_SECTION_OBJCB[0] = "__attribute__((section(\"._kernel_core0s_objcb\"), nocommon))"$


$ 
$ コア1に関する指定
$ 
$CORE_OBJ_LOCK[1] = 1$

$CORE_SECTION_CCB[1]  = "__attribute__((section(\"._kernel_core1s_ccb\"), nocommon))"$
$CORE_SECTION_OSTK[1] = "__attribute__((section(\"._kernel_core1p_osstack\"), nocommon))"$
$CORE_SECTION_TSKSTK[1] = "__attribute__((section(\"._kernel_core1p_tstack\"), nocommon))"$
$CORE_SECTION_TSKCB[1] = "__attribute__((section(\"._kernel_core1s_tskcb\"), nocommon))"$
$CORE_SECTION_OBJCB[1] = "__attribute__((section(\"._kernel_core1s_objcb\"), nocommon))"$

$ 
$  オブジェクト関連のメモリの配置先指定
$ 

$ 
$  CCBの配置先指定
$ 
$FUNCTION GENERATE_CCB$
	CCB kernel_core$+ARGV[1]$_ccb $CORE_SECTION_CCB[ARGV[1]]$ = {0, FALSE, 0, 0, 0, 0, 0};$NL$
$END$

$ 
$  OSSTACKの配置先指定
$ 
$FUNCTION GENERATE_OSSTACK$
	static StackType kernel_core$+ARGV[1]$_ostack[$FORMAT("COUNT_STK_T(0x%xU)", +ARGV[2])$] $CORE_SECTION_OSTK[ARGV[1]]$;$NL$
$END$

$ 
$  OSSTACKの配置先指定
$ 
$FUNCTION GENERATE_OSSTACK_DEF$
	static StackType kernel_core$+ARGV[1]$_ostack[COUNT_STK_T($OSTK.STKSZ[OSTK.COREID[ARGV[1]]]$)] $CORE_SECTION_OSTK[ARGV[1]]$;$NL$
$END$

$ 
$  タスクスタックの配置先指定
$ 
$FUNCTION GENERATE_TSKSTK$
	static StackType kernel_stack_$ARGV[1]$[COUNT_STK_T($TSK.STKSZ[ARGV[1]]$)] $CORE_SECTION_TSKSTK[OSAP.CORE[TSK.OSAPID[ARGV[1]]]]$;$NL$
$END$

$ 
$  基本タスクの共有スタックの配置先指定
$ 
$FUNCTION GENERATE_SHARED_TSKSTK$
	static StackType kernel_shared_stack_$ARGV[2]$[COUNT_STK_T($ARGV[3]$)] $CORE_SECTION_TSKSTK[ARGV[1]]$;$NL$
$END$

$ 
$  TCBの配置先指定
$ 
$FUNCTION GENERATE_TCB$
 	TCB kernel_tcb_$ARGV[1]$$TAB$$CORE_SECTION_TSKCB[OSAP.CORE[TSK.OSAPID[ARGV[1]]]]$;$NL$
$END$

$ 
$  CNTCBの配置先指定
$ 
$FUNCTION GENERATE_CNTCB$
	CNTCB kernel_cntcb_$ARGV[1]$$TAB$$CORE_SECTION_OBJCB[OSAP.CORE[CNT.OSAPID[ARGV[1]]]]$;$NL$
$END$

$ 
$  ALMCBの配置先指定
$ 
$FUNCTION GENERATE_ALMCB$
	ALMCB kernel_almcb_$ARGV[1]$$TAB$$CORE_SECTION_OBJCB[OSAP.CORE[ALM.OSAPID[ARGV[1]]]]$;$NL$
$END$

$ 
$  SCHTBLCBの配置先指定
$ 
$FUNCTION GENERATE_SCHTBLCB$
	SCHTBLCB kernel_schtblcb_$ARGV[1]$$TAB$$CORE_SECTION_OBJCB[OSAP.CORE[SCHTBL.OSAPID[ARGV[1]]]]$;$NL$
$END$

$ 
$  RESCBの配置先指定
$ 
$FUNCTION GENERATE_RESCB$
	RESCB kernel_rescb_$ARGV[1]$$TAB$$CORE_SECTION_OBJCB[RES.CORE[ARGV[1]]]$;$NL$
$END$

$ 
$  ISRCBの配置先指定
$ 
$FUNCTION GENERATE_ISRCB$
	ISRCB kernel_isrcb_$ARGV[1]$$TAB$$CORE_SECTION_OBJCB[OSAP.CORE[ISR.OSAPID[ARGV[1]]]]$;$NL$
$END$

$ 
$  コア依存テンプレートのインクルード（ARM用）
$ 
$INCLUDE"../../arch/arm_gcc/common/core.tf"$

$ 
$  GIC用の記述
$ 
$INCLUDE"../../arch/arm_gcc/common/gic_kernel.tf"$
