#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <lkl.h>
#include <lkl_host.h>

static void close_fd(int fd) {
    int r = lkl_sys_close(fd);
    if(r < 0) {
        lkl_printf("lkl close error: %d\n", r);
    }
}

static int cat_file(char* path) {
    int fd = lkl_sys_open(path, LKL_O_RDONLY, 0);
    if(fd < 0) {
        lkl_printf("lkl open error: %d\n", fd);
        return fd;
    }
    int ret = 0;
    for(;;) {
        char buf[1024];
        int len = lkl_sys_read(fd, buf, 1024);
        if(len < 0) {
            lkl_printf("lkl read error: %d\n", len);
            ret = len;
            goto out;
        }
        if(len == 0) {
            break;
        }
        for(int written=0; written < len;) {
            int n = write(STDOUT_FILENO, buf+written, len-written);
            if(n < 0) {
                lkl_printf("write error: %d\n", n);
                ret = n;
                goto out;
            }
            written += n;
        }
    }
out:
    close_fd(fd);
    return ret;
}

int main(int n, char** args) {
    lkl_start_kernel(&lkl_host_ops, "mem=10M");
    int ret = lkl_sys_mkdir("/proc", 0777);
    if(ret < 0) {
        lkl_printf("lkl mkdir error: %d\n", ret);
        return 1;
    }
    ret = lkl_sys_mount("none", "/proc", "proc", 0, 0);
    if(ret < 0) {
        lkl_printf("lkl mount error: %d\n", ret);
        return 1;
    }
    char* path = "/proc";
    int fd = lkl_sys_open(path, LKL_O_RDONLY|LKL_O_DIRECTORY, 0);
    if(fd < 0) {
        lkl_printf("lkl open error: %d\n", fd);
        return 1;
    }
    for(;;) {
        char buf[1024];
        struct lkl_linux_dirent64 *d = (void*)buf;
        int n = lkl_sys_getdents64(fd, d, sizeof(buf));
        if(n < 0) {
            lkl_printf("lkl getdents error: %d\n", n);
            return 1;
        }
        if(n == 0) {
            break;
        }
        char pathbuf[1024];
        while((char*)d < buf + n) {
            snprintf(pathbuf, sizeof(pathbuf), "%s/%s", path, d->d_name);
            lkl_printf("path: %s\n", pathbuf);
            if(d->d_type == DT_REG && strcmp(d->d_name, "kmsg") != 0) {
                if(cat_file(pathbuf) < 0) {
                    return 1;
                }
            }
            d = (void*)((char*)d + d->d_reclen);
        }
    }
    close_fd(fd);
    lkl_sys_halt();
    return 0;
}
