/*
 *  @(#) $Id: core_cfg1_out.h 849 2017-09-29 13:34:39Z ertl-honda $
 */

/*
 *		cfg1_out.cをリンクするために必要なスタブの定義
 */

void sta_ker(void){}
void kernel_target_mprc_initialize(void){}
StackType *const	kernel_ostkpt_table[1];

/*
 *  オフセットファイルを生成するための定義
 */
const uint8_t   MAGIC_1 = 0x12;
const uint16_t  MAGIC_2 = 0x1234;
const uint32_t  MAGIC_4 = 0x12345678;
