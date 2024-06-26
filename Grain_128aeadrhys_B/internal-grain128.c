/*
 * Copyright (C) 2021 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "internal-grain128.h"
#include "lightwieghtTIE.h"
/* Determine which primitives should be accelerated with assembly code */
#if defined(__AVR__)
#define GRAIN128_ASM_CORE 1
#define GRAIN128_ASM_PREOUTPUT 1
#define GRAIN128_ASM_KEYSTREAM 0
#define GRAIN128_ASM_HELPERS 1
#elif defined(__ARM_ARCH_ISA_THUMB) && __ARM_ARCH == 7
#define GRAIN128_ASM_CORE 1
#define GRAIN128_ASM_PREOUTPUT 1
#define GRAIN128_ASM_KEYSTREAM 1
#define GRAIN128_ASM_HELPERS 0
#else
#define GRAIN128_ASM_CORE 0
#define GRAIN128_ASM_PREOUTPUT 0
#define GRAIN128_ASM_KEYSTREAM 0
#define GRAIN128_ASM_HELPERS 0
#endif

/* Extracts 32 bits from the Grain state that are not word-aligned */
#define GWORD(a, b, start_bit) \
        (((a) << ((start_bit) % 32)) ^ ((b) >> (32 - ((start_bit) % 32))))

#if !GRAIN128_ASM_CORE

