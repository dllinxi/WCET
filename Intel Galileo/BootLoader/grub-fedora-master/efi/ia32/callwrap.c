
#define ENTRY(name)		\
	".globl " #name ";"	\
	".align 16;"		\
	#name ":"

asm(
ENTRY(i386_call0)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$8, %esp	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);

asm(
ENTRY(i386_call1)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$20, %esp	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);

asm(
ENTRY(i386_call2)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$16, %esp	\n"
	"pushl	16(%ebp)	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);

asm(
ENTRY(i386_call3)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$12, %esp	\n"
	"pushl	20(%ebp)	\n"
	"pushl	16(%ebp)	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);

asm(
ENTRY(i386_call4)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$8, %esp	\n"
	"pushl	24(%ebp)	\n"
	"pushl	20(%ebp)	\n"
	"pushl	16(%ebp)	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);

asm(
ENTRY(i386_call5)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$20, %esp	\n"
	"pushl	28(%ebp)	\n"
	"pushl	24(%ebp)	\n"
	"pushl	20(%ebp)	\n"
	"pushl	16(%ebp)	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);

asm(
ENTRY(i386_64_call5)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$12, %esp	\n"
	"pushl	36(%ebp)	\n"
	"pushl	32(%ebp)	\n"
	"pushl	28(%ebp)	\n"
	"pushl	24(%ebp)	\n"
	"pushl	20(%ebp)	\n"
	"pushl	16(%ebp)	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);


asm(
ENTRY(i386_call6)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$16, %esp	\n"
	"pushl	32(%ebp)	\n"
	"pushl	28(%ebp)	\n"
	"pushl	24(%ebp)	\n"
	"pushl	20(%ebp)	\n"
	"pushl	16(%ebp)	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);

asm(
ENTRY(i386_call7)
	"pushl	%ebp		\n"
	"movl	%esp,%ebp	\n"
	"subl	$12, %esp	\n"
	"pushl	36(%ebp)	\n"
	"pushl	32(%ebp)	\n"
	"pushl	28(%ebp)	\n"
	"pushl	24(%ebp)	\n"
	"pushl	20(%ebp)	\n"
	"pushl	16(%ebp)	\n"
	"pushl	12(%ebp)	\n"
	"call	*8(%ebp)	\n"
	"leave			\n"
	"ret			\n"
	);
