# Make motion profile
#
# p setpoint per row.
# p(:,1)	location
# p(:,2)	speed limit (0 for don't change)
# p(:,3)	acceleration limit (0 for don't change)
#
function r=mp(p)
	L=p(1,1);
	S=0; Slimit=p(1,2);
	A=0; Alimit=p(1,3);
	r=[];
	for i=2:size(p,1)
		if p(i,1)>L
			direction=1;
		else
			direction=-1;
		end
		if p(i,2)!=0
			Slimit=p(i,2);
		end
		if p(i,3)!=0
			Alimit=p(i,3);
		end
		if Slimit<0
			p(i,:)
			Slimit
			r(end+1:end-Slimit,1)=r(end,1);
			A=0;
			S=0;
		else
			while direction*L<direction*p(i,1)
				r(end+1,:)=[L,S,A];
				# travel remaining= Alimit/2*steps^2
				# steps=S/Alimit
				remaining=direction*S^2/2/Alimit;
				if direction*(L+remaining)>direction*p(i,1)
					A=-direction*Alimit;
				elseif direction*S<Slimit
					A=direction*Alimit;
				else
					A=0;
				end
				S+=A; 
				L+=S;
			end
		end
	end
	r(end+1,:)=[L,S,A];
endfunction
