#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "avl_tree.h"

#define MAX_NODES 1024

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static avl_tree_t avl_tree = {.root = NULL};
static avl_node_t avl_node_buffer[MAX_NODES];
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

static inline void avl_tree_validate(avl_node_t *node, avl_node_t *parent) {
#ifdef DEBUG
    if (NULL != node) {
        printf("-"); // Validation suffix begin
        // Parent
        if (NULL == parent) {
            assert(NULL == node->parent); // Tree root node has no parent
            printf("R");
        } else {
            assert(NULL != node->parent); // Tree leaves have parent
            printf("P");
        }
        // Left child
        if (NULL != node->left) {
            assert(AVL_CMP_GT != avl_node_cmp(node->left, node)); // left child not greater
            assert(node->left->parent == node);   // left child's parent is this node
            printf("L");
        }
        // Right child
        if (NULL != node->right) {
            assert(node->right->key >= node->key); // right child not smaller
            assert(node->right->parent == node);   // right child's parent is this node
            printf("R");
        }
        // Balance factor within [-1, 1].
        int32_t balance_factor = avl_node_balance_factor(node);
        assert((-1 <= balance_factor) && (balance_factor <= 1));
        printf("B");
        // Check heights are correct.
        int left_height = avl_node_height(node->left);
        int right_height = avl_node_height(node->right);
        assert(node->height == 1 + (left_height > right_height ? left_height : right_height));
        printf("H");
        printf("-valid\n");
    }
#else
    (void)node;
    (void)parent;
    printf("\n");
#endif
}

// NOLINTBEGIN(misc-no-recursion)
// Recursion: this is a visual helper to a test code, recursion is OK here
static inline void avl_tree_node_print(avl_node_t *node, avl_node_t *parent, int level) {
    if (NULL != node) {
        avl_tree_node_print(node->right, node, level + 1);
        for (int i = 0; i < level; i++) {
            printf("\t");
        }
        printf("%lu[h%u.b%d.p%lu]", node->key, node->height, avl_node_balance_factor(node),
               node->parent ? node->parent->key : 0);
        avl_tree_validate(node, parent);
        avl_tree_node_print(node->left, node, level + 1);
    }
}
// NOLINTEND(misc-no-recursion)

static inline void test_avl_node_buffer_init_sequential(void) {
    printf("\n========================\n");
    TEST_PRINTF("INIT NODES: [");
    for (avl_key_t i = 0; i < MAX_NODES; i++) {
        avl_node_buffer[i].key = i + 1;
        avl_node_buffer[i].left = NULL;
        avl_node_buffer[i].right = NULL;
        avl_node_buffer[i].parent = NULL;
        avl_node_buffer[i].height = 0;
        TEST_PRINTF("%lu, ", avl_node_buffer[i].key);
    }
    TEST_PRINTF("]");
    printf("\n========================\n");
}

static inline void test_avl_node_buffer_init_random(void) {
    printf("\n========================\n");
    TEST_PRINTF("INIT NODES: [");
    for (int i = 0; i < MAX_NODES; i++) {
        bool key_exists = false;
        do {
            key_exists = false;
            // NOLINTNEXTLINE -- limited randomness is acceptable, concurrency excluded
            avl_node_buffer[i].key = (avl_key_t)(rand() % (10 * MAX_NODES));
            for (int j = 0; j < i; j++) {
                if (avl_node_buffer[j].key == avl_node_buffer[i].key) {
                    key_exists = true;
                    break;
                }
            }
        } while (key_exists);
        avl_node_buffer[i].left = NULL;
        avl_node_buffer[i].right = NULL;
        avl_node_buffer[i].parent = NULL;
        avl_node_buffer[i].height = 0;
        TEST_PRINTF("%lu, ", avl_node_buffer[i].key);
    }
    TEST_PRINTF("]");
    printf("\n========================\n");
}

static inline void test_find_all_nodes(void) {
    printf("\n------------------------\n");
    for (int i = 0; i < MAX_NODES; i++) {
        avl_node_t *node = avl_tree_node_lookup(avl_tree.root, avl_node_buffer[i].key);
        if (NULL != node) {
            printf("Found node %lu\n", node->key);
        } else {
            printf("Node %lu not found\n", avl_node_buffer[i].key);
        }
    }
    printf("------------------------\n");
}

static inline void test_insert_nodes_in_order(void) {
    for (int i = 0; i < MAX_NODES; i++) {
        printf("\n------------------------\n");
        printf("Insert node %lu\n", avl_node_buffer[i].key);
        avl_tree.root = avl_tree_node_insert(avl_tree.root, &avl_node_buffer[i]);
        avl_tree_node_print(avl_tree.root, NULL, 0);
        printf("------------------------\n");
    }
}

static inline void test_insert_nodes_in_reverse_order(void) {
    for (int i = MAX_NODES - 1; i >= 0; i--) {
        printf("\n------------------------\n");
        printf("Insert node %lu\n", avl_node_buffer[i].key);
        avl_tree.root = avl_tree_node_insert(avl_tree.root, &avl_node_buffer[i]);
        avl_tree_node_print(avl_tree.root, NULL, 0);
        printf("------------------------\n");
    }
}

static inline void test_remove_nodes_in_order(void) {
    for (int i = 0; i < MAX_NODES; i++) {
        printf("\n------------------------\n");
        printf("Removing node %lu\n", avl_node_buffer[i].key);
        TEST_ASSERT(avl_tree_node_lookup(avl_tree.root, avl_node_buffer[i].key) != NULL);
        avl_tree.root = avl_tree_remove_node(avl_tree.root, avl_node_buffer[i].key);
        avl_tree_node_print(avl_tree.root, NULL, 0);
        printf("------------------------\n");
    }
}

static inline void test_remove_nodes_in_reverse_order(void) {
    for (int i = MAX_NODES - 1; i >= 0; i--) {
        printf("\n------------------------\n");
        printf("Removing node %lu\n", avl_node_buffer[i].key);
        TEST_ASSERT(avl_tree_node_lookup(avl_tree.root, avl_node_buffer[i].key) != NULL);
        avl_tree.root = avl_tree_remove_node(avl_tree.root, avl_node_buffer[i].key);
        avl_tree_node_print(avl_tree.root, NULL, 0);
        printf("------------------------\n");
    }
}


int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    printf("Size of avl_node_t: %lu Bytes.\n", sizeof(avl_node_t));

    uint32_t random_seed = (uint32_t)time(NULL);
    printf("Using random_seed: %d\n", random_seed);
    srand(random_seed);

    // First, test with sequential keys
    test_avl_node_buffer_init_sequential();

    test_insert_nodes_in_order();
    test_find_all_nodes();
    test_remove_nodes_in_order();

    test_insert_nodes_in_reverse_order();
    test_find_all_nodes();
    test_remove_nodes_in_reverse_order();

    // Second, test with random keys
    test_avl_node_buffer_init_random();

    test_insert_nodes_in_order();
    test_find_all_nodes();
    test_remove_nodes_in_order();

    test_insert_nodes_in_reverse_order();
    test_find_all_nodes();
    test_remove_nodes_in_reverse_order();

    return 0;
}
