/*
 * PointHandler.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: nut
 */

#include "PointHandler.h"

void initializePoints(int row_col,Line *segs[],Point *pointList[]){

	int index=row_col*row_col; //Nusrat
		for(int i=0;i<index;i++){
				segs[i]->print();
				pointList[2*i]=segs[i]->b;
				pointList[2*i+1]=segs[i]->a;

		}

		/*for(int i=0;i<index*2;i++){
				pointList[i]->print();
		}*/
}
void printVisibilityOfPoints(int numOfPoints,Point *pointList[]){

	for(int i=0;i<numOfPoints;i++){
		pointList[i]->print();
		//pointList[i]->printVisible();
	}
}

Point * searchPoint(int numOfPoints,Point *pointList[],Point* p ){

	for(int i=0;i<numOfPoints;i++){
		if((pointList[i]->x == p->x) && (pointList[i]->y == p->y)){
			return pointList[i];
		}

	}
	return NULL;
}

Point * getPointById(Point *pointList[],int id){
	return pointList[id];
}
//-------------------------------------------------------------------------------
//  Distance Btw 2 Points
//-------------------------------------------------------------------------------
double distance( Point * a, Point * b )
{
	return sqrt( pow(b->x - a->x, 2.0) + pow(b->y - a->y,2.0) );
}




