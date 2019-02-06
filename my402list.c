#include "my402list.h"
#include "cs402.h"
#include <stdlib.h>

int My402ListInit(My402List* lst) {
    lst->num_members = 0;
    lst->anchor.obj = NULL;
    // next and prev pointing to anchor itself
    lst->anchor.next = &(lst->anchor);
    lst->anchor.prev = &(lst->anchor);
    return TRUE;
}

int My402ListLength (My402List* lst) {
    return lst->num_members;
}

int My402ListEmpty (My402List* lst) {
    return lst->num_members <= 0;
}

// return pointer of first element on list
// return null if list is empty
My402ListElem *My402ListFirst (My402List* lst) {
    if (My402ListEmpty(lst)) {
        return NULL;
    } else {
        return lst->anchor.next;
    }
}

// return pointer of last element on list
// return null if list is empty
My402ListElem *My402ListLast (My402List* lst) {
    if (My402ListEmpty(lst)) {
        return NULL;
    } else {
        return lst->anchor.prev;
    }
}

// if current element is last on the list, return NULL
// else return next element of current
My402ListElem *My402ListNext(My402List* lst, My402ListElem* current) {
    // next = anchor of lst signals the end of the list
    if (current->next == &(lst->anchor)) {
        return NULL;
    } else {
        return current->next;
    }
}

// if current element is first on the list, return NULL
// else return previous element of current
My402ListElem *My402ListPrev(My402List* lst, My402ListElem* current) {
    // prev = anchor of lst signals head of the lst
    if (current->prev == &(lst->anchor)) {
        return NULL;
    } else {
        return current->prev;
    }
}

// If list is empty, just add obj to the list. Else, add obj after Last(). 
// update list size
// returns TRUE if successful FALSE otherwise.
int My402ListAppend(My402List* lst, void* num) {
    // create new element of element size
    My402ListElem * add = (My402ListElem *)malloc(sizeof(My402ListElem));
    if (add == NULL) return FALSE;
    lst->num_members += 1;
    // Assign new node value and point next to anchor
    // point prev to previous last element
    add->obj = num;
    add->next = &(lst->anchor);
    add->prev = lst->anchor.prev;
    // assign last to the new element
    lst->anchor.prev = add;
    add->prev->next = add;
    return TRUE;
}

// If list is empty, just add obj to the list. Else, add obj before First(). 
// update list size
// returns TRUE if successful FALSE otherwise.
int My402ListPrepend(My402List* lst, void* num) {
    // create new element of element size
    My402ListElem * add = (My402ListElem *)malloc(sizeof(My402ListElem));
    if (add == NULL) return FALSE;
    lst->num_members += 1;
    add->obj = num;
    add->prev = &(lst->anchor);
    add->next = lst->anchor.next;
    // assign first to the new element
    lst->anchor.next = add;
    add->next->prev = add;
    return TRUE;
}

// unlink and delete current element, update list size
void My402ListUnlink(My402List* lst, My402ListElem* current) {
    // connect current previous and next element together
    current->prev->next = current->next;
    current->next->prev = current->prev;
    // set current as null
    current->obj = NULL;
    current->next = NULL;
    current->prev = NULL; 
    // free current memory
    free(current);
    // update list size after successful unlink
    lst->num_members -= 1;
}

// unlink and delete all elements, make list empty
void My402ListUnlinkAll(My402List* lst) {
    My402ListElem * current = My402ListFirst(lst);
    My402ListElem * ptr = NULL;
    // free memory for every element in list
    for (ptr = current->next; current != &(lst->anchor); ptr = ptr->next) {
        current->obj = NULL;
        //free(current->obj);
        current->next = NULL;
        current->prev = NULL; 
        // free current memory
        free(current);
        current = ptr;
    }
    // make empty list
    My402ListInit(lst);
}

int  My402ListInsertBefore(My402List* lst, void* num, My402ListElem* index) {
    // create new element of element size
    if (index == NULL) {
        return My402ListPrepend(lst, num);
    } else {
        My402ListElem * add = (My402ListElem *)malloc(sizeof(My402ListElem));
        if (add == NULL) return FALSE;
        lst->num_members += 1;
        add->obj = num;
        add->next = index;
        add->prev = index->prev;
        index->prev->next = add;
        index->prev = add;
    }
    return TRUE;
}

int  My402ListInsertAfter(My402List* lst, void* num, My402ListElem* index) {
    if (index == NULL) {
        return My402ListAppend(lst, num);
    } else {
        My402ListElem * add = (My402ListElem *)malloc(sizeof(My402ListElem));
        if (add == NULL) return FALSE;
        lst->num_members += 1;
        add->obj = num;
        add->next = index->next;
        add->prev = index->next->prev;
        index->next->prev = add;
        index->next = add;
    }
    return TRUE;
}

// Returns the list element elem such that elem->obj == obj.
// Returns NULL if no such element can be found.
My402ListElem *My402ListFind(My402List* lst, void* target) {
    My402ListElem *current = NULL;
    for (current = My402ListFirst(lst); current != NULL && current != &(lst->anchor); 
        current = current->next) {
        if (current->obj == target) {
            return current;
        }
    }
    return NULL;
}