void grain128_core
    (grain128_state_t *state, uint32_t x, uint32_t x2)
{
    uint32_t s0, s1, s2, s3,ZERO=0;

    /* From the Grain-128AEAD specification, the LFSR feedback algorithm is:
     *
     *      s'[i] = s[i + 1]
     *      s'[127] = s[0] ^ s[7] ^ s[38] ^ s[70] ^ s[81] ^ s[96] ^ x
     *
     * The bits are numbered from the most significant bit in the first
     * word of the LFSR state.  Calculate the feedback bits 32 at a time.
     */
//    s0 = state->lfsr[0];
//    s1 = state->lfsr[1];
//    s2 = state->lfsr[2];
//    s3 = state->lfsr[3];

    LFSTcalc(state->lfsr[0],state->lfsr[1],state->lfsr[2],state->lfsr[3],x);

//    x ^= s0;                        /* s[0] */
//#ifdef SET_TIE_SPECIFIC
//    x = ROTXOR(x,s0, s1, 7,25);
//    x = ROTXOR(x,s1, s2, 6,26);
//    x = ROTXOR(x,s2, s3, 6,26);
//    x = ROTXOR(x,s2, s3, 17,15);
//#else
//    x ^= GWORD(s0, s1, 7);          /* s[7] */
//    x ^= GWORD(s1, s2, 38);         /* s[38] */
//    x ^= GWORD(s2, s3, 70);         /* s[70] */
//    x ^= GWORD(s2, s3, 81);         /* s[81] */
//
//#endif
//    x ^= s3;                        /* s[96] */
//
//    /* Rotate the LFSR state left by 32 bits and feed s0 into the NFSR */
//    state->lfsr[0] = s1;
//    state->lfsr[1] = s2;
//    state->lfsr[2] = s3;
//    state->lfsr[3] = x;
//    x2 ^= s0;

    /* Perform the NFSR feedback algorithm from the specification:
     *
     *      b'[i] = b[i + 1]
     *      b'[127] = s[0] ^ b[0] ^ b[26] ^ b[56] ^ b[91] ^ b[96]
     *              ^ (b[3] & b[67]) ^ (b[11] & b[13]) ^ (b[17] & b[18])
     *              ^ (b[27] & b[59]) ^ (b[40] & b[48]) ^ (b[61] & b[65])
     *              ^ (b[68] & b[84]) ^ (b[22] & b[24] & b[25])
     *              ^ (b[70] & b[78] & b[82])
     *              ^ (b[88] & b[92] & b[93] & b[95]) ^ x2
     *
     * Once again, we calculate 32 feedback bits in parallel.
     */
    s0 = state->nfsr[0];
    s1 = state->nfsr[1];
    s2 = state->nfsr[2];
    s3 = state->nfsr[3];
    x2 ^= s0;                                       /* b[0] */


    NFSRcalc(state->lfsr[0],state->lfsr[1],state->lfsr[2],state->lfsr[3],x2);



//#ifdef SET_TIE_SPECIFIC
//    x2 = ROTXOR(x2,s0, s1, 26,6);
//    x2 = ROTXOR(x2,s1, s2, 24,8);
//    x2 = ROTXOR(x2,s2, s3, 27,5);
//#else
//    x2 ^= GWORD(s0, s1, 26);                        /* b[26] */
//    x2 ^= GWORD(s1, s2, 56);                        /* b[56] */
//    x2 ^= GWORD(s2, s3, 91);                        /* b[91] */
//#endif
//    x2 ^= s3;                                       /* b[96] */
//
//#ifdef SET_TIE_SPECIFIC
//    x2 = ROTXOR(x2,s0, s1, 3,29)     &    ROTXOR(ZERO,s2, s3, 1,31);
//    x2 = ROTXOR(x2,s0, s1, 11,21)    &    ROTXOR(ZERO,s0, s1, 13,19);
//    x2 = ROTXOR(x2,s0, s1, 17,15)    &    ROTXOR(ZERO,s0, s1, 18,14);
//
//    x2 = ROTXOR(x2,s0, s1, 27,5)     &   ROTXOR(ZERO,s1, s2, 27,5);
//    x2 = ROTXOR(x2,s1, s2, 8,24)     &    ROTXOR(ZERO,s1, s2, 16,16);
//    x2 = ROTXOR(x2,s1, s1, 29,3)     &    ROTXOR(ZERO,s2, s3, 1,31);
//
//    x2 = ROTXOR(x2,s2, s3, 4,28)     &    ROTXOR(ZERO,s2, s3, 20,12);
//	x2 = ROTXOR(x2,s0, s1, 22,10)     &    ROTXOR(x2,s0, s1, 24,8)  & ROTXOR(x2,s0, s1, 25,7);
//	x2 = ROTXOR(x2,s2, s3, 6,26)     &    ROTXOR(ZERO,s2, s3, 14,18)& ROTXOR(ZERO,s2, s3, 18,14);
//	x2 = ROTXOR(x2,s2, s3, 24,8)     &    ROTXOR(ZERO,s2, s3, 28,4)& ROTXOR(ZERO,s2, s3, 29,3)
//																   & ROTXOR(ZERO,s2, s3, 31,1);
//
//
//
//
//#else
//
//
//    x2 ^= GWORD(s0, s1,  3) & GWORD(s2, s3, 67);    /* b[3] & b[67] */
//    x2 ^= GWORD(s0, s1, 11) & GWORD(s0, s1, 13);    /* b[11] & b[13] */
//    x2 ^= GWORD(s0, s1, 17) & GWORD(s0, s1, 18);    /* b[17] & b[18] */
//
//    x2 ^= GWORD(s0, s1, 27) & GWORD(s1, s2, 59);    /* b[27] & b[59] */
//    x2 ^= GWORD(s1, s2, 40) & GWORD(s1, s2, 48);    /* b[40] & b[48] */
//    x2 ^= GWORD(s1, s2, 61) & GWORD(s2, s3, 65);    /* b[61] & b[65] */
//
//    x2 ^= GWORD(s2, s3, 68) & GWORD(s2, s3, 84);    /* b[68] & b[84] */
//    x2 ^= GWORD(s0, s1, 22) & GWORD(s0, s1, 24) &   /* b[22] & b[24] & b[25] */
//    							GWORD(s0, s1, 25);
//    x2 ^= GWORD(s2, s3, 70) & GWORD(s2, s3, 78) &   /* b[70] & b[78] & b[82] */
//    						  GWORD(s2, s3, 82);
//    x2 ^= GWORD(s2, s3, 88) & GWORD(s2, s3, 92) &   /* b[88] & b[92] ... */
//          GWORD(s2, s3, 93) & GWORD(s2, s3, 95);    /* ... & b[93] & b[95] */
//#endif
//    /* Rotate the NFSR state left by 32 bits */
//    state->nfsr[0] = s1;
//    state->nfsr[1] = s2;
//    state->nfsr[2] = s3;
//    state->nfsr[3] = x2;
}

#endif /* !GRAIN128_ASM_CORE */

#if !GRAIN128_ASM_PREOUTPUT

