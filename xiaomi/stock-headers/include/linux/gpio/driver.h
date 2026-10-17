/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_GPIO_DRIVER_H
#define __LINUX_GPIO_DRIVER_H

#include <linux/device.h>
#include <linux/types.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/lockdep.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf-generic.h>

struct gpio_desc;
struct of_phandle_args;
struct device_node;
struct seq_file;
struct gpio_device;
struct module;
enum gpiod_flags;
enum gpio_lookup_flags;

struct gpio_chip;

#ifdef CONFIG_DEBUG_POWER_MI

struct gpio_dbg_device {
	struct list_head list;
	char *name;
	void (*gpiolib_dbg_info_print)(struct gpio_chip *chip);
	struct gpio_chip *chip;
};

void gpiochip_add_dbg_device(struct gpio_dbg_device *dev);

#endif


struct gpio_irq_chip {
	
	struct irq_chip *chip;

	
	struct irq_domain *domain;

	
	const struct irq_domain_ops *domain_ops;

#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY
	
	struct fwnode_handle *fwnode;

	
	struct irq_domain *parent_domain;

	
	int (*child_to_parent_hwirq)(struct gpio_chip *chip,
				     unsigned int child_hwirq,
				     unsigned int child_type,
				     unsigned int *parent_hwirq,
				     unsigned int *parent_type);

	
	void (*populate_parent_fwspec)(struct gpio_chip *chip,
				       struct irq_fwspec *fwspec,
				       unsigned int parent_hwirq,
				       unsigned int parent_type);

	
	unsigned int (*child_offset_to_irq)(struct gpio_chip *chip,
					    unsigned int pin);

	
	struct irq_domain_ops child_irq_domain_ops;
#endif

	
	irq_flow_handler_t handler;

	
	unsigned int default_type;

	
	struct lock_class_key *lock_key;

	
	struct lock_class_key *request_key;

	
	irq_flow_handler_t parent_handler;

	
	void *parent_handler_data;

	
	unsigned int num_parents;

	
	unsigned int *parents;

	
	unsigned int *map;

	
	bool threaded;

	
	int (*init_hw)(struct gpio_chip *chip);

	
	void (*init_valid_mask)(struct gpio_chip *chip,
				unsigned long *valid_mask,
				unsigned int ngpios);

	
	unsigned long *valid_mask;

	
	unsigned int first;

	
	void		(*irq_enable)(struct irq_data *data);

	
	void		(*irq_disable)(struct irq_data *data);
};


struct gpio_chip {
	const char		*label;
	struct gpio_device	*gpiodev;
	struct device		*parent;
	struct module		*owner;

	int			(*request)(struct gpio_chip *chip,
						unsigned offset);
	void			(*free)(struct gpio_chip *chip,
						unsigned offset);
	int			(*get_direction)(struct gpio_chip *chip,
						unsigned offset);
	int			(*direction_input)(struct gpio_chip *chip,
						unsigned offset);
	int			(*direction_output)(struct gpio_chip *chip,
						unsigned offset, int value);
	int			(*get)(struct gpio_chip *chip,
						unsigned offset);
	int			(*get_multiple)(struct gpio_chip *chip,
						unsigned long *mask,
						unsigned long *bits);
	void			(*set)(struct gpio_chip *chip,
						unsigned offset, int value);
	void			(*set_multiple)(struct gpio_chip *chip,
						unsigned long *mask,
						unsigned long *bits);
	int			(*set_config)(struct gpio_chip *chip,
					      unsigned offset,
					      unsigned long config);
	int			(*to_irq)(struct gpio_chip *chip,
						unsigned offset);

	void			(*dbg_show)(struct seq_file *s,
						struct gpio_chip *chip);

	int			(*init_valid_mask)(struct gpio_chip *chip,
						   unsigned long *valid_mask,
						   unsigned int ngpios);

	int			base;
	u16			ngpio;
	const char		*const *names;
	bool			can_sleep;

#if IS_ENABLED(CONFIG_GPIO_GENERIC)
	unsigned long (*read_reg)(void __iomem *reg);
	void (*write_reg)(void __iomem *reg, unsigned long data);
	bool be_bits;
	void __iomem *reg_dat;
	void __iomem *reg_set;
	void __iomem *reg_clr;
	void __iomem *reg_dir_out;
	void __iomem *reg_dir_in;
	bool bgpio_dir_unreadable;
	int bgpio_bits;
	spinlock_t bgpio_lock;
	unsigned long bgpio_data;
	unsigned long bgpio_dir;
#endif 

#ifdef CONFIG_GPIOLIB_IRQCHIP
	

	
	struct gpio_irq_chip irq;
#endif 

	
	unsigned long *valid_mask;

#if defined(CONFIG_OF_GPIO)
	

	
	struct device_node *of_node;

	
	unsigned int of_gpio_n_cells;

	
	int (*of_xlate)(struct gpio_chip *gc,
			const struct of_phandle_args *gpiospec, u32 *flags);
#endif 
};

