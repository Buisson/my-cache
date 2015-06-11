/*!
 * \file cache.c
 *
 * \brief  Implémentation du cache
 * 
 * \author Dorian Blanc
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include "low_cache.h"

//! Calcul du hit rate
 int Hit_Rate_Compute(struct Cache *pcache){
 	return pcache->instrument.n_hits / (pcache->instrument.n_reads + pcache->instrument.n_writes);
 }

//! Création du cache.
 struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
 	size_t recordsz, unsigned nderef){

 	struct Cache *cache = (struct Cache*) malloc(sizeof(struct Cache));

	cache->file = basename(fic);		//!< Nom du fichier   
	cache->fp = fopen(fic, "a+");		//!< Pointeur sur fichier, option 'a+' (Opens a file for reading and appending.)
	cache->nblocks = nblocks;			//!< Nb de blocs dans le cache
	cache->nrecords = nrecords;			//!< Nombre d'enregistrements dans chaque bloc
	cache->recordsz = recordsz;			//!< Taille d'un enregistrement
	cache->blocksz = recordsz*nrecords; //!< Taille d'un bloc.
	cache->nderef = nderef;				//!< période de déréférençage pour NUR 
	//cache->pstrategy = NULL;			//!< Structure de données dépendant de la stratégie

	struct Cache_Instrument instrument;
	instrument.n_reads = 0; 	//!< Nombre de lectures.
    instrument.n_writes = 0;	//!< Nombre d'écritures.
    instrument.n_hits = 0;		//!< Nombre de fois où l'élément était déjà dans le cache->
    instrument.n_syncs = 0;	//<! Nombre d'appels à Cache_Sync().
    instrument.n_deref = 0;

    cache->instrument = instrument;

    struct Cache_Block_Header *headers = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header)*nblocks);

	//initialisation des headers
    for(int i = 0 ; i < nblocks ; ++i){
    	cache->headers[i].ibcache = i;
    	cache->headers[i].flags = 0;
    	cache->headers[i].data = malloc(nrecords * recordsz);
    }

    cache->headers=headers;
    cache->pfree=Get_Free_Block(cache);
	//cache->pfree = &cache->headers[0];

    return &cache;
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
	free(pcache->headers);
	free(pcache->pfree);
	free(pcache);
	return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){
	int fd = fileno(pcache->fp), i;
	for(i=0 ; i< pcache->nblocks ; i++){
		struct Cache_Block_Header *header = pcache->headers+i;
		if( header->flags & MODIF){
			char* data = header->data;
			lseek(fd, pcache->recordsz*header->ibfile, SEEK_SET);
			write(fd, data, pcache->recordsz);
		}
	}
	return CACHE_OK;
}

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache){
	int i;
	for(i=0 ; i< pcache->nblocks ; i++)
		if((pcache->headers+i)->flags & VALID)
			(pcache->headers+i)->flags-=VALID;
		return CACHE_OK;
	}


//! retourne le block en fonction de l'indice d'enregistrement dans le fichier
	struct Cache_Block_Header * getBlockByIbfile(struct Cache *pcache, int irfile){
// Indice du bloc contenant l'enregistrement
		int ibSearch = irfile / pcache->nrecords;
		for(int i = 0 ; i < pcache->nblocks ; ++i){
if(pcache->headers[i].flags & VALID){//si le block est valide
if(pcache->headers[i].ibfile == ibSearch)//si il contient les bonnes infos
	return &pcache->headers[i];
}
}
return NULL;
}
//! retourne le block correspondant a l'irfile passé en param
struct Cache_Block_Header * Read_In_Cache(struct Cache *pcache, int irfile){
	struct Cache_Block_Header * header = getBlockByIbfile(pcache, irfile);
	//si le block n'est pas dans le cache
	if(header == NULL){
		header = Strategy_Replace_Block(pcache);
		header->ibfile = irfile / pcache->nrecords;
		if(fseek(pcache->fp, DADDR(pcache, header->ibfile), SEEK_SET) != 0) return CACHE_KO;
		if(fgets(header->data, pcache->blocksz, pcache->fp) == EOF) return CACHE_KO;
		//MAJ des flags
		header->flags |=VALID;
		header->flags &= ~MODIF;
	} else
	{
		//l'élément est dans le cache
		pcache->instrument.n_hits++;
	}
	return header;
}
//! Lecture (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){
	struct Cache_Block_Header * header = Read_In_Cache(pcache, irfile);
		//on copie dans le buffer
	memcpy(precord, ADDR(pcache, irfile, header) , pcache->recordsz);
		//+1 au nombre de lecture
	pcache->instrument.n_reads++;
	Check_Synchronisation(pcache);
	Strategy_Read(pcache, header);
	return CACHE_OK;
}

//! Résultat de l'instrumentation.
// retourne les stats et les reinit
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache)
{
	struct Cache_Instrument instrument = pcache->instrument;
	pcache->instrument.n_reads = 0;
	pcache->instrument.n_writes = 0;
	pcache->instrument.n_hits = 0;
	pcache->instrument.n_syncs = 0;
	pcache->instrument.n_deref = 0;
	return &instrument;
}