#ifndef NODE_H
#define NODE_H

//Struct decleratoion - doubly linked list
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

/**Return a new Node object after connecting it to the end of the queue.
 * @see createNode() For more information about parameters to this function.
 * In case of failure, return NULL.*/
Node connectNode(Node, void*, int, int);

/**@param head: the first node in the queue
 *
 * Deallocates all the nodes at the queue
 * 
 * Notice: in case of not passing the head of the queue an error messege will be printed - no deallocation in this case!*/
void destroyFromStart(Node head);



#endif // NODE_H 