/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/slab.h>

#include "flashlight-core.h"
#include "flashlight-dt.h"

/*include for pio*/
#include <mt-plat/mt_gpio.h>
#include <mach/gpio_const.h>

/* define device tree */
#ifndef LM3646_DTNAME
#define LM3646_DTNAME "mediatek,flashlights_lm3646"
#endif
#ifndef LM3646_DTNAME_I2C
#define LM3646_DTNAME_I2C "mediatek,strobe_main"
#endif

#define LM3646_NAME "flashlights-lm3646"

/* define level */
#define LM3646_LEVEL_NUM 1
#define LM3646_LEVEL_TORCH 1
#define LM3646_HW_TIMEOUT 1000 /* ms */

/*define pins*/
#define IC_ENABLE_PIN       (GPIO10 | 0x80000000)  //GPIO_CAMERA_FLASH_EN_PIN
#define FLASH_ENABLE_PIN    (GPIO4 | 0x80000000)   //GPIO_CAMERA_FLASH_MODE_PIN
#define TORCH_ENABLE_PIN    (GPIO21 | 0x80000000)  //GPIO_TORCH_EN

/* define mutex and work queue */
static DEFINE_MUTEX(lm3646_mutex);
static struct work_struct lm3646_work;

/* define usage count */
static int use_count;

/* define i2c */
static struct i2c_client *lm3646_i2c_client;

/* platform data */
struct lm3646_platform_data {
	int channel_num;
	struct flashlight_device_id *dev_id;
};

/* lm3646 chip data */
struct lm3646_chip_data {
	struct i2c_client *client;
	struct lm3646_platform_data *pdata;
	struct mutex lock;
};


/******************************************************************************
 * lm3646 operations
 *****************************************************************************/

static int lm3646_level = -1;

static int lm3646_is_torch(int level)
{
	if (level >= LM3646_LEVEL_TORCH)
		return -1;

	return 0;
}

/* i2c wrapper function */
static int LM3642_write_reg(struct i2c_client *client, u8 reg, u8 val)
{
	int ret;
	struct lm3646_chip_data *chip = i2c_get_clientdata(client);

	mutex_lock(&chip->lock);
	ret = i2c_smbus_write_byte_data(client, reg, val);
	mutex_unlock(&chip->lock);

	if (ret < 0)
		pr_err("failed writing at 0x%02x\n", reg);

	return ret;
}

