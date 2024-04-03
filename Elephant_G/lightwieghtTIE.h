/*
 * lightwieghtTIE.h
 *
 *  XXXXXXXXXXXX
 *      XXXXXXXXXXXX
 */

#ifndef LIGHTEIEGHT_H
#define LIGHTEIEGHT_H

// ------------------------ //
// Native Code    --> user with Core A_nc_dev
//#define SET_NO_TIE


// ------------------------ //
// oneslot   --> user with Core B
//#define SET_NO_TIE
// ------------------------ //
#define SET_TIE_SPECIFIC
#include <xtensa/tie/g.h>  // user with Core C

// ------------------------ //
//#define SET_TIE_BLOCK
//#include <xtensa/tie/block_tie.h>

// ------------------------ //
//#include <xtensa/tie/algo_tie.h>
//#define SET_TIE_ALGO

#endif /*  */
