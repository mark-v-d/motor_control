function ret=structure_log(t)

	ret.timestamp=t(:,1:2);
	ret.tx_time=t(:,3:4);
	ret.rx_time=t(:,5:6);
	ret.timer=t(:,7:8);
	ret.position=t(:,9);
	ret.angle=t(:,10);
	ret.encoder_invalid=t(:,11);
	ret.encoder_missing=t(:,12);
	ret.I=t(:,13:14);
	ret.V=t(:,15:16);
	ret.ADC=t(:,17:18);
	ret.timer_delta=t(:,19);
	ret.position2=t(:,25);
	ret.index2=t(:,26);
	ret.Vservo=t(:,27);
	ret.timer_error=(t(:,33));
	s=find(ret.position==0)(end)+1;
	ret.position(1:s)=ret.position(s);

	if(size(t,2)==42)
		printf("motion log\n");
		ret.Isetpoint=t(:,34:35);
		ret.setpoint=t(:,36:37);
		ret.error=t(:,38:39);
	end
end
