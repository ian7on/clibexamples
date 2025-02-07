#ifndef AVL_TREE_H
#define AVL_TREE_H

/**
 * @brief AVL Tree, no recursive functions.
 * @copyright Anton Ivanov, MIT License 2025
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef BUILD_UNIT_TESTS
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define TEST_ASSERT(x) assert(x)
#define TEST_PRINTF(...) printf(__VA_ARGS__)
#else
#define TEST_ASSERT(x)
#define TEST_PRINTF(...)
#endif


/** @brief Key type for AVL Tree: 64 bit */
typedef uint64_t avl_key_t;
typedef uint8_t avl_height_t; ///< for max key ( 2^64 ) max height < 1.44 * log2(n) ~ 92

/** @brief AVL Tree node. */
typedef struct avl_node_s {
    avl_key_t key;
    struct avl_node_s *left;
    struct avl_node_s *right;
    struct avl_node_s *parent; ///< for easier balancing and to avoid recursion
    avl_height_t height;
} avl_node_t;

/** @brief AVL Tree. */
typedef struct avl_tree_s {
    avl_node_t *root;
} avl_tree_t;

/** @brief A type holding node comparison results for this AVL Tree implementation */
typedef enum {
    AVL_CMP_EQ,
    AVL_CMP_LT,
    AVL_CMP_GT,
} avl_node_cmp_result_t;

#ifdef BUILD_UNIT_TESTS
#define AVL_NODE_TO_STR_BUFF_SIZE 21
static inline char *avl_node_to_str(avl_node_t *node) {
    static char ret_str[AVL_NODE_TO_STR_BUFF_SIZE]; // Enough to hold a uint64_t + null terminator
    if (NULL == node) {
        // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        (void)snprintf(ret_str, strlen("NULL") + 1, "%s", "NULL");
    } else {
        // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        (void)snprintf(ret_str, AVL_NODE_TO_STR_BUFF_SIZE, "%lu", node->key);
    }
    return ret_str;
}
#endif


#ifndef AVL_TREE_NODE_CMP_FN_EXTERNAL
/**
 * @brief Node comparison function: left is smaller, right is larger.
 *
 *  64-bit key paired with 32-bit collision index for cases when keys match.
 */
static inline avl_node_cmp_result_t avl_node_cmp(avl_node_t *node_a, avl_node_t *node_b) {
    avl_node_cmp_result_t res = AVL_CMP_EQ;
    if (node_a->key < node_b->key) {
        res = AVL_CMP_LT;
    } else if (node_a->key > node_b->key) {
        res = AVL_CMP_GT;
    }
    return res;
}
#endif

/**
 * @brief Return node's height.
 *
 * @param node AVL-Tree node @ref avl_node_t.
 * @return Height of node's subtree.
 */
static inline avl_height_t avl_node_height(avl_node_t *node) {
    return (NULL == node) ? 0 : node->height;
}

/**
 * @brief Calculate balance factor of node.
 *
 * Balance factor is difference between height of right and left subtrees.
 *
 * @param node AVL-Tree node @ref avl_node_t.
 * @return Node's balance factor.
 */
static inline int32_t avl_node_balance_factor(avl_node_t *node) {
    TEST_ASSERT(NULL != node);
    // NOLINTNEXTLINE(clang-analyzer-core.NullDereference) -- algorithmically not possible
    return (int32_t)avl_node_height(node->right) - (int32_t)avl_node_height(node->left);
}

/**
 * @brief Find node with minimum key in AVL-subtree.
 *
 * @param node Root node @ref avl_node_t of AVL-Tree.
 * @return Node with minimum key.
 */
