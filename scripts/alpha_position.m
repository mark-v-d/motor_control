#{
	Z-as
	resolutie=2^15/mm
#}
if exist("tuning_z")
Kp=0.020;
Kd=-2e-1;
Ki=300e-6;

PID=ss(
	[0 0;
	 0 1],
	[0 Kd;
	 0 Ki],
	[1 1],
	[0 -Kd+Kp],1/4500
)
else
#{
	X-as
	resolutie=2^15*(2/3)
#}

#Kp=0.030;
#Kd=2.5e-1;
#Ki=380e-6;
Kp=700
Kd=11000;
Ki=5;
scale=[-2^15*2/3 1];
limit=3;

PID_fd=ss(
	[0 0;
	 0 1],
	[-Kd 0;
	 Ki 0],
	[1 1],
	[Kd+Kp 0],1/4500
)

PID=ss(
	[0 0 0;
	 1 0 0;
	 0 0 1],
	[-Kd/2 0;
	 0 0;
	 Ki 0],
	[0 1 1],
	[Kd/2+Kp 0],1/4500
)

TYPE3=[ss(c2d(zpk([-150 -150],[0 -1500 -1500],4e6),1/4500)*tf('z',1/4500)),0];
end
