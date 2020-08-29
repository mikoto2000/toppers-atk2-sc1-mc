/* This file is generated from core_rename.def by genrename. */

#ifndef TOPPERS_CORE_RENAME_H
#define TOPPERS_CORE_RENAME_H

/*
 *  core_config.c
 */
#define core_initialize				kernel_core_initialize
#define core_terminate				kernel_core_terminate
#define initialize_exception		kernel_initialize_exception
#define default_int_handler			kernel_default_int_handler
#define dump_excframe				kernel_dump_excframe

/*
 *  Os_Lcfg.c
 */
#define ostkpt_table				kernel_ostkpt_table
#define isr_tbl						kernel_isr_tbl
#define isr_p_isrcb_tbl				kernel_isr_p_isrcb_tbl

/*
 *  prc_support.S
 */

#ifdef TOPPERS_LABEL_ASM

/*
 *  core_config.c
 */
#define _core_initialize			_kernel_core_initialize
#define _core_terminate				_kernel_core_terminate
#define _initialize_exception		_kernel_initialize_exception
#define _default_int_handler		_kernel_default_int_handler
#define _dump_excframe				_kernel_dump_excframe

/*
 *  Os_Lcfg.c
 */
#define _ostkpt_table				_kernel_ostkpt_table
#define _isr_tbl					_kernel_isr_tbl
#define _isr_p_isrcb_tbl			_kernel_isr_p_isrcb_tbl

/*
 *  prc_support.S
 */

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_CORE_RENAME_H */
