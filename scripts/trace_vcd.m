#
# function vcd=trace_vcd(r,vcd_name,format)
#
# r		result from trace_decode
# vcd_name	Filename of VCD file to create
# format	aquisition details from dlm2000(...)
#
function vcd=trace_vcd(r,vcd_name,format)
	global vcd;

	vcd.header={};
	vcd.time=[];
	vcd.data={};

	legend=1;
	[vcd legend]=recurse_struct(r,"",legend);

	[vcd.time idx]=sort(vcd.time);
	vcd.data={vcd.data{idx}};
	save vcd vcd

	f=fopen(vcd_name,"w");
	fprintf(f,"$timescale %d ps $end\n",1e12/format.srate);
	fprintf(f,"%s",vcd.header{:});
	fputs(f,"$enddefinitions $end\n");
	x={num2cell(vcd.time)'{:};vcd.data'{:}};
	#num2str(vcd.time')
	fprintf(f,"#%d\n%s\n",x{:});
	fclose(f);
endfunction

function [vcd legend]=recurse_struct(r,scope,legend)
	global vcd;
	if isfield(r,"time")
		vcd.time(end+1:end+length(r.time))=r.time;
		V=ones(length(r.time),1);
		leg_name=vcd_leg(++legend);
	end

	if isfield(r,"time") && isfield(r,"data")
		switch typeinfo(r.data)
		case {"bool matrix" "bool"} bits=1;
		case {"uint8 matrix" "uint8 scalar"} bits=8;
		case {"uint16 matrix" "uint16 scalar"} bits=16;
		case {"uint32 matrix" "uint32 scalar"} bits=32;
		case {"float matrix" "float scalar" "matrix"} bits=0;
		otherwise
			type=typeinfo(r.data)
			scope=scope
		end

		if bits==1
			vcd.header{end+1}=sprintf("$var wire %d %s %s $end\n",
				bits,leg_name,scope);
			d=dec2bin(r.data,1);
			for x=1:length(leg_name)
				d(:,end+1)=leg_name(x);
			end
		elseif bits
			vcd.header{end+1}=sprintf("$var wire %d %s %s $end\n",
				bits,leg_name,scope);
			d=[char(V*'b'), dec2bin(r.data,1), ...
				char(V*[" " leg_name])];
		else
			vcd.header{end+1}=sprintf("$var real 1 %s %s $end\n",
				leg_name,scope);
			LN=regexprep(leg_name,"%","%%");
			d=sprintf(["r%+.16e " LN "\n"],r.data);
			if sum(find(isnan(r.data)))
				d=strrep(d,"NaN","NaN                   ");
			end
			if sum(find(isinf(r.data)))
				d=strrep(d,"Inf","Inf                   ");
			end
			d=reshape(d,[length(d)/length(r.data),length(r.data)])';
		end
		vcd.data={vcd.data{:},mat2cell(d,V,size(d,2)){:}};
	elseif isfield(r,"time")
		vcd.header{end+1}=sprintf("$var event 1 %s %s $end\n",
			leg_name,scope);
		d=char(V*['1', leg_name, "\n"]);
		vcd.data={vcd.data{:}, mat2cell(d,V,size(d,2)){:}};
	elseif isstruct(r)
		if length(scope)
			vcd.header{end+1}=sprintf("$scope module %s $end\n",
				scope);
		end
		for name=fieldnames(r)'
			[vcd legend]=recurse_struct(
				r.(name{1}), name{1}, legend);
		end
		if length(scope)
			vcd.header{end+1}="$upscope $end\n";
		end
	end
endfunction

function result=vcd_leg(legend)
	while legend
		result(end+1)=char(mod(legend,93)+33);
		if result(end)>='\'
			result(end)+=1;
		end
		legend=floor(legend/93);
	end
endfunction
