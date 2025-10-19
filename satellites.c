// POPOVICI ANDREI-RAZVAN 314CC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct treeNode
{
    int Frequency;
    char *Name;
    struct treeNode *lp, *rp;
};
struct heap
{
    int heapSize;
    struct treeNode **heapHead;
};

struct queue
{
    struct treeNode *value;
    struct queue *next;
    int level;
};

void enqueue(struct queue **head, struct treeNode *value, int level)
{ // function loads a new node into the queue
    struct queue *q = malloc(sizeof(struct queue));
    q->value = value;
    q->level = level;
    q->next = NULL;
    if (*head == NULL)
    {
        *head = q;
        return;
    }
    struct queue *ptr = *head;
    while (ptr->next != NULL)
        ptr = ptr->next;
    ptr->next = q;
    return;
}

struct treeNode *dequeue(struct queue **head, int *level)
{ // function removes an element from the queue and returns a pointer to the removed node,
  // also returning the node's level through the int *level reference parameter
    struct queue *q = *head;
    *head = (*head)->next;
    struct treeNode *retValue = q->value;
    *level = q->level;
    free(q);
    return retValue;
}

struct treeNode *createTreeNode(int Frequency, char *Name)
{
    // function creates a node of type struct treeNode*, with a given name and frequency,
    // and returns a pointer to this node
    struct treeNode *q = malloc(sizeof(struct treeNode));
    q->Frequency = Frequency;
    q->Name = malloc(sizeof(char) * (strlen(Name) + 1));
    strcpy(q->Name, Name);
    q->lp = NULL;
    q->rp = NULL;
    return q;
}

int heapParent(int currentIndex)
{
    return (currentIndex - 1) / 2;
}

void swapElements(struct heap heap, int index1, int index2)
{
    // swaps two elements of type struct treeNode
    if (index1 > heap.heapSize || index2 > heap.heapSize)
        return; // invalid index!

    struct treeNode *aux;
    aux = heap.heapHead[index1];
    heap.heapHead[index1] = heap.heapHead[index2];
    heap.heapHead[index2] = aux;
}

int minNode(struct heap heap, int index1, int index2)
{
    if (index1 > heap.heapSize || index2 > heap.heapSize)
        return -1; // invalid index!
    if (heap.heapHead[index1]->Frequency > heap.heapHead[index2]->Frequency)
        return index2;
    else if (heap.heapHead[index1]->Frequency < heap.heapHead[index2]->Frequency)
        return index1;
    if (strcmp(heap.heapHead[index1]->Name, heap.heapHead[index2]->Name) > 0)
        return index2;
    else
        return index1;
}

void verifyHeap(struct heap heap, int currentIndex)
{
    int parentIndex = heapParent(currentIndex);
    while (minNode(heap, currentIndex, parentIndex) == currentIndex && parentIndex != currentIndex)
    { // check if upward filtering is needed
        swapElements(heap, currentIndex, parentIndex);
        currentIndex = parentIndex;
        parentIndex = heapParent(currentIndex);
    }
}

struct treeNode *removeNode(struct heap *heap)
{
    struct treeNode *returnNode = heap->heapHead[0];
    heap->heapHead[0] = heap->heapHead[--heap->heapSize]; // moved the last inserted element to the first heap position
    int currentIndex = 0, minimumIndex, decisionIndex;
    while (currentIndex * 2 + 1 < heap->heapSize)
    {
        minimumIndex = minNode(*heap, currentIndex * 2 + 1, currentIndex * 2 + 2); // find the smaller child
        decisionIndex = minNode(*heap, currentIndex, minimumIndex);                // check if the smaller child is smaller than the current node
        if (decisionIndex == currentIndex)
        {
            break;
        }
        swapElements(*heap, decisionIndex, currentIndex); // swap current node with the smaller child
        currentIndex = decisionIndex;
    }
    return returnNode; // returns the removed node for further use
}

