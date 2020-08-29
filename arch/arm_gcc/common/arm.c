/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
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
 *  @(#) $Id: arm.c 780 2017-03-07 01:32:17Z ertl-honda $
 */

#include "kernel_impl.h"

#ifdef __TARGET_PROFILE_A
#if __TARGET_ARCH_ARM >= 6

/*
 *  Dキャッシュを開始
 */
void
dcache_enable(void)
{
	uint32_t bits;

	CP15_CONTROL_READ(bits);

	/* すでにONならリターン */
	if (bits & CP15_CONTROL_C_BIT){
		return;
	}

	dcache_invalidate();

	CP15_DATA_SYNC_BARRIER();
	CP15_PBUFFER_FLUSH();

	bits |= CP15_CONTROL_C_BIT;
	CP15_CONTROL_WRITE(bits);
}

/*
 *  Dキャッシュを停止して無効とする．
 *  CA9では，Dキャッシュが無効な状態でClean and Invalidate()を実行すると，
 *  暴走するため，Dキャッシュの状態を判断して，無効な場合は，Invalidate
 *  のみを行う．
 */
void
dcache_disable(void)
{
	uint32_t bits;

	CP15_CONTROL_READ(bits);
	if( bits & CP15_CONTROL_C_BIT ){
		dcache_clean_and_invalidate();
		outer_cache_flush();
		bits &= ~(CP15_CONTROL_C_BIT|CP15_CONTROL_M_BIT);
		CP15_CONTROL_WRITE(bits);
	}
	else{
		dcache_invalidate();
	}

	CP15_DATA_SYNC_BARRIER();
	CP15_PBUFFER_FLUSH();
}

/*
 * Iキャッシュの開始
 */
void icache_enable(void)
{
	uint32_t bits;

	CP15_CONTROL_READ(bits);

	/*
	 *  すでに有効ならリターン
	 */
	if((bits & (CP15_CONTROL_I_BIT|CP15_CONTROL_Z_BIT))
	   == (CP15_CONTROL_I_BIT|CP15_CONTROL_Z_BIT)){
		return;
	}

	icache_invalidate();
	CP15_BRANCHP_INVALIDATE();

	CP15_DATA_SYNC_BARRIER();
	CP15_PBUFFER_FLUSH();

	bits |= CP15_CONTROL_I_BIT|CP15_CONTROL_Z_BIT;
	CP15_CONTROL_WRITE(bits);
}

/*
 *  Iキャッシュを停止
 */
void
icache_disable(void)
{
	uint32_t bits;

	CP15_CONTROL_READ(bits);
	bits &= ~CP15_CONTROL_I_BIT;
	CP15_CONTROL_WRITE(bits);

	icache_invalidate();

	CP15_DATA_SYNC_BARRIER();
	CP15_PBUFFER_FLUSH();
}

/*
 *  I/Dキャッシュを両方を有効に
 */
void
cache_enable(void)
{
	dcache_enable();
	icache_enable();
}

/*
 *  I/Dキャッシュを両方を無効に
 */
void
cache_disable(void)
{
	icache_disable();
	dcache_disable();
}
#endif /* __TARGET_ARCH_ARM >= 6 */

#if __TARGET_ARCH_ARM >= 7

#define DCACHE_INVAL_ALL        1
#define DCACHE_CLEAN_ALL        2
#define DCACHE_CLEAN_INVAL_ALL  3

Inline void
dcache_setway(uint32_t level, uint32_t num_sets,
				 uint32_t num_ways, uint32_t log2_line_len,
				 uint32_t op)
{
	uint32_t way_shift;
	int way, set;
	uint32_t setway;
	uint32_t log2_num_ways;
	uint32_t tmp;

	/* word から byte 単位に変換 */
	log2_line_len += 2;

	log2_num_ways = -1;
	for(tmp = num_ways; tmp; tmp = tmp >> 1){
		log2_num_ways++;
	}
	way_shift = (32 - log2_num_ways);

	for (way = 0; way < num_ways; way++) {
		for (set = 0; set < num_sets; set++) {
			setway = (level << 1) | (set << log2_line_len) | (way << way_shift);
			if (op == DCACHE_INVAL_ALL) {
				CP15_DCACHE_INVALIDATE(setway);
			} else if (op == DCACHE_CLEAN_ALL) {
				CP15_DCACHE_CLEAN(setway);
			} else if (op == DCACHE_CLEAN_INVAL_ALL) {
				CP15_DCACHE_CLEAN_AND_INVALIDATE(setway);
			}
		}
	}
	CP15_DATA_SYNC_BARRIER();
}

static bool_t
is_dcache(uint32_t level)
{
	uint32_t clidr;
	uint32_t type;

	CP15_CLIDR_READ(clidr);

	type = (clidr >> (CLIDR_CTYPE_BITWIDTH*level)) & CLIDR_CTYPE_MASK;
	if ((type == CLIDR_CTYPE_DATA_ONLY) ||
		(type == CLIDR_CTYPE_INSTRUCTION_DATA) ||
		(type == CLIDR_CTYPE_UNIFIED)) {
		return true;
	}
	return false;
}


static void
dcache_getinfo(uint32_t level, uint32_t *p_num_sets, uint32_t *p_num_ways,
					 uint32_t *p_log2_line_len)
{
	uint32_t ccsidr;

	CP15_CSSELR_WRITE((level << 1)| CSSELR_IND_DATA_UNIFIED);
	CP15_CCSIDR_READ(ccsidr);

	*p_log2_line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >> CCSIDR_LINE_SIZE_OFFSET) + 2;
	*p_num_ways  = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK) >> CCSIDR_ASSOCIATIVITY_OFFSET) + 1;
	*p_num_sets  = ((ccsidr & CCSIDR_NUM_SETS_MASK) >> CCSIDR_NUM_SETS_OFFSET) + 1;
}

