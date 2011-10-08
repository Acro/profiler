#!/bin/sh
if test ! -f "$1" 
then
	echo "Error: executable $1 does not exist."
	exit 1
fi
if test ! -f "$2" 
then
	echo "Error: trace log $2 does not exist."
	exit 1
fi

EXECUTABLE="$1"
TRACELOG="$2"

while read LINETYPE FADDR CADDR CTIME CSEC; do

	FNAME="$(addr2line -f -e ${EXECUTABLE} ${FADDR}|head -1)"
	CDATE="$(date -Iseconds -d @${CTIME})"

	if test "${LINETYPE}" = "e" 
	then
		CNAME="$(addr2line -f -e ${EXECUTABLE} ${CADDR}|head -1)"
		CLINE="$(addr2line -s -e ${EXECUTABLE} ${CADDR})"
		echo "Enter ${FNAME} at ${CDATE}, called from ${CNAME} (${CLINE})"

		RUN_T[$FNAME]=$CTIME
		RUN_S[$FNAME]=$CSEC
	fi
	
	if test "${LINETYPE}" = "x" 
	then
		echo "Exit ${FNAME} at ${CDATE}"
	
		if test ${CTIME} = ${RUN_T[$FNAME]} 			# pokud jsou timestampy ve stejne sekunde, jen odecteme
		then
			echo ${FNAME}\(\) runtime = 0.$(((${CSEC})-(${RUN_S[$FNAME]}))) second
		fi

		if test ${CTIME} -gt $(((${RUN_T[$FNAME]})+(1))) 	# provede se, pokud se timestampy lisi v sekundach
		then
			TOTAL_SEC=$(((($CTIME)-(${RUN_T[$FNAME]})-1)))
			TOTAL_MSEC=$((($CSEC)+((1000000)-(${RUN_S[$FNAME]}))))
			if test $TOTAL_MSEC -gt 1000000 
			then
				TOTAL_MSEC=$((($TOTAL_MSEC)-(1000000)))
				TOTAL_SEC=$((($TOTAL_SEC)+(1)))
			fi
			echo ${FNAME}\(\) runtime = $TOTAL_SEC.$TOTAL_MSEC seconds
		elif test ${CTIME} -gt ${RUN_T[$FNAME]}			# provede se, pokud se timestampy lisi jen o 1 sekundu
		then
			SEC_PASSED=$((($CSEC)+(1000000)-(${RUN_S[$FNAME]})))

			if test 1000000 -gt $SEC_PASSED
			then
				echo ${FNAME}\(\) runtime = 0.$SEC_PASSED second
			else
				echo ${FNAME}\(\) runtime = 1.$((($SEC_PASSED)-(1000000))) second
			fi
		fi

	fi

done < "${TRACELOG}"
