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

data.time=log(:,1)-log(1,1)+log(:,2)*1e-9;
data.tx_time=log(:,3)-log(:,3)+log(:,4)*1e-9;
data.rx_time=log(:,5)-log(:,5)+log(:,6)*1e-9;

if size(log,2)>26
	data.Iset=log(:,[27,28]);
end
if size(log,2)>29
	data.setpoint=log(:,[27,30]);
	data.error=log(:,[31,32]);
end
