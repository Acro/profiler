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
I=0

while read LINETYPE FADDR CADDR CTIME CSEC; do

	FNAME="$(addr2line -f -e ${EXECUTABLE} ${FADDR}|head -1)"
	CDATE="$(date -Iseconds -d @${CTIME})"

	if test "${LINETYPE}" = "e" 
	then
		CNAME="$(addr2line -f -e ${EXECUTABLE} ${CADDR}|head -1)"
		CLINE="$(addr2line -s -e ${EXECUTABLE} ${CADDR})"

		# echo "Enter ${FNAME} at ${CDATE}, called from ${CNAME} (${CLINE})"
		# printf "FADDR = %s ... FNAME = %s\n" $FADDR $FNAME
		
		#printf "%s\n" ${RUN_T[$FADDR]}

		#[ "$TEMP" ] || TEMP=
		[ "$ADDR_TEMP" ] || ADDR_TEMP=

		#if [ ${CTIME}-ne${TEMP} ];
		#if test $CTIME -ne $TEMP
	 	if [ "$FADDR" != "$ADDR_TEMP" ]
		then
			RUN_T[$FADDR]=$CTIME
			RUN_S[$FADDR]=$CSEC
			#echo New CTEMP value = $CTIME
			AR[$I]=$FADDR
			I=$((I+1))
			#TEMP=$CTIME

			ADDR_TEMP=$FADDR
		fi

		#echo ${AR[$(($I-1))]}

	fi
	
	if test "${LINETYPE}" = "x" 
	then
		# echo "Exit ${FNAME} at ${CDATE}"
	
		#printf "Pocet elementu v poli: %d\n" ${#RUN_T[@]}    

		if test ${CTIME} -eq ${RUN_T[($FADDR)]} 			# pokud jsou timestampy ve stejne sekunde, jen odecteme
		then
			printf "%s() runtime = 0.%06d second\n" ${FNAME} $(((${CSEC})-(${RUN_S[$FADDR]})))
		fi

		if test ${CTIME} -gt $(((${RUN_T[($FADDR)]})+(1))) 	# provede se, pokud se timestampy lisi v sekundach
		then
			TOTAL_SEC=$(((($CTIME)-(${RUN_T[($FADDR)]})-1)))
			TOTAL_MSEC=$((($CSEC)+((1000000)-(${RUN_S[($FADDR)]}))))
			if test $TOTAL_MSEC -gt 1000000 
			then
				TOTAL_MSEC=$((($TOTAL_MSEC)-(1000000)))
				TOTAL_SEC=$((($TOTAL_SEC)+(1)))
			fi
			printf "%s() runtime = %d.%06d seconds\n" ${FNAME} $TOTAL_SEC $TOTAL_MSEC
		elif test ${CTIME} -gt ${RUN_T[$FADDR]}			# provede se, pokud se timestampy lisi jen o 1 sekundu
		then
			SEC_PASSED=$((($CSEC)+(1000000)-(${RUN_S[$FADDR]})))

			if test 1000000 -gt $SEC_PASSED
			then
				printf "%s() runtime = 0.%06d second\n" ${FNAME} $SEC_PASSED
			else
				printf "%s() runtime = 1.%06d second\n" ${FNAME} $((($SEC_PASSED)-(1000000)))
			fi
		fi
	fi

done < "${TRACELOG}"


