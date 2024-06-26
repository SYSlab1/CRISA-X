/******************************************************************************
* Implementation of the SKINNY tweakey schedule to match fixslicing.
*
* For more details, see the paper at
* https://csrc.nist.gov/CSRC/media/Events/lightweight-cryptography-workshop-2020
* /documents/papers/fixslicing-lwc2020.pdf
*  
*
* @author	Alexandre Adomnicai, Nanyang Technological University,
*			alexandre.adomnicai@ntu.edu.sg
*
* @date		May 2020
******************************************************************************/
#include <stdio.h>
#include <string.h> 		//for memcmp
#include <stdint.h>
#include "tk_schedule.h"
#include "skinny128.h"

/******************************************************************************
* The round constants according to the new representation.
******************************************************************************/
uint32_t rconst_32_bs[160] = {
	0x00000004, 0xffffffbf, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x10000100, 0xfffffeff, 0x44000000, 0xfbffffff, 0x00000000, 0x04000000,
	0x00100000, 0x00100000, 0x00100001, 0xffefffff, 0x00440000, 0xffafffff,
	0x00400000, 0x00400000, 0x01000000, 0x01000000, 0x01401000, 0xffbfffff,
	0x01004000, 0xfefffbff, 0x00000400, 0x00000400, 0x00000010, 0x00000000,
	0x00010410, 0xfffffbef, 0x00000054, 0xffffffaf, 0x00000000, 0x00000040,
	0x00000100, 0x00000100, 0x10000140, 0xfffffeff, 0x44000000, 0xfffffeff,
	0x04000000, 0x04000000, 0x00100000, 0x00100000, 0x04000001, 0xfbffffff,
	0x00140000, 0xffafffff, 0x00400000, 0x00000000, 0x00000000, 0x00000000,
	0x01401000, 0xfebfffff, 0x01004400, 0xfffffbff, 0x00000000, 0x00000400,
	0x00000010, 0x00000010, 0x00010010, 0xffffffff, 0x00000004, 0xffffffaf,
	0x00000040, 0x00000040, 0x00000100, 0x00000000, 0x10000140, 0xffffffbf,
	0x40000100, 0xfbfffeff, 0x00000000, 0x04000000, 0x00100000, 0x00000000,
	0x04100001, 0xffefffff, 0x00440000, 0xffefffff, 0x00000000, 0x00400000,
	0x01000000, 0x01000000, 0x00401000, 0xffffffff, 0x00004000, 0xfeffffff, 
	0x00000400, 0x00000000, 0x00000000, 0x00000000, 0x00010400, 0xfffffbff,
	0x00000014, 0xffffffbf, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x10000100, 0xffffffff, 0x40000000, 0xfbffffff, 0x00000000, 0x04000000,
	0x00100000, 0x00000000, 0x00100001, 0xffefffff, 0x00440000, 0xffafffff,
	0x00000000, 0x00400000, 0x01000000, 0x01000000, 0x01401000, 0xffffffff,
	0x00004000, 0xfeffffff, 0x00000400, 0x00000400, 0x00000010, 0x00000000,
	0x00010400, 0xfffffbff, 0x00000014, 0xffffffaf, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x10000140, 0xfffffeff, 0x44000000, 0xffffffff,
	0x00000000, 0x04000000, 0x00100000, 0x00100000, 0x00000001, 0xffefffff,
	0x00440000, 0xffafffff, 0x00400000, 0x00000000, 0x00000000, 0x01000000,
	0x01401000, 0xffbfffff, 0x01004000, 0xfffffbff, 0x00000400, 0x00000400,
	0x00000010, 0x00000000, 0x00010010, 0xfffffbff
};

