/*===============================================================================================================================
  Include Files

  Note: You may need to set your compilers include search path to the VXIPNP include directory.
		  This is typically 'C:\Program Files (x86)\IVI Foundation\VISA\WinNT\WFS'.

===============================================================================================================================*/

#include "include/WFS.h" // Wavefront Sensor driver's header file
#include "include/TLDFMX.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>



/*===============================================================================================================================
  Defines
===============================================================================================================================*/

#define  DEVICE_OFFSET_WFS10           (0x00100) // device IDs of WFS10 instruments start at 256 decimal
#define  DEVICE_OFFSET_WFS20           (0x00200) // device IDs of WFS20 instruments start at 512 decimal
#define  DEVICE_OFFSET_WFS30           (0x00400) // device IDs of WFS30 instruments start at 1024 decimal
#define  DEVICE_OFFSET_WFS40           (0x00800) // device IDs of WFS40 instruments start at 2048 decimal

// settings for this sample program, you may adapt settings to your preferences
#define  OPTION_OFF                    (0)
#define  OPTION_ON                     (1)

#define  SAMPLE_PIXEL_FORMAT           PIXEL_FORMAT_MONO8   // only 8 bit format is supported
#define  SAMPLE_CAMERA_RESOL_WFS       CAM_RES_768          // 768x768 pixels, see wfs.h for alternative cam resolutions
#define  SAMPLE_CAMERA_RESOL_WFS10     CAM_RES_WFS10_360    // 360x360 pixels
#define  SAMPLE_CAMERA_RESOL_WFS20     CAM_RES_WFS20_512    // 512x512 pixels
#define  SAMPLE_CAMERA_RESOL_WFS30     CAM_RES_WFS30_512    // 512x512 pixels
#define  SAMPLE_CAMERA_RESOL_WFS40     CAM_RES_WFS40_512    // 512x512 pixels
#define  SAMPLE_REF_PLANE              WFS_REF_INTERNAL

#define  SAMPLE_PUPIL_CENTROID_X       (0.0) // in mm
#define  SAMPLE_PUPIL_CENTROID_Y       (0.0)
#define  SAMPLE_PUPIL_DIAMETER_X       (2.0) // in mm, needs to fit to selected camera resolution
#define  SAMPLE_PUPIL_DIAMETER_Y       (2.0)

#define  SAMPLE_IMAGE_READINGS         (10) // trials to read a exposed spotfield image

#define  SAMPLE_OPTION_DYN_NOISE_CUT   OPTION_ON   // use dynamic noise cut features  
#define  SAMPLE_OPTION_CALC_SPOT_DIAS  OPTION_OFF  // don't calculate spot diameters
#define  SAMPLE_OPTION_CANCEL_TILT     OPTION_ON   // cancel average wavefront tip and tilt
#define  SAMPLE_OPTION_LIMIT_TO_PUPIL  OPTION_OFF  // don't limit wavefront calculation to pupil interior

#define  SAMPLE_OPTION_HIGHSPEED       OPTION_ON   // use highspeed mode (only for WFS10 and WFS20 instruments)
#define  SAMPLE_OPTION_HS_ADAPT_CENTR  OPTION_ON   // adapt centroids in highspeed mode to previously measured centroids
#define  SAMPLE_HS_NOISE_LEVEL         (30)        // cut lower 30 digits in highspeed mode
#define  SAMPLE_HS_ALLOW_AUTOEXPOS     (1)         // allow autoexposure in highspeed mode (runs somewhat slower)

#define  SAMPLE_WAVEFRONT_TYPE         WAVEFRONT_MEAS // calculate measured wavefront

#define  SAMPLE_ZERNIKE_ORDERS         (3)  // calculate up to 3rd Zernike order

#define  SAMPLE_PRINTOUT_SPOTS         (5)  // printout results for first 5 x 5 spots only

#define  SAMPLE_OUTPUT_FILE_NAME       "WFS_sample_output.txt"

#define VAL_ESC_VKEY                   (3L << 8)

#define  MAX_SEGMENTS	(40)

typedef struct
{
	ViUInt8 firstHighByte;
	ViUInt8 firstLowByte;
	ViUInt8 secondHighByte;
	ViUInt8 secondLowByte;
} SAMPLE_dword_converter_t;

