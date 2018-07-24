#ifndef BYTESWAP_H
#define BYTESWAP_H 1

#if defined(__i386__)
#define LITTLE_ENDIAN 1
#elif defined(__x86_64__)
#define LITTLE_ENDIAN 1
#else
#error endian not defined
#endif

#define bswap_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#define bswap_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

static inline grub_efi_uint16_t htons(grub_efi_uint16_t hostshort)
{
#ifdef LITTLE_ENDIAN
	return bswap_16(hostshort);
#else
	return hostshort;
#endif
}

static inline grub_efi_uint32_t htonl(grub_efi_uint32_t hostshort)
{
#ifdef LITTLE_ENDIAN
	return bswap_32(hostshort);
#else
	return hostshort;
#endif
}

#endif /* BYTESWAP_H */