uint32_t grain128_preoutput(const grain128_state_t *state)
{
    uint32_t s0, s1, s2, s3;
    uint32_t b0, b1, b2, b3;
    uint32_t x0, x4, y;

    /* From the Grain-128AEAD specification, each pre-output bit y is given by:
     *
     *      x[0..8] = b[12], s[8], s[13], s[20], b[95],
     *                s[42], s[60], s[79], s[94]
     *      h(x) = (x[0] & x[1]) ^ (x[2] & x[3]) ^ (x[4] & x[5])
     *           ^ (x[6] & x[7]) ^ (x[0] & x[4] & x[8])
     *      y = h(x) ^ s[93] ^ b[2] ^ b[15] ^ b[36] ^ b[45]
     *               ^ b[64] ^ b[73] ^ b[89]
     *
     * Calculate 32 pre-output bits in parallel.
     */
    s0 = state->lfsr[0];
    s1 = state->lfsr[1];
    s2 = state->lfsr[2];
    s3 = state->lfsr[3];
    b0 = state->nfsr[0];
    b1 = state->nfsr[1];
    b2 = state->nfsr[2];
    b3 = state->nfsr[3];
    x0 = GWORD(b0, b1, 12);
    x4 = GWORD(b2, b3, 95);
    y  = (x0 & GWORD(s0, s1, 8));                   /* x[0] & x[1] */
    y ^= (GWORD(s0, s1, 13) & GWORD(s0, s1, 20));   /* x[2] & x[3] */
    y ^= (x4 & GWORD(s1, s2, 42));                  /* x[4] & x[5] */
    y ^= (GWORD(s1, s2, 60) & GWORD(s2, s3, 79));   /* x[6] & x[7] */
    y ^= (x0 & x4 & GWORD(s2, s3, 94));             /* x[0] & x[4] & x[8] */
    y ^= GWORD(s2, s3, 93);                         /* s[93] */
    y ^= GWORD(b0, b1, 2);                          /* b[2] */
    y ^= GWORD(b0, b1, 15);                         /* b[15] */
    y ^= GWORD(b1, b2, 36);                         /* b[36] */
    y ^= GWORD(b1, b2, 45);                         /* b[45] */
    y ^= b2;                                        /* b[64] */
    y ^= GWORD(b2, b3, 73);                         /* b[73] */
    y ^= GWORD(b2, b3, 89);                         /* b[89] */
    return y;
}

#endif /* !GRAIN128_ASM_PREOUTPUT */

/* http://programming.sirrida.de/perm_fn.html#bit_permute_step */
#define bit_permute_step(_y, mask, shift) \
    do { \
        uint32_t y = (_y); \
        uint32_t t = ((y >> (shift)) ^ y) & (mask); \
        (_y) = (y ^ t) ^ (t << (shift)); \
    } while (0)

/* http://programming.sirrida.de/perm_fn.html#bit_permute_step_simple */
#define bit_permute_step_simple(_y, mask, shift) \
    do { \
        (_y) = (((_y) & (mask)) << (shift)) | (((_y) >> (shift)) & (mask)); \
    } while (0)

#if GRAIN128_ASM_HELPERS

/**
 * \brief Loads a 32-bit word and swaps it from big-endian bit order
 * into little-endian bit order.
 *
 * \param data Points to the word to be loaded.
 * \return Little-endian version of the 32-bit word at \a data.
 */
uint32_t grain128_swap_word32(const unsigned char *data);

/**
 * \brief Interleaves the bits in a 16-byte keystream block to separate
 * out the even and odd bits.
 *
 * \param ks Points to the keystream block.
 */
void grain128_interleave(unsigned char *ks);

#endif

void grain128_setup
    (grain128_state_t *state, const unsigned char *key,
     const unsigned char *nonce)
{
    uint32_t k[4];
    uint32_t y;
    uint8_t round;

    /* Internally, the Grain-128 stream cipher uses big endian bit
     * order, but the Grain-128AEAD specification for NIST uses little
     * endian bit order.  We need to swap the bits around when we load
     * the bits of the key and the nonce.
     *
     * Permutation generated with "http://programming.sirrida.de/calcperm.php".
     *
     * P = [7 6 5 4 3 2 1 0 15 14 13 12 11 10 9 8
     *      23 22 21 20 19 18 17 16 31 30 29 28 27 26 25 24]
     */
    #if GRAIN128_ASM_HELPERS
    #define SWAP_BITS(out, in) \
        do { \
            (out) = grain128_swap_word32((in)); \
        } while (0)
    #else
    #define SWAP_BITS(out, in) \
        do { \
            uint32_t tmp = be_load_word32((in)); \
            bit_permute_step_simple(tmp, 0x55555555, 1); \
            bit_permute_step_simple(tmp, 0x33333333, 2); \
            bit_permute_step_simple(tmp, 0x0f0f0f0f, 4); \
            (out) = tmp; \
        } while (0)
    #endif

    /* Initialize the LFSR state with the nonce and padding */
    SWAP_BITS(state->lfsr[0], nonce);
    SWAP_BITS(state->lfsr[1], nonce + 4);
    SWAP_BITS(state->lfsr[2], nonce + 8);
    state->lfsr[3] = 0xFFFFFFFEU; /* pad with all-1s and a terminating 0 */

    /* Initialize the NFSR state with the key */
    SWAP_BITS(k[0], key);
    SWAP_BITS(k[1], key + 4);
    SWAP_BITS(k[2], key + 8);
    SWAP_BITS(k[3], key + 12);
    state->nfsr[0] = k[0];
    state->nfsr[1] = k[1];
    state->nfsr[2] = k[2];
    state->nfsr[3] = k[3];

    /* Perform 320 rounds of Grain-128 to mix up the initial state.
     * The rounds can be performed 32 at a time: 32 * 10 = 320 */
    for (round = 0; round < 10; ++round) {
        y = grain128_preoutput(state);
        grain128_core(state, y, y);
    }

    /* Re-introduce the key into the LFSR and NFSR state */
    y = grain128_preoutput(state);
    grain128_core(state, y ^ k[2], y ^ k[0]);
    y = grain128_preoutput(state);
    grain128_core(state, y ^ k[3], y ^ k[1]);

    /* Generate the initial state of the accumulator and the shift register */
    state->accum = ((uint64_t)(grain128_preoutput(state))) << 32;
    grain128_core(state, 0, 0);
    state->accum |= grain128_preoutput(state);
    grain128_core(state, 0, 0);
    state->sr = ((uint64_t)(grain128_preoutput(state))) << 32;
    grain128_core(state, 0, 0);
    state->sr |= grain128_preoutput(state);
    grain128_core(state, 0, 0);

    /* No keystream data has been generated yet */
    state->posn = sizeof(state->ks);
}

