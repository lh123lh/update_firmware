#include "update_dev.h"

int unpack_img(const char *name, unsigned int len, unsigned int skip_len)
{
    char cmd[128];

    sprintf(cmd, "dd if=test.bin of=%s_u bs=1k skip=%d count=%d", name, skip_len, len);

    system(cmd);

    printf("[system]:%s\n", cmd);

    return 0;
}

int calc_skip_len(const char *name, const img_info *info)
{
    unsigned int len = 0;
    unsigned int skip_len = 0;

    if(strcmp(name, "uImage") == 0)
    {
        len = info->kernel_info->len;

        skip_len = 0;
    }
    else if(strcmp(name, "devicetree.dtb") == 0)
    {
        len = info->dts_info->len;

        skip_len = info->kernel_info->len;
    }

    // printf("len:%d, seek_len:%d\n", len, skip_len);

    unpack_img(name, len, skip_len);

    return 0;
}

int choose_to_unpack(const img_info *info)
{
    if(info->kernel_info->len != 0)
    {
        calc_skip_len("uImage", info);
    }

    if(info->dts_info->len != 0)
    {
        calc_skip_len("devicetree.dtb", info);
    }

    return 0;
}