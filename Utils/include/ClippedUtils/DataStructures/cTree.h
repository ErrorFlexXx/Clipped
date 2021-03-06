/*
** Clipped -- a Multipurpose C++ Library.
**
** Copyright (C) 2019-2020 Christian Löpke. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#pragma once

#include <functional>
#include <map>
#include <ClippedUtils/cLogger.h>

namespace Clipped
{
    template <class I, class T>
    /**
     * @brief The Tree class template class to implement a tree data structure.
     *   template param I as key, T as payload.
     *   Element: payload in current tree layer.
     *   subtree: Next Tree stage possibly containing subtrees and elements.
     */
    class Tree
    {
        class TreeIterator : public std::iterator<std::output_iterator_tag, T>
        {
        public:
            explicit TreeIterator(Tree<I, T>& container, size_t index = 0)
                : container(container)
                , index(index)
            {}

            std::pair<const I, T>& operator*()
            {
                if(index < container.countLocalElements())
                {
                    auto iterator = container.elements.begin();
                    for(size_t i = 0; i < index; i++) iterator++;
                    return *iterator;
                }
                size_t childLookupIndex = index - container.countLocalElements();
                for(auto& child : container.childs)
                {
                    if(childLookupIndex < child.second.countElements())
                        return *(child.second.begin() + childLookupIndex);
                    else
                        childLookupIndex -= child.second.countElements();
                }
				//Should never happen.
				LogError() << "Iterator end() overrun detected!";
				auto iterator = container.elements.begin(); //Fallback return first element.
				return *iterator;
            }

            TreeIterator& operator++()
            {
                index++;
                return *this;
            }

            TreeIterator& operator+(const size_t rhs)
            {
                index += rhs;
                return *this;
            }

            bool operator !=(const TreeIterator& rhs)const
            {
                return rhs.index != index;
            }

        private:
            Tree<I, T>& container;
            size_t index;
        };

    public:
        /**
         * @brief Tree creates a Tree identified by ident.
         * @param ident
         */
        Tree() {}

        TreeIterator begin()
        {
            return Tree<I, T>::TreeIterator(*this, 0);
        }

        TreeIterator end()
        {
            return Tree<I, T>::TreeIterator(*this, countElements());
        }

        /**
         * @brief elementExist checks if an element specified by key exists in this tree layer.
         * @param key to lookup.
         * @return true if exists, false otherwise.
         */
        bool elementExist(const I& key)
        {
            auto result = elements.find(key);
            return (result != elements.end());
        }

        /**
         * @brief getElement returns a reference to the element specified by I.
         *   Note, this will create a new element, if key doesn't exist!
         *   Check with elementExists before, if you don't want that.
         * @param key to lookup
         * @return reference to element specified by key.
         */
        T& getElement(const I& key) { return elements[key]; }

        /**
         * @brief getElements gets a complete map of all elements of this tree layer.
         * @return const reference of elements.
         */
        const std::map<I, T>& getElements() { return elements; }

        /**
         * @brief addElement adds a new element to this tree layer, specified by key.
         * @param key of the new element.
         * @param element to store.
         * @return true, if new element was added. False if not (e.g. if it already exists).
         */
        bool addElement(const I& key, const T& element)
        {
            if (elementExist(key)) return false;  // Element already exists.
            elements[key] = element;
            return true;
        }

        /**
         * @brief removeElement removes an element from the current tree layer.
         * @param key of the element.
         */
        void removeElement(const I& key)
        {
            auto it = elements.find(key);
            if (it != elements.end()) elements.erase(it);
        }

        /**
         * @brief removeElement removes element with key from the tree.
         *   Searches recursively for the given element.
         * @param key to delete
         * @param element to delete
         * @return true, if successfully deleted.
         */
        bool removeElement(const I& key, const T* element)
        {
            if(elementExist(key)) //Look for element locally
            {
                if(&getElement(key) == element)
                {
                    removeElement(key);
                    return true;
                }
            }
            else //Lookup child trees
            {
                for(auto & child : childs)
                {
                    if(child.second.removeElement(key, element))
                        return true; //Removed successfully. Stop searching.
                }
            }
            return false; //Not found locally and not in child trees.
        }

        /**
         * @brief subtreeExist checks if a subtree specified by I exists in this tree layer.
         * @param key of the subtree.
         * @return true if it exists, false otherwise.
         */
        bool subtreeExist(const I& key) const
        {
            auto result = childs.find(key);
            return (result != childs.end());
        }

        /**
         * @brief getSubtree returns the subtree specified by key.
         *   Note: A new subtree will be created if it doesn't exists already.
         *   Call subtreeExists before, if you don't want that.
         * @param key of the subtree.
         * @return reference to subtree specified by key.
         */
        Tree<I, T>& getSubtree(const I& key) { return childs[key]; }

        /**
         * @brief getSubtrees gets a complete map of all subtrees of this tree layer.
         * @return reference to subtree map.
         */
        std::map<I, Tree<I, T>>& getSubtrees() { return childs; }

        /**
         * @brief addSubtree adds a subtree, if it doesn't exists already.
         * @param key of the new subtree.
         * @return true if a subtree has been added, false otherwise (e.g. if it existed already).
         */
        Tree<I, T>& addSubtree(const I& key)
        {
            if (subtreeExist(key)) LogWarn() << "Subtree already exists!";
            return childs[key];
        }

        /**
         * @brief removeSubtree removes a subtree from this layer.
         * @param key of the subtree.
         */
        void removeSubtree(const I& key)
        {
            auto it = childs.find(key);
            if (it != childs.end()) childs.erase(it);
        }

        /**
         * @brief countSubtrees counts all subtrees in this tree.
         * @return amount of child trees.
         */
        size_t countSubtrees() const
        {
            size_t count = childs.size();
            for(const auto& child : childs)
                count += child.second.countSubtrees();
            return count;
        }

        /**
         * @brief countSubtrees counts all subtrees in this tree stage.
         * @return amount of child trees in this stage.
         */
        size_t countLocalSubtrees() const
        {
            size_t count = childs.size();
            return count;
        }

        /**
         * @brief countElements counts all elements in this tree and subtrees.
         * @return amount of elements stored in the tree.
         */
        size_t countElements() const
        {
            size_t count = elements.size();
            for(const auto& child : childs)
                count += child.second.countElements();
            return count;
        }

        /**
         * @brief countLocalElements counts the elements in this stage of the tree.
         * @return the elements stored on this stage.
         */
        size_t countLocalElements() const
        {
            return elements.size();
        }

        /**
         * @brief countChildsAndElements counts all elements and child tree entries.
         * @return total amount of child tree entries and elements.
         */
        size_t countChildsAndElements() const
        {
            size_t count = elements.size();
            count += childs.size();
            for(const auto& child : childs)
                count += child.second.countChildsAndElements();
            return count;
        }

        /**
         * @brief removeEmptyChilds recursive cleanup of empty child trees.
         */
        void removeEmptyChilds()
        {
            for(auto child : childs) //Cleanup this stage.
            {
                if(child.second.countLocalElements() == 0 && child.second.countLocalSubtrees() == 0)
                    removeSubtree(child.first);
            }
            for(auto child : childs) //Cleanup childs.
                child.second.removeEmptyChilds();
        }

        std::map<I, Tree<I, T>> childs;  //!< subtrees identified by key of type I.
        std::map<I, T> elements;         //!< Key, elements map.
    };
}  // namespace Clipped