/* flashlight enable function */
static int lm3646_enable(void)
{
	unsigned char reg = 0, val = 0;

	mt_set_gpio_mode(IC_ENABLE_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(IC_ENABLE_PIN,GPIO_DIR_OUT);
	mt_set_gpio_out(IC_ENABLE_PIN,GPIO_OUT_ONE);

	if (!lm3646_is_torch(lm3646_level)) {
		/*toucrh mode*/
		mt_set_gpio_mode(FLASH_ENABLE_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(FLASH_ENABLE_PIN,GPIO_DIR_OUT);
		mt_set_gpio_out(FLASH_ENABLE_PIN,GPIO_OUT_ZERO);
		mt_set_gpio_mode(TORCH_ENABLE_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(TORCH_ENABLE_PIN,GPIO_DIR_OUT);
		mt_set_gpio_out(TORCH_ENABLE_PIN,GPIO_OUT_ONE);
		
        reg=7;
        val=0x8F;        // 200 ma
        LM3642_write_reg(lm3646_i2c_client, reg , val);
        
		reg=1;
        val=6;  //enable software  torch  mode
        LM3642_write_reg(lm3646_i2c_client, val , reg);

	} else {
		/*flashlight mode*/
  		mt_set_gpio_mode(FLASH_ENABLE_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(FLASH_ENABLE_PIN,GPIO_DIR_OUT);
		mt_set_gpio_out(FLASH_ENABLE_PIN,GPIO_OUT_ONE);
		mt_set_gpio_mode(TORCH_ENABLE_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(TORCH_ENABLE_PIN,GPIO_DIR_OUT);
		mt_set_gpio_out(TORCH_ENABLE_PIN,GPIO_OUT_ZERO);
		
		reg=6;
        val=0x53;        // 1 a
        LM3642_write_reg(lm3646_i2c_client, reg , val);
        
		reg=1;
        val=7;  //enable software  flash  mode
        LM3642_write_reg(lm3646_i2c_client, val , reg);
	}

	return 0;
}

/* flashlight disable function */
static int lm3646_disable(void)
{
	unsigned char reg = 0, val = 0;
		reg=1;
		val=0x00;
		
		LM3642_write_reg(lm3646_i2c_client, val , reg);

		mt_set_gpio_mode(IC_ENABLE_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(IC_ENABLE_PIN,GPIO_DIR_OUT);
		mt_set_gpio_out(IC_ENABLE_PIN,GPIO_OUT_ZERO);
		mt_set_gpio_mode(FLASH_ENABLE_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(FLASH_ENABLE_PIN,GPIO_DIR_OUT);
		mt_set_gpio_out(FLASH_ENABLE_PIN,GPIO_OUT_ZERO);
		mt_set_gpio_mode(TORCH_ENABLE_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(TORCH_ENABLE_PIN,GPIO_DIR_OUT);
		mt_set_gpio_out(TORCH_ENABLE_PIN,GPIO_OUT_ZERO);

	return 0;
}

/* set flashlight level */
static int lm3646_set_level(int level)
{
  lm3646_level = level;
	return 0;
}

/* flashlight init */
static int lm3646_init(void)
{
	return 0;
}

/* flashlight uninit */
static int lm3646_uninit(void)
{
	lm3646_disable();
	return 0;
}

/******************************************************************************
 * Timer and work queue
 *****************************************************************************/
static struct hrtimer lm3646_timer;
static unsigned int lm3646_timeout_ms;

static void lm3646_work_disable(struct work_struct *data)
{
	pr_debug("work queue callback\n");
	lm3646_disable();
}

static enum hrtimer_restart lm3646_timer_func(struct hrtimer *timer)
{
	schedule_work(&lm3646_work);
	return HRTIMER_NORESTART;
}


/******************************************************************************
 * Flashlight operations
 *****************************************************************************/
static int lm3646_ioctl(unsigned int cmd, unsigned long arg)
{
	struct flashlight_dev_arg *fl_arg;
	int channel;
	ktime_t ktime;
	unsigned int s;
	unsigned int ns;

	fl_arg = (struct flashlight_dev_arg *)arg;
	channel = fl_arg->channel;

	switch (cmd) {
	case FLASH_IOC_SET_TIME_OUT_TIME_MS:
		pr_debug("FLASH_IOC_SET_TIME_OUT_TIME_MS(%d): %d\n",
				channel, (int)fl_arg->arg);
		lm3646_timeout_ms = fl_arg->arg;
		break;

	case FLASH_IOC_SET_DUTY:
		pr_debug("FLASH_IOC_SET_DUTY(%d): %d\n",
				channel, (int)fl_arg->arg);
		lm3646_set_level(fl_arg->arg);
		break;

	case FLASH_IOC_SET_ONOFF:
		pr_debug("FLASH_IOC_SET_ONOFF(%d): %d\n",
				channel, (int)fl_arg->arg);
		if (fl_arg->arg == 1) {
			if (lm3646_timeout_ms) {
				s = lm3646_timeout_ms / 1000;
				ns = lm3646_timeout_ms % 1000 * 1000000;
				ktime = ktime_set(s, ns);
				hrtimer_start(&lm3646_timer, ktime, HRTIMER_MODE_REL);
			}
			lm3646_enable();
		} else {
			lm3646_disable();
			hrtimer_cancel(&lm3646_timer);
		}
		break;
		
	case FLASH_IOC_GET_DUTY_NUMBER:
		pr_debug("FLASH_IOC_GET_DUTY_NUMBER(%d)\n", channel);
		fl_arg->arg = LM3646_LEVEL_NUM;
		break;

	case FLASH_IOC_GET_MAX_TORCH_DUTY:
		pr_debug("FLASH_IOC_GET_MAX_TORCH_DUTY(%d)\n", channel);
		fl_arg->arg = LM3646_LEVEL_TORCH - 1;
		break;

	case FLASH_IOC_GET_HW_TIMEOUT:
		pr_debug("FLASH_IOC_GET_HW_TIMEOUT(%d)\n", channel);
		fl_arg->arg = LM3646_HW_TIMEOUT;
		break;
	default:
		pr_info("No such command and arg(%d): (%d, %d)\n",
				channel, _IOC_NR(cmd), (int)fl_arg->arg);
		return -ENOTTY;
	}

	return 0;
}

static int lm3646_open(void)
{
	/* Move to set driver for saving power */
	return 0;
}

static int lm3646_release(void)
{
	/* Move to set driver for saving power */
	return 0;
}

static int lm3646_set_driver(int set)
{
	int ret = 0;

	/* set chip and usage count */
	mutex_lock(&lm3646_mutex);
	if (set) {
		if (!use_count)
			ret = lm3646_init();
		use_count++;
		pr_debug("Set driver: %d\n", use_count);
	} else {
		use_count--;
		if (!use_count)
			ret = lm3646_uninit();
		if (use_count < 0)
			use_count = 0;
		pr_debug("Unset driver: %d\n", use_count);
	}
	mutex_unlock(&lm3646_mutex);

	return ret;
}

static ssize_t lm3646_strobe_store(struct flashlight_arg arg)
{
	lm3646_set_driver(1);
	lm3646_set_level(arg.level);
	lm3646_timeout_ms = 0;
	lm3646_enable();
	msleep(arg.dur);
	lm3646_disable();
	lm3646_set_driver(0);

	return 0;
}

static struct flashlight_operations lm3646_ops = {
	lm3646_open,
	lm3646_release,
	lm3646_ioctl,
	lm3646_strobe_store,
	lm3646_set_driver
};


/******************************************************************************
 * I2C device and driver
 *****************************************************************************/
static int lm3646_chip_init(struct lm3646_chip_data *chip)
{
	/* NOTE: Chip initialication move to "set driver" for power saving.
	 * lm3646_init();
	 */

	return 0;
}

static int lm3646_parse_dt(struct device *dev,
		struct lm3646_platform_data *pdata)
{
	struct device_node *np, *cnp;
	u32 decouple = 0;
	int i = 0;

	if (!dev || !dev->of_node || !pdata)
		return -ENODEV;

	np = dev->of_node;

	pdata->channel_num = of_get_child_count(np);
	if (!pdata->channel_num) {
		pr_info("Parse no dt, node.\n");
		return 0;
	}
	pr_info("Channel number(%d).\n", pdata->channel_num);

	if (of_property_read_u32(np, "decouple", &decouple))
		pr_info("Parse no dt, decouple.\n");

	pdata->dev_id = devm_kzalloc(dev,
			pdata->channel_num *
			sizeof(struct flashlight_device_id),
			GFP_KERNEL);
	if (!pdata->dev_id)
		return -ENOMEM;

	for_each_child_of_node(np, cnp) {
		if (of_property_read_u32(cnp, "type", &pdata->dev_id[i].type))
			goto err_node_put;
		if (of_property_read_u32(cnp, "ct", &pdata->dev_id[i].ct))
			goto err_node_put;
		if (of_property_read_u32(cnp, "part", &pdata->dev_id[i].part))
			goto err_node_put;
		snprintf(pdata->dev_id[i].name, FLASHLIGHT_NAME_SIZE,
				LM3646_NAME);
		pdata->dev_id[i].channel = i;
		pdata->dev_id[i].decouple = decouple;

		pr_info("Parse dt (type,ct,part,name,channel,decouple)=(%d,%d,%d,%s,%d,%d).\n",
				pdata->dev_id[i].type, pdata->dev_id[i].ct,
				pdata->dev_id[i].part, pdata->dev_id[i].name,
				pdata->dev_id[i].channel,
				pdata->dev_id[i].decouple);
		i++;
	}

	return 0;

err_node_put:
	of_node_put(cnp);
	return -EINVAL;
}

static int lm3646_i2c_probe(
		struct i2c_client *client, const struct i2c_device_id *id)
{
	struct lm3646_chip_data *chip;
	int err;

	pr_debug("Probe start.\n");

	/* check i2c */
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("Failed to check i2c functionality.\n");
		err = -ENODEV;
		goto err_out;
	}

	/* init chip private data */
	chip = kzalloc(sizeof(struct lm3646_chip_data), GFP_KERNEL);
	if (!chip) {
		err = -ENOMEM;
		goto err_out;
	}
	chip->client = client;

	i2c_set_clientdata(client, chip);
	lm3646_i2c_client = client;

	/* init mutex and spinlock */
	mutex_init(&chip->lock);

	/* init chip hw */
	lm3646_chip_init(chip);

	pr_debug("Probe done.\n");

	return 0;

err_out:
	return err;
}

static int lm3646_i2c_remove(struct i2c_client *client)
{
	struct lm3646_chip_data *chip = i2c_get_clientdata(client);

	pr_debug("Remove start.\n");

	client->dev.platform_data = NULL;

	/* free resource */
	kfree(chip);

	pr_debug("Remove done.\n");

	return 0;
}

static const struct i2c_device_id lm3646_i2c_id[] = {
	{LM3646_NAME, 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, lm3646_i2c_id);

#ifdef CONFIG_OF
static const struct of_device_id lm3646_i2c_of_match[] = {
	{.compatible = LM3646_DTNAME_I2C},
	{},
};
MODULE_DEVICE_TABLE(of, lm3646_i2c_of_match);
#endif

static struct i2c_driver lm3646_i2c_driver = {
	.driver = {
		.name = LM3646_NAME,
#ifdef CONFIG_OF
		.of_match_table = lm3646_i2c_of_match,
#endif
	},
	.probe = lm3646_i2c_probe,
	.remove = lm3646_i2c_remove,
	.id_table = lm3646_i2c_id,
};

/******************************************************************************
 * Platform device and driver
 *****************************************************************************/
static int lm3646_probe(struct platform_device *pdev)
{
	struct lm3646_platform_data *pdata = dev_get_platdata(&pdev->dev);
	struct lm3646_chip_data *chip = NULL;
	int err;
	int i;

	pr_debug("Probe start.\n");

	if (i2c_add_driver(&lm3646_i2c_driver)) {
		pr_debug("Failed to add i2c driver.\n");
		return -1;
	}

	/* init platform data */
	if (!pdata) {
		pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
		if (!pdata) {
			err = -ENOMEM;
			goto err_free;
		}
		pdev->dev.platform_data = pdata;
		err = lm3646_parse_dt(&pdev->dev, pdata);
		if (err)
			goto err_free;
	}

	/* init work queue */
	INIT_WORK(&lm3646_work, lm3646_work_disable);

	/* init timer */
	lm3646_timeout_ms = 1000;
	hrtimer_init(&lm3646_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	lm3646_timer.function = lm3646_timer_func;

	/* clear usage count */
	use_count = 0;

	/* register flashlight device */
	if (pdata->channel_num) {
		for (i = 0; i < pdata->channel_num; i++)
			if (flashlight_dev_register_by_device_id(
						&pdata->dev_id[i],
						&lm3646_ops)) {
				err = -EFAULT;
				goto err_free;
			}
	} else {
		if (flashlight_dev_register(LM3646_NAME, &lm3646_ops)) {
			err = -EFAULT;
			goto err_free;
		}
	}

	pr_debug("Probe done.\n");

	return 0;
err_free:
	chip = i2c_get_clientdata(lm3646_i2c_client);
	i2c_set_clientdata(lm3646_i2c_client, NULL);
	kfree(chip);
	return err;
}

static int lm3646_remove(struct platform_device *pdev)
{
	struct lm3646_platform_data *pdata = dev_get_platdata(&pdev->dev);
	int i;

	pr_debug("Remove start.\n");

	i2c_del_driver(&lm3646_i2c_driver);

	pdev->dev.platform_data = NULL;

	/* unregister flashlight device */
	if (pdata && pdata->channel_num)
		for (i = 0; i < pdata->channel_num; i++)
			flashlight_dev_unregister_by_device_id(
					&pdata->dev_id[i]);
	else
		flashlight_dev_unregister(LM3646_NAME);

	/* flush work queue */
	flush_work(&lm3646_work);

	pr_debug("Remove done.\n");

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lm3646_of_match[] = {
	{.compatible = LM3646_DTNAME},
	{},
};
MODULE_DEVICE_TABLE(of, lm3646_of_match);
#else
static struct platform_device lm3646_platform_device[] = {
	{
		.name = LM3646_NAME,
		.id = 0,
		.dev = {}
	},
	{}
};
MODULE_DEVICE_TABLE(platform, lm3646_platform_device);
#endif

static struct platform_driver lm3646_platform_driver = {
	.probe = lm3646_probe,
	.remove = lm3646_remove,
	.driver = {
		.name = LM3646_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = lm3646_of_match,
#endif
	},
};

static int __init flashlight_lm3646_init(void)
{
	int ret;

	pr_debug("Init start.\n");

#ifndef CONFIG_OF
	ret = platform_device_register(&lm3646_platform_device);
	if (ret) {
		pr_info("Failed to register platform device\n");
		return ret;
	}
#endif

	ret = platform_driver_register(&lm3646_platform_driver);
	if (ret) {
		pr_info("Failed to register platform driver\n");
		return ret;
	}

	pr_debug("Init done.\n");

	return 0;
}

static void __exit flashlight_lm3646_exit(void)
{
	pr_debug("Exit start.\n");

	platform_driver_unregister(&lm3646_platform_driver);

	pr_debug("Exit done.\n");
}

module_init(flashlight_lm3646_init);
module_exit(flashlight_lm3646_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Simon Wang <Simon-TCH.Wang@mediatek.com>");
MODULE_DESCRIPTION("MTK Flashlight LM3646 Driver");

