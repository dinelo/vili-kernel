#!/system/bin/sh
# Incarca ath9k_htc la boot (late_start). Nu poate bloca boot-ul: ruleaza dupa ce sistemul a pornit.
MODDIR=${0%/*}
exec "$MODDIR/system/bin/ar9271" load > "$MODDIR/load.log" 2>&1