typedef union
{
	SAMPLE_dword_converter_t byteRepresentation;
	ViUInt32                 dwordRepresentation;
} SAMPLE_dword_t;

typedef struct{
	long unsigned int*	WFS_handle;
	ViSession* handle;
	float* target;
	int*	thflag;
} threadArgs;

/*=============================================================================
 Callbacks
=============================================================================*/
ViStatus _VI_FUNCH StatusCallback(ViChar[]);

/*===============================================================================================================================
  Data type definitions
===============================================================================================================================*/
typedef struct
{
	int               selected_id;
	long int               handle;
	long int               status;
	
	char              version_wfs_driver[WFS_BUFFER_SIZE];
	char              version_cam_driver[WFS_BUFFER_SIZE];
	char              manufacturer_name[WFS_BUFFER_SIZE];
	char              instrument_name[WFS_BUFFER_SIZE];
	char              serial_number_wfs[WFS_BUFFER_SIZE];
	char              serial_number_cam[WFS_BUFFER_SIZE];
	
	long int               mla_cnt;
	int               selected_mla;
	int               selected_mla_idx;
	char              mla_name[WFS_BUFFER_SIZE];
	double            cam_pitch_um;
	double            lenslet_pitch_um;
	double            center_spot_offset_x;
	double            center_spot_offset_y;
	double            lenslet_f_um;
	double            grd_corr_0;
	double            grd_corr_45;
	
	long int               spots_x;
	long int               spots_y;

}  instr_t;


/*===============================================================================================================================
  Function Prototypes
===============================================================================================================================*/
void handle_errors (int);
int select_instrument (int *selection, ViChar resourceName[]);
int select_mla (int *selection);

void waitKeypress (void);
void error_exit (ViSession handle, ViStatus err);
ViStatus select_instrument_DMH (ViChar** resource);

void get_Zernike_list (void);
void *Loop(void * Argstruct);

/*===============================================================================================================================
  Global Variables
===============================================================================================================================*/
const int   cam_wfs_xpixel[] = { 1280, 1024, 768, 512, 320 }; // WFS150/300
const int   cam_wfs_ypixel[] = { 1024, 1024, 768, 512, 320 };
const int   cam_wfs10_xpixel[] = {  640,  480, 360, 260, 180 };
const int   cam_wfs10_ypixel[] = {  480,  480, 360, 260, 180 };
const int   cam_wfs20_xpixel[] = {  1440, 1080, 768, 512, 360,  720, 540, 384, 256, 180 };
const int   cam_wfs20_ypixel[] = {  1080, 1080, 768, 512, 360,  540, 540, 384, 256, 180 };
const int   cam_wfs30_xpixel[] = {  1936, 1216, 1024, 768, 512, 360, 968, 608, 512, 384, 256, 180 };
const int   cam_wfs30_ypixel[] = {  1216, 1216, 1024, 768, 512, 360, 608, 608, 512, 384, 256, 180 };
const int   cam_wfs40_xpixel[] = {  2048, 1536, 1024, 768, 512, 360, 1024, 768, 512, 384, 256, 180 }; 
const int   cam_wfs40_ypixel[] = {  2048, 1536, 1024, 768, 512, 360, 1024, 768, 512, 384, 256, 180 }; 

const int   zernike_modes[] = { 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66 }; // converts Zernike order to Zernike modes

instr_t     instr = { 0 };    // all instrument related data are stored in this structure

int         hs_win_count_x,hs_win_count_y,hs_win_size_x,hs_win_size_y; // highspeed windows data
int         hs_win_start_x[MAX_SPOTS_X],hs_win_start_y[MAX_SPOTS_Y];
ViSession instrHdl = VI_NULL;
float	target_zernike[16];

