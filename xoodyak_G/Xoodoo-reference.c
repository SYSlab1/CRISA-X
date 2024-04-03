/*
Implementation by Ronny Van Keer, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our website:
https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

/* #define VERBOSE_LEVEL    0 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "Xoodoo.h"
#include "lightwieghtTIE.h"
#define sizetXood 4
//char sizetXood = sizeof(tXoodooLane);

/* ---------------------------------------------------------------- */

void Xoodoo_StaticInitialize( void )
{
}

/* ---------------------------------------------------------------- */

void Xoodoo_Initialize(void *state)
{
    memset(state, 0, NLANES*sizeof(tXoodooLane));
}

/* ---------------------------------------------------------------- */

void Xoodoo_AddByte(void *state, unsigned char byte, unsigned int offset)
{
    assert(offset < NLANES*sizeof(tXoodooLane));
    ((unsigned char *)state)[offset] ^= byte;
}

/* ---------------------------------------------------------------- */

void Xoodoo_AddBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length)
{
    unsigned int i;

    assert(offset < NLANES*sizeof(tXoodooLane));
    assert(offset+length <= NLANES*sizeof(tXoodooLane));
    for(i=0; i<length; i++)
        ((unsigned char *)state)[offset+i] ^= data[i];
}

/* ---------------------------------------------------------------- */

void Xoodoo_OverwriteBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length)
{
    assert(offset < NLANES*sizeof(tXoodooLane));
    assert(offset+length <= NLANES*sizeof(tXoodooLane));
    memcpy((unsigned char*)state+offset, data, length);
}

/* ---------------------------------------------------------------- */

void Xoodoo_OverwriteWithZeroes(void *state, unsigned int byteCount)
{
    assert(byteCount <= NLANES*sizeof(tXoodooLane));
    memset(state, 0, byteCount);
}

/* ---------------------------------------------------------------- */

void Xoodoo_ExtractBytes(const void *state, unsigned char *data, unsigned int offset, unsigned int length)
{
    assert(offset < NLANES*sizeof(tXoodooLane));
    assert(offset+length <= NLANES*sizeof(tXoodooLane));
    memcpy(data, (unsigned char*)state+offset, length);
}

/* ---------------------------------------------------------------- */

void Xoodoo_ExtractAndAddBytes(const void *state, const unsigned char *input, unsigned char *output, unsigned int offset, unsigned int length)
{
    unsigned int i;

    assert(offset < NLANES*sizeof(tXoodooLane));
    assert(offset+length <= NLANES*sizeof(tXoodooLane));
    for(i=0; i<length; i++)
        output[i] = input[i] ^ ((unsigned char *)state)[offset+i];
}

/* ---------------------------------------------------------------- */

#if defined(VERBOSE_LEVEL)

static void Dump(char * text, tXoodooLane * a, unsigned int level)
{
    if (level == VERBOSE_LEVEL) {
    #if 0
        printf("%-8.8s ", text);
        printf("%u %u %u %u - ", a[0+0], a[0+1], a[0+2], a[0+3] );
        printf("%u %u %u %u - ", a[4+0], a[4+1], a[4+2], a[4+3] );
        printf("%u %u %u %u\n", a[8+0], a[8+1], a[8+2], a[8+3] );
        if ((level == 2) && !strcmp(text, "Rho-east"))
            printf("\n");
    #elif 0
        printf("%-8.8s ", text);
        printf("%08x %08x %08x %08x - ", a[0+0], a[0+1], a[0+2], a[0+3] );
        printf("%08x %08x %08x %08x - ", a[4+0], a[4+1], a[4+2], a[4+3] );
        printf("%08x %08x %08x %08x\n", a[8+0], a[8+1], a[8+2], a[8+3] );
    #else
        printf("%s\n", text);
        printf("a00 %08x, a01 %08x, a02 %08x, a03 %08x\n", a[0+0], a[0+1], a[0+2], a[0+3] );
        printf("a10 %08x, a11 %08x, a12 %08x, a13 %08x\n", a[4+0], a[4+1], a[4+2], a[4+3] );
        printf("a20 %08x, a21 %08x, a22 %08x, a23 %08x\n\n", a[8+0], a[8+1], a[8+2], a[8+3] );
    #endif
    }
}