struct treeNode *createTree(struct heap *heap)
{
    char *concName;
    int concFreq;
    struct treeNode *aux1, *aux2, *aux3;
    while (heap->heapSize != 1)
    {
        aux1 = removeNode(heap);
        aux2 = removeNode(heap); // take the two nodes to be merged
        if ((aux1->Frequency == aux2->Frequency && strcmp(aux1->Name, aux2->Name) > 0) || aux1->Frequency > aux2->Frequency)
        { // determine which node appears first in the encoding,
          // so the first node is the one with the smaller value
            aux3 = aux1;
            aux1 = aux2;
            aux2 = aux3;
        }
        concFreq = aux1->Frequency + aux2->Frequency;
        concName = malloc(strlen(aux1->Name) + strlen(aux2->Name) + 1);
        strcpy(concName, aux1->Name);
        strcat(concName, aux2->Name); // compute the new attributes

        struct treeNode *newNode = createTreeNode(concFreq, concName);
        newNode->lp = aux1;
        newNode->rp = aux2;
        heap->heapHead[heap->heapSize++] = newNode;
        verifyHeap(*heap, heap->heapSize - 1); // ensure the inserted node is properly positioned in the heap via upward filtering
        free(concName);
    }
    return heap->heapHead[0];
}

void bfsPrint(struct treeNode *root, char *outFilePath) // print requested in task 1
{
    struct queue *q = NULL;
    enqueue(&q, root, 0);
    // variable 'level' will be used to determine when to print "\n",
    // representing the level of a node in the tree,
    // the root having value 0, its children 1, etc.
    int level = 0;

    FILE *fPtrOut = fopen(outFilePath, "w");

    while (q != NULL)
    {
        struct treeNode *elem = dequeue(&q, &level);
        if (elem->lp != NULL)
        {
            enqueue(&q, elem->lp, level + 1);
        }
        if (elem->rp != NULL)
        {
            enqueue(&q, elem->rp, level + 1);
        }
        fprintf(fPtrOut, "%d-%s ", elem->Frequency, elem->Name);

        if (q != NULL && level != q->level)
            fprintf(fPtrOut, "\n");
    }
    fprintf(fPtrOut, "\n");

    fclose(fPtrOut);
}

struct treeNode *createSateliteNetwork(FILE *fPtrIn)
{
    char Name[16];
    int numberOfNodes, Frequency;

    fscanf(fPtrIn, "%d", &numberOfNodes); // read number of nodes

    struct heap heap;
    heap.heapSize = numberOfNodes; // number of nodes in heap
    heap.heapHead = malloc(numberOfNodes * sizeof(struct treeNode *));

    for (int i = 0; i < numberOfNodes; i++)
    {
        fscanf(fPtrIn, "%d %s", &Frequency, Name); // read Name/Frequency
        struct treeNode *Node = createTreeNode(Frequency, Name);

        heap.heapHead[i] = Node; // add new node to heap

        verifyHeap(heap, i); // "sort" heap
    }

    struct treeNode *root = createTree(&heap);

    free(heap.heapHead);

    return root;
}

int parcurgeCodificare(struct treeNode *root, char *codif, int i, FILE *fPtrOut)
{
    while (1)
    {
        if (codif[i] == '1')
        {
            if (root->rp != NULL)
            { // if current encoding step = '1' and there is a right child
                root = root->rp;
                i++;
            } // used two ifs to allow this "else break"
            else
                break;
        }
        if (codif[i] == '0')
        {
            if (root->lp != NULL)
            { // if current encoding step = '0' and there is a left child
                root = root->lp;
                i++;
            }
            else // used two ifs to allow this "else break"
                break;
        }
        if (codif[i] != '0' && codif[i] != '1')
            break; // if encountering unwanted characters
    }
    if (root->rp == NULL && root->lp == NULL)
        fprintf(fPtrOut, "%s ", root->Name); // check if node is a leaf
    return i;
}

void findSatelites(FILE *fPtrIn, struct treeNode *root, char *outFilePath)
{
    int nrNodes, strIndex = 0;
    fscanf(fPtrIn, "%d", &nrNodes);
    char codif[1000];

    FILE *fPtrOut = fopen(outFilePath, "w");

    for (int i = 0; i < nrNodes; i++)
    {
        fscanf(fPtrIn, "%s", codif);
        // read an encoding from the source file
        while (codif[strIndex] != '\0')
        {
            strIndex = parcurgeCodificare(root, codif, strIndex, fPtrOut);
            // search nodes until the current encoding ends
        }
        fprintf(fPtrOut, "\n");
        strIndex = 0;
    }
    fclose(fPtrOut);
}

