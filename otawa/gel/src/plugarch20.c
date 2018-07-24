/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-08, IRIT- UPS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gel/error.h>
#include <gel/gel_plugin.h>
#include <gel/gel_elf.h>
#include "gel_mem.h"

extern int gel_errno;

/* PPC relocation */
#define R_PPC_NONE 0
#define R_PPC_ADDR32 1
#define R_PPC_ADDR24 2
#define R_PPC_ADDR16 3
#define R_PPC_ADDR16_LO 4
#define R_PPC_ADDR16_HI 5
#define R_PPC_ADDR16_HA 6
#define R_PPC_ADDR14 7
#define R_PPC_ADDR14_BRTAKEN 8
#define R_PPC_ADDR14_BRNTAKEN 9
#define R_PPC_REL24 10
#define R_PPC_REL14 11
#define R_PPC_REL14_BRTAKEN 12
#define R_PPC_REL14_BRNTAKEN 13
#define R_PPC_GOT16 14
#define R_PPC_GOT16_LO 15
#define R_PPC_GOT16_HI 16
#define R_PPC_GOT16_HA 17
#define R_PPC_PLTREL24 18
#define R_PPC_COPY 19
#define R_PPC_GLOB_DAT 20
#define R_PPC_JMP_SLOT 21
#define R_PPC_RELATIVE 22
#define R_PPC_LOCAL24PC 23
#define R_PPC_UADDR32 24
#define R_PPC_UADDR16 25
#define R_PPC_REL32 26
#define R_PPC_PLT32 27
#define R_PPC_PLTREL32 28
#define R_PPC_PLT16_LO 29
#define R_PPC_PLT16_HI 30
#define R_PPC_PLT16_HA 31
#define R_PPC_SDAREL16 32
#define R_PPC_SECTOFF 33
#define R_PPC_SECTOFF_LO 34
#define R_PPC_SECTOFF_HI 35
#define R_PPC_SECTOFF_HA 36
#define R_PPC_ADDR30 37

#define LO(x)  ((x) & 0xFFFF)
#define HI(x)  (((x) >> 16) & 0xFFFF)
#define HA(x)  ((((x) >> 16) + (((x) & 0x8000) ? 1 : 0)) & 0xFFFF)

/* fast access macro */
#define USESYM ((symi.sect == SHN_UNDEF) ? (symi2) : (symi))

/* PLT building constants */
#define PLT_PLTCALL 24
#define PLT_RESERVE 72
#define PLT_REL_PLTCALL_HA (24 + 2)
#define PLT_REL_PLTCALL_LO (24 + 2 + 4)

char plt_prefix[]=
	/* reserved bytes */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	/* PLT call begin */
	 "\x3d\x6b\x12\x34"  /* addis r11,r11,0x1234 */
	 "\x81\x6b\x56\x78"  /* lwz   r11,0x5678(r11) */
	 "\x7d\x69\x03\xa6"  /* mtctr r11 */
	 "\x4e\x80\x04\x20"  /* bctr */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
	 "\x60\x00\x00\x00"  /* nop */
;
#define PLT_ENTSIZE 8
#define PLT_REL_PLTCALL 4
char plt_entry[]=
	"\x39\x60\x00\x00"  /* li    r11,0 */
	/* "\x48\x00\x00\x00"   b     0x00 */
;



/**
 * Retrive a relocation entry.
 * @param c 	Pointer to the entry.
 * @param rela 	Returned entry descriptor.
 */
static void rela_get(gel_cursor_t *c, Elf32_Rela *rela) {
  rela->r_offset = gel_read_u32(*c);
  rela->r_info = gel_read_u32(*c);
  rela->r_addend = gel_read_s32(*c);
}

/**
 * Build the PLT.
 * @param s PLT section.
 * @return	Error code.
 */
