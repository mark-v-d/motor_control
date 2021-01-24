#
# function trace_init(a,varargin)
#
# a	ARM connection
#
# args	cycevtena	0/1
#	foldevtena	0/1
#	LSUEVTENA	0/1
#	SLEEPEVTENA	0/1
#	EXCEVTENA	0/1
#	CPIEVTENA	0/1
#	exception	0/1
#	pcsample	0/1
#	synctap		0..3
#	cyctap		0/1
#	postinit	0..15
#	postpreset	0..15
#
function trace_init(a,varargin)
	if isfield(a.var,"dbgmcu")
		a.dbgmcu.CR=bitor(a.dbgmcu.CR,0x20);
	end
	a.tpi.ACPR=1;	# Asynchronous Clock Prescaler Register, TPIU_ACPR
	a.tpi.SPPR=2;
	a.tpi.FFCR=0x100;

	# See ARM DDI0403D ID021310 page C1-881 for more details
	control=a.dwt.CTRL;
	control=bitset(control,1,1);	# Cycle counter enable
	for x=1:length(varargin)
		switch(varargin{x})
		case "cycevtena"
			# Enables POSTCNT underflow Event counter packets
			val=varargin{++x};
			control=bitset(control,23,val);
		case "foldevtena"
			# Enables generation of the Folded-instruction counter
			val=varargin{++x};
			control=bitset(control,22,val);
		case "LSUEVTENA"
			# Enables generation of the LSU counter overflow event.
			val=varargin{++x};
			control=bitset(control,21,val);
		case "SLEEPEVTENA"
			# Enables generation of the Sleep counter overflow evnt.
			val=varargin{++x};
			control=bitset(control,20,val);
		case "EXCEVTENA"
			# Enables generation of the Exception overhead counter
			val=varargin{++x};
			control=bitset(control,19,val);
		case "CPIEVTENA"
			# Enables generation of the CPI counter overflow event
			val=varargin{++x};
			control=bitset(control,18,val);
		case "exception"
			# Exception trace enable
			val=varargin{++x};
			control=bitset(control,17,val);
		case "pcsample"
			# Enables use of POSTCNT counter as a timer for
			# Periodic PC sampling
			val=varargin{++x};
			control=bitset(control,13,val);
		case "synctap"
			# Sync packets
			# 0 = Disabled. No Synchronization packets.
			# 1 = Synchronization counter tap at CYCCNT[24]
			# 2 = Synchronization counter tap at CYCCNT[26]
			# 3 = Synchronization counter tap at CYCCNT[28]
			val=varargin{++x};
			control=bitset(control,12,bitand(val,2));
			control=bitset(control,11,bitand(val,1));
		case "cyctap"
			# Selects the position of the POSTCNT tap on the CYCCNT
			val=varargin{++x};
			control=bitset(control,10,val);
		case "postinit"
			val=varargin{++x}*2;
			control=bitand(control,0xfffffe1f);
			control=bitor(control,bitand(val,0x1e));
		case "postpreset"
			val=varargin{++x}*2;
			control=bitand(control,0xffffffe1);
			control=bitor(control,bitand(val,0x1e));
		end
	end

	#a.dwt.CTRL=0x40010401;
	a.dwt.CTRL=control;

	a.itm.LAR=0xC5ACCE55;
	a.itm.TCR=0x0001000d;
	a.itm.TER=0xffffffff;
endfunction
