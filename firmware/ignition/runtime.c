

#include "runtime.h"
#include "board.h"
#include "io.h"
#include "modp_atonum.h"
#include "mod_string.h"
#include "comm.h"
#include "interrupt_utils.h"

//****************************************************
//Runtime data
unsigned int		g_wheelSyncAttempts;
unsigned int 		g_toothCountAtLastSyncAttempt;
unsigned int		g_currentCrankRevolutionPeriodRaw;
unsigned int 		g_lastCrankRevolutionPeriodRaw;
unsigned int		g_lastCrankRevolutionPeriodUSec;
unsigned int 		g_lastInterToothPeriodRaw;
unsigned int 		g_currentInterToothPeriodRaw;
unsigned int 		g_currentToothPeriodOverflowCount;
int					g_lockedAdvance;
unsigned int 		g_currentRPM;
unsigned int		g_currentDwellUSec;
unsigned int 		g_currentLoad;
unsigned int 		g_currentLoadBin;
unsigned int 		g_currentRPMBin;
int 				g_currentAdvance;
unsigned int 		g_currentDwellDegrees;
unsigned int 		g_engineIsRunning;


unsigned int		g_currentTooth;
unsigned int 		g_recalculateTooth;
unsigned int		g_coilDriversToFire;
unsigned int		g_coilDriversToCharge;

unsigned int 		g_coilFirePort[CRANK_TEETH];
unsigned int		g_coilFireTimerCount[CRANK_TEETH];
unsigned int		g_coilChargePort[CRANK_TEETH];
unsigned int		g_coilChargeTimerCount[CRANK_TEETH];

struct logical_coil_driver 	g_logical_coil_drivers[MAX_COIL_DRIVERS];
unsigned int		g_logicalCoilDriverCount;

//flags
unsigned int 		g_wheelSynchronized;
unsigned int		g_lockedAdvanceEnabled;
unsigned int		g_recalculatePending;

//****************************************************

int							g_selectedIgnitionConfigIndex;
enum OUTPUT_STATE			g_output_state[USER_DEFINED_OUTPUTS];

/* Engine configuration */
struct engine_config 		g_active_engine_config;
struct ignition_config		g_active_ignition_config[IGNITION_CONFIGS];
struct ignition_config 		*g_selected_ignition_config;

//mapping the actual PIO ports of each coil driver output
const unsigned int g_coilDriver_mappings[COIL_DRIVERS]={COIL_0,COIL_1,COIL_2,COIL_3,COIL_4,COIL_5,COIL_6,COIL_7};

//Flash storage for configuration data

//Ignition configuration
const struct ignition_config g_saved_ignition_config[IGNITION_CONFIGS] __attribute__((section(".text\n\t#"))) = {DEFAULT_IGNITION_CONFIG,DEFAULT_IGNITION_CONFIG};
//Engine configuration	
const struct engine_config g_saved_engine_config __attribute__((section(".text\n\t#"))) = DEFAULT_ENGINE_CONFIG;

//ADC Calibrations
const short g_saved_MAP_calibration[ADC_CALIBRATION_SIZE]  __attribute__((section(".text\n\t#"))) = DEFAULT_MAP_CALIBRATION;
const short g_saved_TPS_calibration[ADC_CALIBRATION_SIZE]  __attribute__((section(".text\n\t#"))) = DEFAULT_GENERIC_CALIBRATION;
const short g_saved_Temp_calibration[ADC_CALIBRATION_SIZE] __attribute__((section(".text\n\t#"))) = DEFAULT_TEMP_CALIBRATION;
const short g_saved_Aux_calibration[ADC_CALIBRATION_SIZE]    __attribute__((section(".text\n\t#"))) = DEFAULT_GENERIC_CALIBRATION;



#define COIL_PACK_INTERRUPT_LEVEL		7
#define TRIGGER_WHEEL_INTERRUPT_LEVEL	6

/*-----------------*/
/* Clock Selection */
/*-----------------*/
#define TC_CLKS                  0x7
#define TC_CLKS_MCK2             0x0
#define TC_CLKS_MCK8             0x1
#define TC_CLKS_MCK32            0x2
#define TC_CLKS_MCK128           0x3
#define TC_CLKS_MCK1024          0x4


extern void ( coilPackCharge_irq_entry )( void );
extern void ( coilPackFire_irq_entry ) ( void );
extern void ( triggerWheel_irq_entry )( void );