/******************************************************************************
* 	Pack the input into the bitsliced representation
* 	24 28 56 60 88 92 120 124 | ... | 0 4 32 36 64 68 96 100
* 	25 29 57 61 89 93 121 125 | ... | 1 5 33 37 65 69 97 101
* 	26 30 58 62 90 94 122 126 | ... | 2 6 34 38 66 70 98 102
* 	27 31 59 63 91 95 123 127 | ... | 3 7 35 39 67 71 99 103
******************************************************************************/
void packing(uint32_t* out, const uint8_t* in) {

#ifdef SET_TIE_TIE
		uint32_t tmp;
		LE_LOAD(out, in);
		LE_LOAD(out + 1, in + 8);
		LE_LOAD(out + 2, in + 4);
		LE_LOAD(out + 3, in + 12);
		XORSHFTAND(out[0], out[0], 0x0a0a0a0a, 3);
		XORSHFTAND(out[1], out[1], 0x0a0a0a0a, 3);
		XORSHFTAND(out[2], out[2], 0x0a0a0a0a, 3);
		XORSHFTAND(out[3], out[3], 0x0a0a0a0a, 3);
		XORSHFTAND(out[2], out[0], 0x30303030, 2);
		XORSHFTAND(out[1], out[0], 0x0c0c0c0c, 4);
		XORSHFTAND(out[3], out[0], 0x03030303, 6);
		XORSHFTAND(out[1], out[2], 0x0c0c0c0c, 2);
		XORSHFTAND(out[3], out[2], 0x03030303, 4);
		XORSHFTAND(out[3], out[1], 0x03030303, 2);
#else
	uint32_t tmp;
	LE_LOAD(out, in);
	LE_LOAD(out + 1, in + 8);
	LE_LOAD(out + 2, in + 4);
	LE_LOAD(out + 3, in + 12);
	SWAPMOVE(out[0], out[0], 0x0a0a0a0a, 3);
	SWAPMOVE(out[1], out[1], 0x0a0a0a0a, 3);
	SWAPMOVE(out[2], out[2], 0x0a0a0a0a, 3);
	SWAPMOVE(out[3], out[3], 0x0a0a0a0a, 3);
	SWAPMOVE(out[2], out[0], 0x30303030, 2);
	SWAPMOVE(out[1], out[0], 0x0c0c0c0c, 4);
	SWAPMOVE(out[3], out[0], 0x03030303, 6);
	SWAPMOVE(out[1], out[2], 0x0c0c0c0c, 2);
	SWAPMOVE(out[3], out[2], 0x03030303, 4);
	SWAPMOVE(out[3], out[1], 0x03030303, 2);
#endif
}