extern const char *gpiochip_is_requested(struct gpio_chip *chip,
			unsigned offset);


extern int gpiochip_add_data_with_key(struct gpio_chip *chip, void *data,
				      struct lock_class_key *lock_key,
				      struct lock_class_key *request_key);


#ifdef CONFIG_LOCKDEP
#define gpiochip_add_data(chip, data) ({		\
		static struct lock_class_key lock_key;	\
		static struct lock_class_key request_key;	  \
		gpiochip_add_data_with_key(chip, data, &lock_key, \
					   &request_key);	  \
	})
#else
#define gpiochip_add_data(chip, data) gpiochip_add_data_with_key(chip, data, NULL, NULL)
#endif 

static inline int gpiochip_add(struct gpio_chip *chip)
{
	return gpiochip_add_data(chip, NULL);
}
extern void gpiochip_remove(struct gpio_chip *chip);
extern int devm_gpiochip_add_data(struct device *dev, struct gpio_chip *chip,
				  void *data);

extern struct gpio_chip *gpiochip_find(void *data,
			      int (*match)(struct gpio_chip *chip, void *data));

bool gpiochip_line_is_irq(struct gpio_chip *chip, unsigned int offset);
int gpiochip_reqres_irq(struct gpio_chip *chip, unsigned int offset);
void gpiochip_relres_irq(struct gpio_chip *chip, unsigned int offset);
void gpiochip_disable_irq(struct gpio_chip *chip, unsigned int offset);
void gpiochip_enable_irq(struct gpio_chip *chip, unsigned int offset);


bool gpiochip_line_is_open_drain(struct gpio_chip *chip, unsigned int offset);
bool gpiochip_line_is_open_source(struct gpio_chip *chip, unsigned int offset);


bool gpiochip_line_is_persistent(struct gpio_chip *chip, unsigned int offset);
bool gpiochip_line_is_valid(const struct gpio_chip *chip, unsigned int offset);


void *gpiochip_get_data(struct gpio_chip *chip);

struct bgpio_pdata {
	const char *label;
	int base;
	int ngpio;
};

#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY

void gpiochip_populate_parent_fwspec_twocell(struct gpio_chip *chip,
					     struct irq_fwspec *fwspec,
					     unsigned int parent_hwirq,
					     unsigned int parent_type);
void gpiochip_populate_parent_fwspec_fourcell(struct gpio_chip *chip,
					      struct irq_fwspec *fwspec,
					      unsigned int parent_hwirq,
					      unsigned int parent_type);

#else

static inline void gpiochip_populate_parent_fwspec_twocell(struct gpio_chip *chip,
						    struct irq_fwspec *fwspec,
						    unsigned int parent_hwirq,
						    unsigned int parent_type)
{
}

static inline void gpiochip_populate_parent_fwspec_fourcell(struct gpio_chip *chip,
						     struct irq_fwspec *fwspec,
						     unsigned int parent_hwirq,
						     unsigned int parent_type)
{
}

#endif 

int bgpio_init(struct gpio_chip *gc, struct device *dev,
	       unsigned long sz, void __iomem *dat, void __iomem *set,
	       void __iomem *clr, void __iomem *dirout, void __iomem *dirin,
	       unsigned long flags);

#define BGPIOF_BIG_ENDIAN		BIT(0)
#define BGPIOF_UNREADABLE_REG_SET	BIT(1) 
#define BGPIOF_UNREADABLE_REG_DIR	BIT(2) 
#define BGPIOF_BIG_ENDIAN_BYTE_ORDER	BIT(3)
#define BGPIOF_READ_OUTPUT_REG_SET	BIT(4) 
#define BGPIOF_NO_OUTPUT		BIT(5) 

int gpiochip_irq_map(struct irq_domain *d, unsigned int irq,
		     irq_hw_number_t hwirq);
void gpiochip_irq_unmap(struct irq_domain *d, unsigned int irq);

int gpiochip_irq_domain_activate(struct irq_domain *domain,
				 struct irq_data *data, bool reserve);
void gpiochip_irq_domain_deactivate(struct irq_domain *domain,
				    struct irq_data *data);

void gpiochip_set_chained_irqchip(struct gpio_chip *gpiochip,
		struct irq_chip *irqchip,
		unsigned int parent_irq,
		irq_flow_handler_t parent_handler);

