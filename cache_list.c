#include "low_cache.h"
#include "cache_list.h"


struct Cache_List *Cache_List_Create(){
	printf("iciCache_List_Create\n");
	struct Cache_List* ptrLst = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	
	struct Cache_Block_Header *header = NULL;
	
	ptrLst->pheader=header;
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
	struct Cache_List* new = (struct Cache_List*)malloc(sizeof(struct Cache_List));
	
	new->next = NULL;
	new->pheader = pbh;

	struct Cache_List *temp = list;
	while(temp->next!=NULL){
		temp = temp->next;
	}
	temp->next = new;	
	new->prev = temp;
	printf("FINAppend\n");
}

void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
	printf("iciPrepend\n");
	struct Cache_List* tmp = list->next;
	struct Cache_List* new = (struct Cache_List*)malloc(sizeof(struct Cache_List));

	new->next = tmp;
	new->prev = list;
	new->pheader = pbh;

	tmp->prev=new;
}


struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
	printf("iciCache_List_Remove_First\n");	
	if(!Cache_List_Is_Empty(list)){	
		struct Cache_List * tmp = list->next;
		list->next = list->next->next;
		return tmp->pheader;
	}
	return NULL;
}


struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){
	printf("iciCache_List_Remove_Last\n");	
	struct Cache_List * tmp = list->prev;
	list->prev = list->prev->prev;
	list->prev->next = list;

	return tmp->pheader;
}

struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,struct Cache_Block_Header *pbh){
	printf("iciCache_List_Remove\n");	
	struct Cache_List * tmp;
	while((tmp = list->next) != NULL){
		if((tmp->pheader)==pbh){
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			free(tmp->next);
			free(tmp->prev);
			return tmp->pheader;
		}
	}
}


void Cache_List_Clear(struct Cache_List *list){
	printf("iciCache_List_Clear\n");
	Cache_List_Delete(list);
	list = Cache_List_Create();
}

bool Cache_List_Is_Empty(struct Cache_List *list){
	printf("iciCache_List_Is_Empty\n");	
	return (list->next==NULL && list->prev==NULL) ? true : false;
}

void Cache_List_Move_To_End(struct Cache_List *list,struct Cache_Block_Header *pbh){	
	printf("iciCache_List_Move_To_End\n");
	struct Cache_List * tmp = list;
	while((tmp->next) != NULL){
		if(tmp->pheader == pbh){
			/*			
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			tmp->prev = list->prev;
			tmp->next=list;
			list->prev->next = tmp;
			list->prev = tmp;*/
			if(tmp->prev!=NULL){
				tmp->prev->next = tmp->next;
			}
			if(tmp->next!=NULL){
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
		}
		tmp=tmp->next;		
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
