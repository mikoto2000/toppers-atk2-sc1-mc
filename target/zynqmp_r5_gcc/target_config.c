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
 *  $Id: target_config.c 849 2017-09-29 13:34:39Z ertl-honda $
 */

/*
 *		ターゲット依存モジュール（ZYNQMP_R5用）
 */

#include "kernel_impl.h"
#include "mc.h"
#include "arm_gcc/common/core_sil.h"
#include "target_sysmod.h"
#include "target_ici.h"
#ifdef ENABLE_RETURN_MAIN
#include "interrupt.h"
#endif /* ENABLE_RETURN_MAIN */
#ifdef TOPPERS_ENABLE_TRACE
#include "logtrace/trace_config.h"
#endif /* TOPPERS_ENABLE_TRACE */

#include "xuart.h"

/*
 *  UARTのベースアドレス
 */
const uint32	target_xuart_base_table[TotalNumberOfCores] = {
	XUART0_BASE,
#if TotalNumberOfCores > 1
	XUART1_BASE,
#endif /* TotalNumberOfCores > 1 */
};

/*
 *  コア間割込み番号
 */
const InterruptNumberType
target_ici_intno_table[TotalNumberOfCores] = {
	INTNO_ICI0,
#if TotalNumberOfCores > 1
	INTNO_ICI1,
#endif /* TotalNumberOfCores > 1 */
};

/*
 *  コア間割込み優先度
 */
const PriorityType
target_ici_intpri_table[TotalNumberOfCores] = {
	-(INTPRI_ICI0),
#if TotalNumberOfCores > 1
	-(INTPRI_ICI1),
#endif /* TotalNumberOfCores > 1 */
};

/*
 *  起動時同期用の変数
 */
volatile uint32 core_state_table[TNUM_HWCORE];

/*
 *  文字列の出力
 */
void
target_fput_str(const char8 *c)
{
	while (*c != '\0') {
		xuart_putc(*c);
		c++;
	}
	xuart_putc('\n');
}

#if 0
/*
 *  Execute on QEMU?
 */
static bool_t
is_qemu(void) {
	return(((sil_rew_mem((void *)(XPAR_CSU_BASEADDR + XPAR_CSU_VER_OFFSET)) >> 12U) & XPLAT_INFO_MASK)
		   == XPLAT_ZYNQ_ULTRA_MPQEMU);
}
#endif

/*
 * Initilize MPU Region
 */
static void
init_mpu_region(void) {
	uint32_t base, size, attr, number = 0;

	mpu_disable_allregion();

	/*
	 *  DDR
	 */
	base = 0x00000000;
	size = RSAE_2G;
	attr = RAC_AP_PRW_URW | RAC_OI_WB_WA;
	mpu_set_region(base, size, number++, attr);

#if TotalNumberOfCores >= 2
	/*
	 * デュアルコアモードではデータ領域をキャッシュOFFとする
	 * MPUは領域番号が大きい方の設定が有効となる 
	 */
	base = DDR_DATA_START;
	size = DDR_DATA_SIZE;
//	attr = RAC_AP_PRW_URW | RAC_OI_NCACHE | RAC_S;
	attr = RAC_AP_PRW_URW | RAC_OI_WB_WA | RAC_S;
	mpu_set_region(base, size, number++, attr);
#endif /* TotalNumberOfCores >= 2 */

	/*
	 *  PL :0x80000000 - 0xBFFFFFFF
	 */
	base = 0x80000000;
	size = RSAE_1G;
	attr = RAC_AP_PRW_URW | RAC_STRONGO_SHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * QSPI : 0xC0000000 - 0xDFFFFFFF
	 */
	base = 0xC0000000;
	size = RSAE_512M;
	attr = RAC_AP_PRW_URW | RAC_DEV_NSHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * PCIe Low : 0xE0000000 - 0xEFFFFFFF
	 */
	base = 0xE0000000;
	size = RSAE_256M;
	attr = RAC_AP_PRW_URW | RAC_DEV_NSHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * STM CoreSight : 0xF8000000 - 0xF8FFFFFF
	 */
	base = 0xF8000000;
	size = RSAE_16M;
	attr = RAC_AP_PRW_URW | RAC_DEV_NSHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * RPU_A53_GIC : 0xF9000000 - 0xF90FFFFF
	 */
	base = 0xF9000000;
	size = RSAE_16M;
	attr = RAC_AP_PRW_URW | RAC_DEV_NSHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * FPS slaves : 0xFD000000 - 0xFDFFFFFF
	 */
	base = 0xFD000000;
	size = RSAE_16M;
	attr = RAC_AP_PRW_URW | RAC_DEV_NSHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * Upper LPS : 0xFE000000 - 0xFEFFFFFF
	 */
	base = 0xFE000000;
	size = RSAE_16M;
	attr = RAC_AP_PRW_URW | RAC_DEV_NSHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * Lower LPS : 0xFF000000 - 0xFFFFFFFF
	 */
	base = 0xFF000000;
	size = RSAE_16M;
	attr = RAC_AP_PRW_URW | RAC_DEV_NSHAR;
	mpu_set_region(base, size, number++, attr);

	/*
	 * OCM RAM : 0xFFFC0000 - 0xFFFFFFFF
	 */
	base = 0xFFFC0000;
	size = RSAE_256K;
	attr = RAC_AP_PRW_URW | RAC_OI_WB_WA  | RAC_S;
//	attr = RAC_AP_PRW_URW | RAC_OI_NCACHE | RAC_S;
	mpu_set_region(base, size, number++, attr);
}

