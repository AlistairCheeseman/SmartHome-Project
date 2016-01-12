EESchema Schematic File Version 2
LIBS:valves
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:RevC-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L GNDD #PWR01
U 1 1 5589785A
P 2450 3650
F 0 "#PWR01" H 2450 3400 50  0001 C CNN
F 1 "GNDD" H 2450 3500 50  0000 C CNN
F 2 "" H 2450 3650 60  0000 C CNN
F 3 "" H 2450 3650 60  0000 C CNN
	1    2450 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 3250 2850 3250
Wire Wire Line
	2850 3250 2850 3550
Wire Wire Line
	2850 3550 2050 3550
Wire Wire Line
	2050 3550 2050 3250
Wire Wire Line
	2050 3250 2200 3250
Wire Wire Line
	2200 3350 2050 3350
Connection ~ 2050 3350
Wire Wire Line
	2700 3350 2850 3350
Connection ~ 2850 3350
Wire Wire Line
	2450 3650 2450 3550
Connection ~ 2450 3550
$Comp
L GNDD #PWR02
U 1 1 558978D3
P 3500 1150
F 0 "#PWR02" H 3500 900 50  0001 C CNN
F 1 "GNDD" H 3500 1000 50  0000 C CNN
F 2 "" H 3500 1150 60  0000 C CNN
F 3 "" H 3500 1150 60  0000 C CNN
	1    3500 1150
	0    -1   -1   0   
$EndComp
$Comp
L GNDD #PWR03
U 1 1 5589790A
P 1400 1150
F 0 "#PWR03" H 1400 900 50  0001 C CNN
F 1 "GNDD" H 1400 1000 50  0000 C CNN
F 2 "" H 1400 1150 60  0000 C CNN
F 3 "" H 1400 1150 60  0000 C CNN
	1    1400 1150
	0    1    1    0   
$EndComp
$Comp
L GNDD #PWR04
U 1 1 558979C1
P 4650 1150
F 0 "#PWR04" H 4650 900 50  0001 C CNN
F 1 "GNDD" H 4650 1000 50  0000 C CNN
F 2 "" H 4650 1150 60  0000 C CNN
F 3 "" H 4650 1150 60  0000 C CNN
	1    4650 1150
	0    1    1    0   
$EndComp
$Comp
L GNDD #PWR05
U 1 1 558979E8
P 5650 1150
F 0 "#PWR05" H 5650 900 50  0001 C CNN
F 1 "GNDD" H 5650 1000 50  0000 C CNN
F 2 "" H 5650 1150 60  0000 C CNN
F 3 "" H 5650 1150 60  0000 C CNN
	1    5650 1150
	0    -1   -1   0   
$EndComp
$Comp
L +3V3 #PWR06
U 1 1 55897A67
P 2850 950
F 0 "#PWR06" H 2850 800 50  0001 C CNN
F 1 "+3V3" H 2850 1090 50  0000 C CNN
F 2 "" H 2850 950 60  0000 C CNN
F 3 "" H 2850 950 60  0000 C CNN
	1    2850 950 
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR07
U 1 1 55897A7F
P 3050 950
F 0 "#PWR07" H 3050 800 50  0001 C CNN
F 1 "+5V" H 3050 1090 50  0000 C CNN
F 2 "" H 3050 950 60  0000 C CNN
F 3 "" H 3050 950 60  0000 C CNN
	1    3050 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 1250 2700 1250
Wire Wire Line
	2850 950  2850 1250
Wire Wire Line
	3050 1350 2700 1350
Wire Wire Line
	3050 950  3050 1350
Wire Wire Line
	3300 950  3300 1450
Wire Wire Line
	3300 1450 2700 1450
$Comp
L +3V3 #PWR08
U 1 1 55897EE7
P 2050 950
F 0 "#PWR08" H 2050 800 50  0001 C CNN
F 1 "+3V3" H 2050 1090 50  0000 C CNN
F 2 "" H 2050 950 60  0000 C CNN
F 3 "" H 2050 950 60  0000 C CNN
	1    2050 950 
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR09
U 1 1 55897EF8
P 1850 950
F 0 "#PWR09" H 1850 800 50  0001 C CNN
F 1 "+5V" H 1850 1090 50  0000 C CNN
F 2 "" H 1850 950 60  0000 C CNN
F 3 "" H 1850 950 60  0000 C CNN
	1    1850 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 1250 2050 1250
