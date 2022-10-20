// SPI IP Example
// SPI IP Kernel Default Handler (spi_driver.c)
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: DE1-SoC Board

// Hardware configuration:
// SPI Port:
//   SPI_1[31-0] is used as a general purpose SPI port
// HPS interface:
//   Mapped to offset of 0 in light-weight MM interface aperature

//-----------------------------------------------------------------------------

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/init.h>
#include <asm/io.h>           // iowrite, ioread (platform specific)
#include "address_map.h"
#include "spi_regs.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static unsigned int *base = NULL;

//-----------------------------------------------------------------------------
// Kernel module information
//-----------------------------------------------------------------------------

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ali Nawaz");
MODULE_DESCRIPTION("SPI IP DRIVER");

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void set_baud_rate(uint brd)
{
	iowrite32(brd, base + OFS_BRD);
}

uint get_baud_rate(void)
{
	return ioread32(base + OFS_BRD);
}

void set_word_size(uint8_t word_size)
{
	uint value = ioread32(base + OFS_CONTROL);
	value &=  ~WORDSIZE_MASK;
	iowrite32((value | ((word_size-1)& WORDSIZE_MASK)), base + OFS_CONTROL);
}

uint get_word_size(void)
{
	return (ioread32(base + OFS_CONTROL) & WORDSIZE_MASK);
}

void set_cs_select(uint8_t device)
{
	uint value = ioread32(base + OFS_CONTROL);
	value &=  ~(CS_SELECT_MASK << CS_SELECT_BIT_OFS);
	iowrite32((value | ((device & CS_SELECT_MASK) << CS_SELECT_BIT_OFS)), base + OFS_CONTROL);
}

uint get_cs_select(void)
{
	uint value = ioread32(base + OFS_CONTROL);
	return (value >> CS_SELECT_BIT_OFS) & CS_SELECT_MASK;
}

void set_device_mode(uint8_t device, uint8_t mode)
{
	uint value = ioread32(base + OFS_CONTROL);
	value &= ~(DEVICE_MODE_MASK << (DEVICE_MODE_BIT_OFS+ 2*device));
	iowrite32((value | ((mode & DEVICE_MODE_MASK) << (DEVICE_MODE_BIT_OFS + 2*device))), base + OFS_CONTROL);
}

uint get_device_mode(uint8_t device)
{
	uint value = ioread32(base + OFS_CONTROL);
	return (value >> (DEVICE_MODE_BIT_OFS+2*device)) & DEVICE_MODE_MASK;
}

void enable_cs_auto(uint8_t device)
{
	uint value = ioread32(base + OFS_CONTROL);
	value &= ~(1 << (device+CS_AUTO_BIT_OFS));
	iowrite32((value | (1 << (device+CS_AUTO_BIT_OFS))), base + OFS_CONTROL);
}

void disable_cs_auto(uint8_t device)
{
	uint value = ioread32(base + OFS_CONTROL);
	iowrite32((value & ~(1 << (device+CS_AUTO_BIT_OFS))), base + OFS_CONTROL);
}

bool is_cs_auto_Enabled(uint8_t device)
{
	uint value = ioread32(base + OFS_CONTROL);
	return (value >> (device+CS_AUTO_BIT_OFS)) & 1;
}

void enable_cs_enable(uint8_t device)
{
	uint value = ioread32(base + OFS_CONTROL);
	value &= ~(1 << (device+CS_ENABLE_BIT_OFS));
	iowrite32((value | (1 << (device+CS_ENABLE_BIT_OFS))), base + OFS_CONTROL);
}

void disable_cs_enable(uint8_t device)
{
	uint value = ioread32(base + OFS_CONTROL);
	iowrite32((value & ~(1 << (device+CS_ENABLE_BIT_OFS))), base + OFS_CONTROL);
}

bool is_cs_enable(uint8_t device)
{
    return (ioread32(base + OFS_CONTROL) >> (device+CS_ENABLE_BIT_OFS)) & 1;
}

void set_tx_data(uint fifo_value)
{
	iowrite32(fifo_value, base + OFS_DATA);
}

uint get_rx_data(void)
{
	uint value = ioread32(base + OFS_DATA);
	if (value)
		return value;
	else
		return -1;
}

//-----------------------------------------------------------------------------
// Kernel Objects
//-----------------------------------------------------------------------------

