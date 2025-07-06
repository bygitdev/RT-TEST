/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** AXHS.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Resource Define Header File
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

#ifndef __AXT_AXHS_H__
#define __AXT_AXHS_H__

#ifndef __DATA_TYPES__
    #define __DATA_TYPES__
    typedef unsigned long int                    DWORD;
    typedef unsigned short int                    WORD;
    typedef unsigned char                        BYTE;
    typedef int                                    BOOL;
    typedef void                                *HANDLE;
#endif

// Define Baseboard
#ifndef __AXT_BASE_BOARD__
    #define __AXT_BASE_BOARD__
    typedef enum _AXT_BASE_BOARD
    {
        AXT_UNKNOWN                   = 0x00,             // Unknown Baseboard
        AXT_BIHR                      = 0x01,             // ISA bus, Half size
        AXT_BIFR                      = 0x02,             // ISA bus, Full size
        AXT_BPHR                      = 0x03,             // PCI bus, Half size
        AXT_BPFR                      = 0x04,             // PCI bus, Full size
        AXT_BV3R                      = 0x05,             // VME bus, 3U size
        AXT_BV6R                      = 0x06,             // VME bus, 6U size
        AXT_BC3R                      = 0x07,             // cPCI bus, 3U size
        AXT_BC6R                      = 0x08,             // cPCI bus, 6U size
        AXT_BEHR                      = 0x09,             // PCIe bus, Half size
	AXT_BEFR                      = 0x0A,             // PCIe bus, Full size
        AXT_BEHD                      = 0x0B,             // PCIe bus, Half size, DB-32T
        AXT_FMNSH4D                   = 0x52,             // ISA bus, Full size, DB-32T, SIO-2V03 * 2
        AXT_PCI_DI64R                 = 0x43,             // PCI bus, Digital IN 64 point
        AXT_PCIE_DI64R                = 0x44,             // PCIe bus, Digital IN 64 point
        AXT_PCI_DO64R                 = 0x53,             // PCI bus, Digital OUT 64 point
        AXT_PCIE_DO64R                = 0x54,             // PCIe bus, Digital OUT 64 point
        AXT_PCI_DB64R                 = 0x63,             // PCI bus, Digital IN 32 point, OUT 32 point
        AXT_PCIE_DB64R                = 0x64,             // PCIe bus, Digital IN 32��, OUT 32 point
	AXT_PCIN204                   = 0x82,             // PCI bus, Half size On-Board 2 Axis controller.
        AXT_BPHD                      = 0x83,             // PCI bus, Half size, DB-32T
        AXT_PCIN404                   = 0x84,             // PCI bus, Half size On-Board 4 Axis controller.
        AXT_PCIN804                   = 0x85,             // PCI bus, Half size On-Board 8 Axis controller.
        AXT_PCIEN804                  = 0x86,             // PCIe bus, Half size On-Board 8 Axis controller.
        AXT_PCIEN404                                            = 0x87,    // PCIe bus, Half size On-Board 4 Axis controller.
        AXT_PCI_AIO1602HR             = 0x93,             // PCI bus, Half size, AI-16ch, AO-2ch AI16HR
        AXT_PCI_R1604                 = 0xC1,             // PCI bus[PCI9030], Half size, RTEX based 16 axis controller
        AXT_PCI_R3204                 = 0xC2,             // PCI bus[PCI9030], Half size, RTEX based 32 axis controller
        AXT_PCI_R32IO                 = 0xC3,             // PCI bus[PCI9030], Half size, RTEX based IO only.
        AXT_PCI_REV2                  = 0xC4,             // Reserved.                                                          
        AXT_PCI_R1604MLII             = 0xC5,             // PCI bus[PCI9030], Half size, Mechatrolink-II 16/32 axis controller.
	AXT_PCI_R0804MLII             = 0xC6,             // PCI bus[PCI9030], Half size, Mechatrolink-II 08 axis controller.
        AXT_PCI_Rxx00MLIII            = 0xC7,    // Master PCI Board, Mechatrolink III AXT, PCI Bus[PCI9030], Half size, Max.32 Slave module support
        AXT_PCIE_Rxx00MLIII           = 0xC8,    // Master PCI Express Board, Mechatrolink III AXT, PCI Bus[], Half size, Max.32 Slave module support
        AXT_PCP2_Rxx00MLIII           = 0xC9,    // Master PCI/104-Plus Board, Mechatrolink III AXT, PCI Bus[], Half size, Max.32 Slave module support
        AXT_PCI_R1604SIIIH            = 0xCA,    // PCI bus[PCI9030], Half size, SSCNET III / H 16/32 axis controller.
        AXT_PCI_R32IOEV               = 0xCB,    // PCI bus[PCI9030], Half size, RTEX based IO only. Economic version.
        AXT_PCIE_R0804RTEX            = 0xCC,    // PCIe bus, Half size, Half size, RTEX based 08 axis controller.
        AXT_PCIE_R1604RTEX            = 0xCD,    // PCIe bus, Half size, Half size, RTEX based 16 axis controller.
        AXT_PCIE_R2404RTEX            = 0xCE,    // PCIe bus, Half size, Half size, RTEX based 24 axis controller.
        AXT_PCIE_R3204RTEX            = 0xCF,    // PCIe bus, Half size, Half size, RTEX based 32 axis controller.
        AXT_PCIE_Rxx04SIIIH           = 0xD0,    // PCIe bus, Half size, SSCNET III / H 16(or 32)-axis(CAMC-QI based) controller.
        AXT_PCIE_Rxx00SIIIH           = 0xD1     // PCIe bus, Half size, SSCNET III / H Max. 32-axis(DSP Based) controller.
    } AXT_BASE_BOARD;
#endif

// Define Module
#ifndef __AXT_FUNC_MODULE__
    #define __AXT_FUNC_MODULE__
    typedef enum _AXT_FUNC_MODULE
    {
        AXT_SMC_2V01                  = 0x01,             // CAMC-5M, 2 Axis
        AXT_SMC_2V02                  = 0x02,             // CAMC-FS, 2 Axis
        AXT_SMC_1V01                  = 0x03,             // CAMC-5M, 1 Axis
        AXT_SMC_1V02                  = 0x04,             // CAMC-FS, 1 Axis
        AXT_SMC_2V03                  = 0x05,             // CAMC-IP, 2 Axis
        AXT_SMC_4V04                  = 0x06,             // CAMC-QI, 4 Axis
        AXT_SMC_R1V04A4               = 0x07,             // CAMC-QI, 1 Axis, For RTEX A4 slave only
        AXT_SMC_1V03                  = 0x08,             // CAMC-IP, 1 Axis
        AXT_SMC_R1V04                                           = 0x09,    // CAMC-QI, 1 Axis, For RTEX SLAVE only(Pulse Output Module)
        AXT_SMC_R1V04MLIISV           = 0x0A,             // CAMC-QI, 1 Axis, For Sigma-X series.                      
        AXT_SMC_R1V04MLIIPM           = 0x0B,             // 2 Axis, For Pulse output series(JEPMC-PL2910).            
        AXT_SMC_2V04                  = 0x0C,             // CAMC-QI, 2 Axis
        AXT_SMC_R1V04A5               = 0x0D,             // CAMC-QI, 1 Axis, For RTEX A5N slave only
        AXT_SMC_R1V04MLIICL           = 0x0E,             // CAMC-QI, 1 Axis, For MLII Convex Linear only
        AXT_SMC_R1V04MLIICR           = 0x0F,             // CAMC-QI, 1 Axis, For MLII Convex Rotary only
 	AXT_SMC_R1V04PM2Q             = 0x10,             // CAMC-QI, 2 Axis, For RTEX SLAVE only(Pulse Output Module)
	AXT_SMC_R1V04PM2QE            = 0x11,             // CAMC-QI, 4 Axis, For RTEX SLAVE only(Pulse Output Module)
        AXT_SMC_R1V04MLIIORI          = 0x12,             // CAMC-QI, 1 Axis, For MLII Oriental Step Driver only
        AXT_SMC_R1V04SIIIHMIV         = 0x14,             // CAMC-QI, 1 Axis, For SSCNETIII/H MRJ4
        AXT_SMC_R1V04SIIIHMIV_R       = 0x15,    // CAMC-QI, 1 Axis, For SSCNETIII/H MRJ4
        AXT_SMC_R1V04MLIIISV          = 0x20,    // DSP, 1 Axis, For ML-3 SigmaV/YASKAWA only 
        AXT_SMC_R1V04MLIIIPM          = 0x21,    // DSP, 1 Axis, For ML-3 SLAVE/AJINEXTEK only(Pulse Output Module)
        AXT_SMC_R1V04MLIIISV_MD       = 0x22,    // DSP, 1 Axis, For ML-3 SigmaV-MD/YASKAWA only (Multi-Axis Control amp)
        AXT_SMC_R1V04MLIIIS7S                                   = 0x23,    // DSP, 1 Axis, For ML-3 Sigma7S/YASKAWA only
        AXT_SMC_R1V04MLIIIS7W                                   = 0x24,    // DSP, 2 Axis, For ML-3 Sigma7W/YASKAWA only(Dual-Axis control amp)
        AXT_SIO_RDI32MLIII                                      = 0x70,    // DI slave module, MechatroLink III AXT, IN 32-Channel NPN
        AXT_SIO_RDO32MLIII                                      = 0x71,    // DO slave module, MechatroLink III AXT, OUT 32-Channel  NPN
        AXT_SIO_RDB32MLIII                                      = 0x72,    // DB slave module, MechatroLink III AXT, IN 16-Channel, OUT 16-Channel  NPN
        AXT_SIO_RDI32MSMLIII                                    = 0x73,    // DI slave module, MechatroLink III M-SYSTEM, IN 32-Channel
        AXT_SIO_RDO32AMSMLIII                                   = 0x74,    // DO slave module, MechatroLink III M-SYSTEM, OUT 32-Channel
        AXT_SIO_RDI32PMLIII                                     = 0x75,    // DI slave module, MechatroLink III AXT, IN 32-Channel PNP
        AXT_SIO_RDO32PMLIII                                     = 0x76,    // DO slave module, MechatroLink III AXT, OUT 32-Channel  PNP
        AXT_SIO_RDB32PMLIII                                     = 0x77,    // DB slave module, MechatroLink III AXT, IN 16-Channel, OUT 16-Channel  PNP
        AXT_SIO_UNDEFINEMLIII                                   = 0x79,    // IO slave module, MechatroLink III Any, Max. IN 480-Channel, Max. OUT 480-Channel
        AXT_SIO_RDB128MLIIIAI                                   = 0x7D,    // Intelligent I/O (Product by IAI), For MLII only
        AXT_SIO_RSIMPLEIOMLII         = 0x7E,             // Digital IN/Out xx��, Simple I/O sereies, MLII ����.
        AXT_SIO_RDI16MLII             = 0x80,             // DISCRETE INPUT MODULE, 16 points (Product by M-SYSTEM), For MLII only
        AXT_SIO_RDO16AMLII            = 0x81,             // NPN TRANSISTOR OUTPUT MODULE, 16 points (Product by M-SYSTEM), For MLII only
        AXT_SIO_RDO16BMLII            = 0x82,             // PNP TRANSISTOR OUTPUT MODULE, 16 points (Product by M-SYSTEM), For MLII only	
        AXT_SIO_RDB96MLII             = 0x83,             // Digital IN/OUT(Selectable), MAX 96 points, For MLII only
        AXT_SIO_RDO32RTEX             = 0x84,             // Digital OUT  32 point
        AXT_SIO_RDI32RTEX             = 0x85,             // Digital IN  32 point
        AXT_SIO_RDB32RTEX             = 0x86,             // Digital IN/OUT  32 point
        AXT_SIO_DI32_P                = 0x92,             // Digital IN  32 point, PNP type(source type)
        AXT_SIO_DO32T_P               = 0x93,             // Digital OUT 32 point, Power TR, PNT type(source type)
        AXT_SIO_RDB128MLII            = 0x94,             // Digital IN 64 point / OUT 64 point, For MLII Only (JEPMC-IO2310)     
        AXT_SIO_RDI32                 = 0x95,             // Digital IN  32 point, For RTEX only
        AXT_SIO_RDO32                 = 0x96,             // Digital OUT 32 point, For RTEX only
        AXT_SIO_DI32                  = 0x97,             // Digital IN  32 point
        AXT_SIO_DO32P                 = 0x98,             // Digital OUT 32 point
        AXT_SIO_DB32P                 = 0x99,             // Digital IN 16 point / OUT 16 point
        AXT_SIO_RDB32T                = 0x9A,             // Digital IN 16 point / OUT 16 point, For RTEX only
        AXT_SIO_DO32T                 = 0x9E,             // Digital OUT 16 point, Power TR output
        AXT_SIO_DB32T                 = 0x9F,             // Digital IN 16 point / OUT 16 point, Power TR output
        AXT_SIO_RAI16RB               = 0xA0,             // A0h(160) : AI 16Ch, 16 bit, For RTEX only
        AXT_SIO_AI4RB                 = 0xA1,             // A1h(161) : AI 4Ch, 12 bit
        AXT_SIO_AO4RB                 = 0xA2,             // A2h(162) : AO 4Ch, 12 bit
        AXT_SIO_AI16H                 = 0xA3,             // A3h(163) : AI 4Ch, 16 bit
        AXT_SIO_AO8H                  = 0xA4,             // A4h(164) : AO 4Ch, 16 bit
        AXT_SIO_AI16HB                = 0xA5,             // A5h(165) : AI 16Ch, 16 bit (SIO-AI16HR(input module))
        AXT_SIO_AO2HB                 = 0xA6,             // A6h(166) : AO 2Ch, 16 bit  (SIO-AI16HR(output module))
        AXT_SIO_RAI8RB                = 0xA7,             // A7h(167) : AI 8Ch, 16 bit, For RTEX only
        AXT_SIO_RAO4RB                = 0xA8,             // A8h(168) : AO 4Ch, 16 bit, For RTEX only
        AXT_SIO_RAI4MLII              = 0xA9,             // A9h(169) : AI 4Ch, 16 bit, For MLII only
        AXT_SIO_RAO2MLII              = 0xAA,             // AAh(170) : AO 2Ch, 16 bit, For MLII only
        AXT_SIO_RAVCI4MLII            = 0xAB,             // DC VOLTAGE/CURRENT INPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
        AXT_SIO_RAVO2MLII             = 0xAC,             // DC VOLTAGE OUTPUT MODULE, 2 points (Product by M-SYSTEM), For MLII only
        AXT_SIO_RACO2MLII             = 0xAD,             // DC CURRENT OUTPUT MODULE, 2 points (Product by M-SYSTEM), For MLII only
        AXT_SIO_RATI4MLII             = 0xAE,             // THERMOCOUPLE INPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
        AXT_SIO_RARTDI4MLII           = 0xAF,             // RTD INPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
        AXT_SIO_RCNT2MLII                                       = 0xB0,    // Counter slave module, Reversible counter, 2 channels (Product by YASKWA), For MLII only
	AXT_SIO_CN2CH                 = 0xB1,             // Counter Module, 2 channels, Remapped ID, Actual ID is (0xA8)
	AXT_SIO_RCNT2RTEX             = 0xB2,             // Counter slave module, Reversible counter, 2 channels, For RTEX Only
        AXT_SIO_RCNT2MLIII                                      = 0xB3,    // Counter slave moudle, MechatroLink III AXT, 2 ch, Trigger per channel
        AXT_SIO_RHPC4MLIII                                      = 0xB4,    // Counter slave moudle, MechatroLink III AXT, 4 ch
        AXT_SIO_RAI16RTEX             = 0xC0,             // ANALOG VOLTAGE INPUT(+- 10V) 16 Channel RTEX 
        AXT_SIO_RAO08RTEX             = 0xC1,             // ANALOG VOLTAGE OUTPUT(+- 10V) 08 Channel RTEX
        AXT_SIO_RAI8MLIII                                       = 0xC2,    // AI slave module, MechatroLink III AXT, Analog IN 8ch, 16 bit
        AXT_SIO_RAI16MLIII                                      = 0xC3,    // AI slave module, MechatroLink III AXT, Analog IN 16ch, 16 bit
        AXT_SIO_RAO4MLIII                                       = 0xC4,    // A0 slave module, MechatroLink III AXT, Analog OUT 4ch, 16 bit
        AXT_SIO_RAO8MLIII                                       = 0xC5,    // A0 slave module, MechatroLink III AXT, Analog OUT 8ch, 16 bit
        AXT_SIO_RAVO4MLII                                       = 0xC6,    // DC VOLTAGE OUTPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
        AXT_COM_234R                  = 0xD3,             // COM-234R                              
        AXT_COM_484R                  = 0xD4,             // COM-484R
        AXT_COM_234IDS                                          = 0xD5,    // COM-234IDS
        AXT_COM_484IDS                                          = 0xD6,    // COM-484IDS
        AXT_SIO_AO4F                                            = 0xD7,    // AO 4Ch, 16 bit
        AXT_SIO_AI8F                                            = 0xD8,    // AI 8Ch, 16 bit
        AXT_SIO_AI8AO4F                                         = 0xD9,    // AI 8Ch, AO 4Ch, 16 bit
        AXT_SIO_HPC4                                            = 0xDA     // External Encoder module for 4Channel with Trigger function.
    } AXT_FUNC_MODULE;
