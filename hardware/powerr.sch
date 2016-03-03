EESchema Schematic File Version 2
LIBS:motor_sturing-rescue
LIBS:Local
LIBS:xilinx-azonenberg
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
LIBS:special
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
LIBS:valves
LIBS:motor_sturing-cache
EELAYER 25 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 5 5
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
L BARREL_JACK CON1
U 1 1 56D49D93
P 1850 2750
F 0 "CON1" H 1850 3000 60  0000 C CNN
F 1 "BARREL_JACK" H 1850 2550 60  0000 C CNN
F 2 "00_Passives:smd_barrel_jack" H 1850 2750 60  0001 C CNN
F 3 "" H 1850 2750 60  0000 C CNN
	1    1850 2750
	1    0    0    -1  
$EndComp
$Comp
L OKR_T3 U41
U 1 1 56D49E2D
P 4400 3450
F 0 "U41" H 4400 3750 60  0000 C CNN
F 1 "OKR_T3" H 4400 3450 60  0000 C CNN
F 2 "00_Passives:okr_t3" H 4300 3650 60  0001 C CNN
F 3 "okr-t3-w12_long.pdf" H 4400 3750 60  0001 C CNN
F 4 "1736768" H 4500 3850 60  0001 C CNN "Farnell"
F 5 "811-2115-5-ND" H 4600 3950 60  0001 C CNN "Digikey"
F 6 "Murata" H 4700 4050 60  0001 C CNN "Manufacturer"
F 7 "OKR-T/3" H 4800 4150 60  0001 C CNN "PartNr"
	1    4400 3450
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR48
U 1 1 56D49EC2
P 5150 3250
F 0 "#PWR48" H 5150 3100 50  0001 C CNN
F 1 "+3.3V" H 5150 3390 50  0000 C CNN
F 2 "" H 5150 3250 50  0000 C CNN
F 3 "" H 5150 3250 50  0000 C CNN
	1    5150 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3250 5150 3300
Wire Wire Line
	5050 3300 5650 3300
$Comp
L GND #PWR34
U 1 1 56D49EE6
P 4400 5200
F 0 "#PWR34" H 4400 4950 50  0001 C CNN
F 1 "GND" H 4400 5050 50  0000 C CNN
F 2 "" H 4400 5200 50  0000 C CNN
F 3 "" H 4400 5200 50  0000 C CNN
	1    4400 5200
	1    0    0    -1  
$EndComp
$Comp
L R R107
U 1 1 56D49F00
P 4500 4300
F 0 "R107" V 4580 4300 50  0000 C CNN
F 1 "390" V 4500 4300 50  0000 C CNN
F 2 "00_Passives:R0603" H 4500 4300 60  0001 C CNN
F 3 "" H 4500 4300 60  0000 C CNN
	1    4500 4300
	1    0    0    -1  
$EndComp
$Comp
L R R108
U 1 1 56D49F45
P 4500 4850
F 0 "R108" V 4580 4850 50  0000 C CNN
F 1 "47" V 4500 4850 50  0000 C CNN
F 2 "00_Passives:R0603" H 4500 4850 60  0001 C CNN
F 3 "" H 4500 4850 60  0000 C CNN
	1    4500 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 5150 4500 5100
Wire Wire Line
	4300 5150 4500 5150
Wire Wire Line
	4300 5150 4300 4000
Wire Wire Line
	4500 4000 4500 4050
Wire Wire Line
	4500 4550 4500 4600
Wire Wire Line
	4400 5150 4400 5200
Connection ~ 4400 5150
$Comp
L +12V #PWR25
U 1 1 56D49FA0
P 3600 3250
F 0 "#PWR25" H 3600 3100 50  0001 C CNN
F 1 "+12V" H 3600 3390 50  0000 C CNN
F 2 "" H 3600 3250 50  0000 C CNN
F 3 "" H 3600 3250 50  0000 C CNN
	1    3600 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 3250 3600 3650
Wire Wire Line
	3600 3300 3750 3300