int build_plt(gel_sect_t *s) {
	gel_sect_info_t si;
	gel_file_info_t fi;
	gel_cursor_t c;
	vaddr_t plttable;
	int i,j;
	int num;

	/* get the block matching the section */
	gel_sect_infos(s, &si);
	gel_file_infos(si.container, &fi);
	for(i = 0; i < fi.blknum; i++)
		if(fi.blocks[i]->exec_begin <= si.vaddr
		&& si.vaddr < fi.blocks[i]->exec_begin + fi.blocks[i]->exec_size)
			break;
	assert(i < fi.blknum);
	if (gel_block2cursor(fi.blocks[i], &c) == -1)
		return -1;
	gel_move_abs(&c, VADDR2RADDR(fi.blocks[i], si.vaddr));

	/* Compute the max PLT size */
	num = (si.size - PLT_RESERVE) / 12;
	if (num <= 0)  {
		gel_errno = GEL_EFORMAT;
		return -1;
	}

	/* insert the pefix in the PLT */
	for (i = 0; i < PLT_RESERVE; i++)
		gel_write_u8(c, plt_prefix[i]);

	/* insert entries */
	for (i = 0; i < num; i++) {
		for (j = 0; j < 4; j++) {
			gel_write_u8(c, plt_entry[j]);
		}
		/* insert relative branch to the .PLTcall */
		gel_write_u32(c, 0x48000000 | ((PLT_PLTCALL - (PLT_RESERVE + i*PLT_ENTSIZE + 4)) & 0x03FFFFFF));
	}

	/* initialize the PLT-table (written by the R_ARM_JMP_SLOT relocations) */
	for (i = 0; i < num; i++) {
		gel_write_u32(c, 0);
	}

	/* relocation of .PLTtable */
	plttable = si.vaddr + PLT_RESERVE + PLT_ENTSIZE*num;
	gel_move(&c, PLT_REL_PLTCALL_HA);
	gel_write_s16(c, HA(plttable));
	gel_move(&c, PLT_REL_PLTCALL_LO);
	gel_write_s16(c, LO(plttable));
	return 0;
}

/**
 * Perform the relocation
 * @param im 		Image to work on.
 * @param h 		Hashing table (symbol, cursor) defining sources address of
 * 					relocations R_xxx_COPY
 * @param flags 	Flags
 * @param do_copy	Pass number (0 == normal, 1 == do R_xxx_COPY)
 * @return 			Error code.
 */
