/*
UEI_IRRC_DRIVER_FOR_MSM9860
*/

#include <linux/err.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/sysdev.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/version.h>
#include <mach/gpiomux.h>
#include <linux/of_gpio.h>
#include <mach/msm_iomap.h>
#include <linux/regulator/consumer.h>
#include "lge_uei_irrc.h"

struct uei_irrc_pdata_type irrc_data;

#ifdef CONFIG_OF
static int irrc_parse_dt(struct device *dev, struct uei_irrc_pdata_type *pdata)
{
	struct device_node *np = dev->of_node;
	pdata->reset_gpio = of_get_named_gpio_flags(np, "uei,reset-gpio", 0, NULL);
	printk(KERN_INFO "[IRRC] uei,reset-gpio: %d\n", pdata->reset_gpio);
	return 0;
}
#else
//                             
static int irrc_parse_dt(struct device *dev, struct uei_irrc_pdata_type *pdata)
{
	pdata->reset_gpio = GPIO_IRRC_RESET_N;
	return 0;
}
#endif

static int uei_irrc_probe(struct platform_device *pdev)
{
	int rc = 0;
	printk(KERN_INFO "%s\n", __func__);

	if (pdev->dev.of_node) {
		printk(KERN_INFO "[IRRC] probe: pdev->dev.of-node\n");
		irrc_parse_dt(&pdev->dev, &irrc_data);
	}

	rc = gpio_request(irrc_data.reset_gpio, "irrc_reset_n");
	if (rc) {
		printk(KERN_ERR "%s: irrc_reset_n %d request failed\n",
		__func__, irrc_data.reset_gpio);
		return rc;
	}

	rc = gpio_tlmm_config(GPIO_CFG(irrc_data.reset_gpio, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=fg%d\n",
			__func__, irrc_data.reset_gpio, rc);
		return rc;
	}
	gpio_set_value(irrc_data.reset_gpio, 1);

	return rc;
}

static int uei_irrc_remove(struct platform_device *pdev)
{
	struct uei_irrc_pdata_type *pdata = platform_get_drvdata(pdev);
	printk(KERN_ERR "[IRRC] remove (err:%d)\n", 104);
	pdata = NULL;
	return 0;
}

static void uei_irrc_shutdown(struct platform_device *pdev)
{
	printk(KERN_INFO "%s\n", __func__);
}

static int uei_irrc_suspend(struct platform_device *pdev, pm_message_t state)
{
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}

static int uei_irrc_resume(struct platform_device *pdev)
{
	printk(KERN_INFO "%s\n", __func__);
	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id irrc_match_table[] = {
	{ .compatible = "uei,irrc",},
	{ },
};
#endif

static struct platform_driver uei_irrc_driver = {
	.probe = uei_irrc_probe,
	.remove = uei_irrc_remove,
	.shutdown = uei_irrc_shutdown,
	.suspend = uei_irrc_suspend,
	.resume = uei_irrc_resume,
	.driver = {
		.name = UEI_IRRC_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = irrc_match_table,
#endif
	},
};

static int __init uei_irrc_init(void)
{
	int ret = 0;
	printk(KERN_INFO "%s\n", __func__);
	ret = platform_driver_register(&uei_irrc_driver);
	if (ret)
		printk(KERN_INFO "%s: init fail\n", __func__);
	return ret;
}

static void __exit uei_irrc_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);
	platform_driver_unregister(&uei_irrc_driver);
}

module_init(uei_irrc_init);
module_exit(uei_irrc_exit);

MODULE_AUTHOR("LG Electronics");
MODULE_DESCRIPTION("UEI IrRC Driver");
MODULE_LICENSE("GPL");