// BAUD_RATE
static uint baud_rate = 25000000;
module_param(baud_rate, uint, S_IRUGO);
MODULE_PARM_DESC(baud_rate, " baud_rate of SPI");

static ssize_t baud_rateStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &baud_rate);
    if (result == 0)
    	set_baud_rate(baud_rate);
    return count;
}

static ssize_t baud_rateShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	baud_rate = get_baud_rate();
    return sprintf(buffer, "%d\n", baud_rate);
}

static struct kobj_attribute baud_rateAttr = __ATTR(baud_rate, 0664, baud_rateShow, baud_rateStore);

// WORD_SIZE
static uint word_size = 32;
module_param(word_size, uint, S_IRUGO);
MODULE_PARM_DESC(word_size, " word_size in the data bytes");

static ssize_t word_sizeStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &word_size);
    if (result == 0)
        set_word_size(word_size);
    return count;
}

static ssize_t word_sizeShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
    word_size = get_word_size();
    return sprintf(buffer, "%d\n", word_size);
}

static struct kobj_attribute word_sizeAttr = __ATTR(word_size, 0664, word_sizeShow, word_sizeStore);

// CS_SELECT
static int cs_select = 0;
module_param(cs_select, int, S_IRUGO);
MODULE_PARM_DESC(cs_select, " Select device 0-3");

static ssize_t cs_selectStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &cs_select);
    if (result == 0)
    	set_cs_select(cs_select);
    return count;
}

static ssize_t cs_selectShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_select = get_cs_select();
    return sprintf(buffer, "%d\n", cs_select);
}

static struct kobj_attribute cs_selectAttr = __ATTR(cs_select, 0664, cs_selectShow, cs_selectStore);

// DEVICE MODE 0
static int mode0 = 0;
module_param(mode0, int, S_IRUGO);
MODULE_PARM_DESC(mode0, " Select mode for device 0");

static ssize_t mode0Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &mode0);
    if (result == 0)
    	set_device_mode(0, mode0);
    return count;
}

static ssize_t mode0Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	mode0 = get_device_mode(0);
    return sprintf(buffer, "%d\n", mode0);
}

static struct kobj_attribute mode0Attr = __ATTR(mode0, 0664, mode0Show, mode0Store);

// DEVICE MODE 1
static int mode1 = 0;
module_param(mode1, int, S_IRUGO);
MODULE_PARM_DESC(mode1, " Select mode for device 1");

static ssize_t mode1Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &mode1);
    if (result == 0)
    	set_device_mode(1, mode1);
    return count;
}

static ssize_t mode1Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	mode1 = get_device_mode(1);
    return sprintf(buffer, "%d\n", mode1);
}

static struct kobj_attribute mode1Attr = __ATTR(mode1, 0664, mode1Show, mode1Store);

// DEVICE MODE 2
static int mode2 = 0;
module_param(mode2, int, S_IRUGO);
MODULE_PARM_DESC(mode2, " Select mode for device 2");

static ssize_t mode2Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &mode2);
    if (result == 0)
    	set_device_mode(2, mode2);
    return count;
}

static ssize_t mode2Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	mode2 = get_device_mode(2);
    return sprintf(buffer, "%d\n", mode2);
}

static struct kobj_attribute mode2Attr = __ATTR(mode2, 0664, mode2Show, mode2Store);

// DEVICE MODE 3
static int mode3 = 0;
module_param(mode3, int, S_IRUGO);
MODULE_PARM_DESC(mode3, " Select mode for device 3");

static ssize_t mode3Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &mode3);
    if (result == 0)
    	set_device_mode(3, mode3);
    return count;
}

static ssize_t mode3Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	mode3 = get_device_mode(3);
    return sprintf(buffer, "%d\n", mode3);
}

static struct kobj_attribute mode3Attr = __ATTR(mode3, 0664, mode3Show, mode3Store);

// CS_AUTO0
static bool cs_auto0 = 0;
module_param(cs_auto0, bool, S_IRUGO);
MODULE_PARM_DESC(cs_auto0, " Select cs_auto for device 0");

static ssize_t cs_auto0Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_auto(0);
		cs_auto0 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_auto(0);
			cs_auto0 = false;
		}
	return count;
}

static ssize_t cs_auto0Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_auto0 = is_cs_auto_Enabled(0);
    if (cs_auto0)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_auto0Attr = __ATTR(cs_auto0, 0664, cs_auto0Show, cs_auto0Store);

