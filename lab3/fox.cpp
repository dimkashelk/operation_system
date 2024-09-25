#include "fox.hpp"

#include <functional>
#include <random>
#include <utility>

Fox::Fox(std::size_t id, TreeNode * head, TreeNode * pos):
    id_(id),
    head_(head),
    pos_(pos)
{}

const int Fox::get_id() const
{
    return id_;
}

void Fox::set_pos(TreeNode * pos)
{
    pos_ = pos;
}

void Fox::update()
{
    if (pos_->__head != nullptr && pos_->__head->__fox == nullptr)
    {
        auto temp = pos_;
        pos_ = pos_->__head;
        pos_->__fox = temp->__fox;
        head_ = pos_->__head;
        temp->__fox = nullptr;
    }
}

// TREE

std::pair< TreeNode *, TreeNode * >  push_fox(TreeNode * head, Fox * fox)
{
    // auto flip = std::bind(std::uniform_int_distribution<>(0, 10), std::default_random_engine());
    if (head == nullptr)
    {
        head = new TreeNode {fox, nullptr, nullptr, nullptr};
        return std::make_pair(head, head);
    }

    auto new_fox = new TreeNode {fox, nullptr, nullptr, nullptr};

    TreeNode * new_head = nullptr;
    if (true)
    {
        new_head = new TreeNode {nullptr, head, new_fox, nullptr};
    }
    else
    {
        new_head = new TreeNode {nullptr, new_fox, head, nullptr};
    }
    head->__head = new_head;
    new_fox->__head = new_head;
    
    return std::make_pair(new_head, new_fox);
}

void draw_binary_tree(std::ostream & out, std::size_t prefix, const TreeNode * src, std::size_t amount)
{
    if (src == NULL)
    {
        return;
    }
    prefix += amount;

    draw_binary_tree(out, prefix, src->__right, amount);

    out << std::endl;
    for (std::size_t i = amount; i < prefix; ++i)
    {
        out << " ";
    }

    if (src->__fox != nullptr)
    {
        out << src->__fox->get_id() << std::endl;
    }
    else
    {
        out << "X" << std::endl; 
    }

    draw_binary_tree(out, prefix, src->__left, amount);
}

void draw_binary_tree(std::ostream & out, const TreeNode * head, std::size_t amount)
{
    draw_binary_tree(out, 0, head, amount);
}