#endif

#ifndef __AXT_FUNC_RESULT__
    #define __AXT_FUNC_RESULT__
    typedef enum _AXT_FUNC_RESULT
    {
        AXT_RT_SUCCESS                      = 0,          // API function executed successfully
        AXT_RT_OPEN_ERROR                   = 1001,       // Library is not open
        AXT_RT_OPEN_ALREADY                 = 1002,       // Library is already open
        AXT_RT_NOT_OPEN                     = 1053,       // Failed to initialize the library
        AXT_RT_NOT_SUPPORT_VERSION          = 1054,       // Hardware not supported
        AXT_RT_LOCK_FILE_MISMATCH           = 1055,       // Lock file mismatch
        
        AXT_RT_INVALID_BOARD_NO             = 1101,       // IInvalide board number  
        AXT_RT_INVALID_MODULE_POS           = 1102,       // Invalid module position
        AXT_RT_INVALID_LEVEL                = 1103,       // Invalid level
        AXT_RT_INVALID_VARIABLE             = 1104,       // Invalid Variable
        AXT_RT_ERROR_VERSION_READ           = 1151,       // Failed to read library version
        AXT_RT_NETWORK_ERROR                = 1152,       // Hardware network error
        AXT_RT_NETWORK_LOCK_MISMATCH        = 1153,       //

        AXT_RT_1ST_BELOW_MIN_VALUE          = 1160,       // First parameter is below the minimum value
        AXT_RT_1ST_ABOVE_MAX_VALUE          = 1161,       // First parameter is above the maximum value
        AXT_RT_2ND_BELOW_MIN_VALUE          = 1170,       // Second parameter is below the minimum value
        AXT_RT_2ND_ABOVE_MAX_VALUE          = 1171,       // Second parameter is above the maximum value
        AXT_RT_3RD_BELOW_MIN_VALUE          = 1180,       // Third parameter is below the minimum value
        AXT_RT_3RD_ABOVE_MAX_VALUE          = 1181,       // Third parameter is above the maximum value
        AXT_RT_4TH_BELOW_MIN_VALUE          = 1190,       // Fourth parameter is below the minimum value
        AXT_RT_4TH_ABOVE_MAX_VALUE          = 1191,       // Fourth parameter is above the maximum value
        AXT_RT_5TH_BELOW_MIN_VALUE          = 1200,       // Fifth parameter is below the minimum value
        AXT_RT_5TH_ABOVE_MAX_VALUE          = 1201,       // Fifth parameter is above the maximum value 
        AXT_RT_6TH_BELOW_MIN_VALUE          = 1210,       // Sixth parameter is below the minimum value 
        AXT_RT_6TH_ABOVE_MAX_VALUE          = 1211,       // Sixth parameter is above the maximum value
        AXT_RT_7TH_BELOW_MIN_VALUE          = 1220,       // Seventh parameter is below the minimum value
        AXT_RT_7TH_ABOVE_MAX_VALUE          = 1221,       // Seventh parameter is above the maximum value
        AXT_RT_8TH_BELOW_MIN_VALUE          = 1230,       // Eighth parameter is below the minimum value 
        AXT_RT_8TH_ABOVE_MAX_VALUE          = 1231,       // Eighth parameter is above the maximum value
        AXT_RT_9TH_BELOW_MIN_VALUE          = 1240,       // Ninth parameter is below the minimum value
        AXT_RT_9TH_ABOVE_MAX_VALUE          = 1241,       // Ninth parameter is above the maximum value
        AXT_RT_10TH_BELOW_MIN_VALUE         = 1250,       // Tenth parameter is above the minimum value
        AXT_RT_10TH_ABOVE_MAX_VALUE         = 1251,       // Tenth parameter is above the maximum value

        AXT_RT_AIO_OPEN_ERROR               = 2001,       // Filed to open AIO module
        AXT_RT_AIO_NOT_MODULE               = 2051,       // No AIO module
        AXT_RT_AIO_NOT_EVENT                = 2052,       // Failed to read AIO event 
        AXT_RT_AIO_INVALID_MODULE_NO        = 2101,       // Invalid AIO module
        AXT_RT_AIO_INVALID_CHANNEL_NO       = 2102,       // Invalid AIO channel number
        AXT_RT_AIO_INVALID_USE              = 2106,       // Can not use AIO
        AXT_RT_AIO_INVALID_TRIGGER_MODE     = 2107,       // Invalid trigger mode
        AXT_RT_AIO_EXTERNAL_DATA_EMPTY      = 2108,
        AXT_RT_AIO_INVALID_VALUE            = 2109,        // Invalid Value Setting

        AXT_RT_DIO_OPEN_ERROR               = 3001,       // Failed to open DIO module
        AXT_RT_DIO_NOT_MODULE               = 3051,       // No DIO module
        AXT_RT_DIO_NOT_INTERRUPT            = 3052,       // DIO Interrupt not set
        AXT_RT_DIO_INVALID_MODULE_NO        = 3101,       // Invalid DIO module number
        AXT_RT_DIO_INVALID_OFFSET_NO        = 3102,       // Invalid DIO OFFSET number
        AXT_RT_DIO_INVALID_LEVEL            = 3103,       // Invalid DIO level
        AXT_RT_DIO_INVALID_MODE             = 3104,       // Invalid DIO mode
        AXT_RT_DIO_INVALID_VALUE            = 3105,       // Invalid setting value
        AXT_RT_DIO_INVALID_USE              = 3106,       // Can not use DIO API function

        AXT_RT_CNT_OPEN_ERROR               = 3201,       // Fail to open CND module
        AXT_RT_CNT_NOT_MODULE               = 3251,       // No CNT module
        AXT_RT_CNT_NOT_INTERRUPT            = 3252,       // CNT Interrupt not set
        AXT_RT_CNT_INVALID_MODULE_NO        = 3301,       // Invalid CNT module number
        AXT_RT_CNT_INVALID_CHANNEL_NO       = 3302,       // Invalid CNT Channel number
        AXT_RT_CNT_INVALID_OFFSET_NO        = 3303,       // Invalid CNT OFFSET number
        AXT_RT_CNT_INVALID_LEVEL            = 3304,       // Invalid CNT level
        AXT_RT_CNT_INVALID_MODE             = 3305,       // Invalid CNT mode
        AXT_RT_CNT_INVALID_VALUE            = 3306,       // Invalid setting value
        AXT_RT_CNT_INVALID_USE              = 3307,       // Can not use CNT API function

        AXT_RT_MOTION_OPEN_ERROR            = 4001,       // Failed to open motion library
        AXT_RT_MOTION_NOT_MODULE            = 4051,       // No motion module is installed in the system
        AXT_RT_MOTION_NOT_INTERRUPT         = 4052,       // Failed to read the result of interrupt
        AXT_RT_MOTION_NOT_INITIAL_AXIS_NO   = 4053,       // Failed to initialize the motion of corresponding axis
        AXT_RT_MOTION_NOT_IN_CONT_INTERPOL  = 4054,       // Command to stop continuous interpolation while not in continuous interpolation motion
        AXT_RT_MOTION_NOT_PARA_READ         = 4055,       // Failed to load parameters for home return drive
        AXT_RT_MOTION_INVALID_AXIS_NO       = 4101,       // Corresponding axis does not exist
        AXT_RT_MOTION_INVALID_METHOD        = 4102,       // Invalid setting for corresponding axis drive
        AXT_RT_MOTION_INVALID_USE           = 4103,       // Invalid setting for 'uUse' parameter
        AXT_RT_MOTION_INVALID_LEVEL         = 4104,       // Invalid setting for 'uLevel' parameter
        AXT_RT_MOTION_INVALID_BIT_NO        = 4105,       // Invalid setting for general purpose input/output bit
        AXT_RT_MOTION_INVALID_STOP_MODE     = 4106,       // Invalid setting values for motion stop mode
        AXT_RT_MOTION_INVALID_TRIGGER_MODE  = 4107,       // Invalid setting for trigger setting mode
        AXT_RT_MOTION_INVALID_TRIGGER_LEVEL = 4108,       // Invalid setting for trigger output level
        AXT_RT_MOTION_INVALID_SELECTION     = 4109,       // 'uSelection' parameter is set to a value other than COMMAND or ACTUAL
        AXT_RT_MOTION_INVALID_TIME          = 4110,       // Invalid setting for Trigger output time value
        AXT_RT_MOTION_INVALID_FILE_LOAD     = 4111,       // Failed to load the file containing motion setting values
        AXT_RT_MOTION_INVALID_FILE_SAVE     = 4112,       // Failed to save the file containing motion setting values
        AXT_RT_MOTION_INVALID_VELOCITY      = 4113,       // Motion error occurred since the motion drive velocity value is set as zero
        AXT_RT_MOTION_INVALID_ACCELTIME     = 4114,       // Motion error occurred since motion drive acceleration time value is set as zero
        AXT_RT_MOTION_INVALID_PULSE_VALUE   = 4115,       // Input pulse value is set to a value less than zero when setting motion unit
        AXT_RT_MOTION_INVALID_NODE_NUMBER   = 4116,       // Called position or velocity override function while not in motion
        AXT_RT_MOTION_INVALID_TARGET        = 4117,       // Flag indicating the cause of multi axis motion stop
        
        AXT_RT_MOTION_ERROR_IN_NONMOTION    = 4151,       // Not in motion drive when it should be
        AXT_RT_MOTION_ERROR_IN_MOTION       = 4152,       // Called another motion drive function before the completion of current motion
        AXT_RT_MOTION_ERROR                 = 4153,       // Error occurred during the execution of multi axis motion stop function
        AXT_RT_MOTION_ERROR_GANTRY_ENABLE   = 4154,       // Gantry-enable is activated again while in motion with gantry enabled
        AXT_RT_MOTION_ERROR_GANTRY_AXIS     = 4155,       // Invalid input of gantry axis master channel (axis) number (starting from zero)
        AXT_RT_MOTION_ERROR_MASTER_SERVOON  = 4156,       // Servo ON is not enabled for the master axis
        AXT_RT_MOTION_ERROR_SLAVE_SERVOON   = 4157,       // Servo ON is not enabled for the slave axis
        AXT_RT_MOTION_INVALID_POSITION      = 4158,       // Not in a valid position
        AXT_RT_ERROR_NOT_SAME_MODULE        = 4159,       // Not in the same module
        AXT_RT_ERROR_NOT_SAME_BOARD         = 4160,       // Not in the same board
        AXT_RT_ERROR_NOT_SAME_PRODUCT       = 4161,       // When products are different each other
        AXT_RT_NOT_CAPTURED                 = 4162,       // Failed to capture the position
        AXT_RT_ERROR_NOT_SAME_IC            = 4163,       // Not in the same chip
        AXT_RT_ERROR_NOT_GEARMODE           = 4164,       // Failed to change to the gear mode
        AXT_ERROR_CONTI_INVALID_AXIS_NO     = 4165,       // Invalid axis during continuous interpolation axies mapping
        AXT_ERROR_CONTI_INVALID_MAP_NO      = 4166,       // Invalid mapping number during continuous interpolation mapping
        AXT_ERROR_CONTI_EMPTY_MAP_NO        = 4167,       // Continuous interpolation mapping number is empty
        AXT_RT_MOTION_ERROR_CACULATION      = 4168,       // Error in calculation
        AXT_RT_ERROR_MOVE_SENSOR_CHECK      = 4169,       // 

        AXT_ERROR_HELICAL_INVALID_AXIS_NO   = 4170,       // Invalid axis for helical axis mapping
        AXT_ERROR_HELICAL_INVALID_MAP_NO    = 4171,       // Invalid mapping number for helical mapping
        AXT_ERROR_HELICAL_EMPTY_MAP_NO      = 4172,       // Helical mapping number is empty

        AXT_ERROR_SPLINE_INVALID_AXIS_NO    = 4180,       // Invalid axis for spline axis mapping
        AXT_ERROR_SPLINE_INVALID_MAP_NO     = 4181,       // Invalid mapping number for spline mapping
        AXT_ERROR_SPLINE_EMPTY_MAP_NO       = 4182,       // Spline Mapping number is empty
        AXT_ERROR_SPLINE_NUM_ERROR          = 4183,       // Spline point value is inapplicable
        AXT_RT_MOTION_INTERPOL_VALUE        = 4184,       // Invalid input value for interpolation
        AXT_RT_ERROR_NOT_CONTIBEGIN         = 4185,       // CONTIBEGIN function is not called in continuous interpolation
        AXT_RT_ERROR_NOT_CONTIEND           = 4186,       // CONTIEND function is not called in continuous interpolation

        AXT_RT_MOTION_HOME_SEARCHING        = 4201,       // Other function is called while in home search motion
        AXT_RT_MOTION_HOME_ERROR_SEARCHING  = 4202,       // Home search motion is forced to stop by the user or something from external
        AXT_RT_MOTION_HOME_ERROR_START      = 4203,       // Failed to start home search drive due to Initialization  problem
        AXT_RT_MOTION_HOME_ERROR_GANTRY     = 4204,       // Failed to execute Gantry enable during home search motion

        AXT_RT_MOTION_READ_ALARM_WAITING    = 4210,       // Waiting for alarm code from servo driver.
        AXT_RT_MOTION_READ_ALARM_NO_REQUEST = 4211,       // Alarm request is not executed.
        AXT_RT_MOTION_READ_ALARM_TIMEOUT    = 4212,       // Time out during reading servo alarm.(exceed 1 Sec)
        AXT_RT_MOTION_READ_ALARM_FAILED     = 4213,       // Fail to read servo alarm.
        AXT_RT_MOTION_READ_ALARM_UNKNOWN    = 4220,       // Unknown alarm code.
        AXT_RT_MOTION_READ_ALARM_FILES      = 4221,       // No alarm information file.
        
        AXT_RT_MOTION_POSITION_OUTOFBOUND   = 4251,       // Configured position is either above the maximum value or below the minimum value
        AXT_RT_MOTION_PROFILE_INVALID       = 4252,       // Invalid setting for velocity profile
        AXT_RT_MOTION_VELOCITY_OUTOFBOUND   = 4253,       // Configured velocity is either above the maximum value or below the minimum value
        AXT_RT_MOTION_MOVE_UNIT_IS_ZERO     = 4254,       // Unit of the motion values is set as zero
        AXT_RT_MOTION_SETTING_ERROR         = 4255,       // Invalid setting for Velocity, Acceleration, Jerk, or Profile
        AXT_RT_MOTION_IN_CONT_INTERPOL      = 4256,       // Execution of motion start or restart function before the completion of current continuous interpolated motion
        AXT_RT_MOTION_DISABLE_TRIGGER       = 4257,       // Trigger output is disabled
        AXT_RT_MOTION_INVALID_CONT_INDEX    = 4258,       // Invalid setting for continuous interpolation index value
        AXT_RT_MOTION_CONT_QUEUE_FULL       = 4259,       // Continuous Interpolation queue of motion chip is full
        AXT_RT_PROTECTED_DURING_SERVOON     = 4260,       // Protected during servo-on
        AXT_RT_HW_ACCESS_ERROR              = 4261,       // Failed memory Read / Write

        AXT_RT_HW_DPRAM_CMD_WRITE_ERROR_LV1 = 4262,       // DPRAM Command Write fail Level1
        AXT_RT_HW_DPRAM_CMD_WRITE_ERROR_LV2 = 4263,       // DPRAM Command Write fail Level2
        AXT_RT_HW_DPRAM_CMD_WRITE_ERROR_LV3 = 4264,       // DPRAM Command Write fail Level3
        AXT_RT_HW_DPRAM_CMD_READ_ERROR_LV1  = 4265,       // DPRAM Command Read fail Level1
        AXT_RT_HW_DPRAM_CMD_READ_ERROR_LV2  = 4266,       // DPRAM Command Read fail Level2
        AXT_RT_HW_DPRAM_CMD_READ_ERROR_LV3  = 4267,       // DPRAM Command Read fail Level3

        AXT_RT_COMPENSATION_SET_PARAM_FIRST = 4300,

        AXT_RT_SEQ_NOT_IN_SERVICE           = 4400,
        AXT_ERROR_SEQ_INVALID_MAP_NO        = 4401,
        AXT_ERROR_INVALID_AXIS_NO           = 4402,
        AXT_RT_ERROR_NOT_SEQ_NODE_BEGIN     = 4403,
        AXT_RT_ERROR_NOT_SEQ_NODE_END       = 4404,
        AXT_RT_ERROR_NO_NODE                = 4405,
	AXT_RT_ERROR_SEQ_STOP_TIMEOUT       = 4406,

        AXT_RT_M3_COMMUNICATION_FAILED                          = 4500,    // ML3 Only
        AXT_RT_MOTION_ONE_OF_AXES_IS_NOT_M3                     = 4501,    // ML3 Only
        AXT_RT_MOTION_BIGGER_VEL_THEN_MAX_VEL                   = 4502,    // ML3 Only
        AXT_RT_MOTION_SMALLER_VEL_THEN_MAX_VEL                  = 4503,    // ML3 Only
        AXT_RT_MOTION_ACCEL_MUST_BIGGER_THEN_ZERO               = 4504,    // ML3 Only
        AXT_RT_MOTION_SMALL_ACCEL_WITH_UNIT_PULSE               = 4505,    // ML3 Only
        AXT_RT_MOTION_INVALID_INPUT_ACCEL                       = 4506,    // ML3 Only
        AXT_RT_MOTION_SMALL_DECEL_WITH_UNIT_PULSE               = 4507,    // ML3 Only
        AXT_RT_MOTION_INVALID_INPUT_DECEL                       = 4508,    // ML3 Only
        AXT_RT_MOTION_SAME_START_AND_CENTER_POS                 = 4509,    // ML3 Only
        AXT_RT_MOTION_INVALID_JERK                              = 4510,    // ML3 Only
        AXT_RT_MOTION_INVALID_INPUT_VALUE                       = 4511,    // ML3 Only
        AXT_RT_MOTION_NOT_SUPPORT_PROFILE                       = 4512,    // ML3 Only
        AXT_RT_MOTION_INPOS_UNUSED                              = 4513,    // ML3 Only
        AXT_RT_MOTION_AXIS_IN_SLAVE_STATE                       = 4514,    // ML3 Only
        AXT_RT_MOTION_AXES_ARE_NOT_SAME_BOARD                   = 4515,    // ML3 Only
        AXT_RT_MOTION_ERROR_IN_ALARM                            = 4516,    // ML3 Only
        AXT_RT_MOTION_ERROR_IN_EMGN                             = 4517,    // ML3 Only
        AXT_RT_MOTION_CAN_NOT_CHANGE_COORD_NO                   = 4518,    // ML3 Only
        AXT_RT_MOTION_INVALID_INTERNAL_RADIOUS                  = 4519,    // ML3 Only
        AXT_RT_MOTION_CONTI_QUEUE_FULL                          = 4521,    // ML3 Only
        AXT_RT_MOTION_SAME_START_AND_END_POSITION               = 4522,    // ML3 Only
        AXT_RT_MOTION_INVALID_ANGLE                             = 4523,    // ML3 Only
        AXT_RT_MOTION_CONTI_QUEUE_EMPTY                         = 4524,    // ML3 Only
        AXT_RT_MOTION_ERROR_GEAR_ENABLE                         = 4525,    // ML3 Only
        AXT_RT_MOTION_ERROR_GEAR_AXIS                           = 4526,    // ML3 Only
        AXT_RT_MOTION_ERROR_NO_GANTRY_ENABLE                    = 4527,    // ML3 Only
        AXT_RT_MOTION_ERROR_NO_GEAR_ENABLE                      = 4528,    // ML3 Only
        AXT_RT_MOTION_ERROR_GANTRY_ENABLE_FULL                  = 4529,    // ML3 Only
        AXT_RT_MOTION_ERROR_GEAR_ENABLE_FULL                    = 4530,    // ML3 Only
        AXT_RT_MOTION_ERROR_NO_GANTRY_SLAVE                     = 4531,    // ML3 Only
        AXT_RT_MOTION_ERROR_NO_GEAR_SLAVE                       = 4532,    // ML3 Only
        AXT_RT_MOTION_ERROR_MASTER_SLAVE_SAME                   = 4533,    // ML3 Only
        AXT_RT_MOTION_NOT_SUPPORT_HOMESIGNAL                    = 4534,    // ML3 Only
        AXT_RT_MOTION_ERROR_NOT_SYNC_CONNECT                    = 4535,    // ML3 Only
        AXT_RT_MOTION_OVERFLOW_POSITION                         = 4536,    // ML3 Only
        AXT_RT_MOTION_ERROR_INVALID_CONTIMAPAXIS                = 4537,    // ML3 Only
        AXT_RT_MOTION_ERROR_INVALID_CONTIMAPSIZE                = 4538,    // ML3 Only
        AXT_RT_MOTION_ERROR_IN_SERVO_OFF                        = 4539,    // ML3 Only
        AXT_RT_MOTION_ERROR_POSITIVE_LIMIT                      = 4540,    // ML3 Only
        AXT_RT_MOTION_ERROR_NEGATIVE_LIMIT                      = 4541,    // ML3 Only
        AXT_RT_MOTION_ERROR_OVERFLOW_SWPROFILE_NUM              = 4542,    // ML3 Only
        AXT_RT_PROTECTED_DURING_INMOTION                        = 4543,    // ML3 Only

        AXT_RT_DATA_FLASH_NOT_EXIST         = 5000,
        AXT_RT_DATA_FLASH_BUSY              = 5001
    } AXT_FUNC_RESULT;
