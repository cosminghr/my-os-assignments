#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 1024

typedef struct
{
    char sect_name[18];
    int sect_type;
    int sect_offset;
    int sect_size;
} section_header;

void listIterative(const char *path, char *is_in_path, int control, int has_perm)
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
                if (is_in_path != NULL && strncmp(entry->d_name, is_in_path, strlen(is_in_path)) == 0 && control == 1 && has_perm == 0) // verificam daca substring-ul se afla in in numele fisierului la inceput si daca executam cautarea pentru name_starts_with
                {
                    printf("%s\n", fullPath);
                }
                else if (has_perm == 1) // verificam daca parametrul has_perm este 1 pentru a face executia
                {
                    if (((S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode)) && ((statbuf.st_mode & S_IXUSR) == S_IXUSR))) // verificam daca avem director sau regular file si aplicam o operatie pe biti pentru a afla daca are acces de tip write
                    {

                        printf("%s\n", fullPath);
                    }
                }
                else if (control == 0 && has_perm == 0) // executia normala a listIterative fara a face name_starts_with si has_perm_execute
                {
                    printf("%s\n", fullPath);
                }
            }
        }
    }
    // free(fullPath);
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
                if (S_ISDIR(statbuf.st_mode)) // verificam daca este director
                {
                    listRec(fullPath); // se face parcurgerea recursiva
                }
            }
        }
    }
    // free(fullPath);
    closedir(dir);
}

void parseSFformat(const char *path)
{
    int fd = 0;
    int file_size = 0;
    char magic[5] = "";
    int header_size = 0;
    int no_of_sect = 0;
    int version = 0;

    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open file");
        return;
    }

    file_size = lseek(fd, 0, SEEK_END);
    // printf("%d\n", file_size);
    lseek(fd, -4, SEEK_CUR);
    read(fd, &magic, 4);
    magic[4] = '\0';
    // printf("%s\n", magic);

    lseek(fd, -6, SEEK_END);
    read(fd, &header_size, 2);
    // printf("%d\n", header_size);

    lseek(fd, file_size - header_size, SEEK_SET);
    read(fd, &version, 4);
    // printf("%d\n", version);

    read(fd, &no_of_sect, 1);
    // printf("%d\n", no_of_sect);

    section_header *headere = (section_header *)malloc(no_of_sect * sizeof(section_header));
    if (!headere)
    {
        printf("Failed to allocate memory");
        free(headere);
        close(fd);
        return;
    }

    int verifica = 0;
    // printf("%s\n",magic);
    if (magic[0] != 'a' && magic[1] != 'x' && magic[2] != 't' && magic[3] != 'n')
    {
        printf("ERROR\nwrong magic\n");
        verifica = 1;
        free(headere);
        close(fd);
        return;
    }
    else if (version < 82 || version > 142)
    {
        printf("ERROR\nwrong version");
        verifica = 1;
        free(headere);
        close(fd);
        return;
    }
    else if (no_of_sect < 3 || no_of_sect > 17)
    {
        printf("ERROR\nwrong sect_nr");
        verifica = 1;
        free(headere);
        close(fd);
        return;
    }
    else
    {
        for (int i = 0; i < no_of_sect; i++)
        {
            read(fd, &headere[i].sect_name, 18);
            read(fd, &headere[i].sect_type, 2);
            read(fd, &headere[i].sect_offset, 4);
            read(fd, &headere[i].sect_size, 4);
            if (headere[i].sect_type != 55 && headere[i].sect_type != 58)
            {
                printf("ERROR\nwrong sect_types");
                verifica = 1;
                break;
            }
        }
        if (verifica == 0)
        {
            printf("SUCCESS\n");
            printf("version=%d\n", version);
            printf("nr_sections=%d\n", no_of_sect);
            for (int j = 0; j < no_of_sect; j++)
            {
                printf("section%d: ", j + 1);
                printf("%s ", headere[j].sect_name);
                printf("%d ", headere[j].sect_type);
                printf("%d \n", headere[j].sect_size);
            }
        }
    }

    free(headere);

    close(fd);
}

