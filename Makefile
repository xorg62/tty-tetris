#TTY-Tetris MakeFile
#Under BSD License
#See the top of *.c files for the license detail.

SRC = util.c frame.c shapes.c tetris.c
CC = cc
BIN = tty-tetris
INSTALLPATH = /usr/local/bin/
CFLAGS = -Wall


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
	@echo "rm ${BIN}"
	@rm ${BIN}
	@echo "cleaned."