#endif

#ifndef __AFX_H__
    #define HWND                                HANDLE
#endif

#ifndef FALSE
#define FALSE                                   0
#define TRUE                                    1
#endif

#ifndef WM_USER
    #define WM_USER                             0x0400
#endif

#ifndef WM_AXL_INTERRUPT
    #define WM_AXL_INTERRUPT                    (WM_USER + 1001)
#endif

#ifndef __AXT_LOG_LEVEL__
    #define __AXT_LOG_LEVEL__
    typedef enum _AXT_LOG_LEVEL
    {
        LEVEL_NONE,
        LEVEL_ERROR,
        LEVEL_RUNSTOP,
        LEVEL_FUNCTION
    } AXT_LOG_LEVEL;
#endif

#ifndef __AXT_EXISTENCE__
    #define __AXT_EXISTENCE__
    typedef enum _AXT_EXISTENCE
    {
        STATUS_NOTEXIST,
        STATUS_EXIST
    } AXT_EXISTENCE;
#endif
 
#ifndef __AXT_USE__
    #define __AXT_USE__
    typedef enum _AXT_USE
    {
        DISABLE,
        ENABLE
    } AXT_USE;
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef __AXT_AIO_TRIGGER_MODE__
    #define __AXT_AIO_TRIGGER_MODE__
    typedef enum _AXT_AIO_TRIGGER_MODE
    {
        DISABLE_MODE                            = 0,
        NORMAL_MODE                             = 1,
        TIMER_MODE, 
        EXTERNAL_MODE
    } AXT_AIO_TRIGGER_MODE;
#endif

#ifndef __AXT_AIO_FULL_MODE__
    #define __AXT_AIO_FULL_MODE__
    typedef enum _AXT_AIO_FULL_MODE
    {
        NEW_DATA_KEEP,
        CURR_DATA_KEEP
    } AXT_AIO_FULL_MODE;
#endif

#ifndef __AXT_AIO_EVENT_MASK__
    #define __AXT_AIO_EVENT_MASK__
    typedef enum _AXT_AIO_EVENT_MASK
    {
        DATA_EMPTY                              = 0x01,
        DATA_MANY                               = 0x02,
        DATA_SMALL                              = 0x04,
        DATA_FULL                               = 0x08
    } AXT_AIO_EVENT_MASK;
#endif

#ifndef __AXT_AIO_INTERRUPT_MASK__
    #define __AXT_AIO_INTERRUPT_MASK__
    typedef enum _AXT_AIO_INTERRUPT_MASK
    {
        ADC_DONE                                = 0x00,
        SCAN_END                                = 0x01,
        FIFO_HALF_FULL                          = 0x02,
        NO_SIGNAL                               = 0x03
    } AXT_AIO_INTERRUPT_MASK;
#endif
#ifndef __AXT_AIO_QUEUE_EVENT__
#define __AXT_AIO_QUEUE_EVENT__ 
typedef enum  _AXT_AIO_EVENT_MODE  
    { 
        AIO_EVENT_DATA_RESET                    = 0x00, 
        AIO_EVENT_DATA_UPPER, 
        AIO_EVENT_DATA_LOWER, 
        AIO_EVENT_DATA_FULL, 
        AIO_EVENT_DATA_EMPTY 
} AXT_AIO_EVENT_MODE;
#endif

#ifndef AXT_AIO_FIFO_STATUS_DEF
#define AXT_AIO_FIFO_STATUS_DEF
    typedef enum _AXT_AIO_FIFO_STATUS
    {
        FIFO_DATA_EXIST         = 0,
        FIFO_DATA_EMPTY         = 1,
        FIFO_DATA_HALF          = 2,
        FIFO_DATA_FULL          = 6
    } AXT_AIO_FIFO_STATUS;
#endif
	
#ifndef AXT_AIO_EXTERNAL_STATUS_DEF
#define AXT_AIO_EXTERNAL_STATUS_DEF
    typedef enum _AXT_AIO_EXTERNAL_STATUS
    {
        EXTERNAL_DATA_DONE      = 0,
        EXTERNAL_DATA_FINE      = 1,
        EXTERNAL_DATA_HALF      = 2,
        EXTERNAL_DATA_FULL      = 3,
        EXTERNAL_COMPLETE       = 4
    } AXT_AIO_EXTERNAL_STATUS;
#endif
    
//////////////////////////////////////////////////////////////////////////
#ifndef __AXT_DIO_EDGE__
    #define __AXT_DIO_EDGE__
    typedef enum _AXT_DIO_EDGE
    {
        DOWN_EDGE,
        UP_EDGE
    } AXT_DIO_EDGE;
#endif

#ifndef __AXT_DIO_STATE__
    #define __AXT_DIO_STATE__
    typedef enum _AXT_DIO_STATE
    {
        OFF_STATE,
        ON_STATE
    } AXT_DIO_STATE;
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef AXT_MOTION_STOPMODE_DEF
    #define AXT_MOTION_STOPMODE_DEF
    typedef enum _AXT_MOTION_STOPMODE
    {
        EMERGENCY_STOP,
        SLOWDOWN_STOP
    } AXT_MOTION_STOPMODE;
#endif

#ifndef AXT_MOTION_EDGE_DEF
    #define AXT_MOTION_EDGE_DEF
    typedef enum _AXT_MOTION_EDGE
    {
        SIGNAL_DOWN_EDGE,
        SIGNAL_UP_EDGE,
        SIGNAL_LOW_LEVEL,
        SIGNAL_HIGH_LEVEL
    } AXT_MOTION_EDGE;
#endif

#ifndef AXT_MOTION_SELECTION_DEF
    #define AXT_MOTION_SELECTION_DEF
    typedef enum _AXT_MOTION_SELECTION
    {
        COMMAND,
        ACTUAL
    } AXT_MOTION_SELECTION;
#endif

#ifndef AXT_MOTION_TRIGGER_MODE_DEF
    #define AXT_MOTION_TRIGGER_MODE_DEF
    typedef enum _AXT_MOTION_TRIGGER_MODE
    {
        PERIOD_MODE,
        ABS_POS_MODE
    } AXT_MOTION_TRIGGER_MODE;
#endif

#ifndef AXT_MOTION_LEVEL_MODE_DEF
    #define AXT_MOTION_LEVEL_MODE_DEF
    typedef enum _AXT_MOTION_LEVEL_MODE
    {
        LOW,
        HIGH,
        UNUSED,
        USED
    } AXT_MOTION_LEVEL_MODE;
#endif

#ifndef AXT_MOTION_ABSREL_MODE_DEF
    #define AXT_MOTION_ABSREL_MODE_DEF
    typedef enum _AXT_MOTION_ABSREL_MODE
    {
        POS_ABS_MODE,
        POS_REL_MODE,
        POS_ABS_LONG_MODE
    } AXT_MOTION_ABSREL_MODE;
#endif

#ifndef AXT_MOTION_PROFILE_MODE_DEF
    #define AXT_MOTION_PROFILE_MODE_DEF
    typedef enum _AXT_MOTION_PROFILE_MODE
    {
        SYM_TRAPEZOIDE_MODE,
        ASYM_TRAPEZOIDE_MODE,
        QUASI_S_CURVE_MODE,
        SYM_S_CURVE_MODE,
        ASYM_S_CURVE_MODE,
        SYM_TRAP_M3_SW_MODE,     //ML-3 Only, Support Velocity profile
        ASYM_TRAP_M3_SW_MODE,    //ML-3 Only, Support Velocity profile
        SYM_S_M3_SW_MODE,        //ML-3 Only, Support Velocity profile
        ASYM_S_M3_SW_MODE        //ML-3 Only, Support Velocity profile
    } AXT_MOTION_PROFILE_MODE;
#endif

#ifndef AXT_MOTION_SIGNAL_LEVEL_DEF
    #define AXT_MOTION_SIGNAL_LEVEL_DEF
    typedef enum _AXT_MOTION_SIGNAL_LEVEL
    {
        INACTIVE,
        ACTIVE
    } AXT_MOTION_SIGNAL_LEVEL;
#endif

#ifndef AXT_MOTION_HOME_RESULT_DEF
    #define AXT_MOTION_HOME_RESULT_DEF
    typedef enum AXT_MOTION_HOME_RESULT
    {
    HOME_RESERVED                                           = 0x00,
    HOME_SUCCESS                                            = 0x01,
    HOME_SEARCHING                                          = 0x02,
    HOME_ERR_GNT_RANGE                                      = 0x10,
    HOME_ERR_USER_BREAK                                     = 0x11,
    HOME_ERR_VELOCITY                                       = 0x12,
    HOME_ERR_AMP_FAULT                                      = 0x13,
    HOME_ERR_NEG_LIMIT                                      = 0x14,
    HOME_ERR_POS_LIMIT                                      = 0x15,
    HOME_ERR_NOT_DETECT                                     = 0x16,
    HOME_ERR_SETTING                                        = 0x17,
    HOME_ERR_TIMEOUT                                        = 0x20,
    HOME_ERR_FUNCALL                                        = 0x30,
    HOME_ERR_COUPLING                                       = 0x40,
    HOME_ERR_UNKNOWN                                        = 0xFF
    } AXT_MOTION_HOME_RESULT;
#endif

#ifndef AXT_MOTION_UNIV_INPUT_DEF
    #define AXT_MOTION_UNIV_INPUT_DEF
    typedef enum _AXT_MOTION_UNIV_INPUT
    {
        UIO_INP0,
        UIO_INP1,
        UIO_INP2,
        UIO_INP3,
        UIO_INP4,
        UIO_INP5
    } AXT_MOTION_UNIV_INPUT;
