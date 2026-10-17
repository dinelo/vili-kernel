#include<linux/list.h>
#include<linux/seq_file.h>
#include<linux/kobject.h>

struct wakeup_device {
	struct list_head list;
	char *name;
	void *data;
	unsigned int count;
	bool (*check_wakeup_event)(void *data);
};

struct system_event_file_ops {
	int (*event_show)(struct seq_file *seq, void *v);
};

enum system_event_type {
	SYSTEM_EVENT_ALARM,
	SYSTEM_EVENT_UEVENT,
	SYSTEM_EVENT_MAX,
};

struct system_event {
	char creator_info[100];
	struct timespec set_time;
};

enum system_dbg_type {
	DEBUG_INFO_CLOCK,
	DEBUG_INFO_REGULATOR,
	DEBUG_INFO_GPIO,
	DEBUG_INFO_RPM_STATS,
	DEBUG_INFO_RPM_MASTER_STATS,
	DEBUG_INFO_MAX,
};

enum power_mode_type {
        POWER_DISABLE_MODE,
	POWER_PERF_MODE,
	POWER_BALANCE_MODE,
	POWER_SAVE_MODE,
	POWER_DEEP_SAVE_MODE,
	POWER_DIVINA_MODE, 
	POWER_MODE_MAX,
};

struct system_dbg_info {
	struct list_head list;
	int type;
	void (*system_dbg_info_print)(struct system_dbg_info *info);
	void *data;
};

struct system_event_recorder {
	struct list_head list;
	char *name;
	enum system_event_type type;
	struct system_event_file_ops *ops;
	void (*system_event_record)(struct system_event_recorder *recorder, void *data);
	void (*system_event_print_records)(struct system_event_recorder *recorder);
	u32 max_num;
	void *buff;
	u32 count;
	u32 index_head;
	u32 index_tail;
};

int pm_register_system_dbg_info(struct system_dbg_info *info);
void alarm_record_add(struct alarm *alarm);
int pm_register_wakeup_device(struct wakeup_device *dev);
void pm_system_dbg_info_print(enum system_dbg_type type);
int pm_register_system_event_recorder(struct system_event_recorder *rec);
void pm_trigger_system_event_record(enum system_event_type type, void *data);
void uevent_record_add(struct kobj_uevent_env *uevent_env);
bool power_debug_print_enabled(void);
uint8_t mi_power_mode(void);
ssize_t mi_power_save_battery_cave(ssize_t capcity);
extern int power_mode_register_notifier(struct notifier_block *nb);
extern int power_mode_unregister_notifier(struct notifier_block *nb);
