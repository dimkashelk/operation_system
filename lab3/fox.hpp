#ifndef FOX_HPP
#define FOX_HPP

#include <cstddef>
#include "binary-tree.hpp"

class Fox
{
    public:
        Fox(std::size_t id, TreeNode * head, TreeNode * pos);

        const int get_id() const;
        void set_pos(TreeNode * pos);
        void update();

    private:
        int id_;
        TreeNode * head_;
        TreeNode * pos_;
};

std::pair< TreeNode *, TreeNode * > push_fox(TreeNode * head, Fox * fox);
void draw_binary_tree(std::ostream & out, const TreeNode * head, std::size_t amount);
void draw_binary_tree(std::ostream & out, std::size_t prefix, const TreeNode * src, std::size_t amount);

#endif
