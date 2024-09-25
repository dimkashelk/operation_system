#ifndef BINARY_TREE_HPP
#define BINARY_TREE_HPP

#include <ostream>

class Fox;
struct TreeNode
{
    Fox * __fox;
    TreeNode * __left;
    TreeNode * __right;
    TreeNode * __head;
};

void free_binary_tree(TreeNode * head);

#endif
