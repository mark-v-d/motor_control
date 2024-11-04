EESchema Schematic File Version 5
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
$Comp
L Local:1EDI20N12AF U?
U 1 1 5DC230EA
P 4150 2900
AR Path="/5DC1EE24/5DC230EA" Ref="U?"  Part="1" 
AR Path="/5BD49AD2/5DC230EA" Ref="U?"  Part="1" 
AR Path="/5BDD570D/5DC230EA" Ref="U?"  Part="1" 
F 0 "U?" H 4150 3200 60  0000 C CNN
F 1 "1EDI20N12AF" H 4150 2600 60  0000 C CNN
F 2 "" H 4150 2900 60  0001 C CNN
F 3 "" H 4150 3250 60  0001 C CNN
F 4 "1EDI20N12AFXUMA1" H 4350 3450 60  0001 C CNN "PartNr"
F 5 "1EDI20N12AFXUMA1CT-ND" H 4550 3650 60  0001 C CNN "Digikey"
F 6 "Infineon" H 4250 3350 60  0001 C CNN "Manufacturer"
	1    4150 2900
	1    0    0    -1  
$EndComp
$Comp
L small_drive-rescue:D-device1-small_drive-rescue D?
U 1 1 5DC2CDBA
P 4850 2350
AR Path="/5DC1EE24/5DC2CDBA" Ref="D?"  Part="1" 
AR Path="/5BD49AD2/5DC2CDBA" Ref="D?"  Part="1" 
AR Path="/5BDD570D/5DC2CDBA" Ref="D?"  Part="1" 
F 0 "D?" H 4850 2450 50  0000 C CNN
F 1 "STTH112A" H 4850 2250 50  0000 C CNN
F 2 "" H 4850 2350 50  0001 C CNN
F 3 "" H 4850 2350 50  0001 C CNN
	1    4850 2350
	-1   0    0    -1  
$EndComp
$Comp
L small_drive-rescue:+12P-power1-small_drive-rescue #PWR?
U 1 1 5BDAE3A2
P 4600 2250
AR Path="/5DC1EE24/5BDAE3A2" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BDAE3A2" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BDAE3A2" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4600 2100 50  0001 C CNN
F 1 "+12P" H 4600 2390 50  0000 C CNN
F 2 "" H 4600 2250 50  0001 C CNN
F 3 "" H 4600 2250 50  0001 C CNN
	1    4600 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 2350 4600 3350
Wire Wire Line
	4600 2250 4600 2350
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5DC24BAD
P 5800 2850
AR Path="/5DC1EE24/5DC24BAD" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5DC24BAD" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5DC24BAD" Ref="R?"  Part="1" 
F 0 "R?" V 5880 2850 50  0000 C CNN
F 1 "4.7" V 5800 2850 50  0000 C CNN
F 2 "" V 5730 2850 50  0001 C CNN
F 3 "" H 5800 2850 50  0001 C CNN
	1    5800 2850
	0    1    -1   0   
$EndComp
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5DC2508E
P 5800 2950
AR Path="/5DC1EE24/5DC2508E" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5DC2508E" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5DC2508E" Ref="R?"  Part="1" 
F 0 "R?" V 5880 2950 50  0000 C CNN
F 1 "4.7" V 5800 2950 50  0000 C CNN
F 2 "" V 5730 2950 50  0001 C CNN
F 3 "" H 5800 2950 50  0001 C CNN
	1    5800 2950
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 3200 5550 3200
$Comp
L small_drive-rescue:C-device1-small_drive-rescue C?
U 1 1 5DC2AC71
P 5550 2600
AR Path="/5DC1EE24/5DC2AC71" Ref="C?"  Part="1" 
AR Path="/5BD49AD2/5DC2AC71" Ref="C?"  Part="1" 
AR Path="/5BDD570D/5DC2AC71" Ref="C?"  Part="1" 
AR Path="/5DC2AC71" Ref="C?"  Part="1" 
F 0 "C?" H 5575 2700 50  0000 L CNN
F 1 "1uF" H 5575 2500 50  0000 L CNN
F 2 "" H 5588 2450 50  0001 C CNN
F 3 "" H 5550 2600 50  0001 C CNN
	1    5550 2600
	1    0    0    -1  