#else

#define Dump(text, a, level)

#endif


static void fromBytesToWords(tXoodooLane *stateAsWords, unsigned char *state)
{
    unsigned int i, j,i_cal;
#ifdef SET_TIE_BLOCK
    for(i=0; i<NLANES; i++) {
    	i_cal = i<<2;  // equl to i*sizetXood
    	stateAsWords[i] = stateAsWords[i] | BytesToWords(
    				state[i_cal+0],state[i_cal+1],state[i_cal+2],state[i_cal+3]);}


#elif defined( SET_NO_TIE)
    for(i=0; i<NLANES; i++) {
            stateAsWords[i] = 0;
        for(j=0; j<sizetXood;j++){
           stateAsWords[i] |= (tXoodooLane)(state[i*sizetXood+j]) << (8*j);}
    }
#endif
}

static void fromWordsToBytes(unsigned char *state, const tXoodooLane *stateAsWords)
{
    unsigned int i, j,i_cal;

#ifdef SET_TIE_BLOCK
    for(i=0; i<NLANES; i++) {
    	i_cal = i<<2;  // equl to i*sizetXood
    	WordsToBytes(state[i_cal+0],state[i_cal+1],state[i_cal+2],state[i_cal+3],stateAsWords[i]);
    }

#elif  defined(SET_NO_TIE)
    for(i=0; i<NLANES; i++)
        for(j=0; j<sizetXood; j++)
            state[i*sizetXood+j] = (stateAsWords[i] >> (8*j)) & 0xFF;
#endif
}

