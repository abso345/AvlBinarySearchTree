#include <stdio.h>
#include <stdlib.h>
#include <time.h> // time stuff
#include <utility> //swap
#include <algorithm> // min/max

/*************************************************************************
 * Compile method:
 * g++ -lrt -lstdc++ hw1.cc -o hw1.o
 *************************************************************************/

int MAX_RANGE = 10000000; // Maximum range of random used.

typedef struct AVLNODE AVLNODE;
typedef struct AVLNODE* PTR_AVLNODE;

struct AVLNODE
{
    int value;
    int keys;
    PTR_AVLNODE left;
    PTR_AVLNODE right;
    PTR_AVLNODE parent;
};

typedef struct _AVLTREE
{
    PTR_AVLNODE root;
    int keys;
} AVLTREE, *PTR_AVLTREE;

enum printOrder
{
    INORDER = 0,
    PREORDER = 1,
    POSTORDER = 2
};

PTR_AVLNODE avlInsert(PTR_AVLTREE tree, int key);
void avlBalance(PTR_AVLTREE tree, PTR_AVLNODE node);
PTR_AVLNODE avlSearch(PTR_AVLTREE tree, int key);


PTR_AVLNODE avlMinimum(PTR_AVLNODE node)
{
    PTR_AVLNODE min = node;
    while(min->left != 0)
    {
       min = min->left;
    }

    return min;
}


PTR_AVLNODE avlSuccessor(PTR_AVLNODE node)
{
    PTR_AVLNODE child = node;
    PTR_AVLNODE parent = node;

    if(node->right != 0)
    {
        return avlMinimum(node->right);
    }

    parent = child->parent;

    while(parent != 0 && child == parent->right)
    {
        child = parent;
        parent = parent->parent;
    }

    return parent;
}

// Prints elements of an array into a file.
void printAVLtreeElements(PTR_AVLNODE node, printOrder order, int all)
{
    if(!node)
        return;

    int keys = (all) ? node->keys + 1 : 1;
    PTR_AVLNODE printNode = 0;

    if(order != INORDER)
    {
        printf("Wrong order type\n");
        return;
    }

    if(keys == 1 && !all)
    {
        printNode = node;
        printf("Only one value:%d  parentVal:%d leftChildVal:%d rightChildVal:%d\n",
               printNode->value,
               printNode->parent ? printNode->parent->value : 0,
               printNode->left ? printNode->left->value : 0,
               printNode->right ? printNode->right->value : 0);
    }
    else
    {
        printNode = avlMinimum(node);

        for(int i = 0; i < keys; i++)
        {
            if(!printNode)
                return;

            printf("value:%d  parentVal:%d leftChildVal:%d rightChildVal:%d\n",
               printNode->value,
               printNode->parent ? printNode->parent->value : 0,
               printNode->left ? printNode->left->value : 0,
               printNode->right ? printNode->right->value : 0);

            printNode = avlSuccessor(printNode);
        }
    }
}

void generateRandomAVLtree(PTR_AVLTREE tree, int numElements)
{
    // Create the seed.
    srand(time(0));
    int key = 0;

    // Populate array with random numbers in the range of MAX_RANGE
    for(int i = 0; i < numElements; i++)
    {
        key = rand() % (MAX_RANGE);

        if(i % 1000 == 0)
            printf("Inserting the %dth item now\n", i);

        avlInsert(tree, key);
    }
}

PTR_AVLNODE avlMaximum(PTR_AVLNODE node)
{
    PTR_AVLNODE max = node;

    while(max->right != 0)
    {
       max = max->right;
    }

    return max;
}

int avlNodeHeight(PTR_AVLNODE node)
{
    if(!node)
        return 0;

    //Find the heigh of an avlNode, since this is an avl tree this should be
    // log n.
    int keys = (node->keys + 1) >> 1;
    int height = 1;

    while(keys)
    {
        if(keys > 0)
            height++;

        keys >>= 1;
    }

    return height;
}

