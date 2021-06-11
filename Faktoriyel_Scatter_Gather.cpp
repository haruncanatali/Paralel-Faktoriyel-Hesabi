#include "mpi.h"
#include <iostream>
#include <malloc.h>

using namespace std;

int main(int argc, char* argv[]) {

    int myRank, size, sayi, chunkSize, faktoriyel = 1, lokalFaktoriyel = 1;

    int* sendBuffer=nullptr,
        * receiveBuffer= nullptr,
        * results= nullptr;

    double baslangic = 0.0, bitis = 0.0;

    MPI_Init(&argc, &argv);

    baslangic = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if (myRank == 0) {
        cout << "Faktoriyeli alinacak sayiyi giriniz : ";
        cin >> sayi;

        if (sayi % size != 0) {
            cout << "Herbir islemci esit yuk paylasmiyor!" << endl;
            MPI_Abort(MPI_COMM_WORLD, 99);
        }
        else {
            chunkSize = sayi / size;
            results = new int[size];
            sendBuffer = new int[sayi];

            for (int i = 0; i < sayi; i++) {
                sendBuffer[i] = (i + 1);
            }
        }
    }

    MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    receiveBuffer = new int[chunkSize];
    MPI_Scatter(sendBuffer, chunkSize, MPI_INT, receiveBuffer, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunkSize; i++) {
        lokalFaktoriyel *= receiveBuffer[i];
    }
    MPI_Gather(&lokalFaktoriyel, 1, MPI_INT, results, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (myRank == 0) {
        for (int i = 0; i < size; i++) {
            faktoriyel *= results[i];
        }

        bitis = MPI_Wtime();

        cout << "Hesaplanan Faktoriyel Degeri : " << faktoriyel << endl;
        cout << "Hesaplama icin gecen toplam sure : " << bitis - baslangic << endl;

        delete[] sendBuffer;
        delete[] results;
    }

    delete[] receiveBuffer;

    MPI_Finalize();

    return 0;
}