//*----------------------------------------------------------------------------
//* Function Name       : timer_init
//* Object              : Init timer counter
//* Input Parameters    : none
//* Output Parameters   : TRUE
//*----------------------------------------------------------------------------
static void triggerWheelTimerInit( void )
{
		

	//pullup enable on timer 0
	//AT91F_PIO_CfgInput(AT91C_BASE_PIOA, 1 << 0);
	AT91C_BASE_PIOA->PIO_PPUER = (1 << 0);

  /* Set PIO pins for Timer Counter 0 */
   //AT91F_TC0_CfgPIO();
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA0_TIOA0   ) |
		((unsigned int) AT91C_PA1_TIOB0   ) ); // Peripheral B
   /* Enable TC0's clock in the PMC controller */
   AT91F_TC0_CfgPMC();

	AT91F_TC_Open ( 
	AT91C_BASE_TC0,
	
	TC_CLKS_MCK2 | 
	AT91C_TC_ETRGEDG_FALLING |
	AT91C_TC_ABETRG |
	AT91C_TC_LDRA_RISING |
	AT91C_TC_LDRB_FALLING
	
	,AT91C_ID_TC0
	);

	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC0, TRIGGER_WHEEL_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE, triggerWheel_irq_entry);

	AT91C_BASE_TC0->TC_IER = AT91C_TC_COVFS | AT91C_TC_LDRBS;  //  IRQ enable RB loading
	
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC0);
}


static void coilPackChargeTimerInit( void ){

	AT91PS_TCB pTCB = AT91C_BASE_TCB;		// create a pointer to TC Global Register structure
	pTCB->TCB_BCR = 0;						// SYNC trigger not used
	

	AT91F_TC_Open ( 
	AT91C_BASE_TC2,
	
	TC_CLKS_MCK8    | 
	AT91C_TC_WAVE   |
	AT91C_TC_CPCSTOP
	
	,AT91C_ID_TC2
	);

	AT91F_AIC_ConfigureIt (
			AT91C_BASE_AIC, 
			AT91C_ID_TC2, 
			COIL_PACK_INTERRUPT_LEVEL,
			AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, 
			coilPackCharge_irq_entry
			);
	
	AT91C_BASE_TC2->TC_IER = AT91C_TC_CPCS;  	// IRQ enable RC compare
	
	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC2);
}

static void coilPackFireTimerInit( void ){

	AT91PS_TCB pTCB = AT91C_BASE_TCB;		// create a pointer to TC Global Register structure
	pTCB->TCB_BCR = 0;						// SYNC trigger not used


	AT91F_TC_Open (
	AT91C_BASE_TC1,

	TC_CLKS_MCK8    |
	AT91C_TC_WAVE   |
	AT91C_TC_CPCSTOP

	,AT91C_ID_TC1
	);

	AT91F_AIC_ConfigureIt (
			AT91C_BASE_AIC,
			AT91C_ID_TC1,
			COIL_PACK_INTERRUPT_LEVEL,
			AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,
			coilPackFire_irq_entry
			);

	AT91C_BASE_TC1->TC_IER = AT91C_TC_CPCS;  	// IRQ enable RC compare

	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC1);
}


//returns an existing logical coil driver for the specified cylinderTDC
static struct logical_coil_driver * findMatchingLogicalCoilDriver(int cylinderTDC,struct logical_coil_driver *logical_coil_drivers){
	for (int i = 0; i < g_logicalCoilDriverCount; i++){
		struct logical_coil_driver * logicalCoilDriver = logical_coil_drivers + i;
		if ( logicalCoilDriver->cylinderTDC == cylinderTDC ) return logicalCoilDriver;
	}
	return NULL;
}


//map the cylinder configuration to the logical coil driver structure
//to make runtime processing of the coil drivers more efficient.
static void initLogicalCoilDrivers(struct logical_coil_driver *logical_coil_drivers){
	
	g_logicalCoilDriverCount = 0;
	for (int i = 0; i < g_active_engine_config.cylinderCount;i++){
		struct cylinder_config *cylinderCfg = g_active_engine_config.cylinderConfigs + i;
		int coilDriver = cylinderCfg->coilDriver;
		
		//account for missing tooth offset
		int cylinderTDC = cylinderCfg->tdcFireDegrees + g_active_engine_config.missingToothBTDCDegrees;
		if (cylinderTDC >= MAX_DEGREES) cylinderTDC = cylinderTDC - MAX_DEGREES;
		
		struct logical_coil_driver *logicalCoilDriver = findMatchingLogicalCoilDriver(cylinderTDC,logical_coil_drivers);
		if (NULL == logicalCoilDriver){ //add it to the first one
			logicalCoilDriver = logical_coil_drivers + g_logicalCoilDriverCount;
			logicalCoilDriver->cylinderTDC = cylinderTDC;
			//set the physical port
			logicalCoilDriver->physicalCoilDriverPorts = g_coilDriver_mappings[coilDriver];
			g_logicalCoilDriverCount++;
		}
		else{
			//Add the pysical port to the existing one
			logicalCoilDriver->physicalCoilDriverPorts = logicalCoilDriver->physicalCoilDriverPorts | g_coilDriver_mappings[coilDriver];
		}
	}
}


