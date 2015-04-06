#include "prep.h"

/*In this function, we will look through all transferred binary files,
and generate two files. One file is for traning, another file is for testing.
First, all files' record number will be read out, and a total records number
is calculated. The random algorithm is based on rand%total_records_num, then
copy this record to the new file. For example:

file_1: rds_num_1
file_2: rds_num_2
file_3: rds_num_3 

select a random record from these 3 files.

total_num = rds_num_1 + rds_num_2 + rds_num_3;

index = rand()%total_num
 
if this index is in the range of file_2 [rds_num_1 ~ rds_num_1+rds_num_2]
then select record (index-rds_num_1) from file_2.

*/

void prepDir(char *dirName)
{
    DIR *dp;
    struct dirent *ep;
    char tmpdirname[DIRNAME_LEN], tmpstr[DIRNAME_LEN];
    struct FILE_INFO files[MAX_FILES_PER_DIR];
    int files_num = 0, total_rds_num = 0, training_rds_num = 0;
    int stb_ind, rds_ind_in_file, i, j, p;
    char *stb = NULL;
    FILE *training_fp, *testing_fp;
    struct RECORD rds;
    int normal_ind = 0, normal_t_rds = 0, normal_e_rds;

    dp = opendir (dirName);
    if (dp == NULL)
    {
        printf("ERR: Couldn't open the directory\n");
        return;
    }
    else
    {
        memset(tmpdirname, 0, DIRNAME_LEN);
        strcpy(tmpdirname, dirName);
        tmpdirname[strlen(dirName)] = '/';

        while (ep = readdir(dp))
        {
             printf("%s \n", ep->d_name);
             if (strstr(ep->d_name, "Optimized"))
             {
                 memset(tmpstr, 0, DIRNAME_LEN);
                 strcpy(tmpstr, tmpdirname);
                 strcat(tmpstr, ep->d_name);

                 /*read this file's info*/
                 readFileInfo(tmpstr, &files[files_num]);

                 if (strstr(tmpstr, "Normal"))
                 {
                     normal_ind = files_num;
                 }
                 files[files_num].start_ind = total_rds_num;
                 total_rds_num += files[files_num].rds_num;

                 files_num++;
             }
        }

        closedir (dp);
    }
        

    /*Now, we allocate a memory to construct a one dimensional array 
      for fast file searching and prevent have same pseudo-rand number.*/
    stb = (char *)malloc(total_rds_num);
    p = 0;
    memset(stb, -1, total_rds_num);
    for (i=0; i<files_num; i++)
    {
        for(j=0; j<files[i].rds_num; j++)
        {
            stb[p++] = i;
        }
    }

    /*genearte random files*/
    training_fp = fopen(TRAINING_FILENAME, "w+");
    if (training_fp == NULL)
    {
        printf("ERR: cannot create file [%s]\n", TRAINING_FILENAME);
        return;
    }

    testing_fp = fopen(TESTING_FILENAME, "w+");
    if (testing_fp == NULL)
    {
        printf("ERR: cannot create file [%s]\n", TESTING_FILENAME);
        return;
    }

    training_rds_num = total_rds_num * 2 / 3;
    for (i=0; i<total_rds_num; i++)
    {
        stb_ind = rand()%total_rds_num;
        if (stb[stb_ind] == -1)
        {
            /*looks for next nearest entry*/
            j = 0;
            while (j<total_rds_num)
            {
                j++;
                if (stb[(stb_ind+j)%total_rds_num] != -1)
                {
                    stb_ind = (stb_ind+j)%total_rds_num;
                    break;
                }
            }
        }
        /*now the entry pointed by stb_ind is not selected before*/        
        /*go to file files[stb[index]]*/
        rds_ind_in_file = stb_ind - files[stb[stb_ind]].start_ind;

        /*pick this record and write iit to new file*/
        fseek(files[stb[stb_ind]].fp, 
              rds_ind_in_file*sizeof(struct RECORD), SEEK_SET);

        fread(&rds, sizeof(struct RECORD), 1, files[stb[stb_ind]].fp); 
        if (i<training_rds_num)
        {
            fwrite(&rds, sizeof(struct RECORD), 1, training_fp);
 
            if (stb[stb_ind] == normal_ind)
            {
                normal_t_rds++;
            }
        }
        else
        {
            fwrite(&rds, sizeof(struct RECORD), 1, testing_fp);

            if (stb[stb_ind] == normal_ind)
            {
                normal_e_rds++;
            }
        }
        stb[stb_ind] = -1;
    }



    /*close all files*/
    for (i=0; i<files_num; i++)
    {
        fclose(files[i].fp);
    }

    printf("training_rds = [%d] testing_rds = [%d]\n", normal_t_rds, normal_e_rds);
}
