/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** AXM.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Ajinextek Motion Library Header File
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

#ifndef __AXT_AXM_H__
#define __AXT_AXM_H__

#include "AXHS.h"

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//========== Board and module verification API(Info) - Information =================================================================================
    // Return board number, module position and module ID of relevant axis. 
    DWORD    __stdcall AxmInfoGetAxis(long lAxisNo, long *lpBoardNo, long *lpModulePos, DWORD *upModuleID);
    // Return whether the motion module exists.
    DWORD    __stdcall AxmInfoIsMotionModule(DWORD *upStatus);
    // Return whether relevant axis is valid.
    DWORD    __stdcall AxmInfoIsInvalidAxisNo(long lAxisNo);
    // Return whether relevant axis status. 
    DWORD     __stdcall AxmInfoGetAxisStatus(long lAxisNo);    
    // number of RTEX Products, return number of valid axis installed in system.
    DWORD    __stdcall AxmInfoGetAxisCount(long *lpAxisCount);
    // Return the first axis number of relevant board/module 
    DWORD    __stdcall AxmInfoGetFirstAxisNo(long lBoardNo, long lModulePos, long *lpAxisNo);

    DWORD    __stdcall AxmInfoGetBoardFirstAxisNo(long lBoardNo, long lModulePos, long *lpAxisNo);    
//========= virtual axis function ============================================================================================    
    // Set virtual axis.
    DWORD    __stdcall AxmVirtualSetAxisNoMap(long lRealAxisNo, long lVirtualAxisNo);
    // Return the set virtual channel(axis) number. 
    DWORD    __stdcall AxmVirtualGetAxisNoMap(long lRealAxisNo, long *lpVirtualAxisNo);
    // Set multi-virtual axes. 
    DWORD    __stdcall AxmVirtualSetMultiAxisNoMap(long lSize, long *lpRealAxesNo, long *lpVirtualAxesNo);
    // Return the set multi-virtual channel(axis) number.
    DWORD    __stdcall AxmVirtualGetMultiAxisNoMap(long lSize, long *lpRealAxesNo, long *lpVirtualAxesNo);
    // Reset the virtual axis setting.
    DWORD    __stdcall AxmVirtualResetAxisMap();

//========= API related interrupt ======================================================================================
    // Call-back API method has the advantage which can be advised the event most fast timing as the call-back API is called immediately when the event occurs, but  
    // the main processor shall be congested until the call-back API is completed. 
    // i.e, it shall be carefully used when there is any work loaded in the call-bak API. 
    // Event method monitors if interrupt occurs continuously by using thread, and when interrupt is occurs 
    // it manages, and even though this method has disadvantage which system resource is occupied by thread , 
    // it can detect interrupt most quickly and manage it. 
    // It is not used a lot in general, but used when quick management of interrupt is the most concern. 
    // Event method is operated using specific thread which monitors the occurrence of event separately from main processor , so 
    // it able to use the resources efficiently in multi-processor system and expressly recommendable method. 

    // Window message or call back API is used for getting the interrupt message. 
    // (message handle, message ID, call back API, interrupt event)
    //    hWnd    : use to get window handle and window message. Enter NULL if it is not used.
    //    wMsg    : message of window handle, enter 0 if is not used or default value is used. 
    //    proc    : API pointer to be called when interrupted, enter NULL if not use 
    //    pEvent  : Event handle when event method is used. 
    // Ex)
    // AxmInterruptSetAxis(0, Null, 0, AxtInterruptProc, NULL);
    // void __stdcall AxtInterruptProc(long lActiveNo, DWORD uFlag){
    //     ... ;
    // }
    DWORD    __stdcall AxmInterruptSetAxis(long lAxisNo, HWND hWnd, DWORD uMessage, AXT_INTERRUPT_PROC pProc, HANDLE *pEvent);

    // Set whether to use interrupt of set axis or not. 
    // Set interrupt in the relevant axis/ verification
     // uUse : use or not use => DISABLE(0), ENABLE(1)
    DWORD    __stdcall AxmInterruptSetAxisEnable(long lAxisNo, DWORD uUse);
    // Return whether to use interrupt of set axis or not
    DWORD    __stdcall AxmInterruptGetAxisEnable(long lAxisNo, DWORD *upUse);

    //Read relevant information when interrupt is used in event method
    DWORD    __stdcall AxmInterruptRead(long *lpAxisNo, DWORD *upFlag);
    
    // Return interrupt flag value of relevant axis.
    DWORD    __stdcall AxmInterruptReadAxisFlag(long lAxisNo, long lBank, DWORD *upFlag);

    // Set whether the interrupt set by user to specific axis occurs or not
    // lBank         : Enable to set interrupt bank number(0 - 1).
    // uInterruptNum : Enable to set interrupt number by setting bit number( 0 - 31 ).
    DWORD    __stdcall AxmInterruptSetUserEnable(long lAxisNo, long lBank, DWORD uInterruptNum);

    // Verify whether the interrupt set by user of specific axis occurs or not
    DWORD    __stdcall AxmInterruptGetUserEnable(long lAxisNo, long lBank, DWORD *upInterruptNum);

//======== set motion parameter ===========================================================================================================================================================
    // If file is not loaded by AxmMotLoadParaAll, set default parameter in initial parameter setting. 
    // Apply to all axes which is being used in PC equally. Default parameters are as below. 
    // 00:AXIS_NO.             =0       01:PULSE_OUT_METHOD.    =4      02:ENC_INPUT_METHOD.    =3     03:INPOSITION.          =2
    // 04:ALARM.               =0       05:NEG_END_LIMIT.       =0      06:POS_END_LIMIT.       =0     07:MIN_VELOCITY.        =1
    // 08:MAX_VELOCITY.        =700000  09:HOME_SIGNAL.         =4      10:HOME_LEVEL.          =1     11:HOME_DIR.            =-1
    // 12:ZPHASE_LEVEL.        =1       13:ZPHASE_USE.          =0      14:STOP_SIGNAL_MODE.    =0     15:STOP_SIGNAL_LEVEL.   =0
    // 16:HOME_FIRST_VELOCITY. =10000   17:HOME_SECOND_VELOCITY.=10000  18:HOME_THIRD_VELOCITY. =2000  19:HOME_LAST_VELOCITY.  =100
    // 20:HOME_FIRST_ACCEL.    =40000   21:HOME_SECOND_ACCEL.   =40000  22:HOME_END_CLEAR_TIME. =1000  23:HOME_END_OFFSET.     =0
    // 24:NEG_SOFT_LIMIT.      =0.000   25:POS_SOFT_LIMIT.      =0      26:MOVE_PULSE.          =1     27:MOVE_UNIT.           =1
    // 28:INIT_POSITION.       =1000    29:INIT_VELOCITY.       =200    30:INIT_ACCEL.          =400   31:INIT_DECEL.          =400
    // 32:INIT_ABSRELMODE.     =0       33:INIT_PROFILEMODE.    =4

    // 00=[AXIS_NO             ]: axis (start from 0axis)
    // 01=[PULSE_OUT_METHOD    ]: Pulse out method TwocwccwHigh = 6
    // 02=[ENC_INPUT_METHOD    ]: disable = 0   1 multiplication = 1  2 multiplication = 2  4 multiplication = 3, for replacing the direction of splicing (-).1 multiplication = 11  2 multiplication = 12  4 multiplication = 13
    // 03=[INPOSITION          ], 04=[ALARM     ], 05,06 =[END_LIMIT   ]  : 0 = A contact 1= B contact 2 = not use. 3 = keep current mode
    // 07=[MIN_VELOCITY        ]: START VELOCITY
    // 08=[MAX_VELOCITY        ]: command velocity which driver can accept. Generally normal servo is 700k
    // Ex> screw : 20mm pitch drive: 10000 pulse motor: 400w
    // 09=[HOME_SIGNAL         ]: 4 - Home in0 , 0 :PosEndLimit , 1 : NegEndLimit // refer _HOME_SIGNAL.
    // 10=[HOME_LEVEL          ]: : 0 = A contact 1= B contact 2 = not use. 3 = keep current mode
    // 11=[HOME_DIR            ]: HOME DIRECTION 1:+direction, 0:-direction
    // 12=[ZPHASE_LEVEL        ]: : 0 = A contact 1= B contact 2 = not use. 3 = keep current mode
    // 13=[ZPHASE_USE          ]: use of Z phase. 0: not use , 1: - direction, 2: +direction 
    // 14=[STOP_SIGNAL_MODE    ]: ESTOP, mode in use of SSTOP  0:slowdown stop, 1:emergency stop 
    // 15=[STOP_SIGNAL_LEVEL   ]: ESTOP, SSTOP use level. : 0 = A contact 1= B contact 2 = not use. 3 = keep current mode 
    // 16=[HOME_FIRST_VELOCITY ]: 1st move velocity 
    // 17=[HOME_SECOND_VELOCITY]: velocity after detecting 
    // 18=[HOME_THIRD_VELOCITY ]: the last velocity 
    // 19=[HOME_LAST_VELOCITY  ]: velocity for index detecting and detail detecting 
    // 20=[HOME_FIRST_ACCEL    ]: 1st acceleration, 21=[HOME_SECOND_ACCEL   ] : 2nd acceleration 
    // 22=[HOME_END_CLEAR_TIME ]: queue time to set origin detecting Enc value,  23=[HOME_END_OFFSET] : move as much as offset after detecting of origin.
    // 24=[NEG_SOFT_LIMIT      ]: - not use if set same as SoftWare Limit , 25=[POS_SOFT_LIMIT ]: - not use if set same as SoftWare Limit.
    // 26=[MOVE_PULSE          ]: amount of pulse per driver revolution               , 27=[MOVE_UNIT  ]: travel distance per driver revolution :screw Pitch
    // 28=[INIT_POSITION       ]: initial position when use agent , user can use optionally
    // 29=[INIT_VELOCITY       ]: initial velocity when use agent, user can use optionally
    // 30=[INIT_ACCEL          ]: initial acceleration when use agent, user can use optionally
    // 31=[INIT_DECEL          ]: initial deceleration when use agent, user can use optionally
    // 32=[INIT_ABSRELMODE     ]: absolute(0)/relative(1) set position
    // 33=[INIT_PROFILEMODE    ]: set profile mode in (0 - 4) 
    //                            '0': symmetry Trapezode, '1': asymmetric Trapezode, '2': symmetry Quasi-S Curve, '3':symmetry S Curve, '4':asymmetric S Curve
    
    // load .mot file which is saved as AxmMotSaveParaAll. Optional modification is available by user. 
    DWORD    __stdcall AxmMotLoadParaAll(char *szFilePath);
    // Save all parameter for all current axis by axis. Save as .mot file. Load file by using  AxmMotLoadParaAll. 
    DWORD    __stdcall AxmMotSaveParaAll(char *szFilePath);

    // In parameter 28 - 31, user sets by using this API in the program. 
    DWORD    __stdcall AxmMotSetParaLoad(long lAxisNo, double dInitPos, double dInitVel, double dInitAccel, double dInitDecel);    
    // In parameter 28 - 31, user verifys by using this API in the program.
    DWORD    __stdcall AxmMotGetParaLoad(long lAxisNo, double *dpInitPos, double *dpInitVel, double *dpInitAccel, double *dpInitDecel);    

    // Set the pulse output method of specific axis.
    //uMethod  0 :OneHighLowHigh, 1 :OneHighHighLow, 2 :OneLowLowHigh, 3 :OneLowHighLow, 4 :TwoCcwCwHigh
    //         5 :TwoCcwCwLow,    6 :TwoCwCcwHigh,   7 :TwoCwCcwLow,   8 :TwoPhase,      9 :TwoPhaseReverse
    // OneHighLowHigh          = 0x0,   // 1 pulse method, PULSE(Active High), forward direction(DIR=Low)  / reverse direction(DIR=High)
    // OneHighHighLow          = 0x1,   // 1 pulse method, PULSE(Active High), forward direction (DIR=High) / reverse direction (DIR=Low)
    // OneLowLowHigh           = 0x2,   // 1 pulse method, PULSE(Active Low), forward direction (DIR=Low)  / reverse direction (DIR=High)
    // OneLowHighLow           = 0x3,   // 1 pulse method, PULSE(Active Low), forward direction (DIR=High) / reverse direction (DIR=Low)
    // TwoCcwCwHigh            = 0x4,   // 2 pulse method, PULSE(CCW: reverse direction),  DIR(CW: forward direction),  Active High
    // TwoCcwCwLow             = 0x5,   // 2 pulse method, PULSE(CCW: reverse direction),  DIR(CW: forward direction),  Active Low
    // TwoCwCcwHigh            = 0x6,   // 2 pulse method, PULSE(CW: forward direction),   DIR(CCW: reverse direction), Active High
    // TwoCwCcwLow             = 0x7,   // 2 pulse method, PULSE(CW: forward direction),   DIR(CCW: reverse direction), Active Low
    // TwoPhase                = 0x8,   // 2 phase (90' phase difference),  PULSE lead DIR(CW: forward direction), PULSE lag DIR(CCW: reverse direction)
    // TwoPhaseReverse         = 0x9    // 2 phase(90' phase difference),  PULSE lead DIR(CCW: Forward diredtion), PULSE lag DIR(CW: Reverse direction)
    DWORD    __stdcall AxmMotSetPulseOutMethod(long lAxisNo, DWORD uMethod);
    // Return the setting of pulse output method of specific axis. 
    DWORD    __stdcall AxmMotGetPulseOutMethod(long lAxisNo, DWORD *upMethod);

    // Set the Encoder input method including the setting of increase direction of actual count of specific axis. 
    // ObverseUpDownMode       = 0x0,   // Forward direction Up/Down
    // ObverseSqr1Mode         = 0x1,   // Forward direction 1 multiplication
    // ObverseSqr2Mode         = 0x2,   // Forward direction 2 multiplication
    // ObverseSqr4Mode         = 0x3,   // Forward direction 4 multiplication
    // ReverseUpDownMode       = 0x4,   // Reverse direction Up/Down
    // ReverseSqr1Mode         = 0x5,   // Reverse direction 1 multiplication
    // ReverseSqr2Mode         = 0x6,   // Reverse direction 2 multiplication
    // ReverseSqr4Mode         = 0x7    // Reverse direction 4 multiplication
    DWORD    __stdcall AxmMotSetEncInputMethod(long lAxisNo, DWORD uMethod);
    // Return the Encoder input method including the setting of increase direction of actual count of specific axis.
    DWORD    __stdcall AxmMotGetEncInputMethod(long lAxisNo, DWORD *upMethod);

    // If you want to set specified velocity unit in RPM(Revolution Per Minute),
    // ex>    calculate rpm :
    // 4500 rpm ?
    // When unit/ pulse = 1 : 1, then it becomes pulse per sec, and
    // if you want to set at 4500 rpm , then  4500 / 60 sec : 75 revolution / 1sec
    // The number of pulse per 1 revolution of motor shall be known. This can be know by detecting of Z phase in Encoder. 
    // If 1 revolution:1800 pulse,  75 x 1800 = 135000 pulses are required. 
    // Operate by input Unit = 1, Pulse = 1800 into AxmMotSetMoveUnitPerPulse.
    // Caution : If it is controlled with rpm, velocity and acceleration will be changed to rpm unit as well. 

    // Set the travel distance of specific axis per pulse. 
    DWORD    __stdcall AxmMotSetMoveUnitPerPulse(long lAxisNo, double dUnit, long lPulse);
    // Return the travel distance of specific axis per pulse.
    DWORD    __stdcall AxmMotGetMoveUnitPerPulse(long lAxisNo, double *dpUnit, long *lpPulse);
    
    // Set deceleration starting point detecting method to specific axis. 
    // AutoDetect 0x0 : automatic acceleration/deceleration.
    // RestPulse  0x1 : manual acceleration/deceleration."
    DWORD    __stdcall AxmMotSetDecelMode(long lAxisNo, DWORD uMethod);
    // Return the deceleration starting point detecting method of specific axis.
    DWORD    __stdcall AxmMotGetDecelMode(long lAxisNo, DWORD *upMethod);
    
    // Set remain pulse to the specific axis in manual deceleration mode.
    DWORD    __stdcall AxmMotSetRemainPulse(long lAxisNo, DWORD uData);
    // Return remain pulse of the specific axis in manual deceleration mode.
    DWORD    __stdcall AxmMotGetRemainPulse(long lAxisNo, DWORD *upData);

    // Set maximum velocity to the specific axis in uniform velocity movement API. 
    DWORD    __stdcall AxmMotSetMaxVel(long lAxisNo, double dVel);
    // Return maximum velocity of the specific axis in uniform velocity movement API
    DWORD    __stdcall AxmMotGetMaxVel(long lAxisNo, double *dpVel);

    // Set travel distance calculation mode of specific axis.
    //uAbsRelMode : POS_ABS_MODE '0' - absolute coordinate system
    //              POS_REL_MODE '1' - relative coordinate system
    DWORD    __stdcall AxmMotSetAbsRelMode(long lAxisNo, DWORD uAbsRelMode);
    // Return travel distance calculation mode of specific axis.
    DWORD    __stdcall AxmMotGetAbsRelMode(long lAxisNo, DWORD *upAbsRelMode);

    //Set move velocity profile mode of specific axis. 
    //ProfileMode : SYM_TRAPEZOIDE_MODE  '0' - symmetry Trapezode
    //              ASYM_TRAPEZOIDE_MODE '1' - asymmetric Trapezode
    //              QUASI_S_CURVE_MODE   '2' - symmetry Quasi-S Curve
    //              SYM_S_CURVE_MODE     '3' - symmetry S Curve
    //              ASYM_S_CURVE_MODE    '4' - asymmetric S Curve
    DWORD    __stdcall AxmMotSetProfileMode(long lAxisNo, DWORD uProfileMode);
    // Return move velocity profile mode of specific axis.
    DWORD    __stdcall AxmMotGetProfileMode(long lAxisNo, DWORD *upProfileMode);
    
    //Set acceleration unit of specific axis.
    //AccelUnit : '0' ? use unit/sec2 for the unit of acceleration/deceleration
    //            '1' - use sec for the unit of acceleration/deceleration
    DWORD    __stdcall AxmMotSetAccelUnit(long lAxisNo, DWORD uAccelUnit);
    // Return acceleration unit of specific axis.
    DWORD    __stdcall AxmMotGetAccelUnit(long lAxisNo, DWORD *upAccelUnit);

    // Set initial velocity to the specific axis.
    DWORD    __stdcall AxmMotSetMinVel(long lAxisNo, double dMinVel);
    // Return initial velocity of the specific axis.
    DWORD    __stdcall AxmMotGetMinVel(long lAxisNo, double *dpMinVel);

    // Set acceleration jerk value of specific axis.[%].
    DWORD    __stdcall AxmMotSetAccelJerk(long lAxisNo, double dAccelJerk);
    // Return acceleration jerk value of specific axis.
    DWORD    __stdcall AxmMotGetAccelJerk(long lAxisNo, double *dpAccelJerk);

    // Set deceleration jerk value of specific axis.[%].
    DWORD    __stdcall AxmMotSetDecelJerk(long lAxisNo, double dDecelJerk);
    // Return deceleration jerk value of specific axis.
    DWORD    __stdcall AxmMotGetDecelJerk(long lAxisNo, double *dpDecelJerk);
    
    DWORD    __stdcall AxmMotSetProfilePriority(long lAxisNo, DWORD uPriority);
    DWORD    __stdcall AxmMotGetProfilePriority(long lAxisNo, DWORD *upPriority);