Wire Wire Line
	2050 1250 2050 950 
Wire Wire Line
	2200 1350 1850 1350
Wire Wire Line
	1850 1350 1850 950 
Wire Wire Line
	2200 1450 1600 1450
Wire Wire Line
	1600 1450 1600 950 
Wire Wire Line
	2700 1150 3500 1150
Wire Wire Line
	2200 1150 1400 1150
Wire Wire Line
	4650 1150 4900 1150
Wire Wire Line
	5400 1150 5650 1150
$Comp
L SYS_5V #PWR010
U 1 1 55898497
P 3300 950
F 0 "#PWR010" H 3300 800 50  0001 C CNN
F 1 "SYS_5V" H 3300 1090 50  0000 C CNN
F 2 "" H 3300 950 60  0000 C CNN
F 3 "" H 3300 950 60  0000 C CNN
	1    3300 950 
	1    0    0    -1  
$EndComp
$Comp
L SYS_5V #PWR011
U 1 1 558984AF
P 1600 950
F 0 "#PWR011" H 1600 800 50  0001 C CNN
F 1 "SYS_5V" H 1600 1090 50  0000 C CNN
F 2 "" H 1600 950 60  0000 C CNN
F 3 "" H 1600 950 60  0000 C CNN
	1    1600 950 
	1    0    0    -1  
$EndComp
$Comp
L VDD_ADC #PWR012
U 1 1 558985D8
P 3050 2590
F 0 "#PWR012" H 3050 2440 50  0001 C CNN
F 1 "VDD_ADC" H 3050 2730 50  0000 C CNN
F 2 "" H 3050 2590 60  0000 C CNN
F 3 "" H 3050 2590 60  0000 C CNN
	1    3050 2590
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 2650 3100 2650
Wire Wire Line
	3050 2650 3050 2590
$Comp
L GNDA_ADC #PWR013
U 1 1 5589865D
P 3300 2750
F 0 "#PWR013" H 3300 2500 50  0001 C CNN
F 1 "GNDA_ADC" H 3300 2600 50  0000 C CNN
F 2 "" H 3300 2750 60  0000 C CNN
F 3 "" H 3300 2750 60  0000 C CNN
	1    3300 2750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2700 2750 3300 2750
Text GLabel 2800 1550 2    50   Input ~ 0
SYS_RESETN
Wire Wire Line
	2700 1550 2800 1550
Text GLabel 2100 1550 0    50   Input ~ 0
PWR_BUT
Wire Wire Line
	2200 1550 2100 1550
$Comp
L BeagleBone_Black_Header P9
U 1 1 55DF7DBA
P 2450 2250
F 0 "P9" H 2450 3450 50  0000 C CNN
F 1 "BeagleBone_Black_Header" V 2450 2250 50  0000 C CNN
F 2 "Socket_BeagleBone_Black:Socket_BeagleBone_Black" H 2450 1400 60  0001 C CNN
F 3 "" H 2450 1400 60  0000 C CNN
	1    2450 2250
	1    0    0    -1  
$EndComp
$Comp
L BeagleBone_Black_Header P8
U 1 1 55DF7DE1
P 5150 2250
F 0 "P8" H 5150 3450 50  0000 C CNN
F 1 "BeagleBone_Black_Header" V 5150 2250 50  0000 C CNN
F 2 "Socket_BeagleBone_Black:Socket_BeagleBone_Black" H 5150 1400 60  0001 C CNN
F 3 "" H 5150 1400 60  0000 C CNN
	1    5150 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 1950 2810 1950
Wire Wire Line
	2200 2150 2120 2150
Wire Wire Line
	2700 2150 2810 2150
Text GLabel 2810 2150 2    60   Input ~ 0
SCK
Text GLabel 2810 1950 2    60   Input ~ 0
MOSI
Text GLabel 2120 2150 0    60   Input ~ 0
MISO
Wire Wire Line
	2700 2250 2810 2250
Wire Wire Line
	2700 2350 2810 2350
Text GLabel 2810 2250 2    60   Input ~ 0
UART_TX
Text GLabel 2810 2350 2    60   Input ~ 0
UART_RX
Wire Wire Line
	2700 2050 2810 2050
