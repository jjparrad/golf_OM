#!/bin/sh
bindir=$(pwd)
cd /home/e20240011884/Documents/M1/Motor_de_jeux/TP4/src/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /home/e20240011884/Documents/M1/Motor_de_jeux/TP4/build/main 
	else
		"/home/e20240011884/Documents/M1/Motor_de_jeux/TP4/build/main"  
	fi
else
	"/home/e20240011884/Documents/M1/Motor_de_jeux/TP4/build/main"  
fi