#endif

#ifndef AXT_MOTION_UNIV_OUTPUT_DEF
    #define AXT_MOTION_UNIV_OUTPUT_DEF
    typedef enum _AXT_MOTION_UNIV_OUTPUT
    {
        UIO_OUT0,
        UIO_OUT1,
        UIO_OUT2,
        UIO_OUT3,
        UIO_OUT4,
        UIO_OUT5
    } AXT_MOTION_UNIV_OUTPUT;
#endif


#ifndef AXT_MOTION_DETECT_DOWN_START_POINT_DEF
    #define AXT_MOTION_DETECT_DOWN_START_POINT_DEF
    typedef enum _AXT_MOTION_DETECT_DOWN_START_POINT
    {
        AutoDetect,
        RestPulse
    } AXT_MOTION_DETECT_DOWN_START_POINT;
#endif

#ifndef AXT_MOTION_PULSE_OUTPUT_DEF
    #define AXT_MOTION_PULSE_OUTPUT_DEF
    /* Pulse Output Method                    */
    typedef enum _AXT_MOTION__PULSE_OUTPUT 
    {
        OneHighLowHigh              = 0x0,           // 1 pulse method, PULSE(Active High), forward direction(DIR=Low)  / reverse direction(DIR=High)
        OneHighHighLow              = 0x1,           // 1 pulse method, PULSE(Active High), forward direction (DIR=High) / reverse direction (DIR=Low)
        OneLowLowHigh               = 0x2,           // 1 pulse method, PULSE(Active Low), forward direction (DIR=Low)  / reverse direction (DIR=High)
        OneLowHighLow               = 0x3,           // 1 pulse method, PULSE(Active Low), forward direction (DIR=High) / reverse direction (DIR=Low)
        TwoCcwCwHigh                = 0x4,           // 2 pulse method, PULSE(CCW: reverse direction),  DIR(CW: forward direction),  Active High
        TwoCcwCwLow                 = 0x5,           // 2 pulse method, PULSE(CCW: reverse direction),  DIR(CW: forward direction),  Active Low
        TwoCwCcwHigh                = 0x6,           // 2 pulse method, PULSE(CW: forward direction),   DIR(CCW: reverse direction), Active High
        TwoCwCcwLow                 = 0x7,           // 2 pulse method, PULSE(CW: forward direction),   DIR(CCW: reverse direction), Active Low
        TwoPhase                    = 0x8,           // 2 phase (90' phase difference),  PULSE lead DIR(CW: forward direction), PULSE lag DIR(CCW: reverse direction)
        TwoPhaseReverse             = 0x9            // 2 phase(90' phase difference),  PULSE lead DIR(CCW: Forward diredtion), PULSE lag DIR(CW: Reverse direction)
    } AXT_MOTION_PULSE_OUTPUT;
#endif

#ifndef AXT_MOTION_EXTERNAL_COUNTER_INPUT_DEF
    #define AXT_MOTION_EXTERNAL_COUNTER_INPUT_DEF
    /* Mode2 Data   External Counter Input */
    typedef enum _AXT_MOTION__EXTERNAL_COUNTER_INPUT
    {
        ObverseUpDownMode           = 0x0,           // Forward direction Up/Down
        ObverseSqr1Mode             = 0x1,           // Forward direction 1 multiplication
        ObverseSqr2Mode             = 0x2,           // Forward direction 2 multiplication
        ObverseSqr4Mode             = 0x3,           // Forward direction 4 multiplication
        ReverseUpDownMode           = 0x4,           // Reverse direction Up/Down
        ReverseSqr1Mode             = 0x5,           // Reverse direction 1 multiplication
        ReverseSqr2Mode             = 0x6,           // Reverse direction 2 multiplication
        ReverseSqr4Mode             = 0x7            // Reverse direction 4 multiplication
    } AXT_MOTION_EXTERNAL_COUNTER_INPUT;
#endif

#ifndef AXT_MOTION_ACC_UNIT_DEF
    #define AXT_MOTION_ACC_UNIT_DEF
    typedef enum _AXT_MOTION_ACC_UNIT
    {
        UNIT_SEC2                   = 0x0,           // unit/sec2
        SEC                         = 0x1            // sec
    } AXT_MOTION_ACC_UNIT;
#endif

#ifndef AXT_MOTION_MOVE_DIR_DEF
    #define AXT_MOTION_MOVE_DIR_DEF
    typedef enum _AXT_MOTION_MOVE_DIR
    {
        DIR_CCW                     = 0x0,            // Counterclockwise direction
        DIR_CW                      = 0x1             // Clockwise direction
    } AXT_MOTION_MOVE_DIR;
#endif

#ifndef AXT_MOTION_RADIUS_DISTANCE_DEF
    #define AXT_MOTION_RADIUS_DISTANCE_DEF
    typedef enum _AXT_MOTION_RADIUS_DISTANCE
    {
        SHORT_DISTANCE              = 0x0,            // Short distance circular movement 
        LONG_DISTANCE               = 0x1             // Lone distance circular movement
    } AXT_MOTION_RADIUS_DISTANCE;
#endif

#ifndef AXT_MOTION_POS_TYPE_DEF
#define AXT_MOTION_POS_TYPE_DEF
	typedef enum _AXT_MOTION_POS_TYPE_DEF
	{
		POSITION_LIMIT              = 0x0,           // All Area use
		POSITION_BOUND              = 0x1            // Pos select area use
	} AXT_MOTION_POS_TYPE;
#endif

#ifndef AXT_MOTION_INTERPOLATION_AXIS_DEF
    #define AXT_MOTION_INTERPOLATION_AXIS_DEF
    typedef enum _AXT_MOTION_INTERPOLATION_AXIS
    {
        INTERPOLATION_AXIS2         = 0x0,            // When 2 axis is used for interpolation
        INTERPOLATION_AXIS3         = 0x1,            // When 3 axis is used for interpolation
        INTERPOLATION_AXIS4         = 0x2             // When 4 axis is used for interpolation
    } AXT_MOTION_INTERPOLATION_AXIS;
#endif

#ifndef AXT_MOTION_CONTISTART_NODE_DEF
    #define AXT_MOTION_CONTISTART_NODE_DEF
    typedef enum _AXT_MOTION_CONTISTART_NODE
    {
        CONTI_NODE_VELOCITY         = 0x0,            // Velocity assigned interpolation mode
        CONTI_NODE_MANUAL           = 0x1,            // Node acceleration and deceleration interpolation mode
        CONTI_NODE_AUTO             = 0x2             // Automatic acceleration and deceleration interpolation mode
    } AXT_MOTION_CONTISTART_NODE;
#endif

#ifndef AXT_MOTION_HOME_DETECT_SIGNAL_DEF
    #define AXT_MOTION_HOME_DETECT_SIGNAL_DEF
    typedef enum _AXT_MOTION_HOME_DETECT_SIGNAL
    {
        PosEndLimit                 = 0x0,            // +Elm(End limit) + direction limit sensor signal
        NegEndLimit                 = 0x1,            // -Elm(End limit) - direction limit sensor signal
        PosSloLimit                 = 0x2,            // +Slm(Decelerate limit) signal - Not-used
        NegSloLimit                 = 0x3,            // -Slm(Decelerate limit) signal - Not-used
        HomeSensor                  = 0x4,            // IN0(ORG)  home sensor signal
        EncodZPhase                 = 0x5,            // IN1(Z phase)  Encoder Z phase signal
        UniInput02                  = 0x6,            // IN2(universal) universal input number 2 signal
        UniInput03                  = 0x7,            // IN3(universal) universal input number 3 signal
        UniInput04                  = 0x8,            // IN4(universal) universal input number 4 signal
        UniInput05                  = 0x9,            // IN5(universal) universal input number 5 signal
    } AXT_MOTION_HOME_DETECT_SIGNAL;
#endif

#ifndef AXT_MOTION_INPUT_FILTER_SIGNAL_DEF
#define AXT_MOTION_INPUT_FILTER_SIGNAL_DEF
    typedef enum _AXT_MOTION_INPUT_FILTER_SIGNAL
    {
        END_LIMIT                   = 0x10,           // End limit +/- limit sensor signal
        INP_ALARM                   = 0x11,           // Inposition/Alarm signal
        UIN_00_01                   = 0x12,           // Home/Z-Phase signal
        UIN_02_04                   = 0x13,           // UIN 2, 3, 4 signal
    } AXT_MOTION_INPUT_FILTER_SIGNAL;
#endif

#ifndef AXT_MOTION_MPG_INPUT_METHOD_DEF
    #define AXT_MOTION_MPG_INPUT_METHOD_DEF
    typedef enum _AXT_MOTION_MPG_INPUT_METHOD
    {
        MPG_DIFF_ONE_PHASE          = 0x0,            // MPG input method One Phase
        MPG_DIFF_TWO_PHASE_1X       = 0x1,            // MPG input method TwoPhase1
        MPG_DIFF_TWO_PHASE_2X       = 0x2,            // MPG input method TwoPhase2
        MPG_DIFF_TWO_PHASE_4X       = 0x3,            // MPG input method TwoPhase4
        MPG_LEVEL_ONE_PHASE         = 0x4,            // MPG input method Level One Phase
        MPG_LEVEL_TWO_PHASE_1X      = 0x5,            // MPG input method Level Two Phase1
        MPG_LEVEL_TWO_PHASE_2X      = 0x6,            // MPG input method Level Two Phase2
        MPG_LEVEL_TWO_PHASE_4X      = 0x7,            // MPG input method Level Two Phase4
    } AXT_MOTION_MPG_INPUT_METHOD;
#endif

#ifndef AXT_MOTION_SENSOR_INPUT_METHOD_DEF
    #define AXT_MOTION_SENSOR_INPUT_METHOD_DEF
    typedef enum _AXT_MOTION_SENSOR_INPUT_METHOD
    {
        SENSOR_METHOD1              = 0x0,             // General drive
        SENSOR_METHOD2              = 0x1,             // Slow speed drive before sensor signal detection. General drive after signal detection
        SENSOR_METHOD3              = 0x2              // Slow speed drive
    } AXT_MOTION_SENSOR_INPUT_METHOD;
#endif

#ifndef AXT_MOTION_HOME_CRC_SELECT_DEF
    #define AXT_MOTION_HOME_CRC_SELECT_DEF
    typedef enum _AXT_MOTION_HOME_CRC_SELECT
    {
        CRC_SELECT1                 = 0x0,             // No use of position clear, no use of remaining pulse clear
        CRC_SELECT2                 = 0x1,             // Use of position clear, no use of remaining pulse clear 
        CRC_SELECT3                 = 0x2,             // No use of position clear, use of remaining pulse clear 
        CRC_SELECT4                 = 0x3              // Use of position clear, use of remaining pulse clear 
    } AXT_MOTION_HOME_CRC_SELECT;
#endif

#ifndef AXT_MOTION_IPDETECT_DESTINATION_SIGNAL_DEF
    #define AXT_MOTION_IPDETECT_DESTINATION_SIGNAL_DEF
    /* Detect Destination Signal            */
    typedef enum _AXT_MOTION_IPDETECT_DESTINATION_SIGNAL
    {
        PElmNegativeEdge            = 0x0,             // +Elm(End limit) falling edge
        NElmNegativeEdge            = 0x1,             // -Elm(End limit) falling edge
        PSlmNegativeEdge            = 0x2,             // +Slm(Slowdown limit) falling edge
        NSlmNegativeEdge            = 0x3,             // -Slm(Slowdown limit) falling edge
        In0DownEdge                 = 0x4,             // IN0(ORG) falling edge
        In1DownEdge                 = 0x5,             // IN1(Z phase) falling edge
        In2DownEdge                 = 0x6,             // IN2(universal) falling edge
        In3DownEdge                 = 0x7,             // IN3(universal) falling edge
        PElmPositiveEdge            = 0x8,             // +Elm(End limit) rising edge
        NElmPositiveEdge            = 0x9,             // -Elm(End limit) rising edge
        PSlmPositiveEdge            = 0xa,             // +Slm(Slowdown limit) rising edge
        NSlmPositiveEdge            = 0xb,             // -Slm(Slowdown limit) rising edge
        In0UpEdge                   = 0xc,             // IN0(ORG) rising edge
        In1UpEdge                   = 0xd,             // IN1(Z phase) rising edge
        In2UpEdge                   = 0xe,             // IN2(universal) rising edge
        In3UpEdge                   = 0xf              // IN3(universal) rising edge
    } AXT_MOTION_IPDETECT_DESTINATION_SIGNAL;
#endif

#ifndef AXT_MOTION_IPEND_STATUS_DEF
    #define AXT_MOTION_IPEND_STATUS_DEF
    typedef    enum _AXT_MOTION_IPEND_STATUS
    {
        IPEND_STATUS_SLM                  = 0x0001,    // Bit 0, exit by limit decelerate stop signal input
        IPEND_STATUS_ELM                  = 0x0002,    // Bit 1, exit by limit emergency stop signal input 
        IPEND_STATUS_SSTOP_SIGNAL         = 0x0004,    // Bit 2, exit by decelerate stop signal input
        IPEND_STATUS_ESTOP_SIGNAL         = 0x0008,    // Bit 3, exit by emergency stop signal input
        IPEND_STATUS_SSTOP_COMMAND        = 0x0010,    // Bit 4, exit by decelerate stop command
        IPEND_STATUS_ESTOP_COMMAND        = 0x0020,    // Bit 5, exit by emergency stop command
        IPEND_STATUS_ALARM_SIGNAL         = 0x0040,    // Bit 6, exit by Alarm signal input
        IPEND_STATUS_DATA_ERROR           = 0x0080,    // Bit 7, exit by data setting error
        IPEND_STATUS_DEVIATION_ERROR      = 0x0100,    // Bit 8, exit by deviation error
        IPEND_STATUS_ORIGIN_DETECT        = 0x0200,    // Bit 9, exit by home search
        IPEND_STATUS_SIGNAL_DETECT        = 0x0400,    // Bit 10, exit by signal search(Signal search-1/2 drive exit)
        IPEND_STATUS_PRESET_PULSE_DRIVE   = 0x0800,    // Bit 11, Preset pulse drive exit
        IPEND_STATUS_SENSOR_PULSE_DRIVE   = 0x1000,    // Bit 12, Sensor pulse drive exit
        IPEND_STATUS_LIMIT                = 0x2000,    // Bit 13, exit by Limit complete stop
        IPEND_STATUS_SOFTLIMIT            = 0x4000,    // Bit 14, exit by Soft limit
        IPEND_STATUS_INTERPOLATION_DRIVE  = 0x8000     // Bit 15, Interpolation drive exit
    } AXT_MOTION_IPEND_STATUS;
#endif