void gpiochip_set_nested_irqchip(struct gpio_chip *gpiochip,
		struct irq_chip *irqchip,
		unsigned int parent_irq);

int gpiochip_irqchip_add_key(struct gpio_chip *gpiochip,
			     struct irq_chip *irqchip,
			     unsigned int first_irq,
			     irq_flow_handler_t handler,
			     unsigned int type,
			     bool threaded,
			     struct lock_class_key *lock_key,
			     struct lock_class_key *request_key);

bool gpiochip_irqchip_irq_valid(const struct gpio_chip *gpiochip,
				unsigned int offset);

#ifdef CONFIG_LOCKDEP


static inline int gpiochip_irqchip_add(struct gpio_chip *gpiochip,
				       struct irq_chip *irqchip,
				       unsigned int first_irq,
				       irq_flow_handler_t handler,
				       unsigned int type)
{
	static struct lock_class_key lock_key;
	static struct lock_class_key request_key;

	return gpiochip_irqchip_add_key(gpiochip, irqchip, first_irq,
					handler, type, false,
					&lock_key, &request_key);
}

static inline int gpiochip_irqchip_add_nested(struct gpio_chip *gpiochip,
			  struct irq_chip *irqchip,
			  unsigned int first_irq,
			  irq_flow_handler_t handler,
			  unsigned int type)
{

	static struct lock_class_key lock_key;
	static struct lock_class_key request_key;

	return gpiochip_irqchip_add_key(gpiochip, irqchip, first_irq,
					handler, type, true,
					&lock_key, &request_key);
}
#else 
static inline int gpiochip_irqchip_add(struct gpio_chip *gpiochip,
				       struct irq_chip *irqchip,
				       unsigned int first_irq,
				       irq_flow_handler_t handler,
				       unsigned int type)
{
	return gpiochip_irqchip_add_key(gpiochip, irqchip, first_irq,
					handler, type, false, NULL, NULL);
}

static inline int gpiochip_irqchip_add_nested(struct gpio_chip *gpiochip,
			  struct irq_chip *irqchip,
			  unsigned int first_irq,
			  irq_flow_handler_t handler,
			  unsigned int type)
{
	return gpiochip_irqchip_add_key(gpiochip, irqchip, first_irq,
					handler, type, true, NULL, NULL);
}
#endif 

int gpiochip_generic_request(struct gpio_chip *chip, unsigned offset);
void gpiochip_generic_free(struct gpio_chip *chip, unsigned offset);
int gpiochip_generic_config(struct gpio_chip *chip, unsigned offset,
			    unsigned long config);


struct gpio_pin_range {
	struct list_head node;
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range range;
};

#ifdef CONFIG_PINCTRL

int gpiochip_add_pin_range(struct gpio_chip *chip, const char *pinctl_name,
			   unsigned int gpio_offset, unsigned int pin_offset,
			   unsigned int npins);
int gpiochip_add_pingroup_range(struct gpio_chip *chip,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group);
void gpiochip_remove_pin_ranges(struct gpio_chip *chip);

#else 

static inline int
gpiochip_add_pin_range(struct gpio_chip *chip, const char *pinctl_name,
		       unsigned int gpio_offset, unsigned int pin_offset,
		       unsigned int npins)
{
	return 0;
}
static inline int
gpiochip_add_pingroup_range(struct gpio_chip *chip,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group)
{
	return 0;
}

static inline void
gpiochip_remove_pin_ranges(struct gpio_chip *chip)
{
}

#endif 

struct gpio_desc *gpiochip_request_own_desc(struct gpio_chip *chip, u16 hwnum,
					    const char *label,
					    enum gpio_lookup_flags lflags,
					    enum gpiod_flags dflags);
void gpiochip_free_own_desc(struct gpio_desc *desc);

void devprop_gpiochip_set_names(struct gpio_chip *chip,
				const struct fwnode_handle *fwnode);

#ifdef CONFIG_GPIOLIB


int gpiochip_lock_as_irq(struct gpio_chip *chip, unsigned int offset);
void gpiochip_unlock_as_irq(struct gpio_chip *chip, unsigned int offset);


struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc);

#else 

static inline struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc)
{
	
	WARN_ON(1);
	return ERR_PTR(-ENODEV);
}

static inline int gpiochip_lock_as_irq(struct gpio_chip *chip,
				       unsigned int offset)
{
	WARN_ON(1);
	return -EINVAL;
}

static inline void gpiochip_unlock_as_irq(struct gpio_chip *chip,
					  unsigned int offset)
{
	WARN_ON(1);
}
#endif 

#endif 
