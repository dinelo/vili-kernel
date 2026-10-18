#!/system/bin/sh
# Incarca rtl8187 la boot (late_start). Nu poate bloca boot-ul: ruleaza dupa ce sistemul a pornit.
MODDIR=${0%/*}
exec "$MODDIR/system/bin/rtl8187" load > "$MODDIR/load.log" 2>&1