int real_do_reloc(gel_image_t *im, gel_hash_t h, int flags, int do_copy)  {
	gel_image_info_t ii;		/**< This image's info */
	gel_block_info_t bi;		/**< Current block's info */
	gel_block_info_t sbi;		/**< Symbol block's info */
	gel_file_info_t fi;			/**< Container file's info */
	gel_sect_t *s;				/**< Current section */
	gel_sect_t *relsect;		/**< Section being relocated */
	gel_sect_info_t si;			/**< Current section's info */
	gel_sect_t *symtab;			/**< Symtab section of the current object */
	gel_sect_info_t relsi;		/**< Info of the section defining the symbol */
	gel_cursor_t c,bc;			/**< rel sect. cursor / block cursor / block source cursor */
	gel_sym_t *s1, *s2;			/**< Symbol used by the relocation */
	gel_sym_info_t symi, symi2;	/**< Sym info for s1 and s2 */
	Elf32_Rela rela;			/**< Current RELA entry */
	vaddr_t A,B,L,P,R,S;		/**< Relocation values */
	s32_t diff;
	int i,j,k,l;
	s32_t res;					/**< Temp. storage for reloc. calculations (32 bits) */
	s16_t res16;				/**< Idem 16bits */
	s8_t res8;					/**< Idem 8bits */
	vaddr_t sda_base;			/**< Location of Small Data Area (PowerPC) */
	gel_cursor_t *copy;
	int plt_built = 0;
	int num, pltentry;

	/* For each image member */
	gel_image_infos(im, &ii);
	for (i = 0; i < ii.membersnum; i++) {
		plt_built = 0;

		gel_block_infos(ii.members[i], &bi);
		if (bi.container == NULL)
			continue; /* Non-file blocks can't have a relocation section */

		gel_file_infos(bi.container, &fi);

		/* get SDA_BASE (see ELF specs for PowerPC) */
		sda_base = 0;
		s1 = gel_find_file_symbol(bi.container,"_SDA_BASE_");
		if (s1 != NULL) {
			gel_sym_infos(s1, &symi);
			if (symi.sect != SHN_UNDEF) {
				sda_base = (sda_base == 0) ? 0 : VADDR2VRELOC(&bi, symi.vaddr);
			}
		}

		/* BC = block cursor */
		if (gel_block2cursor(ii.members[i], &bc) == -1) {
			return -1;
		}

		/* Browse member's sections */
		for (j = 0; j < fi.sectnum; j++) {
			s = gel_getsectbyidx(bi.container, j);
			if (s == NULL) {
				return -1;
			}
			gel_sect_infos(s, &si);

			if (si.type == SHT_RELA) {
				/* c = Section Cursor */
				if (gel_sect2cursor(s, &c) == -1) {
					return -1;
				}

				/* Get symtab associated with rel section */
				if (si.link >= fi.sectnum)  {
					gel_errno = GEL_EFORMAT;
					return -1;
				}

				symtab = gel_getsectbyidx(bi.container, si.link);
				if (symtab == NULL) {
					return -1;
				}

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
				}
				else {
					relsect = NULL;
				}

				/* Browse relocation entries */
				for (k = 0; k < si.size; k += sizeof(Elf32_Rela))  {
					rela_get(&c, &rela);
					if ((ELF32_R_TYPE(rela.r_info) != R_PPC_COPY) && do_copy) {
						continue;
					}

					if (ELF32_R_TYPE(rela.r_info) == R_PPC_JMP_SLOT)  {
						if ((!plt_built) && (relsect != NULL)) {
							if (build_plt(relsect) == -1)
							{
								return -1;
							}
							plt_built = 1;
						}
					}


					/* Get symbol associated with relocation entry */
					if (ELF32_R_SYM(rela.r_info) != 0)
					{
						s1 = gel_symbyidx(symtab, ELF32_R_SYM(rela.r_info));
						if (s1 == NULL)
						{
							return -1;
						}

						gel_sym_infos(s1, &symi);

						s2 = gel_find_glob_symbol(im, symi.name);
						if (s2 != NULL)
						{
							gel_sym_infos(s2, &symi2);
						}

						if ((symi.sect == SHN_UNDEF) && ((s2 == NULL) || (symi2.sect == SHN_UNDEF)))
						{
							if (flags & (GEL_IMAGE_PLTBLOCK_NOW|GEL_IMAGE_PLTBLOCK_LAZY))
							{
								if (ELF32_R_TYPE(rela.r_info) == R_PPC_JMP_SLOT)
								{
									gel_write_block(&bc, gel_image_env(im)->pltblock, gel_image_env(im)->pltblocksize, VADDR2RADDR(&bi, rela.r_offset));
								}
							}
							else
							{
								gel_errno = GEL_WNONFATAL;
							}
							/* skip unresolved symbol */
							delete(s1);
							continue;
						}
					}
					else
					{
						/* STN_UNDEF */
						s1 = NULL;
						s2 = NULL;
						symi.vaddr = 0;
						symi.sect = SHN_ABS;
						symi2=symi;
					}


					if (hash_get(h, USESYM.name) != NULL)
					{
						/* Symbol index is source for R_xxx_COPY, do not use it */
						symi.sect = SHN_UNDEF;
					}

					/* A ce stade:
					 * - symi existe forcement (mais peut etre UNDEF)
					 * - USESYM existe forcement (et est forcement defini)
					 * - symi2 n'existe pas forcement (s2 peut etre NULL)
					 */
					assert(USESYM.blockcont != NULL);

					gel_block_infos(USESYM.blockcont, &sbi);

					A = rela.r_addend; /* addend */
					B = bi.base_vaddr; /* file base vaddr */
					/* cas special: reloc PLT32, voir spec ELF */
					L = (symi.vaddr == 0) ? 0 : VADDR2VRELOC(&bi, symi.vaddr); /* PLT entry: undef symbol's addr */
					P = VADDR2VRELOC(&bi, rela.r_offset);
					R = (relsect == NULL) ? 0 : (USESYM.vaddr - relsi.vaddr);
					S = (USESYM.vaddr == 0) ? 0 : VADDR2VRELOC(&sbi, USESYM.vaddr);

					gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
					if (gel_cursor_bounds(&bc) == -1)
					{
						gel_errno = GEL_EFORMAT;
						delete(s1);
						return -1;
					}

					switch(ELF32_R_TYPE(rela.r_info))
					{
					case R_PPC_ADDR32:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res = gel_read_s32(bc);
						res = S + A;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_ADDR24:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res = gel_read_s32(bc);
						res &= ~(0x04FFFFFC);
						res |= ((S + A) >> 2) << 2;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_ADDR16:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res16 = gel_read_s16(bc);
						res16 = (S + A) ;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_ADDR16_LO:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res16 = gel_read_s16(bc);
						res16 = LO(S + A) ;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_ADDR16_HI:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res16 = gel_read_s16(bc);
						res16 = HI(S + A) ;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_ADDR16_HA:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res16 = gel_read_s16(bc);
						res16 = HA(S + A) ;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_ADDR14:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res = gel_read_s32(bc);
						res &= ~(0x0000FFFC);
						res |= ((S + A) >> 2) << 2;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_ADDR14_BRTAKEN:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res = gel_read_s32(bc);
						res &= ~(0x0000FFFC);
						res |= 0x00200000;
						res |= ((S + A) >> 2) << 2;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_ADDR14_BRNTAKEN:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res = gel_read_s32(bc);
						res &= ~(0x0000FFFC);
						res &= ~(0x00200000);
						res |= ((S + A) >> 2) << 2;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_GOT16:
					case R_PPC_GOT16_LO:
					case R_PPC_GOT16_HI:
					case R_PPC_GOT16_HA:
					case R_PPC_LOCAL24PC:
						gel_errno = GEL_EFORMAT;
						delete(s1);
						return -1;

					case R_PPC_PLTREL24:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res = gel_read_s32(bc);
						res &= ~(0x04FFFFFC);
						res |= ((L + A - P) >> 2) << 2;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_COPY:
						if (!do_copy)
						{
							/* first pass: add copied symbols to hashtable */
							copy = new(gel_cursor_t);
							if (copy == NULL)
							{
								gel_errno = GEL_ERESOURCE;
								free(s1);
								return -1;
							}
							if ((s2 == NULL) || (symi2.sect == SHN_UNDEF))
							{
#                   ifndef NDEBUG
printf("Unresolved COPY relocation: %s\n", symi.name);
#                   endif
gel_errno = GEL_EFORMAT;
delete(s1);
return -1;
							}
							if (gel_block2cursor(symi2.blockcont, copy) == -1)
							{
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

						for (l = 0; l < symi2.size; l++)
						{

							if (gel_cursor_bounds(copy) == -1)
							{
								gel_errno = GEL_EFORMAT;
								delete(s1);
								return -1;
							}
							res8 = gel_read_s8(*copy);

							if (gel_cursor_bounds(&bc) == -1)
							{
								gel_errno = GEL_EFORMAT;
								delete(s1);
								return -1;
							}
							gel_write_s8(bc, res8);
						}
						break;


					case R_PPC_GLOB_DAT:
						res = S + A;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_JMP_SLOT:
						if (relsect == NULL)
						{
							gel_errno = GEL_EFORMAT;
							delete(s1);
							return -1;
						}
						if (flags & GEL_IMAGE_PLTBLOCK_LAZY)
						{
							gel_write_block(&bc, gel_image_env(im)->pltblock, gel_image_env(im)->pltblocksize, VRELOC2RADDR(&bi, P));
							break;
						}

						num = (relsi.size - PLT_RESERVE) / 12;
						pltentry = (VRELOC2VADDR(&bi, P) - PLT_RESERVE - relsi.vaddr) / PLT_ENTSIZE;

						if ((pltentry < 0) || (pltentry >= num))
						{
							gel_errno = GEL_EFORMAT;
							delete(s1);
							return -1;
						}
						P += 4; /* P now points to the "b" instruction of the PLT entry */
						diff = S - P;
						if ((diff > 0x01FFFFFF) || (diff < -0x02000000))
						{
							/* trop grand: indirection avec la PLTtable */
							gel_move_abs(&bc, VADDR2RADDR(&bi, relsi.vaddr + PLT_RESERVE + (num*PLT_ENTSIZE) + pltentry*4));
							gel_write_s32(bc, S + A);
						}
						else
						{
							/* ca va: ecriture directe du branchement relatif */
							gel_move_abs(&bc, VRELOC2RADDR(&bi, P));
							gel_write_s32(bc, ((S + A - P) & 0x03FFFFFF) | 0x48000000);
						}
						break;


					case R_PPC_RELATIVE:
						res = B + A;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_UADDR32:
						res = S + A;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_UADDR16:
						res16 = S + A;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_REL32:
						res = S + A - P;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_PLT32:
						res = L + A;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_PLTREL32:
						res = L + A - P;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					case R_PPC_PLT16_LO:
						res16 = LO(L+A);
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_PLT16_HI:
						res16 = HI(L + A);
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_PLT16_HA:
						res16 = HA(L + A);
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_SDAREL16:
						if (sda_base == 0)
						{
#                 ifndef NDEBUG
							fprintf(stderr, "Undefined reference to _SDA_BASE_\n");
#                 endif
							gel_errno = GEL_EFORMAT;
							delete(s1);
							return -1;
						}

						res16 = S + A - sda_base;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);


					case R_PPC_SECTOFF:
						res16 = R + A;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);

					case R_PPC_SECTOFF_LO:
						res16 = LO(R + A);
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);

					case R_PPC_SECTOFF_HI:
						res16 = HI(R + A);
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);

					case R_PPC_SECTOFF_HA:
						res16 = HA(R + A);
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s16(bc, res16);
						break;

					case R_PPC_ADDR30:
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						res = gel_read_s32(bc);
						res &= ~(0xFFFFFFFC);
						res |= ((S + A - P) >> 2) << 2;
						gel_move_abs(&bc, VRELOC2RADDR(&bi,P));
						gel_write_s32(bc, res);
						break;

					default:
						gel_errno = GEL_EFORMAT;
						delete(s1);
						return -1;
					} /* end of switch */
					delete(s1);
				} /* end of this relocation entry */
			}
		} /* end of this relocation section */
	} /* end of this member file */
	return 0;
} /* end of function */


/**
 * Perform the relocation.
 * @param im	Image to work on.
 * @param flags	Flags to use.
 * @return		Error code.
 */
int do_reloc(gel_image_t *im, int flags) {
	int r;
	gel_hash_t h;
	h = gel_hash_new(211);
	r = real_do_reloc(im, h, flags, 0);
	if (r == -1)
		return r;
	r = real_do_reloc(im, h, flags, 1);
	hash_free(h);
	return r;
}

/* PPC archictecture plugin */
arch_plugin_t plugin_arch = {
		20,
		1,
		4,			/* stack alignment */
		NULL,
		do_reloc,
		1,
		4096
};
extern sys_plugin_t null_plugin_sys;
sys_plugin_t *plugin_sys = &null_plugin_sys;
