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
#define ROL8(a, offset) ((offset != 0) ? ((((BYTE)a) << offset) ^ (((BYTE)a) >> (sizeof(BYTE)*8-offset))) : a)
//__attribute__((noinline))
//#pragma no_unroll

#if defined(SET_NO_TIE)
void theta(BYTE *A)
	{
		unsigned int x, y;
		BYTE C[5],  D[5];
		/* DEBUG
		printf("\nA[x]");
		for(x=0; x<25; x++)	printf("%d,",A[x]);
		printf("\n");
		*/

		for(x=0; x<5; x++) {
			C[x] = 0;
			for(y=0; y<5; y++){
				//printf("\nindex(x, y):");
				//printf("%d,",index(x, y));
				C[x] ^= A[index(x, y)];
			}
		}
		/* DEBUG
		/*printf("\nC[x]");
		for(x=0; x<5; x++)	printf("%d,",C[x]);
		printf("\n");*/

		printf("\nA[index()]\n");
						for(int x=0; x<5; x++){
							printf("%d,%d,",(x+1)%5,(x+4)%5);
						}
						printf("\n");

		for(x=0; x<5; x++){
		D[x] = ROL8(C[(x+1)%5], 1) ^ C[(x+4)%5];
		}

		//DEBUG//
		/*printf("\nD[x]");
		for(x=0; x<5; x++)	printf("%d,",D[x]);
		printf("\n");*/
		printf("\nA[index()]\n");
				for(int x=0; x<5; x++){
					for(int y=0; y<5; y++)
					printf("theta(%d)",index(x, y));
					printf("\n");
				}
				printf("\n");

		for(x=0; x<5; x++)
			for(y=0; y<5; y++) {
				A[index(x, y)] ^= D[x];
			}

		/* DEBUG//
		printf("\nA[index()]");
		for(x=0; x<5; x++)
			for(y=0; y<5; y++)
			printf("%d,",A[index(x, y)]);
		printf("\n");
		*/

	}
#elif  defined(SET_TIE_BLOCK)
void theta(BYTE *A){ //__attribute__((noinline))

//	printf("\nA[x]theta BRFORE ");
//	    		for(int x=0; x<25; x++)	printf("%d,",A[x]);
//	    		printf("\n");



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


//			printf("\nA[x] theta AFTER ");
//							    		for(int x=0; x<25; x++)	printf("%d,",A[x]);
//							    		printf("\n");
}
#endif



void rho(BYTE *A){ //__attribute__((noinline)) {



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
					A[index(x, y)] = ROL8(A[index(x, y)], KeccakRhoOffsets[index(x, y)]);
	}

#elif defined(SET_TIE_SPECIFIC)
	for(unsigned int x=0; x<5; x++){
		for(unsigned int y=0; y<5; y++)
			{
				A[index(x, y)] = ROL8_T_R(A[index(x, y)], KeccakRhoOffsets[index(x, y)]);
			}
	}
#elif defined(SET_TIE_BLOCK)
//	printf("\nA[x]rho  BRFORE ");
//	    		for(int x=0; x<25; x++)	printf("%d,",A[x]);




	unsigned int *A0 = &A[0],
	             *A1 = &A[4],
				 *A2 = &A[8],
			     *A3 = &A[12],
				 *A4 = &A[16],
				 *A5 = &A[20],
				 *A6 = &A[24];
	rho_T(*A0,*A1,*A2,*A3,*A4,*A5,*A6);

	*((unsigned int*)&A[0]) = *A0 ;
	*((unsigned int*)&A[4]) = *A1 ;
	*((unsigned int*)&A[8]) = *A2 ;
	*((unsigned int*)&A[12]) = *A3 ;
	*((unsigned int*)&A[16]) = *A4 ;
	*((unsigned int*)&A[20]) = *A5 ;
	A[24] = (BYTE)*A6 ;


//	printf("\nA[x]rho  AFTER ");
//				    		for(int x=0; x<25; x++)	printf("%d,",A[x]);
//				    		printf("\n");


//	for(unsigned int x=0; x<5; x++){
//		BROT8(A[index(x, 0)],A[index(x, 1)],A[index(x, 2)],A[index(x, 3)],A[index(x, 4)],
//	    			 	   A[index(x, 0)],A[index(x, 1)],A[index(x, 2)],A[index(x, 3)],A[index(x, 4)],x );
//
//
//	}
#endif

}

void pi(BYTE *A) //__attribute__((noinline))
{
    BYTE tempA[25];

    for(unsigned int x=0; x<5; x++)
        for(unsigned int y=0; y<5; y++)
            tempA[index(x, y)] = A[index(x, y)];
    for(unsigned int x=0; x<5; x++)
        for(unsigned int y=0; y<5; y++)
            A[index(0*x+1*y, 2*x+3*y)] = tempA[index(x, y)];
}

void chi(BYTE *A)
{

    unsigned int x, y;
    BYTE C[5];

//    		printf("\nA[x] chi BRFORE ");
//    		for(int x=0; x<25; x++)	printf("%d,",A[x]);
//    		printf("\n");

//    		for(y=0; y<5; y++) {
//				for(x=0; x<5; x++)
//					C[x] = A[index(x, y)] ^ ((~A[index(x+1, y)]) & A[index(x+2, y)]);
//				for(x=0; x<5; x++){
//					A[index(x, y)] = C[x];
//					printf("%d,",A[index(x, y)]);
//				}
//				printf("\n");
//    		}


	unsigned int *A0 = &A[0],
			             *A1 = &A[4],
						 *A2 = &A[8],
					     *A3 = &A[12],
						 *A4 = &A[16],
						 *A5 = &A[20],
						 *A6 = &A[24];
	chi_T(*A0,*A1,*A2,*A3,*A4,*A5,*A6);

			*((unsigned int*)&A[0]) = *A0 ;
			*((unsigned int*)&A[4]) = *A1 ;
			*((unsigned int*)&A[8]) = *A2 ;
			*((unsigned int*)&A[12]) = *A3 ;
			*((unsigned int*)&A[16]) = *A4 ;
			*((unsigned int*)&A[20]) = *A5 ;
			A[24] = (BYTE)*A6 ;
//
//			printf("\nA[x] chi AFTER ");
//			    		for(int x=0; x<25; x++)	printf("%d,",A[x]);
//			    		printf("\n");


		/*  original code
			for(y=0; y<5; y++) {
			        for(x=0; x<5; x++)
			            C[x] = A[index(x, y)] ^ ((~A[index(x+1, y)]) & A[index(x+2, y)]);
			        for(x=0; x<5; x++)
			            A[index(x, y)] = C[x];
			    }

              */
//    for(int yy=0; yy<5; yy++){
//        	for(int xx=0; xx<5; xx++)
//            		printf("%02x,", (unsigned char)A[index(xx, yy)]);
//
//        	printf("\n");
//    }

}

void iota(BYTE *A, unsigned int indexRound)
{
    A[index(0, 0)] ^= KeccakRoundConstants[indexRound];
}

void KeccakP200Round(BYTE *state, unsigned int indexRound)
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

void permutation(BYTE* state)
{
    for(unsigned int i=0; i<maxNrRounds; i++)
        KeccakP200Round(state, i);
}