//=========== Setting API related in/output signal ================================================================================

    // Set Z phase level of specific axis.
    // uLevel : LOW(0), HIGH(1)
    DWORD    __stdcall AxmSignalSetZphaseLevel(long lAxisNo, DWORD uLevel);
    // Return Z phase level of specific axis.
    DWORD    __stdcall AxmSignalGetZphaseLevel(long lAxisNo, DWORD *upLevel);

    // Set output level of Servo-On signal of specific axis.
    // uLevel : LOW(0), HIGH(1)
    DWORD    __stdcall AxmSignalSetServoOnLevel(long lAxisNo, DWORD uLevel);
    // Return output level of Servo-On signal of specific axis.
    DWORD    __stdcall AxmSignalGetServoOnLevel(long lAxisNo, DWORD *upLevel);

    // Set output level of Servo-Alarm Reset signal of specific axis.
    // uLevel : LOW(0), HIGH(1)
    DWORD    __stdcall AxmSignalSetServoAlarmResetLevel(long lAxisNo, DWORD uLevel);
    // Return output level of Servo-Alarm Reset signal of specific axis.
    DWORD    __stdcall AxmSignalGetServoAlarmResetLevel(long lAxisNo, DWORD *upLevel);

    //    Set whether to use Inposition signal of specific axis and signal input level.
    // uLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)    
    DWORD    __stdcall AxmSignalSetInpos(long lAxisNo, DWORD uUse);
    // Return whether to use Inposition signal of specific axis and signal input level.
    DWORD    __stdcall AxmSignalGetInpos(long lAxisNo, DWORD *upUse);
    // Return inposition signal input mode of specific axis. 
    DWORD    __stdcall AxmSignalReadInpos(long lAxisNo, DWORD *upStatus);

    //    Set whether to use emergency stop or not against to alarm signal input and set signal input level of specific axis. 
    // uLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)
    DWORD    __stdcall AxmSignalSetServoAlarm(long lAxisNo, DWORD uUse);
    // Return whether to use emergency stop or not against to alarm signal input and set signal input level of specific axis.
    DWORD    __stdcall AxmSignalGetServoAlarm(long lAxisNo, DWORD *upUse);
    // Return input level of alarm signal of specific axis.
    DWORD    __stdcall AxmSignalReadServoAlarm(long lAxisNo, DWORD *upStatus);

    // Set whether to use end limit sensor of specific axis and input level of signal. 
    // Available to set of slow down stop or emergency stop when end limit sensor is input.
    // uStopMode: EMERGENCY_STOP(0), SLOWDOWN_STOP(1)
    // uPositiveLevel, uNegativeLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)
    DWORD    __stdcall AxmSignalSetLimit(long lAxisNo, DWORD uStopMode, DWORD uPositiveLevel, DWORD uNegativeLevel);
    // Return whether to use end limit sensor of specific axis , input level of signal and stop mode for signal input. 
    DWORD    __stdcall AxmSignalGetLimit(long lAxisNo, DWORD *upStopMode, DWORD *upPositiveLevel, DWORD *upNegativeLevel);
    // Return the input state of end limit sensor of specific axis. 
    DWORD    __stdcall AxmSignalReadLimit(long lAxisNo, DWORD *upPositiveStatus, DWORD *upNegativeStatus);

    // Set whether to use software limit, count to use and stop method of specific axis.
    // uUse       : DISABLE(0), ENABLE(1)
    // uStopMode  : EMERGENCY_STOP(0), SLOWDOWN_STOP(1)
    // uSelection : COMMAND(0), ACTUAL(1)
    // Caution: When software limit is set in advance by using above API in origin detecting and is moving, if the detecting of origin is stopped during detecting, it becomes DISABLE.
    DWORD    __stdcall AxmSignalSetSoftLimit(long lAxisNo, DWORD uUse, DWORD uStopMode, DWORD uSelection, double dPositivePos, double dNegativePos);
    // Return whether to use software limit, count to use and stop method of specific axis.
    DWORD    __stdcall AxmSignalGetSoftLimit(long lAxisNo, DWORD *upUse, DWORD *upStopMode, DWORD *upSelection, double *dpPositivePos, double *dpNegativePos);
    DWORD    __stdcall AxmSignalReadSoftLimit(long lAxisNo, DWORD *upPositiveStatus, DWORD *upNegativeStatus);
    
    // Set the stop method of emergency stop(emergency stop/slowdown stop) ,or whether to use or not.
    // uStopMode  : EMERGENCY_STOP(0), SLOWDOWN_STOP(1)
    // uLevel : LOW(0), HIGH(1), UNUSED(2), USED(3)
    DWORD    __stdcall AxmSignalSetStop(long lAxisNo, DWORD uStopMode, DWORD uLevel);
    // Return the stop method of emergency stop(emergency stop/slowdown stop) ,or whether to use or not.
    DWORD    __stdcall AxmSignalGetStop(long lAxisNo, DWORD *upStopMode, DWORD *upLevel);
    // Return input state of emergency stop.
    DWORD    __stdcall AxmSignalReadStop(long lAxisNo, DWORD *upStatus);

    // Output the Servo-On signal of specific axis.
    // uOnOff : FALSE(0), TRUE(1) ( The case of universal 0 output)
    DWORD    __stdcall AxmSignalServoOn(long lAxisNo, DWORD uOnOff);
    // Return the output state of Servo-On signal of specific axis. 
    DWORD    __stdcall AxmSignalIsServoOn(long lAxisNo, DWORD *upOnOff);

    // Output the Servo-Alarm Reset signal of specific axis.
    // uOnOff : FALSE(0), TRUE(1) (The case of universal 1 output)
    DWORD    __stdcall AxmSignalServoAlarmReset(long lAxisNo, DWORD uOnOff);
    
    //    Set universal output value.
    //  uValue : Hex Value 0x00
    DWORD    __stdcall AxmSignalWriteOutput(long lAxisNo, DWORD uValue);
    // Return universal output value.
    DWORD    __stdcall AxmSignalReadOutput(long lAxisNo, DWORD *upValue);

    // Only ML3 Master
    // Return Brake sensor signal value
    DWORD    __stdcall AxmSignalReadBrakeOn(long lAxisNo, DWORD *upOnOff);

    // lBitNo : Bit Number(0 - 4)
    // uOnOff : FALSE(0), TRUE(1)
    // Set universal output values by bit.
    DWORD    __stdcall AxmSignalWriteOutputBit(long lAxisNo, long lBitNo, DWORD uOnOff);
    // Return universal output values by bit.
    DWORD    __stdcall AxmSignalReadOutputBit(long lAxisNo, long lBitNo, DWORD *upOnOff);

    // Return universal input value in Hex value.
    DWORD    __stdcall AxmSignalReadInput(long lAxisNo, DWORD *upValue);
    
    // lBitNo : Bit Number(0 - 4)
    // Return universal input value by bit. 
    DWORD    __stdcall AxmSignalReadInputBit(long lAxisNo, long lBitNo, DWORD *upOn);

	// Set universal In/Out digital filter coefficient.
	// uSignal: END_LIMIT(0), INP_ALARM(1), UIN_00_01(2), UIN_02_04(3)
	// dBandwidthUsec: 0.2uSec~26666usec
	DWORD    __stdcall AxmSignalSetFilterBandwidth(long lAxisNo, DWORD uSignal, double dBandwidthUsec);

