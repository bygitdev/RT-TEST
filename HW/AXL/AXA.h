/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** AXA.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Analog Library Header File
** 
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
** 
** (None)
**
**
*****************************************************************************
*****************************************************************************
**
** Website
** ---------------------
**
** http://www.ajinextek.com
**
*****************************************************************************
*****************************************************************************
*/

#ifndef __AXT_AXA_H__
#define __AXT_AXA_H__

#include "AXHS.h"

#ifdef __cplusplus
extern "C" {

#endif    //__cplusplus
//========== Board and verification API group of module information
    //Verify if AIO module exists
    DWORD    __stdcall AxaInfoIsAIOModule(DWORD *upStatus);
    
    //Verify AIO module number
    DWORD    __stdcall AxaInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    
    //Verify the number of AIO module
    DWORD    __stdcall AxaInfoGetModuleCount(long *lpModuleCount);
    
    //Verify the number of input channels of specified module
    DWORD    __stdcall AxaInfoGetInputCount(long lModuleNo, long *lpCount);
    
    //Verify the number of output channels of specified module
    DWORD    __stdcall AxaInfoGetOutputCount(long lModuleNo, long *lpCount);
    
    //Verify the first channel number of specified module
    DWORD    __stdcall AxaInfoGetChannelNoOfModuleNo(long lModuleNo, long *lpChannelNo);

	// Verify the first Input channel number of specified module (Inputmodule, Integration for input/output Module)
	DWORD    __stdcall AxaInfoGetChannelNoAdcOfModuleNo(long lModuleNo, long *lpChannelNo);

	// Verify the first output channel number of specified module (Inputmodule, Integration for input/output Module)
	DWORD    __stdcall AxaInfoGetChannelNoDacOfModuleNo(long lModuleNo, long *lpChannelNo);
    
    //Verify base board number, module position and module ID with specified module number
    DWORD    __stdcall AxaInfoGetModule (long lModuleNo, long *lpBoardNo, long *lpModulePos, DWORD *upModuleID);

	//Verify Module status of specified module board
    DWORD    __stdcall AxaInfoGetModuleStatus(long lModuleNo);
    
//========== API group of input module information search
    //Verify module number with specified input channel number
    DWORD    __stdcall AxaiInfoGetModuleNoOfChannelNo(long lChannelNo, long *lpModuleNo);
    
    //Verify the number of entire channels of analog input module
    DWORD    __stdcall AxaiInfoGetChannelCount(long *lpChannelCount);

//========== API group for setting and verifying of input module interrupt
    //Use window message, callback API or event method in order to get event message into specified channel. Use for the time of collection action( refer AxaStartMultiChannelAdc ) of continuous data by H/W timer
    //(Timer Trigger Mode, External Trigger Mode)
    DWORD    __stdcall AxaiEventSetChannel(long lChannelNo, HWND hWnd, DWORD uMesssage, AXT_EVENT_PROC pProc, HANDLE *pEvent);
    
    //Set whether to use event in specified input channel
    //======================================================//
    // uUse       : DISABLE(0)    // Event Disable
    //            : ENABLE(1)     // Event Enable
    //======================================================//
    DWORD    __stdcall AxaiEventSetChannelEnable(long lChannelNo, DWORD uUse);
    
    //Verify whether to use event in specified input channel
    //======================================================//
    // *upUse     : DISABLE(0)    // Event Disable
    //            : ENABLE(1)     // Event Enable
    //======================================================//
    DWORD    __stdcall AxaiEventGetChannelEnable(long lChannelNo, DWORD *upUse);
    
    //Set whether to use event in specified multiple input channels
    //======================================================//
    // uUse       : DISABLE(0)    // Event Disable
    //            : ENABLE(1)     // Event Enable
    //======================================================//
    DWORD    __stdcall AxaiEventSetMultiChannelEnable(long lSize, long *lpChannelNo, DWORD uUse);
    
    //Set kind of event in specified input channel
    //======================================================//
    // uMask      : DATA_EMPTY(1)
    //            : DATA_MANY(2)
    //            : DATA_SMALL(3)
    //            : DATA_FULL(4)
    //======================================================//
    DWORD    __stdcall AxaiEventSetChannelMask(long lChannelNo, DWORD uMask);
    
    //Verify kind of event in specified input channel
    //======================================================//
    // *upMask    : DATA_EMPTY(1)
    //            : DATA_MANY(2)
    //            : DATA_SMALL(3)
    //            : DATA_FULL(4)
    //======================================================//
    DWORD    __stdcall AxaiEventGetChannelMask(long lChannelNo, DWORD *upMask);
    
    //Set kind of event in specified multiple input channels
    //==============================================================================//
    // uMask      : DATA_EMPTY(1)
    //            : DATA_MANY(2)
    //            : DATA_SMALL(3)
    //            : DATA_FULL(4)
    //==============================================================================//
    DWORD    __stdcall AxaiEventSetMultiChannelMask(long lSize, long *lpChannelNo, DWORD uMask);
    
    //Verify event occurrence position
    //==============================================================================//
    // *upMode    : AIO_EVENT_DATA_UPPER(1)
    //            : AIO_EVENT_DATA_LOWER(2)
    //            : AIO_EVENT_DATA_FULL(3)
    //            : AIO_EVENT_DATA_EMPTY(4)
    //==============================================================================//
    DWORD    __stdcall AxaiEventRead(long *lpChannelNo, DWORD *upMode);
    
    //Set interrupt mask of specified module. (SIO-AI4RB is not supportive.)
    //==================================================================================================//
    // uMask      : SCAN_END(1)
    //            : FIFO_HALF_FULL(2)
    //==================================================================================================//
    DWORD    __stdcall AxaiInterruptSetModuleMask(long lModuleNo, DWORD uMask);
    
    //Verify interrupt mask of specified module
    //==================================================================================================//
    // *upMask    : SCAN_END(1)
    //            : FIFO_HALF_FULL(2)
    //==================================================================================================//
    DWORD    __stdcall AxaiInterruptGetModuleMask(long lModuleNo, DWORD *upMask);

//========== API group for setting and verifying of input module parameter
    //Set the input voltage range in specified input channel
    //==================================================================================================//
    // AI4RB
    // dMinVolt    : -10V/-5V/0V
    // dMaxVolt    : 10V/5V/
    //
    // AI16Hx
    // dMinVolt    : -10V Fix
    // dMaxVolt    : 10V Fix
    //==================================================================================================//
    DWORD    __stdcall AxaiSetRange(long lChannelNo, double dMinVolt, double dMaxVolt);
    
    //Verify the input voltage range in specified input channel
    //==================================================================================================//
    // AI4RB
    // *dpMinVolt    : -10V/-5V/0V
    // *dpMaxVolt    : 10V/5V/
    //
    // AI16Hx
    // *dpMaxVolt    : -10V Fix
    // *dpMaxVolt    : 10V Fix
    //==================================================================================================//
    DWORD    __stdcall AxaiGetRange(long lChannelNo, double *dpMinVolt, double *dpMaxVolt);

	//Set the allowed input voltage range in specified multiple input Modules
	//==================================================================================================//
	// lModuleNo   : Analog Module Number
	//
	// RTEX AI16F
	// Mode -5~+5  : dMinVolt = -5, dMaxVolt = +5
	// Mode -10~+10: dMinVolt = -10, dMaxVolt = +10
	//==================================================================================================//
	DWORD    __stdcall AxaiSetRangeModule(long lModuleNo, double dMinVolt, double dMaxVolt);

	//Verify the input voltage range in specified input Module
	//==================================================================================================//
	// lModuleNo   : Analog Module Number
	//
	// RTEX AI16F
	// *dMinVolt   : -5V, -10V
	// *dMaxVolt   : +5V, +10V
	//==================================================================================================//
	DWORD    __stdcall AxaiGetRangeModule(long lModuleNo, double *dMinVolt, double *dMaxVolt);
    
    //Set the allowed input voltage range in specified multiple input channels
    //==================================================================================================//
    // AI4RB
    // dMinVolt    : -10V/-5V/0V
    // dMaxVolt    : 10V/5V/
    //
    // AI16Hx
    // dMinVolt    : -10V Fix
    // dMaxVolt    : 10V Fix
    //==================================================================================================//
    DWORD    __stdcall AxaiSetMultiRange(long lSize, long *lpChannelNo, double dMinVolt, double dMaxVolt);
    
    //Set trigger mode in the specified input module
    //==================================================================================================//
    // uTriggerMode : NORMAL_MODE(1) 
    //              : TIMER_MODE(2)
    //              : EXTERNAL_MODE(3)
    //==================================================================================================//
    DWORD    __stdcall AxaiSetTriggerMode(long lModuleNo, DWORD uTriggerMode);
    
    //Verify trigger mode in the specified input module
    //==================================================================================================//
    // *upTriggerMode : NORMAL_MODE(1)
    //                : TIMER_MODE(2)
    //                : EXTERNAL_MODE(3)
    //==================================================================================================//
    DWORD    __stdcall AxaiGetTriggerMode(long lModuleNo, DWORD *upTriggerMode);
    
    //Set offset of specified input module by mVolt (mV) unit. Max -100~100mVolt
    //==================================================================================================//
    // dMiliVolt    : -100 ~ 100 
    //==================================================================================================//
    DWORD    __stdcall AxaiSetModuleOffsetValue(long lModuleNo, double dMiliVolt); 
    
    //Verify offset value of specified input module. mVolt unit(mV)
    //==================================================================================================//
    // *dpMiliVolt    : -100 ~ 100 
    //==================================================================================================//
    DWORD    __stdcall AxaiGetModuleOffsetValue(long lModuleNo, double *dpMiliVolt); 

//==Software Timer (Normal Trigger Mode) group
    //Software Trigger Mode API, Convert analog input value to A/D in the specified input channel by user , then return it in voltage value
    DWORD    __stdcall AxaiSwReadVoltage(long lChannelNo, double *dpVolt);

    //Software Trigger Mode API, Return analog input value in digit value to specified input channel
    DWORD    __stdcall AxaiSwReadDigit(long lChannelNo, DWORD *upDigit);
    
    //Software Trigger Mode API, Return analog input value in voltage value to specified multiple input channels
    DWORD    __stdcall AxaiSwReadMultiVoltage(long lSize, long *lpChannelNo, double *dpVolt);
    
    //Software Trigger Mode API, Return analog input value in digit value to specified multiple input channels
    DWORD    __stdcall AxaiSwReadMultiDigit(long lSize, long *lpChannelNo, DWORD *upDigit);
    
//==Hardware Timer (Timer Trigger Mode + External Trigger Mode) group
    //Hardware Trigger Mode API, Set setting value in order to use immediate mode in specified multiple channels
    DWORD    __stdcall AxaiHwSetMultiAccess(long lSize, long *lpChannelNo, long *lpWordSize);
    
    //Hardware Trigger Mode API, Convert A/D as much as number of specified, then return the voltage value
    DWORD    __stdcall AxaiHwStartMultiAccess(double *dpBuffer);
    
    //Set sampling interval to specified module by frequency unit(Hz)
    //==================================================================================================//
    // dSampleFreq    : 10 ~ 100000 
    //==================================================================================================//
    DWORD    __stdcall AxaiHwSetSampleFreq(long lModuleNo, double dSampleFreq);
    
    //Verify the setting value of sampling interval to specified module by frequency unit(Hz)
    //==================================================================================================//
    // *dpSampleFreq  : 10 ~ 100000 
    //==================================================================================================//
    DWORD    __stdcall AxaiHwGetSampleFreq(long lModuleNo, double *dpSampleFreq);
    
    //Set sampling interval to specified module by time unit (uSec)
    //==================================================================================================//
    // dSamplePeriod  : 100000 ~ 1000000000
    //==================================================================================================//
    DWORD    __stdcall AxaiHwSetSamplePeriod(long lModuleNo, double dSamplePeriod);
    
    //Verify setting value of sampling interval to specified module by time unit(uSec)
    //==================================================================================================//
    // *dpSamplePeriod: 100000 ~ 1000000000
    //==================================================================================================//
    DWORD    __stdcall AxaiHwGetSamplePeriod(long lModuleNo, double *dpSamplePeriod);
    
    //Set control method when the buffer is full in specified input channel
    //==================================================================================================//
    // uFullMode      : NEW_DATA_KEEP(0)
    //                : CURR_DATA_KEEP(1)
    //==================================================================================================//
    DWORD    __stdcall AxaiHwSetBufferOverflowMode(long lChannelNo, DWORD uFullMode);
    
    //Verify control method when the buffer is full in specified input channel
    //==================================================================================================//
    // *upFullMode    : NEW_DATA_KEEP(0)
    //                : CURR_DATA_KEEP(1)
    //==================================================================================================//
    DWORD    __stdcall AxaiHwGetBufferOverflowMode(long lChannelNo, DWORD *upFullMode);
    
    //control method when the buffer is full in specified multiple input channels
    //==================================================================================================//
    // uFullMode      : NEW_DATA_KEEP(0)
    //                : CURR_DATA_KEEP(1)
    //==================================================================================================//
    DWORD    __stdcall AxaiHwSetMultiBufferOverflowMode(long lSize, long *lpChannelNo, DWORD uFullMode);
    
    //Set the upper limit and lower limit of buffer in specified input channel
    DWORD    __stdcall AxaiHwSetLimit(long lChannelNo, long lLowLimit, long lUpLimit);
    
    //Verify the upper limit and lower limit of buffer in specified input channel
    DWORD    __stdcall AxaiHwGetLimit(long lChannelNo, long *lpLowLimit, long *lpUpLimit);
    
    //Set the upper limit and lower limit of buffer in multiple input channels
    DWORD    __stdcall AxaiHwSetMultiLimit(long lSize, long *lpChannelNo, long lLowLimit, long lUpLimit);
    
    //Start A/D conversion using H/W timer in specified multiple channels
    DWORD    __stdcall AxaiHwStartMultiChannel(long lSize, long *lpChannelNo, long lBuffSize);

    DWORD    __stdcall AxaiHwStartSingleChannelAdc(long lChannelNo, long lBuffSize);

    DWORD    __stdcall AxaiHwStopSingleChannelAdc(long lChannelNo);
    
    //After starting of A/D conversion in specified multiple channels, manage filtering as much as specified and return into voltage
    DWORD    __stdcall AxaiHwStartMultiFilter(long lSize, long *lpChannelNo, long lFilterCount, long lBuffSize);
    
    //Stop continuous signal A/D conversion used H/W timer
    DWORD    __stdcall AxaiHwStopMultiChannel(long lModuleNo);
    
    //Inspect the numbers of data in memory buffer of specified input channel
    DWORD    __stdcall AxaiHwReadDataLength(long lChannelNo, long *lpDataLength);
    
    //Read A/D conversion data used H/W timer in specified input channel by voltage value
    DWORD    __stdcall AxaiHwReadSampleVoltage(long lChannelNo, long *lpSize, double *dpVolt);
    
    //Read A/D conversion data used H/W timer in specified input channel by digit value
    DWORD    __stdcall AxaiHwReadSampleDigit(long lChannelNo, long *lpsize, DWORD *upDigit);

//========== API group of input module state check
    //Inspect if there is no data in memory buffer of specified input channel
    //==================================================================================================//
    // *upEmpty   : FALSE(0)
    //            : TRUE(1)
    //==================================================================================================//
    DWORD    __stdcall AxaiHwIsBufferEmpty(long lChannelNo, DWORD *upEmpty);
    
    //Inspect if the data is more than the upper limit specified in memory buffer of specified input channel
    //==================================================================================================//
    // *upUpper   : FALSE(0)
    //            : TRUE(1)
    //==================================================================================================//
    DWORD    __stdcall AxaiHwIsBufferUpper(long lChannelNo, DWORD *upUpper);
    
    //Inspect if the data is less than the upper limit specified in memory buffer of specified input channel
    //==================================================================================================//
    // *upLower   : FALSE(0)
    //            : TRUE(1)
    //==================================================================================================//
    DWORD    __stdcall AxaiHwIsBufferLower(long lChannelNo, DWORD *upLower);
    
    DWORD   __stdcall AxaiExternalStartADC(long lModuleNo, long lSize, long *lpChannelPos);
    DWORD   __stdcall AxaiExternalStopADC(long lModuleNo);
    DWORD   __stdcall AxaiExternalReadFifoStatus(long lModuleNo, DWORD *dwpStatus);
    DWORD   __stdcall AxaiExternalReadVoltage(long lModuleNo, long lSize, long *lpChannelPos, long lDataSize, long lBuffSize, long lStartDataPos, double *dpVolt, long *lpRetDataSize, DWORD *dwpStatus);
//== API group of output module information search
    //Verify module number with specified output channel number
    DWORD    __stdcall AxaoInfoGetModuleNoOfChannelNo(long lChannelNo, long *lpModuleNo);
    
    //Verify entire number of channel of analog output module
    DWORD    __stdcall AxaoInfoGetChannelCount(long *lpChannelCount);
    
//========== API group for output module setting and verification
    //Set output voltage range in specified output channel
    //==================================================================================================//
    // AO4R
    // dMinVolt    : -10V/-5V/0V
    // dMaxVolt    : 10V/5V/
    //
    // AO2Hx
    // dMinVolt    : -10V Fix
    // dMaxVolt    : 10V Fix
    //==================================================================================================//
    DWORD    __stdcall AxaoSetRange(long lChannelNo, double dMinVolt, double dMaxVolt);
    
    //Verify output voltage range in specified output channel
    //==================================================================================================//
    // AO4R
    // dMinVolt    : -10V/-5V/0V
    // dMaxVolt    : 10V/5V/
    //
    // AO2Hx
    // dMinVolt    : -10V Fix
    // dMaxVolt    : 10V Fix
    //==================================================================================================//
    DWORD    __stdcall AxaoGetRange(long lChannelNo, double *dpMinVolt, double *dpMaxVolt);
    
    //Set output voltage range in specified multiple output channels
    //==================================================================================================//
    // AO4R
    // dMinVolt    : -10V/-5V/0V
    // dMaxVolt    : 10V/5V/
    //
    // AO2Hx
    // dMinVolt    : -10V Fix
    // dMaxVolt    : 10V Fix
    //==================================================================================================//
    DWORD    __stdcall AxaoSetMultiRange(long lSize, long *lpChannelNo, double dMinVolt, double dMaxVolt);
    
    //The Input voltage is output in specified output channel
    DWORD    __stdcall AxaoWriteVoltage(long lChannelNo, double dVolt);
    
    //The Input voltage is output in specified multiple output channel
    DWORD    __stdcall AxaoWriteMultiVoltage(long lSize, long *lpChannelNo, double *dpVolt);
    
    //Verify voltage which is output in specified output channel
    DWORD    __stdcall AxaoReadVoltage(long lChannelNo, double *dpVolt);
    
    //Verify voltage which is output in specified multiple output channels
    DWORD    __stdcall AxaoReadMultiVoltage(long lSize, long *lpChannelNo, double *dpVolt);
#ifdef __cplusplus

}
#endif    //__cplusplus

#endif    //__AXT_AXA_H__
