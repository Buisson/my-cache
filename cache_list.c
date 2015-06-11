/*!
 * \file low_cache.c
 * 
 * \author Aurelien Colombet
 * \author Alexandre Cazala
 */

#include <stdlib.h>
#include "cache_list.h"

/*! Création d'une liste de blocs */
struct Cache_List *Cache_List_Create(){
    struct Cache_List* new_list = (struct Cache_List*)malloc(sizeof(struct Cache_List));
    new_list->pheader = NULL;
    new_list->prev    = new_list;
    new_list->next    = new_list;

    return new_list;
}

/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list){
    if (!Cache_List_Is_Empty(list)){
       for (struct Cache_List* tmp = list->next; tmp != list; tmp = tmp->next){
            tmp->pheader    = NULL;
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            free(tmp);
        }
    }

    list->pheader = NULL;
    free(list);
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List *new_elem = Cache_List_Create();
    new_elem->pheader = pbh;

    struct Cache_List *last;
    // on va à la fin de la liste
    for (last = list->next ; last != list; last = last->next);
		
	// on insère notre élément
    new_elem->prev   = last->prev;
    last->prev->next = new_elem;
    last->prev       = new_elem;
    new_elem->next   = last;
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List *new_elem = Cache_List_Create();
    new_elem->pheader = pbh;
	struct Cache_List *first;
	// Par sécurité on va au début de la liste
	for (first = list->prev ; first != list; first = first->prev);
    new_elem->prev = first->next->prev;
    first->next = new_elem;
    first->next->prev = new_elem;
    new_elem->next = first->next;
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
	struct Cache_List *tmp;
	// Par sécurité on va au début de la liste
	for (tmp = list->prev ; tmp != list; tmp = tmp->prev);
	// on récup le deuxième
	tmp = tmp->next;
                    
    tmp->prev->next = tmp->next;
    tmp->next->prev = tmp->prev;

    struct Cache_Block_Header *header = tmp->pheader;
    free(tmp);
    return header;
}

/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){
    struct Cache_List* last;
    // on va au dernier de la liste
    for (last = list->next; last != list; last = last->next);

    last->prev->next = last->next;
    last->next->prev = last->prev;

    struct Cache_Block_Header *header = last->pheader;
    free(last);
    return header;
}

/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List* element = list;

	/*On parcours la liste et des qu'on trouve l'element on break*/
	while(element->next!=NULL){
		if(element->pheader==pbh)
			break;
		element = element->next;
	}
            
    element->prev->next = element->next;  
    element->next->prev = element->prev;

    struct Cache_Block_Header *header = element->pheader;
    free(element);
    return header;
}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list){
    for (struct Cache_List* tmp = list->next; tmp != list; tmp = tmp->next)
    {
        tmp->pheader    = NULL;
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        free(tmp);
    }

    list->pheader = NULL;
}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list){
	// si jamais on ne peut pas partir de l'élément et qu'on a pas d'header
    return (list->next == list->prev) && (list->pheader == NULL);
}

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh){
	// on l'enlève et on le met a la fin
    Cache_List_Remove(list, pbh);
    Cache_List_Append(list, pbh);
}

/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh){
	// on l'enlève et on le met au début
    Cache_List_Remove(list, pbh);
    Cache_List_Prepend(list, pbh);
}