$EndComp
Connection ~ 5550 3050
Wire Wire Line
	4550 2950 5650 2950
Wire Wire Line
	5550 3200 5550 3050
Wire Wire Line
	5550 2750 5550 3050
Wire Wire Line
	4550 2850 5650 2850
Wire Wire Line
	5400 2350 5550 2350
Wire Wire Line
	5550 2350 5550 2450
Wire Wire Line
	5000 2350 5050 2350
Wire Wire Line
	4600 2350 4700 2350
Connection ~ 5400 2350
Wire Wire Line
	5400 2350 5350 2350
Wire Wire Line
	4550 2750 5400 2750
Wire Wire Line
	5400 2750 5400 2350
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5DC4C354
P 5200 2350
AR Path="/5DC1EE24/5DC4C354" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5DC4C354" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5DC4C354" Ref="R?"  Part="1" 
F 0 "R?" V 5280 2350 50  0000 C CNN
F 1 "4.7" V 5200 2350 50  0000 C CNN
F 2 "" V 5130 2350 50  0001 C CNN
F 3 "" H 5200 2350 50  0001 C CNN
	1    5200 2350
	0    1    1    0   
$EndComp
Connection ~ 4600 2350
Wire Wire Line
	5950 2850 6000 2850
Wire Wire Line
	6000 2850 6000 2950
Wire Wire Line
	6000 2950 6050 2950
Connection ~ 6000 2950
$Comp
L Local:+800V #PWR?
U 1 1 5BD3A9AD
P 6300 2550
AR Path="/5DC1EE24/5BD3A9AD" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD3A9AD" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD3A9AD" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 6300 2400 50  0001 C CNN
F 1 "+800V" H 6300 2690 50  0000 C CNN
F 2 "" H 6300 2550 50  0001 C CNN
F 3 "" H 6300 2550 50  0001 C CNN
	1    6300 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 2950 5950 2950
Wire Wire Line
	5400 3350 5550 3350
Wire Wire Line
	5550 3350 5550 3400
Connection ~ 5400 3350
Wire Wire Line
	6300 3250 6300 3350
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5DC34640
P 5800 3900
AR Path="/5DC1EE24/5DC34640" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5DC34640" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5DC34640" Ref="R?"  Part="1" 
F 0 "R?" V 5880 3900 50  0000 C CNN
F 1 "4.7" V 5800 3900 50  0000 C CNN
F 2 "" V 5730 3900 50  0001 C CNN
F 3 "" H 5800 3900 50  0001 C CNN
	1    5800 3900
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 4150 5550 4150
Wire Wire Line
	5550 4150 5550 4000
Wire Wire Line
	5550 3700 5550 4000
Connection ~ 5550 4000
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5DC34636
P 5800 3800
AR Path="/5DC1EE24/5DC34636" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5DC34636" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5DC34636" Ref="R?"  Part="1" 
F 0 "R?" V 5880 3800 50  0000 C CNN
F 1 "4.7" V 5800 3800 50  0000 C CNN
F 2 "" V 5730 3800 50  0001 C CNN
F 3 "" H 5800 3800 50  0001 C CNN
	1    5800 3800
	0    1    -1   0   
$EndComp
Wire Wire Line
	5950 3800 6000 3800
$Comp
L small_drive-rescue:C-device1-small_drive-rescue C?
U 1 1 5DC34656
P 5550 3550
AR Path="/5DC1EE24/5DC34656" Ref="C?"  Part="1" 
AR Path="/5BD49AD2/5DC34656" Ref="C?"  Part="1" 
AR Path="/5BDD570D/5DC34656" Ref="C?"  Part="1" 
AR Path="/5DC34656" Ref="C?"  Part="1" 
F 0 "C?" H 5575 3650 50  0000 L CNN
F 1 "1uF" H 5575 3450 50  0000 L CNN
F 2 "" H 5588 3400 50  0001 C CNN
F 3 "" H 5550 3550 50  0001 C CNN
	1    5550 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3700 5400 3350
Wire Wire Line
	5400 3350 5350 3350
