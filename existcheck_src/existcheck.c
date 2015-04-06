#include "prep.h"

int main(int argc, char* argv[])
{
    struct FILE_INFO match, orig; 
    struct RECORD_S temp;
    int matched = 0, i;
    time_t t;

    /*parse argument*/
    if (argc != 3)
    {
        return 1;
    }
    t = time(NULL);

    printf("matchfile=[%s]\n", argv[1]);
    printf("originalfile=[%s]\n", argv[2]);

    memset(&match, 0, sizeof(struct FILE_INFO));
    readFileInfo(argv[1], &match);
    memset(&orig, 0, sizeof(struct FILE_INFO));
    readFileInfo(argv[2], &orig);
    
    memset(&temp, 0, sizeof(struct RECORD_S));
    /*iterate original file to see how many of records are 
    same in the original file*/
    for (i = 0; i < match.rds_num; i++)
    {
        fread(&temp, sizeof(struct RECORD_S), 1, match.fp);
        if (recordExist(&temp, &orig))
        {
            printf(".");
            matched++;
        }
        else
        {
            printf("X");
        }
    }

    t = time(NULL) - t;
    printf("\n There are totally [%d] of [%d] records accord to original file.\n",
          matched, match.rds_num);
    printf("time consuming: %d seconds", t);

    return 0;

}