void packing_TIE(uint32_t* out, const uint8_t* in) {
#ifdef SET_TIE_TIE
	uint32_t tmp;
	LE_LOAD(out, in);
	LE_LOAD(out + 1, in + 8);
	LE_LOAD(out + 2, in + 4);
	LE_LOAD(out + 3, in + 12);
	XORSHFTAND(out[0], out[0], 0x0a0a0a0a, 3);
	XORSHFTAND(out[1], out[1], 0x0a0a0a0a, 3);
	XORSHFTAND(out[2], out[2], 0x0a0a0a0a, 3);
	XORSHFTAND(out[3], out[3], 0x0a0a0a0a, 3);
	XORSHFTAND(out[2], out[0], 0x30303030, 2);
	XORSHFTAND(out[1], out[0], 0x0c0c0c0c, 4);
	XORSHFTAND(out[3], out[0], 0x03030303, 6);
	XORSHFTAND(out[1], out[2], 0x0c0c0c0c, 2);
	XORSHFTAND(out[3], out[2], 0x03030303, 4);
	XORSHFTAND(out[3], out[1], 0x03030303, 2);

#else
	uint32_t tmp;
	LE_LOAD(out, in);
	LE_LOAD(out + 1, in + 8);
	LE_LOAD(out + 2, in + 4);
	LE_LOAD(out + 3, in + 12);
	SWAPMOVE(out[0], out[0], 0x0a0a0a0a, 3);
	SWAPMOVE(out[1], out[1], 0x0a0a0a0a, 3);
	SWAPMOVE(out[2], out[2], 0x0a0a0a0a, 3);
	SWAPMOVE(out[3], out[3], 0x0a0a0a0a, 3);
	SWAPMOVE(out[2], out[0], 0x30303030, 2);
	SWAPMOVE(out[1], out[0], 0x0c0c0c0c, 4);
	SWAPMOVE(out[3], out[0], 0x03030303, 6);
	SWAPMOVE(out[1], out[2], 0x0c0c0c0c, 2);
	SWAPMOVE(out[3], out[2], 0x03030303, 4);
	SWAPMOVE(out[3], out[1], 0x03030303, 2);
#endif
}
/******************************************************************************
* Unpack the input to a byte-wise representation
******************************************************************************/
void unpacking(uint8_t* out, uint32_t *in) {
	uint32_t tmp;
	SWAPMOVE(in[3], in[1], 0x03030303, 2);
	SWAPMOVE(in[3], in[2], 0x03030303, 4);
	SWAPMOVE(in[1], in[2], 0x0c0c0c0c, 2);
	SWAPMOVE(in[3], in[0], 0x03030303, 6);
	SWAPMOVE(in[1], in[0], 0x0c0c0c0c, 4);
	SWAPMOVE(in[2], in[0], 0x30303030, 2);
	SWAPMOVE(in[0], in[0], 0x0a0a0a0a, 3);
	SWAPMOVE(in[1], in[1], 0x0a0a0a0a, 3);
	SWAPMOVE(in[2], in[2], 0x0a0a0a0a, 3);
	SWAPMOVE(in[3], in[3], 0x0a0a0a0a, 3);
	LE_STORE(out, in[0]);
	LE_STORE(out + 8, in[1]);
	LE_STORE(out + 4, in[2]);
	LE_STORE(out + 12, in[3]);
}
void unpacking_TIE(uint8_t* out, uint32_t *in) {
	uint32_t tmp;
	XORSHFTAND(in[3], in[1], 0x03030303, 2);
	XORSHFTAND(in[3], in[2], 0x03030303, 4);
	XORSHFTAND(in[1], in[2], 0x0c0c0c0c, 2);
	XORSHFTAND(in[3], in[0], 0x03030303, 6);
	XORSHFTAND(in[1], in[0], 0x0c0c0c0c, 4);
	XORSHFTAND(in[2], in[0], 0x30303030, 2);
	XORSHFTAND(in[0], in[0], 0x0a0a0a0a, 3);
	XORSHFTAND(in[1], in[1], 0x0a0a0a0a, 3);
	XORSHFTAND(in[2], in[2], 0x0a0a0a0a, 3);
	XORSHFTAND(in[3], in[3], 0x0a0a0a0a, 3);
	LE_STORE(out, in[0]);
	LE_STORE(out + 8, in[1]);
	LE_STORE(out + 4, in[2]);
	LE_STORE(out + 12, in[3]);
}
/******************************************************************************
* 	0 4        1 5
* 	1 5  --->  2 6
* 	2 6        3 7
* 	3 7        4 0
******************************************************************************/
void lfsr2_bs(uint32_t* tk) {
	uint32_t tmp;
#ifdef SET_TIE_TIE

	tmp = XORAND(tk[0],tk[2] & 0xaaaaaaaa);
	tmp = ((tmp & 0xaaaaaaaa) >> 1) | ((tmp << 1) & 0xaaaaaaaa);
#else
	tmp = tk[0] ^ (tk[2] & 0xaaaaaaaa);
	tmp = ((tmp & 0xaaaaaaaa) >> 1) | ((tmp << 1) & 0xaaaaaaaa);
#endif
	tk[0] = tk[1];
	tk[1] = tk[2];
	tk[2] = tk[3];
	tk[3] = tmp;
}

