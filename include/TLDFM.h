/**************************************************************************//**
 *
 *	\file          TLDFM.h
 *	\ingroup       TLDFM
 *	\brief         Thorlabs TLDFM
 *
 *                 Thorlabs GmbH - TLDFM -
 *
 *	\date          Jun 6, 2019
 *	\copyright     Copyright© 2019 Thorlabs GmbH. All Rights Reserved.
 *	\author        cwestphal (cwestphal@thorlabs.com)
 *
 ******************************************************************************/
#ifndef INCL_TLDFM_H_
#define INCL_TLDFM_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C"
{
#endif

//==============================================================================
// Include Files
#include "TLDFM_def.h"

//==============================================================================
// Global functions
TLDFM_API ViStatus _VI_FUNC TLDFM_get_device_count (ViSession instrumentHandle ,
                                                    ViPUInt32 pDeviceCount);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_device_information (ViSession instrumentHandle ,
                                                          ViUInt32 deviceIndex ,
                                                          ViPChar manufacturer ,
                                                          ViPChar instrumentName ,
                                                          ViPChar serialNumber ,
                                                          ViPBoolean pDeviceAvailable ,
                                                          ViPChar resourceName);

//******************************************************************************
// required VXIpnp instrument driver functions

TLDFM_API ViStatus _VI_FUNC TLDFM_init (ViRsrc     resourceName ,
                                        ViBoolean  IDQuery ,
                                        ViBoolean  resetDevice ,
                                        ViPSession pInstrumentHandle);

TLDFM_API ViStatus _VI_FUNC TLDFM_close (ViSession  instrumentHandle);

TLDFM_API ViStatus _VI_FUNC TLDFM_reset (ViSession  instrumentHandle);

TLDFM_API ViStatus _VI_FUNC TLDFM_self_test (ViSession  instrumentHandle,
                                             ViPInt16   pSelfTestResult,
                                             ViPChar    selfTestMessage);

TLDFM_API ViStatus _VI_FUNC TLDFM_error_query (ViSession  instrumentHandle,
                                               ViPStatus   pErrorCode,
                                               ViPChar     errorMessage);

TLDFM_API ViStatus _VI_FUNC TLDFM_error_message (ViSession  instrumentHandle,
                                                 ViStatus   errorCode,
                                                 ViPChar     errorMessage);

TLDFM_API ViStatus _VI_FUNC TLDFM_revision_query (ViSession  instrumentHandle,
                                                  ViPChar     driverRevision,
                                                  ViPChar     firmwareRevision);

//******************************************************************************

TLDFM_API ViStatus _VI_FUNC TLDFM_set_USB_access_mode (ViSession instrumentHandle,
                                                       ViUInt32  accessMode,
                                                       ViString  requestedKey,
                                                       ViChar    accessKey[]);

//******************************************************************************
// segment voltage functions

TLDFM_API ViStatus _VI_FUNC TLDFM_get_segment_voltage (ViSession instrumentHandle,
                                                       ViUInt32  segmentIndex,
                                                       ViReal64* pSegmentVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_segment_voltage (ViSession instrumentHandle,
                                                       ViUInt32  segmentIndex,
                                                       ViReal64  segmentVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_segment_voltages (ViSession instrumentHandle,
                                                       ViReal64  segmentVoltages[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_segment_voltages (ViSession instrumentHandle,
                                                       ViReal64  segmentVoltages[]);

//******************************************************************************
// tilt element voltage functions

TLDFM_API ViStatus _VI_FUNC TLDFM_get_tilt_voltage (ViSession instrumentHandle,
                                                    ViUInt32  tiltIndex,
                                                    ViReal64* pTiltVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_tilt_voltage (ViSession instrumentHandle,
                                                    ViUInt32  tiltIndex,
                                                    ViReal64  tiltVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_tilt_voltages (ViSession instrumentHandle,
                                                     ViReal64  tiltVoltages[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_tilt_voltages (ViSession instrumentHandle,
                                                     ViReal64  tiltVoltages[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_tilt_amplitude_angle (ViSession instrumentHandle,
                                                            ViReal64  amplitute,
                                                            ViReal64  angle);

//******************************************************************************
// convenience voltage functions
// sets the voltages for all segments and all tilt elements

TLDFM_API ViStatus _VI_FUNC TLDFM_get_voltages (ViSession instrumentHandle,
                                                ViReal64  segmentVoltages[],
                                                ViReal64  tiltVoltages[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_voltages (ViSession instrumentHandle,
                                                ViReal64  segmentVoltages[],
                                                ViReal64  tiltVoltages[]);

//******************************************************************************
// device information functions

TLDFM_API ViStatus _VI_FUNC TLDFM_get_manufacturer_name (ViSession instrumentHandle,
                                                         ViChar    manufacturerName[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_instrument_name (ViSession instrumentHandle,
                                                       ViChar    instrName[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_instrument_name (ViSession instrumentHandle,
                                                       ViChar    instrName[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_serial_Number (ViSession instrumentHandle,
                                                     ViChar    serialNumber[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_serial_number (ViSession instrumentHandle,
                                                     ViChar    serialNumber[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_user_text (ViSession instrumentHandle,
                                                 ViChar    userText[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_user_text (ViSession instrumentHandle,
                                                 ViChar    userText[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_update_firmware (ViSession instrumentHandle,
                                                   ViChar    firmwareFile[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_enable_event (ViSession instrumentHandle);

TLDFM_API ViStatus _VI_FUNC TLDFM_disable_event (ViSession instrumentHandle);

TLDFM_API ViStatus _VI_FUNC TLDFM_add_status_delegate (ViSession    instrumentHandle,
	                                                   StatusUpdate statusUpdateDelegate);

//******************************************************************************

TLDFM_API ViStatus _VI_FUNC TLDFM_get_segment_count (ViSession instrumentHandle,
                                                     ViUInt32* pSegmentCount);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_segment_maximum (ViSession instrumentHandle,
                                                       ViReal64* pSegmentMaximum);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_segment_minimum (ViSession instrumentHandle,
                                                       ViReal64* pSegmentMinimum);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_tilt_count (ViSession instrumentHandle,
                                                  ViUInt32* pTiltCount);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_tilt_maximum (ViSession instrumentHandle,
                                                    ViReal64* pTiltMaximum);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_tilt_minimum (ViSession instrumentHandle,
                                                    ViReal64* pTiltMinimum);

//******************************************************************************

TLDFM_API ViStatus _VI_FUNC TLDFM_set_access_level (ViSession instrumentHandle,
                                                    ViChar    accessLevel,
                                                    ViChar    accessPassword[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_device_configuration (ViSession instrumentHandle,
                                                            ViUInt32* pSegmentCnt,
                                                            ViReal64* pMinSegmentVoltage,
                                                            ViReal64* pMaxSegmentVoltage,
                                                            ViReal64* pSegmentCommonVoltageMax,
                                                            ViUInt32* pTiltElementCnt,
                                                            ViReal64* pMinTiltVoltage,
                                                            ViReal64* pMaxTiltVoltage,
                                                            ViReal64* pTiltCommonVoltageMax);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_device_configuration (ViSession instrumentHandle,
                                                            ViUInt32  segmentCnt,
                                                            ViReal64  minSegmentVoltage,
                                                            ViReal64  maxSegmentVoltage,
                                                            ViReal64  segmentCommonVoltageMax,
                                                            ViUInt32  tiltElementCnt,
                                                            ViReal64  minTiltVoltage,
                                                            ViReal64  maxTiltVoltage,
                                                            ViReal64  tiltCommonVoltageMax);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_device_calibration (ViSession instrumentHandle,
                                                          ViReal64* pSegmentVoltageCompensation,
                                                          ViReal64* pTiltVoltageCompensation,
                                                          ViReal64* pSingleSegmentTiltVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_device_calibration (ViSession instrumentHandle,
                                                          ViReal64  segmentVoltageCompensation,
                                                          ViReal64  tiltVoltageCompensation,
                                                          ViReal64  singleSegmentTiltVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_hysteresis_parameters (ViSession instrumentHandle,
                                                             ViUInt32  target,
                                                             ViUInt32* pCount,
                                                             ViReal64* pNonlinearFactor,
                                                             ViReal64  arrayTresholdInverter[],
                                                             ViReal64  arrayWeightInverter[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_set_hysteresis_parameters (ViSession instrumentHandle,
                                                             ViUInt32  target,
                                                             ViUInt32  count,
                                                             ViReal64  nonlinearFactor,
                                                             ViReal64  arrayTresholdInverter[],
                                                             ViReal64  arrayWeightInverter[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_enabled_hysteresis_compensation (ViSession  instrumentHandle,
                                                                   ViUInt32   target,
                                                                   ViBoolean* pEnabled);

TLDFM_API ViStatus _VI_FUNC TLDFM_enable_hysteresis_compensation (ViSession instrumentHandle,
                                                                  ViUInt32  target,
                                                                  ViBoolean enable);

//******************************************************************************

TLDFM_API ViStatus _VI_FUNC TLDFM_get_measured_segment_voltage (ViSession instrumentHandle,
                                                                ViUInt32  segmentIndex,
                                                                ViReal64* pSegmentVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_measured_segment_voltages (ViSession instrumentHandle,
                                                                 ViReal64  segmentVoltages[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_measured_tilt_voltage (ViSession instrumentHandle,
                                                             ViUInt32  tiltIndex,
                                                             ViReal64* pTiltVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_measured_tilt_voltages (ViSession instrumentHandle,
                                                              ViReal64  tiltVoltages[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_measured_voltages (ViSession instrumentHandle,
                                                         ViReal64  segmentVoltages[],
                                                         ViReal64  tiltVoltages[]);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_feedback_voltage (ViSession instrumentHandle,
                                                        ViReal64* pFeedbackVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_feedback_current (ViSession instrumentHandle,
                                                        ViReal64* pFeedbackCurrent);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_feedback (ViSession instrumentHandle,
                                                ViReal64* pFeedbackVoltage,
                                                ViReal64* pFeedbackCurrent);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_monitor_voltage (ViSession instrumentHandle,
                                                       ViReal64* pMonitorVoltage);

TLDFM_API ViStatus _VI_FUNC TLDFM_get_temperatures (ViSession instrumentHandle,
                                                    ViReal64* pIC1Temperatur,
                                                    ViReal64* pIC2Temperatur,
                                                    ViReal64* pMirrorTemperatur,
                                                    ViReal64* pElectronicTemperatur);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif /* INCL_TLDFM_H_ */