$Comp
L small_drive-rescue:R_Shunt-device1-small_drive-rescue R?
U 1 1 5DC454A2
P 6300 4550
AR Path="/5DC1EE24/5DC454A2" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5DC454A2" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5DC454A2" Ref="R?"  Part="1" 
F 0 "R?" V 6125 4550 50  0000 C CNN
F 1 "R_Shunt" V 6200 4550 50  0000 C CNN
F 2 "" V 6230 4550 50  0001 C CNN
F 3 "" H 6300 4550 50  0001 C CNN
	1    6300 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 4350 6300 4200
Wire Wire Line
	6600 4000 6600 4450
Wire Wire Line
	6600 4450 6450 4450
Wire Wire Line
	6450 4650 6600 4650
Wire Wire Line
	7150 4700 7200 4700
$Comp
L small_drive-rescue:C-device1-small_drive-rescue C?
U 1 1 5BCA91CA
P 6900 4550
AR Path="/5DC1EE24/5BCA91CA" Ref="C?"  Part="1" 
AR Path="/5BD49AD2/5BCA91CA" Ref="C?"  Part="1" 
AR Path="/5BDD570D/5BCA91CA" Ref="C?"  Part="1" 
AR Path="/5BCA91CA" Ref="C?"  Part="1" 
F 0 "C?" V 6648 4432 50  0000 L CNN
F 1 "100nF" V 6739 4432 50  0000 L CNN
F 2 "" H 6938 4400 50  0001 C CNN
F 3 "" H 6900 4550 50  0001 C CNN
	1    6900 4550
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 4550 7200 4550
Wire Wire Line
	6600 4000 6800 4000
Wire Wire Line
	6750 4550 6700 4550
$Comp
L Local:NMOS_KELVIN M?
U 1 1 5DC21BB9
P 6150 2950
AR Path="/5DC1EE24/5DC21BB9" Ref="M?"  Part="1" 
AR Path="/5BD49AD2/5DC21BB9" Ref="M?"  Part="1" 
AR Path="/5BDD570D/5DC21BB9" Ref="M?"  Part="1" 
F 0 "M?" H 6400 3100 60  0000 L CNN
F 1 "NMOS_KELVIN" H 6400 3000 60  0000 L CNN
F 2 "" H 6300 3000 60  0001 C CNN
F 3 "" H 6150 2950 60  0000 C CNN
	1    6150 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 2550 6300 2750
$Comp
L small_drive-rescue:L-device1-small_drive-rescue L?
U 1 1 5BDA14E9
P 7250 3350
AR Path="/5DC1EE24/5BDA14E9" Ref="L?"  Part="1" 
AR Path="/5BD49AD2/5BDA14E9" Ref="L?"  Part="1" 
AR Path="/5BDD570D/5BDA14E9" Ref="L?"  Part="1" 
F 0 "L?" V 7200 3350 50  0000 C CNN
F 1 "22uH" V 7325 3350 50  0000 C CNN
F 2 "00_Passives:WE-HCF2013" H 7250 3350 50  0001 C CNN
F 3 "" H 7250 3350 50  0001 C CNN
	1    7250 3350
	0    1    1    0   
$EndComp
Wire Wire Line
	7100 3350 6300 3350
Connection ~ 6300 3350
Wire Wire Line
	6300 3350 6300 3700
$Comp
L Local:NMOS_KELVIN M?
U 1 1 5DC3461F
P 6150 3900
AR Path="/5DC1EE24/5DC3461F" Ref="M?"  Part="1" 
AR Path="/5BD49AD2/5DC3461F" Ref="M?"  Part="1" 
AR Path="/5BDD570D/5DC3461F" Ref="M?"  Part="1" 
F 0 "M?" H 6400 4050 60  0000 L CNN
F 1 "NMOS_KELVIN" H 6400 3950 60  0000 L CNN
F 2 "" H 6300 3950 60  0001 C CNN
F 3 "" H 6150 3900 60  0000 C CNN
	1    6150 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 3800 6000 3900
Wire Wire Line
	6000 3900 5950 3900
Wire Wire Line
	6000 3900 6050 3900
