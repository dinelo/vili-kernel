# Instalare: verificam ca e exact kernelul stock pentru care sunt compilate modulele
WANT_REL="5.4.86-qgki-g4d120346d9d9"
DEV="$(getprop ro.product.device)"; [ -n "$DEV" ] || DEV="$(getprop ro.product.vendor.device)"
REL="$(uname -r)"
ui_print "- device: $DEV  kernel: $REL"
case "$DEV" in vili*) ;; *) abort "! Doar pentru Xiaomi 11T Pro (vili)";; esac
[ "$REL" = "$WANT_REL" ] || abort "! Kernel $REL != $WANT_REL (modulele sunt pentru kernelul stock)"
set_perm_recursive "$MODPATH" 0 0 0755 0644
set_perm "$MODPATH/service.sh" 0 0 0755
set_perm "$MODPATH/system/bin/rtl8187" 0 0 0755
ui_print "- Modulele se incarca la boot (service.sh). Manual: su -c rtl8187 load|unload|status|fix|monitor|managed"
ui_print "- Interfata RTL8187 e fixata pe wlan3 (AR9271 ramane pe wlan2)"