void
hardware_init_hook(void)
{
	icache_disable();
	dcache_disable();
	mpu_disable();
	bp_disable();
	tcm_ecc_disable();
	cache_parity_disable();
	icache_invalidate();
	/* Invalidate Dcache */
	Asm("mcr p15, 0, %0, c15, c5, 0"::"r"(0));

	/* Initialize for LockStep mode */
	if ((sil_rew_mem((void*)RPU_RPU_GLBL_CNTL) & RPU_RPU_GLBL_CNTL_SLSPLIT_MASK) == 0) {
		sil_wrw_mem((void*)RPU_RPU_ERR_INJ,
					sil_rew_mem((void*)RPU_RPU_ERR_INJ)|RPU_RPU_ERR_INJ_FAULTLOGENABLE);
	}

	init_mpu_region();

	bp_enable();
	icache_enable();
	dcache_enable();
	mpu_enable();

	set_high_vector(false);
}

/*
 *  core_support.S
 */
extern void vector_table(void);
extern void vector_ref_tbl_end(void);

extern const uint32 isr_table[];

/*
 *  コアを起動させる
 *   有効なコアIDであればTRUEを返す
 */
boolean
x_start_core(CoreIdType coreid)
{
	boolean valid_core;
	int loop;

	if (coreid < TNUM_HWCORE) {
		/*
		 *  PowerUp
		 */
		/* R5 and TCM split mode */
		sil_wrw_mem((void*)RPU_RPU_GLBL_CNTL,
					(sil_rew_mem((void*)RPU_RPU_GLBL_CNTL) | RPU_RPU_GLBL_CNTL_SLSPLIT_MASK)
					& ~(RPU_RPU_GLBL_CNTL_TCM_COMB_MASK | RPU_RPU_GLBL_CNTL_SLCLAMP_MASK));

		/* R5-1 in Halt and setp vector is 0x00000000(ATCM) */
		sil_wrw_mem((void*)RPU_RPU_1_CFG,
					(sil_rew_mem((void*)RPU_RPU_1_CFG) & ~(RPU_RPU_1_CFG_NCPUHALT_MASK|RPU_RPU_1_CFG_VINITHI_MASK)));

		/* Clock Enable */
		sil_wrw_mem((void*)CRL_APB_CPU_R5_CTRL,
					(sil_rew_mem((void*)CRL_APB_CPU_R5_CTRL) | CRL_APB_CPU_R5_CTRL_CLKACT_MASK));

		/* Release Reset */
		sil_wrw_mem((void*)CRL_APB_RST_LPD_TOP,
					(sil_rew_mem((void*)CRL_APB_RST_LPD_TOP) &
					 ~(CRL_APB_RST_LPD_TOP_RPU_R51_RESET_MASK|CRL_APB_RST_LPD_TOP_RPU_AMBA_RESET_MASK)));

		/*
		 * Copy Vector
		 */
		for(loop = 0; loop < ((uint32_t*)vector_ref_tbl_end - (uint32_t*)vector_table); loop++) {
			*((uint32_t*)PSU_R5_1_ATCM_S_AXI_BASEADDR + loop) = *((uint32_t*)vector_table + loop);
		}

		{
			volatile int i;
			for(i = 0; i < 1000000; i++);
		}

		/* Release Halt and start from 0x00000000(ATCM) */
		sil_wrw_mem((void*)RPU_RPU_1_CFG,
					(sil_rew_mem((void*)RPU_RPU_1_CFG) | RPU_RPU_1_CFG_NCPUHALT_MASK));
		core_state_table[coreid] = MAGIC_START;
		valid_core = TRUE;
	}
	else {
		valid_core = FALSE;
	}
	return(valid_core);
}

/*
 *  コアが停止しているかを返す
 */
boolean
is_halt(CoreIdType coreid)
{
	if(sil_rew_mem((void*)CRL_APB_RST_LPD_TOP) & CRL_APB_RST_LPD_TOP_RPU_R51_RESET_MASK) {
		return TRUE;
	}
	return FALSE;
}


/*
 *  str_ker() の前でマスタプロセッサで行う初期化
 */