Connection ~ 6000 3900
$Comp
L Local:R_N2 R?
U 1 1 5BC82133
P 6950 4000
AR Path="/5DC1EE24/5BC82133" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5BC82133" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5BC82133" Ref="R?"  Part="1" 
F 0 "R?" V 7030 4000 50  0000 C CNN
F 1 "100" V 6950 4000 50  0000 C CNN
F 2 "" V 6880 4000 50  0001 C CNN
F 3 "" H 6950 4000 50  0001 C CNN
	1    6950 4000
	0    1    1    0   
$EndComp
Wire Wire Line
	7150 4000 7150 4400
Wire Wire Line
	7150 4400 7200 4400
Wire Wire Line
	7100 4000 7150 4000
Connection ~ 7150 4000
$Comp
L Local:R_N2 R?
U 1 1 5BC86BCC
P 6950 5100
AR Path="/5DC1EE24/5BC86BCC" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5BC86BCC" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5BC86BCC" Ref="R?"  Part="1" 
F 0 "R?" V 7030 5100 50  0000 C CNN
F 1 "100" V 6950 5100 50  0000 C CNN
F 2 "" V 6880 5100 50  0001 C CNN
F 3 "" H 6950 5100 50  0001 C CNN
	1    6950 5100
	0    1    1    0   
$EndComp
Wire Wire Line
	7150 5100 7150 4700
Wire Wire Line
	7150 5100 7100 5100
Wire Wire Line
	6700 4550 6700 5250
Wire Wire Line
	6700 5250 7200 5250
Wire Wire Line
	7200 5300 7200 5250
Wire Wire Line
	6600 4650 6600 5100
Wire Wire Line
	6600 5100 6800 5100
$Comp
L small_drive-rescue:GNDPWR-power1-small_drive-rescue #PWR?
U 1 1 5BD0E6F5
P 6300 4800
AR Path="/5DC1EE24/5BD0E6F5" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD0E6F5" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD0E6F5" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 6300 4600 50  0001 C CNN
F 1 "GNDPWR" H 6300 4670 50  0000 C CNN
F 2 "" H 6300 4750 50  0001 C CNN
F 3 "" H 6300 4750 50  0001 C CNN
	1    6300 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 4800 6300 4750
Wire Wire Line
	5550 3050 4550 3050
$Comp
L Local:1EDI20N12AF U?
U 1 1 5DC3462C
P 4150 3850
AR Path="/5DC1EE24/5DC3462C" Ref="U?"  Part="1" 
AR Path="/5BD49AD2/5DC3462C" Ref="U?"  Part="1" 
AR Path="/5BDD570D/5DC3462C" Ref="U?"  Part="1" 
F 0 "U?" H 4150 4150 60  0000 C CNN
F 1 "1EDI20N12AF" H 4150 3550 60  0000 C CNN
F 2 "" H 4150 3850 60  0001 C CNN
F 3 "" H 4150 4200 60  0001 C CNN
F 4 "1EDI20N12AFXUMA1" H 4350 4400 60  0001 C CNN "PartNr"
F 5 "1EDI20N12AFXUMA1CT-ND" H 4550 4600 60  0001 C CNN "Digikey"
F 6 "Infineon" H 4250 4300 60  0001 C CNN "Manufacturer"
	1    4150 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 3900 5650 3900
Wire Wire Line
	5550 4000 4550 4000
Wire Wire Line
	4550 3700 5400 3700
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5DC34671
P 5200 3350
AR Path="/5DC1EE24/5DC34671" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5DC34671" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5DC34671" Ref="R?"  Part="1" 
F 0 "R?" V 5280 3350 50  0000 C CNN
F 1 "4.7" V 5200 3350 50  0000 C CNN
F 2 "" V 5130 3350 50  0001 C CNN
F 3 "" H 5200 3350 50  0001 C CNN
	1    5200 3350
	0    1    1    0   
$EndComp
Wire Wire Line
	4550 3800 5650 3800
Wire Wire Line
	4600 3350 5050 3350
Text HLabel 3350 3800 0    50   Input ~ 0
L
Text HLabel 3350 3900 0    50   Input ~ 0
DISABLE
Wire Wire Line
	3350 3900 3500 3900
Connection ~ 3500 3900
Text HLabel 3350 2850 0    50   Input ~ 0
H
Wire Wire Line
	3350 2850 3750 2850
Wire Wire Line
	3750 2750 3700 2750
Wire Wire Line
	3500 2950 3750 2950
