#include <math.h>

#include <morphology.h>
#include <se.h>

void 
maximum(unsigned short *val, unsigned short *min){ 
  (void) val;
  (void) min;
}

void 
minimum(unsigned short *val, unsigned short *max){ 
  (void) val;
  (void) max;
}

void 
process(int s, 
	int hs, 
	pnm ims, 
	pnm imd, 
	void (*pf)(unsigned short*, unsigned short*))
{  
  (void) s;
  (void) hs;
  (void) ims;
  (void) imd;
  (void) pf;
  puts(">> morphology-mrg.c");
}
