function [ret t]=current(setpoints)
	save -text setpoints.mdat setpoints
	ret=system("../software/rt/current_setpoints setpoints.mdat >t")
	load t
	ret=structure_log(t);
endfunction
