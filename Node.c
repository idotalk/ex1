#include <stdio.h>


typedef struct Node{
    int m_remainedFriends;
    int m_reminedRivals;
    void* m_data;
    struct Node* m_prevNode;
    struct Node* m_nextNode;
}* Node;

Node createNode(void* item, int friendQuota, int rivalQuota){
    Node ptr = malloc(sizeof(*ptr));
    if(!ptr){
        return NULL;
    }
    ptr->m_remainedFriends = friendQuota;
    ptr->m_reminedRivals = rivalQuota;
    ptr->m_data = item;
    ptr->m_prevNode = NULL;
    ptr->m_prevNode = NULL;
    return ptr;
}

Node connectNode(Node node, void* item,int friendQuota, int rivalQuota){
    Node newNode = createNode(item, friendQuota, rivalQuota);
    if(!newNode){
        return NULL;
    }
    newNode->m_remainedFriends = friendQuota;
    newNode->m_reminedRivals = rivalQuota;
    node->m_nextNode = newNode;
    newNode->m_prevNode = node;
    newNode->m_nextNode = NULL;
    return newNode;
}

void destroyFromStart(Node head){
    if(head->m_nextNode != NULL){
        return;
    }
    while(head){
        Node delete = head;
        head = head->m_prevNode;
        free(delete);
    }
}