Wire Wire Line
	2200 2050 2120 2050
Text GLabel 2810 2050 2    60   Input ~ 0
UART_CTS
Text GLabel 2120 2050 0    60   Input ~ 0
UART_RTS
Wire Wire Line
	2200 2350 2100 2350
NoConn ~ 2100 2350
Text Notes 1190 2390 0    60   ~ 0
NEEDED FOR HDMI
Wire Wire Line
	2200 1950 2090 1950
NoConn ~ 2090 1950
Text Notes 910  1990 0    60   ~ 0
HARDWARE CS NOT USED
Text GLabel 4720 1450 0    60   Input ~ 0
CSN_RADIO
Text GLabel 4720 1550 0    60   Input ~ 0
CE_RADIO
$Comp
L CONN_01X12 P2
U 1 1 56937017
P 7770 2660
F 0 "P2" H 7770 3310 50  0000 C CNN
F 1 "CONN_01X12" V 7870 2660 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x12" H 7770 2660 50  0001 C CNN
F 3 "" H 7770 2660 50  0000 C CNN
	1    7770 2660
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR016
U 1 1 569370A7
P 7510 2030
F 0 "#PWR016" H 7510 1880 50  0001 C CNN
F 1 "+3V3" H 7510 2170 50  0000 C CNN
F 2 "" H 7510 2030 60  0000 C CNN
F 3 "" H 7510 2030 60  0000 C CNN
	1    7510 2030
	1    0    0    -1  
$EndComp
$Comp
L GNDD #PWR017
U 1 1 569370C1
P 6990 3300
F 0 "#PWR017" H 6990 3050 50  0001 C CNN
F 1 "GNDD" H 6990 3150 50  0000 C CNN
F 2 "" H 6990 3300 60  0000 C CNN
F 3 "" H 6990 3300 60  0000 C CNN
	1    6990 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6990 2910 6990 3300
Wire Wire Line
	7510 2030 7510 2110
Wire Wire Line
	7570 3210 7460 3210
Wire Wire Line
	7570 2210 7460 2210
Wire Wire Line
	7570 2310 7460 2310
Wire Wire Line
	7570 2410 7460 2410
Wire Wire Line
	7570 2510 7460 2510
Wire Wire Line
	7570 2610 7460 2610
Wire Wire Line
	7570 2710 7460 2710
Wire Wire Line
	7570 2810 7460 2810
Wire Wire Line
	6990 2910 7570 2910
Wire Wire Line
	7570 3010 7460 3010
Text GLabel 7460 2610 0    60   Input ~ 0
MOSI
Text GLabel 7460 2310 0    60   Input ~ 0
MISO
Text GLabel 7460 2410 0    60   Input ~ 0
SCK
Text GLabel 7460 2510 0    60   Input ~ 0
IRQ_RADIO
Text GLabel 7460 2210 0    60   Input ~ 0
CSN_MEM
Text GLabel 7460 2710 0    60   Input ~ 0
CSN_RADIO
Text GLabel 7460 2810 0    60   Input ~ 0
CE_RADIO
Text GLabel 7460 3010 0    60   Input ~ 0
UART_TX
Wire Wire Line
	7570 3110 7460 3110
Text GLabel 7460 3110 0    60   Input ~ 0
UART_RX
Text Notes 1050 4310 0    60   ~ 0
NO_CONNECT PINS
Text GLabel 1670 4400 2    50   Input ~ 0
SYS_RESETN
Text GLabel 1220 4410 0    50   Input ~ 0
PWR_BUT
Text GLabel 1310 4510 0    60   Input ~ 0
UART_RTS
Text GLabel 1660 4510 2    60   Input ~ 0
UART_CTS
Wire Wire Line
	1670 4400 1570 4400
Wire Wire Line
	1660 4510 1570 4510
Wire Wire Line
	1220 4410 1300 4410
Wire Wire Line
	1310 4510 1380 4510
