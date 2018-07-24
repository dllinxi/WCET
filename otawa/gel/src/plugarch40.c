#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <gel/error.h>
#include <gel/gel_plugin.h>
#include <gel/gel_elf.h>
#include "gel_mem.h"


/**
#define FORMER_VALUE_AS_ADDEND 1
*/

extern int gel_errno;

/* ensemble des relocations ARM */

#define R_ARM_PC24 1
#define R_ARM_ABS32 2
#define R_ARM_BREL_ADJ 12
#define R_ARM_SWI24 13
#define R_ARM_THM_SWI8 14
#define R_ARM_XPC25 15
#define R_ARM_THM_XPC22 16
#define R_ARM_TLS_DTPMOD32 17
#define R_ARM_TLS_DTPOFF32 18
#define R_ARM_TLS_TPOFF32 19
#define R_ARM_COPY 20
#define R_ARM_GLOB_DAT 21
#define R_ARM_JUMP_SLOT 22
#define R_ARM_RELATIVE 23
#define R_ARM_PLT32 27
#define R_ARM_ALU_PCREL_7_0 32
#define R_ARM_ALU_PCREL_15_8 33
#define R_ARM_ALU_PCREL_23_15 34
#define R_ARM_LDR_SBREL_11_0_NC 35
#define R_ARM_ALU_SBREL_19_12_NC 36
#define R_ARM_ALU_SBREL_27_20_CK 37
#define R_ARM_SBREL31 39
#define R_ARM_GNU_VTENTRY 100
#define R_ARM_GNU_VTINHERIT 101

#define USESYM ((symi.sect == SHN_UNDEF) ? (symi2) : (symi))

/**
 * Fonction permettant de recuperer une entree de relocation de type RELA
 *
 * @param c Le curseur pointant sur l'entree
 * @param rela La structure permettant de recevoir l'entree
 */

static void rela_get(gel_cursor_t *c, Elf32_Rela *rela) {
  rela->r_offset = gel_read_u32(*c);
  rela->r_info = gel_read_u32(*c);
  rela->r_addend = gel_read_s32(*c);
}

/**
 * Fonction permettant de recuperer une entree de relocation de type REL
 *
 * @param c Le curseur pointant sur l'entree
 * @param rela La structure permettant de recevoir l'entree
 */
static void rel_get(gel_cursor_t *c, Elf32_Rel *rel) {
  rel->r_offset = gel_read_u32(*c);
  rel->r_info = gel_read_u32(*c);
}

/**
 * Realise la relocation
 *
 * @param im L'image subissant la relocation
 * @param h La table de hachage (symbole,curseur) qui definit les adresse
sourc
 * @param flags Flags
 * @param do_copy Numero de passe (0 == normal, 1 == faire les R_xxx_COPY)
 * @return Code d'erreur
 */
