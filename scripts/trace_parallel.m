# function [serial vcd]=trace_parallel(B);
#
# B	input data from oscilloscope, B(:,1)=clk; B(:,2)=MSB; B(:,end)=LSB;
#
# serial	Data to be used for trace_decode
# vcd		Information about the decoding process which may be
#		written as/added to the vcd file.

#{
[scope_data f]=rigol(scope,4);
clk=bitand(scope_data',0x80)>0;
d=[	bitand(scope_data',0x40)>0, ...
	bitand(scope_data',0x20)>0, ...
	bitand(scope_data',0x10)>0, ...
	bitand(scope_data',0x08)>0, ...
];

B=[clk,d];

trace_vcd(vcd,"test.vcd",f);

#}
function [serial vcd]=trace_parallel(B)


#
# Clock recovery
#
t=toc;
periods=sum(diff(B(:,1))>0)
if 0
	# If ltfat is not installed
	[periods,v,nev]=nelder_mead_min(@(p,B) ...
		1/abs(exp(linspace(0,2i*pi*p,length(B)))*B),
		{periods,double(B(:,1))}, "isz",0.25, "maxev",100)
else
	# 10 times faster
	[periods,v,nev]=nelder_mead_min(@(p,B) ...
		1/abs(gga(B,p/(length(B)-1))),
		{periods,double(B(:,1))}, "isz",0.25, "maxev",100)
end
printf("Clock recovery %f (%f)\n",toc,toc-t);
carrier=exp(linspace(0,2i*pi*periods,length(B)));
corr=carrier*B(:,1);
corr/=abs(corr);
carrier/=corr;
carrier_phase=linspace(0,2*pi*periods,length(B))+arg(carrier(1));
if(floor(carrier_phase(1)/pi)<1)
	carrier_phase+=2*pi;
end
carrier_time=(carrier_phase-carrier_phase(1))*length(B)/(2*pi*periods);


#
# Deskew the data
#
edge_pos=diff(B)>0;
edge_pos(end+1,:)=0;
edge_pos+=shift(edge_pos,1);

edge_neg=diff(B)<0;
edge_neg(end+1,:)=0;
edge_neg+=shift(edge_neg,1);

deskew_pos=carrier*edge_pos;
deskew_neg=carrier*edge_neg;
deskew_tot=arg(deskew_pos.*deskew_neg);
deskew_tot+=pi*((deskew_tot<-3*pi/4)-(deskew_tot>3*pi/4));
deskew_tot/=2;

if 0
	sig=[2,3];
	r=12800+(1:200);
	#r=(1:100);
	t=carrier_phase(r)'-deskew_tot;
	plot( 
		t(:,1)/pi, real(carrier(r)), "x",
		t(:,[1,sig])/pi, B(r,[1,sig])+linspace(1,0,length(sig)+1)
	)
end

#
# The convert matrix uses a weighed average according to the carrier
#
result.bitstream_q=result.bitstream=[];
for x=1:size(B,2)
	convert=sparse(
		round((carrier_phase-deskew_tot(x))/pi),
		1:length(B),
		abs(real(carrier*exp(-1i*deskew_tot(x))))
	);
	result.bitstream_q(1:size(convert,1),x)=(convert*(B(:,x)-0.5));
	result.bitstream(1:size(convert,1),x)=(convert*(B(:,x)-0.5))>0;
end
x=1;
convert=sparse(
	round((carrier_phase-deskew_tot(x))/pi),
	1:length(B),
	abs(real(carrier*exp(-1i*deskew_tot(x))))
);
result.bitstream_time=(convert>0)*(
	[1,diff(round(carrier_phase/pi))].*carrier_time
)';

#
# Serialize the bits, also keep the timestamps in a vector.
#
bitstream=result.bitstream(:,end:-1:2)'(:); # Remove the clock and serialize
bitstream_time=repmat(result.bitstream_time,1,size(B,2)-1)'(:);
bitstream_q=repmat(result.bitstream_q,1,size(B,2)-1)'(:);

# find sync frames
words=floor(length(bitstream)/16)-1;
for x=0:15
	result.words=reshape(bitstream(x+(1:words*16)),[16,words]); 
	if max(sum(result.words==[ones(15,1);0]))==16
		frame_skip=x;
		break;
	end
end
result.data=2.^(0:15)*result.words;
result.data_time=bitstream_time(frame_skip+(1:16:words*16));


syncs=[(result.data(2:end)==0x7fff).*(result.data(1:end-1)==0xffff),0];


# Now remove all the halfword syncs
keep=find(result.data!=0x7fff);
data=result.data(keep);
time=result.data_time(keep);
sync=syncs(keep);

data+=0x10000*[0,sync(1:end-1)];
data=data(find(sync==0));
time=time(find(sync==0));
serial.dt=min(diff(time))/2;
while find(bitand(data,0x10000))(1)<9
	data=[0x0001,data];
	time=[time(1)-2*serial.dt;time];
end

sof=1;x=1; frame={}; unit=0;
sof_time=serial.data=serial.time=[];
do
	eof=sof+7;
	if(eof>length(data))
		break;
	end
	frame{end+1}=data(sof:eof); 
	if length(frame{end})!=8
		sof=eof+1;
		continue;
	end
	control=bitand(frame{end},1);
	aux=bitand(2.^(8:15),frame{end}(end))>0;
	bytes=frame{end}+aux.*!control;
	bytes=[bitand(bytes,255);bitand(floor(bytes/256),255)];
	control(2,:)=0;
	aux(2,:)=0;
	u=unit*ones(size(bytes));
	for c=find(control)'
		u(c+aux(c)+1:end)=floor(bytes(c)/2);
	end
	unit=u(end);
	u(end)=0;
	u.*=!control;
	# FIXME, only unit 1 is supported
	serial.data=[serial.data;bytes(find(u==1))];
	t=time(sof:eof)';
	t(2,:)=t+serial.dt;
	serial.time=[serial.time;t(find(u==1))];
	sof_time(end+1)=time(sof);

	sof=eof+1;
until(sof>length(data))

result.frame=frame;
serial.data=uint8(serial.data);

if 1
	vcd=[];
	vcd.trace.clk.data=result.bitstream(:,1)>0;
	vcd.trace.clk.time=result.bitstream_time;
	vcd.trace.d.data=uint8(2.^(size(result.bitstream,2)-2:-1:0)*...
		result.bitstream(:,2:end)');
	vcd.trace.d.bits=size(result.bitstream,2)-1;
	vcd.trace.d.time=result.bitstream_time;
	#vcd.trace.quality.data=min(abs(result.bitstream_q),[],2);
	#vcd.trace.quality.time=result.bitstream_time;

	vcd.trace.words.data=uint16(result.data);
	vcd.trace.words.time=result.data_time;

	#vcd.trace.carrier.data=real(carrier);
	#vcd.trace.carrier.time=1:length(carrier);

	vcd.trace.sync.time=result.data_time(find(syncs));
	vcd.trace.sof.time=sof_time;

	vcd.serial.data.data=uint8(serial.data);
	vcd.serial.data.time=serial.time;
end

endfunction
