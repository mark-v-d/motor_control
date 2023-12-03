if 1
	# @125V Vmax=3200mm/min
	#
	Kp=0.15;		# Best so far
	Kd=-1.5e-3;
	Ki=2e-3;
	f=[5e-6, 5e-3/3/2^20];	# factor between encoders
	F=0.5;
else
	Kp=0.06211*2.3;
	Kd=-1.0e-3;
	Ki=2e-3;
	f=[5e-6, 5e-3/3/2^20];	# factor between encoders
	F=0.0;
end
PID=ss(
	[0 0;
	 0 1],
	[0,	Kd;
	 Ki,	0],
	[1 1],
	[Kp*F, -Kd+Kp*f2/f1*(1-F)],1/4500
)
