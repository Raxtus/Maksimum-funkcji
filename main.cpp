#include <iostream>
#include <cstdlib>
#include <random>
#include <fstream>
#include <math.h>
#include <string>

using namespace std;

unsigned int constexpr MAX_POPULACJA = 10;
unsigned int constexpr Pk_lim = 80; // lim / 100
unsigned int constexpr Pm_lim = 20; // lim / 100
unsigned int constexpr MAX_BITS = 5;

double a, b, c, d;
ofstream chromosomsFile;
ofstream valuesFile;

int stagnation = 0;
unsigned int constexpr stagnationLIM = 20;


string printBits(unsigned int n)
{
	const int Bits = MAX_BITS;
	char tmp[Bits + 1];

	for (int i = 0; i < Bits; ++i)
	{
		tmp[Bits - i - 1] = '0' + n % 2;
		n /= 2;
	}

	tmp[Bits] = 0;
	return string(tmp);
}

class Chromosom
{
public:
	unsigned int x;
	double value;

	Chromosom()
	{
		x = rand() % (int)pow(2,MAX_BITS);
	}

	double foo() //przystosowanie
	{
		//       ax^3 +   bx^2 +  cx + d
		 this->value = a * x*x*x + b * x*x + c * x + d;
		 return this->value;
		//wiem ze nie jest ladne ale jest szybsze od wywolania pow()
	}
	void operator<<(Chromosom p)
	{
		this->x = p.x;
	}
	void operator =(Chromosom p)
	{
		this->x = p.x;
		this->value = p.value;
	}
	bool operator ==(Chromosom p)
	{
		if (this->x == p.x && this->value == p.value) return true;
		else return false;
	}
};

void Crossover(Chromosom &a, Chromosom &b)
{
	unsigned int  l;
	unsigned int buffA, buffB;
	if ((rand() % 100) < Pk_lim)
	{
		l = MAX_BITS  -(1 + rand() % MAX_BITS-2);
		buffA = a.x - ((a.x >> l) << l);
		buffB = b.x - ((b.x >> l) << l);
		a.x = a.x - buffA + buffB;
		b.x = b.x - buffB + buffA;

	}
}
int mutate(Chromosom &a)
{
	unsigned int l;
	// front ((a.x >> (l + 1)) << (l + 1))
	unsigned int buff;
	
	if ((rand() % 100) < Pm_lim)
	{
		l = rand() % MAX_BITS;
		if (a.x >> l % 2)
		{
			buff = a.x - ((a.x >> l) << l);
			a.x = (((a.x >> l) - 1) << l) + buff;
		}
		else
		{
			buff = a.x - ((a.x >> l) << l);
			a.x = (((a.x >> l) + 1) << l) + buff;
		}
		
		//a.x = (a.x - ((a.x >> (l + 1)) << (l + 1))) >> l;
		return l;
	}
	return -1;

}

int Roulte(double *lim)
{
	static double value;
	static std::random_device rd;
	static std::default_random_engine eng(rd());
	std::uniform_real_distribution<double> distr(0, lim[MAX_POPULACJA]);
	value = distr(eng);
	int i;
	for (i = MAX_POPULACJA; i >= 0; i--)
	{
		if (!(value < lim[i]))
		{
			return i;
		}
	}
	exit(-1);
	return -1;
}


int main()
{
	
	chromosomsFile.open("chromosomes.csv");	
	valuesFile.open("values.csv");

	Chromosom max;
	Chromosom LOG;
	max.x = 0;
	max.value = 0;
	Chromosom *populacja = new Chromosom [MAX_POPULACJA];
	Chromosom *Bpopulacja = new Chromosom[MAX_POPULACJA];
	double *lim = new double[MAX_POPULACJA + 1];
	lim[0] = 0;
	int i;
	int point;

	cout << "Podaj wspolczynniki(spacje pomiedzy) a b c d" << endl;
	cin >> a >> b >> c >> d;
	cout << endl;

	do
	{	
		
		for (i = 0; i < MAX_POPULACJA; i++)
		{		
			//wyznaczenie wartośći 
			if(populacja[i].foo() > max.value) max = populacja[i];
			//zapis wyników
			chromosomsFile << printBits(populacja[i].x) <<",";
			valuesFile << populacja[i].value << ",";
			//liczenie granic losowości
			lim[i + 1] = lim[i] + populacja[i].value;
			
		}
		chromosomsFile << printBits(max.x) << endl;
		valuesFile << max.value << endl;

		//selckcja
		for (i = 0; i < MAX_POPULACJA; i++)
		{
			point = Roulte(lim);
			Bpopulacja[i] = populacja[point];
		}

		//parowanie	
		for (i = 0; i < MAX_POPULACJA; i += 2)
		{
			Crossover(Bpopulacja[i], Bpopulacja[i + 1]);
		}

		//mutacja
		for (i = 0; i < MAX_POPULACJA; i++)
		{
			mutate(Bpopulacja[i]);	
		}

		// przeniesienie populacji

		for (i = 0; i < MAX_POPULACJA; i++)
		{
			populacja[i] = Bpopulacja[i];
		}

		if (LOG == max)
		{
			stagnation++;
		}
		else
		{
			LOG = max;
			stagnation = 0;
		}



	} while (stagnation <= stagnationLIM);

	chromosomsFile.close();
  valuesFile.close();

	return 0;
}