PTR_AVLNODE avlInsert(PTR_AVLTREE tree, int key)
{
    PTR_AVLNODE newNode = (PTR_AVLNODE)calloc(1, sizeof(AVLNODE));
    PTR_AVLNODE place = 0;
    PTR_AVLNODE parent = 0;

    if(!newNode)
        return 0;

    //Initialize new node.
    newNode->value = key;
    newNode->left = 0;
    newNode->right = 0;
    newNode->parent = 0;
    newNode->keys = 0;

    // Increment the number of keys in the tree.
    tree->keys = tree->keys + 1;

    place = tree->root;

    while(place != 0)
    {
        // Increase number of keys in each node we traverse before inserting
        // new element into bottom.
        place->keys = place->keys + 1;
        parent = place;
        if(newNode->value < place->value)
        {
            place = place->left;
        }
        else
        {
            place = place->right;
        }
    }

    newNode->parent = parent;

    if(parent == 0)
    {
        tree->root = newNode;
    }
    else if(newNode->value < parent->value)
    {
        parent->left = newNode;
    }
    else
    {
        parent->right = newNode;
    }

    avlBalance(tree, newNode);
}

void avlTransplant(PTR_AVLTREE tree, PTR_AVLNODE sub1, PTR_AVLNODE sub2)
{
    if(sub1->parent == 0)
    {
        tree->root = sub2;
    }
    else if(sub1 == sub1->parent->left)
    {
        sub1->parent->left = sub2;
    }
    else
    {
        sub1->parent->right = sub2;
    }

    if(sub2 != 0)
    {
        sub2->parent = sub1->parent;
    }
}

void avlDelete(PTR_AVLTREE tree, int key)
{
    // Search for node that needs to be deleted.
    PTR_AVLNODE deleteNode = avlSearch(tree, key);
    PTR_AVLNODE parent = 0;

    if(deleteNode == 0)
        return;

    // Decrement the number of keys in the tree.
    tree->keys--;

    parent = tree->root;
    while((parent != deleteNode) && (parent != 0))
    {
        parent->keys = parent->keys - 1;
        if(key < parent->value)
        {
            parent = parent->left;
        }
        else
        {
            parent = parent->right;
        }
    }

    if(deleteNode->left == 0)
    {
        avlTransplant(tree, deleteNode, deleteNode->right);

        if(deleteNode == tree->root)
        {
            tree->root = deleteNode->right;
        }

        avlBalance(tree, deleteNode->right);
    }
    else if(deleteNode->right == 0)
    {
        avlTransplant(tree, deleteNode, deleteNode->left);

        if(deleteNode == tree->root)
        {
            tree->root = deleteNode->left;
        }

        avlBalance(tree, deleteNode->left);
    }
    else
    {
        PTR_AVLNODE newRoot = avlMinimum(deleteNode->right);

        if(newRoot->parent != deleteNode)
        {
            avlTransplant(tree, newRoot, newRoot->right);
            newRoot->right = deleteNode->right;
            newRoot->right->parent = newRoot;
        }

        avlTransplant(tree, deleteNode, newRoot);
        newRoot->left = deleteNode->left;
        newRoot->left->parent = newRoot;

        if(deleteNode == tree->root)
        {
            tree->root = newRoot;
        }

        avlBalance(tree, newRoot);
    }

    // Free deleted nodes memory;
    free(deleteNode);
}

PTR_AVLNODE avlSearch(PTR_AVLTREE tree, int key)
{
    if(tree->root == 0)
        return 0;

    PTR_AVLNODE foundNode = tree->root;

    while((foundNode != 0) && (key != foundNode->value))
    {
        if(key < foundNode->value)
        {
            foundNode = foundNode->left;
        }
        else
        {
            foundNode = foundNode->right;
        }
    }

    if(foundNode && foundNode->value == key)
    {
        return foundNode;
    }
    else
    {
        return 0;
    }
}

