// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <deque>
#include <utility>

#include "Eecs281PQ.hpp"

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        // Description: Custom constructor that creates a node containing
        //              the given value.
        explicit Node(const TYPE &val)
            : elt { val } {}

        // Description: Allows access to the element at that Node's position.
        //              There are two versions, getElt() and a dereference
        //              operator, use whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child = nullptr;
        Node *sibling = nullptr;
        Node *pre = nullptr;
        // TODO: Add and initialize one extra pointer (parent or previous) as desired.
    };  // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp } {
        // TODO: Implement this function.
        root = nullptr;
        numNodes = 0;
    }  // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp } {
        // TODO: Implement this function.
        root = nullptr;
        numNodes = 0;
        for (InputIterator it = start; it != end; ++it) {
            push(*it);
        }
    }  // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other)
        : BaseClass { other.compare } {
        // TODO: Implement this function.
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
        root = nullptr;
        numNodes = 0;
        copyPairingPQ(other.root);
    }  // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        if (this != &rhs) {
            // Create a temporary copy of rhs
            PairingPQ temp(rhs);
            
            // Swap this object's contents with the temporary copy
            std::swap(root, temp.root);
            std::swap(numNodes, temp.numNodes);
            std::swap(this->compare, temp.compare);
        }
        return *this;
    }  // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        deletePairingPQ(root);
    }  // ~PairingPQ()


    // Description: Move constructor and assignment operators don't need any
    //              code, the members will be reused automatically.
    PairingPQ(PairingPQ &&) noexcept = default;
    PairingPQ &operator=(PairingPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        if (root == nullptr) return;
        collectAllNode();
    }  // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) { addNode(val); }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        // if (empty()) {
        //     throw std::underflow_error("Priority queue is empty");
        // }
        Node* oldRoot = root;
        Node* child = root->child;
        if (child == nullptr) {
            root = nullptr;
        } else {
            // collect the first layer, chindren of root
            std::deque<Node*> candidates;
            while (child != nullptr) {
                Node* sibling = child->sibling;
                child->sibling = nullptr;
                child->pre = nullptr;
                candidates.push_back(child);
                child = sibling;
            }

            // melt all chindren to a root
            meldRoot(candidates);
            root = candidates.front();
        }
        --numNodes;
        delete oldRoot;
    }  // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function
        if (empty())
        {
            throw std::underflow_error("Priority queue is empty");
        }
        return root->elt;
    }  // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        // TODO: Implement this function
        return numNodes;
    }  // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        // TODO: Implement this function
        return root == nullptr;
    }  // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node *node, const TYPE &new_value) {
        // TODO: Implement this function
        node->elt = new_value;
        if (node == root) return;
        
        if (node->sibling != nullptr)
        {
            node->sibling->pre = node->pre;
        }

        if (node->pre->child != node)
        {
            node->pre->sibling = node->sibling;
        } else {
            node->pre->child = node->sibling;
        }

        node->sibling = nullptr;
        node->pre = nullptr;
    
        root = meld(root, node);
    }  // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node *addNode(const TYPE &val) {
        // TODO: Implement this function
        Node *newNode = new Node(val);
        root = meld(root, newNode);
        ++numNodes;
        return newNode;
    }  // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).
    Node *root;
    std::size_t numNodes;
    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
    
    Node *meld(Node *a, Node *b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;

        if (this->compare(a->elt, b->elt)) {
            // b has higher priority, so b becomes the root
            a->sibling = b->child;
            b->child = a;
            a->pre = b;
            if (a->sibling != nullptr)
            {
                a->sibling->pre = a;
            }
            
            return b;
        } else {
            // a has higher priority, so a becomes the root
            b->sibling = a->child;
            a->child = b;
            b->pre = a;
            if (b->sibling != nullptr)
            {
                b->sibling->pre = b;
            }
            
            return a;
        }
    }

    void copyPairingPQ(Node *otherNode) {
        if (otherNode == nullptr) return;

        push(otherNode->elt);
        if (otherNode->child != nullptr) copyPairingPQ(otherNode->child);
        if (otherNode->sibling != nullptr) copyPairingPQ(otherNode->sibling);
    }

    // void deletePairingPQ(Node *node) {
    //     if (node == nullptr) return;

    //     deletePairingPQ(node->child);
    //     deletePairingPQ(node->sibling);
    //     delete node;
    // }

    void deletePairingPQ(Node* node) {
        if (node == nullptr) return;

        std::deque<Node*> nodedeque;
        nodedeque.push_back(node);

        while (!nodedeque.empty()) {
            Node* current = nodedeque.back();
            nodedeque.pop_back();
            if (current->child) {
                nodedeque.push_back(current->child);
            }
            if (current->sibling) {
                nodedeque.push_back(current->sibling);
            }
            delete current;
        }
    }



    void collectAllNode() {
        std::deque<Node*> collectionQueue;
        collectionQueue.push_back(root);
        root = nullptr;

        while (!collectionQueue.empty())
        {
            Node* current = collectionQueue.back();
            collectionQueue.pop_back();
            if (current->child)
            {
                collectionQueue.push_back(current->child);
            }
            if (current->sibling)
            {
                collectionQueue.push_back(current->sibling);
            }

            current->sibling = nullptr;
            current->pre = nullptr;
            current->child = nullptr;
            
            root = meld(root, current);
        }
        
    }

    void meldRoot(std::deque<Node*>& Queue) {
        while (Queue.size() > 1) {
            Node* first = Queue.front(); Queue.pop_front();
            Node* second = Queue.front(); Queue.pop_front();
            Queue.push_back(meld(first, second));
        }
    }
};


#endif  // PAIRINGPQ_H
