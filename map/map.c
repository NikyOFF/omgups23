#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

map_t
map_create_node(map_key key, map_value value, map_t next)
{
    /* Creates the map node. */
    map_t map = malloc(sizeof(map_node_t));
    map->key = key;
    strcpy(map->value, value);
    map->next = next;

    return map;
}

void
map_add(map_t* map, map_key key, map_value value)
{
    /* Finds the correct insertion position. */
    while (*map != NULL && (*map)->key < key)
        map = &(*map)->next;

    /* If already exists, updates its value. */
    if (*map != NULL && (*map)->key == key)
        strcpy((*map)->value, value);

        /* Otherwise, creates the node. */
    else
        *map = map_create_node(key, value, *map);
}

int
map_remove(map_t* map, map_key key)
{
    /* Finds the correct removal position. */
    while (*map != NULL && (*map)->key < key)
        map = &(*map)->next;

    /* Key not found. */
    if (*map == NULL || (*map)->key > key)
        return 0;

    /* Removes the map node. */
    map_t map_to_remove = *map;
    *map = (*map)->next;
    free(map_to_remove);
    return 1;
}

int
map_get(map_t map, map_key key, map_value value)
{
    /* Finds the correct node position. */
    while (map != NULL && map->key < key)
        map = map->next;

    /* If key exists, copies its value to the variable. */
    if (map != NULL && map->key == key)
        strcpy(value, map->value);

    /* Returns true if key exists; false otherwise. */
    return (map != NULL && map->key == key);
}

void
map_destroy(map_t* map)
{
    while (*map != NULL)
    {
        map_t map_to_remove = *map;
        *map = (*map)->next;
        free(map_to_remove);
    }
}