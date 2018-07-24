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
#ifndef GEL_ELF_H
#define GEL_ELF_H

#include <gel/common.h>
#include <gel/gel.h>

__BEGIN_DECLS

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4

 typedef vaddr_t Elf32_Addr;
 typedef u16_t Elf32_Half;
 typedef u32_t Elf32_Off;
 typedef s32_t Elf32_Sword;
 typedef u32_t Elf32_Word;

/* ELF header identification */
#define	EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define EI_CLASS	4
#define EI_DATA		5
#define EI_VERSION	6
#define EI_PAD		7
#define EI_OSABI	7
#define EI_NIDENT	16

 /* ELF identification */
#define	ELFMAG0 0x7f
#define ELFMAG1 'E'
#define	ELFMAG2 'L'
#define ELFMAG3 'F'

/* File classes */
#define	ELFCLASSNONE	0
#define	ELFCLASS32		1
#define	ELFCLASS64		2


/* File data encoding */
#define ELFDATANONE	0
#define ELFDATA2LSB	1
#define ELFDATA2MSB 2


/* Versions */
#define EV_NONE		0
#define EV_CURRENT	1


 /* auxiliairy vector types */
 #define AT_NULL			0
 #define AT_IGNORE		1
 #define AT_EXECFD		2
 #define AT_PHDR			3
 #define AT_PHENT		4
 #define AT_PHNUM		5
 #define AT_PAGESZ		6
 #define AT_BASE			7
 #define AT_FLAGS		8
 #define AT_ENTRY		9
 #define AT_DCACHEBSIZE	10
 #define AT_ICACHEBSIZE	11
 #define AT_UCACHEBSIZE	12


typedef struct {
        unsigned char e_ident[EI_NIDENT];
        Elf32_Half    e_type;
        Elf32_Half    e_machine;
        Elf32_Word    e_version;
        Elf32_Addr    e_entry;
        Elf32_Off     e_phoff;
        Elf32_Off     e_shoff;
        Elf32_Word    e_flags;
        Elf32_Half    e_ehsize;
        Elf32_Half    e_phentsize;
        Elf32_Half    e_phnum;
        Elf32_Half    e_shentsize;
        Elf32_Half    e_shnum;
        Elf32_Half    e_shstrndx;
} Elf32_Ehdr;

/* Section Header */
typedef struct {
        Elf32_Word sh_name;
        Elf32_Word sh_type;
        Elf32_Word sh_flags;
        Elf32_Addr sh_addr;
        Elf32_Off  sh_offset;
        Elf32_Word sh_size;
        Elf32_Word sh_link;
        Elf32_Word sh_info;
        Elf32_Word sh_addralign;
        Elf32_Word sh_entsize;
} Elf32_Shdr;

/* sh_type */
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff

/* sh_flags */
#define SHF_WRITE		0x00000001
#define SHF_ALLOC		0x00000002
#define SHF_EXECINSTR	0x00000004
#define SHF_MASKPROC	0xf0000000

/**
 * Elf file structure of a program header.
 * @ingroup		phdr
 */
typedef struct {
        Elf32_Word p_type;		/**< Type of the program header. */
        Elf32_Off  p_offset;	/**< Offset in the file. */
        Elf32_Addr p_vaddr;		/**< Virtual address in host memory. */
        Elf32_Addr p_paddr;		/**< Physical address in host memory. */
        Elf32_Word p_filesz;	/**< Size in the file. */
        Elf32_Word p_memsz;		/**< Size in host memory */
        Elf32_Word p_flags;		/**< Flags: PF_X, PF_W and and PF_R. */
        Elf32_Word p_align;		/**< Alignment in power of two (2**n). */
} Elf32_Phdr;

#define DT_NULL		0
#define DT_NEEDED 	1
#define DT_STRTAB	5

/* Elf32_Phdr p_flags */
#define PF_X		(1 << 0)
#define PF_W		(1 << 1)
#define PF_R		(1 << 2)
#define PF_MASKOS	0x0ff00000
#define PF_MASKPROC	0xf0000000

/* Legal values for note segment descriptor types for core files. */
typedef struct {
  Elf32_Sword d_tag;
  union {
    Elf32_Word d_val;
    Elf32_Addr d_ptr;
  } d_un;
} Elf32_Dyn;


/* ELF32_Sym */
typedef struct {
  Elf32_Word	st_name;
  Elf32_Addr	st_value;
  Elf32_Word	st_size;
  unsigned char	st_info;
  unsigned char st_other;
  Elf32_Half	st_shndx;
} Elf32_Sym;

#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i) & 0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4


#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff


typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
	Elf32_Sword r_addend;
} Elf32_Rela;

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))

#define SHN_UNDEF 0
#define SHN_LORESERVE 0x0000FF00
#define SHN_HIRESERVE 0x0000FFFF
#define SHN_ABS 0x0000FFF1
#define SHN_COMMON 0x0000FFF2

__END_DECLS

#endif	/* GEL_ELF_H */