//========== API which verifies the state during motion moving and after moving. ============================================================

    // Return pulse output state of specific axis. 
    // (Status of move)"
    DWORD    __stdcall AxmStatusReadInMotion(long lAxisNo, DWORD *upStatus);

    //  Return move pulse counter value of specific axis after start of move. 
    //  (pulse count value)"
    DWORD    __stdcall AxmStatusReadDrivePulseCount(long lAxisNo, long *lpPulse);
    
    // Return DriveStatus register (status of in-motion) of specific Axis. 
    // Caution: All Motion Product is different Hardware bit signal. Let's refer Manual and AXHS.xxx
    DWORD    __stdcall AxmStatusReadMotion(long lAxisNo, DWORD *upStatus);
    
    // Return EndStatus(status of stop) register of specific axis.
    // Caution: All Motion Product is different Hardware bit signal. Let's refer Manual and AXHS.xxx
    DWORD    __stdcall AxmStatusReadStop(long lAxisNo, DWORD *upStatus);
    
    // Return Mechanical Signal Data(Current mechanical signal status)of specific axis.
    // Caution: All Motion Product is different Hardware bit signal. Let's refer Manual and AXHS.xxx
    DWORD    __stdcall AxmStatusReadMechanical(long lAxisNo, DWORD *upStatus);
    
    // Read current move velocity oo specific axis.
    DWORD    __stdcall AxmStatusReadVel(long lAxisNo, double *dpVel);
    
    // Return the error between Command Pos and Actual Pos of specific axis.
    DWORD    __stdcall AxmStatusReadPosError(long lAxisNo, double *dpError);
    
    // Verify the travel(traveled) distance to the final drive.
    DWORD    __stdcall AxmStatusReadDriveDistance(long lAxisNo, double *dpUnit);

	// Set use the Position information Type of specific Axis. 
	// uPosType  : Select Position Information Type (Actual position / Command position)
	//    POSITION_LIMIT '0' - Normal action, In all round action.
	//    POSITION_BOUND '1' - Position cycle type, dNegativePos ~ dPositivePos Range
	// Caution(PCI-Nx04)
	// - BOUNT설정시 카운트 값이 Max값을 초과 할 때 Min값이되며 반대로 Min값을 초과 할 때 Max값이 된다.
	// - 다시말해 현재 위치값이 설정한 값 밖에서 카운트 될 때는 위의 Min, Max값이 적용되지 않는다.
	DWORD    __stdcall AxmStatusSetPosType(long lAxisNo, DWORD uPosType, double dPositivePos, double dNegativePos);
	// Return the Position Information Type of of specific axis.
	DWORD    __stdcall AxmStatusGetPosType(long lAxisNo, DWORD *upPosType, double *dpPositivePos, double *dpNegativePos);
	// Set absolute encoder origin offset Position of specific axis. [Only for PCI-R1604-MLII]
	DWORD    __stdcall AxmStatusSetAbsOrgOffset(long lAxisNo, double dOrgOffsetPos);

    // Set the actual position of specific axis. 
    DWORD    __stdcall AxmStatusSetActPos(long lAxisNo, double dPos);
    // Return the actual position of specific axis.
    DWORD    __stdcall AxmStatusGetActPos(long lAxisNo, double *dpPos);

    // Set command position of specific axis.
    DWORD    __stdcall AxmStatusSetCmdPos(long lAxisNo, double dPos);
    // Return command position of specific axis.
    DWORD    __stdcall AxmStatusGetCmdPos(long lAxisNo, double *dpPos);

    // Set command position and actual position of specific axis
    // Only RTEX use
    DWORD    __stdcall AxmStatusSetPosMatch(long lAxisNo, double dPos);

    DWORD    __stdcall AxmStatusReadMotionInfo(long lAxisNo, PMOTION_INFO pMI);
    
    // Network
    DWORD    __stdcall AxmStatusRequestServoAlarm(long lAxisNo);    
    // upAlarmCode      :
    // uReturnMode      :
    // [0-Immediate]    :
    // [1-Blocking]     :
    // [2-Non Blocking] :
    DWORD    __stdcall AxmStatusReadServoAlarm(long lAxisNo, DWORD uReturnMode, DWORD *upAlarmCode);
    DWORD    __stdcall AxmStatusGetServoAlarmString(long lAxisNo, DWORD uAlarmCode, long lAlarmStringSize, char *szAlarmString);
    DWORD    __stdcall AxmStatusRequestServoAlarmHistory(long lAxisNo);
    // lpCount          :
    // upAlarmCode      :
    // uReturnMode      :
    // [0-Immediate]    :
    // [1-Blocking]     :
    // [2-Non Blocking] :
    DWORD    __stdcall AxmStatusReadServoAlarmHistory(long lAxisNo, DWORD uReturnMode, long *lpCount, DWORD *upAlarmCode);
    DWORD    __stdcall AxmStatusClearServoAlarmHistory(long lAxisNo);

//======== API related home. ======================================================================================================

    // Set home sensor level of specific axis. 
    // uLevel : LOW(0), HIGH(1)
    DWORD    __stdcall AxmHomeSetSignalLevel(long lAxisNo, DWORD uLevel);
    // Return home sensor level of specific axis.
    DWORD    __stdcall AxmHomeGetSignalLevel(long lAxisNo, DWORD *upLevel);
    // Verify current home signal input status. Hoe signal can be set by user optionally by using AxmHomeSetMethod API. 
    // upStatus : OFF(0), ON(1)
    DWORD    __stdcall AxmHomeReadSignal(long lAxisNo, DWORD *upStatus);
    
    // Parameters related to origin detecting must be set in order to detect origin of relevant axis. 
    // If the initialization is done properly by using MotionPara setting file, no separate setting is required.  
    // In the setting of origin detecting method, direction of detecting proceed, signal to be used for origin, active level of origin sensor and detecting/no detecting of encoder Z phase are set. 
    // Caution : When the level is set wrong, it may operate + direction even though ? direction is set, and may cause problem in finding home.
    // (Refer the guide part of AxmMotSaveParaAll for detail information. )
    // Use AxmSignalSetHomeLevel for home level.
    // HClrTim : HomeClear Time : Queue time for setting origin detecting Encoder value. 
    // HmDir(Home direction): DIR_CCW(0): - direction    , DIR_CW(1) = + direction   // HOffset ? traveled distance after detecting of origin. 
    // uZphas: Set whether to detect of encoder Z phase after completion of the 1st detecting of origin. 
    // HmSig : PosEndLimit(0) -> +Limit
    //         NegEndLimit(1) -> -Limit
    //         HomeSensor (4) -> origin sensor(universal input 0)    
    DWORD   __stdcall AxmHomeSetMethod(long lAxisNo, long lHmDir, DWORD uHomeSignal, DWORD uZphas, double dHomeClrTime, double dHomeOffset);
    // Return set parameters related to home.
    DWORD   __stdcall AxmHomeGetMethod(long lAxisNo, long *lpHmDir, DWORD *upHomeSignal, DWORD *upZphas, double *dpHomeClrTime, double *dpHomeOffset);

	// Set Home Search Fine tunning Method (Don't care Setting by default).
	// dHomeDogDistance[500 pulse]: Set Dog Length. (Unit = AxmMotSetMoveUnitPerPulse function Setting Unit)
	// lLevelScanTime[100msec]: Set level status confirmation scan time. (msec[1~1000])
	// dwFineSearchUse[USE]: Select find search method. (Default use 5Step, Select 0 use 3Step)
	// dwHomeClrUse[USE]: Set After Home Search Automatically Command/Actual Position Set 0 value of  specific axis.
	DWORD   __stdcall AxmHomeSetFineAdjust(long lAxisNo, double dHomeDogLength, long lLevelScanTime, DWORD uFineSearchUse, DWORD uHomeClrUse);
	// Return set Home  Search fine tunning Method parameters related to home.
	DWORD   __stdcall AxmHomeGetFineAdjust(long lAxisNo, double *dpHomeDogLength, long *lpLevelScanTime, DWORD *upFineSearchUse, DWORD *upHomeClrUse);

    // Detect through several steps in order to detect origin quickly and precisely. Now, set velocities to be used for each step.  
    // The time of origin detecting and the accuracy of origin detecting are decided by setting of these velocities. 
    // Set velocity of origin detecting of each axis by changing velocities of each step.  
    // (Refer the guide part of AxmMotSaveParaAll for detail information.)
    // API which sets velocity to be used in origin detecting. 
    // [dVelFirst]- 1st move velocity   [dVelSecond]- velocity after detecting   [dVelThird]- the last velocity  [dvelLast]- index detecting and in order to detect precisely. 
    // [dAccFirst]- 1st move acceleration [dAccSecond]-acceleration after detecting.  
    DWORD    __stdcall AxmHomeSetVel(long lAxisNo, double dVelFirst, double dVelSecond, double dVelThird, double dVelLast, double dAccFirst, double dAccSecond);
    // Return set velocity to be used in origin detecting. 
    DWORD    __stdcall AxmHomeGetVel(long lAxisNo, double *dpVelFirst, double *dpVelSecond, double *dpVelThird, double *dpVelLast, double *dpAccFirst, double *dpAccSecond);

    // Start to detect origin.
    // When origin detecting start API is executed, thread which will detect origin of relevant axis in the library is created automatically and it is automatically closed after carrying out of the origin detecting in sequence. 
    DWORD    __stdcall AxmHomeSetStart(long lAxisNo);
    // User sets the result of origin detecting optionally. 
    // When the detecting of origin is completed successfully by using origin detecting API, the result of detecting is set as HOME_SUCCESS.
    // This API enables user to set result optionally without execution of origin detecting. 
    // uHomeResult Setup
    // HOME_SUCCESS               = 0x01,       
    // HOME_SEARCHING             = 0x02,     
    // HOME_ERR_GNT_RANGE         = 0x10, // Gantry Home Range Over
    // HOME_ERR_USER_BREAK        = 0x11, // User Stop Command
    // HOME_ERR_VELOCITY          = 0x12, // Velocity is very slow and fast
    // HOME_ERR_AMP_FAULT         = 0x13, // Servo Drive Alarm 
    // HOME_ERR_NEG_LIMIT         = 0x14, // (+)Limit sensor check (-)dir during Motion
    // HOME_ERR_POS_LIMIT         = 0x15, // (-)Limit sensor check (+)dir during Motion
    // HOME_ERR_NOT_DETECT        = 0x16, // not detect User set signal
    // HOME_ERR_UNKNOWN           = 0xFF,    
    DWORD    __stdcall AxmHomeSetResult(long lAxisNo, DWORD uHomeResult);
    // Return the result of origin detecting. 
    // Verify detecting result of origin detection API. When detecting of origin is started, it sets as HOME_SEARCHING, and if the detecting of origin is failed the reason of failure is set. Redo origin detecting after eliminating of failure reasons.
    DWORD    __stdcall AxmHomeGetResult(long lAxisNo, DWORD *upHomeResult);

    // Return progress rate of origin detection.
    // Progress rate can be verified when origin detection is commenced. When origin detection is completed, return 100% whether success or failure. The success or failure of origin detection result can be verified by using GetHome Result API.
    // upHomeMainStepNumber : Progress rate of Main Step . 
    // In case of gentry FALSE upHomeMainStepNumber : When 0 , only selected axis is in proceeding, home progress rate is indicated upHomeStepNumber.
    // In case of gentry TRUE upHomeMainStepNumber : When 0, master home is in proceeding, master home progress rate is indicated upHomeStepNumber. 
    // In case of gentry TRUE upHomeMainStepNumber : When 10 , slave home is in proceeding, master home progress rate is indicated upHomeStepNumber .
    // upHomeStepNumber     : Indicate progress rate against to selected axis. 
    // In case of gentry FALSE  : Indicate progress rate against to selected axis only.
    // In case of gentry TRUE, progress rate is indicated by sequence of master axis and slave axis.
    DWORD    __stdcall AxmHomeGetRate(long lAxisNo, DWORD *upHomeMainStepNumber, DWORD *upHomeStepNumber);

