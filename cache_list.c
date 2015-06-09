#include "low_cache.h"
#include "cache_list.h"


struct Cache_List *Cache_List_Create(){
	struct Cache_List* ptrLst = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	
	struct Cache_Block_Header *header = NULL;
	
	ptrLst->pheader=header;
	ptrLst->next=ptrLst;
	ptrLst->prev=ptrLst;

	return ptrLst;
}

void Cache_List_Delete(struct Cache_List *list){
	struct Cache_List *nxt, *current;

	while( (current = list->next) != list){
		nxt = current->next;
		free(current);
	}

	free(list);
}

void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
	struct Cache_List* new = (struct Cache_List*)malloc(sizeof(struct Cache_List));
	
	new->next = list;
	new->prev = list->prev;
	new->pheader = pbh;

	list->prev->next = new;
}

void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
	struct Cache_List* tmp = list->next;
	struct Cache_List* new = (struct Cache_List*)malloc(sizeof(struct Cache_List));

	new->next = tmp;
	new->prev = list;
	new->pheader = pbh;

	tmp->prev=new;
}


struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
	struct Cache_List * tmp = list->next;
	list->next = list->next->next;
	list->next->prev = list;

	return tmp->pheader;
}


struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){
	struct Cache_List * tmp = list->prev;
	list->prev = list->prev->prev;
	list->prev->next = list;

	return tmp->pheader;
}

struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,struct Cache_Block_Header *pbh){
	struct Cache_List * tmp;
	while((tmp = list->next) != list){
		
	}
}


void Cache_List_Clear(struct Cache_List *list){
	Cache_List_Delete(list);
	list = Cache_List_Create();
}

bool Cache_List_Is_Empty(struct Cache_List *list){
	return list->next==list && list->prev==list;
}

void Cache_List_Move_To_End(struct Cache_List *list,struct Cache_Block_Header *pbh){	
	struct Cache_List * tmp;
	while((tmp = list->next) != list){
		
		if(tmp->pheader == pbh){
			printf("ici\n");
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			tmp->prev = list->prev;
			tmp->next=list;
			list->prev->next = tmp;
			list->prev = tmp;
			break;			
		}				
	}
	printf("ici\n");
}

void Cache_List_Move_To_Begin(struct Cache_List *list,struct Cache_Block_Header *pbh){
	struct Cache_List * tmp;
	while((tmp = list->next) != list){
		printf("la\n");
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