/*===============================================================================================================================
  Code
===============================================================================================================================*/
void main (void)
{
	long int               err;
	int               i,j,cnt;
	int               rows, cols;   // image height and width, depending on camera resolution
	int               selection;
	unsigned char     *ImageBuffer; // pointer to the camera image buffer
	
	double            expos_act, master_gain_act;
	double            beam_centroid_x, beam_centroid_y;
	double            beam_diameter_x, beam_diameter_y;
	
	float             centroid_x[MAX_SPOTS_Y][MAX_SPOTS_X];
	float             centroid_y[MAX_SPOTS_Y][MAX_SPOTS_X];

	float             deviation_x[MAX_SPOTS_Y][MAX_SPOTS_X];
	float             deviation_y[MAX_SPOTS_Y][MAX_SPOTS_X];

	float             wavefront[MAX_SPOTS_Y][MAX_SPOTS_X];
	
	float             zernike_um[MAX_ZERNIKE_MODES+1];             // index runs from 1 - MAX_ZERNIKE_MODES
	float             zernike_orders_rms_um[MAX_ZERNIKE_ORDERS+1]; // index runs from 1 - MAX_ZERNIKE_MODES
	double            roc_mm;
	
	long int               zernike_order;
	
	double            wavefront_min, wavefront_max, wavefront_diff, wavefront_mean, wavefront_rms, wavefront_weighted_rms;
	ViChar            resourceName[256];
	FILE              *fp;
	int               key;
	int	thread_flag = 0;
	

	// ViStatus  err = VI_SUCCESS;
	
	ViChar    *rscPtr;
	ViInt32   zernikeCount, systemMeasurementSteps, relaxSteps;
	ViReal64  minZernikeAmplitude, maxZernikeAmplitude, voltage = 50.0;
	ViReal64  mirrorPattern[MAX_SEGMENTS];
	ViInt32 remainingSteps;
	ViReal64 nextMirrorPattern[50];
	
	for(ViInt32 i = 0; MAX_SEGMENTS > i; ++i)
	{
		mirrorPattern[i] = voltage;
	}
	
	// Show all and select one WFS instrument
	if(select_instrument(&instr.selected_id, resourceName) == 0)
	{
		printf("\nNo WFS selected. Press <ENTER> to exit.\n");
		fflush(stdin);
		getchar();
		return; // program ends here if no instrument selected
	}
	
    err = select_instrument_DMH(&rscPtr);
		if(VI_SUCCESS != err)
		{
			error_exit(instrHdl, err);  // Something went wrong
		}
		if(!rscPtr)
		{
			exit(EXIT_SUCCESS);     // None found
		}
	// Open the Wavefront Sensor instrument
	//if(err = WFS_init (instr.selected_id, &instr.handle))
	if(err = WFS_init (resourceName, VI_FALSE, VI_FALSE, &instr.handle)) 
		handle_errors(err);

    err = TLDFMX_init(rscPtr, VI_TRUE, VI_TRUE, &instrHdl);
	    if(err) error_exit(instrHdl, err);

    err = TLDFM_enable_hysteresis_compensation (instrHdl, 2, 1);
	    if(err) error_exit(instrHdl, err);

	// Select a microlens array (MLA)
	if(select_mla(&instr.selected_mla) < 0)
	{
		printf("\nNo MLA selected. Press <ENTER> to exit.\n");
		fflush(stdin);
		getchar();
		return;
	}
	
	// Activate desired MLA
	if(err = WFS_SelectMla (instr.handle, instr.selected_mla))
		handle_errors(err);

	
	
	// Configure WFS camera, use a pre-defined camera resolution
	if((instr.selected_id & DEVICE_OFFSET_WFS10) == 0 && (instr.selected_id & DEVICE_OFFSET_WFS20) == 0 && (instr.selected_id & DEVICE_OFFSET_WFS30) == 0 && (instr.selected_id & DEVICE_OFFSET_WFS40) == 0) // WFS150/300 instrument
	{   
		printf("\n\nConfigure WFS camera with resolution index %d (%d x %d pixels).\n", SAMPLE_CAMERA_RESOL_WFS, cam_wfs_xpixel[SAMPLE_CAMERA_RESOL_WFS], cam_wfs_ypixel[SAMPLE_CAMERA_RESOL_WFS]);
		
		if(err = WFS_ConfigureCam (instr.handle, SAMPLE_PIXEL_FORMAT, SAMPLE_CAMERA_RESOL_WFS, &instr.spots_x, &instr.spots_y))
			handle_errors(err);
	}
	
	if(instr.selected_id & DEVICE_OFFSET_WFS10) // WFS10 instrument
	{
		printf("\n\nConfigure WFS10 camera with resolution index %d (%d x %d pixels).\n", SAMPLE_CAMERA_RESOL_WFS10, cam_wfs10_xpixel[SAMPLE_CAMERA_RESOL_WFS10], cam_wfs10_ypixel[SAMPLE_CAMERA_RESOL_WFS10]);
	
		if(err = WFS_ConfigureCam (instr.handle, SAMPLE_PIXEL_FORMAT, SAMPLE_CAMERA_RESOL_WFS10, &instr.spots_x, &instr.spots_y))
			handle_errors(err);
	}
	
	if(instr.selected_id & DEVICE_OFFSET_WFS20) // WFS20 instrument
	{
		printf("\n\nConfigure WFS20 camera with resolution index %d (%d x %d pixels).\n", SAMPLE_CAMERA_RESOL_WFS20, cam_wfs20_xpixel[SAMPLE_CAMERA_RESOL_WFS20], cam_wfs20_ypixel[SAMPLE_CAMERA_RESOL_WFS20]);
	
		if(err = WFS_ConfigureCam (instr.handle, SAMPLE_PIXEL_FORMAT, SAMPLE_CAMERA_RESOL_WFS20, &instr.spots_x, &instr.spots_y))
			handle_errors(err);
	}
	
	if(instr.selected_id & DEVICE_OFFSET_WFS30) // WFS30 instrument
	{
		printf("\n\nConfigure WFS30 camera with resolution index %d (%d x %d pixels).\n", SAMPLE_CAMERA_RESOL_WFS30, cam_wfs30_xpixel[SAMPLE_CAMERA_RESOL_WFS30], cam_wfs30_ypixel[SAMPLE_CAMERA_RESOL_WFS30]);
	
		if(err = WFS_ConfigureCam (instr.handle, SAMPLE_PIXEL_FORMAT, SAMPLE_CAMERA_RESOL_WFS30, &instr.spots_x, &instr.spots_y))
			handle_errors(err);
	}
	
	if(instr.selected_id & DEVICE_OFFSET_WFS40) // WFS40 instrument
	{
		printf("\n\nConfigure WFS40 camera with resolution index %d (%d x %d pixels).\n", SAMPLE_CAMERA_RESOL_WFS40, cam_wfs40_xpixel[SAMPLE_CAMERA_RESOL_WFS40], cam_wfs40_ypixel[SAMPLE_CAMERA_RESOL_WFS40]);
	
		if(err = WFS_ConfigureCam (instr.handle, SAMPLE_PIXEL_FORMAT, SAMPLE_CAMERA_RESOL_WFS40, &instr.spots_x, &instr.spots_y))
			handle_errors(err);
	}
	

	printf("Camera is configured to detect %d x %d lenslet spots.\n\n", instr.spots_x, instr.spots_y);
	
	
	// set camera exposure time and gain if you don't want to use auto exposure
	// use functions WFS_GetExposureTimeRange, WFS_SetExposureTime, WFS_GetMasterGainRange, WFS_SetMasterGain
	
	// set WFS internal reference plane
	printf("\nSet WFS to internal reference plane.\n");
	if(err = WFS_SetReferencePlane (instr.handle, SAMPLE_REF_PLANE))
		handle_errors(err);
	
	
	// define pupil
	printf("\nDefine pupil to:\n");
	printf("Centroid_x = %6.3f\n", SAMPLE_PUPIL_CENTROID_X);
	printf("Centroid_y = %6.3f\n", SAMPLE_PUPIL_CENTROID_Y);
	printf("Diameter_x = %6.3f\n", SAMPLE_PUPIL_DIAMETER_X);
	printf("Diameter_y = %6.3f\n", SAMPLE_PUPIL_DIAMETER_Y);

	if(err = WFS_SetPupil (instr.handle, SAMPLE_PUPIL_CENTROID_X, SAMPLE_PUPIL_CENTROID_Y, SAMPLE_PUPIL_DIAMETER_X, SAMPLE_PUPIL_DIAMETER_Y))
		handle_errors(err);
	
	printf("\nRead camera images:\n");
	
	printf("Image No.     Status     ->   newExposure[ms]   newGainFactor\n");
	
	// do some trials to read a well exposed image
	for(cnt = 0; cnt < SAMPLE_IMAGE_READINGS; cnt++)
	{
		// take a camera image with auto exposure, note that there may several function calls required to get an optimal exposed image
		if(err = WFS_TakeSpotfieldImageAutoExpos (instr.handle, &expos_act, &master_gain_act))
			handle_errors(err);
	
		printf("    %d     ", cnt);
	
		// check instrument status for non-optimal image exposure
		if(err = WFS_GetStatus (instr.handle, &instr.status))
			handle_errors(err);   
	
		if(instr.status & WFS_STATBIT_PTH) printf("Power too high!    ");
		 else
		if(instr.status & WFS_STATBIT_PTL) printf("Power too low!     ");
		 else
		if(instr.status & WFS_STATBIT_HAL) printf("High ambient light!");
		 else
			printf(                                "OK                 ");
		
		printf("     %6.3f          %6.3f\n", expos_act, master_gain_act);
		
		if( !(instr.status & WFS_STATBIT_PTH) && !(instr.status & WFS_STATBIT_PTL) && !(instr.status & WFS_STATBIT_HAL) )
			break; // image well exposed and is usable
	}
	

	// close program if no well exposed image is feasible
	if( (instr.status & WFS_STATBIT_PTH) || (instr.status & WFS_STATBIT_PTL) ||(instr.status & WFS_STATBIT_HAL) )
	{
		printf("\nSample program will be closed because of unusable image quality, press <ENTER>.");
		WFS_close(instr.handle); // required to release allocated driver data
        error_exit(instrHdl, 0);
		fflush(stdin);
		getchar();
		exit(1);
	}
	

	// calculate all spot centroid positions using dynamic noise cut option
	if(err = WFS_CalcSpotsCentrDiaIntens (instr.handle, SAMPLE_OPTION_DYN_NOISE_CUT, SAMPLE_OPTION_CALC_SPOT_DIAS))
		handle_errors(err);

	// get centroid result arrays
	if(err = WFS_GetSpotCentroids (instr.handle, *centroid_x, *centroid_y))
		handle_errors(err);

	// get centroid and diameter of the optical beam, you may use this beam data to define a pupil variable in position and size
	// for WFS20: this is based on centroid intensties calculated by WFS_CalcSpotsCentrDiaIntens()
	if(err = WFS_CalcBeamCentroidDia (instr.handle, &beam_centroid_x, &beam_centroid_y, &beam_diameter_x, &beam_diameter_y))
		handle_errors(err);
	// calculate spot deviations to internal reference
	if(err = WFS_CalcSpotToReferenceDeviations (instr.handle, SAMPLE_OPTION_CANCEL_TILT))
		handle_errors(err);
	
	// get spot deviations
	if(WFS_GetSpotDeviations (instr.handle, *deviation_x, *deviation_y))
		handle_errors(err);
	
	// calculate and printout measured wavefront
	if(err = WFS_CalcWavefront (instr.handle, SAMPLE_WAVEFRONT_TYPE, SAMPLE_OPTION_LIMIT_TO_PUPIL, *wavefront))
		handle_errors(err);
	
	// calculate wavefront statistics within defined pupil
	if(err = WFS_CalcWavefrontStatistics (instr.handle, &wavefront_min, &wavefront_max, &wavefront_diff, &wavefront_mean, &wavefront_rms, &wavefront_weighted_rms))
		handle_errors(err);
	

	// calculate Zernike coefficients
	printf("\nZernike fit up to order %d:\n",SAMPLE_ZERNIKE_ORDERS);
	zernike_order = SAMPLE_ZERNIKE_ORDERS; // pass 0 to function for auto Zernike order, choosen order is returned
	if(err = WFS_ZernikeLsf (instr.handle, &zernike_order, zernike_um, zernike_orders_rms_um, &roc_mm)) // calculates also deviation from centroid data for wavefront integration
		handle_errors(err);
		
	printf("\nZernike Mode    Coefficient\n");
	for(i=0; i < zernike_modes[SAMPLE_ZERNIKE_ORDERS]; i++)
	{
		printf("  %2d         %9.3f\n",i, zernike_um[i]);
	}

	
	if(err = TLDFMX_measure_system_parameters (instrHdl, VI_TRUE, zernike_um, nextMirrorPattern,&remainingSteps))
		error_exit(instrHdl, err);
	
	if(err = TLDFM_set_segment_voltages (instrHdl, nextMirrorPattern))
		error_exit(instrHdl, err);

	while (remainingSteps){
		if(err = WFS_TakeSpotfieldImageAutoExpos (instr.handle, &expos_act, &master_gain_act))
				handle_errors(err);

		zernike_order = SAMPLE_ZERNIKE_ORDERS; // pass 0 to function for auto Zernike order, choosen order is returned
		if(err = WFS_ZernikeLsf (instr.handle, &zernike_order, zernike_um, zernike_orders_rms_um, &roc_mm)) // calculates also deviation from centroid data for wavefront integration
			handle_errors(err);
		
		if(err = TLDFMX_measure_system_parameters (instrHdl, VI_FALSE, zernike_um, nextMirrorPattern,&remainingSteps))
			error_exit(instrHdl, err);
	
		if(err = TLDFM_set_segment_voltages (instrHdl, nextMirrorPattern))
			error_exit(instrHdl, err);
	}
	
	get_Zernike_list();
	pthread_t thread_id;
	threadArgs loopArgs;
	loopArgs.WFS_handle = &instr.handle;
	loopArgs.handle = &instrHdl;
	loopArgs.target = target_zernike;
	loopArgs.thflag = &thread_flag;
	
	pthread_create(&thread_id, NULL, Loop, (void*) &loopArgs);
	if (thread_flag){
		printf("The Zernike amplitudes are achieved. Enter new Zernikes.\n");
		get_Zernike_list();
		thread_flag = 0;
	}

	// Close instrument, important to release allocated driver data!
	WFS_close(instr.handle);
}



