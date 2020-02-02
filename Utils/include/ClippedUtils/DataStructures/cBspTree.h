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

#include <stdlib.h>

namespace Clipped
{
    // Forward declarations:
    template <typename T, typename U, typename V>
    struct BspLeaf;
    template <typename T, typename U, typename V>
    struct BspNode;
    template <typename T, typename U, typename V>
    struct BspBase;

    /**
     * @brief BspTree contains the complete BspTree.
     */
    template <typename T, typename U, typename V>
    struct BspTree
    {
        BspBase<T, U, V> root;     //!< The root of this BspTree.
        BspNode<T, U, V>* nodes;   //!< Nodes of this tree.
        size_t numNodes;           //!< Number of nodes.
        BspLeaf<T, U, V>* leafes;  //!< Leafes of this tree.
        size_t numLeafes;          //!< Number of leafes.
    };

    /**
     * @brief The NodeType enum possible node types.
     */
    enum class NodeType
    {
        Node = 0,
        Leaf
    };

    template <typename T, typename U, typename V>
    struct BspBase
    {
        T baseData;  //!< Data stored in the BspTree Base.

        BspBase<T, U, V>* getRoot();

        BspLeaf<T, U, V>* getLeaf(const V& search);
    };

    /**
     * @brief The BspNode contains data and has further nodes.
     */
    template <typename T, typename U, typename V>
    struct BspNode : public BspBase<T, U, V>
    {
        BspBase<T, U, V>* left;   //!< The left subnode.
        BspBase<T, U, V>* right;  //!< The right subnode.
        U nodeData;               //!< Data stored in a node
    };

    /**
     * @brief The BspLeaf contains data but does not have any deeper nodes.
     */
    template <typename T, typename U, typename V>
    struct BspLeaf : BspBase<T, U, V>
    {
        V leafData;  //!< Data stored in the leaf
    };
}  // namespace Clipped
