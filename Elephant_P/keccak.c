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


void theta(BYTE *A)
	{
		unsigned int x, y;
		BYTE C[5],  D[5];


	}


void rho(BYTE *A)
{



}

void pi(BYTE *A)
{
    }

void chi(BYTE *A)
{

}

void iota(BYTE *A, unsigned int indexRound)
{

}

void KeccakP200Round(BYTE *state, unsigned int indexRound)
{

/*    theta(state);// added TIE
    rho(state);  // added TIE
    pi(state);
    chi(state);  // added TIE
    iota(state, indexRound);
*/
	unsigned int *A0 = &state[0],
                 *A1 = &state[4],
				 *A2 = &state[8],
			     *A3 = &state[12],
				 *A4 = &state[16],
				 *A5 = &state[20],
				 *A6 = &state[24];

	//DEBUG//

//	printf("\nBEFORE A[x]");
//	 for(int x=0; x<25; x++)	printf("%d,",state[x]);

	Kecccakprm(*A0,*A1,*A2,*A3,*A4,*A5,*A6,indexRound);

		*((unsigned int*)&state[0]) = *A0 ;
		*((unsigned int*)&state[4]) = *A1 ;
		*((unsigned int*)&state[8]) = *A2 ;
		*((unsigned int*)&state[12]) = *A3 ;
		*((unsigned int*)&state[16]) = *A4 ;
		*((unsigned int*)&state[20]) = *A5 ;
		state[24] = (BYTE)*A6 ;

//		printf("\nAFTER A[x]");
//			 for(int x=0; x<25; x++)	printf("%d,",state[x]);

}

void permutation(BYTE* state)
{
    for(unsigned int i=0; i<maxNrRounds; i++)
        KeccakP200Round(state, i);
}