/*===============================================================================================================================
  Handle Errors
  This function retrieves the appropriate text to the given error number and closes the connection in case of an error
===============================================================================================================================*/
void handle_errors (int err)
{
	char buf[WFS_ERR_DESCR_BUFFER_SIZE];

	if(!err) return;

	// Get error string
	WFS_error_message (instr.handle, err, buf);

	if(err < 0) // errors
	{
		printf("\nWavefront Sensor Error: %s\n", buf);

		// close instrument after an error has occured
		printf("\nSample program will be closed because of the occured error, press <ENTER>.");
		WFS_close(instr.handle); // required to release allocated driver data
		if(VI_NULL != instrHdl)
		{
			TLDFMX_close(instrHdl);
		}
		fflush(stdin);
		getchar();
		exit(1);
	}
}



/*===============================================================================================================================
	Select Instrument
===============================================================================================================================*/
int select_instrument (int *selection, ViChar resourceName[])
{
	int            i,err;
	long int 	instr_cnt;
	ViInt32        device_id;
	long int            in_use;
	char           instr_name[WFS_BUFFER_SIZE];
	char           serNr[WFS_BUFFER_SIZE];
	char           strg[WFS_BUFFER_SIZE];

	// Find available instruments
	if(err = WFS_GetInstrumentListLen (VI_NULL, &instr_cnt))
		handle_errors(err);
		
	if(instr_cnt == 0)
	{
		printf("No Wavefront Sensor instrument found!\n");
		return 0;
	}

	// List available instruments
	printf("Available Wavefront Sensor instruments:\n\n");
	
	for(i=0;i<instr_cnt;i++)
	{
		if(err = WFS_GetInstrumentListInfo (VI_NULL, i, &device_id, &in_use, instr_name, serNr, resourceName))
			handle_errors(err);
		
		printf("%4d   %s    %s    %s\n", device_id, instr_name, serNr, (!in_use) ? "" : "(inUse)");
	}

	// Select instrument
	printf("\nSelect a Wavefront Sensor instrument: ");
	fflush(stdin);
	
	fgets (strg, WFS_BUFFER_SIZE, stdin);
	*selection = atoi(strg);

	// get selected resource name
	for(i=0;i<instr_cnt;i++)
	{   
		if(err = WFS_GetInstrumentListInfo (VI_NULL, i, &device_id, &in_use, instr_name, serNr, resourceName))
		   handle_errors(err);
		
		if(device_id == *selection)
			break; // resourceName fits to device_id
	}
	
	return *selection;
}