void avlBalance(PTR_AVLTREE tree, PTR_AVLNODE node)
{
    if(!node)
        return;

    PTR_AVLNODE changingNode = node;

    while(changingNode != 0)
    {
        int rightHeight = avlNodeHeight(changingNode->right);
        int leftHeight = avlNodeHeight(changingNode->left);

        if((leftHeight - rightHeight) == 2)
        {
            rightHeight = avlNodeHeight(changingNode->left->right);
            leftHeight = avlNodeHeight(changingNode->left->left);

            if(leftHeight > rightHeight)
            {
                // Perform single right rotation.
                PTR_AVLNODE child = changingNode->left;
                PTR_AVLNODE parent = changingNode->parent;

                int left = (parent && (parent->left == changingNode)) ? 1 : 0;

                if(child->right)
                {
                    child->right->parent = changingNode;
                }

                changingNode->left = child->right;

                changingNode->parent = child;

                child->parent = parent;
                child->right = changingNode;

                if(parent == 0)
                {
                    //update root
                    tree->root = child;
                }
                else
                {
                    if(left)
                        parent->left = child;
                    else
                        parent->right = child;
                }

                int tempKeys = 0;

                //Update keys
                if(child->right->left)
                    tempKeys+=(child->right->left->keys + 1);

                if(child->right->right)
                    tempKeys+=(child->right->right->keys + 1);

                child->right->keys = tempKeys;

                child->keys = child->left->keys + child->right->keys + 2;

                //Go up the tree now.
                changingNode = parent;
            }
            else
            {
                PTR_AVLNODE leftRightChild = changingNode->left->right->left;
                PTR_AVLNODE rightLeftChild = changingNode->left->right->right;
                PTR_AVLNODE rootParent = changingNode->parent;
                PTR_AVLNODE child = changingNode->left;
                PTR_AVLNODE newRoot = changingNode->left->right;

                int left = (rootParent && (rootParent->left == changingNode)) ? 1 : 0;

                child->right = leftRightChild;
                if(leftRightChild)
                {
                    leftRightChild->parent = child;
                }

                changingNode->left = rightLeftChild;
                if(rightLeftChild)
                {
                    rightLeftChild->parent = changingNode;
                }

                changingNode->parent = newRoot;
                newRoot->right = changingNode;

                child->parent = newRoot;
                newRoot->left = child;

                newRoot->parent = rootParent;

                if(rootParent == 0)
                {
                    //update root
                    tree->root = newRoot;
                }
                else
                {
                    if(left)
                        rootParent->left = newRoot;
                    else
                        rootParent->right = newRoot;
                }

                int tempKeys = 0;

                if(newRoot->right->right)
                    tempKeys=newRoot->right->right->keys+1;
                if(newRoot->right->left)
                    tempKeys+=newRoot->right->left->keys+1;

                newRoot->right->keys = tempKeys;

                if(newRoot->left->right)
                    tempKeys=newRoot->left->right->keys+1;
                if(newRoot->left->left)
                    tempKeys+=newRoot->left->left->keys+1;

                newRoot->left->keys = tempKeys;

                newRoot->keys = newRoot->right->keys + newRoot->left->keys + 2;

                changingNode = rootParent;
            }
        }
        else if(rightHeight - leftHeight == 2)
        {
            rightHeight = avlNodeHeight(changingNode->right->right);
            leftHeight = avlNodeHeight(changingNode->right->left);

            if(rightHeight > leftHeight)
            {
                // Perform single right rotation.
                PTR_AVLNODE child = changingNode->right;
                PTR_AVLNODE parent = changingNode->parent;

                int left = (parent && (parent->left == changingNode)) ? 1 : 0;

                if(child->left)
                {
                    child->left->parent = changingNode;
                }

                changingNode->right = child->left;

                changingNode->parent = child;

                child->parent = parent;
                child->left = changingNode;

                if(parent == 0)
                {
                    //update root
                    tree->root = child;
                }
                else
                {
                    if(left)
                        parent->left = child;
                    else
                        parent->right = child;
                }

                int tempKeys = 0;

                //update keys
                if(child->left->left)
                    tempKeys+=(child->left->left->keys + 1);

                if(child->left->right)
                    tempKeys+=(child->left->right->keys + 1);

                child->left->keys = tempKeys;

                child->keys = child->left->keys + child->right->keys + 2;

                //Go up the tree now.
                changingNode = parent;
            }
            else
            {
                PTR_AVLNODE leftRightChild = changingNode->right->left->left;
                PTR_AVLNODE rightLeftChild = changingNode->right->left->right;
                PTR_AVLNODE rootParent = changingNode->parent;
                PTR_AVLNODE child = changingNode->right;
                PTR_AVLNODE newRoot = changingNode->right->left;

                int left = (rootParent && (rootParent->left == changingNode)) ? 1 : 0;

                changingNode->right = leftRightChild;
                if(leftRightChild)
                {
                    leftRightChild->parent = changingNode;
                }

                child->left = rightLeftChild;
                if(rightLeftChild)
                {
                    rightLeftChild->parent = child;
                }

                changingNode->parent = newRoot;
                newRoot->left = changingNode;

                child->parent = newRoot;
                newRoot->right = child;

                newRoot->parent = rootParent;

                changingNode = rootParent;

                if(rootParent == 0)
                {
                    //update root
                    tree->root = newRoot;
                }
                else
                {
                    if(left)
                        rootParent->left = newRoot;
                    else
                        rootParent->right = newRoot;
                }

                int tempKeys = 0;

                if(newRoot->right->right)
                    tempKeys=newRoot->right->right->keys+1;
                if(newRoot->right->left)
                    tempKeys+=newRoot->right->left->keys+1;

                newRoot->right->keys = tempKeys;

                if(newRoot->left->right)
                    tempKeys=newRoot->left->right->keys+1;
                if(newRoot->left->left)
                    tempKeys+=newRoot->left->left->keys+1;

                newRoot->left->keys = tempKeys;

                newRoot->keys = newRoot->right->keys + newRoot->left->keys + 2;
            }
        }
        else
        {
            changingNode = changingNode->parent;
        }
    }
}

