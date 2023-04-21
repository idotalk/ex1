#ifndef NODE_H
#define NODE_H

//Struct declaration - doubly linked list
typedef struct Node{
    int m_remainedFriends;
    int m_reminedRivals;
    void* m_data;
    struct Node* m_prevNode;
    struct Node* m_nextNode;
}* Node;

/**@param item: an item to for the node
 * @param  rivalQuota: maximum amount of rivals the node can block from getting ahead in th queue
 * @param  friendQuota: maximum amount of friends the node can bring into the queue
 * 
 * Creates a new Node object with provided item and friends + rivals limits.
 * In case of failure, return NULL.*/
Node createNode(void*, int, int);

/**Connect a Node object after an already placed Node.
 * In case of Null parameter does nothing.*/
void connectNodes(Node , Node);

/**@param head: the first node in the queue
 *
 * Deallocates all the nodes at the queue
 * 
 * Notice: in case of not passing the head of the queue an error message will be printed - no deallocation in this case!*/
void destroyFromStart(Node head);



#endif // NODE_H 