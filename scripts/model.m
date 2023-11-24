L=3.1e-3
R=10
k=10

motor=ss(-R/L, 1/L, 1, 0)

controller=zpk(-3225.8,0,1)
c2=zpk(-3225.8,[0 -3e4],3e4)
c3=zpk(-3225.8,[0 -1e4],1e4)

C=c2d(controller,1/18e3);
C2=c2d(c2,1/18e3);
C3=c2d(c3,1/18e3);
M=c2d(motor,1/18e3);

T1=feedback(M*k*C);
V1=feedback(k*C,M);
T2=feedback(M*k*C2);
V2=feedback(k*C2,M);
T3=feedback(k*C3*M);
V3=feedback(k*C3,M);
figure(1)
step(linspace(0,2e-3,1000),V1,V2,V3)
figure(2)
step(linspace(0,2e-3,1000),T1,T2,T3)

#
# Create limiter for K2 and simulate limiting
#
K2=ss(C2)/77;
[Klim Kl]=speedup_limiter(K2)
I=[ones(1,10), zeros(1,10)+0.7]
[u t x]=lsim(K2,I)
[s o]=sim(K2,Kl,I,2)
figure(3)
plot(s',";s;",o,";limited;x",u,";unlimited;+",x,";su;")