//========= Position move API ===============================================================================================================
    
    // If you want to set specified velocity unit in RPM(Revolution Per Minute),
    // ex>    calculate rpm :
    // 4500 rpm ?
    // When unit/ pulse = 1 : 1, then it becomes pulse per sec, and
    // if you want to set at 4500 rpm , then  4500 / 60 sec : 75 revolution / 1sec
    // The number of pulse per 1 revolution of motor shall be known. This can be know by detecting of Z phase in Encoder. 
    // If 1 revolution:1800 pulse,  75 x 1800 = 135000 pulses are required. 
    // Operate by input Unit = 1, Pulse = 1800 into AxmMotSetMoveUnitPerPulse.
    
    // Travel up to set distance or position.
    // It moves by set velocity and acceleration up to the position set by absolute coordinates/ relative coordinates of specific axis. 
    // Velocity profile is set in AxmMotSetProfileMode API. 
    // It separates from API at the timing of pulse output start.
    DWORD    __stdcall AxmMoveStartPos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel);

    // Travel up to set distance or position.
    // It moves by set velocity and acceleration up to the position set by absolute coordinates/ relative coordinates of specific axis.
    // Velocity profile is set in AxmMotSetProfileMode API
    // It separates from API at the timing of pulse output finish.
    DWORD    __stdcall AxmMovePos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel);

    // Move by set velocity.
    // It maintain velocity mode move by velocity and acceleration  set against to specific axis. 
    // It separates from API at the timing of pulse output start.
    // It moves toward to CW direction when Vel value is positive, CCW when negative.
    DWORD    __stdcall AxmMoveVel(long lAxisNo, double dVel, double dAccel, double dDecel);

    // It maintain velocity mode move by velocity and acceleration  set against to specific multi-axis.
    // It separates from API at the timing of pulse output start.
    // It moves toward to CW direction when Vel value is positive, CCW when negative.
    DWORD    __stdcall AxmMoveStartMultiVel(long lArraySize, long *lpAxesNo, double *dpVel, double *dpAccel, double *dpDecel);

	DWORD    __stdcall AxmMoveStartMultiVelEx(long lArraySize, long *lpAxesNo, double *dpVel, double *dpAccel, double *dpDecel, DWORD dwSyncMode);

    DWORD    __stdcall AxmMoveStartLineVel(long lArraySize, long *lpAxesNo, double *dpDis, double dVel, double dAccel, double dDecel);
    
    // API which detects Edge of specific Input signal and makes emergency stop or slowdown stop. 
    // lDetect Signal : Select input signal to detect . 
    // lDetectSignal  : PosEndLimit(0), NegEndLimit(1), HomeSensor(4), EncodZPhase(5), UniInput02(6), UniInput03(7)
    // Signal Edge   : Select edge direction of selected input signal (rising or falling edge).
    //                  SIGNAL_DOWN_EDGE(0), SIGNAL_UP_EDGE(1)
    // Move direction      : CW when Vel value is positive, CCW when negative.
    // SignalMethod  : EMERGENCY_STOP(0), SLOWDOWN_STOP(1)
    // Caution: When SignalMethod is used as EMERGENCY_STOP(0), acceleration/deceleration is ignored and it is accelerated to specific velocity and emergency stop. 
    //          lDetectSignal is PosEndLimit , in case of searching NegEndLimit(0,1) active status of signal level is detected.
    DWORD    __stdcall AxmMoveSignalSearch(long lAxisNo, double dVel, double dAccel, long lDetectSignal, long lSignalEdge, long lSignalMethod);
    
    // API which detects signal set in specific axis and travels in order to save the position. 
    // In case of searching acting API to select and find desired signal, save the position and read the value using AxmGetCapturePos. 
    // Signal Edge   : Select edge direction of selected input signal. (rising or falling edge).
    //                 SIGNAL_DOWN_EDGE(0), SIGNAL_UP_EDGE(1)
    // Move direction      : CW when Vel value is positive, CCW when negative.
    // SignalMethod  : EMERGENCY_STOP(0), SLOWDOWN_STOP(1)
    // lDetect Signal: Select input signal to detect edge .SIGNAL_DOWN_EDGE(0), SIGNAL_UP_EDGE(1)
    // lDetectSignal : PosEndLimit(0), NegEndLimit(1), HomeSensor(4), EncodZPhase(5), UniInput02(6), UniInput03(7)
    // lTarget       : COMMAND(0), ACTUAL(1)
    // Caution: When SignalMethod is used as EMERGENCY_STOP(0), acceleration/deceleration is ignored and it is accelerated to specific velocity and emergency stop. 
    // lDetectSignal is PosEndLimit , in case of searching NegEndLimit(0,1) active status of signal level is detected.
    DWORD    __stdcall AxmMoveSignalCapture(long lAxisNo, double dVel, double dAccel, long lDetectSignal, long lSignalEdge, long lTarget, long lSignalMethod);
    
    // API which verifies saved position value in 'AxmMoveSignalCapture' API.
    DWORD    __stdcall AxmMoveGetCapturePos(long lAxisNo, double *dpCapPotition);

    // API which travels up to set distance or position.
    // When execute API, it starts relevant motion action and escapes from API without waiting until motion is completed ”
    DWORD    __stdcall AxmMoveStartMultiPos(long lArraySize, long *lpAxisNo, double *dpPos, double *dpVel, double *dpAccel, double *dpDecel);
    
    // Travels up to the distance which sets multi-axis or position. 
    // It moves by set velocity and acceleration up to the position set by absolute coordinates of specific axis. specific axes.
    DWORD    __stdcall AxmMoveMultiPos(long lArraySize, long *lpAxisNo, double *dpPos, double *dpVel, double *dpAccel, double *dpDecel);

    // When execute API, it starts open-loop torque motion action of specific axis.(only for MLII, Sigma 5 servo drivers)
    // dTroque        : Percentage value(%) of maximum torque. (negative value : CCW, positive value : CW)
    // dVel           : Percentage value(%) of maximum velocity.
    // dwAccFilterSel : LINEAR_ACCDCEL(0), EXPO_ACCELDCEL(1), SCURVE_ACCELDECEL(2)
    // dwGainSel      : GAIN_1ST(0), GAIN_2ND(1)
    // dwSpdLoopSel   : PI_LOOP(0), P_LOOP(1)
    DWORD    __stdcall AxmMoveStartTorque(long lAxisNo, double dTorque, double dVel, DWORD dwAccFilterSel, DWORD dwGainSel, DWORD dwSpdLoopSel);

    // It stops motion during torque motion action.
    // it can be only applied for AxmMoveStartTorque API.
    DWORD    __stdcall AxmMoveTorqueStop(long lAxisNo, DWORD dwMethod);

	// To Move Set Position or distance
	// Absolute coordinates / position set to the coordinates relative to the set speed / acceleration rate to drive of specific Axis.
	// Velocity Profile is fixed Asymmetric trapezoid.
	// Accel/Decel Setting Unit is fixed Unit/Sec^2 
	// dAccel != 0.0 and dDecel == 0.0 일 경우 이전 속도에서 감속 없이 지정 속도까지 가속.
	// dAccel != 0.0 and dDecel != 0.0 일 경우 이전 속도에서 지정 속도까지 가속후 등속 이후 감속.
	// dAccel == 0.0 and dDecel != 0.0 일 경우 이전 속도에서 다음 속도까지 감속.

	// The following conditions must be satisfied.
	// dVel[1] == dVel[3] must be satisfied.
	// dVel [2] that can occur as a constant speed drive range is greater dPosition should be enough.
	// Ex) dPosition = 10000;
	// dVel[0] = 300., dAccel[0] = 200., dDecel[0] = 0.;    <== Acceleration
	// dVel[1] = 500., dAccel[1] = 100., dDecel[1] = 0.;    <== Acceleration
	// dVel[2] = 700., dAccel[2] = 200., dDecel[2] = 250.;  <== Acceleration, constant velocity, Deceleration
	// dVel[3] = 500., dAccel[3] = 0.,   dDecel[3] = 150.;  <== Deceleration
	// dVel[4] = 200., dAccel[4] = 0.,   dDecel[4] = 350.;  <== Deceleration
	// Exits API at the point that pulse out starts.
	DWORD    __stdcall AxmMoveStartPosWithList(long lAxisNo, double dPosition, double *dpVel, double *dpAccel, double *dpDecel, long lListNum);

	// Is set by the distance to the target axis position or the position to increase or decrease the movement begins.
	// lEvnetAxisNo    : Start condition occurs axis.
	// dComparePosition: Conditions Occurrence Area of Start condition occurs axis.
	// uPositionSource : Set Conditions Occurrence Area of Start condition occurs axis => COMMAND(0), ACTUAL(1)
	// Cancellations after reservation AxmMoveStop, AxmMoveEStop, AxmMoveSStop use
	// Motion Axis and Start condition occurs axis must be In same group(case by 2V04 In same module).
	DWORD    __stdcall AxmMoveStartPosWithPosEvent(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lEventAxisNo, double dComparePosition, DWORD uPositionSource);

    // It slowdown stops by deceleration set for specific axis.
    // dDecel : Deceleration value when stop. 
    DWORD    __stdcall AxmMoveStop(long lAxisNo, double dDecel);
    DWORD    __stdcall AxmMoveStopEx(long lAxisNo, double dDecel);
    // Stops specific axis emergently .
    DWORD    __stdcall AxmMoveEStop(long lAxisNo);
    // Stops specific axis slow down. 
    DWORD    __stdcall AxmMoveSStop(long lAxisNo);

//========= Overdrive API ============================================================================
    // Overdrives position.
    // Adjust number of specific pulse before the move of specific axis is finished. 
    // Cautions : In here when put in the position subjected to overdrive, as the travel distance is put into the position value of relative form,
    //            position must be put in as position value of relative form.
    DWORD    __stdcall AxmOverridePos(long lAxisNo, double dOverridePos);
    
    // Set the maximum velocity subjected to overdirve before velocity overdriving of specific axis. 
       // Caution : If the velocity overdrive is done 5 times, the max velocity shall be set among them.  
    DWORD    __stdcall AxmOverrideSetMaxVel(long lAxisNo, double dOverrideMaxVel);
    
    // Overdrive velocity.
    // Set velocity variably during the move of specific axis. (Make sure to set variably during the motion.)
    // Caution: Before use of AxmOverrideVel API, set the maximum velocity which can be set by AxmOverrideMaxVel
    // EX> If velocity overdrive two times, 
    // 1. Set the highest velocity among the two as the highest setting velocity of AxmOverrideMaxVel.
    // 2. Set the AxmOverrideVel  variably with the velocity in the moving of AxmMoveStartPos execution specific axis (including move API all) as the first velocity. 
     // 3. Set the AxmOverrideVel  variably with the velocity in the moving of specific axis (including move API all) as the 2nd velocity.
    DWORD    __stdcall AxmOverrideVel(long lAxisNo, double dOverrideVel);
    
    // Overdrive velocity.
    // Set velocity variably during the move of specific axis. (Make sure to set variably during the motion.)
    // Caution: Before use of AxmOverrideVel API, set the maximum velocity which can be set by AxmOverrideMaxVel
    // EX> If velocity overdrive two times, 
    // 1. Set the highest velocity among the two as the highest setting velocity of AxmOverrideMaxVel.
    // 2. Set the AxmOverrideAccelVelDecel  variably with the velocity in the moving of AxmMoveStartPos execution specific axis (including move API all) as the first velocity. 
     // 3. Set the AxmOverrideAccelVelDecel  variably with the velocity in the moving of specific axis (including move API all) as the 2nd velocity.
    DWORD    __stdcall AxmOverrideAccelVelDecel(long lAxisNo, double dOverrideVelocity, double dMaxAccel, double dMaxDecel);
    
    // Velocity overdrive at certain timing. 
    // API which becomes overdrive at the position when a certain position point and velocity to be overdrived
    // lTarget : COMMAND(0), ACTUAL(1)
    DWORD    __stdcall AxmOverrideVelAtPos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel,double dOverridePos, double dOverrideVel, long lTarget);
    DWORD    __stdcall AxmOverrideVelAtMultiPos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel, long lArraySize, double* dpOverridePos, double* dpOverrideVel, long lTarget, DWORD dwOverrideMode);
//========= Move API by master, slave gear ration. ===========================================================================

    // In Electric Gear mode, set gear ratio between master axis and slave axis. 
    // dSlaveRatio : Gear ratio of slave against to master axis ( 0 : 0% , 0.5 : 50%, 1 : 100%)
    DWORD    __stdcall AxmLinkSetMode(long lMasterAxisNo, long lSlaveAxisNo, double dSlaveRatio);
    // In Electric Gear mode, return gear ratio between master axis and slave axis.
    DWORD    __stdcall AxmLinkGetMode(long lMasterAxisNo, long *lpSlaveAxisNo, double *dpGearRatio);
    // Reset electric gear ration between Master axis and slave axis. 
    DWORD    __stdcall AxmLinkResetMode(long lMasterAxisNo);

//======== API related to gentry ===========================================================================================================================================================
    // Motion module supports gentry move system control which two axes are linked mechanically.. 
    // When set master axis for gentry control by using this API, relevant slave axis synchronizes with master axis and moves.  
    // However after setting of gentry, even if any move command or stop command is directed, they are ignored
    // uSlHomeUse     : Whether to use home of slave axis or not ( 0 - 2)
    //             (0 : Search home of master axis without using home of slave axis.)
    //             (1 : Search home of master axis, slave axis. Compensating by applying slave dSlOffset value.)
    //             (2 : Search home of master axis, slave axis. No compensating by applying slave dSlOffset value.))
    // dSlOffset      : Offset value of slave axis
    // dSlOffsetRange : Set offset value range of slave axis.
    // Caution in use : When gentry is enabled, it is normal operation if the slave axis is verified as Inmotion by AxmStatusReadMotion API in its motion, and verified as True.  
    //                When slave axis is verified by AxmStatusReadMotion, if InMotion is False then Gantry Enable is not available, therefore verify whether alarm hits limit. 
    DWORD    __stdcall AxmGantrySetEnable(long lMasterAxisNo, long lSlaveAxisNo, DWORD uSlHomeUse, double dSlOffset, double dSlOffsetRange);

    // The method to know offset value of Slave axis.
    // A. Servo-On both master and slave.         
    // B. After set uSlHomeUse = 2 in AxmGantrySetEnableAPI, then search home by using  AxmHomeSetStart API. 
    // C. After search home, twisted offset value of master axis and slave axis can be seen by reading command value of master axis.
    // D. Read Offsetvalue, and put it into dSlOffset argument of AxmGantrySetEnable API.  
    // E. As dSlOffset value is the slave axis value against to master axis, put its value with reversed sign as ?dSlOffset. 
    // F. dSIOffsetRange means the range of Slave Offset, it is used to originate error when it is out of the specified range.    
    // G. If the offset has been input into AxmGantrySetEnable API, in AxmGantrySetEnable API,after  set uSlHomeUse = 1 then search home by using AxmHomeSetStart API.          
    
    // In gentry move, return parameter set by user. 
    DWORD    __stdcall AxmGantryGetEnable(long lMasterAxisNo, DWORD *upSlHomeUse, double *dpSlOffset, double *dpSlORange, DWORD *upGatryOn);
    // Motion module releases gentry move system control which two axes are linked mechanically.
    DWORD    __stdcall AxmGantrySetDisable(long lMasterAxisNo, long lSlaveAxisNo);

	// Only for PCI-Rxx04-MLII.
	// Set Synchronous compensation of Gentry System.
	// lMasterGain, lSlaveGain : Set % value of Between the two axes about position deviation.
	// lMasterGain, lSlaveGain : If Set Input 0 Not Use. Default value : 0%
	DWORD    __stdcall AxmGantrySetCompensationGain(long lMasterAxisNo, long lMasterGain, long lSlaveGain);
	// Return Synchronous compensation of Gentry System.
	DWORD    __stdcall AxmGantryGetCompensationGain(long lMasterAxisNo, long *lpMasterGain, long *lpSlaveGain);

