/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2007-2013 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *  上記著作権者は，以下の(1)?(4)の条件を満たす場合に限り，本ソフトウェ
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
 */
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm","elf32-littlearm")
OUTPUT_ARCH(arm)
STARTUP(start.o)

PROVIDE(hardware_init_hook = 0);
PROVIDE(software_init_hook = 0);
PROVIDE(software_term_hook = 0);
/*
MEMORY
{
   psu_bbram_0_S_AXI_BASEADDR : ORIGIN = 0xFFCD0000, LENGTH = 0x10000
   psu_ocm_S_AXI_BASEADDR : ORIGIN = 0xFF960000, LENGTH = 0x10000
   psu_ocm_ram_0_S_AXI_BASEADDR : ORIGIN = 0xFFFC0000, LENGTH = 0x30000
   psu_ocm_ram_1_S_AXI_BASEADDR : ORIGIN = 0xFFFF0000, LENGTH = 0x10000
   psu_pmu_ram_S_AXI_BASEADDR : ORIGIN = 0xFFDC0000, LENGTH = 0x20000
   psu_qspi_linear_0_S_AXI_BASEADDR : ORIGIN = 0xC0000000, LENGTH = 0x20000000
   psu_r5_0_atcm_S_AXI_BASEADDR : ORIGIN = 0xFFE00000, LENGTH = 0x10000
   psu_r5_0_atcm_lockstep_S_AXI_BASEADDR : ORIGIN = 0xFFE10000, LENGTH = 0x10000
   psu_r5_0_btcm_S_AXI_BASEADDR : ORIGIN = 0xFFE20000, LENGTH = 0x10000
   psu_r5_0_btcm_lockstep_S_AXI_BASEADDR : ORIGIN = 0xFFE30000, LENGTH = 0x10000
   psu_r5_1_atcm_S_AXI_BASEADDR : ORIGIN = 0xFFE90000, LENGTH = 0x10000
   psu_r5_1_btcm_S_AXI_BASEADDR : ORIGIN = 0xFFEB0000, LENGTH = 0x10000
   psu_r5_ddr_0_S_AXI_BASEADDR : ORIGIN = 0x100000, LENGTH = 0x7FF00000
   psu_r5_ddr_0_S_AXI_TEXT_BASEADDR : ORIGIN = 0x100000, LENGTH = 0x100000
   psu_r5_ddr_0_S_AXI_DATA_BASEADDR : ORIGIN = 0x200000, LENGTH = 0x100000
   psu_r5_tcm_ram_0_S_AXI_BASEADDR : ORIGIN = 0x100, LENGTH = 0x1FF01
}
*/
SECTIONS
{
    .vector 0x0 : {
       KEEP (*(vector))
    }

    .text :
    {
        __text = . ;
	    start.o(.text)
        *(.text)
        *(.text.*)
        *(.glue_7t)
        *(.glue_7)
    }

    _etext = .	;
    PROVIDE (etext = .)	;
    .rodata : {
		*(.rodata)
	}

    . = ALIGN(4);
    __idata_start = . ;
    .data   :  AT(__idata_start)
    {
        __data_start = . ;
        *(.data)
    }
    __idata_end = __idata_start + SIZEOF(.data);
    _edata  =  . ;
    PROVIDE (edata = .);
    . = ALIGN(4);
    .bss       :
    {
      __bss_start = .	;
      *(.bss)
      *(COMMON)
        *(._kernel_core0s_ccb)
        *(._kernel_core0*)
        *(._kernel_core1s_ccb)
        *(._kernel_core1*)
   . = ALIGN(32 / 8);
    }
    __bss_end = .;

     _end = .		;

    PROVIDE (end = .)	;
    .comment 0 : { *(.comment) }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to
     the beginning of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
}
