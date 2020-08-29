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
 *  $Id: xuart.c 780 2017-03-07 01:32:17Z ertl-honda $
 */

/*
 *		シリアルI/Oデバイス（SIO）ドライバ（XUART用）
 */

#include "Os.h"
#include "target_serial.h"

#define XUART_CR		0x0000U
#define XUART_MR		0x0004U
#define XUART_IER		0x0008U
#define XUART_IDR		0x000CU
#define XUART_IMR		0x0010U
#define XUART_ISR		0x0014U
#define XUART_BAUDGEN	0x0018U
#define XUART_RXTOUT	0x001CU
#define XUART_RXWM		0x0020U
#define XUART_MODEMCR	0x0024U
#define XUART_MODEMSR	0x0028U
#define XUART_SR		0x002CU
#define XUART_FIFO		0x0030U
#define XUART_BAUDDIV	0x0034U
#define XUART_FLOWDEL	0x0038U
#define XUART_TXWM		0x0044U
#define XUART_RXBS		0x0048U

#define XUART_SR_RXEMPTY	0x00000002U
#define XUART_SR_TXFULL		0x00000010U

#define XUART_INT_TEMPTY 0x08U
#define XUART_INT_RTRIG  0x01U

extern const uint32	target_xuart_base_table[];

void
xuart_putc(char8 c)
{
	uint32 base = target_xuart_base_table[x_core_id()];

	while((sil_rew_mem((void*)(base + XUART_SR)) & XUART_SR_TXFULL));
	sil_wrw_mem((void*)(base + XUART_FIFO), c);
}

/*
 *  文字を受信したかチェック
 */
LOCAL_INLINE boolean
uart_getready(CoreIdType coreid)
{
	uint32 base = target_xuart_base_table[coreid];
	return((sil_rew_mem((void*)(base + XUART_SR)) & XUART_SR_RXEMPTY) != XUART_SR_RXEMPTY);
}

/*
 *  受信した文字の取り出し
 */
LOCAL_INLINE uint8
uart_getchar(CoreIdType coreid)
{
	char c;

	uint32 base = target_xuart_base_table[coreid];
	c = (char)sil_rew_mem((void*)(base + XUART_FIFO));
	return(c);
}

/*
 *  初期化処理
 */
void
InitHwSerial(void)
{
	uint32 base = target_xuart_base_table[x_core_id()];

	/* Receive FIFO interrupt trigger level */
	sil_wrw_mem((void*)(base + XUART_RXWM), 1);

	/*
	 *  受信割込み許可
	 */
	sil_wrw_mem((void*)(base + XUART_IER), XUART_INT_RTRIG);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
TermHwSerial(void)
{
	uint32 base = target_xuart_base_table[x_core_id()];

	/*
	 * 受信割込みの禁止
	 */
	sil_wrw_mem((void*)(base + XUART_IDR), XUART_INT_RTRIG);
}

LOCAL_INLINE void
RxHwSerialInt_handler(CoreIdType coreid)
{
	uint32 base = target_xuart_base_table[x_core_id()];

	if (uart_getready(coreid) != FALSE) {
		/*
		 *  受信通知コールバックルーチンを呼び出す
		 */
		RxSerialInt(uart_getchar(coreid));
	}

	/* Clear Interrupt */
	sil_wrw_mem((void*)(base + XUART_ISR),0xffffffff);
}

/*
 *  SIOの割込みハンドラ
 */
ISR(RxHwSerialInt0)
{
	RxHwSerialInt_handler(0U);
}

ISR(RxHwSerialInt1)
{
	RxHwSerialInt_handler(1U);
}
