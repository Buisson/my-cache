/*!
 * \file LRU_strategy.c
 *
 * \brief  Stratégie de remplacement du bloc le moins recement utilisé.
 * 
 * \author Aurelien Colombet
 *
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "random.h"
#include "time.h"


/*!
 * LRU : On initialise la liste de blocs. 
 *
 */
void *Strategy_Create(struct Cache *pcache){
    return Cache_List_Create();
}

/*!
 * On détruit la liste pointé par pcache->pstrategy.
 *
 * Ici on cast le pstrategy en struct Cache_List car c'est la structure de la strategie en cours vu que a la base c'est un void * et que la on
 * Utilise une liste on est obliger de cast le void *.
 *
 */
void Strategy_Close(struct Cache *pcache){
	Cache_List_Delete((struct Cache_List *)(pcache->pstrategy));
}

/*!
 * Invalidation du cache, on vide la liste.
 */
void Strategy_Invalidate(struct Cache *pcache){
	Cache_List_Clear((struct Cache_List *)(pcache->pstrategy));
}

/*! 
 * Si on trouve un bloc libre du cache alors on l'ajoute en queue de file et on retourne le ptr sur ce bloc.
 * Sinon on prend le bloc en tête de file et on le met en queue de file et on le retourne.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) {
    struct Cache_Block_Header *pbh;

	if((pbh = Get_Free_Block(pcache)) != NULL){
		Cache_List_Append((struct Cache_List *)(pcache->pstrategy) , pbh );
		return pbh;
	}
	else{
		pbh = Cache_List_Remove_First( (struct Cache_List *)(pcache->pstrategy) );
		Cache_List_Append((struct Cache_List *)(pcache->pstrategy) , pbh );
		return pbh;
	}
}


/*!
 * LRU : Si on lit  un bloc alors on le met en queue de file car on vient de l'utiliser
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) {
	Cache_List_Move_To_End((struct Cache_List *)(pcache->pstrategy),pbh);
}  

/*!
 * LRU : Idem que pour Strategy_Read
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh){
	Cache_List_Move_To_End((struct Cache_List *)(pcache->pstrategy),pbh);
} 

char *Strategy_Name(){
    return "LRU";
}
