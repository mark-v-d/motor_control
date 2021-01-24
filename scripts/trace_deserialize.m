#function result=trace_deserialize(sdata,dt,skip=0)
function result=trace_deserialize(sdata,dt,margin=0.2)
	# Falling edge maybe a start
	falling=find(diff(double(sdata(1:end-10*ceil(dt))))<0)+1;

	bitsum=[]; window=[];
	for x=1:10
		s=round((x-1)*dt);
		e=round(x*dt);
		bitsum(x,:)=sum(sdata(falling+(s:e-1)'));
		window(x,:)=e-s;
	end

	q=bitsum-window/2;
	q(1,:)=-q(1,:);
	q(2:9,:)=abs(q(2:9,:));
	q=min(q)*2/dt;

	# Validated starts
	vs=falling(find(q>margin));
	q=q(find(q>margin));
	vsi=vs;

	#
	# Remove overlapping bytes.
	#
	do
		# The bytes before the idle
		vt=find(diff(vs)>=floor(dt*9.5));
		# and after the idle
		vt=union(vt,vt+1);

		# these start are too soon after a validated start
		remove=find(diff(vs)<floor(dt*9.5))+1;
		remove=intersect(vt+1,remove);

		keep=setdiff(1:length(vs),remove);
		vs=vs(keep);
		q=q(keep);
	until(length(remove)==0);

	result.data=zeros(size(vs'));
	for x=2:9
		s=sum(sdata(round(vs+(((x-1)*dt)+(1:dt)'))));
		result.w(:,x-1)=s;
		result.data+=2^(x-2)*(s'>0.5*dt);
	end
	result.time=vs';
	result.q=q';
	result.data=uint8(result.data);
	result.dt=dt;
	result.vs=vsi;
end
