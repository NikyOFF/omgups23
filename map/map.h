#ifndef __C_MAP_H__
#define __C_MAP_H__

typedef int map_key;    /* Keys are integers. */
typedef char map_value[512]; /* Values are chars. */

/**
 * @brief: Represents a map node. Each item in a map
 *         is a node.
 */
typedef struct map_node_t {
    map_key key;             /* Node's key. */
    map_value value;    /* Node's value. */
    struct map_node_t* next; /* Next map node. */
} map_node_t;

/**
 * @brief: Represents a map. A map is just a pointer
 *         to its first node.
 */
typedef map_node_t* map_t;

/**
 * @brief: Creates a map node. Used to create the
 *         initial map.
 */
map_t
map_create_node(map_key key, map_value value, map_t next);

/**
 * @brief: Adds a new map node to the given map.
 */
void
map_add(map_t* map, map_key key, map_value value);

/**
 * @brief: Removes the map node from the map.
 */
int
map_remove(map_t* map, map_key key);

/**
 * @brief: Gets the value related to the given key.
 */
int
map_get(map_t map, map_key key, map_value value);

/**
 * @brief: Destroys the given map, freeing the memory.
 */
void
map_destroy(map_t* map);

#endif /* __C_MAP_H__ */