	.file	"emtpy.c"
gcc2_compiled.:
	.section	.debug_abbrev
.Ldebug_abbrev0:
	.section	.text
.Ltext0:
	.section	.debug_info
.Ldebug_info0:
	.section	.debug_line
.Ldebug_line0:
	.section	".text"
	.align 2
	.globl f
	.type	 f,@function
f:
.LFB1:
.LM1:

	stwu 1,-16(1)
	stw 31,12(1)
	mr 31,1
.LM2:

	li 3,0
	b .L2
.LM3:

.L2:
	lwz 11,0(1)
	lwz 31,-4(11)
	mr 1,11
	blr
.LFE1:
.Lfe1:
	.size	 f,.Lfe1-f

	.section	.text
.Letext0:

	.section	.debug_line
	.4byte	0x3b
	.2byte	0x2
	.4byte	0x1b
	.byte	0x4
	.byte	0x1
	.byte	-10
	.byte	245
	.byte	10
	.byte	0x0
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x1
	.byte	0x0
	.string	"emtpy.c"

	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM2
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.LM3
	.byte	0x15
	.byte	0x0
	.byte	0x5
	.byte	0x2
	.4byte	.Letext0
	.byte	0x0
	.byte	0x1
	.byte	0x1

	.section	.debug_abbrev
	.byte	0x1
	.byte	0x11
	.byte	0x1
	.byte	0x3
	.byte	0x8
	.byte	0x1b
	.byte	0x8
	.byte	0x25
	.byte	0x8
	.byte	0x13
	.byte	0xb
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x1
	.byte	0x10
	.byte	0x6
	.byte	0,0
	.byte	0x2
	.byte	0x2e
	.byte	0x0
	.byte	0x3f
	.byte	0xc
	.byte	0x3
	.byte	0x8
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x27
	.byte	0xc
	.byte	0x49
	.byte	0x13
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x1
	.byte	0x40
	.byte	0xa
	.byte	0,0
	.byte	0x3
	.byte	0x24
	.byte	0x0
	.byte	0x3
	.byte	0x8
	.byte	0xb
	.byte	0xb
	.byte	0x3e
	.byte	0xb
	.byte	0,0
	.byte	0

	.section	.debug_info
	.4byte	0x79
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.byte	0x1
	.string	"emtpy.c"

	.string	"/home/casse/otawa/gel/test/bin"

	.string	"GNU C 2.95.3 20010315 (release)"

	.byte	0x1
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.byte	0x2
	.byte	0x1
	.string	"f"

	.byte	0x1
	.byte	0x1
	.byte	0x1
	.4byte	0x75
	.4byte	.LFB1
	.4byte	.LFE1
	.byte	0x1
	.byte	0x6f
	.byte	0x3
	.string	"int"

	.byte	0x4
	.byte	0x5
	.byte	0x0

	.section	.debug_pubnames
	.4byte	0x14
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x7d
	.4byte	0x60
	.string	"f"

	.4byte	0x0

	.section	.debug_aranges
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.4byte	0x4
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0x0
	.4byte	0x0
	.ident	"GCC: (GNU) 2.95.3 20010315 (release)"
