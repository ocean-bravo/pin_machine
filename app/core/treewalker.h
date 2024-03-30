#pragma once

#include <QObject>

#include <functional>

template<typename T>
struct TreeWalkerTrait
{
    static QList<T> children(T item)
    {
        Q_UNUSED(item);
        return QList<T>();
    }
};

/**
 * Performs a DFS walk through the hierarchy of T
 *
 * @note Specialize TreeWalkerTrait for your type T to get TreeWalker support
 */
template<typename T>
class TreeWalker
{
public:
    enum VisitResult
    {
        ContinueWalk, ///< Continues traversal with the next sibling of the item just visited, without visiting its children
        RecursiveWalk, ///< Traverse the children of this item
        StopWalk ///< Terminate the traversal
    };

    enum TraversalType
    {
        PreOrderTraversal,
        PostOrderTraversal,
    };

    typedef std::function<VisitResult(T)> VisitFunction;

    explicit TreeWalker(TraversalType type = PreOrderTraversal)
        : m_traversalType(type)
    {
    }

    /**
     * Walk through all items including the start item itself
     *
     * @return True in case we walked through all items, false otherwise
     *
     * @param item the start item
     * @param visit Function called each time a LayoutItem instance is encountered
     */
    bool walkItems(T item, const VisitFunction &visit)
    {
        if (!item)
            return false;

        bool continueWalk = true;
        if (m_traversalType == PreOrderTraversal) {
            continueWalk = (visit(item) == TreeWalker::RecursiveWalk);
        }
        Q_ASSERT(item);
        foreach (T child, TreeWalkerTrait<T>::children(item)) {
            if (!walkItems(child, visit))
                return false;
        }
        if (m_traversalType == PostOrderTraversal) {
            continueWalk = (visit(item) == TreeWalker::RecursiveWalk);
        }
        return continueWalk;
    }

    /**
     * Convenience function. Same as walk(), but omits item @p item
     *
     * @sa walk()
     */
    bool walkChildren(T item, const VisitFunction &visit)
    {
        if (!item)
            return false;

        foreach (T child, TreeWalkerTrait<T>::children(item)) {
            if (!walkItems(child, visit))
                return false;
        }
        return true;
    }

private:
    TraversalType m_traversalType;
};
