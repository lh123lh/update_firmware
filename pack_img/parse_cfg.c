#include "parse_cfg.h"
#include "cJSON.h"

int cjson_parse_SubNode(cJSON *item, _info *info)
{
    cJSON *object, *SubItem;
    int i = 0;
    int size = cJSON_GetArraySize(item);

    for(i=0; i<size; i++)
    {
        object = cJSON_GetArrayItem(item, i);
        if(object == NULL)
        {
            perror("parse SubNode\n");

            return -1;
        }
        else
        {
            SubItem = cJSON_GetObjectItem(object, "offset");

            if(SubItem != NULL)
            {
                // printf("type:%d, key:%s, value:%d\n", SubItem->type, SubItem->string, SubItem->valueint);

                info->offset = SubItem->valueint;
            }

            SubItem = cJSON_GetObjectItem(object, "len");

            if(SubItem != NULL)
            {
                info->len = SubItem->valueint;
            }

            SubItem = cJSON_GetObjectItem(object, "mtdofs");

            if(SubItem != NULL)
            {
                info->mtdofs = SubItem->valueint;
            }

            SubItem = cJSON_GetObjectItem(object, "mtdnum");

            if(SubItem != NULL)
            {
                info->mtdnum = SubItem->valueint;
            }

            SubItem = cJSON_GetObjectItem(object, "name");

            if(SubItem != NULL)
            {
                // printf("type:%d, key:%s, value:%s\n", SubItem->type, SubItem->string, SubItem->valuestring);

                memcpy(info->name, SubItem->valuestring, strlen(SubItem->valuestring));
            }
        }
    }

    return 0;
}

int cjson_to_struct_array(const char *json_string, img_info *info)
{
    int ret = 0;

    _info kernel;
    memset(&kernel, 0, sizeof(kernel));

    _info dts;
    memset(&dts, 0, sizeof(dts));

    cJSON *item, *root, *arrayItem, *object;

    root = cJSON_Parse(json_string);
    if(!root)
    {
        printf("get root error\n");

        return -1;
    }

    arrayItem = cJSON_GetObjectItem(root, "info");
    if(arrayItem != NULL)
    {
        int i = 0;
        int size = cJSON_GetArraySize(arrayItem);

        for(i=0; i<size; i++)
        {
            object = cJSON_GetArrayItem(arrayItem, i);
            if(object == NULL)
            {
                perror("cJSON_GetArrayItem");

                cJSON_Delete(root);

                return -1;
            }
            else
            {
                item = cJSON_GetObjectItem(object, "kernel");
                if(item != NULL)
                {
                    ret = cjson_parse_SubNode(item, &kernel);
                    if(ret == -1)
                    {
                        perror("cjson_parse_SubNode\n");

                        cJSON_Delete(root);

                        return -1;
                    }
                    else
                    {
                        info->kernel_info = &kernel;

                        // printf("info->kernel_info->len : %d\n", info->kernel_info->len);
                    }
                }

                item = cJSON_GetObjectItem(object, "device_tree");
                if(item != NULL)
                {
                    ret = cjson_parse_SubNode(item, &dts);
                    if(ret == -1)
                    {
                        perror("cjson_parse_SubNode\n");

                        cJSON_Delete(root);

                        return -1;
                    }
                    else
                    {
                        info->dts_info = &dts;
                    }
                }
            }
        }
    }

    // choose_to_pack(info); //打包img

    // choose_to_unpack(info); //解包img

    cJSON_Delete(root);

    return 0;
}

int parse_info(char *file_info, img_info *info)
{
    FILE *fp;
    int len = 0;
    int ret = 0;
    char *info_data = NULL;

    fp = fopen(file_info, "rb");
    if(fp == NULL)
    {
        printf("open info file error\n");

        fclose(fp);

        return -1;
    }

    fseek(fp, 0, SEEK_END);

    len = ftell(fp);

    // printf("len:%d\n", len);

    fseek(fp, 0, SEEK_SET);

    info_data = (char *)malloc(len+1);

    fread(info_data, 1, len+1, fp);

    // printf("sizeof info_data:%ld\n", sizeof(info_data));

    fclose(fp);

    free(info_data);

    ret = cjson_to_struct_array(info_data, info);

    return ret;
}