/*===============================================================================================================================
	Select MLA
===============================================================================================================================*/
int select_mla (int *selection)
{
	int            i,err;
	long int	mla_cnt;

	// Read out number of available Microlens Arrays 
	if(err = WFS_GetMlaCount (instr.handle, &instr.mla_cnt))
		handle_errors(err);

	// List available Microlens Arrays
	printf("\nAvailable Microlens Arrays:\n\n");
	for(i=0;i<instr.mla_cnt;i++)
	{   
		if(WFS_GetMlaData (instr.handle, i, instr.mla_name, &instr.cam_pitch_um, &instr.lenslet_pitch_um, &instr.center_spot_offset_x, &instr.center_spot_offset_y, &instr.lenslet_f_um, &instr.grd_corr_0, &instr.grd_corr_45))
			handle_errors(err);   
	
		printf("%2d  %s   CamPitch=%6.3f LensletPitch=%8.3f\n", i, instr.mla_name, instr.cam_pitch_um, instr.lenslet_pitch_um);
	}
	
	// Select MLA
	printf("\nSelect a Microlens Array: ");
	fflush(stdin);
	*selection = getchar() - '0';
	if(*selection < -1)
		*selection = -1; // nothing selected

	return *selection;
}




/*---------------------------------------------------------------------------
  Print keypress message and wait
---------------------------------------------------------------------------*/
void waitKeypress (void)
{
   printf("Press <ENTER> to continue\n");
   while(EOF == getchar());
}