//Update the cylinder runtime data based on the current ignition advance
static void updateLogicalCoilDriverRuntimes(struct logical_coil_driver *logical_coil_drivers){

	memset(g_coilFirePort,0,sizeof(g_coilFirePort));
	memset(g_coilFireTimerCount,0, sizeof(g_coilFireTimerCount));
	memset(g_coilChargePort,0,sizeof(g_coilChargePort));
	memset(g_coilChargeTimerCount,0,sizeof(g_coilChargeTimerCount));

	g_recalculateTooth = 0;

	int count = g_logicalCoilDriverCount;
	
	struct logical_coil_driver *logicalCoilPack = logical_coil_drivers;

	int coilInterToothPeriod = g_lastCrankRevolutionPeriodRaw / CRANK_TEETH;
	unsigned int periodPerDegree = g_lastCrankRevolutionPeriodRaw / MAX_DEGREES;

	while (count > 0){
		
		//calculate coil firing point
		int coilFireDegrees = logicalCoilPack->cylinderTDC - g_currentAdvance;
		if ( coilFireDegrees < 0 ) coilFireDegrees = coilFireDegrees + MAX_DEGREES;

		// we schedule the firing one tooth ahead
		int coilFireTooth = (coilFireDegrees / DEGREES_PER_TOOTH) - 1;
		if (coilFireTooth < 0 ) coilFireTooth = CRANK_TEETH - 1;

		int coilFireInterToothDegrees = coilFireDegrees % DEGREES_PER_TOOTH;

		//the timer fire count is calculated one tooth ahead
		//and compensated for trigger wheel IRQ hander overhead
		unsigned int timerFireCount = coilInterToothPeriod - ISR_OVEREAD_TIMER_TICK_COUNTS + ( periodPerDegree * coilFireInterToothDegrees);

		//check if we've landed on the missing tooth (tooth zero)
		//and roll over to the last tooth if necessary
		if (0 == coilFireTooth){
			coilFireTooth = CRANK_TEETH - 1;
			timerFireCount += coilInterToothPeriod;
		}

		// divide /4 = adjust for different timer clock
		timerFireCount = timerFireCount / 4;

		logicalCoilPack->timerFireCount = timerFireCount;
		logicalCoilPack->coilFireDegrees = coilFireDegrees;
		logicalCoilPack->coilFireTooth = coilFireTooth;

		//check and adjust what tooth we trigger the recalculation on
		if (g_recalculateTooth < coilFireTooth + 1){
			g_recalculateTooth = coilFireTooth + 1;
			if (g_recalculateTooth >= CRANK_TEETH) g_recalculateTooth = 1;
		}

		g_coilFirePort[coilFireTooth] = logicalCoilPack->physicalCoilDriverPorts;
		g_coilFireTimerCount[coilFireTooth] = timerFireCount;

		//Calculate coil charging point
		int coilOnDegrees = coilFireDegrees - g_currentDwellDegrees;
		while( coilOnDegrees < 0 ){
			coilOnDegrees = coilOnDegrees + MAX_DEGREES;
		}

		// we schedule  the charging one tooth ahead
		int coilOnTooth = (coilOnDegrees / DEGREES_PER_TOOTH) - 1;
		if (coilOnTooth < 0 ) coilOnTooth = CRANK_TEETH - 1;

		int coilOnInterToothDegrees = coilOnDegrees % DEGREES_PER_TOOTH;

		//the timer charge count is calculated one tooth ahead
		//and compensated for trigger wheel IRQ hander overhead
		unsigned int timerChargeCount = coilInterToothPeriod - ISR_OVEREAD_TIMER_TICK_COUNTS + (periodPerDegree * coilOnInterToothDegrees);

		//check if we've landed on the missing tooth (tooth zero)
		//and roll over to the last tooth if necessary
		if (0 == coilOnTooth){
			coilOnTooth = CRANK_TEETH - 1;
			timerChargeCount += coilInterToothPeriod;
		}

		// divide /4 = adjust for different timer clock
		timerChargeCount = timerChargeCount / 4;

		logicalCoilPack->timerChargeCount = timerChargeCount;
		logicalCoilPack->coilChargeDegrees = coilOnDegrees;
		logicalCoilPack->coilChargeTooth = coilOnTooth;

		g_coilChargePort[coilOnTooth] = logicalCoilPack->physicalCoilDriverPorts;
		g_coilChargeTimerCount[coilOnTooth] = timerChargeCount;
		
		logicalCoilPack++;
		count--;	
	}
}