#if GRAIN128_ASM_KEYSTREAM

/* Unrolled assembly version of grain128_next_keystream() is available */
void grain128_next_keystream(grain128_state_t *state);

#else /* !GRAIN128_ASM_KEYSTREAM */

/**
 * \brief Generates the next 16 byte block of keystream output data.
 *
 * \param state Grain-128 state.
 */
static void grain128_next_keystream(grain128_state_t *state)
{
#if !GRAIN128_ASM_HELPERS
    unsigned posn;
    for (posn = 0; posn < sizeof(state->ks); posn += 4) {
        /* Get the next word of pre-output and run the Grain-128 core */
        uint32_t x = grain128_preoutput(state);
        grain128_core(state, 0, 0);

        /* Grain-128 uses big-endian bit order, but the NIST functions
         * that are built on top of this use little-endian bit order.
         * Swap the bits around so that they are ready for use later.
         *
         * We also need to separate the bits: even bits are used to encrypt
         * and odd bits are used to authenticate.  Shift them to separate
         * bytes to make it easier to access the even and odd bits later.
         *
         * P = [7 15 6 14 5 13 4 12 3 11 2 10 1 9 0 8
         *      23 31 22 30 21 29 20 28 19 27 18 26 17 25 16 24]
         */
        bit_permute_step(x, 0x11111111, 3);
        bit_permute_step(x, 0x03030303, 6);
        bit_permute_step(x, 0x000f000f, 12);
        bit_permute_step_simple(x, 0x00ff00ff, 8);
        be_store_word32(state->ks + posn, x);
    }
#else
    /* Generate the data and then perform the interleaving */
    unsigned posn;
    for (posn = 0; posn < sizeof(state->ks); posn += 4) {
        uint32_t x = grain128_preoutput(state);
        le_store_word32(state->ks + posn, x);
        grain128_core(state, 0, 0);
    }
    grain128_interleave(state->ks);
#endif
}

#endif /* !GRAIN128_ASM_KEYSTREAM */

void grain128_authenticate
    (grain128_state_t *state, const unsigned char *data,
     size_t len)
{
    unsigned char abyte;
    unsigned char sbyte;
    unsigned char bit;
    uint64_t accum = state->accum;
    uint32_t accum_l=(uint32_t)state->accum;  // add this
    uint64_t sr = state->sr;
    unsigned posn = state->posn;
    while (len > 0) {
        /* Fetch the next byte to be authenticated */
        abyte = *data++;
        --len;

        /* Request more keystream data if necessary */
        if (posn >= sizeof(state->ks)) {
            grain128_next_keystream(state);
            posn = 0;
        }

        /* Get the next byte of keystream to add to the shift register.
         * We use the odd bytes from the keystream and ignore even ones */
        sbyte = state->ks[posn + 1];
        posn += 2;

        /* XOR the shift register with the accumulator for each 1 bit
         * in the byte that we are authenticating.  And shift in the
         * keystream byte we retrieved above */
        for (bit = 0; bit < 8; ++bit) {
#ifdef SET_TIE_SPECIFIC
	   XORAND(accum_l ,sr,abyte & 0x01);
	   sr = SHIFTLXOR(sr,(sbyte & 0x01),1);
#else

	        accum ^= sr & (-((uint64_t)(abyte & 0x01)));
            sr = (sr << 1) ^ (sbyte & 0x01);

#endif

            abyte >>= 1;
            sbyte >>= 1;
        }
    }
    state->accum = accum ^ (uint64_t)accum_l;
    state->sr = sr;
    state->posn = posn;
}

