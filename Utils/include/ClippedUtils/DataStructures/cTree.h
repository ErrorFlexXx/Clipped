/* Copyright 2019 Christian Löpke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    public:
        /**
         * @brief Tree creates a Tree identified by ident.
         * @param ident
         */
        Tree() {}

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
        const T& getElement(const I& key) { return elements[key]; }

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
         * @brief subtreeExist checks if a subtree specified by I exists in this tree layer.
         * @param key of the subtree.
         * @return true if it exists, false otherwise.
         */
        bool subtreeExist(const I& key)
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

    private:
        std::map<I, Tree<I, T>> childs;  //!< subtrees identified by key of type I.
        std::map<I, T> elements;         //!< Key, elements map.
    };
}  // namespace Utils
