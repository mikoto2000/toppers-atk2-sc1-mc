/*
 *  TOPPERS ATK2
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Automotive Kernel Version 2
 *
 *  Copyright (C) 2015 by Center for Embedded Computing Systems
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
 *  $Id: target_config.h 780 2017-03-07 01:32:17Z ertl-honda $
 */

/*
 *		ターゲット依存モジュール（ZYNQMP_R5用）
 *
 *  カーネルのターゲット依存部のインクルードファイル
 *  kernel_impl.hのターゲット依存部の位置付けとなる
 */

#ifndef TOPPERS_TARGET_CONFIG_H
#define TOPPERS_TARGET_CONFIG_H

#include "zynqmp_r5.h"
#include "target_ici.h"

#define TMAX_NATIVE_SPN	256

#define MAGIC_START	UINT_C(0x87654321) /* 同期用のマジックナンバー */

/*
 *  トレースログに関する設定
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "logtrace/trace_config.h"
#endif /* TOPPERS_ENABLE_TRACE */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  コアを起動させる
 *   有効なコアIDであればTRUEを返す
 */
extern boolean x_start_core(CoreIdType coreid);

/*
 *  コアが停止しているかを返す
 */
extern boolean is_halt(CoreIdType coreid);


extern void target_hardware_initialize(void);

/*
 *  ターゲットシステム依存の初期化
 */
extern void target_initialize(void);

/*
 *  ターゲットシステムの終了
 *  システムを終了する時に使う
 */
extern void target_exit(void) NoReturn;

/*
 *  文字列の出力
 */
extern void target_fput_str(const char8 *c);

/*
 *  特定の割込み要求ラインの有効/無効を制御可能かを調べる処理
 */
extern boolean target_is_int_controllable(InterruptNumberType intno);

/*
 *  起動時同期用の変数
 */
extern volatile uint32 core_state_table[];

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  コア依存モジュール（ARM用）
 */
#include "arm_gcc/common/core_config.h"

/*
 *  GIC依存モジュール
 */
#include "arm_gcc/common/gic_kernel.h"

#endif /* TOPPERS_TARGET_CONFIG_H */