$Comp
L OKR_T3 U45
U 1 1 56D4A177
P 4400 5800
F 0 "U45" H 4400 6100 60  0000 C CNN
F 1 "OKR_T3" H 4400 5800 60  0000 C CNN
F 2 "00_Passives:okr_t3" H 4300 6000 60  0001 C CNN
F 3 "okr-t3-w12_long.pdf" H 4400 6100 60  0001 C CNN
F 4 "1736768" H 4500 6200 60  0001 C CNN "Farnell"
F 5 "811-2115-5-ND" H 4600 6300 60  0001 C CNN "Digikey"
F 6 "Murata" H 4700 6400 60  0001 C CNN "Manufacturer"
F 7 "OKR-T/3" H 4800 6500 60  0001 C CNN "PartNr"
	1    4400 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 5600 5150 5650
Wire Wire Line
	5050 5650 6250 5650
$Comp
L GND #PWR37
U 1 1 56D4A185
P 4400 7550
F 0 "#PWR37" H 4400 7300 50  0001 C CNN
F 1 "GND" H 4400 7400 50  0000 C CNN
F 2 "" H 4400 7550 50  0000 C CNN
F 3 "" H 4400 7550 50  0000 C CNN
	1    4400 7550
	1    0    0    -1  
$EndComp
$Comp
L R R109
U 1 1 56D4A18B
P 4500 6650
F 0 "R109" V 4580 6650 50  0000 C CNN
F 1 "240" V 4500 6650 50  0000 C CNN
F 2 "00_Passives:R0603" H 4500 6650 60  0001 C CNN
F 3 "" H 4500 6650 60  0000 C CNN
	1    4500 6650
	1    0    0    -1  
$EndComp
$Comp
L R R110
U 1 1 56D4A191
P 4500 7200
F 0 "R110" V 4580 7200 50  0000 C CNN
F 1 "27" V 4500 7200 50  0000 C CNN
F 2 "00_Passives:R0603" H 4500 7200 60  0001 C CNN
F 3 "" H 4500 7200 60  0000 C CNN
	1    4500 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 7500 4500 7450
Wire Wire Line
	4300 7500 4500 7500
Wire Wire Line
	4300 7500 4300 6350
Wire Wire Line
	4500 6350 4500 6400
Wire Wire Line
	4500 6900 4500 6950
Wire Wire Line
	4400 7500 4400 7550
Connection ~ 4400 7500
$Comp
L +12V #PWR27
U 1 1 56D4A19E
P 3600 5600
F 0 "#PWR27" H 3600 5450 50  0001 C CNN
F 1 "+12V" H 3600 5740 50  0000 C CNN
F 2 "" H 3600 5600 50  0000 C CNN
F 3 "" H 3600 5600 50  0000 C CNN
	1    3600 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 5600 3600 6000
Wire Wire Line
	3600 5650 3750 5650
$Comp
L +5V #PWR49
U 1 1 56D4A1DA
P 5150 5600
F 0 "#PWR49" H 5150 5450 50  0001 C CNN
F 1 "+5V" H 5150 5740 50  0000 C CNN
F 2 "" H 5150 5600 50  0000 C CNN
F 3 "" H 5150 5600 50  0000 C CNN
	1    5150 5600
	1    0    0    -1  
$EndComp
$Comp
L OKR_T3 U26
U 1 1 56D4A621
P 4400 1100
F 0 "U26" H 4400 1400 60  0000 C CNN
F 1 "OKR_T3" H 4400 1100 60  0000 C CNN
F 2 "00_Passives:okr_t3" H 4300 1300 60  0001 C CNN
F 3 "okr-t3-w12_long.pdf" H 4400 1400 60  0001 C CNN
F 4 "1736768" H 4500 1500 60  0001 C CNN "Farnell"
F 5 "811-2115-5-ND" H 4600 1600 60  0001 C CNN "Digikey"
F 6 "Murata" H 4700 1700 60  0001 C CNN "Manufacturer"
F 7 "OKR-T/3" H 4800 1800 60  0001 C CNN "PartNr"
	1    4400 1100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR31
