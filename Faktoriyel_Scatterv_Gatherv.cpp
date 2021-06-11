#include <mpi.h>
#include <iostream>
#include <malloc.h>
#include <math.h>
#include <iomanip>
#include <string.h>
using namespace std;

int YerBul(int dizi[], int index) {
	int toplam = 0;
	for (int i = 0; i < index; i++) {
		toplam += dizi[i];
	}
	return toplam;
}

int main(int argc, char* argv[]) {
	
	int myRank,
		size,
		sayi,
		ceiledValue,
		flooredValue,
		chunkSize,
		lokalCarpim=1,
		globalCarpim=1;

	int * sendBufferPtr = nullptr,
		* receiveBufferPtr = nullptr,
		* sendCountPtr=nullptr,
		* displsPtr=nullptr,
		* resultsBufferPtr=nullptr;

	MPI_Status status;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	if (myRank == 0) {
		cout << "Faktoriyeli alinacak sayiyi giriniz : ";
		cin >> sayi;

		flooredValue = (int)floor((double)sayi / (double)size);
		ceiledValue = sayi-(size-1)*flooredValue;
		
		/*
			Kullanýlan daðýtým algoritmasý <<Blok Daðýtým Tekniði II>> dir.

			=>first(p-1)(flooredValue(n/p))
			=>last(n-(p-1).x)
		*/

		sendBufferPtr = new int[sayi];
		resultsBufferPtr = new int[sayi];
		sendCountPtr = new int[size];
		displsPtr = new int[size];

		for (int i = 0; i < sayi; i++) {
			sendBufferPtr[i] = i + 1;
		}

		for (int i = 0; i < size; i++) {
			if (i < size-1) {
				sendCountPtr[i] = flooredValue;
			}
			else {
				sendCountPtr[i] = ceiledValue;
			}
		}

		for (int i = 0; i < size; i++) {
			displsPtr[i] = YerBul(sendCountPtr, i);
		}

		chunkSize = sendCountPtr[0];

		for (int i = 1; i < size; i++) {
			MPI_Send(&sendCountPtr[i], 1, MPI_INT, i, 25, MPI_COMM_WORLD);
		}

	}
	else {
		MPI_Recv(&chunkSize, 1, MPI_INT, 0, 25, MPI_COMM_WORLD, &status);
	}

	receiveBufferPtr = new int[chunkSize];
	
	MPI_Scatterv(sendBufferPtr, sendCountPtr, displsPtr, MPI_INT, receiveBufferPtr, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < chunkSize; i++) {
		lokalCarpim *= receiveBufferPtr[i];
	}
	for (int i = 0; i < chunkSize; i++) {
		if (i < chunkSize - 1) {
			receiveBufferPtr[i] = 1;
		}
		else {
			receiveBufferPtr[i] = lokalCarpim;
		}
	}

	MPI_Gatherv(receiveBufferPtr, chunkSize, MPI_INT, resultsBufferPtr, sendCountPtr, displsPtr, MPI_INT, 0, MPI_COMM_WORLD);

	if (myRank == 0) {
		for (int i = 0; i < sayi; i++) {
			globalCarpim *= resultsBufferPtr[i];
		}

		cout << sayi << " sayisinin faktoriyeli : " << globalCarpim << endl;

		delete[] resultsBufferPtr;
		delete[] sendBufferPtr;
		delete[] sendCountPtr;
		delete[] displsPtr;
	}

	delete[] receiveBufferPtr;

	MPI_Finalize();
	

	return 0;
}
