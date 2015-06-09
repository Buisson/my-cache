/* ----------------------------------------------------------------- *
 * Alexandre CAZALA                                                  *
 * Groupe 2 - SI3                                                    *
 * Polytech Nice-Sophia                                              *
 *                                                                   *
 * Projet Fin d'année SI3                                            *
 * Stratégie Not Recently Used                                       *
 * ----------------------------------------------------------------- */


#include "strategy.h"
#include "low_cache.h"

// Flag Reférence
#define REFER 0b100

// permet de stocker le compteur
struct pstrategy
{
    unsigned nderef;	//Période de déréférençage
    unsigned cptderef;	// cpt
};

unsigned int getPriority(struct Cache_Block_Header *bloc) {
	int rm = ((bloc->flags & REFER) ? 1:0) << 1;
	rm |= ((bloc->flags & MODIF) ? 1:0);
	return rm;
}

/* NUR : déréférençage
 * pour pcache->nderef > 0
 */
void Deref (struct Cache *pcache)
{
	struct pstrategy *strategy = (struct pstrategy *)(pcache)->pstrategy;
	if (strategy->nderef == 0 || --strategy->cptderef > 0) return;

	// On met le flag REFER à 0 à tous les blocs
	for (int i = 0; i < pcache->nblocks; ++i) 
		pcache->headers[i].flags &= ~REFER;
	// on réinitialise le compteur
	strategy->cptderef = strategy->nderef;
	++pcache->instrument.n_deref;
	
}

/* NUR : initialisation
 */
void *Strategy_Create(struct Cache *pcache) 
{
	struct pstrategy *strategy = malloc(sizeof(struct pstrategy));
	// Initialisation des champs de la structure
	strategy->nderef = pcache->nderef;
	strategy->cptderef = pcache->nderef;
	return strategy;
}

/* NUR : Strategy_Close
 * On libère l'espace alloué
 */
void Strategy_Close(struct Cache *pcache)
{
	free(pcache->pstrategy);
}

/* NUR : Invalidate
 * Remise à zéro des bits de référence
 */
void Strategy_Invalidate(struct Cache *pcache) 
{
	struct pstrategy *strategy = (struct pstrategy *)(pcache)->pstrategy;
	if (strategy->nderef != 0) {
		  strategy->cptderef = 1;
		  Deref(pcache);
	}  
}

/* NUR : Stratégie de remplacement de bloc
 * Renvoie le premier block invalide ou celui avec le rm le plus petit
 * (référencé dans la dernière tranche de temps puis non modifié)
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
	struct Cache_Block_Header *pbh_save = NULL;
	struct  Cache_Block_Header *pbh;

	
	/* On cherche d'abord un bloc invalide */
	if ((pbh = Get_Free_Block(pcache)) != NULL) 
		return pbh;
	
	int rm;
	unsigned int rm_min = 5;
	//à partir de là on commence l'algorithme concret de NUR
	// On parcourt tout les blocks pour trouver celui qui nous intéresse
	for (int i = 0; i < pcache->nblocks; ++i)
	{
		struct Cache_Block_Header *cachi = &pcache->headers[i];
		//on cherche rm minimal
		rm = getPriority(cachi);
		// si rm vaut 0 alors on ne peut pas trouver plus petit
		if (rm == 0) 
			return cachi;
		// s'il est plus petit que celui qu'on a, on le stocke
		if (rm < rm_min) 
		{
			rm_min = rm;
			pbh_save = cachi;
		}	
	}
	return pbh_save;    
}

/* NUR : Read
 * Mets le flag à 1
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
	// Si le compteur de déréférencements est égal à nderef, on réinitialise
	Deref(pcache);
	pbh->flags |= REFER;
} 
  
/* NUR : Write
 * Mets le flag à 1
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
	// Si le compteur de déréférencements est égal à nderef, on réinitialise
	Deref(pcache);
	pbh->flags |= REFER;
} 

/*
 * Renvoie le nom de la stratégie
 */
char *Strategy_Name()
{
	return "NUR";
}
