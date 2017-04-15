function load_symbols(result)
	[output,text]=system(sprintf("arm-none-eabi-readelf --debug-dump -s %s/../xmc4400/main.elf |@arm/reparse >@arm/symbols",pwd));

	source "@arm/symbols"

	result.var=add_decoding(var);
	assignin("caller",inputname(1),result);
end

function var=add_decoding(var)
	for x=fieldnames(var)'
		name=x{1};
		if !isfield(var.(name),"type")
			continue;
		elseif isfield(var.(name).type,"structure") 
			var.(name).decode="structure";
			var.(name).type.structure= ...
				add_decoding(var.(name).type.structure);
			var.(name).decode="structure";
			continue;
		elseif isfield(var.(name).type,"enum") 
			var.(name).decode="enum";
			continue;
		elseif isfield(var.(name).type,"pointer") 
			var.(name).decode="pointer";
			continue;
		elseif isfield(var.(name).type,"union") 
			var.(name).decode="union";
			#var.(name).type.union= ...
				#add_decoding(var.(name).type.union);
			continue;
		end
		if !isfield(var.(name).type,"encoding")
			continue
		end
		encoding=sscanf(var.(name).type.encoding(1),"%d");
		switch(encoding)
		case 1 error(" DW_ATE (DW_ATE_address, 0x1)");
		case 2 error(" DW_ATE (DW_ATE_boolean, 0x2)");
		case 3 error(" DW_ATE (DW_ATE_complex_float, 0x3)");
		case 4 # DW_ATE (DW_ATE_float, 0x4)
			switch(var.(name).type.size)
			case 4 var.(name).decode="single";
			case 8 var.(name).decode="double";
			end;
		case 5 # DW_ATE (DW_ATE_signed, 0x5)
			switch(var.(name).type.size)
			case 1 var.(name).decode="int8";
			case 2 var.(name).decode="int16";
			case 4 var.(name).decode="int32";
			case 8 var.(name).decode="int64";
			end;
		case 6 # DW_ATE (DW_ATE_signed_char, 0x6)
			var.(name).decode="int8";
		case 7 # DW_ATE (DW_ATE_unsigned, 0x7)
			switch(var.(name).type.size)
			case 1 var.(name).decode="uint8";
			case 2 var.(name).decode="uint16";
			case 4 var.(name).decode="uint32";
			case 8 var.(name).decode="uint64";
			end;
		case 8 # DW_ATE (DW_ATE_unsigned_char, 0x8)
			var.(name).decode="uint8";
		case 9 error("DW_ATE (DW_ATE_imaginary_float, 0x9)");
		case 10 error("DW_ATE (DW_ATE_packed_decimal, 0xa)");
		case 11 error("DW_ATE (DW_ATE_numeric_string, 0xb)");
		case 12 error("DW_ATE (DW_ATE_edited, 0xc)");
		case 13 error("DW_ATE (DW_ATE_signed_fixed, 0xd)");
		case 14 error("DW_ATE (DW_ATE_unsigned_fixed, 0xe)");
		case 15 error("DW_ATE (DW_ATE_decimal_float, 0xf)");
		case 16 error("DW_ATE (DW_ATE_UTF, 0x10)");
		otherwise
			#error(sprintf("Unhandled encoding %d\n",encoding"));
		end
	end
endfunction
