#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <cmath>
#include <tuple>

class trajectory {
    float v_max=1;
    float a_max=0.13;

    float x=0, v=0, a=0;
    int na=0, nc=0, nd=0;
public:

    trajectory(float vm, float am):v_max(vm), a_max(am) {
    }

    int get_da() {
	float da=0;
	float V=0;
	for(int i=0;i<na; i++) {
	    V+=a;
	    da+=V;
	}
	return da;
    }

    void set_nc_a(float dx,float da) {
	float dc=dx-2*da; // constant velocity distance
	nc=std::ceil(dc/v_max); // constant velocity steps
	float dt=nc*v_max+2*da; // total distance
	float cf=dx/dt;               // correction factor
	a*=cf;
    }

    void set_move(float dx) {
	a=v_max/std::floor(v_max/a_max);
	na=v_max/a;
	float da=get_da();
	if(dx>2*da) {
	    set_nc_a(dx,da);
	    if(a>a_max) {
		a=v_max/std::ceil(v_max/a_max); // acceleration down
		na=v_max/a;
		da=get_da();
		set_nc_a(dx,da);
	    }
	} else {
	    float V=nc=na=da=0;
	    while(da<dx/2) {
		na++;
		V+=a;
		da+=V;
	    }
	    float cf=dx/2/da;
	    if(a*cf>a_max) {
		na++;
		V+=a;
		da+=V;
		cf=dx/2/da;
	    }
	    a*=cf;
	}
	nd=na;
    }

    auto position() {
	if(na) {
	    na--;
	    v+=a;
	    x+=v;
	} else if(nc) {
	    nc--;
	    x+=v;
	} else if(nd) {
	    nd--;
	    x+=v;
	    v-=a;
	}
	if(!nd)
	    v=0;
	return std::tuple{x,v,a};
    }
};

#endif
