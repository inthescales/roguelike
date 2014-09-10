CC = g++
#OLDLDFLAGS = -lpdcurses
LDFLAGS = -lncursesw

all : build

compile :
	@${CC} -c *.cpp
	@echo Compilation complete!

link :
	@${CC} -o Rogue *.o ${LDFLAGS}
	@echo Linking complete!

build : compile link
	@echo Build complete!

defs :
	@makedefs.rb

clean :
	@rm *.o
	@rm *.rsv
	@echo Cleaned!

clear :
	@rm *.rsv
	@echo Saves cleared!
