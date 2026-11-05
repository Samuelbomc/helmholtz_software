#pragma once
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "../models/pomme.h"
#include "Config.h"

// POMME11 Constants
#define R_E               6371.2  /* Magnetic reference radius  */
#define MAX_0_DEG         720     /* Max degree of static field */
#define MAX_0_NCOEFF      (MAX_0_DEG * (MAX_0_DEG + 2))
#define MAX_V_DEG         20      /* Max degree of time derivatives (all same) */
#define MAX_V_NCOEFF      (MAX_V_DEG * (MAX_V_DEG + 2))
#define MAX_EXT_DEG       3       /* Max degree of external field, limited to 3 */
#define MAX_EXT_NCOEFF    (MAX_EXT_DEG * (MAX_EXT_DEG + 2))  
#define POS_0             0       /* static internal */
#define POS_1             1       /* SV */
#define POS_SM            4       /* static ring current */
#define POS_SM_IND        5       /* if asymmetric then could induce field, not implemented */
#define POS_EST           6       /* external RC field */
#define POS_IST           7       /* induced RC field */
#define POS_GSM           8       /* external GSM static field */
#define POS_GSM_IND       9       /* induced internal*/
#define POS_IMFBY         10      /* external IMFBY correlated field */
#define POS_IMFBY_IND     11      /* induced internal*/
#define POS_EST_FAC       12      /* factor for Est/Ist field */
#define POS_F107          13      /* external F107 correlated field in SM */
#define POS_F107_IND      14      /* induced internal*/
#define POS_EM            15      /* external Em correlated field in GSM */
#define POS_EM_IND        16      /* induced internal*/
#define MAXCONTROL        17      /* number of control parameters */
#define MAX_S_COEFF       15       /* spatial coefficients, must change also in gsm2geo and sm2geo  */
#define MAX_T_COEFF       13       /* temporal coefficients */
#define MAXPERIOD         7        /* const, 24h, 12h, 8h ..., must change also in gsm2geo and sm2geo*/

//POMME11 Coefficients
#define DEG_IMFBY  1
#define DEG_EM     1
#define DEG_F107   2
#define DEG_SM     2
#define DEG_GSM    2
#define CONVENTION 1.0     /* 1.0 for exp(iwt), -1.0 for exp(-iwt) */
#define EM_MEAN    0.5     /* is subtracted to get variation around mean*/
#define EM_PEAK    8       /* Em approaches this peak assymptotically*/
#define F107_MEAN  120.0   /* is subtracted to get variation around mean*/
#define EST_FAC    0.8655
static double POMME_C_F107[8] = {
   0.0833, 0.0000, 0.0000,
   0.0000, -0.0018, 0.0010, 0.0000, 0.0000 };
static double POMME_C_EM[3] = {
   1.4453, 0.0000, 0.0000 };
static double POMME_C_IMFBY[3] = {
   0.0000, 0.0688, -0.2657 };
static double POMME_C_SM[8] = {
   5.7304, 0.0000, 0.0000,
   0.0000, -0.9266, 1.1032, 0.0000, 0.0000 };
static double POMME_C_GSM[8] = {
   8.2474, 0.1169, 0.5475,
   0.1253, -0.6716, -1.4288, -0.0140, 0.5469 };

typedef int tcontrol[MAXCONTROL];

class Simulation {
public:
    // Control loop timing
    void set_speed_multiplier(double value);
    void set_simulation_dt(double value);

	// Save and load configuration
    void saveConfigToFile(const std::string& filename);
    void loadConfigFromFile(const std::string& filename);

    void start_program();
private:
    std::map<std::string, double> config;
	std::string pomme11_exec_path = std::string(TOPLEVEL_PATH) + "/models/";

    static tcontrol control;

    static  double lon, geod_lat, geod_alt, fday;                      /* Position and time */
    static  double E_st, I_st, IMF_By, f107, Em;       /* Magnetic indices for time varying external field */
    static  double x, y, z, h, f, incl, decl;                         /* Resulting field */

	void set_pomme_control_parameters();
    void set_pomme_input_parameters(double fday, double geod_alt, double geod_lat, double lon,
		double E_st, double I_st, double IMF_By, double f107, double Em);

    void run_pomme();
};