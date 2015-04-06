#include "prep.h"



void memsetFlt(float *ptr, float value, int size)
{
    int i;

    for (i=0; i<size; i++)
    {
        *ptr++ = value;
    }
}


void printMathRds(char *fileName)
{
    printf("\nMean==================\n");
    printRecord(fileName, 0);
    printf("\nMin==================\n");
    printRecord(fileName, 1);
    printf("\nMax==================\n");
    printRecord(fileName, 2);
    printf("\nOverMean==================\n");
    printRecord(fileName, 3);
    printf("\n");
    printRecordNum(fileName);
}


void printMemRds(struct RECORD_S *rds)
{
    int i;

    printf("\n");
    for (i=0; i<FIELDS; i++)
    {
        printf("%f ", rds->elem[i]);
    }
    printf("\n");
}



/*Calculate the mean, min max value and count 
overmean number from attack records. We only 
check the attack file with more than 20 records.*/
void recordsDir(char *dirName)
{
    DIR *dp;
    struct dirent *ep;
    char tmpdirname[DIRNAME_LEN], tmpstr[DIRNAME_LEN];
    struct FILE_INFO fileinfo;
    struct RECORD_S min_rds, max_rds, mean_rds, overmean, temp;
    int i,j;
    FILE *sum_fp;

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

        //printMemRds(&overmean);
        //printMemRds(&mean_rds);
        //printMemRds(&min_rds);
        //printMemRds(&max_rds);

        while (ep = readdir(dp))
        {
             printf("%s \n", ep->d_name);
             if (strstr(ep->d_name, "Optimized"))
             {
                 memset(tmpstr, 0, DIRNAME_LEN);
                 strcpy(tmpstr, tmpdirname);
                 strcat(tmpstr, ep->d_name);
                
                 /*read this file's info*/
                 readFileInfo(tmpstr, &fileinfo);

                 memset(&overmean, 0, sizeof(struct RECORD_S));
                 memset(&mean_rds, 0, sizeof(struct RECORD_S));
                 memsetFlt((float *)&min_rds, 2, FIELDS);
                 memsetFlt((float *)&max_rds, -1, FIELDS);

                 /*iterate file to get min, max and mean*/
                 for (i=0; i<fileinfo.rds_num; i++)
                 {
                     fread(&temp, sizeof(struct RECORD_S), 1, fileinfo.fp); 
                     for (j=0; j<FIELDS; j++)
                     {
                         if (temp.elem[j] < min_rds.elem[j])
                         {
                             min_rds.elem[j] = temp.elem[j];
                         }
                        
                         if (temp.elem[j] > max_rds.elem[j])
                         {
                              max_rds.elem[j] = temp.elem[j];
                         }
                     
                         mean_rds.elem[j] += temp.elem[j];
                     }
                 }

                 for (i=0; i<FIELDS; i++)
                 {
                     mean_rds.elem[i] = mean_rds.elem[i]/fileinfo.rds_num;
                 } 


                 /*get overmean*/
                 rewind(fileinfo.fp);
                 for (i=0; i<fileinfo.rds_num; i++)
                 {
                     fread(&temp, sizeof(struct RECORD_S), 1, fileinfo.fp);
                     for (j=0; j<FIELDS; j++)
                     {
                         if (temp.elem[j] > mean_rds.elem[j])
                         {
                             overmean.elem[j]++;
                         }
                     }
                 }

                 /*write min, max, mean and overmean*/
                 strcat(tmpstr, SUMMARY_SUFFIX);
                 sum_fp = fopen(tmpstr, "w+");
                 if (sum_fp == NULL)
                 {
                     printf("ERR: cannot create file [%s]\n", tmpstr);
                     return;
                 }             
                 fwrite(&mean_rds, sizeof(struct RECORD_S), 1, sum_fp);
                 fwrite(&min_rds, sizeof(struct RECORD_S), 1, sum_fp);
                 fwrite(&max_rds, sizeof(struct RECORD_S), 1, sum_fp);
                 fwrite(&overmean, sizeof(struct RECORD_S), 1, sum_fp);
                 fwrite(&fileinfo.rds_num, sizeof(int), 1, sum_fp);
             }
        }

        closedir (dp);
    }
}