/******************************************************************************
* 	0 4        7 3
* 	1 5  --->  0 4
* 	2 6        1 5
* 	3 7        2 6
******************************************************************************/
void lfsr3_bs(uint32_t* tk) {
	uint32_t tmp;
	tmp = tk[3] ^ ((tk[1] & 0xaaaaaaaa) >> 1);
	tmp = ((tmp & 0xaaaaaaaa) >> 1) | ((tmp << 1) & 0xaaaaaaaa);
	tk[3] = tk[2];
	tk[2] = tk[1];
	tk[1] = tk[0];
	tk[0] = tmp;
}

/******************************************************************************
* Apply the permutation in a bitsliced manner, twice
******************************************************************************/
void permute_tk_2(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROR(tmp,14) & 0xcc00cc00;
		tk[i] |= (tmp & 0x000000ff) << 16;
		tk[i] |= (tmp & 0xcc000000)>> 2;
		tk[i] |= (tmp & 0x0033cc00) >> 8;
		tk[i] |= (tmp & 0x00cc0000) >>18;
	}
}

/******************************************************************************
* Apply the permutation in a bitsliced manner, 4 times
******************************************************************************/
void permute_tk_4(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROR(tmp,22) & 0xcc0000cc;
		tk[i] |= ROR(tmp,16) & 0x3300cc00;
		tk[i] |= ROR(tmp, 24) & 0x00cc3300;
		tk[i] |= (tmp & 0x00cc00cc) >> 2;
	}
}

void permute_tk_4_TIE(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROTOR(tmp,tmp,22,10) & 0xcc0000cc;
		tk[i] |= ROTOR(tmp,tmp,16,16) & 0x3300cc00;
		tk[i] |= ROTOR(tmp,tmp, 24,8) & 0x00cc3300;
		tk[i] |= (tmp & 0x00cc00cc) >> 2;
	}
}
/******************************************************************************
* Apply the permutation in a bitsliced manner, 6 times
******************************************************************************/
void permute_tk_6(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROR(tmp,6) & 0xcccc0000;
		tk[i] |= ROR(tmp,24) & 0x330000cc;
		tk[i] |= ROR(tmp,10) & 0x3333;
		tk[i] |= (tmp & 0xcc) << 14;
		tk[i] |= (tmp & 0x3300) << 2;
	}
}

/******************************************************************************
* Apply the permutation in a bitsliced manner, 8 times
******************************************************************************/
void permute_tk_8(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROR(tmp,24) & 0xcc000033;
		tk[i] |= ROR(tmp,8) & 0x33cc0000;
		tk[i] |= ROR(tmp,26) & 0x00333300;
		tk[i] |= (tmp & 0x00333300) >> 6;
	}
}

/******************************************************************************
* Apply the permutation in a bitsliced manner, 10 times
******************************************************************************/
void permute_tk_10(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROR(tmp,8) & 0xcc330000;
		tk[i] |= ROR(tmp,26) & 0x33000033;
		tk[i] |= ROR(tmp,22) & 0x00cccc00;
		tk[i] |= (tmp & 0x00330000) >> 14;
		tk[i] |= (tmp & 0xcc00) >> 2;
	}
}

/******************************************************************************
* Apply the permutation in a bitsliced manner, 12 times
******************************************************************************/
void permute_tk_12(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROR(tmp,8) & 0xcc33;
		tk[i] |= ROR(tmp,30) & 0x00cc00cc;
		tk[i] |= ROR(tmp,10) & 0x33330000;
		tk[i] |= ROR(tmp,16) & 0xcc003300;
	}
}

void permute_tk_12_TIE(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROTOR(tmp,tmp,8,24) & 0xcc33;
		tk[i] |= ROTOR(tmp,tmp,30,2) & 0x00cc00cc;
		tk[i] |= ROTOR(tmp,tmp,10,22) & 0x33330000;
		tk[i] |= ROTOR(tmp,tmp,16,16) & 0xcc003300;
	}
}
/******************************************************************************
* Apply the permutation in a bitsliced manner, 14 times
******************************************************************************/
void permute_tk_14(uint32_t* tk) {
	uint32_t tmp;
	for(int i =0; i < 4; i++) {
		tmp = tk[i];
		tk[i] = ROR(tmp,24) & 0x0033cc00;
		tk[i] |= ROR(tmp,14) & 0x00cc0000;
		tk[i] |= ROR(tmp,30) & 0xcc000000;
		tk[i] |= ROR(tmp,16) & 0x000000ff;
		tk[i] |= ROR(tmp,18) & 0x33003300;
	}
}

