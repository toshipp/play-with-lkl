#include <lkl.h>
#include <lkl_host.h>

int main(int n, char** argv) {
    lkl_start_kernel(&lkl_host_ops, "mem=10M");

    int ret = lkl_sys_vfork();
    if(ret < 0) {
        lkl_printf("vfork error: %s\n", ret);
        return 1;
    }

    lkl_sys_halt();

    return 0;
}
