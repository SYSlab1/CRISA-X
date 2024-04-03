/*
 * main.c
 *
  *      XXXXXXXXXXXX
 */
#include <stdio.h>
typedef unsigned int u32;
#define ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))



int P12(u32 C_e,u32 C_o)__attribute__((nonlinear)){
	typedef struct {
	  u32 e;
	  u32 o;
	} u32_2;
	u32_2 x0, x1, x2, x3, x4;
	x0.e = 123; x0.o = 123;x1.e = 23; x1.o = 13;x2.e = 1; x2.o = 123;x4.e = 123; x4.o = 123;

	u32_2 t0, t1, t2, t3, t4;

	for (int ROUNDi=0; ROUNDi<12; ROUNDi++) {

		 /* round constant */
		    x2.e ^= C_e;                      x2.o ^= C_o;
		    /* s-box layer  22 XOR, 10NOT 10 AND  */
		    x0.e ^= x4.e;
		    x0.o ^= x4.o;
		    x4.e ^= x3.e;
		    x4.o ^= x3.o;
		    x2.e ^= x1.e;
		    x2.o ^= x1.o;
		    t0.e = x0.e;
		    t0.o = x0.o;
		    t4.e = x4.e;
		    t4.o = x4.o;
		    t3.e = x3.e;
		    t3.o = x3.o;
		    t1.e = x1.e;
		    t1.o = x1.o;
		    t2.e = x2.e;
		    t2.o = x2.o;
		    x0.e = t0.e ^ (~t1.e & t2.e);
		    x0.o = t0.o ^ (~t1.o & t2.o);
		    x2.e = t2.e ^ (~t3.e & t4.e);
		    x2.o = t2.o ^ (~t3.o & t4.o);
		    x4.e = t4.e ^ (~t0.e & t1.e);
		    x4.o = t4.o ^ (~t0.o & t1.o);
		    x1.e = t1.e ^ (~t2.e & t3.e);
		    x1.o = t1.o ^ (~t2.o & t3.o);
		    x3.e = t3.e ^ (~t4.e & t0.e);
		    x3.o = t3.o ^ (~t4.o & t0.o);
		    x1.e ^= x0.e;
		    x1.o ^= x0.o;
		    x3.e ^= x2.e;
		    x3.o ^= x2.o;
		    x0.e ^= x4.e;
		    x0.o ^= x4.o;
		    /* linear layer 20 XOR, 2 NOT 19SHIFT 19OR */
		    t0.e  = x0.e ^ ROTR32(x0.o, 4);
		    t0.o  = x0.o ^ ROTR32(x0.e, 5);
		    t1.e  = x1.e ^ ROTR32(x1.e, 11);
		    t1.o  = x1.o ^ ROTR32(x1.o, 11);
		    t2.e  = x2.e ^ ROTR32(x2.o, 2);
		    t2.o  = x2.o ^ ROTR32(x2.e, 3);
		    t3.e  = x3.e ^ ROTR32(x3.o, 3);
		    t3.o  = x3.o ^ ROTR32(x3.e, 4);
		    t4.e  = x4.e ^ ROTR32(x4.e, 17);
		    t4.o  = x4.o ^ ROTR32(x4.o, 17);
		    x0.e ^= ROTR32(t0.o, 9);
		    x0.o ^= ROTR32(t0.e, 10);
		    x1.e ^= ROTR32(t1.o, 19);
		    x1.o ^= ROTR32(t1.e, 20);
		    x2.e ^= t2.o;
		    x2.o ^= ROTR32(t2.e, 1);
		    x3.e ^= ROTR32(t3.e, 5);
		    x3.o ^= ROTR32(t3.o, 5);
		    x4.e ^= ROTR32(t4.o, 3);
		    x4.o ^= ROTR32(t4.e, 4);
		    x2.e = ~x2.e;
		    x2.o = ~x2.o;

		    printf("%d",x0.o);
		    printf("%d",x0.e);
		    printf("%d",x1.o);
		    printf("%d",x1.e);
		    printf("%d",x2.o);
		    printf("%d",x2.e);
		    printf("%d",x3.o);
		    		    printf("%d",x3.e);
		    printf("%d",x4.o);
		  		    printf("%d",x4.e);

	}



}


void main () {

	P12(0x6, 0xc);






	}




