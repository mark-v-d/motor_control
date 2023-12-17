#
# function data=mt2(I)
# Send a current profile to the drive and record the result
#
function data=current_test(A,I, old=[])
	# a.angle_offset=0.885
	# Max RPM @ 150V = 52.16/222222e-9*60/2^14 = 860 RPM
	save -text I I
	command=sprintf("../software/rt/current_setpoints I %s >log",
		mac_addr(A));
	system(command);
	p; 
	if length(old)
		figure(1)
		cursor_plot(data.I,";I;",old.I,";old;");
		figure(2)
		cursor_plot(data.V,";V;",old.V,";old;");
	else
		figure(1)
		cursor_plot(data.I,";I;");
		figure(2)
		cursor_plot(data.V,";V;");
	end
endfunction
