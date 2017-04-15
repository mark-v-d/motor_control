#{
	r=load_fix;
	z={}; for x=1:length(r); z{end+1}=s{x}.d;end
	z=merge(z{:})
	[s2,x0]=arx(z,2);
#}

s=ss(	[0.892,0;1,1],
	[-5.88235294117647e-05; 0],
	[0,1],
	0,
	1/4500,
	"inname", {"I"},
	"outname", {"pos"},
	"stname", {"speed", "position"}
);

# Valid 0.9 -> 0.45
L=place(s.',f)'
observer=ss(s.a-L*s.c,[s.b,L],s.c,[s.d, 0],s.tsam);
observer.stname=s.stname;
observer.inname={s.inname{:}, "fb"};
observer.outname=s.outname



figure(1)
[y t x]=lsim(observer,[r{n}.i,r{n}.pos]); 
plot(
	diff(r{n}.pos),";speed;",
	x(:,1),";est;"
)
title(r{n}.name)
figure(2)
plot(
	r{n}.pos,";pos;",
	y,";est;",
	r{n}.i/10,";i;")