U 1 1 56D4A629
P 4400 2850
F 0 "#PWR31" H 4400 2600 50  0001 C CNN
F 1 "GND" H 4400 2700 50  0000 C CNN
F 2 "" H 4400 2850 50  0000 C CNN
F 3 "" H 4400 2850 50  0000 C CNN
	1    4400 2850
	1    0    0    -1  
$EndComp
$Comp
L R R105
U 1 1 56D4A62F
P 4500 1950
F 0 "R105" V 4580 1950 50  0000 C CNN
F 1 "1.8k" V 4500 1950 50  0000 C CNN
F 2 "00_Passives:R0603" H 4500 1950 60  0001 C CNN
F 3 "" H 4500 1950 60  0000 C CNN
	1    4500 1950
	1    0    0    -1  
$EndComp
$Comp
L R R106
U 1 1 56D4A635
P 4500 2500
F 0 "R106" V 4580 2500 50  0000 C CNN
F 1 "150" V 4500 2500 50  0000 C CNN
F 2 "00_Passives:R0603" H 4500 2500 60  0001 C CNN
F 3 "" H 4500 2500 60  0000 C CNN
	1    4500 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 2800 4500 2750
Wire Wire Line
	4300 2800 4500 2800
Wire Wire Line
	4300 2800 4300 1650
Wire Wire Line
	4500 1650 4500 1700
Wire Wire Line
	4500 2200 4500 2250
Wire Wire Line
	4400 2800 4400 2850
Connection ~ 4400 2800
$Comp
L +12V #PWR23
U 1 1 56D4A642
P 3600 900
F 0 "#PWR23" H 3600 750 50  0001 C CNN
F 1 "+12V" H 3600 1040 50  0000 C CNN
F 2 "" H 3600 900 50  0000 C CNN
F 3 "" H 3600 900 50  0000 C CNN
	1    3600 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 900  3600 1350
Wire Wire Line
	3600 950  3750 950 
$Comp
L +1.2V #PWR47
U 1 1 56D4A685
P 5150 900
F 0 "#PWR47" H 5150 1040 20  0001 C CNN
F 1 "+1.2V" H 5150 1010 30  0000 C CNN
F 2 "" H 5150 900 60  0000 C CNN
F 3 "" H 5150 900 60  0000 C CNN
	1    5150 900 
	1    0    0    -1  
$EndComp
$Comp
L LT3010 U50
U 1 1 56D4AFA6
P 6900 5950
F 0 "U50" H 6900 6400 60  0000 C CNN
F 1 "LT3010" H 7200 5600 60  0000 C CNN
F 2 "Housings_SSOP:MSOP-8-1EP_3x3mm_Pitch0.65mm" H 6900 5950 60  0001 C CNN
F 3 "" H 6900 6400 60  0000 C CNN
F 4 "1273635" H 7000 6500 60  0001 C CNN "Farnell"
F 5 "linear" H 7200 6700 60  0001 C CNN "Manufacturer"
F 6 "LT3010EMS8E#PBF" H 7300 6800 60  0001 C CNN "PartNr"
	1    6900 5950
	1    0    0    -1  
$EndComp
$Comp
L +3.3VADC #PWR82
U 1 1 56D4B08E
P 7700 5600
F 0 "#PWR82" H 7850 5550 50  0001 C CNN
F 1 "+3.3VADC" H 7700 5700 50  0000 C CNN
F 2 "" H 7700 5600 50  0000 C CNN
F 3 "" H 7700 5600 50  0000 C CNN
	1    7700 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 5600 7700 5700
Wire Wire Line
	7550 5650 8600 5650
Connection ~ 5150 5650
Wire Wire Line
	6200 5650 6200 5850
Wire Wire Line
	6200 5850 6250 5850
Connection ~ 6200 5650
$Comp
L GND #PWR74
U 1 1 56D4B154
P 6900 6600
F 0 "#PWR74" H 6900 6350 50  0001 C CNN
F 1 "GND" H 6900 6450 50  0000 C CNN
F 2 "" H 6900 6600 50  0000 C CNN
F 3 "" H 6900 6600 50  0000 C CNN
	1    6900 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 6600 6900 6550
