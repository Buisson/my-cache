#include "low_cache.h"
#include "cache_list.h"

#include <stdbool.h>

struct Cache_List *Cache_List_Create(){
	printf("Cache_List_Create\n");
	struct Cache_List* ptrLst = malloc(sizeof(struct Cache_List));
	ptrLst->pheader=NULL;
	ptrLst->next=NULL;
	ptrLst->prev=NULL;

	return ptrLst;
}

void Cache_List_Delete(struct Cache_List *list){
	printf("iciCache_List_Delete\n");	
	struct Cache_List *current = list;

	while( (current = current->next) != NULL){
		free(current);
	}
	
	free(list);
}

void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
	printf("iciAppend\n");	
	if(Cache_List_Is_Empty(list)){
		list->pheader=pbh;
	} else {
		struct Cache_List* new = Cache_List_Create();
		struct Cache_List* tmp = list;
		// on parcourt la liste pour récupérer le dernier
		while ((tmp->next) != NULL) {
			tmp = tmp->next;
		} 
		new->prev = tmp;
		new->pheader = pbh;
		tmp->next = new;
		printf("FINAppend\n");
	}
	
}

void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
	printf("iciPrepend\n");
	if(Cache_List_Is_Empty(list)){
		list->pheader=pbh;
	} else {	
		struct Cache_List* new = Cache_List_Create();
		new->pheader = pbh;
		if (Cache_List_Is_Empty(list)) {
			list = new;
		} else {
			struct Cache_List* tmp = Cache_List_Create();
			while (tmp->prev != NULL) tmp = tmp->prev;
			new->next = tmp;
			tmp->prev = new;
		}
	}
}


struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
	printf("iciCache_List_Remove_First\n");	
	if(!Cache_List_Is_Empty(list)){	
		struct Cache_List* tmp = list;
		while (tmp->prev != NULL) tmp = tmp->prev;
		struct Cache_Block_Header *bheader = tmp->pheader;	
		tmp = tmp->next;
		Cache_List_Delete(tmp->prev);
		tmp->prev =NULL;
		return bheader;
	}
	return NULL;
}


struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){
	printf("iciCache_List_Remove_Last\n");
	if(!Cache_List_Is_Empty(list)){	
		struct Cache_List *tmp = list;
		while(tmp->next!=NULL){
			tmp = tmp->next;
		}
		return Cache_List_Remove(list,tmp->pheader);
	}
	return NULL;
}

struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,struct Cache_Block_Header *pbh){
	printf("Cache_List_Remove\n");	
	struct Cache_List* tmp;
	if (Cache_List_Is_Empty(list)) {
		printf("AHAHAHAHAHAH t'es con alex\n");
	}
	for (tmp = list; tmp->pheader != pbh && tmp->next != NULL;tmp = tmp->next) {}
	if (pbh == tmp->pheader) {
		printf("REMOVE : Trouvé 1\n");
		struct Cache_Block_Header* htmp = tmp->pheader;
		printf("%p\n",htmp);
		printf("%p\n",pbh);
		struct Cache_List* tmp_prev = tmp->prev;
		struct Cache_List* tmp_next = tmp->next;
		printf("ici ?\n");
		printf("%p\n",tmp);
		tmp_prev->next = tmp->next;
		tmp_next->prev = tmp->prev;
		printf("hey ?\n");
		/*if(tmp->next!=NULL) {
			tmp2 = tmp->next;
			tmp2->prev = tmp->prev;
		}*/
		struct Cache_Block_Header *retour = tmp->pheader;			
		free(tmp);
		printf("##########################################%p############################",retour);
		return retour;
	}
	// au cas où on part dans l'autre sens
	for (tmp = list; tmp->pheader != pbh && tmp->prev != NULL;tmp = tmp->prev) {}
	printf("REMOVE : deuxième for \n");
	if((tmp->pheader)==pbh){
		(tmp->next)->prev = tmp->prev;
		if(tmp->prev!=NULL)
			(tmp->prev)->next = tmp->prev;
		struct Cache_Block_Header *retour = tmp->pheader;			
		free(tmp);
		printf("Boucle de prev %p",retour);
		return retour;
	}
	return NULL;
}


void Cache_List_Clear(struct Cache_List *list){
	printf("iciCache_List_Clear\n");
	Cache_List_Delete(list);
	list = Cache_List_Create();
}

bool Cache_List_Is_Empty(struct Cache_List *list){
	printf("iciCache_List_Is_Empty\n");	
	return (list->pheader==NULL && list->next==NULL && list->prev==NULL) ? true : false;
}

void Cache_List_Move_To_End(struct Cache_List *list,struct Cache_Block_Header *pbh){	
	printf("iciCache_List_Move_To_End\n");
	/*if(!Cache_List_Is_Empty(list)){
		struct Cache_List * tmp = list;
		while((tmp->next) != NULL){
			if(tmp->pheader == pbh){
				tmp->prev->next = tmp->next;

				tmp->next->prev = tmp->prev;
	

				tmp->next = NULL;
	
				struct Cache_List *temp = list;
				while(temp->next!=NULL){
					temp = temp->next;
				}
				temp->next = tmp;	
				tmp->prev = temp;
			}
			break;			

			tmp=tmp->next;		
		}
	}*/
	if(!Cache_List_Is_Empty(list)){
		struct Cache_Block_Header *header = Cache_List_Remove(list, pbh);
		Cache_List_Append(list, header);
	}
}

void Cache_List_Move_To_Begin(struct Cache_List *list,struct Cache_Block_Header *pbh){
	printf("iciCache_List_Move_To_Begin\n");	
	struct Cache_List * tmp;
	while((tmp = list->next) != NULL){
		if(tmp->pheader == pbh){
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			
			tmp->prev = list;
			list->next->prev = tmp;			
			tmp->next = list->next;
			list->next = tmp;
			break;			
		}				
	}
}
