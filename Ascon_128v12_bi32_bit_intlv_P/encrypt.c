#include "api.h"
#include "endian.h"
#include "permutations.h"
#include "lightwieghtTIE.h"

#define RATE (128 / 8)
#define PA_ROUNDS 12
#define PB_ROUNDS 8
#define IV                                                        \
  ((u64)(8 * (CRYPTO_KEYBYTES)) << 56 | (u64)(8 * (RATE)) << 48 | \
   (u64)(PA_ROUNDS) << 40 | (u64)(PB_ROUNDS) << 32)

__attribute__((noinline)) int crypto_aead_encrypt(unsigned char* c, unsigned long long* clen,
                        const unsigned char* m, unsigned long long mlen,
                        const unsigned char* ad, unsigned long long adlen,
                        const unsigned char* nsec, const unsigned char* npub,
                        const unsigned char* k) {
  u32_2 K0, K1, N0, N1;
  u32_2 x0, x1, x2, x3, x4;
  u32_2 t0, t1, t2, t3, t4;
  u64 tmp0, tmp1;
  u32 i;
  (void)nsec;

  // set ciphertext size
  *clen = mlen + CRYPTO_ABYTES;

  // load key and nonce
  to_bit_interleaving(K0, U64BIG(*(u64*)k));
  to_bit_interleaving(K1, U64BIG(*(u64*)(k + 8)));
  to_bit_interleaving(N0, U64BIG(*(u64*)npub));
  to_bit_interleaving(N1, U64BIG(*(u64*)(npub + 8)));

  // initialization
  to_bit_interleaving(x0, IV);
  x1.o = K0.o;
  x1.e = K0.e;
  x2.e = K1.e;
  x2.o = K1.o;
  x3.e = N0.e;
  x3.o = N0.o;
  x4.e = N1.e;
  x4.o = N1.o;
#ifdef SET_TIE_ALGO
  P12_TIE();
#else
  P12();
#endif
  x3.e ^= K0.e;
  x3.o ^= K0.o;
  x4.e ^= K1.e;
  x4.o ^= K1.o;

  // process associated data
  if (adlen) {
    while (adlen >= RATE) {
      to_bit_interleaving(t0, U64BIG(*(u64*)ad));
      x0.e ^= t0.e;
      x0.o ^= t0.o;
      to_bit_interleaving(t1, U64BIG(*(u64*)(ad + 8)));
      x1.e ^= t1.e;
      x1.o ^= t1.o;
#ifdef SET_TIE_ALGO
  P8_TIE();
#else
  P8();
#endif

      adlen -= RATE;
      ad += RATE;
    }
    tmp0 = 0;
    tmp1 = 0;
    for (i = 0; i < adlen; ++i, ++ad)
      if (i < 8)
        tmp0 ^= INS_BYTE64(*ad, i);
      else
        tmp1 ^= INS_BYTE64(*ad, i % 8);
    if (adlen < 8)
      tmp0 ^= INS_BYTE64(0x80, adlen);
    else
      tmp1 ^= INS_BYTE64(0x80, adlen % 8);
    to_bit_interleaving(t0, tmp0);
    x0.e ^= t0.e;
    x0.o ^= t0.o;
    to_bit_interleaving(t1, tmp1);
    x1.e ^= t1.e;
    x1.o ^= t1.o;
#ifdef SET_TIE_ALGO
  P8_TIE();
#else
  P8();
#endif
  }
  x4.e ^= 1;

  // process plaintext
  while (mlen >= RATE) {
    to_bit_interleaving(t0, U64BIG(*(u64*)m));
    x0.e ^= t0.e;
    x0.o ^= t0.o;
    to_bit_interleaving(t1, U64BIG(*(u64*)(m + 8)));
    x1.e ^= t1.e;
    x1.o ^= t1.o;
    from_bit_interleaving(tmp0, x0);
    *(u64*)c = U64BIG(tmp0);
    from_bit_interleaving(tmp1, x1);
    *(u64*)(c + 8) = U64BIG(tmp1);


	#ifdef SET_TIE_ALGO
	  P8_TIE();
	#else
	  P8();
	#endif
    mlen -= RATE;
    m += RATE;
    c += RATE;
  }
  tmp0 = 0;
  tmp1 = 0;
  for (i = 0; i < mlen; ++i, ++m)
    if (i < 8)
      tmp0 ^= INS_BYTE64(*m, i);
    else
      tmp1 ^= INS_BYTE64(*m, i % 8);
  if (mlen < 8)
    tmp0 ^= INS_BYTE64(0x80, mlen);
  else
    tmp1 ^= INS_BYTE64(0x80, mlen % 8);
  to_bit_interleaving(t0, tmp0);
  x0.e ^= t0.e;
  x0.o ^= t0.o;
  to_bit_interleaving(t1, tmp1);
  x1.e ^= t1.e;
  x1.o ^= t1.o;
  from_bit_interleaving(tmp0, x0);
  from_bit_interleaving(tmp1, x1);
  for (i = 0; i < mlen; ++i, ++c)
    if (i < 8)
      *c = EXT_BYTE64(tmp0, i);
    else
      *c = EXT_BYTE64(tmp1, i % 8);

  // finalization
  x2.e ^= K0.e;
  x2.o ^= K0.o;
  x3.e ^= K1.e;
  x3.o ^= K1.o;
#ifdef SET_TIE_ALGO
  P12_TIE();
#else
  P12();
#endif

  x3.e ^= K0.e;
  x3.o ^= K0.o;
  x4.e ^= K1.e;
  x4.o ^= K1.o;

  // set tag
  from_bit_interleaving(tmp0, x3);
  *(u64*)c = U64BIG(tmp0);
  from_bit_interleaving(tmp1, x4);
  *(u64*)(c + 8) = U64BIG(tmp1);

  return 0;
}

