$ 
$     パス2のコア依存テンプレート（ARM用）
$ 

$FUNCTION EXTERN_INT_HANDLER$

$	コア間割込みハンドラ本体生成のextern宣言
	$FOREACH coreid RANGE(0, TMAX_COREID)$
		$FOREACH intno INTNO_ICI_LIST$
			$IF (intno & 0xffff0000) == ( (coreid+1) << 16)$
				extern ISR(target_ici_handler$coreid$);$NL$
			$END$
		$END$
	$END$
$NL$

$END$

$ 
$ DEF_EXCで使用できるCPU例外ハンドラ番号
$ 
$EXCNO_DEFEXC_VALID = EXCNO_VALID$

$ 
$  標準テンプレートファイルのインクルード
$ 
$INCLUDE "kernel/kernel.tf"$

$DEFAULT_EXC_HANDLER[0] = "default_exc_handler"$
$DEFAULT_EXC_HANDLER[1] = "default_undef_handler"$
$DEFAULT_EXC_HANDLER[2] = "default_swi_handler"$
$DEFAULT_EXC_HANDLER[3] = "default_prefetchabort_handler"$
$DEFAULT_EXC_HANDLER[4] = "default_dataabort_handler"$
$DEFAULT_EXC_HANDLER[5] = "default_exc_handler"$
$DEFAULT_EXC_HANDLER[6] = "default_exc_handler"$

$FILE "Os_Lcfg.c"$

$
$ コア間割込みハンドラ本体生成
$
$FOREACH coreid RANGE(0, TMAX_COREID)$
	$FOREACH intno INTNO_ICI_LIST$
		$IF (intno & 0xffff0000) == ( (coreid+1) << 16)$
			void$NL$
			$CONCAT("kernel_inthdr_", FORMAT("0x%x",+intno))$(void)$NL$
			{$NL$
			$TAB$i_begin_int($+intno$U);$NL$
			$TAB$ISRNAME(target_ici_handler$coreid$)();$NL$
			$TAB$i_end_int($+intno$U);$NL$
			}$NL$
		$END$
	$END$
$END$

$ 
$  例外ハンドラテーブル
$ 
$FOREACH prcid RANGE(0, TMAX_COREID)$

const FP prc$prcid$_exch_table[TNUM_EXCH] = {$NL$
$JOINEACH excno {0,1,...,6} ",\n"$ 
    $number = excno|(prcid<<16)$
	$IF LENGTH(EXC.EXCNO[number])$
		$TAB$(FP)($EXC.EXCHDR[number]$)
	$ELSE$
		$TAB$(FP)($DEFAULT_EXC_HANDLER[excno]$)
	$END$
	$SPC$$FORMAT("/* %d */", +excno)$
$END$
$NL$};$NL$
$NL$

$END$

$ 
$  例外ハンドラアクセステーブル
$ 
const FP* const p_exch_table[TotalNumberOfCores] = {$NL$
$JOINEACH prcid RANGE(0, TMAX_COREID) ",\n"$
	$TAB$prc$prcid$_exch_table
$END$
$NL$};$NL$
$NL$