//calculate the dwell period in crankshaft degrees
//based on current RPM and desired dwell duration
void calculateDwellDegrees(unsigned int crankRevolutionPeriodUSec){

	unsigned int dwellDegrees = (MAX_DEGREES * g_currentDwellUSec ) / crankRevolutionPeriodUSec;
	if (dwellDegrees > MAX_DWELL_DEGREES) dwellDegrees = MAX_DWELL_DEGREES;
	g_currentDwellDegrees = dwellDegrees;
}


static void initRuntimeData( void ){

	memset(g_output_state,0,sizeof(g_output_state));
	memset(g_logical_coil_drivers, 0, sizeof(g_logical_coil_drivers));

	//default to the first ignition configuration
	//TODO: read the input state and actually select the right one
	g_selectedIgnitionConfigIndex = 0;

	//copy from flash to RAM
	memcpy(g_active_ignition_config, g_saved_ignition_config, sizeof(g_active_ignition_config));
	memcpy(&g_active_engine_config, &g_saved_engine_config, sizeof(g_active_engine_config));

	//set the pointer to the active config
	g_selected_ignition_config = &(g_active_ignition_config[g_selectedIgnitionConfigIndex]);

	g_toothCountAtLastSyncAttempt = 0;
	g_wheelSyncAttempts = 0;
	g_currentTooth  = 0;
	g_recalculateTooth = CRANK_TEETH - 1;
	g_wheelSynchronized = 0;
	g_currentCrankRevolutionPeriodRaw = 0;
	g_lastCrankRevolutionPeriodUSec = 0;
	g_lastCrankRevolutionPeriodRaw = 0;
	g_lastInterToothPeriodRaw = 0;
	g_currentInterToothPeriodRaw = 0;
	g_currentToothPeriodOverflowCount = 0;
	g_currentRPM = 0;
	g_currentLoad = 0;
	g_currentLoadBin = 0;
	g_currentRPMBin = 0;
	g_engineIsRunning = 0;
	g_lockedAdvanceEnabled = 0;
	g_lockedAdvance = DEFAULT_LOCKED_ADVANCE;
	g_currentDwellUSec = DEFAULT_DWELL_USEC;
	g_currentAdvance = g_active_engine_config.cranking_degrees;
	g_lastCrankRevolutionPeriodRaw = CRANKING_REVOLUTION_PERIOD_RAW;
	initLogicalCoilDrivers(g_logical_coil_drivers);
	g_currentDwellDegrees = 0;
	updateLogicalCoilDriverRuntimes(g_logical_coil_drivers);
}




//calculate the current crank period microseconds based on the raw crank period
static unsigned int calculateCrankPeriodUSec(){
	//if master clock is 48054840 Hz and MCK/2
	//USec = (timer value * 100000) / 2402742		
	return (g_lastCrankRevolutionPeriodRaw * 100) / 2403;
}

//calculate the current RPM based on the last
//crank revolution period
void calculateRPM(unsigned int crankRevolutionPeriodUSec){
	if (crankRevolutionPeriodUSec > 0){
		g_currentRPM = ((1000000000 / crankRevolutionPeriodUSec) * 6) / 100;
	}
}