int findCodif(struct treeNode *root, char *Node, char *cod, int index)
{
    if (root == NULL)
        return 0; // base case

    if (strcmp(root->Name, Node) == 0)
        return 1; // base case

    cod[index] = '0'; // add left-branch encoding, then check if valid
    if (findCodif(root->lp, Node, cod, index + 1))
        return 1;
    cod[index] = 0; // remove recently added encoding if node not found on left branch

    cod[index] = '1'; // add right-branch encoding, then check if valid
    if (findCodif(root->rp, Node, cod, index + 1))
        return 1;
    cod[index] = 0; // remove recently added encoding if node not found on right branch

    return 0;
}

void findCodifSatelites(FILE *fPtrIn, struct treeNode *root, char *outFilePath)
{
    int nrNodes;
    char Node[16];
    fscanf(fPtrIn, "%d", &nrNodes);

    FILE *fPtrOut = fopen(outFilePath, "w");

    for (int i = 0; i < nrNodes; i++)
    {
        char codif[1000] = {0};
        fscanf(fPtrIn, "%s", Node); // read the name of the node whose encoding we seek
        findCodif(root, Node, codif, 0);
        fprintf(fPtrOut, "%s", codif); // print found encoding
    }
    fprintf(fPtrOut, "\n");
    fclose(fPtrOut);
}

struct treeNode *findCommonNode(struct treeNode *root, char **Nodes, int nrNodes)
{

    char codif[1000];
    int checks = 0;

    if (root == NULL)
        return NULL;

    struct treeNode *retValue = NULL;
    retValue = findCommonNode(root->lp, Nodes, nrNodes);
    // try to find the common node on the left side
    if (retValue != NULL)
    {
        return retValue;
    }
    retValue = findCommonNode(root->rp, Nodes, nrNodes);
    // try to find the common node on the right side
    if (retValue != NULL)
    {
        return retValue;
    }

    for (int i = 0; i < nrNodes; i++)
        if (findCodif(root, Nodes[i], codif, 0))
            checks++;

    // check if the current node is the common node
    // also, placing the if-statement at the end guarantees that
    // the common node will be the lowest common ancestor
    if (checks == nrNodes)
        return root;

    return NULL;
}

void findParentSatelite(FILE *fPtrIn, struct treeNode *root, char *outFilePath)
{
    FILE *fPtrOut = fopen(outFilePath, "w");
    int nrNodes;
    fscanf(fPtrIn, "%d", &nrNodes);
    char **Nodes = malloc(sizeof(char *) * nrNodes);
    for (int i = 0; i < nrNodes; i++)
    {
        Nodes[i] = malloc(16);
        fscanf(fPtrIn, "%s", Nodes[i]); // read node names
    }
    struct treeNode *foundNode = findCommonNode(root, Nodes, nrNodes);
    fprintf(fPtrOut, "%s", foundNode->Name);
    fclose(fPtrOut);
    for (int i = 0; i < nrNodes; i++)
    {
        free(Nodes[i]);
    }
    free(Nodes);
}

void freefunction(struct treeNode *root)
{ // function to free allocated memory
    if (root->lp != NULL)
        freefunction(root->lp);
    if (root->rp != NULL)
        freefunction(root->rp);
    free(root->Name);
    free(root);
}

void readInput(char **argv)
{ // function to interpret the first argument and open the input file
    char argTask[4];
    FILE *fPtrIn = fopen(argv[2], "r");
    strcpy(argTask, argv[1]); // read desired task
    struct treeNode *root = createSateliteNetwork(fPtrIn);
    int task = argTask[2] - 48;
    switch (task)
    {
    case 1:
    {
        bfsPrint(root, argv[3]);
        break;
    }
    case 2:
    {
        findSatelites(fPtrIn, root, argv[3]);
        break;
    }
    case 3:
    {
        findCodifSatelites(fPtrIn, root, argv[3]);
        break;
    }
    case 4:
    {
        findParentSatelite(fPtrIn, root, argv[3]);
        break;
    }
    default:
    {
        printf("Invalid task!\n");
        fclose(fPtrIn);
        exit(0);
    }
    }
    freefunction(root);
    fclose(fPtrIn); // close input file
}

int main(int argc, char **argv)
{
    if (argc == 4)
        // check if the number of arguments is valid
        readInput(argv);
    else
    {
        printf("Invalid number of arguments!\nProgram usage: $ ./satelite -c[n] input_file output_file\n");
        exit(0);
    }
}
