#include "packimg.h"

int pack_img(const char *name, const unsigned int seek_len, const unsigned int len)
{
    char cmd[128];

    sprintf(cmd, "dd if=%s of=test.bin bs=1k seek=%d count=%d", name, seek_len+JSON_CFG_SIZE, len);

    printf("[system]:%s\n", cmd);

    system(cmd);

    return 0;
}

int pack_config_file(const char *cfg_name)
{
    char cmd[128];

    sprintf(cmd, "dd if=%s of=test.bin bs=1k seek=0 count=%d", cfg_name, JSON_CFG_SIZE);

    system(cmd);

    return 0;
}

int calc_seek_len(const char *name, const img_info *info)
{
    unsigned int len = 0;
    unsigned int seek_len = 0;

    // printf("name:%s, len:%d\n", info->kernel_info->name, info->kernel_info->len);

    if(strcmp(name, "uImage") == 0)
    {
        len = info->kernel_info->len;

        seek_len = 0;
    }
    else if(strcmp(name, "devicetree.dtb") == 0)
    {
        len = info->dts_info->len;

        seek_len = info->kernel_info->len;
    }

    // printf("len:%d, seek_len:%d\n", len, seek_len);

    pack_img(name, seek_len, len);

    return 0;
}

int choose_to_pack(img_info *info)
{
    // parse_info("./info.json", &info);

    if(info->kernel_info->len != 0)
    {
        calc_seek_len("uImage", info);
    }

    // parse_info("./info.json", &info);

    if(info->dts_info->len != 0)
    {
        calc_seek_len("devicetree.dtb", info);
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

    pack_config_file(file_name);

    ret = parse_info(file_name, &info);

    if(ret == -1)
    {
        perror("parse_info\n");

        return -1;
    }

    return 0;
}