/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBBITCOIN_BLOCKCHAIN_BINARY_TRIE_IPP
#define LIBBITCOIN_BLOCKCHAIN_BINARY_TRIE_IPP

namespace libbitcoin {
namespace chain {

// value_list_node implementation
template<typename Value>
value_list_node<Value>::value_list_node()
: anchor(nullptr), next(nullptr), previous(nullptr), value()
{
}

template<typename Value>
value_list_node<Value>::value_list_node(const value_type& value)
: anchor(nullptr), next(nullptr), previous(nullptr), value(value)
{
}

// trie_node implenentation
template<typename Value>
trie_node<Value>::trie_node()
: parent(0), value_head(nullptr), value_tail(nullptr), value_leftmost(nullptr),
    value_rightmost(nullptr), previous(nullptr), next(nullptr), label()
{
    children[0] = nullptr;
    children[1] = nullptr;
}

template<typename Value>
typename trie_node<Value>::node_type* trie_node<Value>::get_first_child() const
{
    node_type* result = children[0];

    if (result == nullptr)
    {
        result = children[1];
    }

    return result;
}

template<typename Value>
typename trie_node<Value>::node_type* trie_node<Value>::get_last_child() const
{
    node_type* result = children[1];

    if (result == nullptr)
    {
        result = children[0];
    }

    return result;
}

template<typename Value>
bool trie_node<Value>::has_children() const
{
    return (children[0] != nullptr) || (children[1] != nullptr);
}

template<typename Value>
bool trie_node<Value>::has_value() const
{
    return !(value_head == nullptr);
}

// trie_iterator implementation
template <typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator()
: trie_node_(nullptr), value_node_(nullptr)
{
}

template <typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(trie_node_type* node)
: trie_node_(node), value_node_(node->value_head)
{
}

template <typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(value_node_type* value)
: trie_node_(value->anchor), value_node_(value)
{
}

template <typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(trie_node_type* node,
    value_node_type* value)
: trie_node_(node), value_node_(value)
{
}

template <typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(const iterator& it)
: trie_node_(it.trie_node_), value_node_(it.value_node_)
{
}

template <typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::reference
    trie_iterator<Value, Reference, Pointer>::operator*() const
{
    return value_node_->value;
}

template <typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::pointer
    trie_iterator<Value, Reference, Pointer>::operator->() const
{
    return &(operator*());
}

template <typename Value, typename Reference, typename Pointer>
bool trie_iterator<Value, Reference, Pointer>::operator==(
    const trie_iterator& other) const
{
    return (trie_node_ == other.trie_node_)
        && (value_node_ == other.value_node_);
}

template <typename Value, typename Reference, typename Pointer>
bool trie_iterator<Value, Reference, Pointer>::operator!=(
    const trie_iterator& other) const
{
    return !(*this == other);
}

template <typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type&
    trie_iterator<Value, Reference, Pointer>::operator++()
{
    increment();
    return *this;
}

template <typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type
    trie_iterator<Value, Reference, Pointer>::operator++(int)
{
    iter_type tmp = *this;
    increment();
    return tmp;
}

template <typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type&
    trie_iterator<Value, Reference, Pointer>::operator--()
{
    decrement();
    return *this;
}

template <typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type
    trie_iterator<Value, Reference, Pointer>::operator--(int)
{
    iter_type tmp = *this;
    decrement();
    return tmp;
}

template <typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::trie_node_increment()
{
    if (trie_node_->parent != nullptr)
    {
        trie_node_ = trie_node_->next;
    }
}

template <typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::trie_node_decrement()
{
    if (trie_node_->previous->parent != nullptr)
    {
        trie_node_ = trie_node_->previous;
    }
}

template <typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::increment()
{
    // if not at end of current trie node values, continue walk
    if ((value_node_ != nullptr) && (value_node_->next != nullptr))
    {
        value_node_ = value_node_->next;
    }
    // else move to next trie node
    else
    {
        trie_node_increment();
        value_node_ = trie_node_->value_head;
    }
}

template <typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::decrement()
{
    if ((value_node_ != nullptr) && (value_node_->previous != nullptr))
    {
        value_node_ = value_node_->previous;
    }
    else
    {
        trie_node_decrement();
        value_node_ = trie_node_->value_tail;
    }
}

template <typename Value, typename Reference, typename Pointer>
binary_type trie_iterator<Value, Reference, Pointer>::get_key()
{
    binary_type result;

    std::vector<trie_node_type*> nodes;

    auto current = trie_node_;

    while (current != nullptr)
    {
        nodes.push_back(current);
        current = current->parent;
    }

    auto it = nodes.rbegin();

    while (it != nodes.rend())
    {
        result.append((*it)->label);
        it++;
    }

    return result;
}

// trie_reverse_iterator implementation
template <typename TrieIterator>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator()
: base_iterator_()
{
}

template <typename TrieIterator>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator(base_iter_type it)
: base_iterator_(it)
{
}

template <typename TrieIterator>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator(
    const iter_type& other)
: base_iterator_(other.base())
{
}

template <typename TrieIterator>
template<typename Iter>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator(
    const trie_reverse_iterator<Iter>& other)
: base_iterator_(other.base())
{
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::reference
    trie_reverse_iterator<TrieIterator>::operator*() const
{
    base_iter_type tmp = base_iterator_;
    return *--tmp;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::pointer
    trie_reverse_iterator<TrieIterator>::operator->() const
{
    return &(operator*());
}

template <typename TrieIterator>
bool trie_reverse_iterator<TrieIterator>::operator==(
    const iter_type& other) const
{
    return base() == other.base();
}

template <typename TrieIterator>
bool trie_reverse_iterator<TrieIterator>::operator!=(
    const iter_type& other) const
{
    return !(*this == other);
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type&
    trie_reverse_iterator<TrieIterator>::operator++()
{
    --base_iterator_;
    return *this;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type
    trie_reverse_iterator<TrieIterator>::operator++(int)
{
    iter_type tmp = *this;
    --base_iterator_;
    return tmp;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type&
    trie_reverse_iterator<TrieIterator>::operator--()
{
    ++base_iterator_;
    return *this;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type
    trie_reverse_iterator<TrieIterator>::operator--(int)
{
    iter_type tmp = *this;
    ++base_iterator_;
    return tmp;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::base_iter_type
    trie_reverse_iterator<TrieIterator>::base() const
{
    return base_iterator_;
}

template <typename TrieIterator>
binary_type trie_reverse_iterator<TrieIterator>::get_key()
{
    iter_type tmp = base_iterator_;
    return (--tmp).get_key();
}

// binary_trie implementation
template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
binary_trie<Value, NodeAlloc, ValueNodeAlloc>::binary_trie(
    trie_node_allocator trie_allocator,
    value_list_node_allocator value_allocator)
: node_allocator_(trie_allocator), value_allocator_(value_allocator),
    root_(create_trie_node())
{
    root_->next = root_;
    root_->previous = root_;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
binary_trie<Value, NodeAlloc, ValueNodeAlloc>::~binary_trie()
{
    destroy();
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::destroy()
{
    erase_subtree(root_);
    root_ = nullptr;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::create_trie_node()
{
    trie_node_type* node = node_allocator_.allocate(1);

    if (node == nullptr)
    {
        throw std::bad_alloc();
    }

    new(node) trie_node_type();

    return node;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::create_trie_node(
    const binary_type& key)
{
    trie_node_type* node = create_trie_node();

    node->label = key;

    return node;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::create_trie_node(
    const binary_type& key, const value_node_type* value_node)
{
    trie_node_type* node = create_trie_node(key);

    append_value(node, value_node);

    return node;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::create_trie_node(
    const binary_type& key, const value_type& value)
{
    trie_node_type* node = create_trie_node(key);
    value_node_type* value_node = create_value_node(value);

    append_value(node, value_node);

    return node;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
bool binary_trie<Value, NodeAlloc, ValueNodeAlloc>::destroy_trie_node(
    trie_node_type* node)
{
    bool result = false;

    if (node != nullptr)
    {
        erase_values(node);
        node_allocator_.destroy(node);
        node_allocator_.deallocate(node, 1);
        result = true;
    }
    
    return result;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::erase_values(
    trie_node_type* node)
{
    if (node != nullptr)
    {
        value_node_type* value = node->value_head;
        
        while (value != nullptr)
        {
            value_node_type* tmp = value->next;
            destroy_value_node(value);
            value = tmp;
        }
        
        node->value_head = nullptr;
        node->value_tail = nullptr;
    }
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::erase_subtree(
    trie_node_type* node)
{
    if (node != nullptr)
    {
        // identify previous/next pointers which need reset to separate
        // the subtree from the rest of the tree
        trie_node_type* first_in_subtree = node;
        trie_node_type* previous_outside_subtree = nullptr;

        if (node->value_leftmost != nullptr)
        {
            first_in_subtree = node->value_leftmost->anchor;
            previous_outside_subtree = first_in_subtree->previous;
        }

        trie_node_type* last_in_subtree = node;
        trie_node_type* next_outside_subtree = nullptr;

        if (node->value_rightmost != nullptr)
        {
            last_in_subtree = node->value_rightmost->anchor;
            next_outside_subtree = last_in_subtree->next;
        }

        // sever the subtree from the remaining tree by resetting
        // previous, next, parent and child pointers
        trie_node_type* parent_from_tree = node->parent;

        if (parent_from_tree != nullptr)
        {
            bool first_bit = node->label[0];
            int child_index = first_bit ? 1 : 0;
            parent_from_tree->children[child_index] = nullptr;
            node->parent = nullptr;
        }

        first_in_subtree->previous = node;
        last_in_subtree->next = node;

        if (previous_outside_subtree != nullptr)
        {
            previous_outside_subtree->next = next_outside_subtree;
        }

        if (next_outside_subtree != nullptr)
        {
            next_outside_subtree->previous = previous_outside_subtree;
        }

        // should be done, can't do it now without propegating nullptrs
        // update_left_and_right_branch(parent_from_tree);

        // descend first children to leaf
        trie_node_type* current = get_leftmost_leaf(node);

        // erase node, remembering parent and which child was matching
        // if not first child, current is parent
        // otherwise descend first children of last child until leaf
        while (current != nullptr)
        {
            trie_node_type* next = current->parent;

            if (next != nullptr)
            {
                if (next->get_last_child() != current)
                {
                    next = get_leftmost_leaf(next->get_last_child());
                }

                bool first_bit = current->label[0];
                int child_index = first_bit ? 1 : 0;
                current->parent->children[child_index] = nullptr;
            }

            // destroy the leaf, regardless of contained values
            destroy_trie_node(current);

            current = next;
        }
    }
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::compress_branch(
    trie_node_type* node)
{
    if (node != nullptr)
    {
        bool update = true;

        // while not the trie root (designated by null parent), having no value
        // and having no children, delete leaf and replace reference with its parent
        while ((node->parent != nullptr) && !node->has_value())
        {
            trie_node_type* parent = node->parent;

            bool first_bit = node->label[0];
            int child_index = first_bit ? 1 : 0;

            if (node->has_children())
            {
                // Can we collapse this node out of the trie before termination?
                if (node->get_first_child() == node->get_last_child())
                {
                    trie_node_type* replacement = node->get_last_child();
                    replacement->label.prepend(node->label);
                    attach_child(parent, replacement);
                    destroy_trie_node(node);
                    node = replacement;
                    update = false; // was triggered by attach_child
                }

                break;
            }

            // remove child reference within parent, delete leaf
            parent->children[child_index] = nullptr;
            unlink_node(node);
            destroy_trie_node(node);
            node = parent;
        }

        if (update)
        {
            update_left_and_right_branch(node);
        }
    }

    return node;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::value_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::create_value_node(
    const value_type& value)
{
    value_node_type* node = value_allocator_.allocate(1);

    if (node == nullptr)
    {
        throw std::bad_alloc();
    }

    new(node) value_node_type(value);

    return node;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
bool binary_trie<Value, NodeAlloc, ValueNodeAlloc>::destroy_value_node(
    value_node_type* node)
{
    bool result = false;

    if (node != nullptr)
    {
        value_allocator_.destroy(node);
        value_allocator_.deallocate(node, 1);
        result = true;
    }

    return result;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::append_value(
    trie_node_type* node, value_node_type* value_node)
{
    value_node->anchor = node;
    value_node->next = node->value_head;

    if (node->value_head != nullptr)
    {
        node->value_head->previous = value_node;
    }
    else
    {
        node->value_tail = value_node;
    }

    node->value_head = value_node;

    // fixup left/right pointers
    update_left_and_right_branch(node);
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::append_value(
    trie_node_type* node, const value_type& value)
{
    value_node_type* value_node = create_value_node(value);
    append_value(node, value_node);
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::get_leftmost_leaf(
    trie_node_type* origin) const
{
    trie_node_type* current = origin;

    while (current->has_children())
    {
        current = current->get_first_child();
    }

    return current;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::get_rightmost_leaf(
    trie_node_type* origin) const
{
    trie_node_type* current = origin;

    while (current->has_children())
    {
        current = current->get_last_child();
    }

    return current;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::get_leftmost_node(
    trie_node_type* origin) const
{
    trie_node_type* current = origin;

    while (current->has_children() && !(current->has_value()))
    {
        current = current->get_first_child();
    }

    return current;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::get_rightmost_node(
    trie_node_type* origin) const
{
    return get_rightmost_leaf(origin);
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::update_left_and_right(
    trie_node_type* node)
{
    if (!node->has_children())
    {
        node->value_leftmost = node->value_head;
        node->value_rightmost = node->value_tail;
    }
    else
    {
        if (node->has_value())
        {
            node->value_leftmost = node->value_head;
        }
        else
        {
            node->value_leftmost = node->get_first_child()->value_leftmost;
        }

        node->value_rightmost = node->get_last_child()->value_rightmost;
    }
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::update_left_and_right_branch(
    trie_node_type* node)
{
    if (node != nullptr)
    {
        // fixup left/right pointers
        trie_node_type* temp = node;

        while (temp != nullptr)
        {
            update_left_and_right(temp);
            temp = temp->parent;
        }
    }
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::next_node_with_value(
    trie_node_type* node)
{
    // if at root (designated by null parent), terminate
    if (node->parent == nullptr)
        return node;

    trie_node_type* next = node;

    if (next->has_children())
    {
        // if this node has a child, then at least one value will be located
        // within the subtree, walk the subtree favoring first child until
        // a value is reached
        do
        {
            next = next->get_first_child();
        }
        while (!next->has_value());
    }
    else
    {
        // if a leaf, back up until a sibling is reached
        while (next->parent != nullptr)
        {
            trie_node_type* parent = next->parent;

            // if sibling found, decend first children until value
            if (parent->get_last_child() != next)
            {
                next = get_leftmost_node(parent->get_last_child());

                break;
            }

            next = parent;
        }
    }

    return next;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::link_node(
    trie_node_type* node)
{
    unlink_node(node);

    trie_node_type* next = next_node_with_value(node);
    trie_node_type* previous = next->previous;
    node->next = next;
    node->previous = previous;
    next->previous = node;
    previous->next = node;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::unlink_node(
    trie_node_type* node)
{
    // only unlink linked nodes (note that the root node is linked)
    if ((node->next != nullptr) && (node->previous != nullptr))
    {
        trie_node_type* next = node->next;
        trie_node_type* previous = node->previous;
        previous->next = next;
        next->previous = previous;
        node->previous = nullptr;
        node->next = nullptr;
    }
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::begin()
{
    value_node_type* value = root_->value_leftmost;
    return (value != nullptr) ? (iterator)value : (iterator)root_;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::end()
{
    return root_;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::begin() const
{
    value_node_type* value = root_->value_leftmost;
    return (value != nullptr) ? (const_iterator)value : (const_iterator)root_;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::end() const
{
    return root_;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::cbegin() const
{
    value_node_type* value = root_->value_leftmost;
    return (value != nullptr) ? (const_iterator)value : (const_iterator)root_;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::cend() const
{
    return root_;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::reverse_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::rbegin()
{
    return static_cast<reverse_iterator>(end());
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::reverse_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::rend()
{
    return static_cast<reverse_iterator>(begin());
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_reverse_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::rbegin() const
{
    return static_cast<const_reverse_iterator>(end());
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_reverse_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::rend() const
{
    return static_cast<const_reverse_iterator>(begin());
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_reverse_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::crbegin() const
{
    return rbegin();
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::const_reverse_iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::crend() const
{
    return rend();
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
void binary_trie<Value, NodeAlloc, ValueNodeAlloc>::attach_child(
    trie_node_type* parent, trie_node_type* child)
{
    // note: method trusts that the parent's child is safely overwriteable
    bool first_bit = child->label[0];
    int child_index = first_bit ? 1 : 0;

    child->parent = parent;
    parent->children[child_index] = child;

    if (child->has_value())
    {
        link_node(child);

        update_left_and_right_branch(child);
    }
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::insert_at(
    trie_node_type* current, const binary_type& key)
{
    trie_node_type* host = create_trie_node(key);

    attach_child(current, host);

    return host;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::trie_node_type*
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::insert_at(
    trie_node_type* current, const binary_type& key, const value_type& value)
{
    trie_node_type* host = create_trie_node(key, value);

    attach_child(current, host);

    return host;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::pair_iterator_bool
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::insert(
    trie_node_type*& current, const binary_type& key, const value_type& value)
{
    binary_type::size_type key_offset = 0;

    for (; key_offset < key.size();)
    {
        bool first_bit = key[key_offset];
        int index = first_bit ? 1 : 0;

        trie_node_type* initial_match = current->children[index];

        if (initial_match == nullptr)
        {
            binary_type subkey = key.get_substring(
                key_offset, key.size() - key_offset);

            return std::make_pair(
                (iterator)(insert_at(current, subkey, value)->value_head),
                true);
        }

        // scan for substring mismatch with label, note scan begins at
        // second character as first character was redundantly encoded
        // and checked during child choice for the loop's examination
        bool matches_label = true;
        bool initial_match_label_exceeds_key = false;
        binary_type::size_type label_offset = 1;

        for (;
            (label_offset < initial_match->label.size());
            label_offset++)
        {
            if ((key_offset + label_offset) >= key.size())
            {
                initial_match_label_exceeds_key = true;
                break;
            }

            if (initial_match->label[label_offset] != key[key_offset + label_offset])
            {
                matches_label = false;
                break;
            }
        }

        if (!matches_label || initial_match_label_exceeds_key ||
            (label_offset != initial_match->label.size()))
        {
            // if there is a disagreement, introduce intermediary node
            // and insert the new branch
            binary_type intermediary_key = initial_match->label.get_substring(
                0, label_offset);

            binary_type trailing_initial_key = initial_match->label.get_substring(
                label_offset);

            // unlink/remove the initial_match from the tree
            unlink_node(initial_match);
            initial_match->parent = nullptr;

            // add intermediary to tree
            trie_node_type* intermediary = insert_at(current, intermediary_key);

            // add back initial_match with reduced label
            initial_match->label = trailing_initial_key;
            attach_child(intermediary, initial_match);

            if (key.size() > (key_offset + label_offset))
            {
                // if the key has remaining length, insert a sibling
                binary_type remaining_key = key.get_substring(
                    key_offset + label_offset);

                return std::make_pair(
                    (iterator)(insert_at(intermediary, remaining_key, value)->value_head),
                    true);
            }
            else
            {
                // otherwise intermediary label must be key, so add value to 
                // the intermediary which was uniquely added and link_node
                append_value(intermediary, value);
                link_node(intermediary);

                current = intermediary;
                return std::make_pair((iterator)current->value_head, true);
            }
        }
        else
        {
            // otherwise, loop decending the tree
            current = initial_match;
            key_offset += label_offset;
        }
    }

    return std::make_pair((iterator)current, false);
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::pair_iterator_bool
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::insert_equal(
    const binary_type& key, const value_type& value)
{
    trie_node_type* current = root_;
    auto result = insert(current, key, value);

    // if we haven't created a node yet, but exhausted our key
    // we must have matched an existing node, we can append here
    // though we aren't guaranteed to be unique
    if (!result.second && (current != root_))
    {
        bool linked = current->has_value();

        append_value(current, value);

        if (!linked)
        {
            link_node(current);
        }

        return std::make_pair((iterator)current, true);
    }

    return result;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::pair_iterator_bool
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::insert_unique(
    const binary_type& key, const value_type& value)
{
    trie_node_type* current = root_;
    auto result = insert(current, key, value);

    // if we haven't created a node yet, but exhausted our key
    // we must have matched an existing node, if it has no value
    // we can introduce one here
    if (!result.second && !current->has_value() && (current != root_))
    {
        append_value(current, value);

        link_node(current);

        return std::make_pair((iterator)current, true);
    }

    return result;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::pair_node_size 
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::find_closest_subkey_matching_node(
    trie_node_type* start, const binary_type& key)
{
    trie_node_type* current = start;
    binary_type::size_type key_offset = 0;
    bool match = true;

    for (; match && (current != nullptr) && (key_offset < key.size());)
    {
        bool first_bit = key[key_offset];
        int child_index = first_bit ? 1 : 0;

        trie_node_type* initial_match = current->children[child_index];

        if (initial_match != nullptr)
        {
            for (binary_type::size_type label_offset = 0;
                (label_offset < initial_match->label.size()) &&
                    (key_offset + label_offset < key.size());
                label_offset++)
            {
                if (key[key_offset + label_offset] != initial_match->label[label_offset])
                {
                    match = false;
                    break;
                }
            }

            key_offset += initial_match->label.size();
        }

        current = initial_match;
    }

    if (!match)
    {
        current = nullptr;
    }

    return std::make_pair(current, key_offset);
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::iterator_range
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::find_equal(
    const binary_type& key)
{
    auto find_pair = find_closest_subkey_matching_node(root_, key);

    if ((find_pair.first == nullptr) ||
        (find_pair.second != key.size()) || (find_pair.second == 0))
    {
        return std::make_pair(end(), end());
    }

    iterator begin = (iterator)(find_pair.first->value_head);
    iterator end = (iterator)(find_pair.first->value_tail);
    ++end;

    return std::make_pair(begin, end);
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::iterator_range
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::find_prefix(
    const binary_type& key)
{
    auto find_pair = find_closest_subkey_matching_node(root_, key);

    if ((find_pair.first == nullptr) || (find_pair.second == 0))
    {
        return std::make_pair(end(), end());
    }

    iterator begin = (find_pair.first->value_leftmost);
    iterator end = (find_pair.first->value_rightmost);
    ++end;

    return std::make_pair(begin, end);
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
bool binary_trie<Value, NodeAlloc, ValueNodeAlloc>::remove_equal(
    const binary_type& key)
{
    auto find_pair = find_closest_subkey_matching_node(root_, key);

    bool nonremovable = ((find_pair.first == nullptr)
        || (find_pair.second != key.size()) || (find_pair.second == 0));

    if (!nonremovable)
    {
        trie_node_type* node = find_pair.first;

        erase_values(node);
        compress_branch(node);
    }

    return !nonremovable;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
bool binary_trie<Value, NodeAlloc, ValueNodeAlloc>::remove_prefix(
    const binary_type& key)
{
    auto find_pair = find_closest_subkey_matching_node(root_, key);

    bool nonremovable = ((find_pair.first == nullptr)
        || (find_pair.second < key.size()) || (find_pair.second == 0));

    if (!nonremovable)
    {
        trie_node_type* node = find_pair.first;
        trie_node_type* parent = node->parent;

        erase_subtree(node);
        compress_branch(parent);
    }

    return !nonremovable;
}

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
typename binary_trie<Value, NodeAlloc, ValueNodeAlloc>::iterator
    binary_trie<Value, NodeAlloc, ValueNodeAlloc>::remove_value(
    iterator it)
{
    value_node_type* value_node = it.value_node_;

    if (value_node == nullptr)
    {
        return it;
    }

    trie_node_type* anchor = value_node->anchor;

    if (anchor->value_head != anchor->value_tail)
    {
        // if the value can be removed without removing emptying the node
        auto next = value_node->next;
        auto previous = value_node->previous;

        if (next != nullptr)
        {
            next->previous = previous;
        }

        if (previous != nullptr)
        {
            previous->next = next;
        }

        value_node->next = nullptr;
        value_node->previous = nullptr;

        bool update = false;

        if (anchor->value_head == value_node)
        {
            anchor->value_head = next;
            update = true;
        }

        if (anchor->value_tail == value_node)
        {
            anchor->value_tail = previous;
            update = true;
        }

        destroy_value_node(value_node);

        if (update)
        {
            update_left_and_right_branch(anchor);
        }

        if (next != nullptr)
        {
            return (iterator)(next);
        }
        else
        {
            return (iterator)(anchor->next);
        }
    }
    else
    {
        auto next = anchor->next;
        // otherwise, remove all values and attempt to remove the node
        erase_values(anchor);
        compress_branch(anchor);

        return (iterator)(next);
    }
}

}
}

#endif