$Comp
L Local:+3.3V #PWR?
U 1 1 5BD1C2C0
P 3700 2600
AR Path="/5DC1EE24/5BD1C2C0" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD1C2C0" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD1C2C0" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3750 2630 20  0001 C CNN
F 1 "+3.3V" H 3700 2690 30  0000 C CNN
F 2 "" H 3700 2600 60  0000 C CNN
F 3 "" H 3700 2600 60  0000 C CNN
	1    3700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 2600 3700 2750
Connection ~ 3700 2750
Wire Wire Line
	3700 2750 3700 3700
Wire Wire Line
	3700 3700 3750 3700
Wire Wire Line
	3600 4000 3600 3050
Wire Wire Line
	3600 3050 3750 3050
Wire Wire Line
	3500 3900 3500 2950
Wire Wire Line
	3350 3800 3750 3800
$Comp
L small_drive-rescue:GND-power1-small_drive-rescue #PWR?
U 1 1 5BD14BDF
P 3600 4150
AR Path="/5DC1EE24/5BD14BDF" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD14BDF" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD14BDF" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3600 3900 50  0001 C CNN
F 1 "GND" H 3600 4000 50  0000 C CNN
F 2 "" H 3600 4150 50  0001 C CNN
F 3 "" H 3600 4150 50  0001 C CNN
	1    3600 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 4150 3600 4000
Wire Wire Line
	3600 4000 3750 4000
Connection ~ 3600 4000
Wire Wire Line
	3500 3900 3750 3900
$Comp
L small_drive-rescue:C-device1-small_drive-rescue C?
U 1 1 5BD2013E
P 3500 5100
AR Path="/5DC1EE24/5BD2013E" Ref="C?"  Part="1" 
AR Path="/5BD49AD2/5BD2013E" Ref="C?"  Part="1" 
AR Path="/5BDD570D/5BD2013E" Ref="C?"  Part="1" 
AR Path="/5BD2013E" Ref="C?"  Part="1" 
F 0 "C?" H 3525 5200 50  0000 L CNN
F 1 "100nF" H 3525 5000 50  0000 L CNN
F 2 "" H 3538 4950 50  0001 C CNN
F 3 "" H 3500 5100 50  0001 C CNN
	1    3500 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 5300 3500 5250
Wire Wire Line
	3500 4900 3500 4950
Connection ~ 3300 4900
Wire Wire Line
	3300 4900 3500 4900
$Comp
L small_drive-rescue:GND-power1-small_drive-rescue #PWR?
U 1 1 5BD2576B
P 3300 5350
AR Path="/5DC1EE24/5BD2576B" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD2576B" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD2576B" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3300 5100 50  0001 C CNN
F 1 "GND" H 3300 5200 50  0000 C CNN
F 2 "" H 3300 5350 50  0001 C CNN
F 3 "" H 3300 5350 50  0001 C CNN
	1    3300 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 5250 3150 5300
Wire Wire Line
	3150 5300 3300 5300
Wire Wire Line
	3300 5300 3300 5350
Connection ~ 3300 5300
Wire Wire Line
	3300 5300 3500 5300
$Comp
L small_drive-rescue:C-device1-small_drive-rescue C?
U 1 1 5BD1FB1D
P 3150 5100
AR Path="/5DC1EE24/5BD1FB1D" Ref="C?"  Part="1" 
AR Path="/5BD49AD2/5BD1FB1D" Ref="C?"  Part="1" 
AR Path="/5BDD570D/5BD1FB1D" Ref="C?"  Part="1" 
AR Path="/5BD1FB1D" Ref="C?"  Part="1" 
F 0 "C?" H 3175 5200 50  0000 L CNN
F 1 "100nF" H 3175 5000 50  0000 L CNN
F 2 "" H 3188 4950 50  0001 C CNN
F 3 "" H 3150 5100 50  0001 C CNN
	1    3150 5100
	1    0    0    -1  
$EndComp
$Comp
L Local:+3.3V #PWR?
U 1 1 5BD22A2F
P 3300 4850
AR Path="/5DC1EE24/5BD22A2F" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD22A2F" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD22A2F" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3350 4880 20  0001 C CNN
F 1 "+3.3V" H 3300 4940 30  0000 C CNN
F 2 "" H 3300 4850 60  0000 C CNN
F 3 "" H 3300 4850 60  0000 C CNN
	1    3300 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 4950 3150 4900
