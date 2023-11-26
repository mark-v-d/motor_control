load log
log=log(2:end-1,:);
data.position=log(:,9);
data.angle=log(:,10);
data.valid=log(:,11);
data.I=log(:,[12,13]);
data.V=log(:,[14,15]);
data.rx_counter=log(:,20);
data.rx_data=log(:,21:24);
data.glass_counter=log(:,25);
data.glass_index=log(:,26);
data.setpoint=log(:,27);
data.Iset=log(:,[28,29]);
if size(log,2)>29
	data.setpoint=log(:,[27,30]);
	data.error=log(:,[31,32]);
end