//==== Regular interpolation API ============================================================================================================================================;
    // Do linear interpolate.
    // API which moves multi-axis linear interpolation by specifying start point and ending point. It escapes from API after commencing of move.
    // When it is used with AxmContiBeginNode and AxmContiEndNode, it becomes Save API in the queue which moves linear interpolation by specifying start point and ending point in the specified  coordinates system. 
    // For the continuous interpolation move of linear profile, save it in internal queue and start by using AxmContiStart API.
    DWORD    __stdcall AxmLineMove(long lCoord, double *dpEndPos, double dVel, double dAccel, double dDecel);

    // Do linear interpolate. (Software)
    // API which moves multi-axis linear interpolation by specifying start point and ending point. It escapes from API after commencing of move.
    // When it is used with AxmContiBeginNode and AxmContiEndNode, it becomes Save API in the queue which moves linear interpolation by specifying start point and ending point in the specified  coordinates system. 
    // For the continuous interpolation move of linear profile, save it in internal queue and start by using AxmContiStart API.
    DWORD    __stdcall AxmLineMoveEx2(long lCoord, double *dpEndPos, double dVel, double dAccel, double dDecel);

    // Interpolate 2 axis arc
    // API which moves arc interpolation by specifying start point, ending point and center point. It escapes from API after commencing of move.
    // When it is used with AxmContiBeginNode and AxmContiEndNode, it becomes Save API in the arc interpolation queue which moves by specifying start point, ending point and center point in the specified  coordinates system. 
    // For the profile arc continuous interpolation move, save it internal queue and start by using AxmContiStart API.
    // lAxisNo = 2 axis array , dCenterPos = center point X,Y array , dEndPos = ending point X,Y array.
  // uCWDir   DIR_CCW(0): Counterclockwise direction,   DIR_CW(1) Clockwise direction
    DWORD   __stdcall AxmCircleCenterMove(long lCoord, long *lAxisNo, double *dCenterPos, double *dEndPos, double dVel, double dAccel, double dDecel, DWORD uCWDir);

    // API which arc interpolation moves by specifying middle point and ending point. It escapes from API after commencing of move.
    // When it is used with AxmContiBeginNode and AxmContiEndNode, it becomes Save API in the arc interpolation queue which moves by specifying middle point and ending point in the specified  coordinates system. 
    // For the profile arc continuous interpolation move, save it internal queue and start by using AxmContiStart API.
    // lAxisNo = 2 axis array , dMidPos = middle point, X,Y array , dEndPos = ending point X,Y array, lArcCircle = arc(0), circle(1)
    DWORD    __stdcall AxmCirclePointMove(long lCoord, long *lAxisNo, double *dMidPos, double *dEndPos, double dVel, double dAccel, double dDecel, long lArcCircle);
    
    // API which arc interpolation moves by specifying start point, ending point and radius. It escapes from API after commencing of move.
    // When it is used with AxmContiBeginNode and AxmContiEndNode, it becomes Save API in the arc interpolation queue which moves by specifying start point, ending point and radius in the specified  coordinates system. 
    // For the profile arc continuous interpolation move, save it internal queue and start by using AxmContiStart API.
    // lAxisNo = 2 axis array , dRadius = radius, dEndPos = ending point X,Y array , uShortDistance = small circle(0), large circle(1)
    // uCWDir   DIR_CCW(0): Counterclockwise direction,   DIR_CW(1) Clockwise direction
    DWORD   __stdcall AxmCircleRadiusMove(long lCoord, long *lAxisNo, double dRadius, double *dEndPos, double dVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance);

    // API which arc interpolation moves by specifying start point, revolution angle and radius. It escapes from API after commencing of move.
    // When it is used with AxmContiBeginNode and AxmContiEndNode, it becomes Save API in the arc interpolation queue which moves by specifying start point, revolution angle and radius in the specified  coordinates system. 
    // For the profile arc continuous interpolation move, save it internal queue and start by using AxmContiStart API.
    // lAxisNo = 2 axis array , dCenterPos = center point X,Y array , dAngle = angle.
    // uCWDir   DIR_CCW(0): Counterclockwise direction,   DIR_CW(1) Clockwise direction
    DWORD   __stdcall AxmCircleAngleMove(long lCoord, long *lAxisNo, double *dCenterPos, double dAngle, double dVel, double dAccel, double dDecel, DWORD uCWDir);

//==== continuous interpolation API ============================================================================================================================================;
    //Set continuous interpolation axis mapping in specific coordinates system.
    //(Start axis mapping number from 0)
    // Caution: In the axis mapping, the input must be started from smaller number. 
    //         In here, the axis of smallest number becomes master. 
    DWORD    __stdcall AxmContiSetAxisMap(long lCoord, long lSize, long *lpAxesNo);
    // Return continuous interpolation axis mapping in specific coordinates system.
    DWORD    __stdcall AxmContiGetAxisMap(long lCoord, long *lpSize, long *lpAxesNo);

    // Set continuous interpolation axis absolute/relative mode in specific coordinates system.
    // (Caution : available to use only after axis mapping)
    // St travel distance calculation mode of specific axis.
    //uAbsRelMode : POS_ABS_MODE '0' - absolute coordinate system
    //              POS_REL_MODE '1' - relative coordinate system
    DWORD    __stdcall AxmContiSetAbsRelMode(long lCoord, DWORD uAbsRelMode);
    // Return interpolation axis absolute/relative mode in specific coordinates system.
    DWORD    __stdcall AxmContiGetAbsRelMode(long lCoord, DWORD *upAbsRelMode);

    // API which verifies whether internal Queue for the interpolation move is empty in specific coordinate system.
    DWORD    __stdcall AxmContiReadFree(long lCoord, DWORD *upQueueFree);
    // API which verifies the number of interpolation move saved in internal Queue for the interpolation move in specific coordinate system 
    DWORD    __stdcall AxmContiReadIndex(long lCoord, long *lpQueueIndex);
    // API which deletes all internal Queue saved for the continuous interpolation move in specific coordinate system 
    DWORD    __stdcall AxmContiWriteClear(long lCoord);

    // Start registration of tasks to be executed in continuous interpolation at the specific  coordinate system. After call this API,
    // all motion tasks to be executed before calling of AxmContiEndNode API are not execute actual motion, but are registered as continuous interpolation motion, 
    // and when AxmContiStart API is called, then the registered motions execute actually
    DWORD    __stdcall AxmContiBeginNode(long lCoord);
    // Finish registration of tasks to be executed in continuous interpolation at the specific  coordinate system.
    DWORD    __stdcall AxmContiEndNode(long lCoord);

    // Start continuous interpolation.
    // if dwProfileset is  CONTI_NODE_VELOCITY(0), use continuous interpolation.
    //                     CONTI_NODE_MANUAL(1)  , set profile interpolation use. 
    //                     CONTI_NODE_AUTO(2)    , use auto-profile interpolation.
    DWORD    __stdcall AxmContiStart(long lCoord, DWORD dwProfileset, long lAngle); 
    // API which verifies whether the continuous interpolation is moving in the specific coordinate system. 
    DWORD    __stdcall AxmContiIsMotion(long lCoord, DWORD *upInMotion);

    // API which verifies the index number of the continuous interpolation that is being moving currently while the continuous interpolation is moving in the specific coordinate system. 
    DWORD    __stdcall AxmContiGetNodeNum(long lCoord, long *lpNodeNum);
    // API which verifies the total number of continuous interpolation index that were set in  the specific coordinate system. 
    DWORD    __stdcall AxmContiGetTotalNodeNum(long lCoord, long *lpNodeNum);

//==================== trigger API ===============================================================================================================================

    // Set whether to use of trigger function, output level, position comparator, trigger signal delay time and trigger output mode ino specific axis.
    //  dTrigTime  : trigger output time 
    //             : 1usec - max 50msec ( set 1 - 50000)
    //  upTriggerLevel  : whether to use or not use => LOW(0), HIGH(1), UNUSED(2), USED(3)
    //  uSelect         : Standard position to use  => COMMAND(0), ACTUAL(1)
    //  uInterrupt      : set interrupt             => DISABLE(0), ENABLE(1)
    
    // Set trigger signal delay time , trigger output level and trigger output method in specific axis. 
     DWORD    __stdcall AxmTriggerSetTimeLevel(long lAxisNo, double dTrigTime, DWORD uTriggerLevel, DWORD uSelect, DWORD uInterrupt);
    // Return trigger signal delay time , trigger output level and trigger output method to specific axis.
    DWORD    __stdcall AxmTriggerGetTimeLevel(long lAxisNo, double *dpTrigTime, DWORD *upTriggerLevel, DWORD *upSelect, DWORD *upInterrupt);
    
    //  uMethod :   PERIOD_MODE   0x0 : cycle trigger method using trigger position value
    //              ABS_POS_MODE   0x1 : Trigger occurs at trigger absolute position,  absolute position method
    //  dPos : in case of cycle selection : the relevant position  for output by position and position. 
    // in case of absolute selection: The position on which to output, If same as this position then output goes out unconditionally. 
    DWORD    __stdcall AxmTriggerSetAbsPeriod(long lAxisNo, DWORD uMethod, double dPos);
    // Return whether to use of trigger function, output level, position comparator, trigger signal delay time and trigger output mode to specific axis.
    DWORD    __stdcall AxmTriggerGetAbsPeriod(long lAxisNo, DWORD *upMethod, double *dpPos);

    //  Output the trigger by regular interval from the starting position to the ending position specified by user. 
    DWORD    __stdcall AxmTriggerSetBlock(long lAxisNo, double dStartPos, double dEndPos, double dPeriodPos);
    // Read trigger setting value of 'AxmTriggerSetBlock' API.
    DWORD    __stdcall AxmTriggerGetBlock(long lAxisNo, double *dpStartPos, double *dpEndPos, double *dpPeriodPos);

    // User outputs a trigger pulse.
    DWORD    __stdcall AxmTriggerOneShot(long lAxisNo);
    // User outputs a trigger pulse after several seconds. 
    DWORD    __stdcall AxmTriggerSetTimerOneshot(long lAxisNo, long lmSec);
    // Output absolute position trigger infinite absolute position output.
    DWORD    __stdcall AxmTriggerOnlyAbs(long lAxisNo,long lTrigNum, double* dpTrigPos);
    // Reset trigger settings.
    DWORD    __stdcall AxmTriggerSetReset(long lAxisNo); 

//======== CRC( Remaining pulse clear API) =====================================================================    
    //Level   : LOW(0), HIGH(1), UNUSED(2), USED(3) 
    //uMethod : Available to set the width of remaining pulse eliminating output signal pulse in 2 - 6.
    //          0: Don't care , 1: Don't care, 2: 500 uSec, 3:1 mSec, 4:10 mSec, 5:50 mSec, 6:100 mSec
    //Set whether to use CRC signal in specific axis and output level.
    DWORD    __stdcall AxmCrcSetMaskLevel(long lAxisNo, DWORD uLevel, DWORD uMethod);
    // Return whether to use CRC signal of specific axis and output level.
    DWORD    __stdcall AxmCrcGetMaskLevel(long lAxisNo, DWORD *upLevel, DWORD *upMethod);

    //uOnOff  : Whether to generate CRC signal to the Program or not.  (FALSE(0),TRUE(1))
    // Force to generate CRC signal to the specific axis.
    DWORD    __stdcall AxmCrcSetOutput(long lAxisNo, DWORD uOnOff);
    // Return whether to forcedly generate CRC signal of specific axis.
    DWORD    __stdcall AxmCrcGetOutput(long lAxisNo, DWORD *upOnOff);

//====== MPG(Manual Pulse Generation) API ===========================================================
    // lInputMethod : Available to set 0-3. 0:OnePhase, 1:TwoPhase1, 2:TwoPhase2, 3:TwoPhase4
    // lDriveMode   : Available to set 0-5
    //                0 :MPG continuous mode,             1 :MPG PRESET mode (Travel up to specific pulse), 2 :COMMAND ABSOLUTE MPG PRESET mode 
    //                3 :ACTUAL ABSOLUTE MPG PRESET mode, 4 :COMMAND ABSOLUTE ZERO MPG PRESET mode,         5 :ACTUAL ABSOLUTE ZERO MPG PRESET mode 
    // MPGPos        : the travel distance by every MPG input signal
    // MPGdenominator: Divide value in MPG(Enter manual pulse generating device)movement
    // dMPGnumerator : Product value in MPG(Enter manual pulse generating device)movement
    // dwNumerator   : Available to set max(from 1 to  64) 
    // dwDenominator : Available to set max(from 1 to  4096)
    // dMPGdenominator = 4096 and MPGnumerator=1 mean that
    // the output is 1 by 1 pulse as 1:1 if one turn of MPG is 200pulse. 
    // If dMPGdenominator = 4096 and MPGnumerator=2 , then it means the output is 2 by 2 pulse as 1:2.  
    // In here, MPG PULSE = ((Numerator) * (Denominator)/ 4096 ) It’s the calculation which outputs to the inside of chip.

    // Set MPG input method, Drive move mode, travel distance, MPG velocity in specific axis.
    DWORD    __stdcall AxmMPGSetEnable(long lAxisNo, long lInputMethod, long lDriveMode, double dMPGPos, double dVel, double dAccel);
    // Return MPG input method, Drive move mode, travel distance, MPG velocity in specific axis.
    DWORD    __stdcall AxmMPGGetEnable(long lAxisNo, long *lpInputMethod, long *lpDriveMode, double *dpMPGPos, double *dpVel, double *dAccel);

    // Set the pulse ratio to travel per pulse in MPG drive move mode to specific axis.
    DWORD    __stdcall AxmMPGSetRatio(long lAxisNo, DWORD uMPGnumerator, DWORD uMPGdenominator);
    // Return the pulse ratio to travel per pulse in MPG drive move mode to specific axis.
    DWORD    __stdcall AxmMPGGetRatio(long lAxisNo, DWORD *upMPGnumerator, DWORD *upMPGdenominator);
    
    // Release MPG drive settings to specific axis.
    DWORD    __stdcall AxmMPGReset(long lAxisNo);

//======= Helical move ===========================================================================
    // API which moves helical interpolation by specifying start point, ending point and center point to specific coordinate system.
    // API which moves helical continuous interpolation by specifying start point, ending point and center point to specific coordinate system when it is used with AxmContiBeginNode and  AxmContiEndNode together.  
    // API which saves in internal Queue in order to move the arc continuous interpolation. It is started using AxmContiStart API. ( It is used with continuous interpolation API together)
    //dCenterPos = center point X,Y , dEndPos = ending point X,Y.
    // uCWDir   DIR_CCW(0): Counterclockwise direction,   DIR_CW(1) Clockwise direction
    DWORD    __stdcall AxmHelixCenterMove(long lCoord, double dCenterXPos, double dCenterYPos, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dAccel, double dDecel, DWORD uCWDir);

    // API which moves helical interpolation by specifying start point, ending point and radius to specific coordinate system.
    // API which moves helical continuous interpolation by specifying middle point and ending point to specific coordinate system when it is used with AxmContiBeginNode and  AxmContiEndNode together.  
    // API which saves in internal Queue in order to move the arc continuous interpolation. It is started using AxmContiStart API. ( It is used with continuous interpolation API together)
    // dMidPos = middle point X,Y  , dEndPos = ending point X,Y 
    DWORD    __stdcall AxmHelixPointMove(long lCoord, double dMidXPos, double dMidYPos, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dAccel, double dDecel);

    // API which moves helical interpolation by specifying start point, ending point and radius to specific coordinate system.
    // API which moves helical continuous interpolation by specifying middle point ,ending point and radius to specific coordinate system when it is used with AxmContiBeginNode and  AxmContiEndNode together.  
    // API which saves in internal Queue in order to move the arc continuous interpolation. It is started using AxmContiStart API. ( It is used with continuous interpolation API together)
    // dRadius = radius, dEndPos = ending point X,Y  , uShortDistance = small circle(0), large circle(1)
    // uCWDir   DIR_CCW(0): Counterclockwise direction,   DIR_CW(1) Clockwise direction
    DWORD    __stdcall AxmHelixRadiusMove(long lCoord, double dRadius, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance);

    // API which moves helical interpolation by specifying start point, revolution angle and radius to specific coordinate system.
    // API which moves helical continuous interpolation by specifying start point , revolution angle and radius to specific coordinate system when it is used with AxmContiBeginNode and  AxmContiEndNode together.
    // API which saves in internal Queue in order to move the arc continuous interpolation. It is started using AxmContiStart API. ( It is used with continuous interpolation API together)
    // dCenterPos = center point X,Y , dAngle = angle.
    // uCWDir   DIR_CCW(0): Counterclockwise direction,   DIR_CW(1) Clockwise direction
    DWORD    __stdcall AxmHelixAngleMove(long lCoord, double dCenterXPos, double dCenterYPos, double dAngle, double dZPos, double dVel, double dAccel, double dDecel, DWORD uCWDir);