static void Xoodoo_Round( tXoodooLane * a, tXoodooLane rc )
//void Xoodoo_Round( tXoodooLane * a, tXoodooLane rc )
{
    unsigned int x, y;
    tXoodooLane    b[NLANES];
    tXoodooLane    p[NCOLUMS];
    tXoodooLane    e[NCOLUMS];
    char inx_e[] = {3,0,1,2};
//   	int inx []   ={0,1,2,3};
//	int inx_p1 []={4,5,6,7};
//	int inx_p2 []={8,9,10,11};
//	int inxd []   ={0,4,8,1,5,9,2,6,10,3,7,11};
//	int inxd_p1 []={4,8,0,5,9,1,6,10,2,7,11,3};
//	int inxd_p2 []={8,0,4,9,1,5,10,2,6,11,3,7};
//	int inxr_p2 []={10,11,8,9};
//	printf("\n---------------\n" );
//	for (x=0; x<NCOLUMS; ++x){
//		for (y=0; y<NCOLUMS; ++y)
//			printf("%d,",a[y+x*4]);
//		printf("\n" );
//	}
//
//	printf("\n---------------\n" );

    /* Theta: Column Parity Mixer */
for (x=0; x<NCOLUMS; ++x) {
//		printf("index(x,0)%d=%d,",x,index(x,0));
//		printf("index(x,1)%d=%d,",x,index(x,1));
//		printf("index(x,2)%d=%d,",x,index(x,2));
//		printf("\n");
#ifdef SET_TIE_FUNC

    	p[x] =   XOR5(a[index(x,0)],a[index(x,1)] , a[index(x,2)],0,0)  ;
 #else
    	p[x] = a[index(x,0)] ^ a[index(x,1)] ^ a[index(x,2)];
#endif

    }


    for (x=0; x<NCOLUMS; ++x) {
#ifdef SET_TIE_FUNC

    	//printf("\n%d,",(x-1)%4);
    	//printf("\n%d,",inx[x]);
   // e[x] = DROTXOR32c(p[(x-1)%4],p[(x-1)%4],5,14);// ^ ROTXOR32c(p[(x-1)%4],p[(x-1)%4],14,14);
    e[x] = ROTXOR2(p[inx_e[x]],p[inx_e[x]],5,14);// ^ ROTXOR32c(p[(x-1)%4],p[(x-1)%4],14,14);



    #else
        e[x] = ROTL32(p[(x-1)%4], 5) ^ ROTL32(p[(x-1)%4], 14);
#endif

}
    for (x=0; x<NCOLUMS; ++x)
        for (y=0; y<NROWS; ++y)
            a[index(x,y)] ^= e[x];
    Dump("Theta", a, 2);

    /* Rho-west: plane shift */
    for (x=0; x<NCOLUMS; ++x) {

        b[index(x,0)] = a[index(x,0)];
        b[index(x,1)] = a[index(x-1,1)];
       // printf("index(x,2)%d=%d,",x,inx_p2[x]);
#ifdef SET_TIE_FUNC
       // b[index(x,2)] = SROTXOR32c(a[inx_p2[x]],11);
        b[index(x,2)] = ROTXOR(0,a[index(x,2)],a[index(x,2)],11,21);
#else
        b[index(x,2)] = ROTL32(a[index(x,2)], 11);
#endif
    }
    memcpy( a, b, sizeof(b) );
    Dump("Rho-west", a, 2);
        
    /* Iota: round constant */
    a[0] ^= rc;
    Dump("Iota", a, 2);

    /* Chi: non linear layer */
   // printf("\n");
    char trans_inx;
    for (x=0; x<NCOLUMS; ++x)
        for (y=0; y<NROWS; ++y) {


        	#ifdef SET_TIE_FUNC
            //trans_inx =y+(x*3);
        	//b[index(x,y)] = XorNotAnd(a[inxd[trans_inx]],a[inxd_p1[trans_inx]],a[inxd_p2[trans_inx]]);
        	b[index(x,y)] = XORNOTAND(a[index(x,y)],a[index(x,y+1)],a[index(x,y+2)]);

            #else
            b[index(x,y)] = a[index(x,y)] ^ (~a[index(x,y+1)] & a[index(x,y+2)]);
#endif
        }
    memcpy( a, b, sizeof(b) );
    Dump("Chi", a, 2);

    /* Rho-east: plane shift */
    for (x=0; x<NCOLUMS; ++x) {
        b[index(x,0)] = a[index(x,0)];
//	printf("%d=%d,",x,index(x,1));
//	printf("%d=%d,",x,index(x+2,2));

#ifdef SET_TIE_FUNC
//         b[index(x,1)] = SROTXOR32c(a[inx_p1[x]  ],1);
//         b[index(x,2)] = SROTXOR32c(a[inxr_p2[x]],8);

        b[index(x,1)] = ROTXOR(0,a[index(x,1)],a[index(x,1)],1,31);
        b[index(x,2)] = ROTXOR(0,a[index(x+2,2)],a[index(x+2,2)],8,24);

#else
        b[index(x,1)] = ROTL32(a[index(x,1)], 1);
        b[index(x,2)] = ROTL32(a[index(x+2,2)], 8);
#endif

    }
    memcpy( a, b, sizeof(b) );
    Dump("Rho-east", a, 2);

}

static const uint32_t    RC[MAXROUNDS] = {
    _rc12,
    _rc11,
    _rc10,
    _rc9,
    _rc8,
    _rc7,
    _rc6,
    _rc5,
    _rc4,
    _rc3,
    _rc2,
    _rc1
};

void Xoodoo_Permute_Nrounds( void * state, uint32_t nr )
{
    tXoodooLane        a[NLANES];
    unsigned int    i;

    fromBytesToWords(a, (const unsigned char *)state);

    for (i = MAXROUNDS - nr; i < MAXROUNDS; ++i ) {
        Xoodoo_Round( a, RC[i] );
        Dump("Round", a, 1);
    }
    Dump("Permutation", a, 0);

    fromWordsToBytes((unsigned char *)state, a);

}

void Xoodoo_Permute_6rounds( uint32_t * state)
{
    Xoodoo_Permute_Nrounds( state, 6 );
}

void Xoodoo_Permute_12rounds( uint32_t * state)
{
    Xoodoo_Permute_Nrounds( state, 12 );
}
