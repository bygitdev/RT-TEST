#ifndef _AXISDEF_H_
#define _AXISDEF_H_

#include <windows.h>



enum AXISNAME
{
	MT_INDEX_X_01			= 0,		//     1,    1 (Linear), 700W, +limit,	   , Home
	MT_INDEX_X_02			= 1,		//     1,    1 (Linear), 700W, +limit,	   , Home
	MT_INDEX_X_03			= 2,		//     1,    1 (Linear), 700W, +limit,	   , Home
	MT_INDEX_X_04			= 3,		//     1,    1 (Linear), 700W, +limit,	   , Home
	MT_INDEX_T_01			= 4,		//  0.272834983━,   273, 100w, +limit,	   , Home
	MT_INDEX_T_02			= 5,		//  0.272834983━,   273, 100w, +limit,	   , Home
	MT_INDEX_T_03			= 6,		//  0.272834983━,   273, 100w, +limit,	   , Home
	MT_INDEX_T_04			= 7,		//  0.272834983━,   273, 100w, +limit,	   , Home
	MT_ROUTER_Y_01			= 8,		//     1,    1 (Linear), 700W, +limit,	   , Home
	MT_ROUTER_Y_02			= 9,		//     1,    1 (Linear), 400W, +limit,	   , Home
	MT_ROUTER_W_01			= 10,		//   5mm,			 5000, 100W,       ,	   , Home
	MT_ROUTER_W_02			= 11,		//   5mm,			 5000, 100W,       ,	   , Home
	MT_SPINDLE_Z_01			= 12,		//   5mm,			 5000, 100w, +limit,	   , Home
	MT_SPINDLE_Z_02			= 13,		//   5mm,		     5000, 100w, +limit,	   , Home       
	MT_SPINDLE_Z_03			= 14,		//   5mm,		     5000, 100w, +limit,	   , Home       
	MT_SPINDLE_Z_04			= 15,		//   5mm,		     5000, 100w, +limit,	   , Home   
	MT_MGZ_LOAD_Z			= 16,		//  10mm,		    10000, 400W, +limit, -limit, Home
	MT_PUSHER_X				= 17,		// 9.6mm,			 9600, 100W, +limit,	   , Home
	MT_RAIL_GRIPPER_X		= 18,		//  20mm,			20000, 100W, +limit, -limit, Home		
	MT_LD_Y					= 19,		//  10mm,			10000, 400W, +limit, -limit, Home
	MT_LD_Z					= 20,		//  20mm,			20000, 400W, +limit, -limit, Home
	MT_OUTPNP_Y				= 21,		//     1,    1 (Linear) 嘘端 //  20mm,		    20000, 400W, +limit, -limit, Home
	MT_OUTPNP_Z				= 22,		//  10mm,		    10000, 400W, +limit, -limit, Home
	MT_OUTPNP_X				= 23,		//   5mm,		     5000, 100W, +limit,       , Home
	MT_INPNP_CLAMP_Y		= 24,		//   2mm,			 4000, 100w,       ,	   , Home
	MT_INPNP_Y				= 25,		//  20mm,			20000, 400w, +limit, -limit, Home
	MT_INPNP_Z				= 26,		//  10mm,			10000, 200w, +limit, -limit, Home
	MT_ADC_Z 				= 27,		//  20mm,		    20000, 100w, +limit,	   , Home
	MT_ADC_X				= 28,

	MAX_MT_NO				= 29,
};



#endif//_AXISDEF_H_
