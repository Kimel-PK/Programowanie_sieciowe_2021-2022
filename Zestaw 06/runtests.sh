#!/bin/bash

# define max possible number in your program
max=4294967295				# for uint from limits.h
#max=2147483647				# for int from limits.h
#max=9223372036854775807	# for long from limits.h
#max=18446744073709551615	# for ulong from limits.h

path=tests
input=$path/inputs
result=$path/results
expect=$path/expected

echo ""
echo -e "[=================================]"
echo -e "[\u001b[36m Sumator TCP - Tester by Karosek \u001b[0m]"
echo -e "[=================================]"
echo ""

echo -e "[\u001b[33m#\u001b[0m] Creating folders..."
mkdir -p $path
mkdir -p $input
mkdir -p $result
mkdir -p $expect

declare -a input_data=(
	# Standard
	"1 1 1 1\x0D\x0A"								# OK
	"$max\x0D\x0A"									# OK
	$((max-5))" 2 3\x0D\x0A"						# OK
	$((max-5))" 10 20 30\x0D\x0A"					# ERROR
	"Hello World\x0D\x0A"							# ERROR
	"-5 -1 2.5 2\x0D\x0A"							# ERROR
	" 1 2 3\x0D\x0A"								# ERROR
	"1 2 3 \x0D\x0A"								# ERROR
	"1  2  3\x0D\x0A"								# ERROR
	"0\x0D\x0A"										# OK
	"1234567890 20 5 2 0 1\x0D\x0A"					# OK
	"999\x00\x0D\x0A"								# ERROR
	"0 0 0 0 0 0\x0D\x0A"							# OK
	"61 20 25 8 40 72 22 100 44\x0D\x0A"			# OK
	"1 2 3 4 5 6 7 8 9 10 11  12\x0D\x0A"			# ERROR
	"9 9 9 9 9 9 9 9 9 9 9 9 9 99\x0A\x0D\x0A"		# ERROR
	"1 1 1 1 1 1\x0D\x0D\x0A"						# ERROR
	"\x0D\x0A"										# ERROR

	# Space
	"1     2     3     4\x0D\x0A"					# ERROR
	"             1 2 3 4 5\x0D\x0A"				# ERROR
	"1 2 3 4 5             \x0D\x0A"				# ERROR
	" 1 2 3 4 \x0D\x0A"								# ERROR
	"                          \x0D\x0A"			# ERROR
	"!@#$%^&*()_+{}|:\"<>?-=[]\;',./\`~\x0D\x0A"	# ERROR
	"Hello From K4R05EK!\x0D\x0A"					# ERROR

	# Long good inputs
	"3 15 10 7 11 6 13 13 6 7 13 9 11 16 9 9 5 11 7 12 11 9 14 13 15 8 11 14 7 8 13 10 7 14 17 11 12 6 10 8 9 11 8 13 7 8 9 10 12 14 10 11 10 8 5 4 10 14 9 13 9 11 9 9 6 9 12 9 7 10 13 6 10 12 7 8 11 7 6 8 13 10 10 10 20 12 9 13 8 7 12 10 8 13 8 10 7 8 11 15\x0D\x0A"
	"40017 39973 40085 39730 40227 39952 40175 40026 39939 39910 39796 40136 40143 39992 40055 40054 39999 39726 40380 40221 40042 40076 40088 39768 39490\x0D\x0A"
	"0 1 0 1 0 3 0 1 1 2 1 3 0 3 1 0 1 3 1 2 0 0 1 2 0 2 0 0 1 0 0 1 1 1 0 0 2 2 0 0 4 1 1 2 1 1 0 1 2 0\x0D\x0A"
	"117 95 107 118 96 91 87 107 95 87\x0D\x0A"
	"65611 65820 65768 65562 65884 65979 66043 66229 65966 65508 66264 65681 66076 65784 65524 66069 65739 66165 66404 65929 65757 65894 65850 66011 65995 66185 66058 65342 66245 66518 65900 65537 66375 65574 65712 65654 65764 65351 66164 65691 65522 65854 65789 65460 65950 65780 66047 66215 65758 65958 65741 65643 65770 65668 65644 65894 65905 65582 66186 65948 65772 66131 66334 65722 65667 65443 65916 66004 66295 65903 65811 65984 65693 65861 65780 65732 65701 65853 66153 65903 66244 65790 65979 66027 66078 65457 65581 66060 65827 65868 66031 66275 65840 65365 65777 65855 65460 65698 65986 65484 65842 65977 66195 66108 65677 65368 65951 65657 65827 66013 65798 65704 66092 65935 65364 65464 65863 65401 65166 65534 65840 65587 65906 65532 65730 66036 65690 66040 65844 65853 66363 66029 65993 65747 65635 65890 66273 65834 66024 65824 65685 65883 66012 65953 66077 65625 65696 65759 65404 65718\x0D\x0A"

	# Multiple lines at once
	"1 2 3\x0D\x0A10 20\x0D\x0A5\x0D\x0A"								# OK OK OK
	"0 7\x0D\x0A95 5\x0D\x0A0\x0D\x0A\x0D\x0A"							# OK OK OK ERROR
	"Hello World!\x0D\x0A9 8 2\x0D\x0A15  6 2\x0D\x0A9 2 1 \x0D\x0A"	# ERROR OK ERROR ERROR
	"5 5\x0D\x0AThis is bad!\x0D\x0A9 9\x0D\x0A"						# OK ERROR OK
	"This\x0D\x0Ais\x0D\x0Avery\x0D\x0Abad!\x00\x0D\x0A"				# ERROR ERROR ERROR ERROR

	# \r\n combinations
	"00000000000000000000000050 50 010 010\x0D\x0A"	# OK
	"\x0D\x0A\x0A\x0D\x0A"							# ERROR ERROR (\r\n\n\r\n)
	"\x0D\x0A\x0D\x0D\x0A"							# ERROR ERROR (\r\n\r\r\n)
	"\x0D\x0A\x0D\x0A\x0D\x0A"						# ERROR ERROR ERROR (\r\n\r\n\r\n)
	"5\x0D4\x0D\x0A"								# ERROR
	"5\x0A4\x0D\x0A"								# ERROR

	# Multiple zeros
	"0$max\x0D\x0A"																	# OK
	"000000$max\x0D\x0A"															# OK
	"0000000000000$((max-1)) 1\x0D\x0A"												# OK
	"0000000000$max 2 1 3 4\x0D\x0A"												# ERROR
	"0000000000000000000000000000\x0D\x0A"											# OK
	"00000050 000000000000\x0D\x0A"													# OK
	"0000000010 01 000000 05 0000000000000000000000 2\x0D\x0A"						# OK
	"00000000000000000000000$((max-10)) 000002 00000005 0001\x0D\x0A" 				# OK
	"0000000000000000000000999999999999999999999999999999 5 5\x0D\x0A"				# ERROR
	"5200000000000000000000000 1 2 3 4 9\x0D\x0A"									# ERROR
	"1 10 100 1000 10000 100000 1000000 10000000\x0D\x0A"							# OK
	"00000001 00000010 00000100 00001000 10000 100000 1000000 10000000\x0D\x0A"		# OK
)

