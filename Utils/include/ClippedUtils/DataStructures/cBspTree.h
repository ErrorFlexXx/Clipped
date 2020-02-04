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
