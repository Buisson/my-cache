/* ----------------------------------------------------------------- *
 * Alexandre CAZALA                                                  *
 * Groupe 2 - SI3                                                    *
 * Polytech Nice-Sophia                                              *
 *                                                                   *
 * Projet Fin d'année SI3                                            *
 * Low_Cache 									                                       *
 * ----------------------------------------------------------------- */

#include "low_cache.h"

// Recherche d'un bloc libre.
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache){
	struct Cache_Block_Header *bheader;
	int i;
	// on parcourt tout les blocks
	for(i = 0; i < pcache->nblocks; ++i){
		bheader = &(pcache->headers[i]);
		// on renvoie celui qui est valide
		if((bheader->flags & VALID) == 0)	return bheader;
	}
	return NULL;
}