/******************************************************************************
* Precompute all LFSRs on TK2
******************************************************************************/
void precompute_lfsr_tk2(uint32_t* tk, const uint8_t* key, const int rounds) {
	uint32_t tk2[4];
	packing(tk2, key);
	memcpy(tk, tk2, 16);
	for(int i = 0 ; i < rounds; i+=2) {
		lfsr2_bs(tk2);
		memcpy(tk+i*4+4, tk2, 16);
	}
}

/******************************************************************************
* Precompute all LFSRs on TK3
******************************************************************************/
void precompute_lfsr_tk3(uint32_t* tk, const uint8_t* key, const int rounds) {
	uint32_t tk3[4];
	packing(tk3, key);
	tk[0] ^= tk3[0];
	tk[1] ^= tk3[1];
	tk[2] ^= tk3[2];
	tk[3] ^= tk3[3];
	for(int i = 0 ; i < rounds; i+=2) {
		lfsr3_bs(tk3);
		tk[i*4+4] ^= tk3[0];
		tk[i*4+5] ^= tk3[1];
		tk[i*4+6] ^= tk3[2];
		tk[i*4+7] ^= tk3[3];
	}
}

/******************************************************************************
* XOR TK with TK1 before applying the permutations.
* The key is then rearranged to match the fixsliced representation.
******************************************************************************/
void permute_tk(uint32_t* tk, const uint8_t* key, const int rounds) {

#ifdef SET_TIE_TIE
	uint32_t test;
	uint32_t tk1[4], tmp[4];
	packing(tk1, key);
	memcpy(tmp, tk, 16);
	tmp[0] ^= tk1[0];
	tmp[1] ^= tk1[1];
	tmp[2] ^= tk1[2];
	tmp[3] ^= tk1[3];
	for(int i = 0 ; i < rounds; i += 8) {
		test = (i % 16 < 8) ? 1 : 0; 			//to apply the right power of P
		tk[i*4] = tmp[2] & 0xf0f0f0f0;
		tk[i*4+1] = tmp[3] & 0xf0f0f0f0;
		tk[i*4+2] = tmp[0] & 0xf0f0f0f0;
		tk[i*4+3] = tmp[1] & 0xf0f0f0f0;
		memcpy(tmp, tk+i*4+4, 16);
		XOR_BLOCKS_TIE(tmp, tk1);
		if (test)
			permute_tk_2(tmp); 					// applies P^2
		else
			permute_tk_10(tmp); 				// applies P^10
		tk[i*4+4] = ROTOR(tmp[0],tmp[0],26,8) & 0xc3c3c3c3;
		tk[i*4+5] = ROTOR(tmp[1],tmp[1],26,8) & 0xc3c3c3c3;
		tk[i*4+6] = ROTOR(tmp[2],tmp[2],26,8) & 0xc3c3c3c3;
		tk[i*4+7] = ROTOR(tmp[3],tmp[3],26,8) & 0xc3c3c3c3;
		tk[i*4+8] = ROTOR(tmp[2],tmp[2],28,4) & 0x03030303;
		tk[i*4+8] |= ROTOR(tmp[2],tmp[2],12,20) & 0x0c0c0c0c;
		tk[i*4+9] = ROTOR(tmp[3],tmp[3],28,4) & 0x03030303;
		tk[i*4+9] |= ROTOR(tmp[3],tmp[3],12,20) & 0x0c0c0c0c;
		tk[i*4+10] = ROTOR(tmp[0],tmp[0],28,4) & 0x03030303;
		tk[i*4+10] |= ROTOR(tmp[0],tmp[0],12,20) & 0x0c0c0c0c;
		tk[i*4+11] = ROTOR(tmp[1],tmp[1],28,4) & 0x03030303;
		tk[i*4+11] |= ROTOR(tmp[1],tmp[1],12,20) & 0x0c0c0c0c;
		memcpy(tmp, tk+i*4+12, 16);
		XOR_BLOCKS_TIE(tmp, tk1);
		if (test)
			permute_tk_4_TIE(tmp); 					// applies P^4
		else
			permute_tk_12_TIE(tmp); 				// applies P^12
		for(int j = 0; j < 4; j++) {
			tk[i*4+12+j] = ROTOR(tmp[j],tmp[j],14,18) & 0x30303030;
			tk[i*4+12+j] |= ROTOR(tmp[j],tmp[j],6,26) & 0x0c0c0c0c;
		}
		tk[i*4+16] = ROTOR(tmp[2], tmp[2], 16,16) & 0xf0f0f0f0;
		tk[i*4+17] = ROTOR(tmp[3], tmp[3], 16,16) & 0xf0f0f0f0;
		tk[i*4+18] = ROTOR(tmp[0], tmp[0], 16,16) & 0xf0f0f0f0;
		tk[i*4+19] = ROTOR(tmp[1], tmp[1], 16,16) & 0xf0f0f0f0;
		memcpy(tmp, tk+i*4+20, 16);
		XOR_BLOCKS_TIE(tmp, tk1);
		if (test)
			permute_tk_6(tmp); 					//	applies P^6
		else
			permute_tk_14(tmp); 				// applies P^14
		tk[i*4+20] 	= ROTOR(tmp[0],tmp[0],10,22) & 0xc3c3c3c3;
		tk[i*4+21] 	= ROTOR(tmp[1],tmp[1], 10,22) & 0xc3c3c3c3;
		tk[i*4+22] 	= ROTOR(tmp[2],tmp[2],10,22) & 0xc3c3c3c3;
		tk[i*4+23] 	= ROTOR(tmp[3],tmp[3],10,22) & 0xc3c3c3c3;
		tk[i*4+24] 	= ROTOR(tmp[2],tmp[2], 12,22) & 0x03030303;
		tk[i*4+24] |= ROTOR(tmp[2],tmp[2], 28,4) & 0x0c0c0c0c;
		tk[i*4+25] 	= ROTOR(tmp[3],tmp[3], 12,20) & 0x03030303;
		tk[i*4+25] |= ROTOR(tmp[3],tmp[3], 28,4) & 0x0c0c0c0c;
		tk[i*4+26] 	= ROTOR(tmp[0],tmp[0],  12,20) & 0x03030303;
		tk[i*4+26] |= ROTOR(tmp[0],tmp[0], 28,4) & 0x0c0c0c0c;
		tk[i*4+27] 	= ROTOR(tmp[1],tmp[1],12,20) & 0x03030303;
		tk[i*4+27] |= ROTOR(tmp[1],tmp[1], 28,4) & 0x0c0c0c0c;
		memcpy(tmp, tk+i*4+28, 16);
		XOR_BLOCKS_TIE(tmp, tk1);
		if (test)
			permute_tk_8(tmp); 					// applies P^8
		for(int j = 0; j < 4; j++) {
			tk[i*4+28+j] = ROTOR(tmp[j],tmp[j],30,2) & 0x30303030;
			tk[i*4+28+j] |= ROTOR(tmp[j],tmp[j],22,10) & 0x0c0c0c0c;
		}
		if (test && (i+8 < rounds)) { 			//only if next loop iteration
			tk[i*4+32] = tmp[2] & 0xf0f0f0f0;
			tk[i*4+33] = tmp[3] & 0xf0f0f0f0;
			tk[i*4+34] = tmp[0] & 0xf0f0f0f0;
			tk[i*4+35] = tmp[1] & 0xf0f0f0f0;
		}
	}


#else
	uint32_t test;
		uint32_t tk1[4], tmp[4];
		packing(tk1, key);
		memcpy(tmp, tk, 16);
		tmp[0] ^= tk1[0];
		tmp[1] ^= tk1[1];
		tmp[2] ^= tk1[2];
		tmp[3] ^= tk1[3];
		for(int i = 0 ; i < rounds; i += 8) {
			test = (i % 16 < 8) ? 1 : 0; 			//to apply the right power of P
			tk[i*4] = tmp[2] & 0xf0f0f0f0;
			tk[i*4+1] = tmp[3] & 0xf0f0f0f0;
			tk[i*4+2] = tmp[0] & 0xf0f0f0f0;
			tk[i*4+3] = tmp[1] & 0xf0f0f0f0;
			memcpy(tmp, tk+i*4+4, 16);
			XOR_BLOCKS(tmp, tk1);
			if (test)
				permute_tk_2(tmp); 					// applies P^2
			else
				permute_tk_10(tmp); 				// applies P^10
			tk[i*4+4] = ROR(tmp[0],26) & 0xc3c3c3c3;
			tk[i*4+5] = ROR(tmp[1],26) & 0xc3c3c3c3;
			tk[i*4+6] = ROR(tmp[2],26) & 0xc3c3c3c3;
			tk[i*4+7] = ROR(tmp[3],26) & 0xc3c3c3c3;
			tk[i*4+8] = ROR(tmp[2],28) & 0x03030303;
			tk[i*4+8] |= ROR(tmp[2],12) & 0x0c0c0c0c;
			tk[i*4+9] = ROR(tmp[3],28) & 0x03030303;
			tk[i*4+9] |= ROR(tmp[3],12) & 0x0c0c0c0c;
			tk[i*4+10] = ROR(tmp[0],28) & 0x03030303;
			tk[i*4+10] |= ROR(tmp[0],12) & 0x0c0c0c0c;
			tk[i*4+11] = ROR(tmp[1],28) & 0x03030303;
			tk[i*4+11] |= ROR(tmp[1],12) & 0x0c0c0c0c;
			memcpy(tmp, tk+i*4+12, 16);
			XOR_BLOCKS(tmp, tk1);
			if (test)
				permute_tk_4(tmp); 					// applies P^4
			else
				permute_tk_12(tmp); 				// applies P^12
			for(int j = 0; j < 4; j++) {
				tk[i*4+12+j] = ROR(tmp[j],14) & 0x30303030;
				tk[i*4+12+j] |= ROR(tmp[j],6) & 0x0c0c0c0c;
			}
			tk[i*4+16] = ROR(tmp[2], 16) & 0xf0f0f0f0;
			tk[i*4+17] = ROR(tmp[3], 16) & 0xf0f0f0f0;
			tk[i*4+18] = ROR(tmp[0], 16) & 0xf0f0f0f0;
			tk[i*4+19] = ROR(tmp[1], 16) & 0xf0f0f0f0;
			memcpy(tmp, tk+i*4+20, 16);
			XOR_BLOCKS(tmp, tk1);
			if (test)
				permute_tk_6(tmp); 					//	applies P^6
			else
				permute_tk_14(tmp); 				// applies P^14
			tk[i*4+20] 	= ROR(tmp[0], 10) & 0xc3c3c3c3;
			tk[i*4+21] 	= ROR(tmp[1], 10) & 0xc3c3c3c3;
			tk[i*4+22] 	= ROR(tmp[2], 10) & 0xc3c3c3c3;
			tk[i*4+23] 	= ROR(tmp[3], 10) & 0xc3c3c3c3;
			tk[i*4+24] 	= ROR(tmp[2], 12) & 0x03030303;
			tk[i*4+24] |= ROR(tmp[2], 28) & 0x0c0c0c0c;
			tk[i*4+25] 	= ROR(tmp[3], 12) & 0x03030303;
			tk[i*4+25] |= ROR(tmp[3], 28) & 0x0c0c0c0c;
			tk[i*4+26] 	= ROR(tmp[0], 12) & 0x03030303;
			tk[i*4+26] |= ROR(tmp[0], 28) & 0x0c0c0c0c;
			tk[i*4+27] 	= ROR(tmp[1], 12) & 0x03030303;
			tk[i*4+27] |= ROR(tmp[1], 28) & 0x0c0c0c0c;
			memcpy(tmp, tk+i*4+28, 16);
			XOR_BLOCKS(tmp, tk1);
			if (test)
				permute_tk_8(tmp); 					// applies P^8
			for(int j = 0; j < 4; j++) {
				tk[i*4+28+j] = ROR(tmp[j],30) & 0x30303030;
				tk[i*4+28+j] |= ROR(tmp[j],22) & 0x0c0c0c0c;
			}
			if (test && (i+8 < rounds)) { 			//only if next loop iteration
				tk[i*4+32] = tmp[2] & 0xf0f0f0f0;
				tk[i*4+33] = tmp[3] & 0xf0f0f0f0;
				tk[i*4+34] = tmp[0] & 0xf0f0f0f0;
				tk[i*4+35] = tmp[1] & 0xf0f0f0f0;
			}
		}
#endif

}
/******************************************************************************
* Precompute RTK1.
******************************************************************************/
void tk_schedule_1(uint32_t* rtk1, const uint8_t* tk1) {
	memset(rtk1, 0x00, 16*16);
	permute_tk(rtk1, tk1, 16);
}