Wire Wire Line
	6850 6550 6950 6550
Wire Wire Line
	6850 6550 6850 6500
Wire Wire Line
	6950 6550 6950 6500
Connection ~ 6900 6550
$Comp
L R R111
U 1 1 56D4B990
P 7700 5950
F 0 "R111" V 7780 5950 50  0000 C CNN
F 1 "6.2k" V 7700 5950 50  0000 C CNN
F 2 "00_Passives:R0603" H 7700 5950 60  0001 C CNN
F 3 "" H 7700 5950 60  0000 C CNN
	1    7700 5950
	1    0    0    -1  
$EndComp
$Comp
L R R112
U 1 1 56D4B9F9
P 7700 6550
F 0 "R112" V 7780 6550 50  0000 C CNN
F 1 "3.9k" V 7700 6550 50  0000 C CNN
F 2 "00_Passives:R0603" H 7700 6550 60  0001 C CNN
F 3 "" H 7700 6550 60  0000 C CNN
	1    7700 6550
	1    0    0    -1  
$EndComp
Connection ~ 7700 5650
Wire Wire Line
	7700 6200 7700 6300
Wire Wire Line
	7700 6250 7600 6250
Wire Wire Line
	7600 6250 7600 5850
Wire Wire Line
	7600 5850 7550 5850
Connection ~ 7700 6250
$Comp
L GND #PWR83
U 1 1 56D4BB5E
P 7700 6850
F 0 "#PWR83" H 7700 6600 50  0001 C CNN
F 1 "GND" H 7700 6700 50  0000 C CNN
F 2 "" H 7700 6850 50  0000 C CNN
F 3 "" H 7700 6850 50  0000 C CNN
	1    7700 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 6850 7700 6800
$Comp
L +12V #PWR10
U 1 1 56D4C0A6
P 2300 2600
F 0 "#PWR10" H 2300 2450 50  0001 C CNN
F 1 "+12V" H 2300 2740 50  0000 C CNN
F 2 "" H 2300 2600 50  0000 C CNN
F 3 "" H 2300 2600 50  0000 C CNN
	1    2300 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 2600 2300 2650
Wire Wire Line
	2150 2650 2950 2650
$Comp
L GND #PWR11
U 1 1 56D4C114
P 2300 2950
F 0 "#PWR11" H 2300 2700 50  0001 C CNN
F 1 "GND" H 2300 2800 50  0000 C CNN
F 2 "" H 2300 2950 50  0000 C CNN
F 3 "" H 2300 2950 50  0000 C CNN
	1    2300 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 2750 2300 2750
Wire Wire Line
	2300 2750 2300 2950
Wire Wire Line
	2150 2850 2300 2850
Connection ~ 2300 2850
$Comp
L C C81
U 1 1 56D4C1DA
P 5250 6000
F 0 "C81" H 5300 6100 50  0000 L CNN
F 1 "10uF" H 5300 5900 50  0000 L CNN
F 2 "00_Passives:C1210" H 5250 6000 60  0001 C CNN
F 3 "" H 5250 6000 60  0000 C CNN
	1    5250 6000
	1    0    0    -1  
$EndComp
$Comp
L C C87
U 1 1 56D4C296
P 8200 5900
F 0 "C87" H 8250 6000 50  0000 L CNN
F 1 "10uF" H 8250 5800 50  0000 L CNN
F 2 "00_Passives:C1210" H 8200 5900 60  0001 C CNN
F 3 "" H 8200 5900 60  0000 C CNN
	1    8200 5900
	1    0    0    -1  