//Updates the user outputs
void updateUserOutputs(){

	struct output_config *output;
	
	int runtimeValue;
	int hysteresis;
	enum OUTPUT_STATE *state;
	
	state = g_output_state;
	
	for (int i = 0; i < USER_DEFINED_OUTPUTS; i++){ 	
		output = &(g_selected_ignition_config->user_defined_outputs[i]);

		switch (output->output_type){			
			case OUTPUT_TYPE_RPM:
				runtimeValue = g_currentRPM;
				hysteresis =  USER_OUTPUT_RPM_HYSTERESIS;
				break;
			case OUTPUT_TYPE_MAP:
				runtimeValue = g_currentLoad;
				hysteresis = USER_OUTPUT_LOAD_HYSTERESIS;
				break;
			default:
				runtimeValue = 0;
				hysteresis = 0;
				//TODO: signal error condition here
				break;
		}
		
		if (OUTPUT_ACTIVE == *state){
			if (runtimeValue < output->trigger_threshold - hysteresis) *state = OUTPUT_INACTIVE;	
		}
		else{
			if (runtimeValue > output->trigger_threshold) *state = OUTPUT_ACTIVE;	
		}
		
		int outputPortHigh = (*state == OUTPUT_ACTIVE);
		if (output->output_mode == MODE_INVERTED) outputPortHigh = !outputPortHigh;
		//TODO: enable output port here
		
		//move to the next saved state
		state++;
	}
}

//linear interpolation routine
//            (y2 - y1)
//  y = y1 +  --------- * (x - x1)
//            (x2 - x1)
int linearInterpolate(int x, int x1, int y1, int x2, int y2){
	return y1 + (((((y2 - y1) << 16)  / (x2 - x1)) * (x - x1)) >> 16);
}

//calculate the ignition advance
void calculateAdvance(){

	if (g_lockedAdvanceEnabled){
		g_currentAdvance = g_lockedAdvance;
		return;
	}
	//Local variable declarations

	//local copies of current RPM and current Load values	
	unsigned int currentRPM, currentLoad;
		
	//all of the different advance values
	//selected for linear interpolation purposes
	int advance,           advance_next_rpm;
	int advance_next_load, advance_next_rpm_load;
	
	//the interpolated advance values across the two axis
	int advanceRpmInterpolated, advanceRpmInterpolatedNextLoad;

	//the different bins used for interpolation purposes	
	unsigned int rpmBin, nextRpmBin;
	unsigned int loadBin, nextLoadBin;

	//the RPM bin values, extracted and saved
	unsigned int rpmBinValue, nextRpmBinValue;
	
	//a pointer to the ignition map
	struct ignition_map *map;
	
	//pointers to rpm / load bins
	int *rpmBins;
	int *loadBins;
	int (*advanceTable)[RPM_BIN_COUNT];

	//////////////////////////////
	//end of variable declarations
	//////////////////////////////
	
	//get local copies of the global variables
	currentRPM = g_currentRPM;
	currentLoad = g_currentLoad;
	
	//get our currently selected ignition map
	map = &(g_selected_ignition_config->map);


	///////////////////
	//Find the RPM Bin
	///////////////////
	//start at the end of the rpm bins
	rpmBins = (map->rpm_bins) + RPM_BIN_COUNT - 1;
	rpmBin = RPM_BIN_COUNT - 1;

	//determine our base RPM bin	
	while (currentRPM < *rpmBins && rpmBin > 0){
		rpmBins--;
		rpmBin--;	
	}
	//determine the next RPM bin
	//check edge conditions
	//RPM below the lowest load bin?
	if (rpmBin == 0 && currentRPM < *rpmBins){
		nextRpmBin = 0;	
	}
	else{ 
		nextRpmBin = rpmBin;
		//RPM above the highest RPM Bin?
		if (rpmBin < RPM_BIN_COUNT - 1) nextRpmBin++;
	}

	////////////////////////
	//Find the Load Bin
	////////////////////////
	//start at the end of the load bins	
	loadBins = (map->load_bins) + LOAD_BIN_COUNT - 1;
	loadBin = LOAD_BIN_COUNT - 1;

	//determine our base Load bin
	while (currentLoad < *loadBins && loadBin > 0){
		loadBins--;
		loadBin--;	
	}
	//determine the next Load bin
	//check edge conditions
	//load below the lowest load bin?
	if (loadBin == 0 && currentLoad < *loadBins){
		nextLoadBin = 0;	
	}
	else{
		nextLoadBin = loadBin;
		//load above the highest load bin?
		if (loadBin < LOAD_BIN_COUNT - 1) nextLoadBin++;			
	}

	//get our advance values to feed into the linear
	//interpolation formulas
	advanceTable = map->ignition_advance;
	
	advance 				= advanceTable[loadBin][rpmBin];
	advance_next_rpm 		= advanceTable[loadBin][nextRpmBin];
	advance_next_load 		= advanceTable[nextLoadBin][rpmBin];
	advance_next_rpm_load 	= advanceTable[nextLoadBin][nextRpmBin];


	
	rpmBins = map->rpm_bins;
	loadBins = map->load_bins;

	//pre-fetch these because they are used twice		
	rpmBinValue = rpmBins[rpmBin];
	nextRpmBinValue = rpmBins[nextRpmBin];

	//perform linear interpolation
	//between rpmBin and nextRpmBin
	//for base Load bin
	//y = advance dimension; x = rpm dimension
	
	
	advanceRpmInterpolated = 
		linearInterpolate(currentRPM, 
							rpmBinValue,
							advance, 
							nextRpmBinValue,
							advance_next_rpm);
							
							
	//linear interpolate between rpmBin and nextRpmBin
	//for next Load bin
	//y = advance dimension; x = rpm dimension		
	advanceRpmInterpolatedNextLoad = 
		linearInterpolate(currentRPM,
							rpmBinValue,
							advance_next_load,
							nextRpmBinValue,
							advance_next_rpm_load);
							
	
	//final linear interpolation step
	//y = advance dimension; x = load dimension
	g_currentAdvance = 
		linearInterpolate(currentLoad,
							loadBins[loadBin],
							advanceRpmInterpolated,
							loadBins[nextLoadBin],
							advanceRpmInterpolatedNextLoad);

	//save the global values
	g_currentLoadBin = loadBin;
	g_currentRPMBin = rpmBin;
}