declare -a expected_data=(
	# Standard
	"4\x0D\x0A"
	"$max\x0D\x0A"
	"$max\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"0\x0D\x0A"
	"1234567918\x0D\x0A"
	"ERROR\x0D\x0A"
	"0\x0D\x0A"
	"392\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"

	# Space
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"

	# Long good inputs
	"999\x0D\x0A"
	"1000000\x0D\x0A"
	"50\x0D\x0A"
	"1000\x0D\x0A"
	"9876654\x0D\x0A"

	# Multiple lines at once
	"6\x0D\x0A30\x0D\x0A5\x0D\x0A"
	"7\x0D\x0A100\x0D\x0A0\x0D\x0AERROR\x0D\x0A"
	"ERROR\x0D\x0A19\x0D\x0AERROR\x0D\x0AERROR\x0D\x0A"
	"10\x0D\x0AERROR\x0D\x0A18\x0D\x0A"
	"ERROR\x0D\x0AERROR\x0D\x0AERROR\x0D\x0AERROR\x0D\x0A"

	# \r\n combinations
	"120\x0D\x0A"
	"ERROR\x0D\x0AERROR\x0D\x0A"
	"ERROR\x0D\x0AERROR\x0D\x0A"
	"ERROR\x0D\x0AERROR\x0D\x0AERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"

	# Multiple zeros
	"$max\x0D\x0A"
	"$max\x0D\x0A"
	"$max\x0D\x0A"
	"ERROR\x0D\x0A"
	"0\x0D\x0A"
	"50\x0D\x0A"
	"18\x0D\x0A"
	"$((max-2))\x0D\x0A"
	"ERROR\x0D\x0A"
	"ERROR\x0D\x0A"
	"11111111\x0D\x0A"
	"11111111\x0D\x0A"
)

echo -e "[\u001b[33m#\u001b[0m] Creating tests..."
for i in "${!input_data[@]}"
do
	echo -n -e "${input_data[$i]}" > $input/test_$i.ini
	echo -n -e "${expected_data[$i]}" > $expect/test_$i.ini
done

echo -e "[\u001b[33m#\u001b[0m] Running tests..."
for i in "${!input_data[@]}"
do
	#netcat -w 1 127.0.0.1 2020 < $input/test_$i.ini > $result/test_$i.ini
	socat -t 1 stdio tcp4:127.0.0.1:2020 < $input/test_$i.ini > $result/test_$i.ini
	output=$(cat $result/test_$i.ini)
	value=$(cat $expect/test_$i.ini)
	if [ "$output" = "$value" ]; then
        echo -e "[\u001b[33m$i\u001b[0m] Test: \u001b[32mSuccess!\033[0m"
    else
        echo -e "[\u001b[33m$i\u001b[0m] Test: \u001b[31mFailure!\033[0m"
		echo "# Bytes send:"
		od -A d -t u1 -t c < $input/test_$i.ini
		echo "# Bytes expected:"
		od -A d -t u1 -t c < $expect/test_$i.ini
		echo "# Bytes received:"
		od -A d -t u1 -t c < $result/test_$i.ini
		echo "====================="
	fi
done

# if you don't want tests to disappear uncomment the exit below
#exit

echo -e "[\u001b[33m#\u001b[0m] Cleaning up..."
rm -f $input/*.ini
rm -f $result/*.ini
rm -f $expect/*.ini
rmdir --ignore-fail-on-non-empty -p $path/*