#ifndef AXT_MOTION_IPDRIVE_STATUS_DEF
    #define AXT_MOTION_IPDRIVE_STATUS_DEF
    /* IP Drive status                        */
    typedef    enum _AXT_MOTION_IPDRIVE_STATUS
    {
        IPDRIVE_STATUS_BUSY                     = 0x00001,   // Bit 0, BUSY (in DRIVE)
        IPDRIVE_STATUS_DOWN                     = 0x00002,   // Bit 1, DOWN(in deceleration DRIVE)
        IPDRIVE_STATUS_CONST                    = 0x00004,   // Bit 2, CONST(in constant speed DRIVE)
        IPDRIVE_STATUS_UP                       = 0x00008,   // Bit 3, UP(in acceleration DRIVE)
        IPDRIVE_STATUS_ICL                      = 0x00010,   // Bit 4, ICL(ICM < INCNT)
        IPDRIVE_STATUS_ICG                      = 0x00020,   // Bit 5, ICG(ICM < INCNT)
        IPDRIVE_STATUS_ECL                      = 0x00040,   // Bit 6, ECL(ECM > EXCNT)
        IPDRIVE_STATUS_ECG                      = 0x00080,   // Bit 7, ECG(ECM < EXCNT)
        IPDRIVE_STATUS_DRIVE_DIRECTION          = 0x00100,   // Bit 8, drive direction signal (0=CW/1=CCW)
        IPDRIVE_STATUS_COMMAND_BUSY             = 0x00200,   // Bit 9, In execution of command
        IPDRIVE_STATUS_PRESET_DRIVING           = 0x00400,   // Bit 10, In drive of a specific pulse number
        IPDRIVE_STATUS_CONTINUOUS_DRIVING       = 0x00800,   // Bit 11, In continuous drive
        IPDRIVE_STATUS_SIGNAL_SEARCH_DRIVING    = 0x01000,   // Bit 12, In signal search drive
        IPDRIVE_STATUS_ORG_SEARCH_DRIVING       = 0x02000,   // Bit 13, In home search drive 
        IPDRIVE_STATUS_MPG_DRIVING              = 0x04000,   // Bit 14, In MPG drive 
        IPDRIVE_STATUS_SENSOR_DRIVING           = 0x08000,   // Bit 15, In sensor position drive 
        IPDRIVE_STATUS_L_C_INTERPOLATION        = 0x10000,   // Bit 16, In straight/circular interpolation drive
        IPDRIVE_STATUS_PATTERN_INTERPOLATION    = 0x20000,   // Bit 17, In pattern interpolation drive
        IPDRIVE_STATUS_INTERRUPT_BANK1          = 0x40000,   // Bit 18, Interrupt occurrence in BANK1
        IPDRIVE_STATUS_INTERRUPT_BANK2          = 0x80000    // Bit 19, Interrupt occurrence in BANK2
    } AXT_MOTION_IPDRIVE_STATUS;
#endif

#ifndef AXT_MOTION_IPINTERRUPT_BANK1_DEF
    #define AXT_MOTION_IPINTERRUPT_BANK1_DEF
    /* IP Interrupt MASK setting            */
    typedef    enum _AXT_MOTION_IPINTERRUPT_BANK1
    {
        IPINTBANK1_DONTUSE              = 0x00000000,        // INTERRUT DISABLED.
        IPINTBANK1_DRIVE_END            = 0x00000001,        // Bit 0, Drive end(default value : 1).
        IPINTBANK1_ICG                  = 0x00000002,        // Bit 1, INCNT is greater than INCNTCMP.
        IPINTBANK1_ICE                  = 0x00000004,        // Bit 2, INCNT is equal with INCNTCMP.
        IPINTBANK1_ICL                  = 0x00000008,        // Bit 3, INCNT is less than INCNTCMP.
        IPINTBANK1_ECG                  = 0x00000010,        // Bit 4, EXCNT is greater than EXCNTCMP.
        IPINTBANK1_ECE                  = 0x00000020,        // Bit 5, EXCNT is equal with EXCNTCMP.
        IPINTBANK1_ECL                  = 0x00000040,        // Bit 6, EXCNT is less than EXCNTCMP.
        IPINTBANK1_SCRQEMPTY            = 0x00000080,        // Bit 7, Script control queue is empty.
        IPINTBANK1_CAPRQEMPTY           = 0x00000100,        // Bit 8, Caption result data queue is empty.
        IPINTBANK1_SCRREG1EXE           = 0x00000200,        // Bit 9, Script control register-1 command is executed.
        IPINTBANK1_SCRREG2EXE           = 0x00000400,        // Bit 10, Script control register-2 command is executed.
        IPINTBANK1_SCRREG3EXE           = 0x00000800,        // Bit 11, Script control register-3 command is executed.
        IPINTBANK1_CAPREG1EXE           = 0x00001000,        // Bit 12, Caption control register-1 command is executed.
        IPINTBANK1_CAPREG2EXE           = 0x00002000,        // Bit 13, Caption control register-2 command is executed.
        IPINTBANK1_CAPREG3EXE           = 0x00004000,        // Bit 14, Caption control register-3 command is executed.
        IPINTBANK1_INTGGENCMD           = 0x00008000,        // Bit 15, Interrupt generation command is executed(0xFF)
        IPINTBANK1_DOWN                 = 0x00010000,        // Bit 16, At starting point for deceleration drive.
        IPINTBANK1_CONT                 = 0x00020000,        // Bit 17, At starting point for constant speed drive.
        IPINTBANK1_UP                   = 0x00040000,        // Bit 18, At starting point for acceleration drive.
        IPINTBANK1_SIGNALDETECTED       = 0x00080000,        // Bit 19, Signal assigned in MODE1 is detected.
        IPINTBANK1_SP23E                = 0x00100000,        // Bit 20, Current speed is equal with rate change point RCP23.
        IPINTBANK1_SP12E                = 0x00200000,        // Bit 21, Current speed is equal with rate change point RCP12.
        IPINTBANK1_SPE                  = 0x00400000,        // Bit 22, Current speed is equal with speed comparison data(SPDCMP).
        IPINTBANK1_INCEICM              = 0x00800000,        // Bit 23, INTCNT(1'st counter) is equal with ICM(1'st count minus limit data)
        IPINTBANK1_SCRQEXE              = 0x01000000,        // Bit 24, Script queue command is executed When SCRCONQ's 30 bit is '1'.
        IPINTBANK1_CAPQEXE              = 0x02000000,        // Bit 25, Caption queue command is executed When CAPCONQ's 30 bit is '1'.
        IPINTBANK1_SLM                  = 0x04000000,        // Bit 26, NSLM/PSLM input signal is activated.
        IPINTBANK1_ELM                  = 0x08000000,        // Bit 27, NELM/PELM input signal is activated.
        IPINTBANK1_USERDEFINE1          = 0x10000000,        // Bit 28, Selectable interrupt source 0(refer "0xFE" command).
        IPINTBANK1_USERDEFINE2          = 0x20000000,        // Bit 29, Selectable interrupt source 1(refer "0xFE" command).
        IPINTBANK1_USERDEFINE3          = 0x40000000,        // Bit 30, Selectable interrupt source 2(refer "0xFE" command).
        IPINTBANK1_USERDEFINE4          = 0x80000000         // Bit 31, Selectable interrupt source 3(refer "0xFE" command).
    } AXT_MOTION_IPINTERRUPT_BANK1;
#endif

#ifndef AXT_MOTION_IPINTERRUPT_BANK2_DEF
    #define AXT_MOTION_IPINTERRUPT_BANK2_DEF
    typedef    enum _AXT_MOTION_IPINTERRUPT_BANK2
    {
        IPINTBANK2_DONTUSE                  = 0x00000000,    // INTERRUT DISABLED.
        IPINTBANK2_L_C_INP_Q_EMPTY          = 0x00000001,    // Bit 0, Linear/Circular interpolation parameter queue is empty.
        IPINTBANK2_P_INP_Q_EMPTY            = 0x00000002,    // Bit 1, Bit pattern interpolation queue is empty.
        IPINTBANK2_ALARM_ERROR              = 0x00000004,    // Bit 2, Alarm input signal is activated.
        IPINTBANK2_INPOSITION               = 0x00000008,    // Bit 3, Inposition input signal is activated.
        IPINTBANK2_MARK_SIGNAL_HIGH         = 0x00000010,    // Bit 4, Mark input signal is activated.
        IPINTBANK2_SSTOP_SIGNAL             = 0x00000020,    // Bit 5, SSTOP input signal is activated.
        IPINTBANK2_ESTOP_SIGNAL             = 0x00000040,    // Bit 6, ESTOP input signal is activated.
        IPINTBANK2_SYNC_ACTIVATED           = 0x00000080,    // Bit 7, SYNC input signal is activated.
        IPINTBANK2_TRIGGER_ENABLE           = 0x00000100,    // Bit 8, Trigger output is activated.
        IPINTBANK2_EXCNTCLR                 = 0x00000200,    // Bit 9, External(2'nd) counter is cleard by EXCNTCLR setting.
        IPINTBANK2_FSTCOMPARE_RESULT_BIT0   = 0x00000400,    // Bit 10, ALU1's compare result bit 0 is activated.
        IPINTBANK2_FSTCOMPARE_RESULT_BIT1   = 0x00000800,    // Bit 11, ALU1's compare result bit 1 is activated.
        IPINTBANK2_FSTCOMPARE_RESULT_BIT2   = 0x00001000,    // Bit 12, ALU1's compare result bit 2 is activated.
        IPINTBANK2_FSTCOMPARE_RESULT_BIT3   = 0x00002000,    // Bit 13, ALU1's compare result bit 3 is activated.
        IPINTBANK2_FSTCOMPARE_RESULT_BIT4   = 0x00004000,    // Bit 14, ALU1's compare result bit 4 is activated.
        IPINTBANK2_SNDCOMPARE_RESULT_BIT0   = 0x00008000,    // Bit 15, ALU2's compare result bit 0 is activated.
        IPINTBANK2_SNDCOMPARE_RESULT_BIT1   = 0x00010000,    // Bit 16, ALU2's compare result bit 1 is activated.
        IPINTBANK2_SNDCOMPARE_RESULT_BIT2   = 0x00020000,    // Bit 17, ALU2's compare result bit 2 is activated.
        IPINTBANK2_SNDCOMPARE_RESULT_BIT3   = 0x00040000,    // Bit 18, ALU2's compare result bit 3 is activated.
        IPINTBANK2_SNDCOMPARE_RESULT_BIT4   = 0x00080000,    // Bit 19, ALU2's compare result bit 4 is activated.
        IPINTBANK2_L_C_INP_Q_LESS_4         = 0x00100000,    // Bit 20, Linear/Circular interpolation parameter queue is less than 4.
        IPINTBANK2_P_INP_Q_LESS_4           = 0x00200000,    // Bit 21, Pattern interpolation parameter queue is less than 4.
        IPINTBANK2_XSYNC_ACTIVATED          = 0x00400000,    // Bit 22, X axis sync input signal is activated.
        IPINTBANK2_YSYNC_ACTIVATED          = 0x00800000,    // Bit 23, Y axis sync input siangl is activated.
        IPINTBANK2_P_INP_END_BY_END_PATTERN = 0x01000000     // Bit 24, Bit pattern interpolation is terminated by end pattern.
    //    IPINTBANK2_                       = 0x02000000,    // Bit 25, Don't care.
    //    IPINTBANK2_                       = 0x04000000,    // Bit 26, Don't care.
    //    IPINTBANK2_                       = 0x08000000,    // Bit 27, Don't care.
    //    IPINTBANK2_                       = 0x10000000,    // Bit 28, Don't care.
    //    IPINTBANK2_                       = 0x20000000,    // Bit 29, Don't care.
    //    IPINTBANK2_                       = 0x40000000,    // Bit 30, Don't care.
    //    IPINTBANK2_                       = 0x80000000     // Bit 31, Don't care.
    } AXT_MOTION_IPINTERRUPT_BANK2;
#endif


#ifndef AXT_MOTION_IPMECHANICAL_SIGNAL_DEF
    #define AXT_MOTION_IPMECHANICAL_SIGNAL_DEF
    /* IP Drive status                        */
    typedef    enum _AXT_MOTION_IPMECHANICAL_SIGNAL
    {

        IPMECHANICAL_PELM_LEVEL             = 0x0001,        // Bit 0, +Limit emergency stop signal input Level
        IPMECHANICAL_NELM_LEVEL             = 0x0002,        // Bit 1, -Limit emergency stop signal input Level
        IPMECHANICAL_PSLM_LEVEL             = 0x0004,        // Bit 2, +limit decelerate stop signal input Level
        IPMECHANICAL_NSLM_LEVEL             = 0x0008,        // Bit 3, -limit decelerate stop signal input Level
        IPMECHANICAL_ALARM_LEVEL            = 0x0010,        // Bit 4, Alarm signal input Level
        IPMECHANICAL_INP_LEVEL              = 0x0020,        // Bit 5, InPos signal input Level
        IPMECHANICAL_ENC_DOWN_LEVEL         = 0x0040,        // Bit 6, encoder DOWN(B phase) signal input Level
        IPMECHANICAL_ENC_UP_LEVEL           = 0x0080,        // Bit 7, encoder UP(A phase) signal input Level
        IPMECHANICAL_EXMP_LEVEL             = 0x0100,        // Bit 8, EXMP signal input Level
        IPMECHANICAL_EXPP_LEVEL             = 0x0200,        // Bit 9,EXPP signal input Level
        IPMECHANICAL_MARK_LEVEL             = 0x0400,        // Bit 10, MARK# signal input Level
        IPMECHANICAL_SSTOP_LEVEL            = 0x0800,        // Bit 11, SSTOP signal input Level
        IPMECHANICAL_ESTOP_LEVEL            = 0x1000,        // Bit 12, ESTOP signal input Level
        IPMECHANICAL_SYNC_LEVEL             = 0x2000,        // Bit 13, SYNC signal input Level
        IPMECHANICAL_MODE8_16_LEVEL         = 0x4000         // Bit 14, MODE8_16 signal input Level

    } AXT_MOTION_IPMECHANICAL_SIGNAL;
#endif

#ifndef AXT_MOTION_QIDETECT_DESTINATION_SIGNAL_DEF
    #define AXT_MOTION_QIDETECT_DESTINATION_SIGNAL_DEF
    /* Detect Destination Signal            */
    typedef enum _AXT_MOTION_QIDETECT_DESTINATION_SIGNAL
    {
        Signal_PosEndLimit                  = 0x0,           // +Elm(End limit) + direction limit sensor signal
        Signal_NegEndLimit                  = 0x1,           // -Elm(End limit) - direction limit sensor signal
        Signal_PosSloLimit                  = 0x2,           // +Slm(Slow Down limit) signal - Not-used
        Signal_NegSloLimit                  = 0x3,           // -Slm(Slow Down limit) signal - Not-used
        Signal_HomeSensor                   = 0x4,           // IN0(ORG)  home sensor signal
        Signal_EncodZPhase                  = 0x5,           // IN1(Z phase)  Encoder Z phase signal
        Signal_UniInput02                   = 0x6,           // IN2(universal) universal input number 2 signal
        Signal_UniInput03                   = 0x7            // IN3(universal) universal input number 3 signal
    } AXT_MOTION_QIDETECT_DESTINATION_SIGNAL;
#endif

#ifndef AXT_MOTION_QIMECHANICAL_SIGNAL_DEF
    #define AXT_MOTION_QIMECHANICAL_SIGNAL_DEF
    /* QI Drive status                        */
    typedef    enum _AXT_MOTION_QIMECHANICAL_SIGNAL
    {

        QIMECHANICAL_PELM_LEVEL             = 0x00001,       // Bit 0, +Limit emergency stop signal current state
        QIMECHANICAL_NELM_LEVEL             = 0x00002,       // Bit 1, -Limit emergency stop signal current state
        QIMECHANICAL_PSLM_LEVEL             = 0x00004,       // Bit 2, +limit decelerate stop signal current state
        QIMECHANICAL_NSLM_LEVEL             = 0x00008,       // Bit 3, -limit decelerate stop signal current state
        QIMECHANICAL_ALARM_LEVEL            = 0x00010,       // Bit 4, Alarm signal current state
        QIMECHANICAL_INP_LEVEL              = 0x00020,       // Bit 5, InPos signal current state
        QIMECHANICAL_ESTOP_LEVEL            = 0x00040,       // Bit 6, emergency stop signal(ESTOP) current state
        QIMECHANICAL_ORG_LEVEL              = 0x00080,       // Bit 7, home signal current state
        QIMECHANICAL_ZPHASE_LEVEL           = 0x00100,       // Bit 8, Z phase input signal current state 
        QIMECHANICAL_ECUP_LEVEL             = 0x00200,       // Bit 9, ECUP terminal signal state
        QIMECHANICAL_ECDN_LEVEL             = 0x00400,       // Bit 10, ECDN terminal signal state
        QIMECHANICAL_EXPP_LEVEL             = 0x00800,       // Bit 11, EXPP terminal signal state
        QIMECHANICAL_EXMP_LEVEL             = 0x01000,       // Bit 12, EXMP terminal signal state
        QIMECHANICAL_SQSTR1_LEVEL           = 0x02000,       // Bit 13, SQSTR1 terminal signal state
        QIMECHANICAL_SQSTR2_LEVEL           = 0x04000,       // Bit 14, SQSTR2 terminal signal state
        QIMECHANICAL_SQSTP1_LEVEL           = 0x08000,       // Bit 15, SQSTP1 terminal signal state
        QIMECHANICAL_SQSTP2_LEVEL           = 0x10000,       // Bit 16, SQSTP2 terminal signal state
        QIMECHANICAL_MODE_LEVEL             = 0x20000        // Bit 17, MODE terminal signal state

    } AXT_MOTION_QIMECHANICAL_SIGNAL;