int main(int argc, char **argv)
{

    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("24435\n");
        }

        // pentru parcurgerea recursiva
        if (strcmp(argv[1], "list") == 0)
        {
            if (strcmp(argv[2], "recursive") == 0)
            {
                if (strncmp(argv[3], "path=", 5) == 0) // verificam daca argumentul al treilea coincide cu comanda introdusa in terminal(adica aparita substring-ului path= )
                {
                    char *sirPath = malloc((strlen(argv[3])) * sizeof(char));
                    int countj = 5;
                    printf("SUCCESS\n");
                    for (int i = 0; i < strlen(argv[3]); i++)
                    {
                        sirPath[i] = argv[3][countj++];
                        // printf("%s", sirPath);
                    }
                    listRec(sirPath); // parcurgem recursiv directorul
                    free(sirPath);
                }
            }

            // pentru name_start_with
            if (strncmp(argv[2], "path=", 5) == 0)
            {
                char *sirPath = malloc((strlen(argv[2])) * sizeof(char));
                int countj = 5;
                printf("SUCCESS\n");
                for (int i = 0; i < strlen(argv[2]); i++)
                {
                    sirPath[i] = argv[2][countj++];
                }
                listIterative(sirPath, NULL, 0, 0);
                free(sirPath);
            }
            if (strncmp(argv[2], "name_starts_with=", 17) == 0) // verificam daca argumentul al doilea coincide cu comanda introdusa in terminal( adica aparitia substring-ului name_start_with= )
            {
                char *is_in_path1 = malloc((strlen(argv[2])) * sizeof(char)); // alocam memorie pentru substring
                int countString = 17;
                for (int i = 0; i < strlen(argv[2]); i++)
                {
                    is_in_path1[i] = argv[2][countString++]; // adaugam in substring ce se afla dupa egal, adica literele ce trebuie cautate in numele fisierului
                }
                if (strncmp(argv[3], "path=", 5) == 0) // verificam daca al treilea argument coincide cu path=
                {
                    char *sirPath = malloc((strlen(argv[3])) * sizeof(char)); // alocam memorie pentru path
                    int countj = 5;
                    printf("SUCCESS\n");
                    for (int i = 0; i < strlen(argv[3]); i++)
                    {
                        sirPath[i] = argv[3][countj++]; // scriem numele fisierului
                    }
                    listIterative(sirPath, is_in_path1, 1, 0); // apelam parcurgerea iterativa pentru a se parcurge directorul primit ca aprametru cu parametrii is_in_path1 -> substring -ul cautat si 1 fiind contorul care indica folosirea name_start_with
                    free(sirPath);                             // eliberam memoria alocata
                }
                free(is_in_path1); // eliberam memoria alocata
            }

            // pentru has_perm_execute
            if (strcmp(argv[2], "has_perm_execute") == 0)
            {
                if (strncmp(argv[3], "path=", 5) == 0)
                {
                    char *sirPath = malloc((strlen(argv[3])) * sizeof(char));
                    int countj = 5;
                    printf("SUCCESS\n");
                    for (int i = 0; i < strlen(argv[3]); i++)
                    {
                        sirPath[i] = argv[3][countj++];
                    }
                    listIterative(sirPath, NULL, 0, 1); // setam parametruh has_perm_execute pe 1 pentru a face cautarea de proprietari cu permisiuni de executie
                    free(sirPath);
                }
            }
        }

        if (strcmp(argv[1], "parse") == 0)
        {
            if (strncmp(argv[2], "path=", 5) == 0)
            {
                char *sirPath1 = malloc(strlen(argv[2]) * sizeof(char));
                int countj = 5;
                for (int i = 0; i < strlen(argv[2]); i++)
                {
                    sirPath1[i] = argv[2][countj++];
                }
                parseSFformat(sirPath1);
                free(sirPath1);
            }
        }
    }

    return 0;
}