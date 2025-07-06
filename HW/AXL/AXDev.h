#ifndef __AXT_AXDEV_H__
#define __AXT_AXDEV_H__

#include "AXHD.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

    // Use Board Number and find Board Address
    DWORD    __stdcall AxlGetBoardAddress(long lBoardNo, DWORD *upBoardAddress);
    // Use Board Number and find Board ID
    DWORD    __stdcall AxlGetBoardID(long lBoardNo, DWORD *upBoardID);
    // Use Board Number and find Board Version
    DWORD    __stdcall AxlGetBoardVersion(long lBoardNo, DWORD *upBoardVersion);
    // Use Board Number/Module Position and find Module ID
    DWORD    __stdcall AxlGetModuleID(long lBoardNo, long lModulePos, DWORD *upModuleID);
    // Use Board Number/Module Position and find Module Version
    DWORD    __stdcall AxlGetModuleVersion(long lBoardNo, long lModulePos, DWORD *upModuleVersion);
    
    DWORD    __stdcall AxlGetModuleNodeInfo(long lBoardNo, long lModulePos, long *upNetNo, DWORD *upNodeAddr);
 
    // Only for PCI-R1604[RTEX]
    // Writing user data to embedded flash memory
    // lPageAddr(0 ~ 199)
    // lByteNum(1 ~ 120)
    // Note) Delay time is required for completing writing operation to Flash(Max. 17mSec)
    DWORD    __stdcall AxlSetDataFlash(long lBoardNo, long lPageAddr, long lBytesNum, BYTE *bpSetData);

    // Reading datas from embedded flash memory
    // lPageAddr(0 ~ 199)
    // lByteNum(1 ~ 120)
    DWORD    __stdcall AxlGetDataFlash(long lBoardNo, long lPageAddr, long lBytesNum, BYTE *bpGetData);

    // Use Board and Set the EStopInterLock Parameter
    DWORD    __stdcall AxlSetEStopInterLock(long lBoardNo, DWORD dwInterLock, DWORD dwDigFilterVal);
    // Use Board and Get the EStopInterLock Parameter
    DWORD    __stdcall AxlGetEStopInterLock(long lBoardNo, DWORD *dwInterLock, DWORD  *dwDigFilterVal);

    // Use Board Number/Module Position and find AIO Module Number
    DWORD    __stdcall AxaInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);
    // Use Board Number/Module Position and find DIO Module Number
    DWORD    __stdcall AxdInfoGetModuleNo(long lBoardNo, long lModulePos, long *lpModuleNo);

    // IPCOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommand(long lAxisNo, IPCOMMAND sCommand);
    // 8bit IPCOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData08(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // Get 8bit IPCOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData08(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // 16bit IPCOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData16(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // Get 16bit IPCOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData16(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // 24bit IPCOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData24(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // Get 24bit IPCOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData24(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    // 32bit IPCOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData32(long lAxisNo, IPCOMMAND sCommand, DWORD uData);
    // Get 32bit IPCOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData32(long lAxisNo, IPCOMMAND sCommand, DWORD *upData);
    
    // QICOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandQi(long lAxisNo, QICOMMAND sCommand);
    // 8bit QICOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData08Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // Get 8bit QICOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData08Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // 16bit QICOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData16Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // Get 16bit QICOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData16Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // 24bit QICOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData24Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // Get 24bit QICOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData24Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);
    // 32bit QICOMMAND Setting at an appoint axis
    DWORD    __stdcall AxmSetCommandData32Qi(long lAxisNo, QICOMMAND sCommand, DWORD uData);
    // Get 32bit QICOMMAND at an appoint axis
    DWORD    __stdcall AxmGetCommandData32Qi(long lAxisNo, QICOMMAND sCommand, DWORD *upData);

    // Get Port Data at an appoint axis - IP
    DWORD    __stdcall AxmGetPortData(long lAxisNo,  WORD wOffset, DWORD *upData);
    // Port Data Setting at an appoint axis - IP
    DWORD    __stdcall AxmSetPortData(long lAxisNo, WORD wOffset, DWORD dwData);
    // Get Port Data at an appoint axis - QI
    DWORD    __stdcall AxmGetPortDataQi(long lAxisNo, WORD byOffset, WORD *wData);
    // Port Data Setting at an appoint axis - QI
    DWORD    __stdcall AxmSetPortDataQi(long lAxisNo, WORD byOffset, WORD wData);
        
    // Set the script at an appoint axis.  - IP
    // sc    : Script number (1 - 4)
    // event : Define an event SCRCON to happen.
    //         Set event, a number of axis, axis which the event happens, event content 1, 2 attribute
    // cmd   : Define a selection SCRCMD however we change any content
    // data  : Selection to change any Data.
    DWORD    __stdcall AxmSetScriptCaptionIp(long lAxisNo, long sc, DWORD event, DWORD data);
    // Return the script at an appoint axis.  - IP
    DWORD    __stdcall AxmGetScriptCaptionIp(long lAxisNo, long sc, DWORD *event, DWORD *data);

    // Set the script at an appoint axis.  - QI
    // sc    : Script number (1 - 4)
    // event : Define an event SCRCON to happen.
    //         Set event, a number of axis, axis which the event happens, event content 1, 2 attribute
    // cmd   : Define a selection SCRCMD however we change any content
    // data  : Selection to change any Data.
    DWORD    __stdcall AxmSetScriptCaptionQi(long lAxisNo, long sc, DWORD event, DWORD cmd, DWORD data);
    // Return the script at an appoint axis. - QI
    DWORD    __stdcall AxmGetScriptCaptionQi(long lAxisNo, long sc, DWORD *event, DWORD *cmd, DWORD *data);

    // Clear orders a script inside Queue Index at an appoint axis
    // uSelect IP. 
    // uSelect(0): Script Queue Index Clear.
    //        (1): Caption Queue Index Clear.
    // uSelect QI. 
    // uSelect(0): Script Queue 1 Index Clear.
    //        (1): Script Queue 2 Index Clear.
    DWORD    __stdcall AxmSetScriptCaptionQueueClear(long lAxisNo, DWORD uSelect);
    
    // Return Index of a script inside Queue at an appoint axis.
    // uSelect IP
    // uSelect(0): Read Script Queue Index
    //        (1): Read Caption Queue Index
    // uSelect QI. 
    // uSelect(0): Read Script Queue 1 Index
    //        (1): Read Script Queue 2 Index
    DWORD    __stdcall AxmGetScriptCaptionQueueCount(long lAxisNo, DWORD *updata, DWORD uSelect);

    // Return Data number of a script inside Queue at an appoint axis.
    // uSelect IP
    // uSelect(0): Read Script Queue Data
    //        (1): Read Caption Queue Data
    // uSelect QI.
    // uSelect(0): Read Script Queue 1 Data
    //        (1): Read Script Queue 2 Data
    DWORD    __stdcall AxmGetScriptCaptionQueueDataCount(long lAxisNo, DWORD *updata, DWORD uSelect);

    // Read an inside data.
    DWORD    __stdcall AxmGetOptimizeDriveData(long lAxisNo, double dMinVel, double dVel, double dAccel, double  dDecel, 
            WORD *wRangeData, WORD *wStartStopSpeedData, WORD *wObjectSpeedData, WORD *wAccelRate, WORD *wDecelRate);

    // Setting up confirmes the register besides within the board by Byte.
    DWORD    __stdcall AxmBoardWriteByte(long lBoardNo, WORD wOffset, BYTE byData);
    DWORD    __stdcall AxmBoardReadByte(long lBoardNo, WORD wOffset, BYTE *byData);

    // Setting up confirmes the register besides within the board by Word.
    DWORD    __stdcall AxmBoardWriteWord(long lBoardNo, WORD wOffset, WORD wData);
    DWORD    __stdcall AxmBoardReadWord(long lBoardNo, WORD wOffset, WORD *wData);

    // Setting up confirmes the register besides within the board by DWord.
    DWORD    __stdcall AxmBoardWriteDWord(long lBoardNo, WORD wOffset, DWORD dwData);
    DWORD    __stdcall AxmBoardReadDWord(long lBoardNo, WORD wOffset, DWORD *dwData);

    // Setting up confirmes the register besides within the Module by Byte.
    DWORD    __stdcall AxmModuleWriteByte(long lBoardNo, long lModulePos, WORD wOffset, BYTE byData);
    DWORD    __stdcall AxmModuleReadByte(long lBoardNo, long lModulePos, WORD wOffset, BYTE *byData);

    // Setting up confirmes the register besides within the Module by Word.
    DWORD    __stdcall AxmModuleWriteWord(long lBoardNo, long lModulePos, WORD wOffset, WORD wData);
    DWORD    __stdcall AxmModuleReadWord(long lBoardNo, long lModulePos, WORD wOffset, WORD *wData);

    // Setting up confirmes the register besides within the Module by DWord.
    DWORD    __stdcall AxmModuleWriteDWord(long lBoardNo, long lModulePos, WORD wOffset, DWORD dwData);
    DWORD    __stdcall AxmModuleReadDWord(long lBoardNo, long lModulePos, WORD wOffset, DWORD *dwData);

    // Set EXCNT (Pos = Unit)
    DWORD    __stdcall AxmStatusSetActComparatorPos(long lAxisNo, double dPos);
    // Return EXCNT (Positon = Unit)
    DWORD    __stdcall AxmStatusGetActComparatorPos(long lAxisNo, double *dpPos);

    // Set INCNT (Pos = Unit)
    DWORD    __stdcall AxmStatusSetCmdComparatorPos(long lAxisNo, double dPos);
    // Return INCNT (Pos = Unit)
    DWORD    __stdcall AxmStatusGetCmdComparatorPos(long lAxisNo, double *dpPos);
    
