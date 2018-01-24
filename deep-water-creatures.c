// Experiment 4: pairs of eBUGs

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int max_pairs = 1000;	// max number of pairs
int max_gen = 4004;		// max number of generations
int max_energy = 0;
int prob = 400;			// 1/prob is propbability to apply mutation

int en[max_pairs];		// energy array for all pairs
int oq[max_pairs][40];	// 500 e-bug pairs;
                    	// e-bug 1: from oq[][0] - oq[][19]
                    	//	- response to the environment: oq[][0]- oq[][9];
                    	//	- deployment of food tenticles: oq[][10]-oq[][19];
                    	// e-bug 2: from oq[][20] - oq[][39]
                    	//	- response to the environment: oq[][20]- oq[][29];
                    	//	- deployment of food tenticles: oq[][30]-oq[][39];

int f, f1, f2;             // nr of food items

int total_en_this_gen;
int gen, tmp, tmp1, tmp2;

FILE  *f_out;
 
/* main function */
int main () {
	int c, i,j, g, k, l, n, s1, s2, r, r1, r2, t;

	int next_gen( ), save_state ( );

	// seed random number generator
	printf("enter seed value for the random number generator:  ");
	scanf("%d",&i);
	srandom(i); 
   
// initialize pairs at random;
for (i=0; i<500; i++)
   for (j=0; j<40; j++) oq[i][j]=rand() % 10;

// ---------- main generation loop begins --------------- 
for (gen=0; gen<max_gen; gen++)
 {
  printf("generation %d\n",gen);
  // init energy for each eBug duo
  for (i=0; i<500; i++) en[i]=0;
      
  // 2000 cycles in a single generation before the creation of the next generation
  for (c=0; c<2000; c++)
    {
     // generate the number of food items f1 above eBug1 and f2 above eBug2
     f1=(rand() % 10); f2=(rand() % 10);

     // subject every pair of eBugs to the generated environment
     for (i=0; i<500; i++)
	{
          s2=oq[i][f2];      // the 1st bug sees f2 items above bug2 and returns signal s2 
	  s1=oq[i][f1+20];   // the 2nd bug sees f1 items above bug1 and returns signal s1 
	  // generate responses
	  r1=oq[i][10+s1];         // r1= the number of food tenticles deployed by bug1
	  r2=oq[i][30+s2];         // r2= the number of food tenticles deployed by bug2
	  
	  // adjust energy after capture of food; it is assumed that a catch adds 1 energy unit and
	  // a miss subtracts 1 energy unit
	  
	  if (r1 < f1) en[i]=en[i]+r1;
	  else
	    {en[i]=en[i] + f1;            // increase energy for every food found
	     en[i]=en[i] - (r1 - f1);     // pay for every miss
	    }
	    
	  if (r2 < f2) en[i]=en[i]+r2;
	  else
	     {en[i]=en[i] + f2;   
	      en[i]=en[i] - (r2 - f2); 
	     }
	    
	} // end for (i=0; i<500
   //    printf("energy of heave 1=%d\n",en[1]);
     }  // end of 200 cycles
    if (gen < (max_gen-1)) next_gen(); 
  }  // end for (gen=0...
  
  // save the last generation:
  save_state ( );
}

// =======================  END MAIN  ============================
//

// ------------  NEXT GENERATION ----------
// compute next generation of heaves
//
int next_gen ()
{ int a, c1, i,j,l,d,e,m,n,x1,y1,s,r,v,e_max, e_min, e_min_p, t, z;
  int pairs[500][40];
  int rulette[500];
  float tp;

  // get max energy, e_max, e_min
  e_max=0; e_min=3000000; e_min_p=3000000;
  total_en_this_gen=0;
  c1=0;
 
  // adjust various counters
  for (i=0; i<500; i++)  
      {
//      total_en_this_gen = total_en_this_gen + en[i];
        if (en[i] < e_min) e_min=en[i];
	if ((en[i] < e_min_p) && (en[i]>0)) e_min_p=en[i];
        if (en[i] > e_max) e_max=en[i];
	if (en[i] > max_energy) {max_energy = en[i]; tmp1=gen; tmp2=i;}
	if (en[i]>0) c1++;  // c1=number of pairs alive
      }


printf(" MIN energy this gen=%d\n MIN_pos energy this gen=%d\n MAX energy this gen=%d\n pairs_alive=%d\n",
       e_min,e_min_p,e_max,c1);
printf(" max en so far=%d in gen=%d e-bug=%d\n",max_energy,tmp1,tmp2);

// assign rulette values to each heave and store them in rulette[] 
//   estimate portion of rulette assigned to a heave:
//   - assign 100 - offset slots minus "offset" to max and, in general, 
//     assign (e[i]/e_max)*100 - (e_min/e_max)*100
//     =((e[i] - e_min)/e_max)*100
    r=0;
    for (i=0; i<500; i++)
     {if ((en[i] < 1) || (en[i] == e_min_p)) a=1;  // if energy <0 assign just 1 slot 
      else {
            tp=((en[i] - e_min_p)/(1.0 * e_max))*100;
	    a=tp;
	    if ((tp - a) > 0.5 ) a++;
           }  
         
      r=r+a;
      rulette[i]=r;      
     }


printf(" size of rulette=%d \n------------------------\n",r);

// crossover; start populating pairs[][]; there are 250 pairs to be formed 
  for (i=0; i<250; i++)
    {
     // find parents with probability prop. to number of spots in the rulette
     x1=rand() % r;                      // slot value x1 selected
     for (j=0; j < 500; j++)         // find parent which has x1 in its rulette slots
       if (x1 < rulette[j]) {x1=j; j=600;}  // make x1 the id of the first parent
// look at Fig. 1. If x1=60, then red[3] (or x1=3) is selected as 60 belongs to roulette slot of red[3]
     // find the second parent -- y1
     do
       { y1=rand() % r;
         for (j=0; j < 500; j++)      
             if (y1 < rulette[j]) {y1=j;  j=600;}
       }
     while (x1 == y1); // make sure that parents are different
     d=2*i; e=(2*i) + 1;                              // d, e   -- children

// make cross-over with probability 0.7: O's with O's and Q's with Q's
     if ((rand() % 10) < 7)                             // if rand <7 do cross over
        {
         for (j=0; j<40; j++)                     // copy action vectors switching at random
           { if ((rand() % 2) == 0)                     // if 0, x1-->d,  y1-->e
	        {pairs[d][j]=oq[x1][j]; pairs[e][j]=oq[y1][j];}    
             else {pairs[d][j]=oq[y1][j]; pairs[e][j]=oq[x1][j];}
           }

         // randomly mutate z'th bit with small probability  1/prob 
         for (z=0; z<40; z++)
	   {if ((rand() % prob) == 20) pairs[d][z] = rand() % 10;
	    if ((rand() % prob) == 20) pairs[e][z] = rand() % 10;
	   }    
	}
     else                                       // no crossover -- just copy
       for (z=0; z<40; z++)
           {pairs[d][z]=oq[x1][z]; pairs[e][z]=oq[y1][z];}
  }

// copy pairs to heave collection
  for (i=0; i<500; i++)
    for (j=0; j<40; j++)
      oq[i][j]=pairs[i][j];
}

//-------------------------------------------------
// save state
int save_state ( )
{ int i,j;
  f_out=fopen("data_out","w"); 
  for (i=0; i<500; i++)
    for (j=0; j<40; j++) fprintf(f_out,"%d ",oq[i][j]);
}
