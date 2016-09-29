/*
 * LEDs driver for PCEngines apu2
 *
 * Copyright (C) 2016 Christian Herzog <daduke@daduke.org>, based on
 * Petr Leibman's leds-alix
 * Based on leds-wrap.c
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>
#include <asm/io.h>
#include <linux/io.h>

#define DRVNAME		"apu2-led"

#define FCH_ACPI_MMIO_BASE      0xFED80000
#define FCH_GPIO_BASE           (FCH_ACPI_MMIO_BASE + 0x1500)
#define FCH_GPIO_SIZE           0x300
#define GPIO_BIT_WRITE          22
static DEFINE_SPINLOCK ( gpio_lock);

#define APU2_NUM_GPIO            4
static u8 gpio_offset[APU2_NUM_GPIO] = {89, 68, 69, 70};

static void __iomem *gpio_addr[APU2_NUM_GPIO] = {NULL, NULL, NULL, NULL};

static struct platform_device *pdev;

static void apu2_led_set_1(struct led_classdev *led_cdev, enum led_brightness value) {
	u32 val;

	spin_lock_bh (&gpio_lock);

	val = ioread32 (gpio_addr[1]);

	if (value)
		val &= ~BIT(GPIO_BIT_WRITE);
	else
		val |= BIT(GPIO_BIT_WRITE);

	iowrite32 (val, gpio_addr[1]);

	spin_unlock_bh (&gpio_lock);
}

static void apu2_led_set_2(struct led_classdev *led_cdev, enum led_brightness value) {
	u32 val;

	spin_lock_bh (&gpio_lock);

	val = ioread32 (gpio_addr[2]);

	if (value)
		val &= ~BIT(GPIO_BIT_WRITE);
	else
		val |= BIT(GPIO_BIT_WRITE);

	iowrite32 (val, gpio_addr[2]);

	spin_unlock_bh (&gpio_lock);
}

static void apu2_led_set_3(struct led_classdev *led_cdev, enum led_brightness value) {
	u32 val;

	spin_lock_bh (&gpio_lock);

	val = ioread32 (gpio_addr[3]);

	if (value)
		val &= ~BIT(GPIO_BIT_WRITE);
	else
		val |= BIT(GPIO_BIT_WRITE);

	iowrite32 (val, gpio_addr[3]);

	spin_unlock_bh (&gpio_lock);
}

static struct led_classdev apu2_led_1 = {
	.name		= "apu2:1",
	.brightness_set	= apu2_led_set_1,
};

static struct led_classdev apu2_led_2 = {
	.name		= "apu2:2",
	.brightness_set	= apu2_led_set_2,
};

static struct led_classdev apu2_led_3 = {
	.name		= "apu2:3",
	.brightness_set	= apu2_led_set_3,
};

#ifdef CONFIG_PM
static int apu2_led_suspend(struct platform_device *dev,
		pm_message_t state)
{
	led_classdev_suspend(&apu2_led_1);
	led_classdev_suspend(&apu2_led_2);
	led_classdev_suspend(&apu2_led_3);
	return 0;
}

static int apu2_led_resume(struct platform_device *dev)
{
	led_classdev_resume(&apu2_led_1);
	led_classdev_resume(&apu2_led_2);
	led_classdev_resume(&apu2_led_3);
	return 0;
}
#else
#define apu2_led_suspend NULL
#define apu2_led_resume NULL
#endif

static int apu2_led_probe(struct platform_device *pdev)
{
	int ret;

	ret = led_classdev_register(&pdev->dev, &apu2_led_1);
	if (ret == 0)
	{
		ret = led_classdev_register(&pdev->dev, &apu2_led_2);
		if (ret >= 0)
		{
			ret = led_classdev_register(&pdev->dev, &apu2_led_3);
			if (ret < 0)
				led_classdev_unregister(&apu2_led_2);
		}
		if (ret < 0)
			led_classdev_unregister(&apu2_led_1);
	}
	return ret;
}

static int apu2_led_remove(struct platform_device *pdev)
{
	led_classdev_unregister(&apu2_led_1);
	led_classdev_unregister(&apu2_led_2);
	led_classdev_unregister(&apu2_led_3);
	
	return 0;
}

static struct platform_driver apu2_led_driver = {
	.probe		= apu2_led_probe,
	.remove		= apu2_led_remove,
	.suspend	= apu2_led_suspend,
	.resume		= apu2_led_resume,
	.driver		= {
	.name		= DRVNAME,
	.owner		= THIS_MODULE,
	},
};

static int __init apu2_led_init(void)
{
	int ret;
	int i;

	ret = platform_driver_register(&apu2_led_driver);
	if (ret < 0)
		goto out;

	pdev = platform_device_register_simple(DRVNAME, -1, NULL, 0);
	if (IS_ERR(pdev)) {
		ret = PTR_ERR(pdev);
		platform_driver_unregister(&apu2_led_driver);
		goto out;
	}

	for (i = 0; i < APU2_NUM_GPIO; i++) {
		gpio_addr[i] = ioremap ( FCH_GPIO_BASE + (gpio_offset[i] * sizeof (u32)), sizeof (u32));
	}

out:
	return ret;
}

static void __exit apu2_led_exit(void)
{
	platform_device_unregister(pdev);
	platform_driver_unregister(&apu2_led_driver);
}

module_init(apu2_led_init);
module_exit(apu2_led_exit);

MODULE_AUTHOR("Christian Herzog");
MODULE_DESCRIPTION("PCEngines apu2 LED driver");
MODULE_LICENSE("GPL");