void onRevolutionCalculation(){
	ToggleLED(LED_2);

	unsigned int crankPeriodUSec = calculateCrankPeriodUSec();
	calculateRPM(crankPeriodUSec);
	calculateDwellDegrees(crankPeriodUSec);
	g_lastCrankRevolutionPeriodUSec = crankPeriodUSec;
	g_currentLoad = 20;
	calculateAdvance();

	updateLogicalCoilDriverRuntimes(g_logical_coil_drivers);
//				updateUserOutputs();
}

void initIgnition(){

	AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, COIL_DRIVER_ALL_PORTS );
	AT91F_PIO_SetOutput( AT91C_BASE_PIOA, COIL_DRIVER_ALL_PORTS ) ;


	initRuntimeData();

	triggerWheelTimerInit();
	coilPackChargeTimerInit();
	coilPackFireTimerInit();

	enableIRQ();

	AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, COILS_ENABLE );
	AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, COILS_ENABLE );

	DisableLED(LED_1);
	DisableLED(LED_2);
}

void processIgnition(){
	if (g_recalculatePending) onRevolutionCalculation();
	g_recalculatePending = 0;
}





///Ignition Related API Commands


void SendResult(int success){
	SendNameInt("result",success);
	SendCrlf();
}

void enableLockedAdvance(unsigned int argc, char **argv){
	if (argc > 1){
		g_lockedAdvance = modp_atoi(argv[1]);
	}
	g_lockedAdvanceEnabled = 1;
	SendResult(1);
}

void disableLockedAdvance(unsigned int argc, char **argv){
	g_lockedAdvanceEnabled = 0;
	SendResult(1);
}

void setIgnitionCell(unsigned int argc, char **argv){

	if (argc < 4){
		SendResult(0);
		return;
	}

	//TODO validate params
	int rpmBin = modp_atoi(argv[1]);
	int loadBin = modp_atoi(argv[2]);
	int value = modp_atoi(argv[3]);

	g_selected_ignition_config->map.ignition_advance[loadBin][rpmBin] = value;
	SendResult(1);

}

void setIgnitionRPMBin(unsigned int argc, char **argv){

	if (argc < 3){
		SendResult(0);
		return;
	}

	//TODO validate params
	int rpmBin = modp_atoi(argv[1]);
	int value = modp_atoi(argv[2]);

	g_selected_ignition_config->map.rpm_bins[rpmBin] = value;
	SendResult(1);
}

void setIgnitionLoadBin(unsigned int argc, char **argv){
	if (argc < 3){
		SendResult(0);
		return;
	}

	//TODO validate params
	int loadBin = modp_atoi(argv[1]);
	int value = modp_atoi(argv[2]);

	g_selected_ignition_config->map.load_bins[loadBin] = value;
	SendResult(1);
}

