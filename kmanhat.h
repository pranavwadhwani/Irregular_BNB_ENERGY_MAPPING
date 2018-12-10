///////////////////////////////////////////////////////////////////////////////////////////////
// This class describes the M_hattan_Matrix.It contains a function to read the M_hattan_Matrix.
// M_hattan _Matrix contains the Manhattan distance between each tile.
//////////////////////////////////////////////////////////////////////////////////////////////include <fstream.h>
#include<iostream>
#include<cstdlib>
#include<math.h>
//#include<conio>
#include<fstream>
#include"struct.h"
#define size1 25
//#define permitted_length 386.66
using namespace std;

class M_hattan_Complete_Graph{

private:

	floor_plan tile_info[size1];
public:

	struct M_hattan (*M_hattan_matrix)[size1];
	M_hattan_Complete_Graph();     		// Initialise the M_hattan_Matrix from the input file.

	M_hattan (*Read_m_hattan_Graph())[size1]; // Returns M_hattan_Matrix[][] (returning pointer to an array of size of M_Hattan structure)

};

///////////////////////////////////////////////////////////////////////
// Initialise the M_hattan_Matrix from input file(tile_coordinates.txt)
///////////////////////////////////////////////////////////////////////
M_hattan_Complete_Graph::M_hattan_Complete_Graph()
{
	
	if(M_hattan_matrix=new struct M_hattan[size1][size1]){} //throws exception if specified memory not available
	else{
		cout<<"\nOut of memory in allocation of manhattan matrix\n";
			exit (1);
			}

	for(int j=0;j<size1;j++)
	{
		for(int i=0;i<size1;i++)
		{
			M_hattan_matrix[j][i].length=-1.0;
			M_hattan_matrix[j][i].link=-1;
		}
	}

	//cout<<"after manhatan initialise"<<endl;
	//getch();
	for(int i=0;i<size1;i++)
	{
		tile_info[i].x=0.0;
		tile_info[i].y=0.0;

	}


	//int i=-1;
	ifstream file;
	int tile_id;
	float x;
	float y;

	file.open("tile.txt");

	if(!file.is_open()){
	  	cout<<"Error opening input file \n";
	    	//exit(1);
	      	}

	while(!file.eof()){		// Reading information from input file and adding it to structure tile_info

		//i++;
		file >> tile_id;
		file >> x;
		tile_info[tile_id].x=x;

		file >> y;
		tile_info[tile_id].y=y;

		}
	   //cout<<"after fileopwn and file reading"<<endl;
	   //getch();
	/*if(size1!=i){
		cout<<"Input error : No of tiles(size1) is not equal to no of tiles in input file(tile_coordinates.txt)";
		exit(1);
		}*/

	for(int i=0;i<size1;i++)	// Calculating M_hattan_distance between tiles ( = (x2-x1)+(y2-y1) ).
	{
		for(int j=0;j<size1;j++)
		{
			M_hattan_matrix[i][j].length=fabs(tile_info[i].x-tile_info[j].x)+fabs(tile_info[i].y-tile_info[j].y);

			if(M_hattan_matrix[i][j].length>permitted_length)	M_hattan_matrix[i][j].length=-1;


			if(i==j)	M_hattan_matrix[i][j].length=-1;
		}
	}
	//int debug=1;
       //*	cout<<"after manhatan calculation "<<endl;
	//getch();
	if(debug){
		cout<<"\nM_hattan_matrix after input from file\n";
		for( int i=0;i<size1;i++){
			cout<<"row : "<<i<<"-----"<<endl;
			for(int j=0;j<size1;j++)
			{
				cout<<M_hattan_matrix[i][j].length<<" ";
			}
			cout<<"\n";   		}
	}

	file.close();

}

///////////////////////////////////////////////////////////////////////////
// Function returning pointer to an array of size of M_Hattan structure
///////////////////////////////////////////////////////////////////////////
M_hattan (*M_hattan_Complete_Graph::Read_m_hattan_Graph())[size1]
{
	return M_hattan_matrix;
}

/*int main()
{
cout<<"inside main";
   //clrscr();
   cout<<"inmina"<<endl;
   M_hattan_Complete_Graph m1;
     //getch();
return 0;
}*/



