/**
 * Based on the implementation by the Keccak Team, namely, Guido Bertoni, Joan Daemen,
 * Michaël Peeters, Gilles Van Assche and Ronny Van Keer.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "elephant_200.h"
#include "lightwieghtTIE.h"
//#include <xtensa/tie/genric_tie.h>
//#define SET_TIE_FUNC



#define maxNrRounds 18
#define nrLanes 25
#define index(x, y) (((x)%5)+5*((y)%5))

const BYTE KeccakRoundConstants[maxNrRounds] = {
    0x01, 0x82, 0x8a, 0x00, 0x8b, 0x01, 0x81, 0x09, 0x8a,
    0x88, 0x09, 0x0a, 0x8b, 0x8b, 0x89, 0x03, 0x02, 0x80
};


const unsigned int KeccakRhoOffsets[nrLanes] = {
    0, 1, 6, 4, 3,
	4, 4, 6, 7, 4,
	3, 2, 3, 1, 7,
	1, 5, 7, 5, 0,
	2, 2, 5, 0, 6
};

const unsigned int KeccakRhoOffsets_r[nrLanes] = {
    0,4,3,1,2,
	1,4,2,5,2,
	6,6,3,7,5,
	4,7,1,5,0,
	3,4,7,0,6

};
#define ROL8_C(a, offset) ((offset != 0) ? ((((BYTE)a) << offset) ^ (((BYTE)a) >> (sizeof(BYTE)*8-offset))) : a)
//__attribute__((noinline))
//#pragma no_unroll

#if defined(SET_NO_TIE)
void theta(BYTE *A)__attribute__((noinline))
	{
		unsigned int x, y;
		BYTE C[5],  D[5];
		for(x=0; x<5; x++) {
			C[x] = 0;
			for(y=0; y<5; y++){
				C[x] ^= A[index(x, y)];
			}
		}

		for(x=0; x<5; x++){
		D[x] = ROL8_C(C[(x+1)%5], 1) ^ C[(x+4)%5];
		}


		for(x=0; x<5; x++)
			for(y=0; y<5; y++) {
				A[index(x, y)] ^= D[x];
			}
		}
#elif defined(SET_TIE_SPECIFIC)
void theta(BYTE *A)__attribute__((noinline)){
		unsigned int x, y;
		BYTE C[5],  D[5];
		BYTE ZERO=0;
		//	theta_T(*A0,*A1,*A2,*A3,*A4,*A5,*A6);

		unsigned int 	 *A0 = &A[0],  //0,1,2,3
			             *A1 = &A[4],  //4,5,6,7
						 *A2 = &A[8],  //8.9.10.11
					     *A3 = &A[12], //12,13,14,15
						 *A4 = &A[16], //16,17,18,19
						 *A5 = &A[20], //20,21,22,23
						 *A6 = &A[24]; //24
				C[0]=XOR5(*A0,*(A1+1),*(A2+2),*(A3+3),*A5);   //A0,A5,A10,A15,A20
				//C[1]=XOR5(*A0,*A1,*A2,*A3,*A4);  //XOR5(A[1],A[6],A[11],A[16],A[21]);
				//C[2]=XOR5(*A0,*A1,*A2,*A3,*A4); //XOR5(A[2],A[7],A[12],A[17],A[22]);
				//C[3]=XOR5(*A0,*A1,*A2,*A3,*A4); //XOR5(A[3],A[8],A[13],A[18],A[23]);
				//C[4]=XOR5(*A0,*A1,*A2,*A3,*A4); //XOR5(A[4],A[9],A[14],A[19],A[24]);

			*((unsigned int*)&A[0]) = *A0 ;
			*((unsigned int*)&A[4]) = *A1 ;
			*((unsigned int*)&A[8]) = *A2 ;
			*((unsigned int*)&A[12]) = *A3 ;
			*((unsigned int*)&A[16]) = *A4 ;
			*((unsigned int*)&A[20]) = *A5 ;






//		for(x=0; x<5; x++){
//			D[x] = ROT8(C[(x+1)%5], 1) ^ C[(x+4)%5];
//			}
		D[0] = ROTXOR(ZERO,C[1],C[1], 1,7) ^ C[4];
		D[1] = ROTXOR(ZERO,C[2],C[1], 1,7) ^ C[0];
		D[2] = ROTXOR(ZERO,C[3],C[1], 1,7) ^ C[1];
		D[3] = ROTXOR(ZERO,C[4],C[1], 1,7) ^ C[2];
		D[4] = ROTXOR(ZERO,C[5],C[1], 1,7) ^ C[3];

//		for(x=0; x<5; x++)
//			XOROne2Many(D[x],A[index(x, 0)],A[index(x, 1)],
//					     A[index(x, 2)],A[index(x, 3)],
//					     A[index(x, 4)]);

		XOR5m(D[0],A[0],A[5],A[10],A[15],A[20]);
		XOR5m(D[1],A[1],A[6],A[11],A[16],A[21]);
		XOR5m(D[2],A[2],A[7],A[12],A[17],A[22]);
		XOR5m(D[3],A[3],A[8],A[13],A[18],A[23]);
		XOR5m(D[4],A[4],A[9],A[14],A[19],A[24]);
}

#elif  defined(SET_TIE_BLOCK)
void theta(BYTE *A){
			unsigned int *A0 = &A[0],
			             *A1 = &A[4],
						 *A2 = &A[8],
					     *A3 = &A[12],
						 *A4 = &A[16],
						 *A5 = &A[20],
						 *A6 = &A[24];
			theta_T(*A0,*A1,*A2,*A3,*A4,*A5,*A6);

			*((unsigned int*)&A[0]) = *A0 ;
			*((unsigned int*)&A[4]) = *A1 ;
			*((unsigned int*)&A[8]) = *A2 ;
			*((unsigned int*)&A[12]) = *A3 ;
			*((unsigned int*)&A[16]) = *A4 ;
			*((unsigned int*)&A[20]) = *A5 ;
			A[24] = (BYTE)*A6 ;
}
#endif



void rho(BYTE *A)__attribute__((noinline))
{



#if defined(SET_NO_TIE)
//	printf("\nA[index()]\n");
//		for(int x=0; x<5; x++){
//			for(int y=0; y<5; y++)
//			printf("ROL(%d,%d)",index(x, y),KeccakRhoOffsets[index(x, y)]);
//			printf("\n");
//		}
//		printf("\n");

	for(unsigned int x=0; x<5; x++){
		for(unsigned int y=0; y<5; y++)
					A[index(x, y)] = ROL8_C(A[index(x, y)], KeccakRhoOffsets[index(x, y)]);
	}

#elif defined(SET_TIE_SPECIFIC)
	char offst,a,ZERO=0;
	for(unsigned int x=0; x<5; x++){
		for(unsigned int y=0; y<5; y++)
			{
				offst = KeccakRhoOffsets[index(x, y)];
				a = A[index(x, y)];
				A[index(x, y)] = ROTXOR(ZERO,a,a,offst,8-offst );

			}
	}
#elif defined(SET_TIE_BLOCK)
	for(unsigned int x=0; x<5; x++){
		BROT8(A[index(x, 0)],A[index(x, 1)],A[index(x, 2)],A[index(x, 3)],A[index(x, 4)],
	    			 	   A[index(x, 0)],A[index(x, 1)],A[index(x, 2)],A[index(x, 3)],A[index(x, 4)],x );
	}
#endif

}

void pi(BYTE *A)__attribute__((noinline))
{
    BYTE tempA[25];

    for(unsigned int x=0; x<5; x++)
        for(unsigned int y=0; y<5; y++)
            tempA[index(x, y)] = A[index(x, y)];
    for(unsigned int x=0; x<5; x++)
        for(unsigned int y=0; y<5; y++)
            A[index(0*x+1*y, 2*x+3*y)] = tempA[index(x, y)];
}

void chi(BYTE *A)__attribute__((noinline))
{
    unsigned int x, y;
    BYTE C[5];

//    printf("\nchi\n");
//    		for(int x=0; x<5; x++){
//    			for(int y=0; y<5; y++)
//    			printf("ROL(%d,%d,%d)",index(x, y),index(x+1, y),index(x+2, y));
//    			printf("\n");
//    		}
//    		printf("\n");


    for(y=0; y<5; y++) {
        for(x=0; x<5; x++) {

#ifdef SET_TIE_SPECIFIC
        	C[x] = XORNOTAND(A[index(x, y)],A[index(x+1, y)],A[index(x+2, y)]);
#else
        	C[x] = A[index(x, y)] ^ ((~A[index(x+1, y)]) & A[index(x+2, y)]);
#endif
        }

        for(x=0; x<5; x++)
            A[index(x, y)] = C[x];
         }

//    for(int yy=0; yy<5; yy++){
//        	for(int xx=0; xx<5; xx++)
//            		printf("%02x,", (unsigned char)A[index(xx, yy)]);
//
//        	printf("\n");
//    }

}

void iota(BYTE *A, unsigned int indexRound)__attribute__((noinline))
{
    A[index(0, 0)] ^= KeccakRoundConstants[indexRound];
}

void KeccakP200Round(BYTE *state, unsigned int indexRound)__attribute__((noinline))
{
#if defined(SET_NO_TIE)|| 	defined(SET_TIE_SPECIFIC) || 	defined(SET_TIE_BLOCK)

    theta(state);// added TIE
    rho(state);  // added TIE
    pi(state);
    chi(state);  // added TIE
    iota(state, indexRound);
#elif defined(SET_TIE_ALGO)


#endif
}

void permutation(BYTE* state)__attribute__((noinline))
{
    for(unsigned int i=0; i<maxNrRounds; i++)
        KeccakP200Round(state, i);
}
