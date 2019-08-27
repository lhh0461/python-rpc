#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "Util.h"

int GetPathFiles(const char *basePath, vector<string> & filelist)
{
    std::vector<std::string> result;
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
        {
            filelist.push_back(std::string(basePath)+std::string(ptr->d_name));
        }
        else if(ptr->d_type == 10)    ///link file
        {
            filelist.push_back(std::string(ptr->d_name));
        }
        else if(ptr->d_type == 4)    ///dir
        {
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            filelist.push_back(std::string(ptr->d_name));
            GetPathFiles(base, filelist);
        }
    }
    closedir(dir);
}