Wire Wire Line
	3150 4900 3300 4900
Wire Wire Line
	3300 4900 3300 4850
$Comp
L Local:THS4531 U?
U 1 1 5DC626A2
P 7600 4550
AR Path="/5DC1EE24/5DC626A2" Ref="U?"  Part="1" 
AR Path="/5BD49AD2/5DC626A2" Ref="U?"  Part="1" 
AR Path="/5BDD570D/5DC626A2" Ref="U?"  Part="1" 
F 0 "U?" H 7750 4850 60  0000 L CNN
F 1 "THS4531" H 7650 4250 60  0000 L CNN
F 2 "SO8N" H 7550 4750 60  0001 C CNN
F 3 "" H 7600 4550 60  0000 C CNN
F 4 "TI" H 7950 5150 60  0001 C CNN "Manufacturer"
F 5 "THS4531" H 8050 5250 60  0001 C CNN "PartNr"
F 6 "2115228" H 7750 4950 60  0001 C CNN "Farnell"
F 7 "296-30379-1-ND" H 7850 5050 60  0001 C CNN "Digikey"
F 8 "8,1,2,3,6,4,5,7" H 8250 5350 60  0001 C CNN "Spice_Node_Sequence"
F 9 "/home/mark/kicad/home/Spice_models/THS4531_PSPICE_AIO/ths4531.lib" H 8450 5550 60  0001 C CNN "Spice_Lib_File"
	1    7600 4550
	1    0    0    -1  
$EndComp
$Comp
L Local:R_N2 R?
U 1 1 5BC8AA62
P 7950 4000
AR Path="/5DC1EE24/5BC8AA62" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5BC8AA62" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5BC8AA62" Ref="R?"  Part="1" 
F 0 "R?" V 8030 4000 50  0000 C CNN
F 1 "2k" V 7950 4000 50  0000 C CNN
F 2 "" V 7880 4000 50  0001 C CNN
F 3 "" H 7950 4000 50  0001 C CNN
	1    7950 4000
	0    1    1    0   
$EndComp
$Comp
L small_drive-rescue:+5P-power1-small_drive-rescue #PWR?
U 1 1 5BD0C586
P 7600 3850
AR Path="/5DC1EE24/5BD0C586" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD0C586" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD0C586" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 7600 3700 50  0001 C CNN
F 1 "+5P" H 7600 3990 50  0000 C CNN
F 2 "" H 7600 3850 50  0001 C CNN
F 3 "" H 7600 3850 50  0001 C CNN
	1    7600 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7600 3850 7600 3900
Connection ~ 7600 3900
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5BD5740F
P 8350 4000
AR Path="/5DC1EE24/5BD5740F" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5BD5740F" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5BD5740F" Ref="R?"  Part="1" 
F 0 "R?" V 8430 4000 50  0000 C CNN
F 1 "51" V 8350 4000 50  0000 C CNN
F 2 "" V 8280 4000 50  0001 C CNN
F 3 "" H 8350 4000 50  0001 C CNN
	1    8350 4000
	0    1    1    0   
$EndComp
Wire Wire Line
	7400 3350 8550 3350
Wire Wire Line
	8100 4700 8150 4700
Wire Wire Line
	8150 4700 8150 5100
Wire Wire Line
	7800 5100 7150 5100
Connection ~ 7150 5100
Connection ~ 7200 5250
Wire Wire Line
	7200 5250 7600 5250
$Comp
L small_drive-rescue:GNDPWR-power1-small_drive-rescue #PWR?
U 1 1 5BD83EF9
P 7200 5300
AR Path="/5DC1EE24/5BD83EF9" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD83EF9" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD83EF9" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 7200 5100 50  0001 C CNN
F 1 "GNDPWR" H 7200 5170 50  0000 C CNN
F 2 "" H 7200 5250 50  0001 C CNN
F 3 "" H 7200 5250 50  0001 C CNN
	1    7200 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 4000 7150 4000
Wire Wire Line
	7700 4050 7700 3900
Wire Wire Line
	7700 3900 7600 3900
