load log
log=log(4501:end-1,:);
data.position=log(:,9);
data.angle=log(:,10);
data.valid=log(:,11);
data.I=log(:,[12,13]);
data.V=log(:,[14,15]);
data.rx_counter=log(:,20);
data.rx_data=log(:,21:24);
data.glass_counter=log(:,25);
data.glass_index=log(:,26);
data.Vservo=log(:,27);

data.time=log(:,1)-log(1,1)+log(:,2)*1e-9;
data.tx_time=log(:,3)-log(:,3)+log(:,4)*1e-9;
data.rx_time=log(:,5)-log(:,5)+log(:,6)*1e-9;

if size(log,2)>27
	data.Iset=log(:,[28,29]);
end
if size(log,2)==34
	# voltage test
	data.limit=log(:,[30,31]); 
	data.Vset=log(:,32);
	data.Imax=log(:,33);
	data.Vdelta=log(:,34);
elseif size(log,2)==36
	# Speed test
	data.Imin=log(:,30);
	data.Imax=log(:,31);
	data.P=log(:,32);
	data.speed=log(:,33);
	data.setpoint=log(:,34);
	data.Iset=log(:,[35,36]);
	data=rmfield(data,"error");
elseif size(log,2)>33
	# current test
	data.error=log(:,[32,33]);
	data.timer_error=log(:,34);
	data.counter=log(:,35);
	data.timer_delta=log(:,36);
end