NoConn ~ 1300 4410
NoConn ~ 1570 4400
NoConn ~ 1570 4510
NoConn ~ 1380 4510
$Comp
L PWR_FLAG #FLG018
U 1 1 56939577
P 2850 720
F 0 "#FLG018" H 2850 815 50  0001 C CNN
F 1 "PWR_FLAG" H 2850 900 50  0000 C CNN
F 2 "" H 2850 720 50  0000 C CNN
F 3 "" H 2850 720 50  0000 C CNN
	1    2850 720 
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG019
U 1 1 56939598
P 3050 710
F 0 "#FLG019" H 3050 805 50  0001 C CNN
F 1 "PWR_FLAG" H 3050 890 50  0000 C CNN
F 2 "" H 3050 710 50  0000 C CNN
F 3 "" H 3050 710 50  0000 C CNN
	1    3050 710 
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG020
U 1 1 569395B2
P 3290 720
F 0 "#FLG020" H 3290 815 50  0001 C CNN
F 1 "PWR_FLAG" H 3290 900 50  0000 C CNN
F 2 "" H 3290 720 50  0000 C CNN
F 3 "" H 3290 720 50  0000 C CNN
	1    3290 720 
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG021
U 1 1 569395CC
P 3790 1060
F 0 "#FLG021" H 3790 1155 50  0001 C CNN
F 1 "PWR_FLAG" H 3790 1240 50  0000 C CNN
F 2 "" H 3790 1060 50  0000 C CNN
F 3 "" H 3790 1060 50  0000 C CNN
	1    3790 1060
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG022
U 1 1 56939898
P 3500 2510
F 0 "#FLG022" H 3500 2605 50  0001 C CNN
F 1 "PWR_FLAG" H 3500 2690 50  0000 C CNN
F 2 "" H 3500 2510 50  0000 C CNN
F 3 "" H 3500 2510 50  0000 C CNN
	1    3500 2510
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG023
U 1 1 569398B2
P 3710 2800
F 0 "#FLG023" H 3710 2895 50  0001 C CNN
F 1 "PWR_FLAG" H 3710 2980 50  0000 C CNN
F 2 "" H 3710 2800 50  0000 C CNN
F 3 "" H 3710 2800 50  0000 C CNN
	1    3710 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 720  2730 720 
Wire Wire Line
	2730 720  2730 970 
Wire Wire Line
	2730 970  2850 970 
Connection ~ 2850 970 
Wire Wire Line
	3050 710  2970 710 
Wire Wire Line
	2970 710  2970 970 
Wire Wire Line
	2970 970  3050 970 
Connection ~ 3050 970 
Wire Wire Line
	3290 720  3160 720 
Wire Wire Line
	3160 720  3160 980 
Wire Wire Line
	3160 980  3300 980 
Connection ~ 3300 980 
Wire Wire Line
	3790 1060 3460 1060
Wire Wire Line
	3460 1060 3460 1150
Connection ~ 3460 1150
Wire Wire Line
	3500 2510 3240 2510
Wire Wire Line
	3240 2510 3100 2650
Connection ~ 3050 2650
Wire Wire Line
	3710 2800 3710 2950
Wire Wire Line
	3710 2950 3290 2950
Wire Wire Line
	3290 2950 3290 2750
Connection ~ 3290 2750
NoConn ~ 4820 1250
NoConn ~ 5470 1250
NoConn ~ 4820 1350
NoConn ~ 5470 1350
NoConn ~ 2200 1650
NoConn ~ 2200 1750
NoConn ~ 2200 1850
NoConn ~ 2700 1650
NoConn ~ 2700 1750
NoConn ~ 2700 1850
NoConn ~ 2200 2250
NoConn ~ 2200 2450
NoConn ~ 2700 2450
NoConn ~ 2200 2550
NoConn ~ 2700 2550
NoConn ~ 2700 2850
NoConn ~ 2700 2950
NoConn ~ 2700 3050
NoConn ~ 2700 3150
NoConn ~ 2200 3150
NoConn ~ 2200 3050
NoConn ~ 2200 2950
NoConn ~ 2200 2850
NoConn ~ 2200 2750
NoConn ~ 2200 2650
Wire Wire Line
	4900 1250 4820 1250
Wire Wire Line
	4900 1350 4820 1350
Wire Wire Line
	5400 1250 5470 1250
Wire Wire Line
	5400 1350 5470 1350
Wire Wire Line
	5400 2150 5470 2150
Wire Wire Line
	5400 2250 5470 2250