//=========== Append function. =========================================================================================================
    // Increase a straight line interpolation at speed to the infinity.
    // Must put the distance speed rate.
    DWORD    __stdcall AxmLineMoveVel(long lCoord, double dVel, double dAccel, double dDecel);

//========= Sensor drive API( Read carefully: Available only PI , No function in QI) =========================================================================
    // Set mark signal( used in sensor positioning drive)
    DWORD    __stdcall AxmSensorSetSignal(long lAxisNo, DWORD uLevel);
    // Verify mark signal( used in sensor positioning drive)
    DWORD    __stdcall AxmSensorGetSignal(long lAxisNo, DWORD *upLevel);
    // Verify mark signal( used in sensor positioning drive)state
    DWORD    __stdcall AxmSensorReadSignal(long lAxisNo, DWORD *upStatus);
    
    // Drive API which moves from edge detection of sensor input pin during velocity mode driving as much as specified position, then stop. Applied motion is started upon the start of API, and escapes from the API after the motion is completed.
    DWORD    __stdcall AxmSensorMovePos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lMethod);

    // Drive API which moves from edge detection of sensor input pin during velocity mode driving as much as specified position, then stop. Applied motion is started upon the start of API, then escapes from the API immediately without waiting until the motion is completed.
    DWORD    __stdcall AxmSensorStartMovePos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lMethod);

    DWORD    __stdcall AxmHomeGetStepTrace(long lAxisNo, long *lpStepCount, DWORD *upMainStepNumber, DWORD *upStepNumber, DWORD *upStepBranch);
    
