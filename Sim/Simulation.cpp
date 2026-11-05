#include "Simulation.h"

extern "C" {
void pomme_geodetic(int control[MAXCONTROL],
    double fday, double geod_alt, double geod_lat, double lon,
    double est, double ist, double imf_by, double f107, double Em,
    double* geod_x, double* geod_y, double* geod_z,
    double* h, double* f, double* incl, double* decl);
}

tcontrol Simulation::control = {};

double Simulation::lon = 0.0;
double Simulation::geod_lat = 0.0;
double Simulation::geod_alt = 0.0;
double Simulation::fday = 0.0;

double Simulation::E_st = 0.0;
double Simulation::I_st = 0.0;
double Simulation::IMF_By = 0.0;
double Simulation::f107 = 0.0;
double Simulation::Em = 0.0;

double Simulation::x = 0.0;
double Simulation::y = 0.0;
double Simulation::z = 0.0;
double Simulation::h = 0.0;
double Simulation::f = 0.0;
double Simulation::incl = 0.0;
double Simulation::decl = 0.0;

void Simulation::set_speed_multiplier(double value) {
    config["speed_multiplier"] = value;
}
void Simulation::set_simulation_dt(double value) {
    config["simulation_dt"] = value;
}

void Simulation::saveConfigToFile(const std::string& filename) {
    std::ofstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_Simulation");
    if (config_file.is_open()) {
        for (const auto& pair : config) {
            config_file << pair.first << "=" << pair.second << "\n";
        }
        config_file.close();
    } else {
        throw std::runtime_error("Unable to open file for writing: " + filename);
    }
}

void Simulation::loadConfigFromFile(const std::string& filename) {
    std::ifstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_Simulation");
    if (config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                double value = std::stod(line.substr(pos + 1));
                config[key] = value;
            }
        }
        config_file.close();
    } else {
        throw std::runtime_error("Unable to open file for reading: " + filename);
    }
}

void Simulation::run_pomme() {
	pomme_geodetic(control, fday, geod_alt, geod_lat, lon,
        E_st, I_st, IMF_By, f107, Em,
        &x, &y, &z,
		&h, &f, &incl, &decl);

    printf("\n\n");
    printf("    fday     lon     lat     alt    E_st    I_st   IMF_By     Em   F10.7-av\n");
    printf("----------------------------------------------------------------------------\n");
    printf("%8.1f %7.1f %7.1f %7.1f %7.1f %7.1f %7.1f %7.1f %9.1f\n\n", fday, lon, geod_lat, geod_alt, E_st, I_st, IMF_By, Em, f107);

    printf("        X          Y          Z          H          F       Incl       Decl\n");
    printf("----------------------------------------------------------------------------\n");
    printf("%10.2f %10.2f %10.2f %10.2f %10.2f %10.5f %10.5f\n\n", x, y, z, h, f, incl, decl);
}

void Simulation::set_pomme_control_parameters() {
    control[POS_0] = 133;   /* default: model.deg_int, 0=off */
    control[POS_1] = 15;    /* default: model.deg_1st, 0=off */

    control[POS_SM] = DEG_SM;       /* degree of SM field, default = (*model).deg_sm, 0=off */
    control[POS_SM_IND] = DEG_SM;       /* degree of field induced by SM field, default = (*model).deg_sm, 0=off */
    control[POS_F107] = DEG_F107;     /* degree of SM field, default = (*model).deg_sm, 0=off */
    control[POS_F107_IND] = DEG_F107;     /* degree of field induced by SM field, default = (*model).deg_sm, 0=off */
    control[POS_EST] = 1;                  /* Est dependent field: 1=on, 0=off */
    control[POS_IST] = 1;                  /* Ist dependent field: 1=on, 0=off */
    control[POS_EST_FAC] = 1;                  /* 1 = use model Est factor, 0 = use factor 1.0 */

    control[POS_GSM] = DEG_GSM;     /* degree of GSM field, default = (*model).deg_gsm, 0=off */
    control[POS_GSM_IND] = DEG_GSM;     /* degree of field induced by GSM field, default = (*model).deg_gsm, 0=off */
    control[POS_IMFBY] = 1;                 /* IMF-By dependent field: 1=on, 0=off */
    control[POS_IMFBY_IND] = 1;                 /* Induction by IMF-By dependent field: 1=on, 0=off */
    control[POS_EM] = DEG_EM;      /* degree of GSM field, default = (*model).deg_gsm, 0=off */
    control[POS_EM_IND] = DEG_EM;      /* degree of field induced by GSM field, default = (*model).deg_gsm, 0=off */
}

void Simulation::set_pomme_input_parameters(double fday, double geod_alt, double geod_lat, double lon,
    double E_st, double I_st, double IMF_By, double f107, double Em) {
    Simulation::fday = fday;
    Simulation::geod_alt = geod_alt;
    Simulation::geod_lat = geod_lat;
    Simulation::lon = lon;

    Simulation::E_st = E_st;
    Simulation::I_st = I_st;
    Simulation::IMF_By = IMF_By;
    Simulation::f107 = f107;
    Simulation::Em = Em;
}

void Simulation::start_program() {
    std::cout << "Starting simulation..." << std::endl;
	set_pomme_control_parameters();
    set_pomme_input_parameters(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    run_pomme();
}