#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <ctime>

#define chunk 3		//number of iterations per thread
#define count 100	//size of population
#define F 0.3		//random value between 0 - 1
#define CR 0.9		

using namespace std;

typedef struct {
	double 	x,
			y,
			f;
}Data;

Data population[count], populationPrim[count];

/*Function to evaluate*/
double getF(Data solution){
	return pow(solution.x,2)+pow(solution.y,2);
}

/*Create initial population*/
void createPop(Data population[]){
	#pragma omp parallel
	{
		#pragma omp for schedule(static,chunk) nowait
			for (int i = 0; i < count; ++i)
			{
				population[i].x = rand()%1000;
				population[i].y = rand()%1000;
				population[i].f = getF(population[i]);
			}
	}
}

double H(double Ir1, double Ir2, double Ir3){
	return Ir1 + F * (Ir2 - Ir3);
}

void printPop(Data population[]){
	for (int i = 0; i < count; ++i)
	{
		cout<<"|"<<population[i].x<<"|"<<population[i].y;
		cout<<"|"<<population[i].f<<"|\n";
	}
}

bool acompare(Data a, Data b){
	return a.f < b.f;
}

//Max num of generations:
//200

int main(){
	srand(time(NULL));
	int iri[3];
	int gen_i;
	gen_i = 200;
	createPop(population);
	
	//Main loop structure
	while(gen_i--){
		#pragma omp parallel private(iri) shared(population, populationPrim)
		{
			#pragma omp for schedule(static, chunk) nowait
			for (int i = 0; i < count; ++i){
				Data newIn;
				//Selecting Ir1, Ir2 & Ir3 to make new element
				//Ir1!=Ir2!=Ir3!=i
				do{
					iri[0] = rand()%100;
				}while(iri[0] == i);
				do{ 
					iri[1] = rand()%100; 
				}while(iri[0] == iri[1] || iri[1] == i);
				do{ 
					iri[2] = rand()%100; 
				}while(iri[0] == iri[2] || iri[1] == iri[2] || iri[2] == i);
				//For x
				if(((double) rand()/RAND_MAX) <= CR || rand()%100 == i)
					newIn.x = H(population[iri[0]].x,population[iri[1]].x,
						population[iri[2]].x);
				else
					newIn.x = population[i].x;
				//For y
				if(((double) rand()/RAND_MAX) <= CR || rand()%100 == i)
					newIn.y = H(population[iri[0]].y,population[iri[1]].y,
						population[iri[2]].y);
				else
					newIn.y = population[i].y;
				//Getting value of f for the new individual
				newIn.f = getF(newIn);
				if(newIn.f < population[i].f)
					memcpy(&populationPrim[i], &newIn, sizeof(newIn));
				else
					memcpy(&populationPrim[i], &population[i], sizeof(population[i]));
			}
			#pragma omp barrier
		}
		//Save best of generation and add to history
		//Copy popolationPrim to population
		memcpy(population, populationPrim, sizeof(populationPrim));
	}
	sort(population, population + count, acompare);

	printf("x = %0.30lf\ny = %0.30lf\nf(x,y) = %0.30lf",
		population[count-1].x,population[count-1].y,population[count-1].f);
	
	return 0;
}