//======= Additive home search (Applicable to PI-N804/404  only) =================================================================================

    // Set home setting parameters of axis specified by user. (Use exclusive-use register for QI chip).
    // uZphasCount : Z phase count after home completion. (0 - 15)
    // lHomeMode   : Home setting mode( 0 - 12)
    // lClearSet   : Select use of position clear and remaining pulse clear (0 - 3)
    //               0: No use of position clear, no use of remaining pulse clear
    //               1: use of position clear, no use of remaining pulse clear
    //               2: No use of position clear, use of remaining pulse clear
    //               3: use of position clear, use of remaining pulse clear
    // dOrgVel : Set Org  Speed related home
    // dLastVel: Set Last Speed related home
    DWORD    __stdcall AxmHomeSetConfig(long lAxisNo, DWORD uZphasCount, long lHomeMode, long lClearSet, double dOrgVel, double dLastVel, double dLeavePos);
    // Return home setting parameters of axis specified by user.
    DWORD    __stdcall AxmHomeGetConfig(long lAxisNo, DWORD *upZphasCount, long *lpHomeMode, long *lpClearSet, double *dpOrgVel, double *dpLastVel, double *dpLeavePos); //KKJ(070215)
    
    // Start home search of axis specified by user
    // Set when use lHomeMode : Set 0 - 5 (Start search after Move Return.)
    // If lHomeMode -1is used as it is, the setting is done as used in HomeConfig.
    // Move direction      : CW when Vel value is positive, CCW when negative.
    DWORD    __stdcall AxmHomeSetMoveSearch(long lAxisNo, double dVel, double dAccel, double dDecel);

    // Start home return of axis specified by user.
    // Set when lHomeMode is used: set 0 - 12  
    // If lHomeMode -1is used as it is, the setting is done as used in HomeConfig.
    // Move direction      : CW when Vel value is positive, CCW when negative.
    DWORD    __stdcall AxmHomeSetMoveReturn(long lAxisNo, double dVel, double dAccel, double dDecel);
    
    // Home separation of axis specified by user is started. 
    // Move direction      : CW when Vel value is positive, CCW when negative.
    DWORD    __stdcall AxmHomeSetMoveLeave(long lAxisNo, double dVel, double dAccel, double dDecel);

    // User start home search of multi-axis specified by user. 
    // Set when use lHomeMode : Set 0 - 5 (Start search after Move Return.)
    // If lHomeMode -1is used as it is, the setting is done as used in HomeConfig.
    // Move direction      : CW when Vel value is positive, CCW when negative.
    DWORD    __stdcall AxmHomeSetMultiMoveSearch(long lArraySize, long *lpAxesNo, double *dpVel, double *dpAccel, double *dpDecel);

    //Set move velocity profile mode of specific coordinate system. 
    // (caution : Available to use only after axis mapping)
    //ProfileMode : '0' - symmetry Trapezoid
    //              '1' - asymmetric Trapezoid
    //              '2' - symmetry Quasi-S Curve
    //              '3' - symmetry S Curve
    //              '4' - asymmetric S Curve
    DWORD    __stdcall AxmContiSetProfileMode(long lCoord, DWORD uProfileMode);
    // Return move velocity profile mode of specific coordinate system.
    DWORD    __stdcall AxmContiGetProfileMode(long lCoord, DWORD *upProfileMode);

    //========== Reading group for input interrupt occurrence flag
    // Reading the interrupt occurrence state by bit unit in specified input contact module and Offset position of Interrupt Flag Register
    DWORD    __stdcall AxdiInterruptFlagReadBit(long lModuleNo, long lOffset, DWORD *upValue);
    // Reading the interrupt occurrence state by byte unit in specified input contact module and Offset position of Interrupt Flag Register
    DWORD    __stdcall AxdiInterruptFlagReadByte(long lModuleNo, long lOffset, DWORD *upValue);
    // Reading the interrupt occurrence state by word unit in specified input contact module and Offset position of Interrupt Flag Register
    DWORD    __stdcall AxdiInterruptFlagReadWord(long lModuleNo, long lOffset, DWORD *upValue);
    // Reading the interrupt occurrence state by double word unit in specified input contact module and Offset position of Interrupt Flag Register
    DWORD    __stdcall AxdiInterruptFlagReadDword(long lModuleNo, long lOffset, DWORD *upValue);
    // Reading the interrupt occurrence state by bit unit in entire input contact module and Offset position of Interrupt Flag Register
    DWORD    __stdcall AxdiInterruptFlagRead(long lOffset, DWORD *upValue);

    //========= API related log ==========================================================================================   
    // This API sets or resets in order to monitor the API execution result of set axis in EzSpy. 
    // uUse : use or not use => DISABLE(0), ENABLE(1)
    DWORD    __stdcall AxmLogSetAxis(long lAxisNo, DWORD uUse);
    
    // This API verifies if the API execution result of set axis is monitored in EzSpy. 
    DWORD    __stdcall AxmLogGetAxis(long lAxisNo, DWORD *upUse);

    //==Log
    // Set whether execute log output on EzSpy of specified module
    DWORD    __stdcall AxdLogSetModule(long lModuleNo, DWORD uUse);
    // Verify whether execute log output on EzSpy of specified module
    DWORD    __stdcall AxdLogGetModule(long lModuleNo, DWORD *upUse);
    // Set whether to log output to EzSpy of specified input channel
    DWORD    __stdcall AxaiLogSetChannel(long lChannelNo, DWORD uUse);
    // Verify whether to log output to EzSpy of specified input channel
    DWORD    __stdcall AxaiLogGetChannel(long lChannelNo, DWORD *upUse);
    
    // Set whether to log output in EzSpy of specified output channel
    DWORD    __stdcall AxaoLogSetChannel(long lChannelNo, DWORD uUse);
    // Verify whether log output is done in EzSpy of specified output channel.
    DWORD    __stdcall AxaoLogGetChannel(long lChannelNo, DWORD *upUse);

    DWORD    __stdcall AxdLogSetModule(long lModuleNo, DWORD uUse);

    DWORD    __stdcall AxdLogGetModule(long lModuleNo, DWORD *upUse);
    
    // Verify whether to firmware version designated RTEX board.
    DWORD    __stdcall AxlGetFirmwareVersion(long lBoardNo, PTCHAR szVersion);
    // Sent to firmware designated board.
    DWORD    __stdcall AxlSetFirmwareCopy(long lBoardNo, WORD *wData, WORD *wCmdData);
    // Execute Firmware update to designated board. 
    DWORD    __stdcall AxlSetFirmwareUpdate(long lBoardNo);
    // Verify whether currently RTEX status Initialized.
    DWORD    __stdcall AxlCheckStatus(long lBoardNo, DWORD* dwStatus);
    // To Initialized currently RTEX on designated board.
    DWORD    __stdcall AxlInitRtex(long lBoardNo, DWORD dwOption);
    // Execute universal command designated axis of board.
    DWORD    __stdcall AxlRtexUniversalCmd(long lBoardNo, WORD wCmd, WORD wOffset, WORD *wData);
    // Execute RTEX communication command designated axis.
    DWORD    __stdcall AxmRtexSlaveCmd(long lAxisNo, DWORD dwCmdCode, DWORD dwTypeCode, DWORD dwIndexCode, DWORD dwCmdConfigure, DWORD dwValue);
    // Verify whether Result of RTEX communication command designated axis.
    DWORD    __stdcall AxmRtexGetSlaveCmdResult(long lAxisNo, DWORD* dwIndex, DWORD *dwValue);
    // Verify whether RTEX status information designated axis.
    DWORD    __stdcall AxmRtexGetAxisStatus(long lAxisNo, DWORD *dwStatus);
    // Verify whether RTEX communication return information designated axis.(Actual position, Velocity, Torque)
    DWORD    __stdcall AxmRtexGetAxisReturnData(long lAxisNo,  DWORD *dwReturn1, DWORD *dwReturn2, DWORD *dwReturn3);
    // Verify whether currently status information of RTEX slave axis.(mechanical, Inposition and etc)
    DWORD    __stdcall AxmRtexGetAxisSlaveStatus(long lAxisNo,  DWORD *dwStatus);

    DWORD    __stdcall AxmSetAxisCmd(long lAxisNo, DWORD* tagCommand);
    DWORD    __stdcall AxmGetAxisCmdResult(long lAxisNo, DWORD* tagCommand);

    DWORD    __stdcall AxlGetDpRamData(long lBoardNo, WORD wAddress, DWORD* dwpRdData);
    DWORD    __stdcall AxlBoardReadDpramWord(long lBoardNo, WORD wOffset, DWORD* dwpRdData);

    DWORD   __stdcall AxlBoardWriteDpramWord(long lBoardNo, WORD wOffset, DWORD dwWrData);

    DWORD   __stdcall AxlSetSendBoardEachCommand(long lBoardNo, DWORD dwCommand, DWORD* dwpSendData, DWORD dwLength);

    DWORD    __stdcall AxlSetSendBoardCommand(long lBoardNo, DWORD dwCommand, DWORD* dwpSendData, DWORD dwLength);
    DWORD    __stdcall AxlGetResponseBoardCommand(long lBoardNo, DWORD* dwpReadData);
    DWORD    __stdcall AxmInfoGetFirmwareVersion(long lAxisNo, unsigned char* ucaFirmwareVersion);
    DWORD    __stdcall AxaInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);
    DWORD    __stdcall AxdInfoGetFirmwareVersion(long lModuleNo, unsigned char* ucaFirmwareVersion);

    //======== Only for PCI-R1604-MLII =========================================================================== 
	DWORD    __stdcall AxmSetTorqFeedForward(long lAxisNo, DWORD dwTorqFeedForward);
	DWORD    __stdcall AxmGetTorqFeedForward(long lAxisNo, DWORD* dwpTorqFeedForward);
	DWORD    __stdcall AxmSetVelocityFeedForward(long lAxisNo, DWORD dwVelocityFeedForward);
	DWORD    __stdcall AxmGetVelocityFeedForward(long lAxisNo, DWORD* dwpVelocityFeedForward);

    // Set Encoder type.
    // Default value : 0(TYPE_INCREMENTAL)
    // Setting range : 0 ~ 1
    // dwEncoderType : 0(TYPE_INCREMENTAL), 1(TYPE_ABSOLUTE).
    DWORD   __stdcall AxmSignalSetEncoderType(long lAxisNo, DWORD dwEncoderType);

    // Verify Encoder type.
    DWORD   __stdcall AxmSignalGetEncoderType(long lAxisNo, DWORD* dwpEncoderType);

    // For updating the slave firmware(only for RTEX-PM).
    //DWORD   __stdcall AxmSetSendAxisCommand(long lAxisNo, WORD wCommand, WORD* wpSendData, WORD wLength);

    //======== Only for PCI-R1604-RTEX, RTEX-PM============================================================== 
    // When Input Universal Input 2, 3, Set Jog move velocity
    // Set only once execute after all drive setting (Ex, PulseOutMethod, MoveUnitPerPulse etc..)
    DWORD    __stdcall AxmMotSetUserMotion(long lAxisNo, double dVelocity, double dAccel, double dDecel);

    // When Input Universal Input 2, 3, Set Jog move usage
    // Setting value :  0(DISABLE), 1(ENABLE)
    DWORD    __stdcall AxmMotSetUserMotionUsage(long lAxisNo, DWORD dwUsage);

    // Set Load/UnLoad Position to Automatically move use MPGP Input 
    DWORD    __stdcall  AxmMotSetUserPosMotion(long lAxisNo, double dVelocity, double dAccel, double dDecel, double dLoadPos, double dUnLoadPos, DWORD dwFilter, DWORD dwDelay);
	
    // Set Usage Load/UnLoad Position to Automatically move use MPGP Input 
    // Setting value :  0(DISABLE), 1(Position function A), 2(Position function B)
    DWORD    __stdcall  AxmMotSetUserPosMotionUsage(long lAxisNo, DWORD dwUsage);
    //======================================================================================================== 

    //======== SIO-CN2CH, Only for absolute position trigger module(B0) =======================================
    DWORD  __stdcall AxcKeWriteRamDataAddr(long lChannelNo, DWORD dwAddr, DWORD dwData);
    DWORD  __stdcall AxcKeReadRamDataAddr(long lChannelNo, DWORD dwAddr, DWORD* dwpData);
    DWORD  __stdcall AxcKeResetRamDataAll(long lModuleNo, DWORD dwData);
    DWORD  __stdcall AxcTriggerSetTimeout(long lChannelNo, DWORD dwTimeout);
    DWORD  __stdcall AxcTriggerGetTimeout(long lChannelNo, DWORD* dwpTimeout);
    DWORD  __stdcall AxcStatusGetWaitState(long lChannelNo, DWORD* dwpState);
    DWORD  __stdcall AxcStatusSetWaitState(long lChannelNo, DWORD dwState);

    DWORD  __stdcall AxcKeSetCommandData32(long lChannelNo, DWORD dwCommand, DWORD dwData);
    DWORD  __stdcall AxcKeSetCommandData16(long lChannelNo, DWORD dwCommand, WORD wData);
    DWORD  __stdcall AxcKeGetCommandData32(long lChannelNo, DWORD dwCommand, DWORD *dwpData);
    DWORD  __stdcall AxcKeGetCommandData16(long lChannelNo, DWORD dwCommand, WORD *wpData);