void grain128_encrypt
    (grain128_state_t *state, unsigned char *c, const unsigned char *m,
     size_t len)
{
    unsigned char mbyte;
    unsigned char sbyte;
    unsigned char bit;
    uint64_t accum = state->accum;
    uint32_t accum_l=(uint32_t)state->accum;  // add this
    uint64_t sr = state->sr;
    unsigned posn = state->posn;
    while (len > 0) {
        /* Fetch the next byte to be encrypted and authenticated */
        mbyte = *m++;
        --len;

        /* Request more keystream data if necessary */
        if (posn >= sizeof(state->ks)) {
            grain128_next_keystream(state);
            posn = 0;
        }

        /* Get the next two bytes of keystream data.  The even byte is
         * used to encrypt the input and the odd byte is shifted into
         * the shift register for authentication purposes */
        *c++ = mbyte ^ state->ks[posn];
        sbyte = state->ks[posn + 1];
        posn += 2;

        /* XOR the shift register with the accumulator for each 1 bit
         * in the plaintext byte that we are authenticating.  And shift
         * in the keystream byte we retrieved above */
        for (bit = 0; bit < 8; ++bit) {
#ifdef SET_TIE_SPECIFIC
	   XORAND(accum_l ,sr,mbyte & 0x01);
	   sr = SHIFTLXOR(sr,(sbyte & 0x01),1);
#else

	    accum ^= sr & (-((uint64_t)(mbyte & 0x01)));
            sr = (sr << 1) ^ (sbyte & 0x01);

#endif


            mbyte >>= 1;
            sbyte >>= 1;
        }
    }
    state->accum = accum^ (uint64_t)accum_l;
    state->sr = sr;
    state->posn = posn;
}

void grain128_decrypt
    (grain128_state_t *state, unsigned char *m, const unsigned char *c,
     size_t len)
{
    unsigned char mbyte;
    unsigned char sbyte;
    unsigned char bit;
    uint64_t accum = state->accum;
    uint32_t accum_l=(uint32_t)state->accum;  // add this
    uint64_t sr = state->sr;
    unsigned posn = state->posn;
    while (len > 0) {
        /* Fetch the next byte to be decrypted and authenticated */
        mbyte = *c++;
        --len;

        /* Request more keystream data if necessary */
        if (posn >= sizeof(state->ks)) {
            grain128_next_keystream(state);
            posn = 0;
        }

        /* Get the next two bytes of keystream data.  The even byte is
         * used to decrypt the input and the odd byte is shifted into
         * the shift register for authentication purposes */
        mbyte ^= state->ks[posn];
        *m++ = mbyte;
        sbyte = state->ks[posn + 1];
        posn += 2;

        /* XOR the shift register with the accumulator for each 1 bit
         * in the plaintext byte that we are authenticating.  And shift
         * in the keystream byte we retrieved above */
        for (bit = 0; bit < 8; ++bit) {
#ifdef SET_TIE_SPECIFIC
	   XORAND(accum_l ,sr,mbyte & 0x01);
	   sr = SHIFTLXOR(sr,(sbyte & 0x01),1);
#else

	    accum ^= sr & (-((uint64_t)(mbyte & 0x01)));
            sr = (sr << 1) ^ (sbyte & 0x01);

#endif


            mbyte >>= 1;
            sbyte >>= 1;
        }
    }
    state->accum = accum^ (uint64_t)accum_l;
    state->sr = sr;
    state->posn = posn;
}

#if !defined(__AVR__)

void grain128_compute_tag(grain128_state_t *state)
{
    uint64_t x;

    /* Authenticate a final 1 bit as padding on the stream */
    state->accum ^= state->sr;

    /* Swap the bits of the accumulator into little endian
     * order and write them to the keystream buffer */
    x = state->accum;
    bit_permute_step_simple(x, 0x5555555555555555ULL, 1);
    bit_permute_step_simple(x, 0x3333333333333333ULL, 2);
    bit_permute_step_simple(x, 0x0f0f0f0f0f0f0f0fULL, 4);
    be_store_word64(state->ks, x);
}

#endif /* !__AVR__ */