$EndComp
$Comp
L C C80
U 1 1 56D4C305
P 5250 3600
F 0 "C80" H 5300 3700 50  0000 L CNN
F 1 "10uF" H 5300 3500 50  0000 L CNN
F 2 "00_Passives:C1210" H 5250 3600 60  0001 C CNN
F 3 "" H 5250 3600 60  0000 C CNN
	1    5250 3600
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C85
U 1 1 56D4C5D3
P 5650 3600
F 0 "C85" H 5700 3700 50  0000 L CNN
F 1 "100uF/16V" H 5700 3500 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x5.3" H 5650 3600 60  0001 C CNN
F 3 "" H 5650 3600 60  0000 C CNN
	1    5650 3600
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C86
U 1 1 56D4C648
P 5950 6000
F 0 "C86" H 6000 6100 50  0000 L CNN
F 1 "100uF/16V" H 6000 5900 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x5.3" H 5950 6000 60  0001 C CNN
F 3 "" H 5950 6000 60  0000 C CNN
	1    5950 6000
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C88
U 1 1 56D4C6B6
P 8600 5900
F 0 "C88" H 8650 6000 50  0000 L CNN
F 1 "100uF/16V" H 8650 5800 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x5.3" H 8600 5900 60  0001 C CNN
F 3 "" H 8600 5900 60  0000 C CNN
	1    8600 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3300 5650 3400
Connection ~ 5150 3300
Wire Wire Line
	5250 3300 5250 3400
Connection ~ 5250 3300
$Comp
L GND #PWR51
U 1 1 56D4C7DD
P 5250 3850
F 0 "#PWR51" H 5250 3600 50  0001 C CNN
F 1 "GND" H 5250 3700 50  0000 C CNN
F 2 "" H 5250 3850 50  0000 C CNN
F 3 "" H 5250 3850 50  0000 C CNN
	1    5250 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3800 5250 3850
$Comp
L GND #PWR56
U 1 1 56D4C7FB
P 5650 3850
F 0 "#PWR56" H 5650 3600 50  0001 C CNN
F 1 "GND" H 5650 3700 50  0000 C CNN
F 2 "" H 5650 3850 50  0000 C CNN
F 3 "" H 5650 3850 50  0000 C CNN
	1    5650 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3800 5650 3850
$Comp
L GND #PWR85
U 1 1 56D4C8C1
P 8200 6150
F 0 "#PWR85" H 8200 5900 50  0001 C CNN
F 1 "GND" H 8200 6000 50  0000 C CNN
F 2 "" H 8200 6150 50  0000 C CNN
F 3 "" H 8200 6150 50  0000 C CNN
	1    8200 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 6100 8200 6150
$Comp
L GND #PWR87
U 1 1 56D4C8C8
P 8600 6150
F 0 "#PWR87" H 8600 5900 50  0001 C CNN
F 1 "GND" H 8600 6000 50  0000 C CNN
F 2 "" H 8600 6150 50  0000 C CNN
F 3 "" H 8600 6150 50  0000 C CNN
	1    8600 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 6100 8600 6150
Wire Wire Line
	8200 5650 8200 5700
Wire Wire Line
	8600 5650 8600 5700
Connection ~ 8200 5650
$Comp
L GND #PWR52
U 1 1 56D4C9FA
P 5250 6250
F 0 "#PWR52" H 5250 6000 50  0001 C CNN
F 1 "GND" H 5250 6100 50  0000 C CNN
F 2 "" H 5250 6250 50  0000 C CNN
F 3 "" H 5250 6250 50  0000 C CNN
	1    5250 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 6200 5250 6250
$Comp
L GND #PWR59
U 1 1 56D4CA01
P 5950 6250
F 0 "#PWR59" H 5950 6000 50  0001 C CNN
F 1 "GND" H 5950 6100 50  0000 C CNN
F 2 "" H 5950 6250 50  0000 C CNN
F 3 "" H 5950 6250 50  0000 C CNN
	1    5950 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 6200 5950 6250
Wire Wire Line
	5950 5650 5950 5800
Connection ~ 5950 5650
Wire Wire Line
	5250 5650 5250 5800
Connection ~ 5250 5650
Wire Wire Line
	5150 900  5150 950 
Wire Wire Line
	5050 950  5650 950 