void getRuntime(unsigned int argc, char **argv){
	SendNameUint("rpm",g_currentRPM);
	SendNameUint("load",g_currentLoad);
	SendNameInt("adv",g_currentAdvance);
	SendNameUint("rpmBin",g_currentRPMBin);
	SendNameUint("loadBin", g_currentLoadBin);
	SendNameUint("dwell", g_currentDwellUSec);
	SendCrlf();
}

void getEngineConfig(unsigned int argc, char **argv){
	SendNameUint("cylCount",g_active_engine_config.cylinderCount);
	SendNameUint("missingToothBtdcDeg", g_active_engine_config.missingToothBTDCDegrees);
	SendNameUint("crankingDeg", g_active_engine_config.cranking_degrees);

	for (int i=0; i < MAX_CYLINDERS; i++){
		struct cylinder_config * cylConfig = &g_active_engine_config.cylinderConfigs[i];
		SendNameIndexInt("tdcFireDeg", i, cylConfig->tdcFireDegrees);
		SendNameIndexInt("coilDriver", i, cylConfig->coilDriver);
	}
	SendCrlf();
}

void getRpmBins(unsigned int argc, char **argv){
	for (int i=0; i < RPM_BIN_COUNT; i++){
		SendNameIndexInt("rpmBin", i, g_selected_ignition_config->map.rpm_bins[i]);
	}
	SendCrlf();
}

void getLoadBins(unsigned int argc, char **argv){
	for (int i=0; i < LOAD_BIN_COUNT; i++){
		SendNameIndexInt("loadBin", i, g_selected_ignition_config->map.load_bins[i]);
	}
	SendCrlf();
}

void getIgnMap(unsigned int argc, char **argv){
	int i=0;
	for (int l=0; l < LOAD_BIN_COUNT; l++){
		for (int r=0; r < RPM_BIN_COUNT; r++){
			SendNameIndexInt("adv",i++,g_selected_ignition_config->map.ignition_advance[l][r]);
		}
	}
	SendCrlf();
}

void getUserOutCfg(unsigned int argc, char **argv){
	if (argc < 2){
		SendResult(0);
		return;
	}
	int i= modp_atoi(argv[1]);
	if (i < 0 || i > USER_DEFINED_OUTPUTS - 1){
		SendResult(0);
		return;
	}

	struct output_config * outputConfig = &g_selected_ignition_config->user_defined_outputs[i];
	SendNameIndexInt("userOutType", i, outputConfig->output_type);
	SendNameIndexInt("userOutMode", i, outputConfig->output_mode);
	SendNameIndexInt("userOutTrigger", i, outputConfig->trigger_threshold);
	SendCrlf();
}

void setUserOutCfg(unsigned int argc, char **argv){
	if (argc < 5){
		SendResult(0);
		return;
	}
	int userOut = modp_atoi(argv[1]);
	int type = modp_atoi(argv[2]);
	int mode = modp_atoi(argv[3]);
	int trigger = modp_atoi(argv[4]);

	if (userOut < 0 || userOut > USER_DEFINED_OUTPUTS -1 ){
		SendResult(0);
		return;
	}

	switch (type){
		case OUTPUT_TYPE_RPM:
		case OUTPUT_TYPE_MAP:
			break;
		default:
			SendResult(0);
			return;
	}

	switch (mode){
		case MODE_NORMAL:
		case MODE_INVERTED:
			break;
		default:
			SendResult(0);
			return;
	}

	//TODO validate trigger threshold
	//if (trigger < 0 || trigger > MAX_RPM){
	//	SendResult(0);
	// return;
	//}
	struct output_config * outputConfig = &g_selected_ignition_config->user_defined_outputs[userOut];
	outputConfig->output_type = type;
	outputConfig->output_mode = mode;
	outputConfig->trigger_threshold = trigger;

	SendResult(1);
}