void
invalidate_dcache_all(void)
{
	uint32_t level, num_sets, num_ways, log2_line_len;

	for (level = 0; level < CLIDR_MAX_CTYPE; level++) {
		if(is_dcache(level)) {
			dcache_getinfo(level, &num_sets, &num_ways, &log2_line_len);
			dcache_setway(level, num_sets, num_ways, log2_line_len, DCACHE_INVAL_ALL);
		}
	}
}

void
clean_invalidate_dcache_all(void)
{
	uint32_t level, num_sets, num_ways, log2_line_len;

	for (level = 0; level < CLIDR_MAX_CTYPE; level++) {
		if(is_dcache(level)) {
			dcache_getinfo(level, &num_sets, &num_ways, &log2_line_len);
			dcache_setway(level, num_sets, num_ways, log2_line_len, DCACHE_CLEAN_INVAL_ALL);
		}
	}
}

void
clean_dcache_all(void)
{
	uint32_t level, num_sets, num_ways, log2_line_len;

	for (level = 0; level < CLIDR_MAX_CTYPE; level++) {
		if(is_dcache(level)) {
			dcache_getinfo(level, &num_sets, &num_ways, &log2_line_len);
			dcache_setway(level, num_sets, num_ways, log2_line_len, DCACHE_CLEAN_ALL);
		}
	}
}
#endif /* __TARGET_ARCH_ARM >= 7 */

/*
 *  MMU関連のドライバ
 */

/*
ARMv7のSectionテーブルの仕様
  31:20 : Section base address
  19    : NS
  17    : nG
  16    :  S
  15    : AP[2]
  14:12 : TEX[2:0]
  11:10 : AP[1:0]
  8:5   : Domain
  4     : XN
  3     : C
  2     : B
  0     : PXN

 */

#define DOMAIN_NO  1
#define PAGE_TABLE_SIZE  0x00004000 /* 4KB */

/*
 * ページテーブル
 */
static uint8_t PageTable[TotalNumberOfCores][PAGE_TABLE_SIZE]  __attribute__ ((aligned (0x4000)));

#define my_PageTable (PageTable[x_core_id()])

#define SIZE_1M    0x00100000

/*
 * 指定に従い,メモリのマッピングを行う
 * マッピングの単位は1MB
 */
