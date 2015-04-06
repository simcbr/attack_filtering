#ifndef _PREP_H_
#define _PREP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>


#define MAX_LINE_LEN 256
#define DIRNAME_LEN  256
#define BIN_SUFFIX "_bin"
#define SUMMARY_SUFFIX "_sum"
#define MATCH_SUFFIX  "_matching"

#define MAX_FILES_PER_DIR 64
#define FIELDS 41

#define TRAINING_FILENAME "training_bin"
#define TESTING_FILENAME "testing_bin"

#define THRESHOLD 0.6
#define RULES_PER_SET 1
#define MINSHIFT 0.2

struct RECORD
{
    float elem[FIELDS];
};

struct RECORD_S
{
    float elem[FIELDS];
};

struct RULE
{
    struct RECORD_S min;
    struct RECORD_S max;
    int match_num;
    FILE *fp;
};

struct FILE_INFO
{
    int start_ind;
    int rds_num;
    FILE *fp; 
};





#endif