void getDebug(unsigned int argc, char **argv){


	SendString("==========================================");
	SendCrlf();
	SendString("Activity for Crank Teeth 0 -> ");
	SendInt(CRANK_TEETH - 1);
	SendCrlf();
	SendString("==========================================");
	SendCrlf();

	SendString("Coil Fire Port          ");
	for (int i = 0; i < CRANK_TEETH; i++){
		SendUint(g_coilFirePort[i]);
		SendString(" ");
	}
	SendCrlf();
	SendString("Coil Fire Timer Count   ");
	for (int i = 0; i < CRANK_TEETH; i++){
		SendUint(g_coilFireTimerCount[i]);
		SendString(" ");
	}
	SendCrlf();
	SendString("Coil Charge Port        ");
	for (int i = 0; i < CRANK_TEETH; i++){
		SendUint(g_coilChargePort[i]);
		SendString(" ");
	}
	SendCrlf();
	SendString("Coil Charge Timer Count ");
	for (int i = 0; i < CRANK_TEETH; i++){
		SendUint(g_coilChargeTimerCount[i]);
		SendString(" ");
	}
	SendCrlf();
	SendCrlf();

	int count = 0;
	struct logical_coil_driver *logicalCoilPack = g_logical_coil_drivers;

	while (count < g_logicalCoilDriverCount){
		SendString("==========================================");
		SendCrlf();
		SendString("Logical Coil Pack                   ");
		SendInt(count);
		SendCrlf();
		SendString("==========================================");
		SendCrlf();
		SendString("Cylinder TDC                        ");
		SendInt(logicalCoilPack->cylinderTDC);
		SendCrlf();
		SendString("Physical Driver Ports               ");
		SendUint(logicalCoilPack->physicalCoilDriverPorts);
		SendCrlf();
		SendCrlf();

		SendString("Timer Fire Count                    ");
		SendUint(logicalCoilPack->timerFireCount);
		SendCrlf();
		SendString("Coil Fire Degrees                   ");
		SendInt(logicalCoilPack->coilFireDegrees);
		SendCrlf();
		SendString("Coil Fire Tooth                     ");
		SendInt(logicalCoilPack->coilFireTooth);
		SendCrlf();
		SendCrlf();

		SendString("Timer Charge Count                  ");
		SendUint(logicalCoilPack->timerChargeCount);
		SendCrlf();
		SendString("Coil Charge Degrees                 ");
		SendInt(logicalCoilPack->coilChargeDegrees);
		SendCrlf();
		SendString("Coil Charge Tooth                   ");
		SendInt(logicalCoilPack->coilChargeTooth);
		SendCrlf();
		SendCrlf();

		logicalCoilPack++;
		count++;
	}

	SendString("==========================================");
	SendCrlf();
	SendString("Runtime Variables");
	SendCrlf();
	SendString("==========================================");
	SendCrlf();
	SendString("Engine Is running                   ");
	SendUint(g_engineIsRunning);
	SendCrlf();
	SendString("Wheel Synchronized                  ");
	SendUint(g_wheelSynchronized);
	SendCrlf();
	SendString("Current RPM                         ");
	SendUint(g_currentRPM);
	SendCrlf();
	SendString("Current Load                        ");
	SendUint(g_currentLoad);
	SendCrlf();
	SendString("Current Load Bin                    ");
	SendUint(g_currentLoadBin);
	SendCrlf();
	SendString("Current RPM Bin                     ");
	SendUint(g_currentRPMBin);
	SendCrlf();
	SendString("Current Advance                     ");
	SendUint(g_currentAdvance);
	SendCrlf();
	SendString("Current Dwell Degrees               ");
	SendUint(g_currentDwellDegrees);
	SendCrlf();
	SendString("Current Dwell USec                  ");
	SendUint(g_currentDwellUSec);
	SendCrlf();
	SendString("Current Crank RevolutionPeriod Raw  ");
	SendUint(g_currentCrankRevolutionPeriodRaw);
	SendCrlf();
	SendString("Last Crank Revolution Period Raw    ");
	SendUint(g_lastCrankRevolutionPeriodRaw);
	SendCrlf();
	SendString("Last Crank Revolution USec          ");
	SendUint(g_lastCrankRevolutionPeriodUSec);
	SendCrlf();
	SendString("Last Inter-tooth Period Raw         ");
	SendUint(g_lastInterToothPeriodRaw);
	SendCrlf();
	SendString("Current Tooth Period Overflow Count ");
	SendUint(g_currentToothPeriodOverflowCount);
	SendCrlf();
	SendString("Current Tooth                       ");
	SendUint(g_currentTooth);
	SendCrlf();
	SendString("Wheel Sync Attempts                 ");
	SendUint(g_wheelSyncAttempts);
	SendCrlf();
	SendString("Tooth Count at Last Sync Attemp:    ");
	SendUint(g_toothCountAtLastSyncAttempt);
	SendCrlf();
	SendString("Coil Drivers to Fire                ");
	SendUint(g_coilDriversToFire);
	SendCrlf();
	SendString("Coil Drivers to Charge              ");
	SendUint(g_coilDriversToCharge);
	SendCrlf();
}
