/*
 * a simple char device driver: globalfifo
 *
 * Copyright (C) 2014 Barry Song  (baohua@kernel.org)
 *
 * Licensed under GPLv2 or later.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of_device.h>

#define GLOBALFIFO_SIZE	0x1000
#define FIFO_CLEAR 0x1
#define GLOBALFIFO_MAJOR 231

struct globalfifo_dev {
	unsigned int current_len;
	unsigned char mem[GLOBALFIFO_SIZE];
	struct mutex mutex;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
	struct fasync_struct *async_queue;
	struct miscdevice miscdev;
};

static int globalfifo_fasync(int fd, struct file *filp, int mode)
{
	struct globalfifo_dev *dev = container_of(filp->private_data,
		struct globalfifo_dev, miscdev);

	return fasync_helper(fd, filp, mode, &dev->async_queue);
}

static int globalfifo_open(struct inode *inode, struct file *filp)
{
       return 0;
}

static int globalfifo_release(struct inode *inode, struct file *filp)
{
	globalfifo_fasync(-1, filp, 0);
	return 0;
}

static long globalfifo_ioctl(struct file *filp, unsigned int cmd,
			     unsigned long arg)
{
	struct globalfifo_dev *dev = container_of(filp->private_data,
		struct globalfifo_dev, miscdev);

	switch (cmd) {
	case FIFO_CLEAR:
		mutex_lock(&dev->mutex);
		dev->current_len = 0;
		memset(dev->mem, 0, GLOBALFIFO_SIZE);
		mutex_unlock(&dev->mutex);

		printk(KERN_INFO "globalfifo is set to zero\n");
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

static unsigned int globalfifo_poll(struct file *filp, poll_table * wait)
{
	unsigned int mask = 0;
	struct globalfifo_dev *dev = container_of(filp->private_data,
		struct globalfifo_dev, miscdev);

	mutex_lock(&dev->mutex);

	poll_wait(filp, &dev->r_wait, wait);
	poll_wait(filp, &dev->w_wait, wait);

	if (dev->current_len != 0) {
		mask |= POLLIN | POLLRDNORM;
	}

	if (dev->current_len != GLOBALFIFO_SIZE) {
		mask |= POLLOUT | POLLWRNORM;
	}

	mutex_unlock(&dev->mutex);
	return mask;
}

static ssize_t globalfifo_read(struct file *filp, char __user *buf,
			       size_t count, loff_t *ppos)
{
	int ret;
	struct globalfifo_dev *dev = container_of(filp->private_data,
		struct globalfifo_dev, miscdev);

	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->r_wait, &wait);

	while (dev->current_len == 0) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);

		schedule();
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			goto out2;
		}

		mutex_lock(&dev->mutex);
	}

	if (count > dev->current_len)
		count = dev->current_len;

	if (copy_to_user(buf, dev->mem, count)) {
		ret = -EFAULT;
		goto out;
	} else {
		memcpy(dev->mem, dev->mem + count, dev->current_len - count);
		dev->current_len -= count;
		printk(KERN_INFO "read %d bytes(s),current_len:%d\n", count,
		       dev->current_len);

		wake_up_interruptible(&dev->w_wait);

		ret = count;
	}
 out:
	mutex_unlock(&dev->mutex);
 out2:
	remove_wait_queue(&dev->r_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}

static ssize_t globalfifo_write(struct file *filp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct globalfifo_dev *dev = container_of(filp->private_data,
		struct globalfifo_dev, miscdev);

	int ret;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->w_wait, &wait);

	while (dev->current_len == GLOBALFIFO_SIZE) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);

		mutex_unlock(&dev->mutex);

		schedule();
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			goto out2;
		}

		mutex_lock(&dev->mutex);
	}

	if (count > GLOBALFIFO_SIZE - dev->current_len)
		count = GLOBALFIFO_SIZE - dev->current_len;

	if (copy_from_user(dev->mem + dev->current_len, buf, count)) {
		ret = -EFAULT;
		goto out;
	} else {
		dev->current_len += count;
		printk(KERN_INFO "written %d bytes(s),current_len:%d\n", count,
		       dev->current_len);

		wake_up_interruptible(&dev->r_wait);

		if (dev->async_queue) {
			kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
			printk(KERN_DEBUG "%s kill SIGIO\n", __func__);
		}

		ret = count;
	}

 out:
	mutex_unlock(&dev->mutex);
 out2:
	remove_wait_queue(&dev->w_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}

static const struct file_operations globalfifo_fops = {
	.owner = THIS_MODULE,
	.read = globalfifo_read,
	.write = globalfifo_write,
	.unlocked_ioctl = globalfifo_ioctl,
	.poll = globalfifo_poll,
	.fasync = globalfifo_fasync,
	.open = globalfifo_open,
	.release = globalfifo_release,
};

static int globalfifo_probe(struct platform_device *pdev)
{
	struct globalfifo_dev *gl;
	int ret;

	gl = devm_kzalloc(&pdev->dev, sizeof(*gl), GFP_KERNEL);
	if (!gl)
		return -ENOMEM;
	gl->miscdev.minor = MISC_DYNAMIC_MINOR;
	gl->miscdev.name = "globalfifo";
	gl->miscdev.fops = &globalfifo_fops;

	mutex_init(&gl->mutex);
	init_waitqueue_head(&gl->r_wait);
	init_waitqueue_head(&gl->w_wait);
	platform_set_drvdata(pdev, gl);

	ret = misc_register(&gl->miscdev);
	if (ret < 0)
		goto err;

	dev_info(&pdev->dev, "globalfifo drv probed\n");
	return 0;
err:
	return ret;
}

static int globalfifo_remove(struct platform_device *pdev)
{
	struct globalfifo_dev *gl = platform_get_drvdata(pdev);

	misc_deregister(&gl->miscdev);

	dev_info(&pdev->dev, "globalfifo drv removed\n");
	return 0;
}

static struct platform_driver globalfifo_driver = {
	.driver = {
		.name = "globalfifo",
		.owner = THIS_MODULE,
	},
	.probe = globalfifo_probe,
	.remove = globalfifo_remove,
};

module_platform_driver(globalfifo_driver);

MODULE_AUTHOR("Barry Song <baohua@kernel.org>");
MODULE_LICENSE("GPL v2");
