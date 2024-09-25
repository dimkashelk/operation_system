#include "binary-tree.hpp"
#include <iostream>

void free_binary_tree(TreeNode * head)
{
    if (head == nullptr)
    {
        return;
    }
    free_binary_tree(head->__left);
    free_binary_tree(head->__right);
    delete head;
}
