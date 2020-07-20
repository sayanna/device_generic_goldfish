#include <errno.h>

#define  LOG_TAG  "opersyshw_hal_module"
#include <log/log.h>
#include <cutils/sockets.h>
#include <hardware/opersyshw.h>
#include <utils/Trace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define   OPERSYSHW_DEBUG   1

#define CMD_BASE                  4200
#define CMD_ZERO_OUT              CMD_BASE + 0
#define CMD_IS_NEWDATA            CMD_BASE + 1
#define CMD_IS_GET_LAST_TS        CMD_BASE + 2
#define CMD_IS_GET_READ_STAT      CMD_BASE + 3
#define CMD_IS_GET_WRITE_STAT     CMD_BASE + 4
#define CMD_IS_SET_BUFFER_TO_CHAR CMD_BASE + 5


#if OPERSYSHW_DEBUG
#  define D(...)   ALOGD(__VA_ARGS__)
#else
#  define D(...)   ((void)0)
#endif

static int fd = -1;

int opersyshw__read(char* buffer, int length)
{
    size_t ret;
    D("OPERSYS HW - read()for %d bytes called", length);

    if (fd < 0) {
        D("OPERSYS HW - read() - Err fd is not set");        
        return -1;
    }

    return (int)read(fd, (void*)buffer, length);


    // TODO -- IMPLEMENT ME

    // THIS IS STUBBED -- NEED TO BE REPLACED
    // return 0;
}

int opersyshw__write(char* buffer, int length)
{
    D("OPERSYS HW - write()for %d bytes called", length);

    if (fd < 0) {
        D("OPERSYS HW - read() - Err fd is not set");        
        return -1;
    }

    ATRACE_

    return (int)write(fd, (void*)buffer, length);

    // TODO -- IMPLEMENT ME

    // THIS IS STUBBED -- NEED TO BE REPLACED
    // return length;
}

int opersyshw__test(int value)
{
    return value;
}

void opersyshw__zeroout()
{
    D("OPERSYS HW - zeroout called");
    ioctl(fd, CMD_ZERO_OUT);
}

int opersyshw__isthrenewdata()
{
    int result = 0;
    D("OPERSYS HW - istherenewdata called");

    ioctl(fd, CMD_IS_NEWDATA, &result);
    D("OPERSYS HW - istherenewdata result = %d ", result);

    return result;
}

long opersyshw__get_last_timestamp()
{
    long result = 0;
    D("OPERSYS HW - get_last_timestamp called");
    ioctl(fd, CMD_IS_GET_LAST_TS, &result);

    D("OPERSYS HW - get_last_timestamp result = %ld", result);
    return result;
}

int opersyshw__get_read_stat()
{
    int result = 0;
    D("OPERSYS HW - get_read_stat called");
    ioctl(fd, CMD_IS_GET_READ_STAT, &result);

    D("OPERSYS HW - get_read_stat result = %d", result);
    return result;
}

int opersyshw__get_write_stat()
{
    int result = 0;
    D("OPERSYS HW - get_write_stat called");
    ioctl(fd, CMD_IS_GET_WRITE_STAT, &result);

    D("OPERSYS HW - get_write_stat result = %d", result);
    return result;
}

void opersyshw__set_buffer_to_char(char ch)
{
    D("OPERSYS HW - set_buffer_to_char called with ch = %c", ch);
    ioctl(fd, CMD_IS_SET_BUFFER_TO_CHAR, &ch);
}

static int open_opersyshw(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    opersyshw_device_t *dev = malloc(sizeof(opersyshw_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->read = opersyshw__read;
    dev->write = opersyshw__write;
    dev->test = opersyshw__test;
    dev->zeroout = opersyshw__zeroout;
    dev->istherenewdata = opersyshw__isthrenewdata;
    dev->get_last_timestamp = opersyshw__get_last_timestamp;
    dev->get_read_stat = opersyshw__get_read_stat;
    dev->get_write_stat = opersyshw__get_write_stat;
    dev->set_buffer_to_char = opersyshw__set_buffer_to_char;

    *device = (struct hw_device_t*) dev;

    fd = open("/dev/circchar", O_RDWR);
    if (fd < 0) {
        D("Failed to open /dev/circchar");
        D("OPERSYS HW initialization failed");
        return -1;        
    }

    D("OPERSYS HW has been initialized");

    return 0;
}

static struct hw_module_methods_t opersyshw_module_methods = {
    .open = open_opersyshw
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = OPERSYSHW_HARDWARE_MODULE_ID,
    .name = "Opersys HW Module",
    .author = "Opersys inc.",
    .methods = &opersyshw_module_methods,
};