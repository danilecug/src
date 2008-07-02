/* 1-D finite-difference wave extrapolation */
/*
  Copyright (C) 2008 University of Texas at Austin
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <rsf.h>

int main(int argc, char* argv[]) 
{
    int nx, nt, ix, it;
    float dt, dx, w, g;
    float *old, *new, *cur, *sig, *v, *vx, *a, *b;
    sf_file inp, out, vel, grad;

    sf_init(argc,argv);
    inp = sf_input("in");
    out = sf_output("out");
    vel = sf_input("vel");   /* velocity */
    grad = sf_input("grad"); /* velocity gradient */

    if (SF_FLOAT != sf_gettype(inp)) sf_error("Need float input");
    if (!sf_histint(inp,"n1",&nx)) sf_error("No n1= in input");
    if (!sf_histfloat(inp,"d1",&dx)) sf_error("No d1= in input");
    if (!sf_histint(inp,"n2",&nt)) sf_error("No n2= in input");
    if (!sf_histfloat(inp,"d2",&dt)) sf_error("No d2= in input");

    sig = sf_floatalloc(nx);
    old = sf_floatalloc(nx);
    new = sf_floatalloc(nx);
    cur = sf_floatalloc(nx);
    v = sf_floatalloc(nx);
    vx = sf_floatalloc(nx);
    a = sf_floatalloc(nx);
    b = sf_floatalloc(nx);

    sf_floatread(v,nx,vel);
    sf_floatread(vx,nx,grad);

    sf_fileclose(vel);
    sf_fileclose(grad);

    for (ix=0; ix < nx; ix++) {
	/* dimensionless velocity */
	w = v[ix] * dt/dx;
	/* dimensionless gradient */
	g = 0.5 * vx[ix] * dt;

	a[ix] = w*w * (1.0 + g*g);
	b[ix] = g*w;

	/* initial conditions */
	cur[ix] = 0.;
	new[ix] = 0.;
    }

    free(v);
    free(vx);

    /* propagation in time */
    for (it=0; it < nt; it++) {
	sf_floatread(sig,nx,inp);

	for (ix=0; ix < nx; ix++) {
	    old[ix] = cur[ix];
	    cur[ix] = new[ix];
	}

	/* Stencil */
	new[0] = 
	    a[0]*(cur[0] - cur[1]) +
	    b[0]*(cur[0] - cur[1]);
	for (ix=1; ix < nx-1; ix++) {
	    new[ix] = 
		a[ix]*(2*cur[ix] - cur[ix-1] - cur[ix+1]) + 
		b[ix]*(            cur[ix-1] - cur[ix+1]);
	}
	new[nx-1] = 
	    a[nx-1]*(cur[nx-1] - cur[nx-2]) +
	    b[nx-1]*(cur[nx-2] - cur[nx-1]);
	
	for (ix=0; ix < nx; ix++) {
	    new[ix] = sig[ix] + 2*cur[ix] - new[ix] - old[ix];
	}

	sf_floatwrite(new,nx,out);
    }

    exit(0);
}
