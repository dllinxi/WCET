#ifndef GEL_MEMORY_PLUGIN_H_
#define GEL_MEMORY_PLUGIN_H_

#include <gel/common.h>

__BEGIN_DECLS

gel_memory_image_t* gel_init_memory_image (gel_env_t *env, gel_block_t** blocks, int nb_block);
                                           
                                           
/**
 * Lib�re proprement l'espace memoire occupee par une structure 
 * gel_memory_image_t
 * 
 * @param memory pointeur vers le gel_memory_image_t a liberer
 */
void gel_free_image_memory(gel_memory_image_t* memory);

__END_DECLS

#endif /*GEL_MEMORY_PLUGIN_H_*/