#endif

#ifndef AXT_MOTION_QIEND_STATUS_DEF
    #define AXT_MOTION_QIEND_STATUS_DEF
    /* QI End status                        */
    typedef    enum _AXT_MOTION_QIEND_STATUS
    {
        QIEND_STATUS_0                      = 0x00000001,    // Bit 0, exit by forward direction limit signal (PELM)
        QIEND_STATUS_1                      = 0x00000002,    // Bit 1, exit by reverse direction limit signal (NELM)
        QIEND_STATUS_2                      = 0x00000004,    // Bit 2, exit by forward direction additional limit signal (PSLM)
        QIEND_STATUS_3                      = 0x00000008,    // Bit 3, exit by reverse direction additional limit signal (NSLM)
        QIEND_STATUS_4                      = 0x00000010,    // Bit 4, exit by forward direction soft limit emergency stop function
        QIEND_STATUS_5                      = 0x00000020,    // Bit 5, exit by reverse direction soft limit emergency stop function
        QIEND_STATUS_6                      = 0x00000040,    // Bit 6, exit by forward direction soft limit decelerate stop function
        QIEND_STATUS_7                      = 0x00000080,    // Bit 7, exit by reverse direction soft limit decelerate stop function
        QIEND_STATUS_8                      = 0x00000100,    // Bit 8, drive exit by servo alarm function
        QIEND_STATUS_9                      = 0x00000200,    // Bit 9, drive exit by emergency stop signal input
        QIEND_STATUS_10                     = 0x00000400,    // Bit 10, drive exit by emergency stop command
        QIEND_STATUS_11                     = 0x00000800,    // Bit 11, drive exit by decelerate stop command
        QIEND_STATUS_12                     = 0x00001000,    // Bit 12, drive exit by entire axes emergency stop command 
        QIEND_STATUS_13                     = 0x00002000,    // Bit 13, drive exit by sync stop function #1(SQSTP1)
        QIEND_STATUS_14                     = 0x00004000,    // Bit 14, drive exit by sync stop function #2(SQSTP2)
        QIEND_STATUS_15                     = 0x00008000,    // Bit 15, encoder input (ECUP,ECDN) error occurrence
        QIEND_STATUS_16                     = 0x00010000,    // Bit 16, MPG input (EXPP,EXMP) error occurrence
        QIEND_STATUS_17                     = 0x00020000,    // Bit 17, exit with successful home search
        QIEND_STATUS_18                     = 0x00040000,    // Bit 18, exit with successful signal search
        QIEND_STATUS_19                     = 0x00080000,    // Bit 19, drive exit by interpolation data abnormality
        QIEND_STATUS_20                     = 0x00100000,    // Bit 20, abnormal drive stop occurrence
        QIEND_STATUS_21                     = 0x00200000,    // Bit 21, MPG function block pulse buffer overflow occurrence
        QIEND_STATUS_22                     = 0x00400000,    // Bit 22, DON'CARE
        QIEND_STATUS_23                     = 0x00800000,    // Bit 23, DON'CARE
        QIEND_STATUS_24                     = 0x01000000,    // Bit 24, DON'CARE
        QIEND_STATUS_25                     = 0x02000000,    // Bit 25, DON'CARE
        QIEND_STATUS_26                     = 0x04000000,    // Bit 26, DON'CARE
        QIEND_STATUS_27                     = 0x08000000,    // Bit 27, DON'CARE
        QIEND_STATUS_28                     = 0x10000000,    // Bit 28, current/last move drive direction
        QIEND_STATUS_29                     = 0x20000000,    // Bit 29, in output of remaining pulse clear
        QIEND_STATUS_30                     = 0x40000000,    // Bit 30, abnormal drive stop cause state
        QIEND_STATUS_31                     = 0x80000000     // Bit 31, interpolation drive data error state

    } AXT_MOTION_QIEND_STATUS;
#endif

#ifndef AXT_MOTION_QIDRIVE_STATUS_DEF
    #define AXT_MOTION_QIDRIVE_STATUS_DEF
    /* QI Drive status                        */
    typedef    enum _AXT_MOTION_QIDRIVE_STATUS
    {
        QIDRIVE_STATUS_0                    = 0x0000001,     // Bit 0, BUSY(in drive move)
        QIDRIVE_STATUS_1                    = 0x0000002,     // Bit 1, DOWN(in deceleration)
        QIDRIVE_STATUS_2                    = 0x0000004,     // Bit 2, CONST(in constant speed)
        QIDRIVE_STATUS_3                    = 0x0000008,     // Bit 3, UP(in acceleration)
        QIDRIVE_STATUS_4                    = 0x0000010,     // Bit 4, in move of continuous drive
        QIDRIVE_STATUS_5                    = 0x0000020,     // Bit 5, in move of assigned distance drive
        QIDRIVE_STATUS_6                    = 0x0000040,     // Bit 6, in move of MPG drive
        QIDRIVE_STATUS_7                    = 0x0000080,     // Bit 7, in move of home search drive
        QIDRIVE_STATUS_8                    = 0x0000100,     // Bit 8, in move of signal search drive
        QIDRIVE_STATUS_9                    = 0x0000200,     // Bit 9, in move of interpolation drive
        QIDRIVE_STATUS_10                   = 0x0000400,     // Bit 10, in move of Slave drive
        QIDRIVE_STATUS_11                   = 0x0000800,     // Bit 11, current move drive direction (different indication information in interpolation drive)
        QIDRIVE_STATUS_12                   = 0x0001000,     // Bit 12, in waiting for servo position completion after pulse output
        QIDRIVE_STATUS_13                   = 0x0002000,     // Bit 13, in move of straight line interpolation drive
        QIDRIVE_STATUS_14                   = 0x0004000,     // Bit 14, in move of circular interpolation drive
        QIDRIVE_STATUS_15                   = 0x0008000,     // Bit 15, in pulse output
        QIDRIVE_STATUS_16                   = 0x0010000,     // Bit 16, drive reserved data number(start)(0-7)
        QIDRIVE_STATUS_17                   = 0x0020000,     // Bit 17, drive reserved data number (middle)(0-7)
        QIDRIVE_STATUS_18                   = 0x0040000,     // Bit 18, drive reserved data number (end)(0-7)
        QIDRIVE_STATUS_19                   = 0x0080000,     // Bit 19, drive reversed Queue is cleared
        QIDRIVE_STATUS_20                   = 0x0100000,     // Bit 20, drive reversed Queue is full
        QIDRIVE_STATUS_21                   = 0x0200000,     // Bit 21, velocity mode of current move drive (start)
        QIDRIVE_STATUS_22                   = 0x0400000,     // Bit 22, velocity mode of current move drive (end)
        QIDRIVE_STATUS_23                   = 0x0800000,     // Bit 23, MPG buffer #1 Full
        QIDRIVE_STATUS_24                   = 0x1000000,     // Bit 24, MPG buffer #2 Full
        QIDRIVE_STATUS_25                   = 0x2000000,     // Bit 25, MPG buffer #3 Full
        QIDRIVE_STATUS_26                   = 0x4000000      // Bit 26, MPG buffer data OverFlow

    } AXT_MOTION_QIDRIVE_STATUS;
#endif

#ifndef AXT_MOTION_QIINTERRUPT_BANK1_DEF
    #define AXT_MOTION_QIINTERRUPT_BANK1_DEF
    /* QI Interrupt MASK setting            */
    typedef    enum _AXT_MOTION_QIINTERRUPT_BANK1
    {
        QIINTBANK1_DISABLE                  = 0x00000000,    // INTERRUT DISABLED.
        QIINTBANK1_0                        = 0x00000001,    // Bit 0,  when drive set interrupt occurrence use is exit
        QIINTBANK1_1                        = 0x00000002,    // Bit 1,  when drive is exited
        QIINTBANK1_2                        = 0x00000004,    // Bit 2,  when drive is started
        QIINTBANK1_3                        = 0x00000008,    // Bit 3,  counter #1 < comparator #1 event occurrence
        QIINTBANK1_4                        = 0x00000010,    // Bit 4,  counter #1 = comparator #1 event occurrence
        QIINTBANK1_5                        = 0x00000020,    // Bit 5,  counter #1 > comparator #1 event occurrence
        QIINTBANK1_6                        = 0x00000040,    // Bit 6,  counter #2 < comparator #2 event occurrence
        QIINTBANK1_7                        = 0x00000080,    // Bit 7,  counter #2 = comparator #2 event occurrence
        QIINTBANK1_8                        = 0x00000100,    // Bit 8,  counter #2 > comparator #2 event occurrence
        QIINTBANK1_9                        = 0x00000200,    // Bit 9,  counter #3 < comparator #3 event occurrence
        QIINTBANK1_10                       = 0x00000400,    // Bit 10, counter #3 = comparator #3 event occurrence
        QIINTBANK1_11                       = 0x00000800,    // Bit 11, counter #3 > comparator #3 event occurrence
        QIINTBANK1_12                       = 0x00001000,    // Bit 12, counter #4 < comparator #4 event occurrence
        QIINTBANK1_13                       = 0x00002000,    // Bit 13, counter #4 = comparator #4 event occurrence
        QIINTBANK1_14                       = 0x00004000,    // Bit 14, counter #4 < comparator #4 event occurrence
        QIINTBANK1_15                       = 0x00008000,    // Bit 15, counter #5 < comparator #5 event occurrence
        QIINTBANK1_16                       = 0x00010000,    // Bit 16, counter #5 = comparator #5 event occurrence
        QIINTBANK1_17                       = 0x00020000,    // Bit 17, counter #5 > comparator #5 event occurrence
        QIINTBANK1_18                       = 0x00040000,    // Bit 18, timer #1 event occurrence
        QIINTBANK1_19                       = 0x00080000,    // Bit 19, timer #2 event occurrence
        QIINTBANK1_20                       = 0x00100000,    // Bit 20, drive reservation setting Queue is cleared
        QIINTBANK1_21                       = 0x00200000,    // Bit 21, drive reservation setting Queue is full
        QIINTBANK1_22                       = 0x00400000,    // Bit 22, trigger occurring distance period/absolute position Queue is cleared
        QIINTBANK1_23                       = 0x00800000,    // Bit 23, trigger occurring distance period/absolute position Queue is full
        QIINTBANK1_24                       = 0x01000000,    // Bit 24, trigger signal occurrence event
        QIINTBANK1_25                       = 0x02000000,    // Bit 25, script #1 command reservation setting Queue is cleared
        QIINTBANK1_26                       = 0x04000000,    // Bit 26, script #2 command reservation setting Queue is cleared
        QIINTBANK1_27                       = 0x08000000,    // Bit 27, script #3 initialized with execution of command reservation setting register
        QIINTBANK1_28                       = 0x10000000,    // Bit 28, script #4 initialized with execution of command reservation setting register
        QIINTBANK1_29                       = 0x20000000,    // Bit 29, servo alarm signal is permitted
        QIINTBANK1_30                       = 0x40000000,    // Bit 30, |CNT1| - |CNT2| >= |CNT4| event occurrence
        QIINTBANK1_31                       = 0x80000000     // Bit 31, interrupt occurrence command |INTGEN| execution
    } AXT_MOTION_QIINTERRUPT_BANK1;
#endif

#ifndef AXT_MOTION_QIINTERRUPT_BANK2_DEF
    #define AXT_MOTION_QIINTERRUPT_BANK2_DEF
    typedef    enum _AXT_MOTION_QIINTERRUPT_BANK2
    {
        QIINTBANK2_DISABLE                  = 0x00000000,    // INTERRUT DISABLED.
        QIINTBANK2_0                        = 0x00000001,    // Bit 0,  script #1 reading command result Queue is full
        QIINTBANK2_1                        = 0x00000002,    // Bit 1,  script #2 reading command result Queue is full
        QIINTBANK2_2                        = 0x00000004,    // Bit 2,  script #3 reading command result register is renewed with new data
        QIINTBANK2_3                        = 0x00000008,    // Bit 3,  script #4 reading command result register is renewed with new data
        QIINTBANK2_4                        = 0x00000010,    // Bit 4,  when reservation command of script #1 is executed, command set by interrupt occurrence is executed
        QIINTBANK2_5                        = 0x00000020,    // Bit 5,  when reservation command of script #2 is executed, command set by interrupt occurrence is executed
        QIINTBANK2_6                        = 0x00000040,    // Bit 6,  when reservation command of script #3 is executed, command set by interrupt occurrence is executed
        QIINTBANK2_7                        = 0x00000080,    // Bit 7,  when reservation command of script #4 is executed, command set by interrupt occurrence is executed
        QIINTBANK2_8                        = 0x00000100,    // Bit 8,  start drive
        QIINTBANK2_9                        = 0x00000200,    // Bit 9,  drive using servo position determination completion(InPos) function, exit condition occurrence
        QIINTBANK2_10                       = 0x00000400,    // Bit 10, event selection #1 condition occurrence to use during event counter operation
        QIINTBANK2_11                       = 0x00000800,    // Bit 11, event selection #2 condition occurrence to use during event counter operation
        QIINTBANK2_12                       = 0x00001000,    // Bit 12, SQSTR1 signal is permitted
        QIINTBANK2_13                       = 0x00002000,    // Bit 13, SQSTR2 signal is permitted
        QIINTBANK2_14                       = 0x00004000,    // Bit 14, UIO0 terminal signal is changed to '1'
        QIINTBANK2_15                       = 0x00008000,    // Bit 15, UIO1 terminal signal is changed to '1'
        QIINTBANK2_16                       = 0x00010000,    // Bit 16, UIO2 terminal signal is changed to '1'
        QIINTBANK2_17                       = 0x00020000,    // Bit 17, UIO3 terminal signal is changed to '1'
        QIINTBANK2_18                       = 0x00040000,    // Bit 18, UIO4 terminal signal is changed to '1'
        QIINTBANK2_19                       = 0x00080000,    // Bit 19, UIO5 terminal signal is changed to '1'
        QIINTBANK2_20                       = 0x00100000,    // Bit 20, UIO6 terminal signal is changed to '1'
        QIINTBANK2_21                       = 0x00200000,    // Bit 21, UIO7 terminal signal is changed to '1'
        QIINTBANK2_22                       = 0x00400000,    // Bit 22, UIO8 terminal signal is changed to '1'
        QIINTBANK2_23                       = 0x00800000,    // Bit 23, UIO9 terminal signal is changed to '1'
        QIINTBANK2_24                       = 0x01000000,    // Bit 24, UIO10 terminal signal is changed to '1'
        QIINTBANK2_25                       = 0x02000000,    // Bit 25, UIO11 terminal signal is changed to '1'
        QIINTBANK2_26                       = 0x04000000,    // Bit 26, error stop condition (LMT, ESTOP, STOP, ESTOP, CMD, ALARM) occurrence
        QIINTBANK2_27                       = 0x08000000,    // Bit 27, ata setting error is occurred during interpolation
        QIINTBANK2_28                       = 0x10000000,    // Bit 28, Don't Care
        QIINTBANK2_29                       = 0x20000000,    // Bit 29, limit signal (PELM, NELM) is inputted
        QIINTBANK2_30                       = 0x40000000,    // Bit 30, additional limit signal (PSLM, NSLM) is inputted
        QIINTBANK2_31                       = 0x80000000     // Bit 31, emergency stop signal (ESTOP) is input

    } AXT_MOTION_QIINTERRUPT_BANK2;
#endif

#ifndef AXT_NETWORK_STATUS_DEF
#define AXT_NETWORK_STATUS_DEF
    typedef    enum _AXT_NETWORK_STATUS
    {
        NET_STATUS_DISCONNECTED  = 1,
        NET_STATUS_LOCK_MISMATCH = 5,
        NET_STATUS_CONNECTED     = 6
    } AXT_NETWORK_STATUS;
