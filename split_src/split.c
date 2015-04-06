#include "prep.h"

void itoa( int i,char* string) 
{
    int power, j;
    j=i; 

    memset(string, 0, 16);

    for (power=1;j>=10;j/=10) 
    {
        power*=10; 
    }

    for (;power>0;power/=10)
    {
        *string++='0'+i/power; 
        i%=power; 
    }
    *string='\0';
} 


int main(int argc, char* argv[])
{
    char newFileName[DIRNAME_LEN], string[16];
    struct FILE_INFO fileinfo;
    int suffix = 0, rds_num = 0, tmp;
    struct RECORD_S memblock[4000];
    FILE *fp;

    /*parse argument*/

    if (argc != 2)
    {
        return 1;
    }

    readFileInfo(argv[1], &fileinfo);
   
    while (rds_num < fileinfo.rds_num)
    { 
        memset(newFileName, 0, DIRNAME_LEN);
        strcpy(newFileName, argv[1]);
        itoa(suffix, string);
        strcat(newFileName, string);
        suffix++;
        fp = fopen(newFileName, "w+"); 
        if (fp == NULL)
        {
            printf("ERR: cannot create file [%s]\n", newFileName);
        }

        if (rds_num + 4000 < fileinfo.rds_num)
        {
            tmp  = 4000;
            fread(&memblock, sizeof(struct RECORD_S), tmp, fileinfo.fp);
            fwrite(&memblock, sizeof(struct RECORD_S), tmp, fp);
            fwrite(&tmp, sizeof(int), 1, fp);
            rds_num += tmp;
        } 
        else
        {
            tmp = fileinfo.rds_num - rds_num;
            fread(&memblock, sizeof(struct RECORD_S), tmp, fileinfo.fp);
            fwrite(&memblock, sizeof(struct RECORD_S), tmp, fp);
            rds_num = fileinfo.rds_num;
        }
        fclose(fp);
    }    

    fclose(fileinfo.fp);

    //printMathRds(argv[1]);
    printf("rds_num=[%d]\n", fileinfo.rds_num);

    return 0;

}

