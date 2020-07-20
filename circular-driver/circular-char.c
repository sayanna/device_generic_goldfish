#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <asm/uaccess.h>

#define BUF_SIZE 200

static char buf[BUF_SIZE];
static char *read_ptr;
static char *write_ptr;
static int readCount = 0;
static int writeCount = 0;
static struct timeval ts;

#define CMD_BASE                  4200
#define CMD_ZERO_OUT              CMD_BASE + 0
#define CMD_IS_NEWDATA            CMD_BASE + 1
#define CMD_IS_GET_LAST_TS        CMD_BASE + 2
#define CMD_IS_GET_READ_STAT      CMD_BASE + 3
#define CMD_IS_GET_WRITE_STAT     CMD_BASE + 4
#define CMD_IS_SET_BUFFER_TO_CHAR CMD_BASE + 5

static int device_open(struct inode *inode, struct file *file)
{
  printk("device_open called \n");

  return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
  printk("device_release called \n");

  return 0;
}

static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
  int chars_read = 0;

  printk("device_read called \n");

  while(length && *read_ptr && (read_ptr != write_ptr)) {
    put_user(*(read_ptr++), buffer++);

    printk("Reading %c \n", *read_ptr);

    if(read_ptr >= buf + BUF_SIZE)
      read_ptr = buf;

    chars_read++;
    length--;
  }

  readCount++;
  return chars_read;
}

static void set_buffer_to_char(char ch)
{


}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
  int i;

  printk("device_write called \n");

  for(i = 0; i < len; i++) {
    get_user(*write_ptr, buff++);
    printk("Writing %c \n", *write_ptr);
    write_ptr++;
    if (write_ptr >= buf + BUF_SIZE)
      write_ptr = buf;
  }

  do_gettimeofday(&ts);
  printk("current ts = %lld", ts.tv_sec);
  writeCount++;
  return len;
}

static int
device_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  printk("device_unlocked_ioctl called with cmd = %d arg = %ld", cmd, arg);

  switch (cmd) {
    case CMD_ZERO_OUT:
    {
      read_ptr = write_ptr;
      break;      
    }

    case CMD_IS_NEWDATA:
    {
      int *buffer = (int*)arg;
      int result = (read_ptr == write_ptr) ? 0 : 1;
      put_user(result, buffer);
      break;
    }

    case CMD_IS_GET_LAST_TS:
    {
      int64_t *buffer = (int64_t*)arg;
      put_user(ts.tv_sec, buffer);
      break;
    }

    case CMD_IS_GET_READ_STAT:
    {
      int *buffer = (int*)arg;
      put_user(readCount, buffer);
      break;
    }

    case CMD_IS_GET_WRITE_STAT:
    {
      int *buffer = (int*)arg;
      put_user(writeCount, buffer);
      break;
    }

    case CMD_IS_SET_BUFFER_TO_CHAR:
    {
      char *buffer = (char*)arg;
      char ch;
      int count = 0;

      get_user(ch, buffer);
      printk("ch = %c", ch);

      while (count++ < sizeof(buf)) {
        buf[count] = ch;
      }
      break;
    }
     
    default:
      printk("device_unlocked_ioctl: unsupported command = %d\n", cmd);
      break;
  }

  return 0;
}

static struct file_operations fops = {
  .open = device_open,
  .release = device_release,
  .read = device_read,
  .write = device_write,
  .unlocked_ioctl = device_unlocked_ioctl
};

static struct miscdevice circ_char_misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "circchar",
  .fops = &fops,
};

int circ_char_enter(void)
{
  int retval;

  retval = misc_register(&circ_char_misc);
  printk("CIRC Driver got retval %d\n", retval);
  printk("mmap is %08X\n", (int) fops.mmap);

  read_ptr = buf;
  write_ptr = buf;

  return 0;
}

void circ_char_exit(void)
{
  misc_deregister(&circ_char_misc);
}

module_init(circ_char_enter);
module_exit(circ_char_exit);
