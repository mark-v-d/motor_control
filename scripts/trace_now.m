function [r f]=trace_now(scope, make_vcd=1, analog=[])
	tic
	brd=120e6/2;

	# Power board
	if 1
		[d f]=rigol(scope,4);
		if f.srate<250e6
			printf("Sample rate too low\n");
			return
		end
		if !length(d)
			printf("No data (scope not stopped?)\n");
			return
		end
		fetch=toc;
		printf("Data fetched %f\n",fetch); fflush(stdout);
		fflush(stdout);

		if 1
			clk=bitand(d',0x80)>0;
			td=[	bitand(d',0x40)>0, ...
				bitand(d',0x20)>0, ...
				bitand(d',0x10)>0, ...
				bitand(d',0x08)>0, ...
			];

			[result vcd]=trace_parallel([clk,td]); 
			r.motor=trace_decode(result,d);
			r.scope=r.motor.scope;
			r.motor=rmfield(r.motor,"scope");
			r.trace=vcd.trace;
		elseif strcmp(typeinfo(d),"uint8 matrix")
			sdata=bitand(d,1);
			result=trace_deserialize(sdata,f.srate/brd);
			r.power=trace_decode(result,d);
			r.scope=r.power.scope;
			r.power=rmfield(r.power,"scope");
		else
			sdata=d>1.65;
			result=trace_deserialize(sdata,f.srate/brd);
			r.power=trace_decode(result);
		end
		deser=toc;
		printf("Data deserialized %f (%f)\n",deser,deser-fetch);
		fflush(stdout);
	end
	deser=toc;

	# Control board
	if 0
		if !exist("d") || !strcmp(typeinfo(d),"uint8 matrix")
			[ch1 f]=rigol(scope,1);
			if f.srate<250e6
				printf("Sample rate too low\n");
				return
			end
			if !length(ch1)
				printf("No data (scope not stopped?)\n");
				return
			end
			fetch2=toc;
			printf("Data fetched %f\n",fetch2-deser); fflush(stdout);
			fflush(stdout);

			sdata=ch1>1.65;
		else
			fetch2=toc;
			printf("Using bit 1 of digital channel for control\n");
			sdata=bitand(d,2)/2;
		end
		result=trace_deserialize(sdata,f.srate/brd);
		r.control=trace_decode(result);
		deser2=toc;
		printf("Data deserialized %f (%f)\n",deser2,deser2-fetch2);
		fflush(stdout);
	end
	deser2=toc;

	if(isfield(f,"end"))
		r.scope.trigger.time=f.end*f.trig_pos/100;
	end
	for i=analog
		[X f]=rigol(scope,i);
		r.scope.(sprintf("ch%d",i)).data=X(1:100:end);
		r.scope.(sprintf("ch%d",i)).time=1:100:length(X);
	end
	decode=toc;
	printf("Data decoded %f (%f)\n",decode,decode-deser2); fflush(stdout);

	if make_vcd
		vcd=trace_vcd(r,"tt.vcd",f);
		vcdt=toc;
		printf("VCD written  %f (%f)\n",vcdt,vcdt-decode);
		fflush(stdout);
	end
endfunction