void
target_hardware_initialize(void)
{
	/*
	 *  GICのDICの初期化
	 */
	gicd_initialize();

	/*
	 *  SILのスピンロック用変数の初期化
	 */
	TOPPERS_spn_var = 0U;
}

/*
 *  ターゲット依存の初期化
 */
void
target_initialize(void)
{
	uint32_t id = x_core_id();

	/*
	 *  ARM依存の初期化（これより前はget_my_xxxが動作しない）
	 */
	core_initialize();

	/*
	 *  Initialize GIC Distributer Interface
	 */
	gicc_initialize();

	/*
	 *  Initialize GIC CPU Interface SGI/PPI
	 */
	gicd_sgi_ppi_init();

	/*
	 *  割込みハンドラテーブルへのポインタの初期化
	 */
	(get_my_p_ccb())->target_ccb.p_isr_tbl = isr_table[id];

	/*
	 *  Initialize the UART. If we are using a
	 *  global logger (shared by all cores) only
	 *  the master processor (mprc) initializes it.
	 */
	InitHwSerial();

#ifdef TOPPERS_ENABLE_TRACE
	/*
	 *  トレースログ機能の初期化(マスタコアのみ実施)
	 */
	if (coreid == OS_CORE_ID_MASTER) {
		trace_initialize((uintptr) (TRACE_AUTOSTOP));
	}
#endif /* TOPPERS_ENABLE_TRACE */

	x_config_int(GET_ICI_INTNO(id), ENABLE, GET_ICI_INTPRI(id), id);
}

/*
 *  ターゲット依存の終了処理
 */
void
target_exit(void)
{
#ifdef ENABLE_RETURN_MAIN
	CCB	*p_ccb = get_my_p_ccb();
#endif /* ENABLE_RETURN_MAIN */

	/* コアが停止していることを示す */
	core_state_table[x_core_id()] = 0U;

#ifdef TOPPERS_ENABLE_TRACE
	/*
	 *  トレースログのダンプ(マスタコアのみ実施)
	 */
	if (x_core_id() == OS_CORE_ID_MASTER) {
		trace_dump(&target_fput_log);
	}
#endif /* TOPPERS_ENABLE_TRACE */

#ifndef ENABLE_RETURN_MAIN
	/*
	 *  シャットダウン処理の出力
	 */
	target_fput_str("Kernel Exit...");
#else
	target_fput_str("Kernel Shutdown...");
#endif /* ENABLE_RETURN_MAIN */

	/*
	 *  ARM依存の終了処理
	 */
	core_terminate();

#ifdef ENABLE_RETURN_MAIN
	/* ToDo */
	p_ccb->kerflg = FALSE;
	p_ccb->sus_all_cnt = 0U;
	p_ccb->sus_os_cnt = 0U;
	p_ccb->target_ccb.except_nest_cnt = 0U;
	p_ccb->target_ccb.nested_lock_os_int_cnt = 0U;

	/* 全コアが終了されるのを同期する */
	barrier_sync(5U, FALSE);

	/* 全コアのOSスタックを初期に戻す */
	Asm("rdctl r3, cpuid");
	Asm("srli  r3, r3, 20");
	Asm("subi  r3, r3, 0x20");
	Asm("slli  r3, r3, 2");
	Asm("movhi r4, %hiadj(ostkpt_table)");
	Asm("addi  r4, r4, %lo(ostkpt_table)");
	Asm("add   r2, r4, r3");
	Asm("ldw   sp, 0(r2)");

	/* マスタコアは起動状態のままmain関数へ戻る */
	if (x_core_id() == OS_CORE_ID_MASTER) {
		/* 起動しているOS管理のコア数を1へ戻す */
		activated_cores = 1U;
		Asm("call  main");
	}
	else {
		/*
		 * スレーブコアは次のマスタコアからのStartCoreで
		 * コアが起動されるまで無限ループして待機する(slave_start相当)
		 */
		while (is_halt(x_core_id())) {
		}
		;
		Asm("call main");
	}
#endif /* ENABLE_RETURN_MAIN */

	while (1) {
	}
}

/*
 *  ターゲット依存の文字出力
 */
void
target_fput_log(char8 c)
{
	if (c == '\n') {
		xuart_putc('\r');
	}
	xuart_putc(c);
}

/*
 *  特定の割込み要求ラインの有効/無効を制御可能かを調べる処理
 */
boolean
target_is_int_controllable(InterruptNumberType intno)
{
	/*
	 *  Nios2では全ての割込み要求ラインに対して
	 *  有効/無効を制御可能であるため，常にtrueを返す
	 */
	return(TRUE);
}

/*
 *	コア間割込みの実体定義
 */
ISR(target_ici_handler0)
{
	ici_handler_main();
}

#if TotalNumberOfCores > 1
ISR(target_ici_handler1)
{
	ici_handler_main();
}
#endif /* TotalNumberOfCores > 1 */
