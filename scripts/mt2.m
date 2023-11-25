#
# function data=mt2(I)
# Send a current profile to the drive and record the result
#
function data=mt2(I)
	# a.angle_offset=0.885
	# Max RPM @ 150V = 52.16/222222e-9*60/2^14 = 860 RPM
	save -text I I
	command="../software/rt/motion_test2 I >log"
	system(command);
	p; 
	figure(1)
	cursor_plot(data.I);
	figure(2)
	cursor_plot(data.V);
endfunction
