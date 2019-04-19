#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

// Global Variables
int numRows, numCols, minVal, maxVal;

class Point{
	public:
		int Xcoord;
		int Ycoord;
		int Label;
		double Distance;
	Point(int x, int y){
		Xcoord = x;
		Ycoord = y;
	}
};

class xyCoord {
	public:
		int Xcoord;
		int Ycoord;
		int Label;
	xyCoord(){
		Xcoord = 0;
		Ycoord = 0;
	}
	
	xyCoord(int x, int y){
		Xcoord = x;
		Ycoord = y;
	}
};

class Kmeans{
	public:
		int change;
		int K;
		int numPts;
		xyCoord** Kcentroids;
		int** imgAry;
		Point** pointSet;

	Kmeans(int num, string arg){
		cout << "How many clusters will you have? ";
		cin >> K;
		Kcentroids = new xyCoord*[K+1];		
		imgAry = new int*[numRows]; 
		for(int i=0; i<numRows; i++){
	   		imgAry[i] = new int[numCols];
	   		for(int j=0;j<numCols;j++)
			   imgAry[i][j] = 0;	
    	}
    	numPts = num;
    	pointSet = new Point*[numPts];	
    	loadPointSet(arg, pointSet);
	}

	void loadPointSet(string filename, Point** pointSet){
		ifstream inFile;
		inFile.open(filename.c_str());
		int x, y;
		for(int i=0;i<numPts;i++){
			inFile >> x >> y;
			pointSet[i] = new Point(x,y);
			pointSet[i]->Label = 9999;
			pointSet[i]->Distance = 999; 
		}
		inFile.close();
	}
	
	void assignLabel(Point** pointSet, int K){
		int front = 0, back = numPts - 1, label = 1;
		while(front <= back){
			if(label >= K) label = 1;
			pointSet[front]->Label = label;
			front++;
			label++;
			pointSet[back]->Label = label;
			back--;
			label++; 
		}
	}
	
	void point2Image(Point** pointSet, int** imgAry){
		int x, y, label;
		for(int i=0;i<numPts;i++){
			x = pointSet[i]->Xcoord;
			y = pointSet[i]->Ycoord;
			label = pointSet[i]->Label;
			imgAry[x][y] = label;
		}
	}
	
	void printImage(int** imgAry, ofstream& outFile){
		outFile << "****************************************************\n";
		for(int i=0;i<numRows;i++){
			for(int j=0;j<numCols;j++)
				if(imgAry[i][j] > 0) outFile << imgAry[i][j];
				else outFile << " ";
			outFile << endl;
		}
	}
	
	void computeCentroids(Point** pointSet, xyCoord** Kcentroids){
		int* sumX = new int[K+1];
		int* sumY = new int[K+1];
		int* totalPt = new int[K+1];
		for(int i=0;i<K+1;i++){
			sumX[i] = sumY[i] = totalPt[i] = 0;
			Kcentroids[i] = new xyCoord();
		}
		int label; 
		for(int index=0;index<numPts;index++){
			label = pointSet[index]->Label;
			sumX[label] += pointSet[index]->Xcoord;
			sumY[label] += pointSet[index]->Ycoord;
			totalPt[label]++;
		}
		for(label=1;label<=K;label++){
			Kcentroids[label]->Xcoord = (sumX[label]) / ((totalPt[label]>0) ? totalPt[label]:1);
			Kcentroids[label]->Ycoord = (sumY[label]) / ((totalPt[label]>0) ? totalPt[label]:1);
		}
	}
	
	double ComputeDist(Point* p, xyCoord* avg){
		double principal = (pow(p->Xcoord - avg->Xcoord, 2)) + (pow(p->Ycoord - avg->Ycoord, 2));
		return sqrt(principal);
	}
	
	void DistanceMinLabel(Point* p, xyCoord** Kcentroids){
		int minDist, minLabel, dist;
		minDist = 9999;
		minLabel = 0;
		
		for(int label=1;label<=K;label++){
			dist = ComputeDist(p, Kcentroids[label]);
			if(dist < minDist){
				minLabel = label;
				minDist = dist;
			}
		}
		if(minDist < p->Distance){
			p->Distance = minDist;
			p->Label = minLabel;
			change++;
		}
	}
	
	void writePtSet(Point** pointSet, ofstream& outFile){
		for(int i=0;i<numPts;i++)
			outFile << pointSet[i]->Xcoord << " " 
					<< pointSet[i]->Ycoord << " "
					<< pointSet[i]->Label << " " 
					<< endl; 
	}
	
	void kMeansClustering(ofstream& outFile2){
		int iteration = 0, index;
		change = 1;
		assignLabel(pointSet, K);
		while(change != 0){
			point2Image(pointSet, imgAry);
			printImage(imgAry, outFile2);
			change = 0;
			computeCentroids(pointSet, Kcentroids);
			index = 0;
			while(index < numPts){
				DistanceMinLabel(pointSet[index++], Kcentroids);
			}
			iteration++;	
		}
		cout << "The program iterated process " << iteration << " times. " << endl;
		outFile2 << "****************The Final Image*************" << "\n\n";
		point2Image(pointSet, imgAry);
		printImage(imgAry, outFile2);
	}

};


int extraxtPts(ifstream& inFile, ofstream& outFile){
	int count = 0, temp;
	for(int i=0;i<numRows;i++) 
		for(int j=0;j<numCols;j++){
			inFile>>temp;
			if(temp>0) {
				count++;
				outFile << i << " " << j << endl;
			}
		}		
	return count;		
}

int main(int argc, char** argv) {
	ifstream inFile;
	inFile.open(argv[1]);
	ofstream outFile1;
	outFile1.open(argv[2]);
	ofstream outFile2;
	outFile2.open(argv[3]);
		
	inFile >> numRows >> numCols >> minVal >> maxVal;
	int numPts = extraxtPts(inFile, outFile1);
	inFile.close();
	outFile1.close();
	
	Kmeans* k = new Kmeans(numPts, argv[2]);
	k->kMeansClustering(outFile2);
	
	outFile2.close();
	return 0;
}


