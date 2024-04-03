#ifndef LIGHTEIEGHT_H
#define LIGHTEIEGHT_H


//#define SET_NO_TIE        //use core=A
//#define SET_TIE_SPECIFIC    //use core=B
#define  SET_TIE_BLOCK     //use core=C
//#define SET_TIE_ALGO      //use core=D


#ifdef SET_TIE_SPECIFIC
	#include <xtensa/tie/g.h>
#endif

#ifdef SET_TIE_BLOCK
	#include <xtensa/tie/b.h>
#endif


#ifdef SET_TIE_ALGO
	#include <xtensa/tie/tie_p6.h>
#endif



#endif /*  */