int real_do_reloc(gel_image_t *im, gel_hash_t h, int flags, int do_copy) {
  gel_image_info_t ii; /**< This image's info */
  gel_block_info_t bi; /**< Current block's info */
  gel_block_info_t sbi; /**< Symbol block's info */
  gel_file_info_t fi; /**< Container file's info */
  gel_sect_t *s; /**< Current section */
  gel_sect_t *relsect; /**< Section being relocated */
  gel_sect_info_t si; /**< Current section's info */
  gel_sect_t *symtab; /**< Symtab section of the current object */
  gel_sect_info_t relsi; /**< Info of the section defining the symbol */
  gel_cursor_t c,bc; /**< rel sect. cursor / block cursor / block source cursor */
  gel_sym_t *s1, *s2; /**< Symbol used by the relocation */
  gel_sym_info_t symi, symi2; /**< Sym info for s1 and s2 */
  Elf32_Rela rela; /**< Current RELA entry */
  Elf32_Rel rel;
  vaddr_t T,A,B,P,S; /**< Relocation values */
  int i,j,k,l;
  s32_t res; /**< Storage for reloc. calculations (32 bits) */
  s8_t res8; /**< Idem 8bits */
  gel_cursor_t *copy;

  gel_image_infos(im, &ii);
  /* For each image member */
  for (i = 0; i < ii.membersnum; i++) {

    gel_block_infos(ii.members[i], &bi);
    if (bi.container == NULL)
      continue; /* stack block */
    gel_file_infos(bi.container, &fi);

    /* BC = block cursor */
    if (gel_block2cursor(ii.members[i], &bc) == -1)
      return -1;

    /* Browse member's sections */
    for (j = 0; j < fi.sectnum; j++) {
      s = gel_getsectbyidx(bi.container, j);

      if (s == NULL)
        return -1;
      gel_sect_infos(s, &si);

      if ((si.type == SHT_RELA) || (si.type == SHT_REL)) {

        /** C == relocation-section cursor */
        if (gel_sect2cursor(s, &c) == -1)
          return -1;

        /* Get symtab associated with rel section */
        if (si.link >= fi.sectnum) {
          gel_errno = GEL_EFORMAT;
          return -1;
        }
        symtab = gel_getsectbyidx(bi.container, si.link);
        if (symtab == NULL)
          return -1;

        /* Get section concerned by the relocation */
        if (si.info >= fi.sectnum) {
          gel_errno = GEL_EFORMAT;
          return -1;
        }
        if (si.info != SHN_UNDEF) {
          relsect = gel_getsectbyidx(bi.container, si.info);
          if (relsect == NULL) {
              return -1;
          }
          gel_sect_infos(relsect, &relsi);
        } else relsect = NULL;


        /* Browse relocation entries */
        for (k = 0; k < si.size; k += sizeof(Elf32_Rela)) {
          /* if REL, then addend = 0 */
          if (si.type == SHT_REL) {
            rel_get(&c, &rel);
            rela.r_offset = rel.r_offset;
            rela.r_addend = 0;
            rela.r_info = rel.r_info;
          } else {
              rela_get(&c,&rela);
          }

          if ((ELF32_R_TYPE(rela.r_info) != R_ARM_COPY) && do_copy)
              continue;

          /* Get symbol associated with relocation entry */
          if (ELF32_R_SYM(rela.r_info) != 0) {
            /* not STN_UNDEF */
            s1 = gel_symbyidx(symtab, ELF32_R_SYM(rela.r_info));
            if (s1 == NULL)
              return -1;
            gel_sym_infos(s1, &symi);

            s2 = gel_find_glob_symbol(im, symi.name);
            if (s2 != NULL) {
              gel_sym_infos(s2, &symi2);
            } else symi2.sect = SHN_UNDEF;

            if ((symi.sect == SHN_UNDEF) && ((s2 == NULL) || (symi2.sect == SHN_UNDEF))) {
              if (flags & (GEL_IMAGE_PLTBLOCK_NOW|GEL_IMAGE_PLTBLOCK_LAZY)) {
                if (ELF32_R_TYPE(rela.r_info) == R_ARM_JUMP_SLOT)
                  gel_write_block(&bc, gel_image_env(im)->pltblock, gel_image_env(im)->pltblocksize, VADDR2RADDR(&bi, rela.r_offset));
              } else {
                gel_errno = GEL_WNONFATAL;
                continue;
              }

            }

          } else {
            /* is STN_UNDEF */
            s1 = NULL;
            s2 = NULL;
            symi.vaddr = 0;
            symi.sect = SHN_ABS;
            symi2=symi;
          }

          if (hash_get(h, USESYM.name) != NULL) {
            /* this is the source of a COPY relocation, undef s1 */
            symi.sect = SHN_UNDEF;
          }

          assert(USESYM.blockcont != NULL);
          gel_block_infos(USESYM.blockcont, &sbi);

          P = VADDR2VRELOC(&bi, rela.r_offset);
          B = sbi.base_vaddr; /* Adresse de base du block contenant le symbole) */
          S = (USESYM.vaddr == 0) ? 0 : VADDR2VRELOC(&sbi, USESYM.vaddr);
          T = ((USESYM.vaddr & 1) && (ELF32_ST_TYPE(USESYM.info) == STT_FUNC)) ? 1 : 0;


          gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
          if (gel_cursor_bounds(&bc) == -1) {
#ifndef NDEBUG
            printf("Error: cursor bounds\n");
#endif
            gel_errno = GEL_EFORMAT;
            delete(s1);
            return -1;
          }

          /* read addend from old value */
#ifdef FORMER_VALUE_AS_ADDEND
          rela.r_addend = gel_read_s32(bc);
#endif
          gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
          A = rela.r_addend;

          switch(ELF32_R_TYPE(rela.r_info)) {
              case R_ARM_ABS32:
                res = (S + A) | T;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;
              case R_ARM_BREL_ADJ:
                res = (S-B) + A;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;
/*
              case R_ARM_TLS_DTPMOD32:
                res = ;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;
              case R_ARM_TLS_DTPOFF32:
                res = ;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;
              case R_ARM_TLS_TPOFF32:
                res = ;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;
*/
              case R_ARM_GLOB_DAT:
                res = (S + A) | T;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;
              case R_ARM_JUMP_SLOT:

                if (flags & GEL_IMAGE_PLTBLOCK_LAZY) {
                  gel_write_block(&bc, gel_image_env(im)->pltblock, gel_image_env(im)->pltblocksize, VRELOC2RADDR(&bi, P));
                  break;
                }

                res = (S + A) | T;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;
              case R_ARM_RELATIVE:
                res = B + A;
                gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
                gel_write_s32(bc, res);
                break;

              case R_ARM_COPY:
                if (!do_copy) {
                  /* first pass: add copied symbols to hashtable */
                  copy = new(gel_cursor_t);
                  if (copy == NULL) {
                    gel_errno = GEL_ERESOURCE;
                    delete(s1);
                    return -1;
                  }
                  if ((s2 == NULL) || (symi2.sect == SHN_UNDEF)) {
#ifndef NDEBUG
                    printf("Unresolved COPY relocation: %s\n", symi.name);
#endif
                    gel_errno = GEL_EFORMAT;
                    delete(s1);
                    return -1;
                  }
                  if (gel_block2cursor(symi2.blockcont, copy) == -1) {
                    delete(s1);
                    return -1;
                  }
                  gel_move_abs(copy, VADDR2RADDR(symi2.blockcont, symi2.vaddr));
                  hash_put(h, mystrdup(symi.name), copy);
                  gel_replacesym(im, symi.name, s1);
                  break;
                }
                /* second pass: copy symbols data */
                copy = hash_get(h, symi.name);
                assert(copy != NULL);

                gel_move_abs(&bc, VRELOC2RADDR(&bi, P));

                for (l = 0; l < symi2.size; l++) {

                  if (gel_cursor_bounds(copy) == -1) {
#ifndef NDEBUG
                    printf("error: cursor bounds\n");
#endif
                    gel_errno = GEL_EFORMAT;
                    delete(s1);
                    return -1;
                  }
                  res8 = gel_read_s8(*copy);

                  if (gel_cursor_bounds(&bc) == -1) {
                    gel_errno = GEL_EFORMAT;
                    delete(s1);
                    return -1;
                  }
                  gel_write_s8(bc, res8);
                }
                break;
              default:
#ifndef NDEBUG
                printf("Error: unknown reloc: %u\n", ELF32_R_TYPE(rela.r_info));
#endif
                gel_errno = GEL_EFORMAT;
                delete(s1);
                return -1;
            }
            delete(s1);
        } /* end of this relocation entry*/
      }
    } /* end of this relocation section */
  } /* end of this member file */
  return 0;
}

int do_reloc(gel_image_t *im, int flags) {
  int r;
  gel_hash_t h;
  h = gel_hash_new(211);
  if (h == NULL) {
    gel_errno = GEL_ERESOURCE;
    return -1;
  }
  r = real_do_reloc(im, h, flags, 0);
  if (r == -1)
    return r;
  r = real_do_reloc(im, h, flags, 1);
  hash_free(h);
  return r;
}
arch_plugin_t plugin_arch = {
  40,
  1,
  2,	/* stack alignment */
  NULL,
  do_reloc,
  1,
  4096,
};

extern sys_plugin_t null_plugin_sys;
sys_plugin_t *plugin_sys = &null_plugin_sys;