// CS_AUTO1
static bool cs_auto1 = 0;
module_param(cs_auto1, bool, S_IRUGO);
MODULE_PARM_DESC(cs_auto1, " Select cs_auto for device 1");

static ssize_t cs_auto1Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_auto(1);
		cs_auto1 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_auto(1);
			cs_auto1 = false;
		}
	return count;
}

static ssize_t cs_auto1Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_auto1 = is_cs_auto_Enabled(1);
    if (cs_auto1)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_auto1Attr = __ATTR(cs_auto1, 0664, cs_auto1Show, cs_auto1Store);

// CS_AUTO2
static bool cs_auto2 = 0;
module_param(cs_auto2, bool, S_IRUGO);
MODULE_PARM_DESC(cs_auto2, " Select cs_auto for device 2");

static ssize_t cs_auto2Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_auto(2);
		cs_auto2 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_auto(2);
			cs_auto2 = false;
		}
	return count;
}

static ssize_t cs_auto2Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_auto2 = is_cs_auto_Enabled(2);
    if (cs_auto2)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_auto2Attr = __ATTR(cs_auto2, 0664, cs_auto2Show, cs_auto2Store);

// CS_AUTO3
static bool cs_auto3 = 0;
module_param(cs_auto3, bool, S_IRUGO);
MODULE_PARM_DESC(cs_auto3, " Select cs_auto for device 3");

static ssize_t cs_auto3Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_auto(3);
		cs_auto3 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_auto(3);
			cs_auto3 = false;
		}
	return count;
}

static ssize_t cs_auto3Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_auto3 = is_cs_auto_Enabled(3);
    if (cs_auto3)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_auto3Attr = __ATTR(cs_auto3, 0664, cs_auto3Show, cs_auto3Store);

// CS_ENABLE0
static bool cs_enable0 = 0;
module_param(cs_enable0, bool, S_IRUGO);
MODULE_PARM_DESC(cs_enable0, " Select cs_enable for device 0");

static ssize_t cs_enable0Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_enable(0);
		cs_enable0 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_enable(0);
			cs_enable0 = false;
		}
	return count;
}

static ssize_t cs_enable0Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_enable0 = is_cs_enable(0);
    if (cs_enable0)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_enable0Attr = __ATTR(cs_enable0, 0664, cs_enable0Show, cs_enable0Store);

// CS_ENABLE1
static bool cs_enable1 = 0;
module_param(cs_enable1, bool, S_IRUGO);
MODULE_PARM_DESC(cs_enable1, " Select cs_enable for device 0");

static ssize_t cs_enable1Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_enable(1);
		cs_enable1 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_enable(1);
			cs_enable1 = false;
		}
	return count;
}

static ssize_t cs_enable1Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_enable1 = is_cs_enable(1);
    if (cs_enable1)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_enable1Attr = __ATTR(cs_enable1, 0664, cs_enable1Show, cs_enable1Store);

// CS_ENABLE2
static bool cs_enable2 = 0;
module_param(cs_enable2, bool, S_IRUGO);
MODULE_PARM_DESC(cs_enable2, " Select cs_enable for device 0");

static ssize_t cs_enable2Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_enable(2);
		cs_enable2 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_enable(2);
			cs_enable2 = false;
		}
	return count;
}

static ssize_t cs_enable2Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_enable2 = is_cs_enable(2);
    if (cs_enable2)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_enable2Attr = __ATTR(cs_enable2, 0664, cs_enable2Show, cs_enable2Store);

// CS_ENABLE3
static bool cs_enable3 = 0;
module_param(cs_enable3, bool, S_IRUGO);
MODULE_PARM_DESC(cs_enable3, " Select cs_auto for device 3");

static ssize_t cs_enable3Store(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
	if (strncmp(buffer, "auto", count-1) == 0)
	{
		enable_cs_enable(3);
		cs_enable3 = true;
	}
	else
		if (strncmp(buffer, "manual", count-1) == 0)
		{
			disable_cs_enable(3);
			cs_enable3 = false;
		}
	return count;
}

static ssize_t cs_enable3Show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	cs_enable3 = is_cs_enable(3);
    if (cs_enable3)
        strcpy(buffer, "true\n");
    else
        strcpy(buffer, "false\n");
    return strlen(buffer);
}

static struct kobj_attribute cs_enable3Attr = __ATTR(cs_enable3, 0664, cs_enable3Show, cs_enable3Store);