Wire Wire Line
	5400 2450 5470 2450
Wire Wire Line
	5400 2550 5470 2550
Wire Wire Line
	5400 2650 5470 2650
Wire Wire Line
	5400 2750 5470 2750
Wire Wire Line
	5400 2850 5470 2850
Wire Wire Line
	5400 2950 5470 2950
Wire Wire Line
	5400 3050 5470 3050
Wire Wire Line
	5400 3150 5470 3150
Wire Wire Line
	5400 3250 5470 3250
Wire Wire Line
	5400 3350 5470 3350
NoConn ~ 5470 2150
NoConn ~ 5470 2250
NoConn ~ 5470 2450
NoConn ~ 5470 2550
NoConn ~ 5470 2650
NoConn ~ 5470 2750
NoConn ~ 5470 2850
NoConn ~ 5470 2950
NoConn ~ 5470 3050
NoConn ~ 5470 3150
NoConn ~ 5470 3250
NoConn ~ 5470 3350
NoConn ~ 5400 2350
Wire Wire Line
	4900 2150 4810 2150
Wire Wire Line
	4900 2250 4810 2250
Wire Wire Line
	4900 2350 4810 2350
Wire Wire Line
	4900 2450 4810 2450
Wire Wire Line
	4900 2550 4810 2550
Wire Wire Line
	4900 2650 4810 2650
Wire Wire Line
	4900 2750 4810 2750
Wire Wire Line
	4900 2850 4810 2850
Wire Wire Line
	4900 2950 4810 2950
Wire Wire Line
	4900 3050 4810 3050
Wire Wire Line
	4900 3150 4810 3150
Wire Wire Line
	4900 3250 4810 3250
Wire Wire Line
	4900 3350 4810 3350
NoConn ~ 4810 3350
NoConn ~ 4810 3250
NoConn ~ 4810 3150
NoConn ~ 4810 3050
NoConn ~ 4810 2950
NoConn ~ 4810 2850
NoConn ~ 4810 2750
NoConn ~ 4810 2650
NoConn ~ 4810 2550
NoConn ~ 4810 2450
NoConn ~ 4810 2350
NoConn ~ 4810 2250
NoConn ~ 4810 2150
Text GLabel 5590 1450 2    60   Input ~ 0
IRQ_RADIO
Text GLabel 4720 1650 0    60   Input ~ 0
DETECT
Text GLabel 5590 1550 2    60   Input ~ 0
CSN_MEM
Wire Wire Line
	5590 1450 5400 1450
Wire Wire Line
	4900 1450 4720 1450
Wire Wire Line
	4900 1550 4720 1550
Wire Wire Line
	5590 1550 5400 1550
Wire Wire Line
	4900 1650 4720 1650
NoConn ~ 4900 2050
NoConn ~ 4900 1950
NoConn ~ 4900 1850
NoConn ~ 4900 1750
NoConn ~ 5400 1650
NoConn ~ 5400 1750
NoConn ~ 5400 1850
NoConn ~ 5400 1950
NoConn ~ 5400 2050
$Comp
L +3V3 #PWR024
U 1 1 5693E0F6
P 8040 2940
F 0 "#PWR024" H 8040 2790 50  0001 C CNN
F 1 "+3V3" H 8040 3080 50  0000 C CNN
F 2 "" H 8040 2940 60  0000 C CNN
F 3 "" H 8040 2940 60  0000 C CNN
	1    8040 2940
	1    0    0    -1  
$EndComp
Wire Wire Line
	8040 3350 8040 3300
Text GLabel 7460 3210 0    60   Input ~ 0
DETECT
$Comp
L R R1
U 1 1 5693E5D2
P 8040 3150
F 0 "R1" V 8120 3150 50  0000 C CNN
F 1 "R" V 8040 3150 50  0000 C CNN
F 2 "Resistors_SMD:R_1210_HandSoldering" V 7970 3150 50  0001 C CNN
F 3 "" H 8040 3150 50  0000 C CNN
	1    8040 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8040 2940 8040 3000
Wire Wire Line
	7520 3350 7520 3210
Connection ~ 7520 3210
Connection ~ 7520 3350
Wire Wire Line
	7520 3350 8040 3350
Wire Wire Line
	7510 2110 7570 2110
$EndSCHEMATC
