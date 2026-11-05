DEBUG =   -O3 						  # to debug use -g -O0

# Intel compilers
#CC =   icc  $(DEBUG) 
#LINK =   icc  $(DEBUG) 

# GNU compilers
CC =   gcc  $(DEBUG) 
LINK =   gcc -lm $(DEBUG) 

OBJ = ./obj/
EXE = ./exe/

# Subroutines

$(OBJ)int-ext.o: int-ext.c Makefile
	$(CC) -c int-ext.c  -o $(OBJ)int-ext.o

$(OBJ)magfdz.o: magfdz.c Makefile
	$(CC) -c magfdz.c  -o $(OBJ)magfdz.o

$(OBJ)plmbar.o: plmbar.c Makefile
	$(CC) -c plmbar.c  -o $(OBJ)plmbar.o

$(OBJ)pomme.o: pomme.c POMME11_core.h POMME11_crust.h POMME11_ext.h POMME11_gsm2geo.h POMME11_sm2geo.h POMME11_qfac.h pomme.h Makefile
	${CC} -c pomme.c -o $(OBJ)pomme.o 

$(OBJ)sm2geo.o: sm2geo.c Makefile
	${CC} -c sm2geo.c -o $(OBJ)sm2geo.o

$(OBJ)gsm2geo.o: gsm2geo.c Makefile
	${CC} -c gsm2geo.c -o $(OBJ)gsm2geo.o

$(OBJ)geod2geoc.o: geod2geoc.c Makefile
	${CC} -c geod2geoc.c -o $(OBJ)geod2geoc.o

#pomme_calc

$(OBJ)pomme_calc.o: pomme_calc.c  pomme.h  Makefile
	$(CC) -c pomme_calc.c  -o $(OBJ)pomme_calc.o

pomme_calc_obj = $(OBJ)pomme_calc.o $(OBJ)pomme.o $(OBJ)geod2geoc.o $(OBJ)gsm2geo.o $(OBJ)sm2geo.o\
				   $(OBJ)plmbar.o $(OBJ)magfdz.o $(OBJ)int-ext.o

pomme_calc: $(pomme_calc_obj) Makefile
	$(LINK) $(pomme_calc_obj)  -o $(EXE)pomme_calc
