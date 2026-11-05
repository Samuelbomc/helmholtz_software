#define GEO2GEO 0
#define GEO2GSE 1
#define GSE2GEO 2
#define GEO2GSM 3
#define GSM2GEO 4
#define GEO2SM 5
#define SM2GEO 6
#define GEO2SM_NP 7
#define SM2GEO_NP 8
#define GEO2SM_SP 9
#define SM2GEO_SP 10
/* IGRF-12 */
#define G10  (-29442.0)        /* for dipole latitude */
#define G11  ( -1501.0)  
#define H11     4797.1
#define MOM  (sqrt(G10*G10 + G11*G11 + H11*H11))  