// WRITE TX FIFO
static uint tx_fifo = 0;
module_param(tx_fifo, uint, S_IRUGO);
MODULE_PARM_DESC(tx_fifo, " Write data to Fifo");

static ssize_t tx_fifoStore(struct kobject *kobj, struct kobj_attribute *attr, const char *buffer, size_t count)
{
    int result = kstrtouint(buffer, 0, &tx_fifo);
    if (result == 0)
    	set_tx_data(tx_fifo);
    return count;
}

static struct kobj_attribute tx_fifoAttr = __ATTR(tx_fifo, 0664, NULL, tx_fifoStore);

// READ RX FIFO
static uint rx_fifo = 0;
module_param(rx_fifo, uint, S_IRUGO);
MODULE_PARM_DESC(rx_fifo, " Read data from Fifo");

static ssize_t rx_fifoShow(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
	rx_fifo = get_rx_data();
    return sprintf(buffer, "%d\n", rx_fifo);
}

static struct kobj_attribute rx_fifoAttr = __ATTR(rx_fifo, 0444, rx_fifoShow, NULL);

//-----------------------------------------------------------------------------
// Attributes
//-----------------------------------------------------------------------------

static struct attribute *attrs0[] = {&baud_rateAttr.attr, NULL};
static struct attribute *attrs1[] = {&word_sizeAttr.attr, NULL};
static struct attribute *attrs2[] = {&cs_selectAttr.attr, NULL};
static struct attribute *attrs3[] = {&mode0Attr.attr, &cs_auto0Attr.attr, &cs_enable0Attr.attr, NULL};
static struct attribute *attrs4[] = {&mode1Attr.attr, &cs_auto1Attr.attr, &cs_enable1Attr.attr, NULL};
static struct attribute *attrs5[] = {&mode2Attr.attr, &cs_auto2Attr.attr, &cs_enable2Attr.attr, NULL};
static struct attribute *attrs6[] = {&mode3Attr.attr, &cs_auto3Attr.attr, &cs_enable3Attr.attr, NULL};
static struct attribute *attrs7[] = {&tx_fifoAttr.attr, NULL};
static struct attribute *attrs8[] = {&rx_fifoAttr.attr, NULL};

static struct attribute_group group0 =
{
    .name = "baud_rate",
    .attrs = attrs0
};

static struct attribute_group group1 =
{
    .name = "word_size",
    .attrs = attrs1
};

static struct attribute_group group2 =
{
    .name = "cs_select",
    .attrs = attrs2
};

static struct attribute_group group3 =
{
    .name = "spi0",
    .attrs = attrs3
};

static struct attribute_group group4 =
{
    .name = "spi1",
    .attrs = attrs4
};

static struct attribute_group group5 =
{
    .name = "spi2",
    .attrs = attrs5
};

static struct attribute_group group6 =
{
    .name = "spi3",
    .attrs = attrs6
};

static struct attribute_group group7 =
{
    .name = "tx_data",
    .attrs = attrs7
};

static struct attribute_group group8 =
{
    .name = "rx_data",
    .attrs = attrs8
};

static struct kobject *kobj;

//-----------------------------------------------------------------------------
// Initialization and Exit
//-----------------------------------------------------------------------------

static int __init initialize_module(void)
{
    int result;

    printk(KERN_INFO "SPI driver: starting\n");

    // Create qe directory under /sys/kernel
    kobj = kobject_create_and_add("spi", kernel_kobj);
    if (!kobj)
    {
        printk(KERN_ALERT "SPI driver: failed to create and add kobj\n");
        return -ENOENT;
    }

    // Create baudrate, word_size, cs_select, spi0, spi1, spi2 and spi3 groups
    result = sysfs_create_group(kobj, &group0);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group1);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group2);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group3);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group4);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group5);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group6);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group7);
    if (result !=0)
        return result;
    result = sysfs_create_group(kobj, &group8);
    if (result !=0)
        return result;

    // Physical to virtual memory map to access gpio registers
    base = (unsigned int*)ioremap_nocache(LW_BRIDGE_BASE + SPI_BASE_OFFSET, SPAN_IN_BYTES);
    if (base == NULL)
        return -ENODEV;

    printk(KERN_INFO "SPI driver: initialized\n");

    return 0;
}

static void __exit exit_module(void)
{
    kobject_put(kobj);
    printk(KERN_INFO "SPI driver: exit\n");
}

module_init(initialize_module);
module_exit(exit_module);


