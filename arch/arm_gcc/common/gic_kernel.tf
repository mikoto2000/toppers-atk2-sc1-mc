$ 
$     パス2のチップ依存テンプレート（GIC用）
$ 

$FOREACH intno INTNO_VALID$
	$FOREACH isrid ISR.ID_LIST$
		$IF intno == ISR.INTNO[isrid]$
			$INT.ISRID[intno] = isrid$
		$END$
	$END$
$END$

$
$  割込みハンドラテーブル
$
$FOREACH coreid RANGE(0, TMAX_COREID)$
const FunctionRefType core$coreid$_isr_tbl[TNUM_INT] = {$NL$
$FOREACH intno INTNO_VALID$
	$IF (((intno & 0xffff0000) == ((coreid+1) << 16)) || ((intno & 0xffff0000) == 0xffff0000))$
		$isrid = INT.ISRID[intno]$
		$IF LENGTH(isrid) && EQ(ISR.CATEGORY[isrid], "CATEGORY_2")  && (OSAP.CORE[ISR.OSAPID[isrid]] == coreid)$
			$TAB$&$ISR.INT_ENTRY[isrid]$
		$ELIF LENGTH(FIND(INTNO_ICI_LIST, intno))$
$			//コア間割割込みハンドラ
			$TAB$&$CONCAT("kernel_inthdr_", FORMAT("0x%x",+intno))$
		$ELSE$
			$TAB$&kernel_default_int_handler
		$END$
$		//カンマの出力（最後の要素の後ろに出力しない）
		$IF (intno & 0xffff) < TMAX_INTNO$
			,
		$END$
		$TAB$$FORMAT("/* 0x%x */", +intno)$$NL$
	$END$
$END$
};$NL$
$NL$
$END$

const uint32 kernel_isr_table[TotalNumberOfCores] = {$NL$
$JOINEACH coreid RANGE(0, TMAX_COREID) ",\n"$
	$TAB$(const uint32) core$coreid$_isr_tbl
$END$
$NL$};$NL$
$NL$


$
$  ISRCBの取得テーブル
$
$FOREACH coreid RANGE(0, TMAX_COREID)$
ISRCB * const core$coreid$_isr_p_isrcb_tbl[TNUM_INT] = {$NL$
$FOREACH intno INTNO_VALID$
	$IF ((intno & 0xffff0000) == ((coreid+1) << 16)) || ((intno & 0xffff0000) == 0xffff0000) $
		$isrid = INT.ISRID[intno]$
		$IF LENGTH(isrid) && EQ(ISR.CATEGORY[isrid], "CATEGORY_2") && (OSAP.CORE[ISR.OSAPID[isrid]] == coreid)$
			$TAB$&kernel_isrcb_$isrid$
		$ELSE$
			$TAB$NULL
		$END$
$		//カンマの出力（最後の要素の後ろに出力しない）
		$IF (intno & 0xffff) < TMAX_INTNO$
			,
		$END$
		$TAB$$FORMAT("/* 0x%x */", +intno)$$NL$
	$END$
$END$
};$NL$
$NL$
$END$

const uint32 kernel_isr_p_isrcb_table[TotalNumberOfCores] = {$NL$
$JOINEACH coreid RANGE(0, TMAX_COREID) ",\n"$
	$TAB$(const uint32) core$coreid$_isr_p_isrcb_tbl
$END$
$NL$};$NL$
$NL$
