#include "prep.h"

int main(int argc, char* argv[])
{
    char newFileName[DIRNAME_LEN];

    /*parse argument*/

/*
    if (argc != 2)
    {
        return 1;
    }
    printf("filename=[%s]\n", argv[1]);
    printRecord(argv[1], 1);

    transferOptFile(argv[1]);
    printf("transfer complete\n");

    strcpy(newFileName, argv[1]);
    strcat(newFileName, "_bin");
    printRecord(newFileName, 71);
    printRecordNum(newFileName);
*/

    if (argc != 3)
    {
        return 1;
    }
    printf("dirname=[%s]\n", argv[1]);

    //transferOptDir(argv[1]);
    //prepDir(argv[1]);
    //recordsDir(argv[1]);
    applyRulesToDir(argv[1], argv[2]);

    return 0;

}