$Comp
L C C79
U 1 1 56D4D2FE
P 5250 1250
F 0 "C79" H 5300 1350 50  0000 L CNN
F 1 "10uF" H 5300 1150 50  0000 L CNN
F 2 "00_Passives:C1210" H 5250 1250 60  0001 C CNN
F 3 "" H 5250 1250 60  0000 C CNN
	1    5250 1250
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C84
U 1 1 56D4D304
P 5650 1250
F 0 "C84" H 5700 1350 50  0000 L CNN
F 1 "100uF/16V" H 5700 1150 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x5.3" H 5650 1250 60  0001 C CNN
F 3 "" H 5650 1250 60  0000 C CNN
	1    5650 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 950  5650 1050
Connection ~ 5150 950 
Wire Wire Line
	5250 950  5250 1050
Connection ~ 5250 950 
$Comp
L GND #PWR50
U 1 1 56D4D30E
P 5250 1500
F 0 "#PWR50" H 5250 1250 50  0001 C CNN
F 1 "GND" H 5250 1350 50  0000 C CNN
F 2 "" H 5250 1500 50  0000 C CNN
F 3 "" H 5250 1500 50  0000 C CNN
	1    5250 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 1450 5250 1500
$Comp
L GND #PWR55
U 1 1 56D4D315
P 5650 1500
F 0 "#PWR55" H 5650 1250 50  0001 C CNN
F 1 "GND" H 5650 1350 50  0000 C CNN
F 2 "" H 5650 1500 50  0000 C CNN
F 3 "" H 5650 1500 50  0000 C CNN
	1    5650 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 1450 5650 1500
$Comp
L C C69
U 1 1 56D4D98A
P 2550 2900
F 0 "C69" H 2600 3000 50  0000 L CNN
F 1 "10uF" H 2600 2800 50  0000 L CNN
F 2 "00_Passives:C1210" H 2550 2900 60  0001 C CNN
F 3 "" H 2550 2900 60  0000 C CNN
	1    2550 2900
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C70
U 1 1 56D4D990
P 2950 2900
F 0 "C70" H 3000 3000 50  0000 L CNN
F 1 "100uF/16V" H 3000 2800 50  0000 L CNN
F 2 "Capacitors_SMD:c_elec_6.3x5.3" H 2950 2900 60  0001 C CNN
F 3 "" H 2950 2900 60  0000 C CNN
	1    2950 2900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR12
U 1 1 56D4D996
P 2550 3150
F 0 "#PWR12" H 2550 2900 50  0001 C CNN
F 1 "GND" H 2550 3000 50  0000 C CNN
F 2 "" H 2550 3150 50  0000 C CNN
F 3 "" H 2550 3150 50  0000 C CNN
	1    2550 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 3100 2550 3150
$Comp
L GND #PWR13
U 1 1 56D4D99D
P 2950 3150
F 0 "#PWR13" H 2950 2900 50  0001 C CNN
F 1 "GND" H 2950 3000 50  0000 C CNN
F 2 "" H 2950 3150 50  0000 C CNN
F 3 "" H 2950 3150 50  0000 C CNN
	1    2950 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 3100 2950 3150
Wire Wire Line
	2550 2650 2550 2700
Connection ~ 2300 2650
Wire Wire Line
	2950 2650 2950 2700
Connection ~ 2550 2650
$Comp
L C C72
U 1 1 56D4DB42
P 3600 1550
F 0 "C72" H 3650 1650 50  0000 L CNN
F 1 "10uF" H 3650 1450 50  0000 L CNN
F 2 "00_Passives:C1210" H 3600 1550 60  0001 C CNN
F 3 "" H 3600 1550 60  0000 C CNN
	1    3600 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR24
U 1 1 56D4DB48
P 3600 1800
F 0 "#PWR24" H 3600 1550 50  0001 C CNN
F 1 "GND" H 3600 1650 50  0000 C CNN
F 2 "" H 3600 1800 50  0000 C CNN
F 3 "" H 3600 1800 50  0000 C CNN
	1    3600 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 1750 3600 1800
