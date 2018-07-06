#include <mpi.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include "timer.h"
#include <cmath>
using namespace std;

long long calcPow(const vector<int> &myVec){
    long long sum = 0;
    double mTime = static_cast<double>(timer());
    for(unsigned int i=0; i<myVec.size(); i++)
	sum += pow(myVec[i], 2);
    
    cout << "[PROC #" << MPI::COMM_WORLD.Get_rank()  << "] Sumowanie: " << static_cast<double>(timer()) - mTime << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    return sum;
}
void funcScatter(const vector<int> &myVec, vector<int> &myVec2, int root){
    double time = static_cast<double>(timer());
    MPI_Scatter(&(myVec[0]), myVec2.size(), MPI::INT, &(myVec2[0]), myVec2.size(), MPI::INT, root, MPI_COMM_WORLD);
    cout << "[PROC #" << MPI::COMM_WORLD.Get_rank() << "] Komunikacja:  " << static_cast<double>(timer())-time << endl;
    MPI_Barrier(MPI_COMM_WORLD);
}
long long funcReduce(long long *number, int root){
    double time = static_cast<double>(timer());
    long long outNum;
    MPI::COMM_WORLD.Reduce(number, &outNum, 1, MPI::LONG, MPI::SUM, root);
    cout << "[PROC #" << MPI::COMM_WORLD.Get_rank() << "] Redukcja:  " << static_cast<double>(timer())-time << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    return outNum;
} 
int main(int argc, char** argv){
    MPI::Init(argc, argv);
    
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();
    vector<int> myVec;
    vector<int> dataForEachProc;
    int iResize;
    int iResizeMod;
    int* recvBuf = new int[size];
    int* displs = new int[size];
    if(rank == 0){
	ifstream inFile("wejscie");
	if(!inFile){
	    cerr << "[PROC #0] Nie otworzono pliku." << endl;
	    MPI::COMM_WORLD.Abort(1);
	}
	int number;

  	double  startTime = static_cast<double>(timer());

	while(inFile >> number){
		myVec.push_back(number);
	}
	if(inFile.eof()){
		cerr << "[PROC #0] Wczytano wszystkie liczby" << endl;
		inFile.close();
		cout << "[PROC #0] odczyt: " << static_cast<double>(timer())-startTime << endl;
		
	}
        else {
		cerr << "[PROC #0] Nie wczytano wszystkich liczb." << endl;
        	MPI::COMM_WORLD.Abort(2);
	}
	iResizeMod = myVec.size() % size;
	iResize = myVec.size()/size;
	for(int i=0; i<size; i++){
	    recvBuf[i] = iResize;
	}
	int j=0;
	while( iResizeMod > 0 ){
	    recvBuf[j]++;
	    j++;
	    iResizeMod--;
	    if(j == size-1)
		j=0;
	}
	int sum = 0;
	for(int i=0; i<size; i++){
	    displs[i] = sum;
	    sum += recvBuf[i];
	    cout << displs[i] << endl;
	    }
    }
    
    MPI_Scatter(recvBuf, 1, MPI::INT, &iResize, 1, MPI::INT, 0, MPI_COMM_WORLD);
    dataForEachProc.resize(iResize);
    MPI_Scatterv(&(myVec[0]), recvBuf, displs, MPI::INT, &(dataForEachProc[0]), dataForEachProc.size(), MPI::INT, 0, MPI_COMM_WORLD);
    long long mNum;
    long long llSum = calcPow(dataForEachProc);
    mNum = funcReduce(&llSum, 0);
    if(rank == 0)
	cout << "[PROC #0] Redukcja. Suma wynosi: " << mNum << endl;
    MPI::Finalize();
    delete [] recvBuf;
    delete [] displs;
}