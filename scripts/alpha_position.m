if exist("tuning_z")
	# Z-as
	mac="c2:80:12:1c:0d:00"
	scale=[-2^15 1];
	limit=6.5;

	# PID
	Kp=436;
	Ki=6.5;
	Kd=4400

        # TYPE3
        gain=31e3;
        center=520
        boost=30
else
	# X-as
	mac="c2:00:93:15:0d:00";
	scale=[-2^15*2/3 1];
	limit=6.5;

	# PID
	Kp=700
	Kd=11000;
	Ki=5;

	# TYPE3
	gain=1.1e5;
	center=1080
	boost=30
end


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

f1=center/sqrt(boost)
f2=center*sqrt(boost)
TYPE3=[ss(c2d(
	zpk([-f1 -f1],[0 -f2 -f2],gain*boost^2),
	1/4500)*tf('z',1/4500)),0];
TYPE3.A=(abs(TYPE3.A)>1e-6).*TYPE3.A;

#{
P=[0 30/4500 1/4500/10;30 0 0;0 0 0];
traj=mp(P);
m=motion(traj(:,1)*[1 0],TYPE3,scale,6);
plot(m.error(1:end-1,1),";new;",ref.error(1:end-1,1),";ref;");
#}
