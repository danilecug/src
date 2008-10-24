/* Beam steering */
/*
  Copyright (C) 2008 University of Texas at Austin
  Original author: Steve Cole, Stanford University, 1995
  
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

#include <float.h>
#include <math.h>
#include <rsf.h>

#include "bsteer.h"

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)


void bsteer(float ***data,          
	    int n1pad, int n2, int n3, 
	    int nslo, int nazim,  
	    float slomin, float dslo, 
	    float azmin, float dazim, 
	    float d1, float d2, float d3,
	    int **live,
	    int mode, int n1, int npad, int nlive, int sem,
	    float pmax, int lwind, int n1out, int icoord,
            float ***coord,  
            float xref, float yref,
            float ***semb)


{
    int iazim,islo,i1,i2,i3,j1,itshift,istart,istop;
    float azim,slo,p,x,y,z,tshift;
    float *beam,*pow;                                  /* array [n1pad] */

    /* loop over azimuth and apparent surface slowness */

	for (iazim = 0; iazim < nazim; iazim++) {
	azim = azmin + iazim*dazim;

	for (islo = 0; islo < nslo; islo++) {
	    slo = slomin + islo*dslo;

            /* clear arrays */
	    for (i1 = 0; i1 < n1pad; i1++) {
		beam[i1] = 0.;
		pow[i1] = 0.;
	    }
            for (i1 = 0; i1 < n1out; i1++) semb[iazim][islo][i1] = 0.;

            /* do not do this beam if it is for too big a p */
	    p = sqrt(slo*slo + azim*azim);
	    if (p <= pmax) {

		/* loop over receivers */
		for (i3 = 0; i3 < n3; i3++) {

		    for (i2 = 0; i2 < n2; i2++) {

			if (live[i2][i3] != 0) {
			    if (icoord == 0) {
				x = (i2+1-((n2+1)/2))*d2;
				y = (i3+1-((n3+1)/2))*d3;
			    } else {
				x = coord[2][i2][i3] - xref;
				y = coord[1][i2][i3] - yref;
				z = coord[3][i2][i3];
			    }
                            /* Given azimuth, slowness, and receiver location, compute */
			    /* the time shift necessary to align the current trace */
                            /* with the reference trace. */
                            /* For mode=1, 'slo' is actually py, and 'azim' is px */

			    tshift = azim*x + slo*y;
                            /* Nearest integer */
			    itshift = (tshift/d1);

                            /* loop over samples of beam */
                            /* sum shifted input into beam */

			    istart = MAX(1+itshift,1);
			    istop = MIN(n1pad+itshift,n1pad);

			    for (i1 = istart; i1 < istop+1; i1++){
				beam[i1] = beam[i1] + data[i1-itshift][i2][i3];
				pow[i1] = pow[i1] + data[i1-itshift][i2][i3]*data[i1-itshift][i2][i3];
			    }
			}
		    }
		}

                /* normalize stack or compute semblance */
		if (sem == 0) {
		    for (i1 = 0; i1 < n1out; i1++){
			for (j1 = 0; j1 < lwind; j1++){
			    semb[iazim][islo][i1] = beam[npad+i1*lwind+j1]/nlive;
  			}
		    }
		} else {
		    for (i1 = 0; i1 < n1out; i1++){
			semb[iazim][islo][i1] = beam[npad+i1-1]*beam[npad+i1-1] / (nlive * pow[npad+i1-1] + 0.000000001);
		    }
		}
	    }
	}
    }
}
 