//======== Spline move =========================================================================== 
    // It’s not used with AxmContiBeginNode and AxmContiEndNode together. 
    // API which moves spline continuous interpolation. API which saves in internal Queue in order to move the arc continuous interpolation. 
    //It is started using AxmContiStart API. ( It is used with continuous interpolation API together)

    // lPosSize : Minimum more than 3 pieces.
    // Enter 0 as for dPoZ value when it is used with 2 axes. 
    // Enter 3 piece as for axis mapping and dPosZ value when it is used with 3axes.
    DWORD    __stdcall AxmSplineWrite(long lCoord, long lPosSize, double *dpPosX, double *dpPosY, double dVel, double dAccel, double dDecel, double dPosZ, long lPointFactor);

//======== Compensation Table ====================================================================
    // API which set the parameters for using the geometric compensation feature (Mechatrolink-II products)
    // lNumEntry : minimum entries are 2, maximum entries are 512.
    // dStartPos : starting relative position to apply the compensation.
    // dpPosition, dpCorrection : arrays of position and correction values.
    // bRollOver : enable/disable the roll over feature if the table can not cover the motor travel distance.
    DWORD __stdcall AxmCompensationSet(long lAxisNo, long lNumEntry, double dStartPos, double *dpPosition, double *dpCorrection, DWORD dwRollOver);
    // Return the number of entry, start position, array of position for moving, array of correction for compensating, setting RollOver feature on specific axis.
    DWORD __stdcall AxmCompensationGet(long lAxisNo, long *lpNumEntry, double *dpStartPos, double *dpPosition, double *dpCorrection, DWORD *dwRollOver);
    // API which enable/disable the compensation feature.
    DWORD __stdcall AxmCompensationEnable(long lAxisNo, DWORD dwEnable);
    // Return the setting enable/disable the compensation feature on specific axis.
    DWORD __stdcall AxmCompensationIsEnable(long lAxisNo, DWORD *dwpEnable);
    // Return Current Position Compensation Value
    DWORD __stdcall AxmCompensationGetCorrection(long lAxisNo, double *dpCorrection);
//======== Electronic CAM ========================================================================
    // API which set the parameters for using the Ecam feature (Mechatrolink-II products)
    // lAxisNo : one slave axis only has one master axis.
    // lMasterAxis : one master axis can have more than one slave axis.
    // lNumEntry : minimum entries are 2, maximum entries are 512.
    // dMasterStartPos : starting relative position to apply Ecam on master axis.    
    // dpMasterPos, dpSlavePos : arrays of position values on master and slave axis.
    DWORD __stdcall AxmEcamSet(long lAxisNo, long lMasterAxis, long lNumEntry, double dMasterStartPos, double *dpMasterPos, double *dpSlavePos);
    // Ecam Set With Select Source (COMMAND :0, ACTUAL : 1)
    DWORD __stdcall AxmEcamSetWithSource(long lAxisNo, long lMasterAxis, long lNumEntry, double dMasterStartPos, double *dpMasterPos, double *dpSlavePos, DWORD dwSource);
    // Return the number of master axis, entries, start position of master axis, array of position on master axis, array of position on slave axis.
    DWORD __stdcall AxmEcamGet(long lAxisNo, long *lpMasterAxis, long *lpNumEntry, double *dpMasterStartPos, double *dpMasterPos, double *dpSlavePos);
    // Ecam Get With Select Source
    DWORD __stdcall AxmEcamGetWithSource(long lAxisNo, long *lpMasterAxis, long *lpNumEntry, double *dpMasterStartPos, double *dpMasterPos, double *dpSlavePos, DWORD *dwpSource);
    // API which enable the Ecam feature on slave axis.
    DWORD __stdcall AxmEcamEnableBySlave(long lAxisNo, DWORD dwEnable);
    // Return the setting enable/disable the Ecam feature on slave axis.
    DWORD __stdcall AxmEcamEnableByMaster(long lAxisNo, DWORD dwEnable);
	// API which enable the Ecam feature on corresponding slave axes.
	DWORD __stdcall AxmEcamIsSlaveEnable(long lAxisNo, DWORD *dwpEnable);

//======== Servo Status Monitor =====================================================================================
	// Set exception function of specific axis. (Only for MLII, Sigma-5)
	DWORD __stdcall AxmStatusSetServoMonitor(long lAxisNo, DWORD dwSelMon, double dActionValue, DWORD dwAction);
	// Return exception function of specific axis. (Only for MLII, Sigma-5)
	DWORD __stdcall AxmStatusGetServoMonitor(long lAxisNo, DWORD dwSelMon, double *dpActionValue, DWORD *dwpAction);
	// Set exception function usage of specific axis. (Only for MLII, Sigma-5)
	DWORD __stdcall AxmStatusSetServoMonitorEnable(long lAxisNo, DWORD dwEnable);
	// Return exception function usage of specific axis. (Only for MLII, Sigma-5)
	DWORD __stdcall AxmStatusGetServoMonitorEnable(long lAxisNo, DWORD *dwpEnable);

	// Return exception function execution result Flag of specific axis. Auto reset after function execution. (Only for MLII, Sigma-5)
	DWORD __stdcall AxmStatusReadServoMonitorFlag(long lAxisNo, DWORD dwSelMon, DWORD *dwpMonitorFlag, double *dpMonitorValue);
	// Return exception function monitoring information of specific axis. (Only for MLII, Sigma-5)
	DWORD __stdcall AxmStatusReadServoMonitorValue(long lAxisNo, DWORD dwSelMon, double *dpMonitorValue);
	// Set load ratio monitor function of specific axis. (Only for MLII, Sigma-5)
	// dwSelMon = 0 : Accumulated load ratio
	// dwSelMon = 1 : Regenerative load ratio
	// dwSelMon = 2 : Reference Torque load ratio
	DWORD __stdcall AxmStatusSetReadServoLoadRatio(long lAxisNo, DWORD dwSelMon);
	// Return load ratio of specific axis. (Only for MLII, Sigma-5)
	DWORD __stdcall AxmStatusReadServoLoadRatio(long lAxisNo, double *dpMonitorValue);

	//======== Only for PCI-R1604-RTEX ==================================================================================
	// Set RTEX A4Nx Scale Coefficient. (Only for RTEX, A4Nx)
	DWORD __stdcall AxmMotSetScaleCoeff(long lAxisNo, long lScaleCoeff);
	// Return RTEX A4Nx Scale Coefficient. (Only for RTEX, A4Nx)
	DWORD __stdcall AxmMotGetScaleCoeff(long lAxisNo, long *lpScaleCoeff);
	// Edge detection of specific Input Signal that stop or slow down to stop the function.
	DWORD    __stdcall AxmMoveSignalSearchEx(long lAxisNo, double dVel, double dAccel, long lDetectSignal, long lSignalEdge, long lSignalMethod);
	//-------------------------------------------------------------------------------------------------------------------

	//======== Only for PCI-R1604-MLII ==================================================================================
	// Move to the set absolute position.
	// Velocity profile use Trapezoid.
	// Exits API at the point that pulse out starts.
	// Always position(include -position), Velocity, Accel/Deccel Change possible.
	DWORD    __stdcall AxmMoveToAbsPos(long lAxisNo, double dPos, double dVel, double dAccel, double dDecel);
	// Return current drive velocity of specific axis.
	DWORD    __stdcall AxmStatusReadVelEx(long lAxisNo, double *dpVel);
	//-------------------------------------------------------------------------------------------------------------------

	//======== Only for PCI-R1604-SSCNETIIIH ==================================================================================
	// Set electric ratio. This parameter saved Non-volatile memory.
	// Default value(lNumerator : 4194304(2^22), lDenominator : 10000)
	// MR-J4-B is don't Setting electric ratio, Must be set from the host controller.
	// No.PA06, No.PA07 of Existing Pulse type Servo Driver(MR-J4-A)
	DWORD    __stdcall AxmMotSetElectricGearRatio(long lAxisNo, long lNumerator, long lDenominator);
	// Return electric ratio of specific axis.
	DWORD    __stdcall AxmMotGetElectricGearRatio(long lAxisNo, long *lpNumerator, long *lpDenominator);

	// Set limit torque value of specific axis.
	// Forward, Backward drive torque limit function.
	// Setting range 1 ~ 1000
	// 0.1% of the maximum torque are controlled.
	DWORD    __stdcall AxmMotSetTorqueLimit(long lAxisNo, double dbPluseDirTorqueLimit, double dbMinusDirTorqueLimit);

	// Return torque limit value of specific axis.
	DWORD    __stdcall AxmMotGetTorqueLimit(long lAxisNo, double* dbpPluseDirTorqueLimit, double* dbpMinusDirTorqueLimit);

	DWORD    __stdcall AxmOverridePosSetFunction(long lAxisNo, DWORD dwUsage, long lDecelPosRatio, double dReserved);
	DWORD    __stdcall AxmOverridePosGetFunction(long lAxisNo, DWORD *dwpUsage, long *lpDecelPosRatio, double *dpReserved);
//-------------------------------------------------------------------------------------------------------------------

//======== PCI-R3200-MLIII 전용 함수==================================================================================
    // 잔류 진동 억제(VST) 특수 함수    
	// 사용전에 반드시 코디에 대해서 축을 할당을 해야하며, 코디 한개에 1개의 축만 맵핑을 해야한다.
	// 아래 함수 실행전에 반드시 Servo ON 상태에서 사용한다.
	// lCoordnate        : 입력 성형 적용 코디 번호를 입력한다. 각 보드별 첫번째 부터 10번째의 코디에 축을 할당해서 사용해야 한다.
	//                     MLIII 마스터 보드는 보드 번호를 기준으로 16 ~ 31까지 보드 별로 순차적으로 16씩 증가된다.
	//                     MLIII B/D 0 : 16 ~ 31
	//                     MLIII B/D 1 : 31 ~ 47
	// cISTSize          : 입력 성형 사용 주파수 개수에 대해서 입력한다. 1로 값을 고정해서 사용한다.
	// dbpFrequency,	 : 10H ~ 500Hz
	//                     1차 주파수 부터 순서데로 입력한다.(저주파부터 고주파).
	// dbpDampingRatio   : 0.001 ~ 0.9
	// dwpImpulseCount   : 2 ~ 5
	DWORD    __stdcall AxmAdvVSTSetParameter(long lCoord, DWORD dwISTSize, double* dbpFrequency, double* dbpDampingRatio, DWORD* dwpImpulseCount);
	DWORD    __stdcall AxmAdvVSTGetParameter(long lCoord, DWORD* dwpISTSize, double* dbpFrequency, double* dbpDampingRatio, DWORD* dwpImpulseCount);
    // lCoordnate        : 입력 성형 코디 번호를 입력한다.
    // dwISTEnable       : 입력 성형 사용 유무를 결정한다.
	DWORD    __stdcall AxmAdvVSTSetEnabele(long lCoord, DWORD dwISTEnable);
	DWORD    __stdcall AxmAdvVSTGetEnabele(long lCoord, DWORD* dwpISTEnable);

//====일반 보간함수 =================================================================================================	
    // 직선 보간 한다.
    // 시작점과 종료점을 지정하여 다축 직선 보간 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점과 종료점을 지정하여 직선 보간 구동하는 Queue에 저장함수가된다. 
    // 직선 프로파일 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
        DWORD    __stdcall AxmAdvLineMove(long lCoordinate, double *dPosition, double dMaxVelocity, double dStartVel, double dStopVel, double dMaxAccel, double dMaxDecel);
    // 지정된 좌표계에 시작점과 종료점을 지정하는 다축 직선 보간 오버라이드하는 함수이다.
    // 현재 진행중인 보간구동을 지정된 속도 및 위치로 직선 보간 오버라이드 하며, 다음 노드에 대한 직선 보간 구동 예약도 가능한다.
    // IOverrideMode = 0일 경우, 구동중인 보간이 직선, 원호 보간에 관계없이 현재 구동 노드에서 직선 보간으로 즉시 오버라이드 되고, 
    // IOverrideMode = 1이면 현재 구동 노드 다음의 노드부터 직선보간이 차례로 예약된다.
    // IOverrideMode = 1로 본 함수를 호출할때마다 최소 1개에서 최대 8개까지 오버라이드 큐에 증가되면서 자동적으로 예약이 되며, 예약 후 마지막에 IOverrideMode = 0으로 본 함수가 호출되면
    // 내부적으로 오버라이드 큐에 있는 예약 보간들이 연속보간 큐로 저장되고, 직선 오버라이드 구동과 이후의 예약된 연속보간이 순차적으로 실행된다.
        DWORD    __stdcall AxmAdvOvrLineMove(long lCoordinate, double *dPosition, double dMaxVelocity, double dStartVel, double dStopVel, double dMaxAccel, double dMaxDecel, long lOverrideMode);
    // 2축 원호보간 한다.
    // 시작점, 종료점과 중심점을 지정하여 원호 보간 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode, 와 같이사용시 지정된 좌표계에 시작점, 종료점과 중심점을 지정하여 구동하는 원호 보간 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dCenterPos = 중심점 X,Y 배열 , dEndPos = 종료점 X,Y 배열.
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvCircleCenterMove(long lCoord, long *lAxisNo, double *dCenterPos, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir);
    // 중간점, 종료점을 지정하여 원호 보간 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 중간점, 종료점을 지정하여 구동하는 원호 보간 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dMidPos = 중간점 X,Y 배열 , dEndPos = 종료점 X,Y 배열, lArcCircle = 아크(0), 원(1)
    DWORD    __stdcall AxmAdvCirclePointMove(long lCoord, long *lAxisNo, double *dMidPos, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, long lArcCircle);
    // 시작점, 회전각도와 반지름을 지정하여 원호 보간 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 회전각도와 반지름을 지정하여 원호 보간 구동하는 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dCenterPos = 중심점 X,Y 배열 , dAngle = 각도.
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvCircleAngleMove(long lCoord, long *lAxisNo, double *dCenterPos, double dAngle, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir);
    // 시작점, 종료점과 반지름을 지정하여 원호 보간 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 원호 보간 구동하는 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dRadius = 반지름, dEndPos = 종료점 X,Y 배열 , uShortDistance = 작은원(0), 큰원(1)
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvCircleRadiusMove(long lCoord, long *lAxisNo, double dRadius, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance);
    // 지정된 좌표계에 시작점과 종료점을 지정하여 2축 원호 보간 오버라이드 구동한다.
    // 현재 진행중인 보간구동을 지정된 속도 및 위치로 원호 보간 오버라이드 하며, 다음 노드에 대한 원호 보간 구동 예약도 가능한다.
    // IOverrideMode = 0일 경우, 구동중인 보간이 직선, 원호 보간에 관계없이 현재 구동 노드에서 원호 보간으로 즉시 오버라이드 되고, 
    // IOverrideMode = 1이면 현재 구동 노드 다음의 노드부터 원호보간이 차례로 예약된다.
    // IOverrideMode = 1로 본 함수를 호출할때마다 최소 1개에서 최대 8개까지 오버라이드 큐에 증가되면서 자동적으로 예약이 되며, 예약 후 마지막에 IOverrideMode = 0으로 본 함수가 호출되면
    // 내부적으로 오버라이드 큐에 있는 예약 보간들이 연속보간 큐로 저장되고, 원호 오버라이드 구동과 이후의 예약된 연속보간이 순차적으로 실행된다.
    DWORD    __stdcall AxmAdvOvrCircleRadiusMove(long lCoord, long *lAxisNo, double dRadius, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance, long lOverrideMode);

