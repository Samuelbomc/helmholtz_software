import os
import datetime
import pandas as pd
from time import time as now
from load_gmat import *

class GMATScriptCreator:
    """
    Generates and executes a GMAT script based on a configuration dictionary.
    This class combines the flexibility of Python configuration with a proven,
    robust GMAT script structure.
    """
    
    _defaults = {
        # Orbital Elements
        'object_name': 'TestSat',
        'SMA': 6800.895,    # Semimajor Axis (km)
        'ECC': 0.0010563,   # Eccentricity
        'INC': 51.57977,    # Inclination (deg)
        'RAAN': 327.87345,   # Right Ascension of the Ascending Node (deg)
        'AOP': 43.91870,    # Argument of Perigee (deg)
        'TA': 315.92945,    # True Anomaly (deg)
        
        # Physical Properties
        'DryMass': 450,
        'Cd': 2.2,
        'Cr': 1.8,
        'DragArea': 25.0,
        
        # Simulation Settings
        'simulation_duration_sec': 90 * 60, # Total simulation time in seconds
        'simulation_step_sec': 10,          # Data reporting step in seconds
        'Epoch': datetime.datetime.fromtimestamp(now()).strftime('%d %b %Y %H:%M:%S.%f')[:-3],
        
        # Force Model Settings
        'RelativisticCorrection': 'Off',
        'AtmosphereModel': 'MSISE90',
        
        # File Paths
        'script_path': '.', # Script and report directory
        'report_path': 'GMAT-Mission-Report.txt', # Report file name
    }

    def __init__(self, config: dict = None, **kwargs):
        """
        Initializes the script creator.
        """
        self.params = self._defaults.copy()
        if config:
            self.params.update(config)
        self.params.update(kwargs)
        self.params['script_path'] = os.path.abspath(self.params['script_path'])
        if not os.path.exists(self.params['script_path']):
            os.makedirs(self.params['script_path'])
            print(f"✅ Created script directory: {self.params['script_path']}")

    def create_script(self):
        """Generates the GMAT script file."""
        script_content = self._build_script()
        script_filename = os.path.join(self.params['script_path'], f"{self.params['object_name']}.script")
        
        with open(script_filename, "w") as f:
            f.write(script_content)
        print(f"✅ GMAT script created at: {script_filename}")

    def execute_simulation(self, get_info=False):
        """Loads and runs the generated GMAT script."""
        gmat.Clear()
        script_filename = os.path.join(self.params['script_path'], f"{self.params['object_name']}.script")
        print("🛰️  Running GMAT simulation...")
        gmat.LoadScript(script_filename)
        gmat.RunScript()
        print("✅ Orbit simulation completed.")
        if get_info:
            gmat.Help(f"{self.params['object_name']}")

    def load_mission_report(self) -> pd.DataFrame:
        """Loads the mission report file into a pandas DataFrame, explicitly defining column names."""
        report_filename = os.path.join(self.params['script_path'], self.params['report_path'])
        print(f"📊 Loading mission report from: {report_filename}")

        # FIX: Define the column names exactly as requested in the GMAT script
        column_names = [
            f"{self.params['object_name']}.UTCModJulian",
            f"{self.params['object_name']}.A1ModJulian",
            f"{self.params['object_name']}.Earth.Latitude",
            f"{self.params['object_name']}.Earth.Longitude",
            f"{self.params['object_name']}.Earth.Altitude"
        ]

        try:
            # Skip the header row in the file and apply our own column names
            return pd.read_fwf(report_filename, names=column_names, skiprows=1)
        except FileNotFoundError:
            print(f"❌ Error: Report file not found at {report_filename}. GMAT may have failed silently.")
            return pd.DataFrame()

    def _build_script(self) -> str:
        """Constructs the full GMAT script string from templates."""
        p = self.params
        created_time = datetime.datetime.fromtimestamp(now()).strftime('%Y-%m-%d %H:%M:%S')

        script_title = f"""%General Mission Analysis Tool(GMAT) Script
%Created: {created_time}
"""

        script_spacecraft = f"""
%----------------------------------------
%---------- Spacecraft
%----------------------------------------
Create Spacecraft {p['object_name']};
GMAT {p['object_name']}.DateFormat = UTCGregorian;
GMAT {p['object_name']}.Epoch = '{p['Epoch']}';
GMAT {p['object_name']}.CoordinateSystem = EarthMJ2000Eq;
GMAT {p['object_name']}.DisplayStateType = Keplerian;
GMAT {p['object_name']}.SMA = {p['SMA']};
GMAT {p['object_name']}.ECC = {p['ECC']};
GMAT {p['object_name']}.INC = {p['INC']};
GMAT {p['object_name']}.RAAN = {p['RAAN']};
GMAT {p['object_name']}.AOP = {p['AOP']};
GMAT {p['object_name']}.TA = {p['TA']};
GMAT {p['object_name']}.DryMass = {p['DryMass']};
GMAT {p['object_name']}.Cd = {p['Cd']};
GMAT {p['object_name']}.Cr = {p['Cr']};
GMAT {p['object_name']}.DragArea = {p['DragArea']};
GMAT {p['object_name']}.SRPArea = 1;
GMAT {p['object_name']}.SPADDragScaleFactor = 1;
GMAT {p['object_name']}.SPADSRPScaleFactor = 1;
GMAT {p['object_name']}.AtmosDensityScaleFactor = 1;
GMAT {p['object_name']}.ExtendedMassPropertiesModel = 'None';
GMAT {p['object_name']}.NAIFId = -10000001;
GMAT {p['object_name']}.NAIFIdReferenceFrame = -9000001;
GMAT {p['object_name']}.OrbitColor = Red;
GMAT {p['object_name']}.TargetColor = Teal;
GMAT {p['object_name']}.OrbitErrorCovariance = [ 1e+70 0 0 0 0 0 ; 0 1e+70 0 0 0 0 ; 0 0 1e+70 0 0 0 ; 0 0 0 1e+70 0 0 ; 0 0 0 0 1e+70 0 ; 0 0 0 0 0 1e+70 ];
GMAT {p['object_name']}.CdSigma = 1e+70;
GMAT {p['object_name']}.CrSigma = 1e+70;
GMAT {p['object_name']}.Id = 'SatId';
GMAT {p['object_name']}.Attitude = CoordinateSystemFixed;
GMAT {p['object_name']}.SPADSRPInterpolationMethod = Bilinear;
GMAT {p['object_name']}.SPADSRPScaleFactorSigma = 1e+70;
GMAT {p['object_name']}.SPADDragInterpolationMethod = Bilinear;
GMAT {p['object_name']}.SPADDragScaleFactorSigma = 1e+70;
GMAT {p['object_name']}.AtmosDensityScaleFactorSigma = 1e+70;
GMAT {p['object_name']}.ModelFile = 'aura.3ds';
GMAT {p['object_name']}.ModelOffsetX = 0;
GMAT {p['object_name']}.ModelOffsetY = 0;
GMAT {p['object_name']}.ModelOffsetZ = 0;
GMAT {p['object_name']}.ModelRotationX = 0;
GMAT {p['object_name']}.ModelRotationY = 0;
GMAT {p['object_name']}.ModelRotationZ = 0;
GMAT {p['object_name']}.ModelScale = 1;
GMAT {p['object_name']}.AttitudeDisplayStateType = 'Quaternion';
GMAT {p['object_name']}.AttitudeRateDisplayStateType = 'AngularVelocity';
GMAT {p['object_name']}.AttitudeCoordinateSystem = EarthMJ2000Eq;
GMAT {p['object_name']}.EulerAngleSequence = '321';
"""

        script_force_models = f"""
%----------------------------------------
%---------- ForceModels
%----------------------------------------
Create ForceModel DefaultProp_ForceModel;
GMAT DefaultProp_ForceModel.CentralBody = Earth;
GMAT DefaultProp_ForceModel.PrimaryBodies = {{Earth}};
GMAT DefaultProp_ForceModel.PointMasses = {{Luna, Sun}};
GMAT DefaultProp_ForceModel.SRP = On;
GMAT DefaultProp_ForceModel.RelativisticCorrection = {p['RelativisticCorrection']};
GMAT DefaultProp_ForceModel.ErrorControl = RSSStep;
GMAT DefaultProp_ForceModel.GravityField.Earth.Degree = 4;
GMAT DefaultProp_ForceModel.GravityField.Earth.Order = 4;
GMAT DefaultProp_ForceModel.GravityField.Earth.StmLimit = 100;
GMAT DefaultProp_ForceModel.GravityField.Earth.PotentialFile = 'JGM3.cof';
GMAT DefaultProp_ForceModel.GravityField.Earth.TideModel = 'None';
GMAT DefaultProp_ForceModel.Drag.AtmosphereModel = {p['AtmosphereModel']};
GMAT DefaultProp_ForceModel.Drag.HistoricWeatherSource = 'ConstantFluxAndGeoMag';
GMAT DefaultProp_ForceModel.Drag.PredictedWeatherSource = 'ConstantFluxAndGeoMag';
GMAT DefaultProp_ForceModel.Drag.CSSISpaceWeatherFile = 'SpaceWeather-All-v1.2.txt';
GMAT DefaultProp_ForceModel.Drag.SchattenFile = 'SchattenPredict.txt';
GMAT DefaultProp_ForceModel.Drag.F107 = 150;
GMAT DefaultProp_ForceModel.Drag.F107A = 150;
GMAT DefaultProp_ForceModel.Drag.MagneticIndex = 3;
GMAT DefaultProp_ForceModel.Drag.SchattenErrorModel = 'Nominal';
GMAT DefaultProp_ForceModel.Drag.SchattenTimingModel = 'NominalCycle';
GMAT DefaultProp_ForceModel.Drag.DragModel = 'Spherical';
GMAT DefaultProp_ForceModel.SRP.Flux = 1367;
GMAT DefaultProp_ForceModel.SRP.SRPModel = Spherical;
GMAT DefaultProp_ForceModel.SRP.Nominal_Sun = 149597870.691;
"""

        script_propagators = f"""
%----------------------------------------
%---------- Propagators
%----------------------------------------
Create Propagator DefaultProp;
GMAT DefaultProp.FM = DefaultProp_ForceModel;
GMAT DefaultProp.Type = RungeKutta89;
GMAT DefaultProp.InitialStepSize = 60;
GMAT DefaultProp.Accuracy = 9.999999999999999e-12;
GMAT DefaultProp.MinStep = 0.001;
GMAT DefaultProp.MaxStep = 2700;
GMAT DefaultProp.MaxStepAttempts = 50;
GMAT DefaultProp.StopIfAccuracyIsViolated = true;
"""
        
        report_full_path = os.path.join(p['script_path'], p['report_path'])
        gmat_compatible_path = report_full_path.replace('\\', '/')
        
        script_subscribers = f"""
%----------------------------------------
%---------- Subscribers
%----------------------------------------
Create ReportFile ReportFile1;
GMAT ReportFile1.SolverIterations = Current;
GMAT ReportFile1.UpperLeft = [ 0 0 ];
GMAT ReportFile1.Size = [ 0 0 ];
GMAT ReportFile1.RelativeZOrder = 0;
GMAT ReportFile1.Maximized = false;
GMAT ReportFile1.Filename = '{gmat_compatible_path}';
GMAT ReportFile1.Precision = 16;
GMAT ReportFile1.Add = {{{p['object_name']}.UTCModJulian, {p['object_name']}.A1ModJulian, {p['object_name']}.Earth.Latitude, {p['object_name']}.Earth.Longitude, {p['object_name']}.Earth.Altitude}};
GMAT ReportFile1.WriteHeaders = true;
GMAT ReportFile1.LeftJustify = On;
GMAT ReportFile1.ZeroFill = On;
GMAT ReportFile1.FixedWidth = true;
GMAT ReportFile1.Delimiter = ' ';
GMAT ReportFile1.ColumnWidth = 23;
GMAT ReportFile1.WriteReport = true;
"""

        num_steps = int(p['simulation_duration_sec'] / p['simulation_step_sec'])
        script_mission_sequence = f"""
%----------------------------------------
%---------- Mission Sequence
%----------------------------------------
Create Variable i;
BeginMissionSequence;
For i = 1:{num_steps}
   Propagate DefaultProp({p['object_name']}) {{{p['object_name']}.ElapsedSecs = {p['simulation_step_sec']}}};
EndFor;
"""
        
        return (script_title + script_spacecraft + script_force_models + 
                script_propagators + script_subscribers + script_mission_sequence)