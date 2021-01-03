function load_symbols(result,filename="../software/main.elf")
	# [output,text]=system(sprintf("arm-none-eabi-readelf --debug-dump -s %s/../software/main.elf |@arm/reparse >@arm/symbols",pwd));

	sprintf("@arm/parse %s/%s >@arm/symbols",pwd,filename)
	[output,text]=system(sprintf("@arm/parse %s/%s >@arm/symbols",pwd,filename));
	source "@arm/symbols"
	result.var=var;

	assignin("caller",inputname(1),result);
end
