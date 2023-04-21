#include <stdio.h>
#include <stdlib.h>
#include "Node.h"

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

void connectNodes(Node firstNode, Node secondNode){
    if(firstNode == NULL || secondNode == NULL){
        return;
    }
    Node temp = firstNode->m_nextNode;
    firstNode->m_nextNode = secondNode;
    secondNode->m_prevNode = firstNode;
    secondNode->m_nextNode = temp;
}

void destroyFromStart(Node head){
    if(head->m_prevNode != NULL){
        return;
    }
    while(head){
        Node delete = head;
        head = head->m_nextNode;
        free(delete);
    }
}



