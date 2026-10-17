# Xiaomi stock ABI material (vili, 5.4.86-qgki-g4d120346d9d9, MIUI V12.5.10.0.RKDMIXM)

* `../config/stock.config` – `/proc/config.gz` of the stock kernel.
* `../config/stock_crcs.txt` – symbol CRCs imported by stock vendor modules (msm_drm, cnss2, camera).
* `../config/stock_exports.txt` – export names found in the stock kernel's `__ksymtab_strings`.
* `stock-headers/` – headers taken from the stock kernel's embedded IKHEADERS archive
  (`kheaders_data.tar.xz` inside the stock boot.img kernel) that differ from MiCode `vili-r-oss`.
  The OSS release strips Xiaomi's MIGT / PACKAGE_RUNTIME_INFO fields from `task_struct` and
  `user_struct`; without them every CRC that reaches `task_struct` differs from stock.
  (Comments are stripped, as in every IKHEADERS archive.) `trace/events/f2fs.h` is intentionally
  NOT overlaid: it needs private f2fs enums (F2FS_CP_OPT) and does not affect exported ABI.
* `Kconfig.xiaomi` – Kconfig stubs for options present in the stock config but missing from OSS
  Kconfig, so `olddefconfig` keeps them (and their header effects) exactly as stock.
