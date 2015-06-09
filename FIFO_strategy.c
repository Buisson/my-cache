/*!
 * \file FIFO_strategy.c
 *
 * \brief  Stratégie de remplacement du bloc le plus vieux du cache
 * 
 * \author Jihane Asri
 *
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "time.h"

/*!
 * FIFO : Initialisation de la liste de blocs. 
 *
 */
void *Strategy_Create(struct Cache *pcache){
    return Cache_List_Create();
}


/*!
 * FIFO : Fermeture de la stratégie.
 */
void Strategy_Close(struct Cache *pcache)
{
    Cache_List_Delete((struct Cache_List *)(pcache->pstrategy));
}

/*!
 * FIFO : Invalidation de la stratégie: on vide la liste de blocs.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
    Cache_List_Clear((struct Cache_List *)(pcache->pstrategy));
}

/*! 
 * FIFO : On prend le premier bloc invalide. S'il n'y en a plus, on prend le bloc le plus vieux.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    struct Cache_Block_Header *pbh;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) {
	Cache_List_Append((struct Cache_List *)(pcache->pstrategy) , pbh );
	return pbh;
    }

    /* Sinon on prend le premier bloc de la liste */
    pbh = Cache_List_Remove_First( (struct Cache_List *)(pcache->pstrategy) );
    Cache_List_Append((struct Cache_List *)(pcache->pstrategy) , pbh );
    return pbh;
}


/*!
 * FIFO : Rien à faire ici.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
}  

/*!
 * FIFO : Rien à faire ici.
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
} 

char *Strategy_Name()
{
    return "FIFO";
}
