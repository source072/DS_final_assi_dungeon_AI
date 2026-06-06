#ifndef STACK_H
#define STACK_H

// Linked-node stack. Used for movement history and undo.
template <typename T>
class Stack {
private:
    struct Node {
        T     value;
        Node* next;
        Node(const T& value) : value(value), next(nullptr) {}
    };

    Node* topNode;
    int   count;

public:
    Stack() : topNode(nullptr), count(0) {}
    ~Stack() { clear(); }

    Stack(const Stack&)            = delete;
    Stack& operator=(const Stack&) = delete;

    bool isEmpty() const { return count == 0; }
    int  size()    const { return count; }

    void push(const T& value) {
        Node* node = new Node(value);
        node->next = topNode;
        topNode    = node;
        ++count;
    }

    bool pop(T& output) {
        if (topNode == nullptr) return false;
        Node* old = topNode;
        output    = old->value;
        topNode   = old->next;
        delete old;
        --count;
        return true;
    }

    bool peek(T& output) const {
        if (topNode == nullptr) return false;
        output = topNode->value;
        return true;
    }

    void clear() {
        while (topNode != nullptr) {
            Node* old = topNode;
            topNode   = topNode->next;
            delete old;
        }
        count = 0;
    }
};

#endif