/******************************************************************************
* Precompute LFSR2(TK2) ^ LFSR3(TK3) ^ rconst.
******************************************************************************/
void tk_schedule_23(uint32_t* rtk_23, const uint8_t* tk2, const uint8_t * tk3) {
	memset(rtk_23, 0x00, 16*SKINNY128_384_ROUNDS);
	precompute_lfsr_tk2(rtk_23, tk2, SKINNY128_384_ROUNDS);
	precompute_lfsr_tk3(rtk_23, tk3, SKINNY128_384_ROUNDS);
	permute_tk(rtk_23, (uint8_t*)(rtk_23+8), SKINNY128_384_ROUNDS);
	for(int i = 0; i < SKINNY128_384_ROUNDS; i++) {				// add rconsts
		for(int j = 0; j < 4; j++)
			rtk_23[i*4+j] ^= rconst_32_bs[i*4+j];
	}
}

/******************************************************************************
 * Calculation of round tweakeys related to TK1 and TK3
******************************************************************************/
void tk_schedule_13(
    uint32_t rtk_1[TKPERMORDER*BLOCKBYTES/4],
    uint32_t rtk_3[SKINNY128_384_ROUNDS*BLOCKBYTES/4],
    const uint8_t tk_1[TWEAKEYBYTES],
    const uint8_t tk_3[TWEAKEYBYTES])
{
	uint8_t tk_2[TWEAKEYBYTES];
	memset(tk_2, 0x00, TWEAKEYBYTES);
	tk_schedule_1(rtk_1, tk_1);
    tk_schedule_23(rtk_3, tk_2, tk_3);
}

/******************************************************************************
 * Calculation of round tweakeys related to TK1, TK2 and TK3 (full TK schedule)
******************************************************************************/
void tk_schedule_123(
    uint32_t rtk_1[TKPERMORDER*BLOCKBYTES/4],
    uint32_t rtk_23[SKINNY128_384_ROUNDS*BLOCKBYTES/4],
    const uint8_t tk_1[TWEAKEYBYTES],
    const uint8_t tk_2[TWEAKEYBYTES],
    const uint8_t tk_3[TWEAKEYBYTES])
{
	tk_schedule_1(rtk_1, tk_1);
    tk_schedule_23(rtk_23, tk_2, tk_3);
}