int main(int argc, const char * argv[])
{
    int status = 0;
    bool printHelp = false;
    char printArray;
    int choice;
    int value;
    int numberOfIntegers;
    int continueInt = 1;
    const char* avlOperationType = 0;
    PTR_AVLNODE searchNode = 0;

    while(1)
    {
        // Use file to store timings of insert, search, delete.
        FILE* filePtr = fopen("output.txt", "w+");

        if(!filePtr)
        {
            status = 1;
            break;
        }

        // allocate tree.
        AVLTREE* tree = (PTR_AVLTREE) calloc(1 ,sizeof(AVLTREE));

        if(!tree)
        {
            status = 2;
            break;
        }

        while(continueInt > 0)
        {
            timespec tsBef;
            timespec tsAfter;
            int printTime = 0;

            printf("You can do the following\n");
            printf("1 - Insert new key\n");
            printf("2 - Delete existing key\n");
            printf("3 - search for existing key\n");
            printf("4 - Insert N number of random keys\n");
            printf("5 - Display tree in inorder traversal\n");
            printf("6 - Display node with key\n");
            printf("0 - exit\n");
            printf("What do you want to do?\n");

            int retScan = scanf("%d", &choice);

            if(retScan != 1)
            {
                scanf("%*s");  //clear any characters from the input buffer.
                continue;
            }

            switch(choice)
            {
                case 1:
                    printf("What key value to insert?\n");
                    scanf("%d", &value);

                    if(value < 0)
                    {
                        printf("You entered %d for value.  Please enter positive number\n", value);
                        break;
                    }

                    clock_gettime(CLOCK_REALTIME, &tsBef);
                    avlInsert(tree, value);
                    clock_gettime(CLOCK_REALTIME, &tsAfter);
                    printTime = 1;

                    avlOperationType = "Insert";

                    printf("Key was inserted\n");
                    break;
                case 2:
                    printf("What key value to delete?\n");
                    scanf(" %d", &value);

                    if(value < 0)
                    {
                        printf("You entered %d for value.  Please enter positive number\n", value);
                        break;
                    }
                    avlOperationType = "delete";
                    clock_gettime(CLOCK_REALTIME, &tsBef);
                    avlDelete(tree, value);
                    clock_gettime(CLOCK_REALTIME, &tsAfter);

                    avlOperationType = "Delete";
                    printTime = 1;
                    printf("Key was deleted\n");
                    break;
                case 3:
                    printf("What key value to search for?\n");
                    scanf(" %d", &value);

                    if(value < 0)
                    {
                        printf("You entered %d for value.  Please enter positive number\n", value);
                        break;
                    }

                    avlOperationType = "search";
                    clock_gettime(CLOCK_REALTIME, &tsBef);
                    searchNode = avlSearch(tree, value);
                    clock_gettime(CLOCK_REALTIME, &tsAfter);

                    avlOperationType = "Search";
                    printTime = 1;

                    if(searchNode)
                    {
                        printf("Found node with value:%d\n", value);
                        printAVLtreeElements(searchNode, INORDER, 0);
                    }
                    else
                    {
                        printf("Didn't find node with value %d\n", value);
                    }

                    break;
                case 4:
                    printf("How many random keys do you want to insert?\n");
                    scanf("%d", &numberOfIntegers);

                    if(numberOfIntegers < 0)
                    {
                        printf("You entered %d for numIntegers.  Please enter positive number\n", numberOfIntegers);
                        break;
                    }
                    printTime = 1;
                    avlOperationType = "insertrandom";

                    clock_gettime(CLOCK_REALTIME, &tsBef);
                    generateRandomAVLtree(tree, numberOfIntegers);
                    clock_gettime(CLOCK_REALTIME, &tsAfter);
                    printf("Keys were inserted\n");
                    break;
                case 5:
                    printAVLtreeElements(tree->root, INORDER, 1);
                    break;
                case 6:
                    printf("What key value to display?\n");
                    scanf(" %d", &value);

                    if(value < 0)
                    {
                        printf("You entered %d for value.  Please enter positive number\n", value);
                        break;
                    }
                    printTime = 1;
                    avlOperationType = "display";

                    clock_gettime(CLOCK_REALTIME, &tsBef);
                    printAVLtreeElements(avlSearch(tree, value), INORDER, 0);
                    clock_gettime(CLOCK_REALTIME, &tsAfter);
                    break;
            case 0:
            default:
                status = 3;
                continueInt = 0;
                break;
            }

            if(printTime)
            {
                if(tsBef.tv_sec == tsAfter.tv_sec)
                {
                    fprintf(filePtr, "\nIt took %d nanoseconds to %s key value %d"
                                    " with tree size of %d\n",
                                    (int)(tsAfter.tv_nsec - tsBef.tv_nsec),
                                    avlOperationType, value,
                                    tree->keys);
                }
                else
                {
                    // If we crossed a second boundary we need to calculate that.

                    // First calculate passed nanoseconds no mater how many seconds have passed.
                    long nanoPassed = (1000000000 - tsBef.tv_nsec) + tsAfter.tv_nsec;

                    // Now calculate passed seconds.
                    long secPassed = tsAfter.tv_sec - tsBef.tv_sec - 1 + (nanoPassed / 1000000000);

                    // Remove the seconds worth of nanoseconds.
                    nanoPassed = nanoPassed % 1000000000;

                    fprintf(filePtr, "\nIt took %d seconds %d nanoseconds to %s key value %d"
                                    " with tree size of %d\n",
                                    (int)(secPassed), (int)(nanoPassed),
                                    avlOperationType, value,
                                    tree->keys);
                }
            }
        }

        break;
    }

    return status;
}

