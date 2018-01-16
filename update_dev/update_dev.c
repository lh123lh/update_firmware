#include "update_dev.h"

int unpack_config_file(const char *cfg_name)
{
    char cmd[128];

    sprintf(cmd, "dd if=test.bin of=%s bs=1k skip=0 count=%d", cfg_name, JSON_CFG_SIZE);

    system(cmd);

    return 0;
}

int unpack_img(const char *name, unsigned int len, unsigned int skip_len)
{
    char cmd[128];

    sprintf(cmd, "dd if=test.bin of=%s_u bs=1k skip=%d count=%d", name, skip_len+JSON_CFG_SIZE, len);

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

int update_dev(const char *name, const int mtdnum)
{
    char cmd[128];

    sprintf(cmd, "nandflash eraseall /dev/mtd%d", mtdnum);

    printf("[system] %s\n", cmd);

    system(cmd);

    sprintf(cmd, "nandwrite /dev/mtd%d -p %s", mtdnum, name);

    printf("[system] %s\n", cmd);

    system(cmd);

    return 0;
}

int choose_to_update(const img_info *info)
{
    unsigned int mtdnum = 0;
    char *name = NULL;

    if(info->kernel_info->len != 0)
    {
        mtdnum = info->kernel_info->mtdnum;
        name = info->kernel_info->name;

        update_dev(name, mtdnum);
    }

    if(info->dts_info->len != 0)
    {
        mtdnum = info->dts_info->mtdnum;
        name = info->dts_info->name;

        update_dev(name, mtdnum);
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ch = 0;
    int ret = 0;
    char *file_name = NULL;

    img_info info;
    memset(&info, 0, sizeof(info));

    if(argc == 1)
    {
        printf("usage : %s -h\n", argv[0]);

        exit(0);
    }

    while((ch = getopt(argc, argv, "f:h")) != -1)
    {
        switch(ch)
        {
            case 'f':
                file_name = optarg;
                break;
            case 'h':
                printf("usage : %s -f <json file>\n", argv[0]);
                exit(0);
                break;
        }
    }

    unpack_config_file(file_name);

    ret = parse_info(file_name, &info);

    if(ret == -1)
    {
        perror("parse_info\n");

        return -1;
    }

    return 0;
}
