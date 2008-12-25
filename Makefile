#TTY-Tetris MakeFile
#Under BSD License
#See clock.c for the license detail.

SRC = tetris.c
CC = cc
NCURSESFLAG = -lncurses
BIN = tty-tetris
INSTALLPATH = /usr/local/bin/
CFLAGS = -Wall ${NCURSESFLAG}


tty-tetris : ${SRC}

	@echo "build ${SRC}"
	@echo "CC ${CFLAGS} ${SRC}"
	@${CC} ${CFLAGS} ${SRC} -o ${BIN}

install : ${BIN}

	@echo "installing binary file to ${INSTALLPATH}${BIN}"
	@cp ${BIN} ${INSTALLPATH}
	@chmod 755 ${INSTALLPATH}${BIN}
	@echo "installed."

uninstall :

	@echo "uninstalling binary file (${INSTALLPATH}${BIN})"
	@rm -f ${INSTALLPATH}${BIN}
	@echo "uninstalled :'(."

clean :

	@echo "cleaning"
	@rm ${BIN}
	@echo "cleaned."

