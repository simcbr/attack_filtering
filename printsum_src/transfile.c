#include "prep.h"

/*transfer original file to a binary file to fasten the seek speed*/

void transferOptFile(char *fileName)
{
    FILE *fp, *newfp;
    char line[MAX_LINE_LEN], *token, newFileName[DIRNAME_LEN];
    struct RECORD rds;
    float *flp;
    int rds_num = 0, fields_num = 0;
    const char delimiters[] = " ,";

    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("ERR: invalid filename [%s]\n", fileName);
        return;
    }

    strcpy(newFileName, fileName);
    strcat(newFileName, BIN_SUFFIX);
    newfp = fopen(newFileName, "w+");
    if (newfp == NULL)
    {
        printf("ERR: can not create bin file [%s]\n", newFileName);
        fclose(fp);
        return;
    }

    /*read a line from the original file*/
    while (fgets(line, MAX_LINE_LEN, fp) != NULL)
    {
        /*operate line*/
        //printf("%s\n", line);
        rds_num++;

        /*read attributes to rds*/
        flp = (float *)&rds; 
        token = strtok(line, delimiters);
        for (fields_num = 0; fields_num < FIELDS; fields_num++)
        {
            if (sscanf(token, "%f", &flp[fields_num]) == EOF)
            {
                printf("ERR: end of line\n");
                fclose(fp);
                fclose(newfp);
                return;
            }
            token = strtok(NULL, delimiters);
        }
        
        /*write this struct to the newfile*/
        fwrite(&rds, sizeof(struct RECORD), 1, newfp);

        memset(line, 0, MAX_LINE_LEN);
    }

    /*write the number of records at the end of the file*/
    fwrite(&rds_num, sizeof(int), 1, newfp);

    fclose(fp);
    fclose(newfp);
}


void printRecord(char *fileName, int offset)
{
    FILE *fp;
    struct RECORD rds;
    float *flp;
    int fields_num;

    if (!strstr(fileName, "_bin"))
    {
        printf("ERR: invalid filename [%s]\n", fileName);
        return;
    }

    /*open file*/
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("ERR: can not open file [%s]\n", fileName);
        return;
    }

    fseek(fp, offset*sizeof(struct RECORD), SEEK_SET);

    fread(&rds, sizeof(struct RECORD), 1, fp);
    
    fclose(fp);

    flp = (float *)&rds;
    for (fields_num = 0; fields_num < FIELDS; fields_num++)
    {
        printf("%f ", flp[fields_num]);
    }
}


void printRecordNum(char *fileName)
{
    FILE *fp;
    int rds_num = -1;

    /*open file*/
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("ERR: can not open file [%s]\n", fileName);
        return;
    }

    fseek(fp, -sizeof(int), SEEK_END);
    fread(&rds_num, sizeof(int), 1, fp);

    fclose(fp);
  
    printf("rds_num=[%d]\n", rds_num); 
}


void readFileInfo(char *fileName, struct FILE_INFO *fileInfo)
{
    /*open file*/
    fileInfo->fp = fopen(fileName, "r");
    if (fileInfo->fp == NULL)
    {
        printf("ERR: can not open file [%s]\n", fileName);
        return;
    }

    fseek(fileInfo->fp, -sizeof(int), SEEK_END);
    fread(&fileInfo->rds_num, sizeof(int), 1, fileInfo->fp);

    rewind(fileInfo->fp);
}