/*this function check whether the record is in specified file*/
int recordExist(struct RECORD_S *rds, struct FILE_INFO *fileinfo)
{
    int i, j, match;
    struct RECORD_S temp;

    rewind(fileinfo->fp);
    for (i=0; i<fileinfo->rds_num; i++)
    {
        fread(&temp, sizeof(struct RECORD_S), 1, fileinfo->fp);
        match = 1;
        for (j=0; j<FIELDS && match; j++)
        {
            if (rds->elem[j] != temp.elem[j])
            {
                match = 0;
            }
        }
        if (match)
        {
            return 1;
        }
    }

    return 0;
    
}



/*this function tries to apply a rule to filter records in specified file. The rule is a "record pair" which defines the min and max value of attributes. If entry is within this rnage, we copy this entry to the matching file.*/
void applyRulesToFile(struct FILE_INFO *fileinfo, struct RULE *ruleset)
{
    struct RECORD_S temp;
    int i,j,k, match;


    for (j=0; j<RULES_PER_SET; j++)
    {
        ruleset[j].match_num = 0;
    }

    for (i=0; i<fileinfo->rds_num; i++)
    {
        fread(&temp, sizeof(struct RECORD_S), 1, fileinfo->fp);
        for (j=0; j<RULES_PER_SET; j++)
        {
            match = 1;
            for(k=0; k<FIELDS && match; k++)
            {
                if ( (temp.elem[k] < ruleset[j].min.elem[k]) ||
                     (temp.elem[k] > ruleset[j].max.elem[k])  )
                {
                    match = 0;
                }
            }

            if (match)
            {
                fwrite(&temp, sizeof(struct RECORD_S), 1, ruleset[j].fp);
                ruleset[j].match_num++;
            }
        }
    } 
   
    for (j=0; j<RULES_PER_SET; j++)
    { 
        fwrite(&ruleset[j].match_num, sizeof(int), 1,ruleset[j].fp);
        fclose(ruleset[j].fp);
    }
}



/*this function loads the information from summary file and conclude it to a rule which stores in the min_rds and max_rds*/
void loadRuleFromFile(struct RECORD_S *min_rds, struct RECORD_S *max_rds,
         char *fileName)
{

    struct FILE_INFO fileinfo;
    struct RECORD_S mean, overmean;
    int i;

    readFileInfo(fileName, &fileinfo);
    fread(&mean, sizeof(struct RECORD_S), 1, fileinfo.fp);    
    fread(min_rds, sizeof(struct RECORD_S), 1, fileinfo.fp);    
    fread(max_rds, sizeof(struct RECORD_S), 1, fileinfo.fp);    
    fread(&overmean, sizeof(struct RECORD_S), 1, fileinfo.fp);

    for (i=0; i<FIELDS; i++)
    {
        if (overmean.elem[i] < fileinfo.rds_num*THRESHOLD)
        {
            min_rds->elem[i] = 0;
            max_rds->elem[i] = 1;
        }
        else
        {
            min_rds->elem[i] = mean.elem[i]*MINSHIFT;
        }
    }

    fclose(fileinfo.fp);
}




/*this function pick every rule from specified dir, and apply this rule to a target file.*/
void applyRulesToDir(char *dirName, char *targetFileName)
{
    DIR *dp;
    struct dirent *ep;
    char tmpdirname[DIRNAME_LEN], tmpstr[DIRNAME_LEN];
    struct FILE_INFO fileinfo;
    FILE *match_fp;
    struct RULE ruleset[RULES_PER_SET];
    int rules_num;

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
                 if (rules_num < RULES_PER_SET)
                 {
                     loadRuleFromFile(&ruleset[rules_num].min, &ruleset[rules_num].max,
                                      tmpstr);
                 } 

                 strcat(tmpstr, MATCH_SUFFIX);
                 ruleset[rules_num].fp = fopen(tmpstr, "w+");
                 if (ruleset[rules_num].fp == NULL)
                 {
                     printf("ERR: cannot create file [%s]\n", tmpstr);
                     return;
                 }
                 rules_num++;
             }
        }

        closedir (dp);

        readFileInfo(targetFileName, &fileinfo);
        applyRulesToFile(&fileinfo, ruleset);

        fclose(fileinfo.fp);
    }    
}
