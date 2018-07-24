/* $Id */

/**
 * \file 
 * C'est un exemple d'utilisation de la lib gel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gel/gel.h>

int main(int argc, char **argv)
{
  gel_file_t *f;
  gel_sect_t *s;
  gel_cursor_t c,c2;
  gel_sect_info_t si;
  gel_sym_t *sym;
  gel_sym_info_t syminfo;
  gel_image_t *im;
  gel_image_info_t ii;
  gel_file_info_t fi;
  gel_block_info_t bi;
  gel_enum_t *en;
  gel_env_t *myenv;
  char *str;
  gel_memory_image_info_t mem_info;
  gel_memory_cluster_info_t    cluster_info;
   char * libs_path[]=
    {
    	"/home/specmgr/Compilateur/powerpc/target/powerpc-linux-gnu/lib",
    	 NULL
    };
  int i;
  
  if (argc != 2) {
    printf("usage: ./example <prog>\n");
    exit(-1); 
  }

  /* Ouvrir le binaire */
  f = gel_open(argv[1], "../src", 0);
  if (f == NULL) {
    printf("erreur: %s\n", gel_strerror());
    exit(-1);
  }
  
  gel_file_infos(f, &fi);
  printf("Machine: %u\n", fi.machine);
  /* Charger binaire + librairies, donne une image */

  memset(&myenv, 0, sizeof(myenv));
     
   myenv = gel_default_env();
  myenv->flags = GEL_ENV_CLUSTERISED_MEMORY;
  myenv->pltblocksize = 4;
  myenv->pltblock = "\x12\x34\x56\x78";
  myenv->libpath = libs_path;
  im = gel_image_load(f, myenv, GEL_IMAGE_PLTBLOCK_NOW);
  if (im == NULL) {
    printf("erreur image_load: %s\n", gel_strerror());
    exit(-1);
  }
  
  /* Recuperer infos images pour afficher fichiers membres */
 gel_image_infos(im, &ii);
  printf("\nAffichage des infos sur l'image chargee\n Nombre de blocs: %u\n", ii.membersnum);
  for (i = 0; i < ii.membersnum; i++){
    gel_block_infos(ii.members[i], &bi);
    
    printf("\n--- Bloc %u : ", i+1);
    if (bi.container != NULL) {
      gel_file_infos(bi.container, &fi);
      printf("Fichier %s ---\n", fi.filename);  
    } else printf("Segment independant ---\n");
    printf("\n Adresse virtuelle de debut: 0x%08x\n", bi.vreloc);
    printf("Adresse de base virtuelle: 0x%08x\n", bi.base_vaddr);
  }
  
  printf("\n");
	if(ii.memory) {
		printf("affichage de la memoire clusteursee\n");
		/* affichage de l'image memoire clusterisee */
		gel_memory_image_infos(ii.memory, &mem_info);
		printf("Nombre de clusters : %d\n",mem_info.nb_clusters);
		for (i = 0; i < mem_info.nb_clusters ; i++) {
			gel_memory_cluster_infos(mem_info.clusters[i], &cluster_info);
			printf("\n-- Cluster n�%d --\n\n",i);
			printf("adresse virtuelle de debut : 0x%08x\n",cluster_info.vaddr);
			printf("adresse relle de debut : 0x%p\n", cluster_info.raddr);
			printf("Taille du cluster : 0x%08x\n\n",cluster_info.size);
		}
	}
  
  printf("\n");
  
  /* Pour tester: ecrire dans .text avec le curseur */
  printf("Recherche de la section .text\n");
  s = gel_getsectbyname(f, ".text");
  if (s == NULL) {
    printf("erreur: %s\n", gel_strerror());
    exit(-1);
  }
  printf("section trouvee\n");
  if (gel_sect2cursor(s,&c2) == -1) {
     printf("erreur: %s\n", gel_strerror());
     exit(-1);
  }

  gel_sect_infos(s, &si);
  printf("ecriture de 2 octets dans la section: 0xCAFE\n");
  gel_move(&c2, 0);
  gel_write_u16(c2, 0xcafe);

  
  /* Recherche/affichage d'un symbole global */
  
  /* Affichage du contenu de la section .text */
  
  gel_move(&c2, 0);
  printf("Lecture de la section .text: (size: %u): ", (int)si.size);
  for (i = 0; i < si.size; i++) {
    if (!(i % 25)) printf("\n");
    printf("%02x ", gel_read_u8(c2)); 
  }
  
  printf("\nFin.\n");

  printf("Listage des symboles globaux: \n");
  en = gel_enum_glob_symbol(im);
  if (en != NULL) {
    gel_enum_initpos(en);
    while ((str = gel_enum_next(en)) != NULL) {
      printf("%s ", str);
    }
  } else printf("erreur lors de la recuperation des symboles globaux\n");
  printf("\n----------------------------\n");
  
  gel_enum_free(en);

  printf("\nRecherche du symbole global: %s\n","__libc_start_main");
  sym = gel_find_glob_symbol(im, "__libc_start_main");
  if (sym == NULL) {
    printf("Symbole pas trouve\n");
  } else {
    gel_sym_infos(sym, &syminfo);
    if (syminfo.container != NULL) {
      gel_file_infos(syminfo.container, &fi);
      printf("Symbole trouve dans fichier: %s\n", fi.filename);
    } else printf("Le fichier dans lequel etait le symbole a ete ferme\n");
    gel_block2cursor(syminfo.blockcont, &c);
    printf("NOM: %s   ADRESSE VIRTUELLE: 0x%08x \n", syminfo.name, syminfo.vaddr);
    gel_move_abs(&c, syminfo.raddr);
    printf("raddr: %x\n", (int)syminfo.raddr);
    if (syminfo.raddr != NULL)
      printf("ADRESSE REELLE: 0x%08x , Valeur du symbole: %x\n", (u32_t) syminfo.raddr, gel_read_u32(c));
  }

  printf("\nRecherche du symbole local: %s\n", "main");
  sym = gel_find_file_symbol(f, "main");
  if (sym == NULL) {
    printf("Symbole pas trouve\n");
  } else {
    gel_sym_infos(sym, &syminfo);
    if (syminfo.container != NULL) {
      gel_file_infos(syminfo.container, &fi);
      printf("Symbole trouve dans fichier: %s\n", fi.filename);
    } else printf("Le fichier dans lequel etait le symbole a ete ferme\n");
    gel_block2cursor(syminfo.blockcont, &c);
    printf("NOM: %s   ADRESSE VIRTUELLE: 0x%08x \n", syminfo.name, syminfo.vaddr);
    gel_move_abs(&c, syminfo.raddr);
    if (syminfo.raddr != NULL) 
      printf("ADRESSE REELLE: 0x%08x , Valeur du symbole: %x\n", (u32_t) syminfo.raddr, gel_read_u32(c));
  }

	/* .plt section */
	s = gel_getsectbyname(f, ".plt");
	if(s == NULL)
		fprintf(stderr, "no .plt section !\n");
	else {
		printf("Affichage de la section: .plt (apres relocation)\n");
		gel_file_infos(f, &fi);
		gel_sect_infos(s, &si);
		gel_block2cursor(fi.blocks[0], &c);
		gel_block_infos(fi.blocks[0], &bi);
		gel_move_abs(&c, GEL_VADDR2RADDR(bi, si.vaddr));
		printf("taille sect: %u\n", (int)si.size);
		for (i = 0; i < si.size; i+=4) {
	    	printf("%08x: ", i + si.vaddr);
	    	printf("%02x ", gel_read_u8(c));
	    	printf("%02x ", gel_read_u8(c));
	    	printf("%02x ", gel_read_u8(c));
	    	printf("%02x\n", gel_read_u8(c));
	  	}
	}

	/* .got section */
	s = gel_getsectbyname(f, ".got");
	if(s == NULL)
		fprintf(stderr, "no .got section !\n");
	else {
		printf("Affichage de la section: .got (apres relocation)\n");
		gel_file_infos(f, &fi);
		gel_sect_infos(s, &si);
		gel_block2cursor(fi.blocks[0], &c);
		gel_block_infos(fi.blocks[0], &bi);
		gel_move_abs(&c, GEL_VADDR2RADDR(bi, si.vaddr));
		printf("taille sect: %u\n", (int)si.size);
		for (i = 0; i < si.size; i+=4) {
			printf("%08x: ", i + si.vaddr);
			printf("%02x ", gel_read_u8(c));
			printf("%02x ", gel_read_u8(c));
			printf("%02x ", gel_read_u8(c));
			printf("%02x\n", gel_read_u8(c));
		}
	}

  printf("Fermeture de l'image.\n");
  free(myenv);
  gel_image_close(im);
  gel_close(f);
  return(0);
}