$Comp
L C C75
U 1 1 56D4DBB5
P 3600 3850
F 0 "C75" H 3650 3950 50  0000 L CNN
F 1 "10uF" H 3650 3750 50  0000 L CNN
F 2 "00_Passives:C1210" H 3600 3850 60  0001 C CNN
F 3 "" H 3600 3850 60  0000 C CNN
	1    3600 3850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR26
U 1 1 56D4DBBB
P 3600 4100
F 0 "#PWR26" H 3600 3850 50  0001 C CNN
F 1 "GND" H 3600 3950 50  0000 C CNN
F 2 "" H 3600 4100 50  0000 C CNN
F 3 "" H 3600 4100 50  0000 C CNN
	1    3600 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 4050 3600 4100
$Comp
L C C76
U 1 1 56D4DC27
P 3600 6200
F 0 "C76" H 3650 6300 50  0000 L CNN
F 1 "10uF" H 3650 6100 50  0000 L CNN
F 2 "00_Passives:C1210" H 3600 6200 60  0001 C CNN
F 3 "" H 3600 6200 60  0000 C CNN
	1    3600 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR28
U 1 1 56D4DC2D
P 3600 6450
F 0 "#PWR28" H 3600 6200 50  0001 C CNN
F 1 "GND" H 3600 6300 50  0000 C CNN
F 2 "" H 3600 6450 50  0000 C CNN
F 3 "" H 3600 6450 50  0000 C CNN
	1    3600 6450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 6400 3600 6450
Connection ~ 3600 5650
Connection ~ 3600 3300
Connection ~ 3600 950 
$Comp
L RKZ1212D X3
U 1 1 56D4EB01
P 4350 8350
F 0 "X3" H 4350 8550 60  0000 C CNN
F 1 "RKZ1215D" H 4350 8150 60  0000 C CNN
F 2 "00_Passives:RKZXXXXD" H 4250 8450 60  0001 C CNN
F 3 "" H 4350 8350 60  0000 C CNN
	1    4350 8350
	1    0    0    -1  
$EndComp
$Comp
L +15V #PWR41
U 1 1 56D4EC18
P 4900 7800
F 0 "#PWR41" H 4900 7760 30  0001 C CNN
F 1 "+15V" H 4900 7910 30  0000 C CNN
F 2 "" H 4900 7800 60  0000 C CNN
F 3 "" H 4900 7800 60  0000 C CNN
	1    4900 7800
	1    0    0    -1  
$EndComp
$Comp
L -15V #PWR42
U 1 1 56D4EC7D
P 4900 8900
F 0 "#PWR42" H 4900 9030 20  0001 C CNN
F 1 "-15V" H 4900 9000 30  0000 C CNN
F 2 "" H 4900 8900 60  0000 C CNN
F 3 "" H 4900 8900 60  0000 C CNN
	1    4900 8900
	-1   0    0    1   
$EndComp
Wire Wire Line
	4750 8450 4900 8450
Wire Wire Line
	4900 8450 4900 8900
Wire Wire Line
	4900 8250 4750 8250
Wire Wire Line
	4900 7800 4900 8250
$Comp
L GND #PWR57
U 1 1 56D4EE79
P 5700 8350
F 0 "#PWR57" H 5700 8100 50  0001 C CNN
F 1 "GND" H 5700 8200 50  0000 C CNN
F 2 "" H 5700 8350 50  0000 C CNN
F 3 "" H 5700 8350 50  0000 C CNN
	1    5700 8350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4750 8350 5700 8350
$Comp
L +12V #PWR15
U 1 1 56D4F097
P 3450 8200
F 0 "#PWR15" H 3450 8050 50  0001 C CNN
F 1 "+12V" H 3450 8340 50  0000 C CNN
F 2 "" H 3450 8200 50  0000 C CNN
F 3 "" H 3450 8200 50  0000 C CNN
	1    3450 8200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 8200 3450 8300
Wire Wire Line
	3450 8250 3950 8250
$Comp
L C C71
U 1 1 56D4F09F
P 3450 8500
F 0 "C71" H 3500 8600 50  0000 L CNN
F 1 "10uF" H 3500 8400 50  0000 L CNN
F 2 "00_Passives:C1210" H 3450 8500 60  0001 C CNN
F 3 "" H 3450 8500 60  0000 C CNN
	1    3450 8500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR16