/*---------------------------------------------------------------------------
  Exit with error message
---------------------------------------------------------------------------*/
void error_exit (ViSession instrHdl, ViStatus err)
{
   ViChar buf[TLDFM_ERR_DESCR_BUFER_SIZE];

   // Get error description and print out error
   TLDFMX_error_message(instrHdl, err, buf);
   fprintf(stderr, "\nERROR: %s\n", buf);

   // close session to instrument if open
   if(VI_NULL != instrHdl)
   {
      TLDFMX_close(instrHdl);
   }
   WFS_close(instr.handle);

   // exit program
   printf("\nThe program is shutting down.\n");
   waitKeypress();

   exit(EXIT_FAILURE);
}


/*---------------------------------------------------------------------------
 Read out device ID and print it to screen
---------------------------------------------------------------------------*/
ViStatus select_instrument_DMH (ViChar **resource)
{
	ViStatus err;
	ViUInt32 deviceCount = 0;
	int      choice = 0;
	
	ViChar    manufacturer[TLDFM_BUFFER_SIZE];
	ViChar    instrumentName[TLDFM_MAX_INSTR_NAME_LENGTH];
    ViChar    serialNumber[TLDFM_MAX_SN_LENGTH];
    ViBoolean deviceAvailable;
    ViChar    resourceName[TLDFM_BUFFER_SIZE];
	
	printf("Scanning for instruments...\n");
	err = TLDFM_get_device_count(VI_NULL, &deviceCount);
	if((TL_ERROR_RSRC_NFOUND == err) || (0 == deviceCount))
	{
		printf("No matching instruments found\n\n");
		return err;
	}
	
	printf("Found %d matching instrument(s):\n\n", deviceCount);
	
	for(ViUInt32 i = 0; i < deviceCount;)
	{
		err = TLDFM_get_device_information(VI_NULL,
										   i,
										   manufacturer,
										   instrumentName,
										   serialNumber,
										   &deviceAvailable,
										   resourceName);
		
		printf("%d:\t%s\t%s\tS/N:%s\t%s\n",
				++i,
				manufacturer,
				instrumentName,
				serialNumber,
				deviceAvailable ? "available" : "locked");
	}
	
	if(2 <= deviceCount)
	{
		ViBoolean deviceSelected = VI_FALSE;
		printf("\nPlease choose: ");
		do
		{
			do
			{
				choice = getchar();
			}
			while(EOF == choice);
		
			if((0 >= choice) || (deviceCount < choice))
			{
				printf("Invalid choice\n\n");
				choice = EOF;
			}
			else
			{
				deviceSelected = VI_TRUE;
			}
		}
		while(!deviceSelected);
	}
	else
	{
		choice = 1;
		printf("\nPress any key to continue");
		waitKeypress();
	}
	
	err = TLDFM_get_device_information(VI_NULL,
									   (ViUInt32)(choice - 1),
									   manufacturer,
									   instrumentName,
									   serialNumber,
									   &deviceAvailable,
									   resourceName);
	
	if(VI_SUCCESS == err)
	{
		*resource = malloc(TLDFM_BUFFER_SIZE);
		strncpy(*resource, resourceName, TLDFM_BUFFER_SIZE);
	}
	return err;
}


