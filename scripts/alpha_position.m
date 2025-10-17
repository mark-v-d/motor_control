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



center=1080
boost=30
f1=center/sqrt(boost)
f2=center*sqrt(boost)

TYPE3=[ss(c2d(
	zpk([-f1 -f1],[0 -f2 -f2],1.1e5*boost^2),
	1/4500)*tf('z',1/4500)),0];
TYPE3.A=(abs(TYPE3.A)>1e-6).*TYPE3.A;

#{
P=[0 30/4500 1/4500/10;30 0 0;0 0 0];
traj=mp(P);
m=motion(traj(:,1)*[1 0],TYPE3,scale,6);
plot(m.error(1:end-1,1),";new;",ref.error(1:end-1,1),";ref;");
#}
end