//======= 헬리컬 이동 ===============================================================================================
    // 주의사항 : Helix를 연속보간 사용시 Spline, 직선보간과 원호보간을 같이 사용할수없다.
    
    // 지정된 좌표계에 시작점, 종료점과 중심점을 지정하여 헬리컬 보간 구동하는 함수이다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 종료점과 중심점을 지정하여 헬리컬 연속보간 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다)
    // dCenterPos = 중심점 X,Y  , dEndPos = 종료점 X,Y .
    // uCWDir DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향    	
    DWORD    __stdcall AxmAdvHelixCenterMove(long lCoord, double dCenterXPos, double dCenterYPos, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir);
    // 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 헬리컬 보간 구동하는 함수이다. 
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 중간점, 종료점을 지정하여 헬리컬연속 보간 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다.)
    // dMidPos = 중간점 X,Y  , dEndPos = 종료점 X,Y 
    DWORD    __stdcall AxmAdvHelixPointMove(long lCoord, double dMidXPos, double dMidYPos, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel);
    // 지정된 좌표계에 시작점, 회전각도와 반지름을 지정하여 헬리컬 보간 구동하는 함수이다
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 회전각도와 반지름을 지정하여 헬리컬연속 보간 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다.)
    //dCenterPos = 중심점 X,Y  , dAngle = 각도.
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvHelixAngleMove(long lCoord, double dCenterXPos, double dCenterYPos, double dAngle, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir);
    // 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 헬리컬 보간 구동하는 함수이다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 헬리컬연속 보간 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다.)
    // dRadius = 반지름, dEndPos = 종료점 X,Y  , uShortDistance = 작은원(0), 큰원(1)
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향   
    DWORD    __stdcall AxmAdvHelixRadiusMove(long lCoord, double dRadius, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance);
    // 지정된 좌표계에 시작점과 종료점을 지정하여 3축 헬리컬 보간 오버라이드 구동한다.
    // 현재 진행중인 보간구동을 지정된 속도 및 위치로 헬리컬 보간 오버라이드 하며, 다음 노드에 대한 헬리컬 보간 구동 예약도 가능한다.
    // IOverrideMode = 0일 경우, 구동중인 보간이 직선, 원호 보간에 관계없이 현재 구동 노드에서 헬리컬 보간으로 즉시 오버라이드 되고, 
    // IOverrideMode = 1이면 현재 구동 노드 다음의 노드부터 헬리컬 보간이 차례로 예약된다.
    // IOverrideMode = 1로 본 함수를 호출할때마다 최소 1개에서 최대 8개까지 오버라이드 큐에 증가되면서 자동적으로 예약이 되며, 예약 후 마지막에 IOverrideMode = 0으로 본 함수가 호출되면
    // 내부적으로 오버라이드 큐에 있는 예약 보간들이 연속보간 큐로 저장되고, 헬리컬 오버라이드 구동과 이후의 예약된 연속보간이 순차적으로 실행된다.
    DWORD    __stdcall AxmAdvOvrHelixRadiusMove(long lCoord, double dRadius, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance, long lOverrideMode);

