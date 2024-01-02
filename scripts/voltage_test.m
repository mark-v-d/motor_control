#
# function data=mt2(I)
# Send a current profile to the drive and record the result
#
function data=voltage_test(V, old=[])
	# a.angle_offset=0.885
	# Max RPM @ 150V = 52.16/222222e-9*60/2^14 = 860 RPM
	save -text V V
	command=sprintf("../software/rt/voltage_setpoints V >log");
	system(command);
	p; 
	if length(old)
		figure(1)
		cursor_plot(old.I,";old;",data.I,";I;");
		figure(2)
		cursor_plot(old.V,";old;",data.V,";V;");
		figure(3)
		cursor_plot(
			movmean(diff(data.position),10),";speed;",
			movmean(diff(old.position),10),";old speed;"
		)
	else
		figure(1)
		cursor_plot(data.I,";I;",data.limit,";limit;")
		figure(2)
		cursor_plot(data.V,";V;");
		figure(3)
		cursor_plot(
			movmean(diff(data.position),10),";pos;"
		)
	end
endfunction
