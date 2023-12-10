function result=accel(sz,vmax,amax,dt=1/4500)
        v=0;
        result=[0,0];
        while result(end)<sz/2 && v<vmax*dt
                v+=amax*dt*dt;
                result(end+1,:)=[result(end,1)+v, v];
        end
        accel_length=result(end,1);
        while result(end,1)+accel_length<sz
                result(end+1,:)=[result(end,1)+v,v];
        end
        while v>0 && result(end)<sz
                v-=amax*dt*dt;
                result(end+1,:)=[result(end,1)+v, v];
        end
endfunction

