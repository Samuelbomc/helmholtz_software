## GMAT Link
These classes are used to link GMAT to the rest of the code. They are not meant to be used by the user, but rather to be used by the visualization engine with the user input.

### Ejemplo de Configuración

* ObjectName=Sat1
* Epoch=01 Jan 2026 00:00:00.000
* SMA=7000.0
* ECC=0.001
* INC=98.0
* RAAN=20.0
* AOP=40.0
* TA=0.0
* DryMass=12
* Cd=2.2
* Cr=1.8
* DragArea=0.08
* SRPArea=0.08
* RelativisticCorrection=Off
* AtmosphereModel=NRLMSISE00
* GMAT_data_path=D:/tmp/gmat
* report_name=orbit_report.txt
* script_name=mission.script
* simulation_duration_sec=86400
* simulation_step_sec=60

#### Optional overrides

* DateFormat=UTCGregorian
* CoordinateSystem=EarthMJ2000Eq
* PrimaryBodies={Earth}
* PointMasses={Luna, Sun}
* SRP=On
* EarthGravityDegree=8
* EarthGravityOrder=8
* DragModel='Spherical'
* PropagatorType=RungeKutta89
* InitialStepSize=30
* Accuracy=1e-12
* MinStep=0.001
* MaxStep=120
* MaxStepAttempts=50
* StopIfAccuracyIsViolated=true
* ReportPrecision=16