void
mmu_map_memory(MEMORY_ATTRIBUTE *m_attribute)
{
	uint32_t size;
	uintptr_t va;
	uintptr_t pa;
	uint32_t ttb;
	uint32_t *sptr;
	uint32_t fix_val;

	size = m_attribute->size;
	va   = m_attribute->va;
	pa   = m_attribute->pa;

	/* MB単位になるようにサイズを設定 */
	size = (size + 0x000FFFFF) & ~0x000FFFFF;

	/*
	 * ページテーブルのベースアドレスを取得
	 */
	CP15_TTB0_READ(ttb);

	fix_val =  (m_attribute->s << 16) | (m_attribute->tex << 12) | (m_attribute->ap << 10)
		        | (m_attribute->c << 3) | (m_attribute->b << 2) | (1 << 1);
#if __TARGET_ARCH_ARM >= 7
	fix_val |= (m_attribute->ns << 19);
#endif /* __TARGET_ARCH_ARM >= 7 */
	sptr  = (uint32_t *)((ttb & 0xFFFFC000) | (((va & 0xFFF00000) >> 20) << 2));

	while(size > 0) {
		*sptr++ = pa | fix_val;
		va   += SIZE_1M;
		pa   += SIZE_1M;
		size -= SIZE_1M;
	}
}

/*
 *  MMUの初期化
 */
void
mmu_init(void)
{
	uint32_t bits = 0;

	/*
	 * Disabling MMU
	 */
	CP15_CONTROL_READ(bits);
	bits &= ~CP15_CONTROL_M_BIT;
	CP15_CONTROL_WRITE(bits);

	/*
	 * Invalidate ALL TLB
	 */
	CP15_DATA_SYNC_BARRIER();
	CP15_TLB_INVALIDATE_ALL();
	CP15_DATA_SYNC_BARRIER();
	pbuffer_flash();

	/*
	 * TTBR0を用いる様に指定
	 */
	CP15_TTBCR_WRITE(0);

	/*
	 * 変換テーブル(TT)として，PageTableを使用する
	 * Sharedビットをセット
	 */
#if __TARGET_ARCH_ARM == 6
	CP15_TTB0_WRITE((((uint32_t)my_PageTable)|CP15_TTB0_RGN_S|CP15_TTB0_RGN_WBWA));
#else /* __TARGET_ARCH_ARM == 7 */
	CP15_TTB0_WRITE((((uint32_t)my_PageTable)|CP15_TTB0_RGN_S|CP15_TTB0_RGN_WBWA|CP15_TTB0_IRGN_WBWA));
#endif /* __TARGET_ARCH_ARM == 7 */

	/* プリフェッチバッファをクリア */
	pbuffer_flash();

	/*
	 *  ターゲット依存部でのMMUの初期化
	 */
	target_mmu_init();

	/* プリフェッチバッファをクリア */
	pbuffer_flash();

	/*
	 * ドメイン番号をセット
	 */
	CP15_DOMAINS_WRITE(DOMAIN_NO);

	/*
	 * CONTROLコプロセッサの Mビット，XPビットをセットして，
	 * MMUを有効にする
	 */
	CP15_CONTROL_READ(bits);
	bits |= CP15_CONTROL_M_BIT | CP15_CONTROL_XP_BIT;
	CP15_CONTROL_WRITE(bits);
}
#endif /* __TARGET_PROFILE_A */

#ifdef __TARGET_PROFILE_R

/*
 *  全MPUを無効とする
 */
void
mpu_disable_allregion(void)
{
	uint32_t index, num, tmp;

	CP15_MPUIR_READ(num);
	for(index = 0; index < num; index++) {
		CP15_RGNR_WRITE(index);
		CP15_RSAE_READ(tmp);
		tmp &= (~RSAE_EN);
		CP15_DATA_SYNC_BARRIER();
		CP15_RSAE_WRITE(tmp);
		CP15_DATA_SYNC_BARRIER();
		CP15_PBUFFER_FLUSH();
	}
}

/*
 *  MPUのリージョンの属性を設定
 */
void
mpu_set_region(uint32_t base, uint32_t size, uint32_t number, uint32_t attr)
{
	uint32_t tmp = (size << RSAE_RS_OFFSET) | RSAE_EN;

	CP15_DATA_SYNC_BARRIER();
	CP15_RGNR_WRITE(number);
	CP15_PBUFFER_FLUSH();
	CP15_RBA_WRITE(base);
	CP15_RAC_WRITE(attr);
	CP15_RSAE_WRITE(tmp);
	CP15_DATA_SYNC_BARRIER();
}
#endif /* __TARGET_PROFILE_R */
