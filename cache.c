/*!
 * \file cache.c
 *
 * \brief  Implémentation du cache
 * 
 * \author Dorian Blanc
 *
 */
#include <stdlib.h>
#include <./low_cache.h>

//! Calcul du hit rate
 int Hit_Rate_Compute(struct Cache *pcache){
 	return pcache.instrument->n_hits / (pcache.instrument->n_reads + pcache.instrument->n_writes)
 }

//! Création du cache.
 struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
 	size_t recordsz, unsigned nderef){

 	struct Cache *cache = (struct Cache*) malloc(sizeof(struct Cache));

	cache->file = basename(fic);		//!< Nom du fichier   
	cache->FILE = fopen(fic, "a+");		//!< Pointeur sur fichier, option 'a+' (Opens a file for reading and appending.)
	cache->nblocks = nblocks;			//!< Nb de blocs dans le cache
	cache->nrecords = nrecords;			//!< Nombre d'enregistrements dans chaque bloc
	cache->recordsz = recordsz;			//!< Taille d'un enregistrement
	cache->blocksz = recordsz*nrecords; //!< Taille d'un bloc.
	cache->nderef = nderef;				//!< période de déréférençage pour NUR 
	//cache->pstrategy = NULL;			//!< Structure de données dépendant de la stratégie

	struct Cache_Instrument instrument;
	instrument->n_reads = 0; 	//!< Nombre de lectures.
    instrument->n_writes = 0;	//!< Nombre d'écritures.
    instrument->n_hits = 0;		//!< Nombre de fois où l'élément était déjà dans le cache->
    instrument->n_syncs = 0;	//<! Nombre d'appels à Cache_Sync().
    instrument->n_deref = 0;

    cache->instrument = instrument;

	struct Cache_Block_Header *headers = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header)*nblocks);
	for(int i=0 ; i < nblocks ; i++){
		(headers+i)->flags=VALID;
		(headers+i)->ibfile=0;
		(headers+i)->ibcache=0;
	}

	cache->headers=headers;
	cache->pfree=Get_Free_Block(cache);
 
    return &cache;
};

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
	free(pcache->headers);
	free(pcache->pfree);
	free(pcache);
	return CACHE_OK;
};

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
};

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache){
	int i;
	for(i=0 ; i< pcache->nblocks ; i++)
		if((pcache->headers+i)->flags & VALID)
			(pcache->headers+i)->flags-=VALID;
	return CACHE_OK;
};

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){

};

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){

};

//! Instrumentation du cache.
/*!
 * \ingroup cache_interface
 */
 struct Cache_Instrument
 {
    unsigned n_reads; 	//!< Nombre de lectures.
    unsigned n_writes;	//!< Nombre d'écritures.
    unsigned n_hits;	//!< Nombre de fois où l'élément était déjà dans le cache.
    unsigned n_syncs;	//<! Nombre d'appels à Cache_Sync().
    unsigned n_deref;	//!< Nombre de déréférençage (stratégie NUR).
};

//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache){

};

