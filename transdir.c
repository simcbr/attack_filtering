#include "prep.h"


void transferOptDir(char *dirName)
{
    DIR *dp;
    struct dirent *ep;
    char tmpdirname[DIRNAME_LEN], tmpstr[DIRNAME_LEN];
     
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
                 transferOptFile(tmpstr);
             }
        }

        closedir (dp);
    }
}