//======================================================================================================== 

	//======== Only for PCI-N804/N404, Sequence Motion ===========================================================
	// Set Axis Information of sequence Motion (min 1axis)
	// lSeqMapNo : Sequence Motion Index Point
	// lSeqMapSize : Number of axis
	// long* LSeqAxesNo : Number of arrary
    DWORD   __stdcall AxmSeqSetAxisMap(long lSeqMapNo, long lSeqMapSize, long* lSeqAxesNo);
    DWORD   __stdcall AxmSeqGetAxisMap(long lSeqMapNo, long* lSeqMapSize, long* lSeqAxesNo);

	// Set Standard(Master)Axis of Sequence Motion.
	// By all means Set in AxmSeqSetAxisMap setting axis.
    DWORD   __stdcall AxmSeqSetMasterAxisNo(long lSeqMapNo, long lMasterAxisNo);

	// Notifies the library node start loading of Sequence Motion.
    DWORD   __stdcall AxmSeqBeginNode(long lSeqMapNo);

	// Notifies the library node end loading of Sequence Motion.
    DWORD   __stdcall AxmSeqEndNode(long lSeqMapNo);

	// Start Sequence Motion Move.
    DWORD   __stdcall AxmSeqStart(long lSeqMapNo, DWORD dwStartOption);

	// Set each profile node Information of Sequence Motion in Library.
	// if used 1axis Sequence Motion, Must be Set *dPosition one Array.
    DWORD   __stdcall AxmSeqAddNode(long lSeqMapNo, double* dPosition, double dVelocity, double dAcceleration, double dDeceleration, double dNextVelocity);

	// Return Node Index number of Sequence Motion.
    DWORD   __stdcall AxmSeqGetNodeNum(long lSeqMapNo, long* lCurNodeNo);

	// Return All node count of Sequence Motion.
    DWORD   __stdcall AxmSeqGetTotalNodeNum(long lSeqMapNo, long* lTotalNodeCnt);

	// Return Sequence Motion drive status  of specific SeqMap.
	// dwInMotion : 0(Drive end), 1(In drive)
    DWORD   __stdcall AxmSeqIsMotion(long lSeqMapNo, DWORD* dwInMotion);

	// Clear Sequence Motion Memory.
    DWORD   __stdcall AxmSeqWriteClear(long lSeqMapNo);

	// Stop sequence motion
	// dwStopMode : 0(EMERGENCY_STOP), 1(SLOWDOWN_STOP) 
    DWORD   __stdcall AxmSeqStop(long lSeqMapNo, DWORD dwStopMode);
	//======================================================================================================== 

    DWORD   __stdcall AxmStatusSetMon(long lAxisNo, DWORD dwParaNo1, DWORD dwParaNo2, DWORD dwParaNo3, DWORD dwParaNo4, DWORD dwUse);
    DWORD   __stdcall AxmStatusGetMon(long lAxisNo, DWORD *dwpParaNo1, DWORD *dwpParaNo2, DWORD *dwpParaNo3, DWORD *dwpParaNo4, DWORD *dwpUse);
    DWORD   __stdcall AxmStatusReadMon(long lAxisNo, DWORD *dwpParaNo1, DWORD *dwpParaNo2, DWORD *dwpParaNo3, DWORD *dwpParaNo4);
    DWORD   __stdcall AxlSetIoPort(long lBoardNo, DWORD dwAddr, DWORD dwData);
    DWORD   __stdcall AxlGetIoPort(long lBoardNo, DWORD dwAddr, DWORD* dwpData);

    DWORD   __stdcall AxlM3SetFWUpdateInit(long lBoardNo, DWORD dwTotalPacketSize, DWORD dwProcTotalStepNo);
    DWORD   __stdcall AxlM3GetFWUpdateInit(long lBoardNo, DWORD *dwTotalPacketSize, DWORD *dwProcTotalStepNo);

    DWORD   __stdcall AxlM3SetFWUpdateCopy(long lBoardNo, DWORD *pdwPacketData, DWORD dwPacketSize);
    DWORD   __stdcall AxlM3GetFWUpdateCopy(long lBoardNo, DWORD *dwPacketSize);

    DWORD   __stdcall AxlM3SetFWUpdate(long lBoardNo, DWORD dwFlashBurnStepNo);
    DWORD   __stdcall AxlM3GetFWUpdate(long lBoardNo, DWORD *dwFlashBurnStepNo, DWORD *dwIsFlashBurnDone);

    DWORD   __stdcall AxlM3SetCFGData(long lBoardNo, DWORD *pCmdData, DWORD CmdDataSize);
    DWORD   __stdcall AxlM3GetCFGData(long lBoardNo, DWORD *pCmdData, DWORD CmdDataSize);

    DWORD   __stdcall AxlM3SetMCParaUpdateInit(long lBoardNo, WORD wCh0Slaves, WORD wCh1Slaves, DWORD dwCh0CycTime, DWORD dwCh1CycTime, DWORD dwChInfoMaxRetry);
    DWORD   __stdcall AxlM3GetMCParaUpdateInit(long lBoardNo, WORD *wCh0Slaves, WORD *wCh1Slaves, DWORD *dwCh0CycTime, DWORD *dwCh1CycTime, DWORD *dwChInfoMaxRetry);
    DWORD   __stdcall AxlM3SetMCParaUpdateCopy(long lBoardNo, WORD wIdx, WORD wChannel, WORD wSlaveAddr, DWORD dwProtoCalType, DWORD dwTransBytes, DWORD dwDeviceCode);
    DWORD   __stdcall AxlM3GetMCParaUpdateCopy(long lBoardNo, WORD wIdx, WORD *wChannel, WORD *wSlaveAddr, DWORD *dwProtoCalType, DWORD *dwTransBytes, DWORD *dwDeviceCode);

    DWORD   __stdcall AxlBoardReadDWord(long lBoardNo, WORD wOffset, DWORD *dwData);
    DWORD   __stdcall AxlBoardWriteDWord(long lBoardNo, WORD wOffset, DWORD dwData);

    DWORD    __stdcall AxlBoardReadDWordEx(long lBoardNo, DWORD dwOffset, DWORD *dwData);
    DWORD    __stdcall AxlBoardWriteDWordEx(long lBoardNo, DWORD dwOffset, DWORD dwData);

    DWORD   __stdcall AxmM3ServoSetCtrlStopMode(long lAxisNo, BYTE bStopMode);
    DWORD   __stdcall AxmM3ServoSetCtrlLtSel(long lAxisNo, BYTE bLtSel1, BYTE bLtSel2);
    DWORD   __stdcall AxmStatusReadServoCmdIOInput(long lAxisNo, DWORD *upStatus);    
    DWORD   __stdcall AxmM3ServoExInterpolate(long lAxisNo, DWORD dwTPOS, DWORD dwVFF, DWORD dwTFF, DWORD dwTLIM, DWORD dwExSig1, DWORD dwExSig2);
    DWORD   __stdcall AxmM3ServoSetExpoAccBias(long lAxisNo, WORD wBias);
    DWORD   __stdcall AxmM3ServoSetExpoAccTime(long lAxisNo, WORD wTime);
    DWORD   __stdcall AxmM3ServoSetMoveAvrTime(long lAxisNo, WORD wTime);
    DWORD   __stdcall AxmM3ServoSetAccFilter(long lAxisNo, BYTE bAccFil);
    DWORD   __stdcall AxmM3ServoSetCprmMonitor1(long lAxisNo, BYTE bMonSel);
    DWORD   __stdcall AxmM3ServoSetCprmMonitor2(long lAxisNo, BYTE bMonSel);
    DWORD   __stdcall AxmM3ServoStatusReadCprmMonitor1(long lAxisNo, DWORD *upStatus);
    DWORD   __stdcall AxmM3ServoStatusReadCprmMonitor2(long lAxisNo, DWORD *upStatus);
    DWORD   __stdcall AxmM3ServoSetAccDec(long lAxisNo, WORD wAcc1, WORD wAcc2, WORD wAccSW, WORD wDec1, WORD wDec2, WORD wDecSW);
    DWORD   __stdcall AxmM3ServoSetStop(long lAxisNo, long lMaxDecel);

    DWORD   __stdcall AxlM3GetStationParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    DWORD   __stdcall AxlM3SetStationParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    DWORD   __stdcall AxlM3GetStationIdRd(long lBoardNo, long lModuleNo, BYTE bIdCode, BYTE bOffset, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    DWORD   __stdcall AxlM3SetStationNop(long lBoardNo, long lModuleNo, BYTE bModuleType);
    DWORD   __stdcall AxlM3SetStationConfig(long lBoardNo, long lModuleNo, BYTE bConfigMode, BYTE bModuleType);
    DWORD   __stdcall AxlM3GetStationAlarm(long lBoardNo, long lModuleNo, WORD wAlarmRdMod, WORD wAlarmIndex, BYTE bModuleType, WORD *pwAlarmData);
    DWORD   __stdcall AxlM3SetStationAlarmClear(long lBoardNo, long lModuleNo, WORD wAlarmClrMod, BYTE bModuleType);
    DWORD   __stdcall AxlM3SetStationSyncSet(long lBoardNo, long lModuleNo, BYTE bModuleType);
    DWORD   __stdcall AxlM3SetStationConnect(long lBoardNo, long lModuleNo, BYTE bVer, BYTE bComMode, BYTE bComTime, BYTE bProfileType, BYTE bModuleType);
    DWORD   __stdcall AxlM3SetStationDisConnect(long lBoardNo, long lModuleNo, BYTE bModuleType);
    DWORD   __stdcall AxlM3GetStationStoredParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    DWORD   __stdcall AxlM3SetStationStoredParameter(long lBoardNo, long lModuleNo, WORD wNo, BYTE bSize, BYTE bModuleType, BYTE *pbParam);
    DWORD   __stdcall AxlM3GetStationMemory(long lBoardNo, long lModuleNo, WORD wSize, DWORD dwAddress, BYTE bModuleType, BYTE bMode, BYTE bDataType, BYTE *pbData);
    DWORD   __stdcall AxlM3SetStationMemory(long lBoardNo, long lModuleNo, WORD wSize, DWORD dwAddress, BYTE bModuleType, BYTE bMode, BYTE bDataType, BYTE *pbData);

    DWORD   __stdcall AxlM3SetStationAccessMode(long lBoardNo, long lModuleNo, BYTE bModuleType, BYTE bRWSMode);
    DWORD   __stdcall AxlM3GetStationAccessMode(long lBoardNo, long lModuleNo, BYTE bModuleType, BYTE *bRWSMode);
    DWORD   __stdcall AxlM3SetAutoSyncConnectMode(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD dwAutoSyncConnectMode);
    DWORD   __stdcall AxlM3GetAutoSyncConnectMode(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *dwpAutoSyncConnectMode);
    DWORD   __stdcall AxlM3SyncConnectSingle(long lBoardNo, long lModuleNo, BYTE bModuleType);
    DWORD   __stdcall AxlM3SyncDisconnectSingle(long lBoardNo, long lModuleNo, BYTE bModuleType);
    DWORD   __stdcall AxlM3IsOnLine(long lBoardNo, long lModuleNo, DWORD *dwData);

    DWORD   __stdcall AxlM3GetStationRWS(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    DWORD   __stdcall AxlM3SetStationRWS(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    DWORD   __stdcall AxlM3GetStationRWA(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);
    DWORD   __stdcall AxlM3SetStationRWA(long lBoardNo, long lModuleNo, BYTE bModuleType, DWORD *pdwParam, BYTE bSize);

    // MLIII setting for adjustment operation
    // dwReqCode == 0x1005 : parameter initialization : 20sec
    // dwReqCode == 0x1008 : absolute encoder reset   : 5sec
    // dwReqCode == 0x100E : automatic offset adjustment of motor current detection signals  : 5sec
    // dwReqCode == 0x1013 : Multiturn limit setting  : 5sec
    DWORD   __stdcall AxmM3AdjustmentOperation(long lAxisNo, DWORD dwReqCode);

    DWORD    __stdcall AxmHomeGetM3FWRealRate(long lAxisNo, DWORD *upHomeMainStepNumber, DWORD *upHomeSubStepNumber, DWORD *upHomeLastMainStepNumber, DWORD *upHomeLastSubStepNumber);
    DWORD   __stdcall AxmHomeGetM3OffsetAvoideSenArea(long lAxisNo, double *dPos);
    DWORD   __stdcall AxmHomeSetM3OffsetAvoideSenArea(long lAxisNo, double dPos);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXDEV_H__