Wire Wire Line
	7600 3900 7600 4050
$Comp
L Local:R_N2 R?
U 1 1 5BC8D487
P 7950 5100
AR Path="/5DC1EE24/5BC8D487" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5BC8D487" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5BC8D487" Ref="R?"  Part="1" 
F 0 "R?" V 8030 5100 50  0000 C CNN
F 1 "2k" V 7950 5100 50  0000 C CNN
F 2 "" V 7880 5100 50  0001 C CNN
F 3 "" H 7950 5100 50  0001 C CNN
	1    7950 5100
	0    1    1    0   
$EndComp
Wire Wire Line
	8150 5100 8100 5100
Wire Wire Line
	7600 5050 7600 5250
$Comp
L small_drive-rescue:GNDPWR-power1-small_drive-rescue #PWR?
U 1 1 5BD10F94
P 8050 5950
AR Path="/5DC1EE24/5BD10F94" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD10F94" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD10F94" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 8050 5750 50  0001 C CNN
F 1 "GNDPWR" H 8050 5820 50  0000 C CNN
F 2 "" H 8050 5900 50  0001 C CNN
F 3 "" H 8050 5900 50  0001 C CNN
	1    8050 5950
	1    0    0    -1  
$EndComp
$Comp
L small_drive-rescue:C-device1-small_drive-rescue C?
U 1 1 5BD7C1F4
P 8050 5750
AR Path="/5DC1EE24/5BD7C1F4" Ref="C?"  Part="1" 
AR Path="/5BD49AD2/5BD7C1F4" Ref="C?"  Part="1" 
AR Path="/5BDD570D/5BD7C1F4" Ref="C?"  Part="1" 
AR Path="/5BD7C1F4" Ref="C?"  Part="1" 
F 0 "C?" H 8075 5850 50  0000 L CNN
F 1 "100nF" H 8075 5650 50  0000 L CNN
F 2 "" H 8088 5600 50  0001 C CNN
F 3 "" H 8050 5750 50  0001 C CNN
	1    8050 5750
	1    0    0    -1  
$EndComp
$Comp
L small_drive-rescue:+5P-power1-small_drive-rescue #PWR?
U 1 1 5BD80453
P 8050 5550
AR Path="/5DC1EE24/5BD80453" Ref="#PWR?"  Part="1" 
AR Path="/5BD49AD2/5BD80453" Ref="#PWR?"  Part="1" 
AR Path="/5BDD570D/5BD80453" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 8050 5400 50  0001 C CNN
F 1 "+5P" H 8050 5690 50  0000 C CNN
F 2 "" H 8050 5550 50  0001 C CNN
F 3 "" H 8050 5550 50  0001 C CNN
	1    8050 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 5950 8050 5900
Wire Wire Line
	8050 5550 8050 5600
Wire Wire Line
	8150 4400 8100 4400
Text HLabel 8550 5100 2    50   Output ~ 0
C_N
Wire Wire Line
	8200 5100 8150 5100
Connection ~ 8150 5100
$Comp
L small_drive-rescue:R-device1-small_drive-rescue R?
U 1 1 5BD58C2E
P 8350 5100
AR Path="/5DC1EE24/5BD58C2E" Ref="R?"  Part="1" 
AR Path="/5BD49AD2/5BD58C2E" Ref="R?"  Part="1" 
AR Path="/5BDD570D/5BD58C2E" Ref="R?"  Part="1" 
F 0 "R?" V 8430 5100 50  0000 C CNN
F 1 "51" V 8350 5100 50  0000 C CNN
F 2 "" V 8280 5100 50  0001 C CNN
F 3 "" H 8350 5100 50  0001 C CNN
	1    8350 5100
	0    1    1    0   
$EndComp
Wire Wire Line
	8550 5100 8500 5100
Wire Wire Line
	8100 4000 8150 4000
Wire Wire Line
	8150 4000 8150 4400
Text HLabel 8550 4000 2    50   Output ~ 0
C_P
Wire Wire Line
	8150 4000 8200 4000
Connection ~ 8150 4000
Wire Wire Line
	8500 4000 8550 4000
Text HLabel 8550 3350 2    50   Output ~ 0
HB
$EndSCHEMATC
