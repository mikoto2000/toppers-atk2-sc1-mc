/* This file is generated from core_rename.def by genrename. */

/* This file is included only when core_rename.h has been included. */
#ifdef TOPPERS_CORE_RENAME_H
#undef TOPPERS_CORE_RENAME_H

/*
 *  core_config.c
 */
#undef core_initialize
#undef core_terminate
#undef initialize_exception
#undef default_int_handler
#undef dump_excframe

/*
 *  Os_Lcfg.c
 */
#undef ostkpt_table
#undef isr_tbl
#undef isr_p_isrcb_tbl

/*
 *  prc_support.S
 */

#ifdef TOPPERS_LABEL_ASM

/*
 *  core_config.c
 */
#undef _core_initialize
#undef _core_terminate
#undef _initialize_exception
#undef _default_int_handler
#undef _dump_excframe

/*
 *  Os_Lcfg.c
 */
#undef _ostkpt_table
#undef _isr_tbl
#undef _isr_p_isrcb_tbl

/*
 *  prc_support.S
 */

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_CORE_RENAME_H */
