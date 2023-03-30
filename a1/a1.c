#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void listIterative(const char *path, char *is_in_path, int control)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;


    dir = opendir(path);
    if (dir == NULL)
    {
        perror("Could not open directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if (lstat(fullPath, &statbuf) == 0)
            {
                if (is_in_path != NULL && strncmp(entry->d_name, is_in_path, strlen(is_in_path)) == 0 && control == 1)
                {
                    printf("%s\n", fullPath);
                }
                else if (control == 0)
                {
                    printf("%s\n", fullPath);
                }
            }
        }
    }
    closedir(dir);
}

void listRec(const char *path)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL)
    {
        perror("Could not open directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if (lstat(fullPath, &statbuf) == 0)
            {
                printf("%s\n", fullPath);
                if (S_ISDIR(statbuf.st_mode))
                {
                    listRec(fullPath);
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char **argv)
{

    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("24435\n");
        }
        if (strcmp(argv[1], "list") == 0)
        {
            if (strcmp(argv[2], "recursive") == 0)
            {
                if (strncmp(argv[3], "path=", 5) == 0)
                {
                    char *sirPath = malloc((strlen(argv[3]) - 5) * sizeof(char));
                    int countj = 5;
                    printf("SUCCESS\n");
                    for (int i = 0; i < strlen(argv[3]); i++)
                    {
                        sirPath[i] = argv[3][countj++];
                        // printf("%s", sirPath);
                    }
                    listRec(sirPath);
                }
            }
            if (strncmp(argv[2], "path=", 5) == 0)
            {
                char *sirPath = malloc((strlen(argv[2]) - 5) * sizeof(char));
                int countj = 5;
                printf("SUCCESS\n");
                for (int i = 0; i < strlen(argv[2]); i++)
                {
                    sirPath[i] = argv[2][countj++];
                }
                listIterative(sirPath, NULL, 0);
            }
            if (strncmp(argv[2], "name_starts_with=", 17) == 0)
            {
                char *is_in_path1 = malloc((strlen(argv[2]) - 17) * sizeof(char));
                int countString = 17;
                for (int i = 0; i < strlen(argv[2]); i++)
                {
                    is_in_path1[i] = argv[2][countString++];
                }
                if (strncmp(argv[3], "path=", 5) == 0)
                {
                    char *sirPath = malloc((strlen(argv[3]) - 5) * sizeof(char));
                    int countj = 5;
                    printf("SUCCESS\n");
                    for (int i = 0; i < strlen(argv[3]); i++)
                    {
                        sirPath[i] = argv[3][countj++];
                    }
                    listIterative(sirPath, is_in_path1, 1);
                }
            }
        }
    }

    return 0;
}