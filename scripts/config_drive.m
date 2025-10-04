#{
 settings.mac="c2:80:12:1c:0d:00";
 settings.led=0;
 settings.encoder=0;
 settings.poles=0;
 settings.angle_offset=0;
 settings.P=0.2;
 settings.I=5e-3;
 settings.L=1;
 settings.overvoltage=70;
 settings.overcurrent=5;
#}
function result=config_drive(settings)
	txt=sprintf("../software/rt/set_config %s %d %d %d %f %f %f %f %f %f\n",
		settings.mac, settings.led, settings.encoder, settings.poles,
		settings.angle_offset, settings.P, settings.I, settings.L,
		settings.overvoltage, settings.overcurrent
	);
	result=system(txt);
endfunction