/*---------------------------------------------------------------------------
 Generate Zernike Shape
---------------------------------------------------------------------------*/
void get_Zernike_list (void) {
	float	zernike_input[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int	iter = 0;
	char	holder[10] = {'\0'};
	char* error_check;
	fflush(stdin);
	while (iter < 16){
		printf("Input the %d-th order Zernike in um (input 'p' to zero following orders; input 'e' to terminate)\n",iter);
		fgets(holder,10,stdin);
		if (holder[0] == 'p'){
			break;
		}else if (holder[0] == 'e'){
			WFS_close(instr.handle);
			TLDFMX_close(instrHdl);
			exit(EXIT_FAILURE);
			break;
		}else{
			zernike_input[iter] = strtof(holder,&error_check);
			if (*error_check == '\0'){
				printf("Not a valid float input\n");
			}else{
				iter ++;
			}
		}
	}
	for (int i = 0; i < 16; i++){
		target_zernike[i] = zernike_input[i];
	}
}

void* Loop(void *Args){
	int err;
	int ite = 0;
	int counter = 0;
	int recorder = 0;
	int stable = 0;
	threadArgs * Argstruct = (threadArgs *)Args;
	float measuredZernike[16];
	float zeroZernike[16];
	double resultedZernike[12];
	double ctrlVoltage[60];
	long int zernike_order = 4;
	while(1){
		stable = 1;
		if(err = WFS_TakeSpotfieldImageAutoExpos (*Argstruct->WFS_handle, NULL, NULL))
			handle_errors(err);
		if(err = WFS_ZernikeLsf (*Argstruct->WFS_handle, &zernike_order, measuredZernike, NULL, NULL)) // calculates also deviation from centroid data for wavefront integration
			handle_errors(err);
		for (ite = 0; ite < 16; ite ++){
			zeroZernike[ite] = measuredZernike[ite] - Argstruct->target[ite];
		}
		if(err = TLDFMX_get_flat_wavefront (*Argstruct->handle, 0xFFFFFFFF, zeroZernike, resultedZernike, ctrlVoltage))
			error_exit(*Argstruct->handle, err);
		if(err = TLDFM_set_segment_voltages (*Argstruct->handle, ctrlVoltage))
			error_exit(*Argstruct->handle, err);
		printf("Resulted Zernike starting from Z4: ");
		for (ite = 0; ite < 12; ite ++){
			printf("%f,",resultedZernike[ite]);
			if (resultedZernike[ite] > 0.01 || resultedZernike[ite] < -0.01){
				stable = 0;
			}
		}
		printf("\n");
		if (stable){
			*Argstruct->thflag = 1;
			recorder = 1;
		}else{
			if (recorder){
				counter = 0;
			}else{
				counter ++;
			}
			recorder = 0;
		}
		if (counter > 10){
			printf("Seems the loop fails to lock; input 'e' to terminate otherwise continue\n");
			if(getchar() == 'e'){
				TLDFMX_close(*Argstruct->handle);
				WFS_close(*Argstruct->WFS_handle);
				exit(EXIT_FAILURE);
				break;
			}
		}
	}
}

/*===============================================================================================================================
	End of source file
===============================================================================================================================*/