#endif

#ifndef AXT_MOTION_INFO_DEF
#define AXT_MOTION_INFO_DEF
    typedef struct _MOTION_INFO
    {
        double   dCmdPos;          // Command position[0x01]
        double   dActPos;          // Encoder position[0x02]
        DWORD    dwMechSig;        // Mechanical Signal[0x04]    
        DWORD    dwDrvStat;        // Driver Status[0x08]
        DWORD    dwInput;          // Universal Signal Input[0x10]
        DWORD    dwOutput;         // Universal Signal Output[0x10]
        DWORD    dwMask;           // mask value for reading Mask. Ex) 0x1F, reading all informations.    
    } MOTION_INFO, *PMOTION_INFO;
#endif
    
#ifndef AXT_MOTION_OVERRIDE_MODE_DEF
#define AXT_MOTION_OVERRIDE_MODE_DEF
    typedef enum _AXT_MOTION_OVERRIDE_MODE
    {
        OVERRIDE_POS_START      = 0,
        OVERRIDE_POS_END        = 1
    } AXT_MOTION_OVERRIDE_MODE;
#endif

#ifndef AXT_MOTION_PROFILE_PRIORITY_DEF
#define AXT_MOTION_PROFILE_PRIORITY_DEF
    typedef enum _AXT_MOTION_PROFILE_PRIORITY
    {
        PRIORITY_VELOCITY       = 0,
        PRIORITY_ACCELTIME      = 1
    } AXT_MOTION_PROFILE_PRIORITY;
#endif

#ifndef AXT_MOTION_FUNC_RETURN_MODE_DEF
#define AXT_MOTION_FUNC_RETURN_MODE_DEF
    typedef enum _AXT_MOTION_FUNC_RETURN_MODE
    {
        FUNC_RETURN_IMMEDIATE       = 0,
        FUNC_RETURN_BLOCKING        = 1,
        FUNC_RETURN_NON_BLOCKING    = 2
    } AXT_MOTION_FUNC_RETURN_MODE;
#endif

#ifndef MAX_SERVO_ALARM_HISTORY
#define MAX_SERVO_ALARM_HISTORY       15
#endif

//Mechatrolink III
enum{
COM_CONNECT,
COM_DISCONNECT,
COM_OFFLINE
};

enum MONITOR_SELECT_INFORMATION
{
	_MONI_APOS_ = 0,				// feedback position  : current position of the motor
	_MONI_CPOS_,					// command position   : command position after acceleration/deceleration filter
	_MONI_PERR_,					// position error     : Position error of the control loop
	_MONI_LPOS1_,                   // latched position 1 : motor position 1 latched by the latch signal 
	_MONI_LPOS2_,					// latched position 2 : motor position 2 latched by the latch signal
	_MONI_FSPD_,					// feedback speed     : current speed of the motor 
	_MONI_CSPD_,					// reference speed    : command speed of the motor
	_MONI_TRQ_,						// torque(force) reference : command torque(force) of the motor
	_MONI_ALARM_,					// detailed information on the current alarm : current alarm/warning (2-byte data, higher 2 bytes fixed as 0x0000)
	_MONI_MPOS_,					// command position(optional) : the details of the monitor data are specified in the product specifications. example : internal command position of the control loop
	_MONI_RES1_,					// reserved
	_MONI_RES2_,					// reserved	
	_MONI_CMN1_,					// common monitor 1 : selects the monitor data specified at common parameter 89.(for the contents of the monitor data, refer to common parameter 89)
	_MONI_CMN2_,					// common monitor 2 : selects the monitor data specified at common parameter 8A.(for the contents of the monitor data, refer to common parameter 8A)
	_MONI_OMN1_,					// optional monitor 1 : selects the monitor data specified by parameter.(the contents of the monitor data depend on the product specifications)
	_MONI_OMN2_						// optional monitor 2 : selects the monitor data specified by parameter.(the contents of the monitor data depend on the product specifications)
};

#ifndef	__AXT_CN2CH_DEFINE_H__
#define	__AXT_CN2CH_DEFINE_H__

	/* Main clock							*/
#define F_50M_CLK						50000000L			/* 50.000 MHz */

	/* Read/Write port							*/
	typedef enum _CNTPORT_DATA_WRITE
	{
		CnCommand	= 0x10,
		CnData1		= 0x12,
		CnData2		= 0x14,
		CnData3		= 0x16,
		CnData4		= 0x18,
		CnData12    = 0x44,
		CnData34    = 0x46
	}CNTPORT_DATA;

	typedef enum _CNTRAM_DATA
	{
		CnRamAddr1	= 0x28,
		CnRamAddr2	= 0x2A,
		CnRamAddr3	= 0x2C,
		CnRamAddrx1 = 0x48,
		CnRamAddr23 = 0x4A
	}CNTRAM_DATA;

	typedef enum _PHASE_SEL
	{
		CnAbPhase	= 0x0,
		CnZPhase	= 0x1
	} PHASE_SEL;

	typedef enum _COUNTER_INPUT
	{
		CnUpDownMode = 0x0,					// Up/Down
		CnSqr1Mode   = 0x1,					// 1x
		CnSqr2Mode   = 0x2,					// 2x
		CnSqr4Mode   = 0x3					// 4x
	} COUNTER_INPUT;

	/* CNT COMMAND LIST						*/
	typedef enum _CNTCOMMAND
	{
		// CH-1 Group Register
		CnCh1CounterRead					= 0x10,				// CH1 COUNTER READ, 24BIT
		CnCh1CounterWrite					= 0x90,				// CH1 COUNTER WRITE
		CnCh1CounterModeRead				= 0x11,				// CH1 COUNTER MODE READ, 8BIT
		CnCh1CounterModeWrite				= 0x91,				// CH1 COUNTER MODE WRITE
		CnCh1TriggerRegionLowerDataRead		= 0x12,				// CH1 TRIGGER REGION LOWER DATA READ, 24BIT
		CnCh1TriggerRegionLowerDataWrite	= 0x92,				// CH1 TRIGGER REGION LOWER DATA WRITE
		CnCh1TriggerRegionUpperDataRead		= 0x13,				// CH1 TRIGGER REGION UPPER DATA READ, 24BIT
		CnCh1TriggerRegionUpperDataWrite	= 0x93,				// CH1 TRIGGER REGION UPPER DATA WRITE
		CnCh1TriggerPeriodRead				= 0x14,				// CH1 TRIGGER PERIOD READ, 24BIT, RESERVED
		CnCh1TriggerPeriodWrite				= 0x94,				// CH1 TRIGGER PERIOD WRITE
		CnCh1TriggerPulseWidthRead			= 0x15,				// CH1 TRIGGER PULSE WIDTH READ
		CnCh1TriggerPulseWidthWrite			= 0x95,				// CH1 RIGGER PULSE WIDTH WRITE
		CnCh1TriggerModeRead				= 0x16,				// CH1 TRIGGER MODE READ
		CnCh1TriggerModeWrite				= 0x96,				// CH1 RIGGER MODE WRITE
		CnCh1TriggerStatusRead				= 0x17,				// CH1 TRIGGER STATUS READ
		CnCh1NoOperation_97					= 0x97,				// 
		CnCh1TriggerEnable					= 0x98,
		CnCh1TriggerDisable					= 0x99,
		CnCh1TimeTriggerFrequencyRead		= 0x1A,
		CnCh1TimeTriggerFrequencyWrite		= 0x9A,
		CnCh1ComparatorValueRead			= 0x1B,
		CnCh1ComparatorValueWrite			= 0x9B,
		CnCh1CompareatorConditionRead		= 0x1D,
		CnCh1CompareatorConditionWrite		= 0x9D,

		// CH-2 Group Register
		CnCh2CounterRead					= 0x20,				// CH2 COUNTER READ, 24BIT
		CnCh2CounterWrite					= 0xA1,				// CH2 COUNTER WRITE
		CnCh2CounterModeRead				= 0x21,				// CH2 COUNTER MODE READ, 8BIT
		CnCh2CounterModeWrite				= 0xA1,				// CH2 COUNTER MODE WRITE
		CnCh2TriggerRegionLowerDataRead		= 0x22,				// CH2 TRIGGER REGION LOWER DATA READ, 24BIT
		CnCh2TriggerRegionLowerDataWrite	= 0xA2,				// CH2 TRIGGER REGION LOWER DATA WRITE
		CnCh2TriggerRegionUpperDataRead		= 0x23,				// CH2 TRIGGER REGION UPPER DATA READ, 24BIT
		CnCh2TriggerRegionUpperDataWrite	= 0xA3,				// CH2 TRIGGER REGION UPPER DATA WRITE
		CnCh2TriggerPeriodRead				= 0x24,				// CH2 TRIGGER PERIOD READ, 24BIT, RESERVED
		CnCh2TriggerPeriodWrite				= 0xA4,				// CH2 TRIGGER PERIOD WRITE
		CnCh2TriggerPulseWidthRead			= 0x25,				// CH2 TRIGGER PULSE WIDTH READ, 24BIT
		CnCh2TriggerPulseWidthWrite			= 0xA5,				// CH2 RIGGER PULSE WIDTH WRITE
		CnCh2TriggerModeRead				= 0x26,				// CH2 TRIGGER MODE READ
		CnCh2TriggerModeWrite				= 0xA6,				// CH2 RIGGER MODE WRITE
		CnCh2TriggerStatusRead				= 0x27,				// CH2 TRIGGER STATUS READ
		CnCh2NoOperation_A7					= 0xA7,				//
		CnCh2TriggerEnable					= 0xA8,
		CnCh2TriggerDisable					= 0xA9,
		CnCh2TimeTriggerFrequencyRead		= 0x2A,
		CnCh2TimeTriggerFrequencyWrite		= 0xAA,
		CnCh2ComparatorValueRead			= 0x2B,
		CnCh2ComparatorValueWrite			= 0xAB,
		CnCh2CompareatorConditionRead		= 0x2D,
		CnCh2CompareatorConditionWrite		= 0xAD,

		// Ram Access Group Register
		CnRamDataWithRamAddress				= 0x30,				// READ RAM DATA WITH RAM ADDR PORT ADDRESS
		CnRamDataWrite						= 0xB0,				// RAM DATA WRITE
		CnRamDataRead						= 0x31,				// RAM DATA READ, 32BIT

	}CNTCOMMAND;
#endif


#ifndef	__AXL_HPC_DEFINE_
#define	__AXL_HPC_DEFINE_

/* Read/Write port							*/
typedef enum _HPCPORT_DATA_WRITE
{	
	HpcReset		= 0x06,			// Software reset.
	HpcCommand		= 0x10,
	HpcData12		= 0x12,			// MSB of data port(31 ~ 16 bit)
	HpcData34		= 0x14,			// LSB of data port(15 ~ 0 bit)
	HpcCmStatus     = 0x1C,
}HPCPORT_DATA_WRITE;

typedef enum _HPCPORT_CH_STAUTS
{
	HpcCh1Mech		= 0x20,
	HpcCh1Status	= 0x22,
	HpcCh2Mech		= 0x30,
	HpcCh2Status	= 0x32,
	HpcCh3Mech		= 0x40,
	HpcCh3Status	= 0x42,
	HpcCh4Mech		= 0x50,
	HpcCh4Status	= 0x52
}HPCPORT_CH_STAUTS;

typedef enum _HPCPORT_ETC
{
	HpcDiIntFlag	= 0x60,
	HpcDiIntRiseMask= 0x62,
	HpcDiIntFallMask= 0x64,
	HpcCompIntFlag	= 0x66,
	HpcCompIntMask	= 0x68,
	HpcDinData		= 0x6A,
	HpcDoutData		= 0x6C
}HPCPORT_ETC;


typedef enum _HPCRAM_DATA
{
	HpcRamAddr1		= 0x70,			// MSB of Ram address(31  ~ 16 bit)
	HpcRamAddr2		= 0x72			// LSB of Ram address(15  ~ 0 bit)
}HPCRAM_DATA;


/*
--1) count_mode
--	0 bit : reverse.(0: normal, 1:reverse)
--	1 bit : count source selection.(0 : A/B 2 phase signal, 1 :  Z phase signal)
--	2~3 bit : count mode
--			"00" up/Down mode 
--			"01" 2 phase, 1times
--			"10" 2 phase, 2times
--			"11" 2 phase, 4times
*/

/*
-- Description : Trigger pulse generation block for SIO-CN2CH
-- trig_mode
-- 0 bit : trigger signal active level.
-- 1 bit : reserved.
-- 2 bit : trigger signal output method, valid when trig_mode(1) = '0', (0 : pulse out, 1 : level out)
-- 3 bit : reserved.
-- 5~4 bit : Trigger method
--	"00" : absolute mode[with ram data].
--	"01" : timer mode.
--	"10" : absolute mode[with fifo].
--	"11" : periodic mode.
-- 6 bit : direction valid check enable/disable when periodic mode.
-- 7 bit : direction valid when periodic mode.
*/

