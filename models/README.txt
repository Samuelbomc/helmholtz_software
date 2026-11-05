                 Potsdam Magnetic Model of the Earth (POMME11)
-----------------------------------------------------------------------------
                                source code


POMME is a geomagnetic field model. It represents not only the
main field from the Earth's core, but also a degree 133 (300 km
resolution) contribution from crustal magnetization, and a simple model 
of the magnetosphere. This magnetospheric contribution includes daily
and annually varying fields which induce electric currents in the Earth,
giving rise to secondary induced magnetic fields. These induced fields
are also represented in this model.

POMME uses a piece-wise linear representation of the Gauss coefficients
of the geomagnetic potential. For prediction, it uses a linear secular
variation model to extrapolate the field forward in time. 

The model coefficients are embedded into .h files, so that no coefficient 
files have to be read. Therefore the subroutines do not have to be initialized.

The model is more accurate if the present state of the magnetosphere
is provided as an input in the form of magnetic indices. Unfortunately,
these indices are presently not available in real-time, so any real-time
evaluation of the model has to set these indices to zero. 

For previous periods, the E_st and I_st indices are available from:
ftp://ftp.ngdc.noaa.gov/STP/GEOMAGNETIC_DATA/INDICES/EST_IST/Est_Ist_index.lis

The IMF-By is available from:
http://spidr.ngdc.noaa.gov/spidr/index.jsp
or from
http://omniweb.gsfc.nasa.gov/
The merging electric field Em is a parameter derived from these solar wind
inputs by:
Em (in mV/m) = V * BT * sin(beta/2.0) * sin(beta/2.0) / 1000
where V is the solar wind flow speed in km/s, BT is the strength of the
interplanetary magnetic field perpendicular to the Sun-Earth axis and
beta is the clock angle acos(Bz/BT). Use the default value of 0.5 mV/m 
if the value of Em is not known.

The F10.7 solar flux is available from the SPIDR data base at
http://spidr.ngdc.noaa.gov
IMPORTANT: The model requires the 81-day average value 20 months prior.

This archive contains one main program:
pomme_calc.c is a simple main program that will ask for the location and other
relevant input parameters and compute the magnetic field components. 

After adjusting the compiler options in 'Makefile', compile the program using:
make pomme_calc

The executable is then found in the subdirectory ./exe

The subroutine library in 'pomme.c' has an additional function pomme_geocentric
to evaluate the vector components of the magnetic field in geocentric coordinates
(instead of geodetic coordinates as used in pomme_calc.c)

An additional subroutine pomme_get_internal_coeff() provides the Gauss coefficients
for a given date.
