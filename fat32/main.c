#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DISK_SIZE (20 * 1024 * 1024)   // 20 Mb
#define CLUSTER_SIZE 4096               
#define CLUSTER_COUNT (DISK_SIZE / CLUSTER_SIZE) 

#define FAT_FREE 0x00000000
#define FAT_EOC  0x0FFFFFFF

#define MAX_FILENAME 11    
#define MAX_CHILDREN 128   

typedef enum { FILE_TYPE, DIR_TYPE } EntryType;

typedef struct DirEntry {
    char name[MAX_FILENAME+1];
    EntryType type;
    uint32_t first_cluster;
    uint32_t size;
} DirEntry;

typedef struct Directory {
    DirEntry entries[MAX_CHILDREN];
    int count;
    uint32_t cluster; 
    struct Directory *parent;
} Directory;

uint8_t disk[DISK_SIZE];
uint32_t FAT[CLUSTER_COUNT];

Directory root;
Directory *current_dir;

#include <sys/stat.h>

char* backup_path = NULL;

void save_backup(const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        perror("Failed to save backup");
        return;
    }
    fwrite(FAT, sizeof(FAT), 1, f);
    fwrite(disk, sizeof(disk), 1, f);
    fclose(f);
}

int load_backup(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0; 

    FILE* f = fopen(path, "rb");
    if (!f) {
        perror("Failed to load backup");
        return 0;
    }
    fread(FAT, sizeof(FAT), 1, f);
    fread(disk, sizeof(disk), 1, f);
    fclose(f);

    root.count = 0;
    root.cluster = 2;
    root.parent = NULL;
    current_dir = &root;
    return 1;
}


void error(const char* msg) {
    printf("Error: %s\n", msg);
}

void format() {

    for (int i = 0; i < CLUSTER_COUNT; i++) FAT[i] = FAT_FREE;

    FAT[0] = FAT_EOC;
    FAT[1] = FAT_EOC;

    root.count = 0;
    root.cluster = 2;
    root.parent = NULL;
    FAT[2] = FAT_EOC;

    current_dir = &root;

    printf("Disk formatted.\n");
}

uint32_t alloc_cluster() {
    for (uint32_t i = 2; i < CLUSTER_COUNT; i++) {
        if (FAT[i] == FAT_FREE) {
            FAT[i] = FAT_EOC;
            return i;
        }
    }
    return 0; 
}

DirEntry* find_entry(Directory *dir, const char* name) {
    for (int i = 0; i < dir->count; i++) {
        if (strcmp(dir->entries[i].name, name) == 0)
            return &dir->entries[i];
    }
    return NULL;
}

int add_entry(Directory *dir, const char* name, EntryType type) {
    if (dir->count >= MAX_CHILDREN) {
        error("Directory full");
        return 0;
    }
    if (find_entry(dir, name) != NULL) {
        error("Entry with this name already exists");
        return 0;
    }
    DirEntry *e = &dir->entries[dir->count++];
    strncpy(e->name, name, MAX_FILENAME);
    e->name[MAX_FILENAME] = 0;
    e->type = type;
    e->first_cluster = alloc_cluster();
    if (e->first_cluster == 0) {
        error("No free cluster available");
        dir->count--;
        return 0;
    }
    e->size = 0;
    if (type == DIR_TYPE) {
        memset(disk + e->first_cluster * CLUSTER_SIZE, 0, CLUSTER_SIZE);
    }
    return 1;
}

void ls() {
    if (current_dir->count == 0) {
        printf("Directory empty.\n");
        return;
    }
    for (int i = 0; i < current_dir->count; i++) {
        DirEntry *e = &current_dir->entries[i];
        printf("%s\t%s\n", e->name, e->type == DIR_TYPE ? "<DIR>" : "<FILE>");
    }
}

int mkdir_cmd(const char* name) {
    if (strlen(name) > MAX_FILENAME) {
        error("Name too long");
        return 0;
    }
    return add_entry(current_dir, name, DIR_TYPE);
}

int touch_cmd(const char* name) {
    if (strlen(name) > MAX_FILENAME) {
        error("Name too long");
        return 0;
    }
    return add_entry(current_dir, name, FILE_TYPE);
}

int cd_cmd(const char* name) {
    if (strcmp(name, "..") == 0) {
        if (current_dir->parent != NULL)
            current_dir = current_dir->parent;
        return 1;
    }
    DirEntry *e = find_entry(current_dir, name);
    if (!e) {
        error("Directory not found");
        return 0;
    }
    if (e->type != DIR_TYPE) {
        error("Not a directory");
        return 0;
    }

    Directory *new_dir = malloc(sizeof(Directory));
    if (!new_dir) {
        error("Memory allocation failed");
        return 0;
    }
    new_dir->cluster = e->first_cluster;
    new_dir->count = 0;
    new_dir->parent = current_dir;

    current_dir = new_dir;
    printf("Changed directory to %s\n", name);
    return 1;
}

void prompt() {
    printf("fat32> ");
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <backup-file>\n", argv[0]);
        return 1;
    }
    backup_path = argv[1];

    if (!load_backup(backup_path)) {
        printf("No valid backup found, formatting...\n");
        format();
        save_backup(backup_path);
    } else {
        printf("Backup loaded from '%s'\n", backup_path);
    }

    char line[128];

    while (1) {
        prompt();
        if (!fgets(line, sizeof(line), stdin)) break;
        char *cmd = strtok(line, " \t\n");
        if (!cmd) continue;

        if (strcmp(cmd, "format") == 0) {
            format();
            save_backup(backup_path);
        } else if (strcmp(cmd, "ls") == 0) {
            ls();
        } else if (strcmp(cmd, "mkdir") == 0) {
            char *arg = strtok(NULL, " \t\n");
            if (!arg) {
                error("mkdir needs a name");
                continue;
            }
            if (mkdir_cmd(arg)) {
                printf("Directory '%s' created.\n", arg);
                save_backup(backup_path);
            }
        } else if (strcmp(cmd, "touch") == 0) {
            char *arg = strtok(NULL, " \t\n");
            if (!arg) {
                error("touch needs a name");
                continue;
            }
            if (touch_cmd(arg)) {
                printf("File '%s' created.\n", arg);
                save_backup(backup_path);
            }
        } else if (strcmp(cmd, "cd") == 0) {
            char *arg = strtok(NULL, " \t\n");
            if (!arg) {
                error("cd needs a path");
                continue;
            }
            cd_cmd(arg);
        } else if (strcmp(cmd, "exit") == 0) {
            save_backup(backup_path);
            break;
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    return 0;
}