/* CNT COMMAND LIST						*/
typedef enum _HPCCOMMAND
{
	// CH-1 Group Register
	HpcCh1CounterRead					= 0x10,				// CH1 COUNTER READ, 32BIT
	HpcCh1CounterWrite					= 0x90,				// CH1 COUNTER WRITE, 32BIT
	HpcCh1CounterModeRead				= 0x11,				// CH1 COUNTER MODE READ, 4BIT
	HpcCh1CounterModeWrite				= 0x91,				// CH1 COUNTER MODE WRITE, 4BIT
	HpcCh1TriggerRegionLowerDataRead	= 0x12,				// CH1 TRIGGER REGION LOWER DATA READ, 31BIT
	HpcCh1TriggerRegionLowerDataWrite	= 0x92,				// CH1 TRIGGER REGION LOWER DATA WRITE
	HpcCh1TriggerRegionUpperDataRead	= 0x13,				// CH1 TRIGGER REGION UPPER DATA READ, 31BIT
	HpcCh1TriggerRegionUpperDataWrite	= 0x93,				// CH1 TRIGGER REGION UPPER DATA WRITE
	HpcCh1TriggerPeriodRead				= 0x14,				// CH1 TRIGGER PERIOD READ, 31BIT
	HpcCh1TriggerPeriodWrite			= 0x94,				// CH1 TRIGGER PERIOD WRITE
	HpcCh1TriggerPulseWidthRead			= 0x15,				// CH1 TRIGGER PULSE WIDTH READ, 31BIT
	HpcCh1TriggerPulseWidthWrite		= 0x95,				// CH1 RIGGER PULSE WIDTH WRITE
	HpcCh1TriggerModeRead				= 0x16,				// CH1 TRIGGER MODE READ, 8BIT
	HpcCh1TriggerModeWrite				= 0x96,				// CH1 RIGGER MODE WRITE
	HpcCh1TriggerStatusRead				= 0x17,				// CH1 TRIGGER STATUS READ, 8BIT
	HpcCh1NoOperation_97				= 0x97,				// Reserved.
	HpcCh1NoOperation_18				= 0x17,				// Reserved.
	HpcCh1TriggerEnable					= 0x98,				// CH1 TRIGGER ENABLE.
	HpcCh1NoOperation_19				= 0x19,				// Reserved.
	HpcCh1TriggerDisable				= 0x99,				// CH1 TRIGGER DISABLE.
	HpcCh1TimeTriggerFrequencyRead		= 0x1A,				// CH1 TRIGGER FREQUNCE INFO. WRITE, 28BIT
	HpcCh1TimeTriggerFrequencyWrite		= 0x9A,				// CH1 TRIGGER FREQUNCE INFO. READ
	HpcCh1Comparator1ValueRead			= 0x1B,				// CH1 COMPAREATOR1 READ, 31BIT
	HpcCh1Comparator1ValueWrite			= 0x9B,				// CH1 COMPAREATOR1 WRITE, 31BIT
	HpcCh1Comparator2ValueRead			= 0x1C,				// CH1 COMPAREATOR2 READ, 31BIT
	HpcCh1Comparator2ValueWrite			= 0x9C,				// CH1 COMPAREATOR2 WRITE, 31BIT
	HpcCh1CompareatorConditionRead		= 0x1D,				// CH1 COMPAREATOR CONDITION READ, 4BIT
	HpcCh1CompareatorConditionWrite		= 0x9D,				// CH1 COMPAREATOR CONDITION WRITE, 4BIT
	HpcCh1AbsTriggerTopPositionRead		= 0x1E,				// CH1 ABS TRIGGER POSITION READ, 31BIT
	HpcCh1AbsTriggerPositionWrite		= 0x9E,				// CH1 ABS TRIGGER POSITION WRITE, 31BIT
	HpcCh1AbsTriggerFifoStatusRead		= 0x1F,				// CH1 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
	HpcCh1AbsTriggerPositionClear		= 0x9F,				// CH1 ABS TRIGGER POSITION FIFO CLEAR

	// CH-2 Group Register
	HpcCh2CounterRead					= 0x20,				// CH2 COUNTER READ, 32BIT
	HpcCh2CounterWrite					= 0xA0,				// CH2 COUNTER WRITE, 32BIT
	HpcCh2CounterModeRead				= 0x21,				// CH2 COUNTER MODE READ, 4BIT
	HpcCh2CounterModeWrite				= 0xA1,				// CH2 COUNTER MODE WRITE, 4BIT
	HpcCh2TriggerRegionLowerDataRead	= 0x22,				// CH2 TRIGGER REGION LOWER DATA READ, 31BIT
	HpcCh2TriggerRegionLowerDataWrite	= 0xA2,				// CH2 TRIGGER REGION LOWER DATA WRITE
	HpcCh2TriggerRegionUpperDataRead	= 0x23,				// CH2 TRIGGER REGION UPPER DATA READ, 31BIT
	HpcCh2TriggerRegionUpperDataWrite	= 0xA3,				// CH2 TRIGGER REGION UPPER DATA WRITE
	HpcCh2TriggerPeriodRead				= 0x24,				// CH2 TRIGGER PERIOD READ, 31BIT
	HpcCh2TriggerPeriodWrite			= 0xA4,				// CH2 TRIGGER PERIOD WRITE
	HpcCh2TriggerPulseWidthRead			= 0x25,				// CH2 TRIGGER PULSE WIDTH READ, 31BIT
	HpcCh2TriggerPulseWidthWrite		= 0xA5,				// CH2 RIGGER PULSE WIDTH WRITE
	HpcCh2TriggerModeRead				= 0x26,				// CH2 TRIGGER MODE READ, 8BIT
	HpcCh2TriggerModeWrite				= 0xA6,				// CH2 RIGGER MODE WRITE
	HpcCh2TriggerStatusRead				= 0x27,				// CH2 TRIGGER STATUS READ, 8BIT
	HpcCh2NoOperation_97				= 0xA7,				// Reserved.
	HpcCh2NoOperation_18				= 0x27,				// Reserved.
	HpcCh2TriggerEnable					= 0xA8,				// CH2 TRIGGER ENABLE.
	HpcCh2NoOperation_19				= 0x29,				// Reserved.
	HpcCh2TriggerDisable				= 0xA9,				// CH2 TRIGGER DISABLE.
	HpcCh2TimeTriggerFrequencyRead		= 0x2A,				// CH2 TRIGGER FREQUNCE INFO. WRITE, 28BIT
	HpcCh2TimeTriggerFrequencyWrite		= 0xAA,				// CH2 TRIGGER FREQUNCE INFO. READ
	HpcCh2Comparator1ValueRead			= 0x2B,				// CH2 COMPAREATOR1 READ, 31BIT
	HpcCh2Comparator1ValueWrite			= 0xAB,				// CH2 COMPAREATOR1 WRITE, 31BIT
	HpcCh2Comparator2ValueRead			= 0x2C,				// CH2 COMPAREATOR2 READ, 31BIT
	HpcCh2Comparator2ValueWrite			= 0xAC,				// CH2 COMPAREATOR2 WRITE, 31BIT
	HpcCh2CompareatorConditionRead		= 0x2D,				// CH2 COMPAREATOR CONDITION READ, 4BIT
	HpcCh2CompareatorConditionWrite		= 0xAD,				// CH2 COMPAREATOR CONDITION WRITE, 4BIT
	HpcCh2AbsTriggerTopPositionRead		= 0x2E,				// CH2 ABS TRIGGER POSITION READ, 31BIT
	HpcCh2AbsTriggerPositionWrite		= 0xAE,				// CH2 ABS TRIGGER POSITION WRITE, 31BIT
	HpcCh2AbsTriggerFifoStatusRead		= 0x2F,				// CH2 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
	HpcCh2AbsTriggerPositionClear		= 0xAF,				// CH2 ABS TRIGGER POSITION FIFO CLEAR

	// CH-3 Group Register
	HpcCh3CounterRead					= 0x30,				// CH3 COUNTER READ, 32BIT
	HpcCh3CounterWrite					= 0xB0,				// CH3 COUNTER WRITE, 32BIT
	HpcCh3CounterModeRead				= 0x31,				// CH3 COUNTER MODE READ, 4BIT
	HpcCh3CounterModeWrite				= 0xB1,				// CH3 COUNTER MODE WRITE, 4BIT
	HpcCh3TriggerRegionLowerDataRead	= 0x32,				// CH3 TRIGGER REGION LOWER DATA READ, 31BIT
	HpcCh3TriggerRegionLowerDataWrite	= 0xB2,				// CH3 TRIGGER REGION LOWER DATA WRITE
	HpcCh3TriggerRegionUpperDataRead	= 0x33,				// CH3 TRIGGER REGION UPPER DATA READ, 31BIT
	HpcCh3TriggerRegionUpperDataWrite	= 0xB3,				// CH3 TRIGGER REGION UPPER DATA WRITE
	HpcCh3TriggerPeriodRead				= 0x34,				// CH3 TRIGGER PERIOD READ, 31BIT
	HpcCh3TriggerPeriodWrite			= 0xB4,				// CH3 TRIGGER PERIOD WRITE
	HpcCh3TriggerPulseWidthRead			= 0x35,				// CH3 TRIGGER PULSE WIDTH READ, 31BIT
	HpcCh3TriggerPulseWidthWrite		= 0xB5,				// CH3 RIGGER PULSE WIDTH WRITE
	HpcCh3TriggerModeRead				= 0x36,				// CH3 TRIGGER MODE READ, 8BIT
	HpcCh3TriggerModeWrite				= 0xB6,				// CH3 RIGGER MODE WRITE
	HpcCh3TriggerStatusRead				= 0x37,				// CH3 TRIGGER STATUS READ, 8BIT
	HpcCh3NoOperation_97				= 0xB7,				// Reserved.
	HpcCh3NoOperation_18				= 0x37,				// Reserved.
	HpcCh3TriggerEnable					= 0xB8,				// CH3 TRIGGER ENABLE.
	HpcCh3NoOperation_19				= 0x39,				// Reserved.
	HpcCh3TriggerDisable				= 0xB9,				// CH3 TRIGGER DISABLE.
	HpcCh3TimeTriggerFrequencyRead		= 0x3A,				// CH3 TRIGGER FREQUNCE INFO. WRITE, 28BIT
	HpcCh3TimeTriggerFrequencyWrite		= 0xBA,				// CH3 TRIGGER FREQUNCE INFO. READ
	HpcCh3Comparator1ValueRead			= 0x3B,				// CH3 COMPAREATOR1 READ, 31BIT
	HpcCh3Comparator1ValueWrite			= 0xBB,				// CH3 COMPAREATOR1 WRITE, 31BIT
	HpcCh3Comparator2ValueRead			= 0x3C,				// CH3 COMPAREATOR2 READ, 31BIT
	HpcCh3Comparator2ValueWrite			= 0xBC,				// CH3 COMPAREATOR2 WRITE, 31BIT
	HpcCh3CompareatorConditionRead		= 0x3D,				// CH3 COMPAREATOR CONDITION READ, 4BIT
	HpcCh3CompareatorConditionWrite		= 0xBD,				// CH3 COMPAREATOR CONDITION WRITE, 4BIT
	HpcCh3AbsTriggerTopPositionRead		= 0x3E,				// CH3 ABS TRIGGER POSITION READ, 31BIT
	HpcCh3AbsTriggerPositionWrite		= 0xBE,				// CH3 ABS TRIGGER POSITION WRITE, 31BIT
	HpcCh3AbsTriggerFifoStatusRead		= 0x3F,				// CH3 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
	HpcCh3AbsTriggerPositionClear		= 0xBF,				// CH3 ABS TRIGGER POSITION FIFO CLEAR

	// CH-4 Group Register
	HpcCh4CounterRead					= 0x40,				// CH4 COUNTER READ, 32BIT
	HpcCh4CounterWrite					= 0xC0,				// CH4 COUNTER WRITE, 32BIT
	HpcCh4CounterModeRead				= 0x41,				// CH4 COUNTER MODE READ, 4BIT
	HpcCh4CounterModeWrite				= 0xC1,				// CH4 COUNTER MODE WRITE, 4BIT
	HpcCh4TriggerRegionLowerDataRead	= 0x42,				// CH4 TRIGGER REGION LOWER DATA READ, 31BIT
	HpcCh4TriggerRegionLowerDataWrite	= 0xC2,				// CH4 TRIGGER REGION LOWER DATA WRITE
	HpcCh4TriggerRegionUpperDataRead	= 0x43,				// CH4 TRIGGER REGION UPPER DATA READ, 31BIT
	HpcCh4TriggerRegionUpperDataWrite	= 0xC3,				// CH4 TRIGGER REGION UPPER DATA WRITE
	HpcCh4TriggerPeriodRead				= 0x44,				// CH4 TRIGGER PERIOD READ, 31BIT
	HpcCh4TriggerPeriodWrite			= 0xC4,				// CH4 TRIGGER PERIOD WRITE
	HpcCh4TriggerPulseWidthRead			= 0x45,				// CH4 TRIGGER PULSE WIDTH READ, 31BIT
	HpcCh4TriggerPulseWidthWrite		= 0xC5,				// CH4 RIGGER PULSE WIDTH WRITE
	HpcCh4TriggerModeRead				= 0x46,				// CH4 TRIGGER MODE READ, 8BIT
	HpcCh4TriggerModeWrite				= 0xC6,				// CH4 RIGGER MODE WRITE
	HpcCh4TriggerStatusRead				= 0x47,				// CH4 TRIGGER STATUS READ, 8BIT
	HpcCh4NoOperation_97				= 0xC7,				// Reserved.
	HpcCh4NoOperation_18				= 0x47,				// Reserved.
	HpcCh4TriggerEnable					= 0xC8,				// CH4 TRIGGER ENABLE.
	HpcCh4NoOperation_19				= 0x49,				// Reserved.
	HpcCh4TriggerDisable				= 0xC9,				// CH4 TRIGGER DISABLE.
	HpcCh4TimeTriggerFrequencyRead		= 0x4A,				// CH4 TRIGGER FREQUNCE INFO. WRITE, 28BIT
	HpcCh4TimeTriggerFrequencyWrite		= 0xCA,				// CH4 TRIGGER FREQUNCE INFO. READ
	HpcCh4Comparator1ValueRead			= 0x4B,				// CH4 COMPAREATOR1 READ, 31BIT
	HpcCh4Comparator1ValueWrite			= 0xCB,				// CH4 COMPAREATOR1 WRITE, 31BIT
	HpcCh4Comparator2ValueRead			= 0x4C,				// CH4 COMPAREATOR2 READ, 31BIT
	HpcCh4Comparator2ValueWrite			= 0xCC,				// CH4 COMPAREATOR2 WRITE, 31BIT
	HpcCh4CompareatorConditionRead		= 0x4D,				// CH4 COMPAREATOR CONDITION READ, 4BIT
	HpcCh4CompareatorConditionWrite		= 0xCD,				// CH4 COMPAREATOR CONDITION WRITE, 4BIT
	HpcCh4AbsTriggerTopPositionRead		= 0x4E,				// CH4 ABS TRIGGER POSITION READ, 31BIT
	HpcCh4AbsTriggerPositionWrite		= 0xCE,				// CH4 ABS TRIGGER POSITION WRITE, 31BIT
	HpcCh4AbsTriggerFifoStatusRead		= 0x4F,				// CH4 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
	HpcCh4AbsTriggerPositionClear		= 0xCF,				// CH4 ABS TRIGGER POSITION FIFO CLEAR

	// Ram Access Group Register
	HpcRamDataWithRamAddress			= 0x51,				// READ RAM DATA WITH RAM ADDR PORT ADDRESS
	HpcRamDataWrite						= 0xD0,				// RAM DATA WRITE
	HpcRamDataRead						= 0x50,				// RAM DATA READ, 32BIT

	// Debugging Registers	
	HpcCh1TrigPosIndexRead				= 0x60,				// CH1 Current RAM trigger index position on 32Bit data, 8BIT
	HpcCh1TrigBackwardDataRead			= 0x61,				// CH1 Current RAM trigger backward position data, 32BIT
	HpcCh1TrigCurrentDataRead			= 0x62,				// CH1 Current RAM trigger current position data, 32BIT
	HpcCh1TrigForwardDataRead			= 0x63,				// CH1 Current RAM trigger next position data, 32BIT
	HpcCh1TrigRamAddressRead			= 0x64,				// CH1 Current RAM trigger address, 20BIT

	HpcCh2TrigPosIndexRead				= 0x65,				// CH2 Current RAM trigger index position on 32Bit data, 8BIT
	HpcCh2TrigBackwardDataRead			= 0x66,				// CH2 Current RAM trigger backward position data, 32BIT
	HpcCh2TrigCurrentDataRead			= 0x67,				// CH2 Current RAM trigger current position data, 32BIT
	HpcCh2TrigForwardDataRead			= 0x68,				// CH2 Current RAM trigger next position data, 32BIT
	HpcCh2TrigRamAddressRead			= 0x69,				// CH2 Current RAM trigger address, 20BIT

	HpcCh3TrigPosIndexRead				= 0x70,				// CH3 Current RAM trigger index position on 32Bit data, 8BIT
	HpcCh3TrigBackwardDataRead			= 0x71,				// CH3 Current RAM trigger backward position data, 32BIT
	HpcCh3TrigCurrentDataRead			= 0x72,				// CH3 Current RAM trigger current position data, 32BIT
	HpcCh3TrigForwardDataRead			= 0x73,				// CH3 Current RAM trigger next position data, 32BIT
	HpcCh3TrigRamAddressRead			= 0x74,				// CH3 Current RAM trigger address, 20BIT

	HpcCh4TrigPosIndexRead				= 0x75,				// CH4 Current RAM trigger index position on 32Bit data, 8BIT
	HpcCh4TrigBackwardDataRead			= 0x76,				// CH4 Current RAM trigger backward position data, 32BIT
	HpcCh4TrigCurrentDataRead			= 0x77,				// CH4 Current RAM trigger current position data, 32BIT
	HpcCh4TrigForwardDataRead			= 0x78,				// CH4 Current RAM trigger next position data, 32BIT
	HpcCh4TrigRamAddressRead			= 0x79,				// CH4 Current RAM trigger address, 20BIT

	HpcCh1TestEnable					= 0x81,				// CH1 test enable(Manufacturer only)
	HpcCh2TestEnable					= 0x82,				// CH2 test enable(Manufacturer only)
	HpcCh3TestEnable					= 0x83,				// CH3 test enable(Manufacturer only)
	HpcCh4TestEnable					= 0x84,				// CH4 test enable(Manufacturer only)

	HpcTestFrequency					= 0x8C,				// Test counter output frequency(32bit)
	HpcTestCountStart					= 0x8D,				// Start test counter output with position(32bit signed).
	HpcTestCountEnd						= 0x8E,				// End counter output.

} HPCCOMMAND;
#endif
                                                                    
typedef void (__stdcall *AXT_EVENT_PROC)(long lActiveNo, DWORD uFlag);
typedef void (__stdcall *AXT_INTERRUPT_PROC)(long lActiveNo, DWORD uFlag);

#endif    //__AXT_AXHS_H__
