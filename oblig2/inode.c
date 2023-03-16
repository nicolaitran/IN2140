
#include "inode.h"
#include "alloca.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BLOCKSIZE 4096

int id = 0;

struct inode *create_file(struct inode *parent, char *name, char readonly, int size_in_bytes)
{
    if (find_inode_by_name(parent, name) != NULL)
    {
        return NULL;
    }

    struct inode *node = malloc(sizeof(struct inode));
    if (node == NULL)
    {
        fprinf(stderr, "Malloc failed. Possibly out of memeory \n");
        free(node);
        exit(EXIT_FAILURE);
    }
    node->name = strdup(name);
    if (node->name == NULL)
    {
        perror("strdup");
        free(node);
        exit(EXIT_FAILURE);
    }
    node->is_directory = 0;
    node->filesize = size_in_bytes;
    node->is_readonly = readonly;

    node->num_entries = 64; // Denne må endres

    size_t *entries = malloc(node->num_entries * sizeof(struct inode));
    if (entries == NULL)
    {
        fprinf(stderr, "Malloc failed. Possibly out of memeory \n");
        free(entries);
        exit(EXIT_FAILURE);
    }
    node->entries = entries;

    return node;
}

struct inode *create_dir(struct inode *parent, char *name)
{
    if (find_inode_by_name(parent, name) != NULL)
    {

        return NULL;
    }
    struct inode *node = malloc(sizeof(struct inode));
    if (node == NULL)
    {
        fprinf(stderr, "Malloc failed. Possibly out of memeory \n");
        free(node);
        exit(EXIT_FAILURE);
    }
    node->name = strdup(name);
    node->filesize = 0;
    node->is_directory = 1;
    node->is_readonly = 0;
    node->num_entries = 0;

    return NULL;
}

struct inode *find_inode_by_name(struct inode *parent, char *name)
{
    parent = malloc(sizeof(struct inode));

    if (parent->is_directory)
    {
        for (int x = 0; x < parent->num_entries; x++)
        {
            struct inode *children = parent->entries[x];
            if (strcmp(children->name, name) == 0)
            {

                free(parent);
                return children;
            }
        }
    }
    free(parent);
    return NULL;
}

struct inode *load_inodes()
{
    FILE *f;

    f = fopen("masterfil", "r");
    if (f == NULL)
    {
        perror("Finner ikke fil");
        exit(EXIT_FAILURE);
    }

    struct inode *r = make_inode(f);
    fclose(f);

    return r;
}

void fs_shutdown(struct inode *inode)
{
}

/* This static variable is used to change the indentation while debug_fs
 * is walking through the tree of inodes and prints information.
 */
static int indent = 0;

void debug_fs(struct inode *node)
{
    if (node == NULL)
        return;
    for (int i = 0; i < indent; i++)
        printf("  ");
    if (node->is_directory)
    {
        printf("%s (id %d)\n", node->name, node->id);
        indent++;
        for (int i = 0; i < node->num_entries; i++)
        {
            struct inode *child = (struct inode *)node->entries[i];
            debug_fs(child);
        }
        indent--;
    }
    else
    {
        printf("%s (id %d size %db blocks ", node->name, node->id, node->filesize);
        for (int i = 0; i < node->num_entries; i++)
        {
            printf("%d ", (int)node->entries[i]);
        }
        printf(")\n");
    }
}