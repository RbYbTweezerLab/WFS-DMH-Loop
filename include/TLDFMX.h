/**************************************************************************//**
 *
 *	\file          TLDFMX.h
 *	\ingroup       TLDFMX
 *	\brief         Thorlabs TLDFMX
 *
 *  \details       TODO Auto-generated File-Brief-Details
 *
 *	\date          Jul 1, 2019
 *	\copyright     Copyright© 2019 Thorlabs GmbH. All Rights Reserved.
 *	\author        cwestphal (cwestphal@thorlabs.com)
 *
 ******************************************************************************/
#ifndef INCL_TLDFMX_H_
#define INCL_TLDFMX_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C"
{
#endif

//==============================================================================
// Include files
#include "TLDFMX_def.h"

//==============================================================================
// Global functions

TLDFMX_API ViStatus _VI_FUNC TLDFMX_init (ViRsrc     resourceName ,
                                          ViBoolean  IDQuery ,
                                          ViBoolean  resetDevice ,
                                          ViPSession pInstrumentHandle);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_close (ViSession instrumentHandle);

//==============================================================================
// Utility Functions

TLDFMX_API ViStatus _VI_FUNC TLDFMX_reset (ViSession instrumentHandle);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_self_test (ViSession instrumentHandle ,
                                               ViPInt16  pSelfTestResult ,
                                               ViPChar   selfTestMessage);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_error_query (ViSession instrumentHandle ,
                                                 ViPInt32  pErrorCode ,
                                                 ViPChar   errorMessage);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_error_message (ViSession instrumentHandle ,
                                                   ViStatus errorCode ,
                                                   ViPChar errorMessage);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_revision_query (ViSession instrumentHandle ,
                                                    ViPChar instrumentDriverRevision ,
                                                    ViPChar firmwareRevision);

//==============================================================================
// Action/Status Functions

TLDFMX_API ViStatus _VI_FUNC TLDFMX_relax (ViSession instrumentHandle ,
                                           ViUInt32 devicePart ,
                                           ViBoolean isFirstStep ,
                                           ViBoolean reload ,
                                           ViReal64 relaxPatternMirror[] ,
                                           ViReal64 relaxPatternArms[] ,
                                           ViInt32 * pRemainingSteps);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_measure_system_parameters (ViSession instrumentHandle ,
                                                               ViBoolean isFirstStep ,
                                                               ViReal32 measuredZernikeAmplitudes[] ,
                                                               ViReal64 nextMirrorPattern[] ,
                                                               ViInt32 * pRemainingSteps);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_get_flat_wavefront (ViSession instrumentHandle ,
                                                        TLDFMX_zernikes_t flatZernikes ,
                                                        ViReal32 measuredZernikeAmplitudes[] ,
                                                        ViReal64 deviceZernikeAmplitudes[] ,
                                                        ViReal64 voltagePattern[]);

//==============================================================================
// Configuration Functions

TLDFMX_API ViStatus _VI_FUNC TLDFMX_set_single_voltage_setpoint (ViSession instrumentHandle ,
                                                                 ViInt32 segmentID ,
                                                                 ViReal64 voltage);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_set_voltages_setpoint (ViSession instrumentHandle ,
                                                           ViReal64 mirrorVoltages[]);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_calculate_single_zernike_pattern (ViSession instrumentHandle ,
                                                                      TLDFMX_zernike_flag_t zernike ,
                                                                      ViReal64 deviceZernikeAmplitude ,
                                                                      ViReal64 mirrorPattern[]);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_calculate_zernike_pattern (ViSession instrumentHandle ,
                                                               TLDFMX_zernikes_t zernikes ,
                                                               ViReal64 deviceZernikeAmplitudes[] ,
                                                               ViReal64 mirrorPattern[]);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_set_flip_mode (ViSession instrumentHandle ,
                                                   TLDFMX_flip_t flipMode);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_get_flip_mode (ViSession instrumentHandle ,
                                                   TLDFMX_flip_t * pFlipMode);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_set_rotation_mode (ViSession instrumentHandle ,
                                                       TLDFMX_rotation_t rotation);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_get_rotation_mode (ViSession instrumentHandle ,
                                                       TLDFMX_rotation_t * pRotationMode);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_set_pattern_range_check (ViSession instrumentHandle ,
                                                             ViBoolean enable);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_get_pattern_range_check (ViSession instrumentHandle ,
                                                             ViBoolean * pEnabled);

//==============================================================================
// Data Functions

TLDFMX_API ViStatus _VI_FUNC TLDFMX_get_parameters (ViSession instrumentHandle ,
                                                    ViReal64 * pMinimumZernikeAmplitude ,
                                                    ViReal64 * pMaximumZernikeAmplitude ,
                                                    ViInt32 * pZernikeCount ,
                                                    ViInt32 * pMeasurementSteps ,
                                                    ViInt32 * pRelaxSteps);

TLDFMX_API ViStatus _VI_FUNC TLDFMX_get_system_parameters (ViSession instrumentHandle ,
                                                           TLDFMX_rotation_t * pRotationMode ,
                                                           TLDFMX_flip_t * pFlipMode ,
                                                           ViReal64 maxDeviceZernikeAmplitudes[] ,
                                                           ViBoolean * isDataValid);

//==============================================================================
// Helper Functions

TLDFMX_API ViStatus _VI_FUNC TLDFMX_convert_measured_zernike_amplitudes (ViSession instrumentHandle ,
                                                                         ViReal32 measuredZernikeAmplitudes[] ,
                                                                         ViReal64 deviceZernikeAmplitudes[]);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif /* INCL_TLDFMX_H_ */