//====일반 보간함수 =================================================================================================
    // 직선 보간을 예약 구동한다.
    // 시작점과 종료점을 지정하여 다축 직선 보간을 예약 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점과 종료점을 지정하여 직선 보간 구동하는 Queue에 저장함수가된다. 
    // 직선 프로파일 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    DWORD    __stdcall AxmAdvScriptLineMove(long lCoordinate, double *dPosition, double dMaxVelocity, double dStartVel, double dStopVel, double dMaxAccel, double dMaxDecel, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 지정된 좌표계에 시작점과 종료점을 지정하는 다축 직선 보간 오버라이드를 예약 구동하는 함수이다.
    // 현재 진행중인 보간구동을 지정된 속도 및 위치로 직선 보간 오버라이드를 예약 구동 하며, 다음 노드에 대한 직선 보간 구동 예약이 가능한다.
    // IOverrideMode = 0일 경우, 구동중인 보간이 직선, 원호 보간에 관계없이 현재 구동 노드에서 직선 보간으로 즉시 오버라이드 되고, 
    // IOverrideMode = 1이면 현재 구동 노드 다음의 노드부터 직선보간이 차례로 예약된다.
    // IOverrideMode = 1로 본 함수를 호출할때마다 최소 1개에서 최대 8개까지 오버라이드 큐에 증가되면서 자동적으로 예약이 되며, 예약 후 마지막에 IOverrideMode = 0으로 본 함수가 호출되면
    // 내부적으로 오버라이드 큐에 있는 예약 보간들이 연속보간 큐로 저장되고, 직선 오버라이드 구동과 이후의 예약된 연속보간이 순차적으로 실행된다.
    DWORD    __stdcall AxmAdvScriptOvrLineMove(long lCoordinate, double *dPosition, double dMaxVelocity, double dStartVel, double dStopVel, double dMaxAccel, double dMaxDecel, long lOverrideMode, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 2축 원호보간을 예약 구동한다.
    // 시작점, 종료점과 중심점을 지정하여 원호 보간을 예약 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode, 와 같이사용시 지정된 좌표계에 시작점, 종료점과 중심점을 지정하여 구동하는 원호 보간 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dCenterPos = 중심점 X,Y 배열 , dEndPos = 종료점 X,Y 배열.
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvScriptCircleCenterMove(long lCoord, long *lAxisNo, double *dCenterPos, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 중간점, 종료점을 지정하여 원호 보간을 예약 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 중간점, 종료점을 지정하여 구동하는 원호 보간 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dMidPos = 중간점 X,Y 배열 , dEndPos = 종료점 X,Y 배열, lArcCircle = 아크(0), 원(1)
    DWORD    __stdcall AxmAdvScriptCirclePointMove(long lCoord, long *lAxisNo, double *dMidPos, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, long lArcCircle, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 시작점, 회전각도와 반지름을 지정하여 원호 보간을 예약 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 회전각도와 반지름을 지정하여 원호 보간 구동하는 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dCenterPos = 중심점 X,Y 배열 , dAngle = 각도.
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvScriptCircleAngleMove(long lCoord, long *lAxisNo, double *dCenterPos, double dAngle, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 시작점, 종료점과 반지름을 지정하여 원호 보간을 예약 구동하는 함수이다. 구동 시작 후 함수를 벗어난다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 원호 보간 구동하는 Queue에 저장함수가된다.
    // 프로파일 원호 연속 보간 구동을 위해 내부 Queue에 저장하여 AxmAdvContiStart함수를 사용해서 시작한다.
    // lAxisNo = 두축 배열 , dRadius = 반지름, dEndPos = 종료점 X,Y 배열 , uShortDistance = 작은원(0), 큰원(1)
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvScriptCircleRadiusMove(long lCoord, long *lAxisNo, double dRadius, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 지정된 좌표계에 시작점과 종료점을 지정하여 2축 원호 보간 오버라이드를 예약 구동한다.
    // 현재 진행중인 보간구동을 지정된 속도 및 위치로 원호 보간 오버라이드을 예약 구동하며, 다음 노드에 대한 원호 보간 구동 예약이 가능한다.
    // IOverrideMode = 0일 경우, 구동중인 보간이 직선, 원호 보간에 관계없이 현재 구동 노드에서 원호 보간으로 즉시 오버라이드 되고, 
    // IOverrideMode = 1이면 현재 구동 노드 다음의 노드부터 원호보간이 차례로 예약된다.
    // IOverrideMode = 1로 본 함수를 호출할때마다 최소 1개에서 최대 8개까지 오버라이드 큐에 증가되면서 자동적으로 예약이 되며, 예약 후 마지막에 IOverrideMode = 0으로 본 함수가 호출되면
    // 내부적으로 오버라이드 큐에 있는 예약 보간들이 연속보간 큐로 저장되고, 원호 오버라이드 구동과 이후의 예약된 연속보간이 순차적으로 실행된다.
    DWORD    __stdcall AxmAdvScriptOvrCircleRadiusMove(long lCoord, long *lAxisNo, double dRadius, double *dEndPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance, long lOverrideMode, DWORD dwScript, long lScirptAxisNo, double dScriptPos);

//======= 헬리컬 이동 ===============================================================================================
    // 주의사항 : Helix를 연속보간 사용시 Spline, 직선보간과 원호보간을 같이 사용할수없다.

    // 지정된 좌표계에 시작점, 종료점과 중심점을 지정하여 헬리컬 보간을 예약 구동하는 함수이다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 종료점과 중심점을 지정하여 헬리컬 연속보간을 예약 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다)
    // dCenterPos = 중심점 X,Y  , dEndPos = 종료점 X,Y .
    // uCWDir DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향	
    DWORD    __stdcall AxmAdvScriptHelixCenterMove(long lCoord, double dCenterXPos, double dCenterYPos, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 헬리컬 보간을 예약 구동하는 함수이다. 
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 중간점, 종료점을 지정하여 헬리컬연속 보간을 예약 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다.)
    // dMidPos = 중간점 X,Y  , dEndPos = 종료점 X,Y
    DWORD    __stdcall AxmAdvScriptHelixPointMove(long lCoord, double dMidXPos, double dMidYPos, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 지정된 좌표계에 시작점, 회전각도와 반지름을 지정하여 헬리컬 보간을 예약 구동하는 함수이다
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 회전각도와 반지름을 지정하여 헬리컬연속 보간을 예약 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다.)
    //dCenterPos = 중심점 X,Y  , dAngle = 각도.
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향
    DWORD    __stdcall AxmAdvScriptHelixAngleMove(long lCoord, double dCenterXPos, double dCenterYPos, double dAngle, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 헬리컬 보간을 예약 구동하는 함수이다.
    // AxmAdvContiBeginNode, AxmAdvContiEndNode와 같이사용시 지정된 좌표계에 시작점, 종료점과 반지름을 지정하여 헬리컬연속 보간을 예약 구동하는 함수이다. 
    // 원호 연속 보간 구동을 위해 내부 Queue에 저장하는 함수이다. AxmAdvContiStart함수를 사용해서 시작한다. (연속보간 함수와 같이 이용한다.)
    // dRadius = 반지름, dEndPos = 종료점 X,Y  , uShortDistance = 작은원(0), 큰원(1)
    // uCWDir   DIR_CCW(0): 반시계방향, DIR_CW(1) 시계방향  
    DWORD    __stdcall AxmAdvScriptHelixRadiusMove(long lCoord, double dRadius, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance, DWORD dwScript, long lScirptAxisNo, double dScriptPos);
    // 지정된 좌표계에 시작점과 종료점을 지정하여 3축 헬리컬 보간 오버라이드를 예약 구동한다.
    // 현재 진행중인 보간구동을 지정된 속도 및 위치로 헬리컬 보간 오버라이드를 예약 구동한다.
    // IOverrideMode = 0일 경우, 구동중인 보간이 직선, 원호 보간에 관계없이 현재 구동 노드에서 헬리컬 보간으로 즉시 오버라이드 되고, 
    // IOverrideMode = 1이면 현재 구동 노드 다음의 노드부터 헬리컬 보간이 차례로 예약된다.
    // IOverrideMode = 1로 본 함수를 호출할때마다 최소 1개에서 최대 8개까지 오버라이드 큐에 증가되면서 자동적으로 예약이 되며, 예약 후 마지막에 IOverrideMode = 0으로 본 함수가 호출되면
    // 내부적으로 오버라이드 큐에 있는 예약 보간들이 연속보간 큐로 저장되고, 헬리컬 오버라이드 구동과 이후의 예약된 연속보간이 순차적으로 실행된다.
    DWORD    __stdcall AxmAdvScriptOvrHelixRadiusMove(long lCoord, double dRadius, double dEndXPos, double dEndYPos, double dZPos, double dVel, double dStartVel, double dStopVel, double dAccel, double dDecel, DWORD uCWDir, DWORD uShortDistance, long lOverrideMode, DWORD dwScript, long lScirptAxisNo, double dScriptPos);

//====연속 보간 함수 ================================================================================================
    // 지정된 좌표계에 연속 보간 구동 중 현재 구동중인 연속 보간 인덱스 번호를 확인하는 함수이다.
    DWORD    __stdcall AxmAdvContiGetNodeNum(long lCoordinate, long *lpNodeNum);
    // 지정된 좌표계에 설정한 연속 보간 구동 총 인덱스 갯수를 확인하는 함수이다.
    DWORD    __stdcall AxmAdvContiGetTotalNodeNum(long lCoordinate, long *lpNodeNum);
    // 지정된 좌표계에 보간 구동을 위한 내부 Queue에 저장되어 있는 보간 구동 개수를 확인하는 함수이다.
    DWORD    __stdcall AxmAdvContiReadIndex(long lCoordinate, long *lpQueueIndex);
    // 지정된 좌표계에 보간 구동을 위한 내부 Queue가 비어 있는지 확인하는 함수이다.
    DWORD    __stdcall AxmAdvContiReadFree(long lCoordinate, DWORD *upQueueFree);
    // 지정된 좌표계에 연속 보간 구동을 위해 저장된 내부 Queue를 모두 삭제하는 함수이다.
    DWORD    __stdcall AxmAdvContiWriteClear(long lCoordinate);
    // 지정된 좌표계에 연속 보간 오버라이드 구동을 위해 예약된 오버라이드용 큐를 모두 삭제하는 함수이다.
    DWORD    __stdcall AxmAdvOvrContiWriteClear(long lCoordinate);
    // 연속 보간 시작 한다.
    DWORD    __stdcall AxmAdvContiStart(long lCoord, DWORD dwProfileset, long lAngle); 
    // 연속 보간 정지 한다.
    DWORD    __stdcall AxmAdvContiStop(long lCoordinate, double dDecel);
    //지정된 좌표계에 연속보간 축 맵핑을 설정한다.
    //(축맵핑 번호는 0 부터 시작))
    // 주의점:  축맵핑할때는 반드시 실제 축번호가 작은 숫자부터 큰숫자를 넣는다.
    //          가상축 맵핑 함수를 사용하였을 때 가상축번호를 실제 축번호가 작은 값 부터 lpAxesNo의 낮은 인텍스에 입력하여야 한다.
    //          가상축 맵핑 함수를 사용하였을 때 가상축번호에 해당하는 실제 축번호가 다른 값이라야 한다.
    //          같은 축을 다른 Coordinate에 중복 맵핑하지 말아야 한다.
    DWORD    __stdcall AxmAdvContiSetAxisMap(long lCoord, long lSize, long *lpAxesNo);
    //지정된 좌표계에 연속보간 축 맵핑을 반환한다.
    DWORD    __stdcall AxmAdvContiGetAxisMap(long lCoord, long *lpSize, long *lpAxesNo);
    // 지정된 좌표계에 연속보간 축 절대/상대 모드를 설정한다.
    // (주의점 : 반드시 축맵핑 하고 사용가능)
    // 지정 축의 이동 거리 계산 모드를 설정한다.
    //uAbsRelMode : POS_ABS_MODE '0' - 절대 좌표계
    //              POS_REL_MODE '1' - 상대 좌표계
    DWORD    __stdcall AxmAdvContiSetAbsRelMode(long lCoord, DWORD uAbsRelMode);
    // 지정된 좌표계에 연속보간 축 절대/상대 모드를 반환한다.
    DWORD    __stdcall AxmAdvContiGetAbsRelMode(long lCoord, DWORD *uAbsRelMode);
    // 지정된 좌표계에 연속 보간 구동 중인지 확인하는 함수이다.
    DWORD    __stdcall AxmAdvContiIsMotion(long lCoordinate, DWORD *upInMotion);
    // 지정된 좌표계에 연속보간에서 수행할 작업들의 등록을 시작한다. 이함수를 호출한후,
    // AxmAdvContiEndNode함수가 호출되기 전까지 수행되는 모든 모션작업은 실제 모션을 수행하는 것이 아니라 연속보간 모션으로 등록 되는 것이며,
    // AxmAdvContiStart 함수가 호출될 때 비로소 등록된모션이 실제로 수행된다.
    DWORD    __stdcall AxmAdvContiBeginNode(long lCoord);
    // 지정된 좌표계에서 연속보간을 수행할 작업들의 등록을 종료한다.
    DWORD    __stdcall AxmAdvContiEndNode(long lCoord);
    
    // 지정한 다축을 설정한 감속도로 동기 감속 정지한다.
    DWORD    __stdcall AxmMoveMultiStop(long lArraySize, long *lpAxesNo, double *dMaxDecel);
    // 지정한 다축을 동기 급 정지한다.
    DWORD    __stdcall AxmMoveMultiEStop(long lArraySize, long *lpAxesNo);
    // 지정한 다축을 동기 감속 정지한다.
    DWORD    __stdcall AxmMoveMultiSStop(long lArraySize, long *lpAxesNo);

    // 지정한 축의 실제 구동 속도를 읽어온다.
    DWORD    __stdcall AxmStatusReadActVel(long lAxisNo, double *dpVel);          //2010.10.11
    // 서보 타입 슬레이브 기기의 SVCMD_STAT 커맨드 값을 읽는다.
    DWORD    __stdcall AxmStatusReadServoCmdStat(long lAxisNo, DWORD *upStatus);
    // 서보 타입 슬레이브 기기의 SVCMD_CTRL 커맨드 값을 읽는다.
    DWORD    __stdcall AxmStatusReadServoCmdCtrl(long lAxisNo, DWORD *upStatus);
    
    // 겐트리 구동시 마스터 축과 슬레이브 축 간의 위치 차에 대한 설정된 오차 한계값을 반환한다.    
    DWORD    __stdcall AxmGantryGetMstToSlvOverDist(long lAxisNo, double *dpPosition);
    // 겐트리 구동시 마스터 축과 슬레이브 축 간의 위치 차에 대한 오차 한계값을 설정한다.
    DWORD    __stdcall AxmGantrySetMstToSlvOverDist(long lAxisNo, double dPosition);

    // 지정 축의 알람 신호의 코드 상태를 반환한다.
    DWORD    __stdcall AxmSignalReadServoAlarmCode(long lAxisNo, WORD *upCodeStatus);
    
    // 서보 타입 슬레이브 기기의 좌표계 설정을 실시한다. (MLIII 전용)
    DWORD    __stdcall AxmM3ServoCoordinatesSet(long lAxisNo, DWORD dwPosData, DWORD dwPos_sel, DWORD dwRefe);
    // 서보 타입 슬레이브 기기의 브레이크 작동 신호를 출력한다. (MLIII 전용)
    DWORD    __stdcall AxmM3ServoBreakOn(long lAxisNo);
    // 서보 타입 슬레이브 기기의 브레이크 작동 신호를 해제한다. (MLIII 전용)
    DWORD    __stdcall AxmM3ServoBreakOff(long lAxisNo);
    // 서보 타입 슬레이브 기기의 
    DWORD    __stdcall AxmM3ServoConfig(long lAxisNo, DWORD dwCfMode);
    // 서보 타입 슬레이브 기기의 센서 종보 초기화를 요구한다.
    DWORD    __stdcall AxmM3ServoSensOn(long lAxisNo);
    // 서보 타입 슬레이브 기기의 센서전원 OFF를 요구한다.
    DWORD    __stdcall AxmM3ServoSensOff(long lAxisNo);
    // 서보 타입 슬레이브 기기의 SMON 커맨드를 실행한다.
    DWORD    __stdcall AxmM3ServoSmon(long lAxisNo);
    // 서보 타입 슬레이브 기기의 모니터 정보나 입출력 신호의 상태를 읽는다.
    DWORD    __stdcall AxmM3ServoGetSmon(long lAxisNo,BYTE *pbParam);
    // 서보 타입 슬레이브 기기에 서보 ON을 요구한다.
    DWORD    __stdcall AxmM3ServoSvOn(long lAxisNo);
    // 서보 타입 슬레이브 기기에 서보 OFF를 요구한다.
    DWORD    __stdcall AxmM3ServoSvOff(long lAxisNo);
    // 서보 타입 슬레이브 기기가 지정된 보간 위치로 보간이동을 실시한다.
    DWORD    __stdcall AxmM3ServoInterpolate(long lAxisNo, DWORD dwTPOS, DWORD dwVFF, DWORD dwTFF, DWORD dwTLIM);
    // 서보 타입 슬레이브 기기가 지정한 위치로 위치이송을 실시한다.
    DWORD    __stdcall AxmM3ServoPosing(long lAxisNo, DWORD dwTPOS, DWORD dwSPD, DWORD dwACCR, DWORD dwDECR, DWORD dwTLIM);
    // 서보 타입 슬레이브 기기가 지정된 이동속도로 정속이송을 실시한다.
    DWORD    __stdcall AxmM3ServoFeed(long lAxisNo, long lSPD, DWORD dwACCR, DWORD dwDECR, DWORD dwTLIM);
    // 서보 타입 슬레이브 기기가 이송중 외부 위치결정 신호의 입력에 의해 위치이송을 실시한다.
    DWORD    __stdcall AxmM3ServoExFeed(long lAxisNo, long lSPD, DWORD dwACCR, DWORD dwDECR, DWORD dwTLIM, DWORD dwExSig1, DWORD dwExSig2);
    // 서보 타입 슬레이브 기기가 외부 위치결정 신호 입력에 의해 위치이송을 실시한다.
    DWORD    __stdcall AxmM3ServoExPosing(long lAxisNo, DWORD dwTPOS, DWORD dwSPD, DWORD dwACCR, DWORD dwDECR, DWORD dwTLIM, DWORD dwExSig1, DWORD dwExSig2);
    // 서보 타입 슬레이브 기기가 원점 복귀를 실시한다.
    DWORD    __stdcall AxmM3ServoZret(long lAxisNo, DWORD dwSPD, DWORD dwACCR, DWORD dwDECR, DWORD dwTLIM, DWORD dwExSig1, DWORD dwExSig2, BYTE bHomeDir, BYTE bHomeType);
    // 서보 타입 슬레이브 기기가 속도제어를 실시한다.
    DWORD    __stdcall AxmM3ServoVelctrl(long lAxisNo, DWORD dwTFF, DWORD dwVREF, DWORD dwACCR, DWORD dwDECR, DWORD dwTLIM);
    // 서보 타입 슬레이브 기기가 토크제어를 실시한다.    
    DWORD    __stdcall AxmM3ServoTrqctrl(long lAxisNo, DWORD dwVLIM, long lTQREF);
    // bmode 0x00 : common parameters ram
    // bmode 0x01 : common parameters flash
    // bmode 0x10 : device parameters ram
    // bmode 0x11 : device parameters flash
    // 서보 타입 슬레이브 기기의 서보팩 특정 파라미터 설정값을 반환한다.
    DWORD    __stdcall AxmM3ServoGetParameter(long lAxisNo, WORD wNo, BYTE bSize, BYTE bMode, BYTE *pbParam);
    // 서보 타입 슬레이브 기기의 서보팩 특정 파라미터 값을 설정한다.
    DWORD    __stdcall AxmM3ServoSetParameter(long lAxisNo, WORD wNo, BYTE bSize, BYTE bMode, BYTE *pbParam);
    // 서보 타입 슬레이브 기기의 설정된 토크 제한 값을 반환한다.
    DWORD	__stdcall AxmM3ServoGetTorqLimit(long lAxisNo, DWORD *dwpTorqLimit);
    // 서보 타입 슬레이브 기기에 토크 제한 값을 설정한다.
    DWORD    __stdcall AxmM3ServoSetTorqLimit(long lAxisNo, DWORD dwTorqLimit);

    // 서보 타입 슬레이브 기기에 설정된 SVCMD_IO 커맨드 값을 반환한다.
    DWORD	__stdcall AxmM3ServoGetSendSvCmdIOOutput(long lAxisNo, DWORD *dwData);
    // 서보 타입 슬레이브 기기에 SVCMD_IO 커맨드 값을 설정한다.
    DWORD    __stdcall AxmM3ServoSetSendSvCmdIOOutput(long lAxisNo, DWORD dwData);
  
    // 서보 타입 슬레이브 기기에 설정된 SVCMD_CTRL 커맨드 값을 반환한다.
    DWORD	__stdcall AxmM3ServoGetSvCmdCtrl(long lAxisNo, DWORD *dwData);
    // 서보 타입 슬레이브 기기에 SVCMD_CTRL 커맨드 값을 설정한다.
    DWORD    __stdcall AxmM3ServoSetSvCmdCtrl(long lAxisNo, DWORD dwData);

    // MLIII adjustment operation을 설정 한다.
    // dwReqCode == 0x1005 : parameter initialization : 20sec
    // dwReqCode == 0x1008 : absolute encoder reset   : 5sec
    // dwReqCode == 0x100E : automatic offset adjustment of motor current detection signals  : 5sec
    // dwReqCode == 0x1013 : Multiturn limit setting  : 5sec
	DWORD   __stdcall AxmM3AdjustmentOperation(long lAxisNo, DWORD dwReqCode);
    // 서보 축 추가 모니터링 채널별 선택 값을 설정한다.
	DWORD   __stdcall AxmM3ServoSetMonSel(long lAxisNo, DWORD dwMon0, DWORD dwMon1, DWORD dwMon2);
    // 서보 축 추가 모니터링 채널별 설정 값을 반환한다.
	DWORD   __stdcall AxmM3ServoGetMonSel(long lAxisNo, DWORD *upMon0, DWORD *upMon1, DWORD *upMon2);
    // 서보 축 추가 모니터링 채널별 설정 값을 기준으로 현재 상태를 반환한다.
	DWORD   __stdcall AxmM3ServoReadMonData(long lAxisNo, DWORD dwMonSel, DWORD *dwpMonData);
    // 제어할 토크 축 설정
	DWORD	__stdcall AxmAdvTorqueContiSetAxisMap(long lCoord, long lSize, long *lpAxesNo, DWORD dwTLIM, DWORD dwConMode);
	// 2014.04.28
	// 토크 프로파일 설정 파라미터
	DWORD	__stdcall AxmM3ServoSetTorqProfile(long lCoord, long lAxisNo, long TorqueSign, DWORD dwVLIM, DWORD dwProfileMode, DWORD dwStdTorq, DWORD dwStopTorq);
    // 토크 프로파일 확인 파라미터
	DWORD	__stdcall AxmM3ServoGetTorqProfile(long lCoord, long lAxisNo, long *lpTorqueSign, DWORD *updwVLIM, DWORD *upProfileMode, DWORD *upStdTorq, DWORD *upStopTorq);
//-------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_AXM_H__
