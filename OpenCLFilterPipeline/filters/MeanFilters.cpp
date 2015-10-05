#include "MeanFilters.h"

MeanFilter_CPU::MeanFilter_CPU()
	: BaseFilter()
{
}

MeanFilter_CPU::~MeanFilter_CPU()
{
}

void MeanFilter_CPU::Apply(const Image &input, Image &output)
{
	const int radius = CFilterParameterInterpreter<int>::Convert(_parameters->GetParameter("Radius"));
	const int width = input.Width();
	const int height = input.Height();
	const int depth = input.Depth();
	const unsigned char *data = input.Data();
	unsigned char *out_data = output.Data();

	int i = 0, j = 0, ii = 0, jj = 0;
	int borneInfx = 0, borneSupx = radius + 1;

	std::vector<int> accuYMemoryR; accuYMemoryR.resize(radius + 1, 0);
	std::vector<int> compteurYMemoryR; compteurYMemoryR.resize(radius + 1, 0);
	std::vector<int> accuYMemoryG; accuYMemoryG.resize(radius + 1, 0);
	std::vector<int> compteurYMemoryG; compteurYMemoryG.resize(radius + 1, 0);
	std::vector<int> accuYMemoryB; accuYMemoryB.resize(radius + 1, 0);
	std::vector<int> compteurYMemoryB; compteurYMemoryB.resize(radius + 1, 0);

	int index = 0;

	int nbpt_R = 0;
	int nbpttemp_R = 0;
	int sommetemp_R = 0;
	int somme_R = 0;
	int compteurligneY_R = 0;
	int sommeligneY_R = 0;
	int valtemp_R = 0;

	int nbpt_G = 0;
	int nbpttemp_G = 0;
	int sommetemp_G = 0;
	int somme_G = 0;
	int compteurligneY_G = 0;
	int sommeligneY_G = 0;
	int valtemp_G = 0;

	int nbpt_B = 0;
	int nbpttemp_B = 0;
	int sommetemp_B = 0;
	int somme_B = 0;
	int compteurligneY_B = 0;
	int sommeligneY_B = 0;
	int valtemp_B = 0;


	// Initialisation
	for (jj = 0; jj < radius + 1; jj++){
		for (ii = 0; ii < radius + 1; ii++){
			index = (jj * width + ii) * depth;

			valtemp_R = int(data[index]);
			accuYMemoryR[jj] = accuYMemoryR[jj] + valtemp_R; // Somme des valeurs horizontalement
			compteurYMemoryR[jj] = compteurYMemoryR[jj] + 1;
			somme_R += valtemp_R;
			nbpt_R += 1;

			valtemp_G = int(data[index + 1]);
			accuYMemoryG[jj] = accuYMemoryG[jj] + valtemp_G; // Somme des valeurs horizontalement
			compteurYMemoryG[jj] = compteurYMemoryG[jj] + 1;
			somme_G += valtemp_G;
			nbpt_G += 1;

			valtemp_B = int(data[index + 2]);
			accuYMemoryB[jj] = accuYMemoryB[jj] + valtemp_B; // Somme des valeurs horizontalement
			compteurYMemoryB[jj] = compteurYMemoryB[jj] + 1;
			somme_B += valtemp_B;
			nbpt_B += 1;
		}
	}

	std::vector<int> compteurY_R; compteurY_R.resize(radius + 1, 0);
	std::vector<int> accuY_R; accuY_R.resize(radius + 1, 0);

	std::vector<int> compteurY_G; compteurY_G.resize(radius + 1, 0);
	std::vector<int> accuY_G; accuY_G.resize(radius + 1, 0);

	std::vector<int> compteurY_B; compteurY_B.resize(radius + 1, 0);
	std::vector<int> accuY_B; accuY_B.resize(radius + 1, 0);

	// Boucle principale
	for (i = 0; i < width; ++i){
		accuY_R = accuYMemoryR;
		compteurY_R = compteurYMemoryR;
		accuY_G = accuYMemoryG;
		compteurY_G = compteurYMemoryG;
		accuY_B = accuYMemoryB;
		compteurY_B = compteurYMemoryB;

		sommetemp_R = somme_R;
		nbpttemp_R = nbpt_R;
		sommetemp_G = somme_G;
		nbpttemp_G = nbpt_G;
		sommetemp_B = somme_B;
		nbpttemp_B = nbpt_B;

		index = 0;

		if (nbpttemp_R != 0){
			out_data[i * depth] = (unsigned char)((double)sommetemp_R / (double)nbpttemp_R);
		}
		if (nbpttemp_G != 0){
			out_data[i * depth + 1] = (unsigned char)((double)sommetemp_G / (double)nbpttemp_G);
		}
		if (nbpttemp_B != 0){
			out_data[i * depth + 2] = (unsigned char)((double)sommetemp_B / (double)nbpttemp_B);
		}

		borneInfx = (i - radius > 0) ? (i - radius) : 0; // ATTENTION
		borneSupx = (i + radius + 1 < width) ? (i + radius + 1) : width;


		for (j = 1; j < height; ++j){

			jj = j + radius;

			// Si on peut entrer une nouvelle ligne
			if (jj < height){
				for (ii = borneInfx; ii < borneSupx; ii++){
					sommeligneY_R += int(data[(jj * width + ii) * depth]);
					compteurligneY_R++;
					sommeligneY_G += int(data[(jj * width + ii) * depth + 1]);
					compteurligneY_G++;
					sommeligneY_B += int(data[(jj * width + ii) * depth + 2]);
					compteurligneY_B++;
				}
				sommetemp_R += sommeligneY_R;
				nbpttemp_R += compteurligneY_R;
				accuY_R.push_back(sommeligneY_R);
				compteurY_R.push_back(compteurligneY_R);
				sommeligneY_R = 0;
				compteurligneY_R = 0;

				sommetemp_G += sommeligneY_G;
				nbpttemp_G += compteurligneY_G;
				accuY_G.push_back(sommeligneY_G);
				compteurY_G.push_back(compteurligneY_G);
				sommeligneY_G = 0;
				compteurligneY_G = 0;

				sommetemp_B += sommeligneY_B;
				nbpttemp_B += compteurligneY_B;
				accuY_B.push_back(sommeligneY_B);
				compteurY_B.push_back(compteurligneY_B);
				sommeligneY_B = 0;
				compteurligneY_B = 0;
			}

			// Si on ne touche plus le bord
			if ((j - radius) > 0){
				sommetemp_R -= accuY_R[index];
				nbpttemp_R -= compteurY_R[index];

				sommetemp_G -= accuY_G[index];
				nbpttemp_G -= compteurY_G[index];

				sommetemp_B -= accuY_B[index];
				nbpttemp_B -= compteurY_B[index];

				index++;
			}

			if (nbpttemp_R != 0){
				out_data[(j * width + i) * depth] = (unsigned char)((double)sommetemp_R / (double)nbpttemp_R);
			}
			if (nbpttemp_G != 0){
				out_data[(j * width + i) * depth + 1] = (unsigned char)((double)sommetemp_G / (double)nbpttemp_G);
			}
			if (nbpttemp_B != 0){
				out_data[(j * width + i) * depth + 2] = (unsigned char)((double)sommetemp_B / (double)nbpttemp_B);
			}
		}


		// Reinitialisation
		accuY_R.resize(radius + 1, 0);
		compteurY_R.resize(radius + 1, 0);
		accuY_G.resize(radius + 1, 0);
		compteurY_G.resize(radius + 1, 0);
		accuY_G.resize(radius + 1, 0);
		compteurY_G.resize(radius + 1, 0);
		index = 0;

		if (i - radius >= 0){
			for (jj = 0; jj < (radius + 1) && jj < height; jj++){
				valtemp_R = int(data[(jj * width + borneInfx) * depth]);
				accuYMemoryR[jj] -= valtemp_R;// accuYMemory[jj] - valtemp;
				compteurYMemoryR[jj]--;// = compteurYMemory[jj] - 1;
				somme_R -= valtemp_R;
				nbpt_R--;

				valtemp_G = int(data[(jj * width + borneInfx) * depth + 1]);
				accuYMemoryG[jj] -= valtemp_G;// accuYMemory[jj] - valtemp;
				compteurYMemoryG[jj]--;// = compteurYMemory[jj] - 1;
				somme_G -= valtemp_G;
				nbpt_G--;

				valtemp_B = int(data[(jj * width + borneInfx) * depth + 2]);
				accuYMemoryB[jj] -= valtemp_B;// accuYMemory[jj] - valtemp;
				compteurYMemoryB[jj]--;// = compteurYMemory[jj] - 1;
				somme_B -= valtemp_B;
				nbpt_B--;
			}
		}

		if (borneSupx < width){
			for (jj = 0; jj < (radius + 1) && jj < height; jj++){
				valtemp_R = int(data[(jj * width + borneSupx)]);
				accuYMemoryR[jj] += valtemp_R;// accuYMemory[jj] + valtemp;
				compteurYMemoryR[jj]++;// = compteurYMemory[jj] + 1;
				somme_R += valtemp_R;
				nbpt_R++;

				valtemp_G = int(data[(jj * width + borneSupx)]);
				accuYMemoryG[jj] += valtemp_G;// accuYMemory[jj] + valtemp;
				compteurYMemoryG[jj]++;// = compteurYMemory[jj] + 1;
				somme_G += valtemp_G;
				nbpt_G++;

				valtemp_B = int(data[(jj * width + borneSupx)]);
				accuYMemoryB[jj] += valtemp_B;// accuYMemory[jj] + valtemp;
				compteurYMemoryB[jj]++;// = compteurYMemory[jj] + 1;
				somme_B += valtemp_B;
				nbpt_B++;
			}
		}
	}
}

void MeanFilter_CPU::BuildParameterList()
{
	_parameters->AddParameter(new CFilterInt(1, "Radius", "Radius of the kernel", 1, 9999));
}