static inline avl_node_t *avl_node_find_min(avl_node_t *node) {
    avl_node_t *current = node;
    TEST_ASSERT(NULL != node);
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

/**
 * @brief Recalculate height of node's subtree.
 *
 * This function relies on the height of the left and right subtrees being correct.
 *
 * @param node AVL-Tree node @ref avl_node_t.
 */
static inline void avl_node_height_calc(avl_node_t *node) {
    avl_height_t left_height = avl_node_height(node->left);
    avl_height_t right_height = avl_node_height(node->right);
    node->height = ((left_height > right_height) ? left_height : right_height) + 1;
}

/**
 * @brief Update parent of node.
 *
 * @param curr_root AVL-Tree node @ref avl_node_t.
 * @param new_root New root node @ref avl_node_t.
 */
static inline void avl_node_parent_update(avl_node_t *old_node, avl_node_t *new_node) {
    if (NULL != new_node->parent) {
        if (new_node->parent->left == old_node) {
            new_node->parent->left = new_node;
        } else if (new_node->parent->right == old_node) {
            new_node->parent->right = new_node;
        }
    }
}

/**
 * @brief Rotate node to the right.
 *
 * @param curr_root AVL-Tree node @ref avl_node_t.
 * @return New root node.
 */
static inline avl_node_t *avl_node_rotate_right(avl_node_t *curr_root) {
    TEST_ASSERT(NULL != curr_root);
    TEST_PRINTF("rotate right @ %lu\n", curr_root->key);
    avl_node_t *new_root = curr_root->left;
    curr_root->left = new_root->right;
    if (new_root->right != NULL) {
        new_root->right->parent = curr_root;
    }
    new_root->right = curr_root;
    new_root->parent = curr_root->parent;
    curr_root->parent = new_root;
    avl_node_parent_update(curr_root, new_root);
    avl_node_height_calc(curr_root);
    avl_node_height_calc(new_root);
    return new_root;
}

/**
 * @brief Rotate node to the left.
 *
 * @param curr_root AVL-Tree node @ref avl_node_t.
 * @return New root node.
 */
static inline avl_node_t *avl_node_rotate_left(avl_node_t *curr_root) {
    TEST_ASSERT(NULL != curr_root);
    TEST_PRINTF("rotate left @ %lu\n", curr_root->key);
    avl_node_t *new_root = curr_root->right;
    curr_root->right = new_root->left;
    if (new_root->left != NULL) {
        new_root->left->parent = curr_root;
    }
    new_root->left = curr_root;
    new_root->parent = curr_root->parent;
    curr_root->parent = new_root;
    avl_node_parent_update(curr_root, new_root);
    avl_node_height_calc(curr_root);
    avl_node_height_calc(new_root);
    return new_root;
}

/**
 * @brief Balance node.
 *
 * @param node AVL-Tree node @ref avl_node_t.
 * @return New root node.
 */
static inline avl_node_t *avl_node_balance(avl_node_t *node) {
    TEST_ASSERT(NULL != node);
    TEST_PRINTF("balance @ %lu\n", node->key);
    avl_node_t *new_root_node = node;
    avl_node_height_calc(node);
    if (avl_node_balance_factor(node) == 2) {
        if (avl_node_balance_factor(node->right) < 0) {
            node->right = avl_node_rotate_right(node->right);
        }
        new_root_node = avl_node_rotate_left(node);
    }
    if (avl_node_balance_factor(node) == -2) {
        if (avl_node_balance_factor(node->left) > 0) {
            node->left = avl_node_rotate_left(node->left);
        }
        new_root_node = avl_node_rotate_right(node);
    }
    return new_root_node;
}

/**
 * @brief Find node with key in AVL-Tree.
 *
 * @param node Root node @ref avl_node_t of AVL-Tree.
 * @param key Unique key of node @ref avl_key_t.
 * @return Node with key or NULL if not found.
 */
static inline avl_node_t *avl_tree_node_lookup(avl_node_t *node, avl_key_t key) {
    avl_node_t *current = node;
    avl_node_t *node_found = NULL;
    avl_node_t tmp_node = {.left = NULL, .right = NULL, .parent = NULL, .height = 0, .key = key};
    while ((NULL == node_found) && (NULL != current)) {
        switch (avl_node_cmp(&tmp_node, current)) {
        case AVL_CMP_LT:
            current = current->left;
            break;
        case AVL_CMP_GT:
            current = current->right;
            break;
        case AVL_CMP_EQ:
            node_found = current;
            break;
        default:
            TEST_ASSERT(false); // must never happen
            break;
        }
    }
    return node_found;
}

/**
 * @brief Insert a node into AVL-Tree.
 * @note If key already exists, the function does nothing.
 *
 * @param root_node A root node @ref avl_node_t in a AVL-Tree.
 * @param new_node New node @ref avl_node_t to insert.
 * @return New root node.
 */
static inline avl_node_t *avl_tree_node_insert(avl_node_t *root_node, avl_node_t *new_node) {
    bool key_exists = false;
    avl_node_t *parent = NULL;
    avl_node_t *current = root_node;
    avl_node_t *new_root_node = new_node; // automatically covers the empty tree case
    avl_node_height_calc(new_node);

    // Find the parent of the new node.
    while (NULL != current || key_exists) {
        parent = current;

        switch (avl_node_cmp(new_node, current)) {
        case AVL_CMP_LT:
            current = current->left;
            break;
        case AVL_CMP_GT:
            current = current->right;
            break;
        case AVL_CMP_EQ:
            // Key already exists.
            TEST_ASSERT(new_node->key != current->key);
            new_root_node = root_node;
            key_exists = true;
            break;
        default:
            TEST_ASSERT(false); // must never happen
            break;
        }
    }

    // Insert the new node.
    if (!key_exists) {
        TEST_PRINTF("parent of %s will be %s\n", avl_node_to_str(new_node),
                    avl_node_to_str(parent));
        new_node->parent = parent;

        // Insert the new node.
        if (NULL != parent) {
            switch (avl_node_cmp(new_node, parent)) {
            case AVL_CMP_LT:
                TEST_ASSERT(NULL == parent->left);
                parent->left = new_node;
                break;
            case AVL_CMP_GT:
                TEST_ASSERT(NULL == parent->right);
                parent->right = new_node;
                break;
            case AVL_CMP_EQ:
            default:
                TEST_ASSERT(false); // must never happen
                break;
            }
        }

        // Rebalance the tree, searching for the new root node.
        current = new_node;
        while (current != NULL) {
            new_root_node = avl_node_balance(current);
            TEST_PRINTF("new root = %lu\n", new_root_node->key);
            current = new_root_node->parent;
        }
    }
    return new_root_node;
}

/**
 * @brief Remove a node from AVL-Tree.
 *
 * @param root_node A root node @ref avl_node_t in a AVL-Tree.
 * @param key Key of node to remove @ref avl_key_t.
 * @return New root node.
 */
static inline avl_node_t *avl_tree_remove_node(avl_node_t *root_node, avl_key_t key) {
    avl_node_t *new_root_node = root_node;
    avl_node_t *node_to_remove = avl_tree_node_lookup(root_node, key);

    if (node_to_remove != NULL) {
        avl_node_t *replacement_node = NULL;
        avl_node_t *node_to_rebalance_from = NULL;

        TEST_PRINTF("Node %lu found\n", node_to_remove->key);

        if (node_to_remove->right != NULL) {
            TEST_PRINTF("Node has a right child, replacement node is min of the right subtree\n");
            replacement_node = avl_node_find_min(node_to_remove->right);
            TEST_PRINTF("replacement node is %lu\n", replacement_node->key);

            // Remove the replacement node from its current position.
            if (replacement_node->parent->left == replacement_node) {
                replacement_node->parent->left = replacement_node->right;
                if (replacement_node->right != NULL) {
                    replacement_node->right->parent = replacement_node->parent;
                }
            } else {
                replacement_node->parent->right = replacement_node->right;
                if (replacement_node->right != NULL) {
                    replacement_node->right->parent = replacement_node->parent;
                }
            }

            node_to_rebalance_from = (replacement_node->parent == node_to_remove)
                                         ? replacement_node
                                         : replacement_node->parent;

            // Replace node_to_remove with replacement_node.
            replacement_node->left = node_to_remove->left;
            if (replacement_node->left != NULL) {
                replacement_node->left->parent = replacement_node;
            }
            replacement_node->right = node_to_remove->right;
            if (replacement_node->right != NULL) {
                replacement_node->right->parent = replacement_node;
            }
            replacement_node->parent = node_to_remove->parent;
        } else if (node_to_remove->left != NULL) {
            TEST_PRINTF("Node has only a left child, the hight of left subtree: %u\n",
                        avl_node_height(node_to_remove->left));
            replacement_node = node_to_remove->left;
            TEST_PRINTF("replacement node is %lu\n", replacement_node->key);
            replacement_node->parent = node_to_remove->parent;
            node_to_rebalance_from = replacement_node;
        } else {
            TEST_PRINTF("Node has no children, thus no replacement node\n");
            node_to_rebalance_from = node_to_remove->parent;
            replacement_node = NULL;
        }

        // Adjust the parent of node_to_remove to point to the replacement_node.
        if (node_to_remove->parent != NULL) {
            if (node_to_remove->parent->left == node_to_remove) {
                node_to_remove->parent->left = replacement_node;
            } else {
                node_to_remove->parent->right = replacement_node;
            }
        } else {
            // Node to remove was the root node.
            new_root_node = replacement_node;
        }

        // Clean up the removed node.
        node_to_remove->left = NULL;
        node_to_remove->right = NULL;
        node_to_remove->parent = NULL;

        // Rebalance the tree starting from node_to_rebalance_from.
        TEST_PRINTF("node to rebalance from: %s\n", avl_node_to_str(node_to_rebalance_from));
        avl_node_t *current = node_to_rebalance_from;
        while (current != NULL) {
            new_root_node = avl_node_balance(current);
            current = current->parent;
        }
    }

    return new_root_node;
}

#endif // AVL_TREE_H
