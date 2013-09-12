#include "point.h"


Point::Point()
{

}
Point::Point(double _x1, double _y1)
{
	x = _x1;
	y = _y1;
	static int id_counter = 0;
	id = id_counter++;
	index=0;
	Point();
	//std::cout << "Constructing Point x: " << x << " y: " << y << " \t ID: " << id << " index " << index  <<std::endl;

}
void Point::print()
{
	std::cout << "Point x: " << x << " y: " << y << " \t ID: " << id << std::endl;
}
double Point::value()
{
	// this is the angular distance from the base line
	// for point.cpp, we just cache the initial calculation
	return theta;
}
void Point::printVisible()
{
	for(int i=0;i<index;i++)
	std::cout << "Point (" << x << "," << y << ") \t can see Point (" << visible[i]->x << "," << visible[i]->y<<")"  << std::endl;
}
void Point::addVisible(Point *p)
{

	//std::cout << "At " << x <<"," << y <<"   for  " << p->x <<","<<p->y <<" Index "<<index<<std::endl;
	visible[index]=p;
	index++;
}