U 1 1 56D4F0A5
P 3450 8800
F 0 "#PWR16" H 3450 8550 50  0001 C CNN
F 1 "GND" H 3450 8650 50  0000 C CNN
F 2 "" H 3450 8800 50  0000 C CNN
F 3 "" H 3450 8800 50  0000 C CNN
	1    3450 8800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 8700 3450 8800
Connection ~ 3450 8250
Wire Wire Line
	3450 8750 3850 8750
Wire Wire Line
	3850 8750 3850 8450
Wire Wire Line
	3850 8450 3950 8450
Connection ~ 3450 8750
$Comp
L C C77
U 1 1 56D4FE83
P 5050 8100
F 0 "C77" H 5100 8200 50  0000 L CNN
F 1 "10uF" H 5100 8000 50  0000 L CNN
F 2 "00_Passives:C1210" H 5050 8100 60  0001 C CNN
F 3 "" H 5050 8100 60  0000 C CNN
	1    5050 8100
	1    0    0    -1  
$EndComp
$Comp
L C C78
U 1 1 56D4FECA
P 5150 8600
F 0 "C78" H 5200 8700 50  0000 L CNN
F 1 "10uF" H 5200 8500 50  0000 L CNN
F 2 "00_Passives:C1210" H 5150 8600 60  0001 C CNN
F 3 "" H 5150 8600 60  0000 C CNN
	1    5150 8600
	1    0    0    -1  
$EndComp
$Comp
L C C82
U 1 1 56D4FF22
P 5450 8100
F 0 "C82" H 5500 8200 50  0000 L CNN
F 1 "10uF" H 5500 8000 50  0000 L CNN
F 2 "00_Passives:C1210" H 5450 8100 60  0001 C CNN
F 3 "" H 5450 8100 60  0000 C CNN
	1    5450 8100
	1    0    0    -1  
$EndComp
$Comp
L C C83
U 1 1 56D4FF28
P 5550 8600
F 0 "C83" H 5600 8700 50  0000 L CNN
F 1 "10uF" H 5600 8500 50  0000 L CNN
F 2 "00_Passives:C1210" H 5550 8600 60  0001 C CNN
F 3 "" H 5550 8600 60  0000 C CNN
	1    5550 8600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 8300 5050 8350
Connection ~ 5050 8350
Wire Wire Line
	5150 8350 5150 8400
Connection ~ 5150 8350
Wire Wire Line
	5450 8350 5450 8300
Connection ~ 5450 8350
Wire Wire Line
	5550 8350 5550 8400
Connection ~ 5550 8350
Wire Wire Line
	4900 8850 5550 8850
Wire Wire Line
	5550 8850 5550 8800
Wire Wire Line
	5150 8800 5150 8850
Connection ~ 5150 8850
Connection ~ 4900 8850
Wire Wire Line
	4900 7850 5450 7850
Wire Wire Line
	5450 7850 5450 7900
Connection ~ 4900 7850
Wire Wire Line
	5050 7850 5050 7900
Connection ~ 5050 7850
$Comp
L C C89
U 1 1 56D563C5
P 5550 6000
F 0 "C89" H 5600 6100 50  0000 L CNN
F 1 "10uF" H 5600 5900 50  0000 L CNN
F 2 "00_Passives:C1210" H 5550 6000 60  0001 C CNN
F 3 "" H 5550 6000 60  0000 C CNN
	1    5550 6000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR54
U 1 1 56D563CB
P 5550 6250
F 0 "#PWR54" H 5550 6000 50  0001 C CNN
F 1 "GND" H 5550 6100 50  0000 C CNN
F 2 "" H 5550 6250 50  0000 C CNN
F 3 "" H 5550 6250 50  0000 C CNN
	1    5550 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 6200 5550 6250
Wire Wire Line
	5550 5800 5550 5650
Connection ~ 5550 5650
$EndSCHEMATC
