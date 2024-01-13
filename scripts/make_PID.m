if 1
	# X
	# @125V Vmax=3200mm/min 
	# 1000 mm/s^2
	#
	Kp=0.04;		# Best so far
	Kd=-0.8e-3;
	Ki=1e-3;
	f=[1e-6, 4e-3/3/2^20];	# factor between encoders
	F=0.5;
elseif 1
	# X
	# @125V Vmax=3200mm/min 
	# 1000 mm/s^2
	#
	Kp=0.20;		# Best so far
	Kd=-0.8e-3;
	Ki=5e-3;
	f=[5e-6, 4e-3/3/2^20];	# factor between encoders
	F=0.5;
elseif 1
	# Z
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
	[Kp*F, -Kd+Kp*f(2)/f(1)*(1-F)],1/4500
)
