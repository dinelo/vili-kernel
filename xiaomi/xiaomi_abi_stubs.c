// SPDX-License-Identifier: GPL-2.0
/*
 * Stub-uri pentru simboluri exportate de kernelul stock Xiaomi (cod nepublicat
 * in MiCode vili-r-oss) si importate de modulele vendor stock.
 * qti_battery_charger_main.ko: mi_power_save_battery_cave, power_debug_print_enabled
 * Prototipurile sunt cele din include/linux/power_debug.h (IKHEADERS stock),
 * deci CRC-urile modversions sunt identice cu stock.
 */
#include <linux/types.h>
#include <linux/export.h>
#include <linux/alarmtimer.h>
#include <linux/kobject.h>
#include <linux/notifier.h>
#include <linux/power_debug.h>

bool power_debug_print_enabled(void)
{
	return false;
}
EXPORT_SYMBOL(power_debug_print_enabled);

/* fara mod "power save" Xiaomi: capacitatea raportata ramane neschimbata */
ssize_t mi_power_save_battery_cave(ssize_t capcity)
{
	return capcity;
}
EXPORT_SYMBOL(mi_power_save_battery_cave);
