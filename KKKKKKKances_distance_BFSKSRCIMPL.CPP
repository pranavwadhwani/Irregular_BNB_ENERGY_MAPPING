
#include<iostream>
#include<math.h>
#include<fstream>
#include"kconfig.h"
#include"string.h"
#include"kmanhat.h"
//#include"struct.h"
//#include"kapplic.h"
#include<stdlib.h>
//#include<conio>

//////////////////////////////////////////////////////////////
/////////////global Declaration used for UBC and LBC> calculation/////////////////////////////////
//////////////////////////////////////////////////////////////

/*///////////strucutre: tile --->   to store noparent, level and order and ancestor (could b more than one ) list of the tile after spanning tree creation*/

struct Parent{ short ancestor ; float distance ;};

 struct Tile
{  short nofparent;
   struct Parent  parent[PV][size1];
   short order;   
   short level;
} ; 

struct Tile *tile;
 M_hattan_Complete_Graph MHG;

int (*BW_Claim_matrix)[size1+1];		//Application characterstic matrix
float (*Tree_Rep_Matrix)[size1+3];
short (*parent_info)[size1+1];
	short root;	 

short (*escape_path_array)[size1+3];  //global but not as a member of class (Implemenataion) as each object copy is not needed :)

struct port_info (*utemp_allocated_port_info);

short (*utemp_port_allocated_matrix)[size1];
int (*utemp_b_claim_array)[4];
int (*utemp_bsum_claim_array)[3];
int escapeinfo[6] ; //0-type of escape 1for direct, 2if src is in ancestor list of dest, 3if dest is in ancestor list of src, 4 if comon parent

float gubcost=9999;
float gcost=9999;
short nofsrc_traffic=0;
short noftraffic=0;
short pqsize=200;
int gubcost_stage=0;
float minlbc=9999;
int nofnodes=0;
short directpath=0;
int escape_count=0;

void allocate_memory()
{
	if(utemp_allocated_port_info=new struct port_info[size1]){} //throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global utemp_allocated_port_info\n";
			exit (1);
			}

	if(utemp_port_allocated_matrix= new short[size1][size1]){} //throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global utemp_port_allocated_matrix\n";
			exit (1);
			}


	if(utemp_b_claim_array=new int[noftraffic][4]){} //throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global utemp_B_Claim_arrayrray\n";
			exit (1);
			}

	if(utemp_bsum_claim_array=new int[nofsrc_traffic][3]){} //throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global utemp_Bsum_Claim_arrayrray\n";
			exit (1);
			}
	if(Tree_Rep_Matrix=new float[size1][size1+3]){}//throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global TREE REP MATRIX\n";
			exit (1);
			}

	if(parent_info=new short[size1][size1+1]){}//throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global parent info: spanning tree\n";
			exit (1);
			}
	if(tile=new struct Tile[size1]){} //throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global tile (Info of order,level,ancestor,nofparetns\n";
			exit (1);
			}			 		 
	if(escape_path_array=new short[size1*size1][size1+3]){}//throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global escape path array\n";
			exit (1);
			}
        


}
/*int BW_Claim_matrix[size1][size1+1];		//Application characterstic matrix
struct port_info utemp_allocated_port_info[size1];
short utemp_port_allocated_matrix[size1][size1];
int utemp_b_claim_array[130][4];
int utemp_bsum_claim_array[size1][3];*/

////////////////////////////////////////////////////////////////////
///////////////////////kApplication Class////////////////////
///////////////////////////////////////////////////le////////////
class Application_Characteristic_Graph {


private:



public:
	
	int (*B_Claim_array)[4];	//Contains the bandwidth requirement of different traffic characteristics in decreasing order of weight.
	int (*Bsum_claim_array)[3]; // to store sum of trffic src wise
	int B_Claim_size;
	Application_Characteristic_Graph();      // Initialise the BW_Claim_matrix from the input file.

	void Create_B_Claim_array();             	//create B_claim_array from BW_claim_matrix

	int (*Read_Application_Graph())[4];		//Returns B_Claim_array[m][3]
	int (*Read_Application_Graphsum())[3];		//Returns B_Claim_array[m][3]

	int  (*Read_BW_Claim_matrix())[size1+1];		// Returns BW_Claim_matrix[][]

	int return_B_Claim_size();


	//void print_mapped_B_Claim_array();
};

////////////////////////////////////////////////////////////////////
// Initialise the BW_Claim_matrix from the input file(bandwidth.txt)
////////////////////////////////////////////////////////////////////
Application_Characteristic_Graph::Application_Characteristic_Graph()   // Initialise the BW_Claim_matrix from the input file.
{

	int i,j;

	//countr for number of tiles trffic routed
			//cout<<"Noof traffic value:"<<noftraffic<<endl;
			if(B_Claim_array=new int[noftraffic][4]){} //throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of B_Claim_arrayrray\n";
			exit (1);
			}


			if(Bsum_claim_array=new int[nofsrc_traffic][3])	{} //throws exception if specified memory not available
		else{
			cout<<"\nOut of memory in allocation of Bsum_claim_array\n";
			exit (1);
		    }



	for(i=0;i<noftraffic;i++){		// Initialise B_Claim_array[][] to -1
		for(j=0;j<4;j++)
			B_Claim_array[i][j]=-1;
		}

	//initialise Bsum_claim_array
	for(i=0;i<nofsrc_traffic;i++)
	 {
	 Bsum_claim_array[i][0]=-1;
	 Bsum_claim_array[i][1]=0;
	 Bsum_claim_array[i][2]=0;}




	//cout<<"after BW claim matrix and before Create_B_Claim_array"<<endl;
	Create_B_Claim_array();


       //	getch();
}


//////////////////////////////////////////////////////////////////////
//Function returning pointer to an array of 3 integers (B_Claim_array)
//////////////////////////////////////////////////////////////////////

int (*Application_Characteristic_Graph::Read_Application_Graph())[4]
{
	return B_Claim_array;
}

int (*Application_Characteristic_Graph::Read_Application_Graphsum())[3]
{

	return Bsum_claim_array;
}


/////////////////////////////////////////////////
//create B_claim_array from BW_claim_matrix
/////////////////////////////////////////////////

void Application_Characteristic_Graph::Create_B_Claim_array()
{
		//creating B_Claim_array
		int BW;
		short source,desti;
		int i,j,k=0;
		for(i=0;i<size1;i++)
		{

				for(j=0;j<size1;j++)
				{
				if((i!=j) && (BW_Claim_matrix[i][j] !=-1))
				{
					B_Claim_array[k][0]=i;

					B_Claim_array[k][1]=j;
					B_Claim_array[k][2]=BW_Claim_matrix[i][j];
					k++;
				}
				}

		}

		B_Claim_size = k; //---> phase_2 --- size of B_Claim array
		//sorting in decreasing order using bubble sort, It gives us the chance to choose the best possible shortest _ for the most heavily communicating

		cout<<"B claim size ---------------------"<<B_Claim_size<<endl;
		for(i=0;i<(B_Claim_size);i++)
		{
			for(j=0;j<(B_Claim_size-i);j++)
			{
				if(B_Claim_array[j+1][2]>B_Claim_array[j][2])
				{
					BW=B_Claim_array[j][2];
					B_Claim_array[j][2]=B_Claim_array[j+1][2];
					B_Claim_array[j+1][2]=BW;

					desti=B_Claim_array[j][1];
					B_Claim_array[j][1]=B_Claim_array[j+1][1];
					B_Claim_array[j+1][1]=desti;

					source=B_Claim_array[j][0];
					B_Claim_array[j][0]=B_Claim_array[j+1][0];
					B_Claim_array[j+1][0]=source;
				}
			}
		}


	      //create Bsum_claim_array
	      int cnt=0;
	      for(i=0;i<size1;i++)
	      {
		if(BW_Claim_matrix[i][size1]!=0)
		{
		   Bsum_claim_array[cnt][0]=i;
		 //if(i==12) {cout<<"at 12 :"<<BW_Claim_matrix[i][size1];getch();}
		   Bsum_claim_array[cnt][1]=BW_Claim_matrix[i][size1];
		   cnt++;
	      }
	      }
	      // now sort the Bsum_claim_array


		for(i=0;i<nofsrc_traffic;i++)
		{
			for(j=0;j<(nofsrc_traffic-i-1);j++)
			{
				if(Bsum_claim_array[j+1][1]>Bsum_claim_array[j][1])
				{
					BW=Bsum_claim_array[j][1];
					Bsum_claim_array[j][1]=Bsum_claim_array[j+1][1];
					Bsum_claim_array[j+1][1]=BW;



					source=Bsum_claim_array[j][0];
					Bsum_claim_array[j][0]=Bsum_claim_array[j+1][0];
					Bsum_claim_array[j+1][0]=source;
				}
			}

		 }
    
        //outpur bsum n blaim
        cout<<"*************B clsim array ************"<<endl;
       for(i=0 ;i< noftraffic ;i++)
          {for(j=0;j<4;j++)
		cout<<B_Claim_array[i][j]<<":";
          cout<<endl;}
      cout<<"************bsum ***************"<<endl;
	for(i=0 ;i< nofsrc_traffic ;i++)
          {for(j=0;j<3;j++)
		cout<<Bsum_claim_array[i][j]<<":";	
          cout<<endl;}
	return;
}


//////////////////////////////////////////////////////////////////////
// Function returning pointer to an array of size (size1+1)
//////////////////////////////////////////////////////////////////////

int  (*Application_Characteristic_Graph::Read_BW_Claim_matrix())[size1+1]
{
	return BW_Claim_matrix;
}

int Application_Characteristic_Graph::return_B_Claim_size(){
     return B_Claim_size;
}



/*////8void Application_Characteristic_Graph::print_mapped_B_Claim_array() {
	ofstream file;
	file.open("Output/DS/load_mapped_sorted.txt");

	for(int i=0;i<B_Claim_size;i++)
		file<<B_Claim_array[i][0]<<" "<<B_Claim_array[i][1]<<" "<<B_Claim_array[i][2]<<"\n";

	file.close();
} */


void create_BW_matrix()
{
	int i,j;

	for(i=0;i<size1;i++){		// Initialise BW_Claim_matrix[][] to -1
		for(j=0;j<=size1;j++)
			BW_Claim_matrix[i][j]=-1;
		}

	ifstream file;

	file.open("bandwit.txt");

	if(file==NULL)
	{
		cout<<"Error opening input file \n";
		exit(1);
	}
	short presrc,predest;
	while(!file.eof()){	// Reading information from input file and adding it to BW_Claim_matrix[][]

		noftraffic++;
		short source; file >> source;
		short dest; file >> dest;
		int BW; file >>BW ;
		if(noftraffic >1)
                   if(presrc==source && predest==dest) { cout<<"*****"<<--noftraffic<<endl;		}
		cout<<"source : desrt : BW::"<<source<<":"<<dest<<":"<<BW<<endl;
		BW_Claim_matrix[source][dest]=BW;
		//if(noftraffic == 1)
		 presrc=source, predest=dest;
                
		
		}
	//getch();
	// Sum of the row elements signifying the bandwidth claimed by this(row id) tile.

	cout<<"Noftraffic in BW function ::"<<noftraffic<<endl;

	for( i=0;i<size1;i++)
	{
		BW_Claim_matrix[i][size1]=0;

		for( j=0;j<size1;j++)
		{
			if(BW_Claim_matrix[i][j]!=-1)
				BW_Claim_matrix[i][size1]=BW_Claim_matrix[i][size1]+BW_Claim_matrix[i][j];
		}

		// to count Noof non zero traffic bandwidth at src tiles
		if(BW_Claim_matrix[i][size1]!=0)nofsrc_traffic++;
	}

	file.close();
	

}//fn
/////////////////////////////end of application///////
//////////////////////////////////////////////////////////////////////
/////////////////class implementation Graph////////////
/////////////////////////////////////////////////////////////////////

struct gdata
{
     float cost; int count;int flag;
     struct port_info gallocated_port_info[size1];
     short gport_allocated_matrix[size1][size1];
     short gpath_array[size1*size1][size1];
} gd  , temp;

void initialise_global_data()
{
    gd.cost=9999 ; gd.count=0; temp.count=0;gd.flag=0;
    temp.cost=9999 ; 
    for(int i=0;i< size1; i++)
    {
      for(int j=0;j< size1; j++)
       { gd.gport_allocated_matrix[i][j]=-1;
         temp.gport_allocated_matrix[i][j]=-1;
	}
    }
    
    for(int i=0;i<(size1*size1);i++)
    {
       for(int j=0;j<size1;j++)
       {
         gd.gpath_array[i][j]=-1;
         temp.gpath_array[i][j]=-1;
       }
     }
 

}// initialise

void copy_global_data()
{
	//copy temp to gd
	gd.flag=1;
	for(int i=0;i<size1; i++)
    	{
   	   for(int j=0;j< size1; j++)
       	   { gd.gport_allocated_matrix[i][j]=temp.gport_allocated_matrix[i][j];
   	   }
        }
	
       for(int i=0;i<size1;i++)
	{
		gd.gallocated_port_info[i].n_i_port=temp.gallocated_port_info[i].n_i_port;
		gd.gallocated_port_info[i].n_o_port=temp.gallocated_port_info[i].n_o_port;
		
	}	

}
   	 


typedef class Implementation_Graph{


private:


	
	float cost,lbc,ubc;
	 //short path_array[size1][size1+3];
	 //short(* shortest_path_array)[size1+2];
	 struct port_info allocated_port_info[size1];

	 // number of ports per tile
	 short (*port_allocated_matrix)[size1];



public:
	int illegal,count,stage;
	short (*path_array)[size1+3];//this shuld be changed accoding to nof traffic characteristic
	Application_Characteristic_Graph ACG;
//	M_hattan_Complete_Graph MHG;
	Implementation_Graph *next;
	Implementation_Graph();
	Implementation_Graph(int src);
	Implementation_Graph(Implementation_Graph *parent,int src);
	friend void bbmapping();
	friend class PQueue;
	void upperbound();
	void lowerbound();
	void upper_dig_shortest_path(short source);
	void lower_dig_shortest_path(short source);
	void Dig_shortest_path (short source);
	void Dijkstra(short source, short desti, int R_BW,short prev[size1]);
	void upperDijkstra(short source, short desti, int R_BW,short prev[size1]);
	void lowerDijkstra(short source, short desti, int R_BW,short prev[size1]);
	void deallocate_memory();
	int bestuperbound();
	void Create_Minimum_Spanning_tree3();
	void updownroute(short,short);
	void print();
	void write_escape_path(short,short);
        void create_routing_files();
	void create_topology_files();
} implementation_graph , *pimplementation_graph;



////////////////////////////////////////////////////////////////////////////////////////
//constructor of the class Implementation_Graph(initializing data members of the class)
//Nodes are created as in the orderof Bsum array ,that is higher trafic tiles are routed first
//and calculates the Upper bound cost and lower bound cost
////////////////////////////////////////////////////////////////////////////////////////
Implementation_Graph::Implementation_Graph(int node)
{
	int i,j,x;


		//cout<<"constructor"<<endl;
		//getch();
		count=0;
		cost=lbc=ubc=0.0;
		illegal=0;
		next=NULL;
		for(i=0;i<size1;i++)
		{
			allocated_port_info[i].n_i_port=0;
			allocated_port_info[i].n_o_port=0;
		}
		
		if (port_allocated_matrix=new short[size1][size1])	{} //throws exception if specified memory not available
	     	else{
	       		cout<<"\nOut of memory in allocation of port allocated matrixath_array\n";
		 	exit (1);
		   	}
		for(j=0;j<size1;j++)
		{
			for(i=0;i<size1;i++)
			{
				port_allocated_matrix[j][i]=-1;
			}
		}

	   	if (path_array=new short[noftraffic][size1+3])	{} //throws exception if specified memory not available
	     	else{
	       		cout<<"\nOut of memory in allocation of path_array\n";
		 	exit (1);
		   	}


	//initialise path array
	for(j=0;j<noftraffic;j++)
	{	for( x=0;x<(size1+3);x++)
		path_array[j][x]=-1;
	}

		//for(i=0;i<size1;i++)
		  //if(ACG.Bsum_claim_array[node][1]==0)
		      //illegal=1;

	       if(!illegal)
	       {
		Dig_shortest_path(ACG.Bsum_claim_array[node][0]);

		//getch();
		ACG.Bsum_claim_array[node][2]=1;

		//if(node==8)
		  //for(i=0;i<size1;i++)
		    // cout<<ACG.Bsum_claim_array[i][0]<<":"<<ACG.Bsum_claim_array[i][1]<<endl;
		

		lowerbound();
		upperbound();		
		ubc+=cost;
		lbc+=cost;
		stage=1;
		//if(gubcost > ubc){ gubcost=ubc;}

		if(lbc> gubcost) {illegal=1;}
		if(lbc>ubc) illegal=1;

		cout<<"first level global UBC "<< gubcost<<endl;		
		cout<<"cost of SRC:"<<ACG.Bsum_claim_array[node][0]<<"::"<<cost<<endl;
		cout<<"cost of upper SRC:"<<ACG.Bsum_claim_array[node][0]<<"::"<<ubc<<endl;
		cout<<"cost of lower SRC:"<<ACG.Bsum_claim_array[node][0]<<"::"<<lbc<<endl;
		//getch();*/
		return ;
	       }
	       else
	       return ;

}

/////////////////implementatio
//n graph constructor for parent n tile trfic to be routed

Implementation_Graph::Implementation_Graph(Implementation_Graph *parent,int src)
{

	 int i,j;


	//cout<<"sate 2 constrcutor"<<endl;
	for(i=0;i<noftraffic;i++)
	  for(j=0;j<4;j++)
	      ACG.B_Claim_array[i][j]=parent->ACG.B_Claim_array[i][j];

	//cout<<"noftraffic:"<<noftraffic<<endl;
	//for(i=0;i<noftraffic;i++)
	//{  for(j=0;j<4;j++)
	  //    cout<<ACG.B_Claim_array[i][j]<<":";
	    //  cout<<endl;
	      //getch();
	   //}
     	for(i=0;i<nofsrc_traffic;i++)
	 for(j=0;j<3;j++)
	    ACG.Bsum_claim_array[i][j]=parent->ACG.Bsum_claim_array[i][j];

	if (port_allocated_matrix=new short[size1][size1])	{} //throws exception if specified memory not available
	     	else{
	       		cout<<"\n not first level. Out of memory in allocation of port allocated matrixath_array\n";
		 	exit (1);
		   	}	
	//debug
	//cout<<"noofsrc_traffic"<<endl;
	//for(i=0;i<nofsrc_traffic;i++)
	//{ for(j=0;j<3;j++)
	  //  cout<<ACG.Bsum_claim_array[i][j]<<":";
	  //cout<<endl;
	  //getch();
	 // }

	ACG.B_Claim_size=parent->ACG.B_Claim_size;
	//cout<<endl<<ACG.B_Claim_size<<endl;
	//cout<<"before Path array2"<<endl;

	if (path_array=new short[noftraffic][size1+3])	{} //throws exception if specified memory not available
	     	else{
	       		cout<<"\nOut of memory in allocation of path_array\n";
		 	exit (1);
		   	}
	for(int j=0;j<noftraffic;j++)
	{	for(int x=0;x<(size1+3);x++)
		path_array[j][x]=-1;
	}


	for(i=0;i<noftraffic;i++)
	{ for(j=0;j<(size1+3); j++)
	   path_array[i][j]=parent->path_array[i][j] ;
	}
       	for(i=0;i<size1;i++)
	  {allocated_port_info[i].n_i_port = parent->allocated_port_info[i].n_i_port;
	   allocated_port_info[i].n_o_port = parent->allocated_port_info[i].n_o_port;}

	  //cout<<"allocated port info"<<endl;
	 //for(i=0;i<size1;i++)
	  //cout<<parent->allocated_port_info[i].n_i_port <<":"<< parent->allocated_port_info[i].n_o_port<<endl;

	   for(i=0;i<size1;i++)
	    for(j=0;j<size1;j++)
	      port_allocated_matrix[i][j]=parent->port_allocated_matrix[i][j];

	cost=parent->cost;
	lbc=0.0;
	ubc=0.0;
	illegal=parent->illegal;
	count=parent->count;
	stage=parent->stage;

	     if(!illegal)
	       {
		Dig_shortest_path(ACG.Bsum_claim_array[src][0]);

		//getch();
		ACG.Bsum_claim_array[src][2]=1;

		   
		//cout<<" if 2 ARG CONS"<<endl;
		
		lowerbound();
                lbc+=cost;
		upperbound();
		ubc+=cost;	
                stage++;
		
//cout<<"cost of child:"<<cost<<":"<<ubc<<":"<<lbc<<endl;
		//cout<<"parent:"<<parent->cost<<":"<<parent->ubc<<":"<<parent->lbc<<endl;
		//if(gubcost > ubc){ gubcost=ubc;}
		//cout<<"before ifs child Illegal value :::"<<illegal<<endl;	
		//if(lbc >= gubcost) {illegal=1;}                		
		//cout<<"aflbc > gubcost child Illegal value :::"<<illegal<<endl;	
		//if(lbc >= ubc) illegal=1;
		cout<<" global UBC "<< gubcost<<endl;		
		cout<<"stage of child :"<<"::"<<stage<<endl;
		cout<<"cost of child SRC:"<<"::"<<cost<<endl;
		cout<<"cost of upper SRC:"<<"::"<<ubc<<endl;
		cout<<"cost of lower SRC:"<<"::"<<lbc<<endl;
		cout<<"child parent Illegal value :::"<<illegal<<endl;	
		return	;

	       }
	      else
	       return ;
	      
	       

}

///////////////////////BEST UPER BOUND///////////////////////////
int Implementation_Graph :: bestuperbound()
{
    for(int i=0;i<nofsrc_traffic;i++)
     if(ACG.Bsum_claim_array[i][2] == 0) return i;
}

/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
///// function Dijkstra for find the route of all traffic characteristic of only the src passed as argument
///////////////////////////////////////////////////////////////////////////////////////////////////

void Implementation_Graph::Dijkstra(short source, short desti,int R_BW,short prev[size1])

{
	//cout<<"in dijkstra src :"<<source<<"\t destination:"<<desti<<"\tBW:"<<R_BW<<endl;
	short n=size1;
	float dist[size1];

     //stores min dis of all tiles frm source
	short Q[size1];

     //set of all nodes
	int src=source;
	short i,j, test;
	float min,min_dist;

     //represents min distance from a node

		
	struct port_info   temp_allocated_port_info[size1];


	directpath=0;
	short temp_port_allocated_matrix[size1][size1];

	//if block to check if direct link can be laid between src and destination, inner if to check if already link exist between src and destination if not check whether laying a link violated the port constraint

	if(MHG.M_hattan_matrix[source][desti].length != -1)
	{  
		if(port_allocated_matrix[source][desti] != 1)
		{
			if(allocated_port_info[source].n_i_port+1 < PV && allocated_port_info[source].n_o_port+1 < PV && 				allocated_port_info[desti].n_i_port+1 < PV && allocated_port_info[desti].n_o_port+1 < PV)
			{
				directpath=1;
			}//if
		}//port alloated
	
		else directpath=2;
 	}//MHG


	if(!directpath)
	{	
	for(i=0;i<size1;i++)
	{
		temp_allocated_port_info[i].n_i_port=allocated_port_info[i].n_i_port;//######

		temp_allocated_port_info[i].n_o_port=allocated_port_info[i].n_o_port;//####
	}


	for(i=0;i<size1;i++)
		for(j=0;j<size1;j++)
		temp_port_allocated_matrix[i][j] = port_allocated_matrix[i][j];



	for(i=0;i<n;i++)
	{
		prev[i]=-1;
		dist[i]=infinity;
		Q[i]=i;
	}

	dist[source]=ER_BIT * R_BW; //----> phase 2

	for(i=0;i<=n-1;i++)				//while Q[] is not empty
	{
		min= infinity;
		for(j=0;j<=n-1;j++)
		{	/*to find minimum value in dist[] and check corresponding 			file is not yet visited*/
			if((min> dist[j])&&(Q[j]!=-1))
			{
				min=dist[j];
				source=j;
			}  //if
		} //for
		Q[source]=-1;				//Remove source from Q[]

		//cout <<"inside dijkastra --> source : "<<source<<"\n";
		if(prev[source] != -1)
		{
			//temp_allocated_port_info[source].n_i_port++;
			//temp_allocated_port_info[source].n_o_port++;
			//temp_allocated_port_info[prev[source]].n_i_port++;
			//temp_allocated_port_info[prev[source]].n_o_port++;
			if(temp_port_allocated_matrix[source][prev[source]]!=1)
			{
				temp_port_allocated_matrix[source][prev[source]]=1;

				temp_port_allocated_matrix[prev[source]][source]=1;
				temp_allocated_port_info[source].n_i_port++;
				temp_allocated_port_info[source].n_o_port++;
				temp_allocated_port_info[prev[source]].n_i_port++;
				temp_allocated_port_info[prev[source]].n_o_port++;

			} //if

		}//if

		for(j=0;j<=n-1;j++)
//To find neighbours and update
		{
		if((MHG.M_hattan_matrix[source][j].length != -1)&&Q[j]!=-1 )
		{

		if((temp_port_allocated_matrix[source][j]==1)||((temp_allocated_port_info[source].n_i_port+1<PV)&&(temp_allocated_port_info[source].n_o_port+1<PV)&&(temp_allocated_port_info[j].n_i_port+1<PV)&&(temp_allocated_port_info[j].n_o_port+1<PV)))
			{
				min_dist = dist[source] + MHG.M_hattan_matrix[source][j].length * EL_BIT* R_BW +ER_BIT*R_BW;

				if(min_dist<dist[j])
				{
						dist[j]=min_dist;
						prev[j]=source;

				}//if
			}//if




		}//if

		}//for

	} //for
	} //if(!directpath)
    //cout<<"prev contents for source:"<<source<<"\tdestination"<<desti<<endl;
    //cout<<"\nsrc:"<<src<<"dest:"<<desti<<
    //"BW:"<<R_BW<<endl;
    //int dd=desti;
    //cout<<dd<<"\t";
    //while(prev[dd]!=-1)
    //{  cout<<prev[dd]<<"\t";

      //dd=prev[dd];
    //}

    //getch()
    }

    ///////////////////////////////////////////////////////////////////
//function lower Dijkstra : find the route of given src and destination without reserving ports
///////////////////////////////////////////////////////////////////////////////////////////

void Implementation_Graph::lowerDijkstra(short source, short desti,int R_BW,short prev[size1])

{
	//cout<<"in lowerdijkstra src :"<<source<<"\t destination:"<<desti<<"\tBW:"<<R_BW<<endl;
	short n=size1;
	float dist[size1];

     //stores min dis of all tiles frm source
	short Q[size1];

     //set of all nodes
	int src=source;
	short i,j, test;
	float min,min_dist;

     //represents min distance from a node

	//if block to check if direct link can be laid between src and destination, inner if to check if already link exist between src and destination if not check whether laying a link violated the port constraint
	if(MHG.M_hattan_matrix[source][desti].length != -1)
	{  
		if(utemp_port_allocated_matrix[source][desti] != 1)
		{
			if(utemp_allocated_port_info[source].n_i_port+1 < PV && utemp_allocated_port_info[source].n_o_port+1 < PV && 				utemp_allocated_port_info[desti].n_i_port+1 < PV && utemp_allocated_port_info[desti].n_o_port+1 < PV)
			{
				directpath=1;
			}//if
		}//port alloated
	
		else directpath=2;
 	}//MHG
	struct port_info   temp_allocated_port_info[size1];


	if(!directpath)
	{
	short temp_port_allocated_matrix[size1][size1];

	for(i=0;i<size1;i++)
	{
		temp_allocated_port_info[i].n_i_port=utemp_allocated_port_info[i].n_i_port;//######

		temp_allocated_port_info[i].n_o_port=utemp_allocated_port_info[i].n_o_port;//####
	}


	for(i=0;i<size1;i++)
		for(j=0;j<size1;j++)
		temp_port_allocated_matrix[i][j] = utemp_port_allocated_matrix[i][j];



	for(i=0;i<=n-1;i++)
	{
		prev[i]=-1;
		dist[i]=infinity;
		Q[i]=i;
	}

	dist[source]=ER_BIT * R_BW; //----> phase 2

	for(i=0;i<=n-1;i++)				//while Q[] is not empty
	{
		min= infinity;
		for(j=0;j<=n-1;j++)
		{	/*to find minimum value in dist[] and check corresponding 			file is not yet visited*/
			if((min> dist[j])&&(Q[j]!=-1))
			{
				min=dist[j];
				source=j;
			}  //if
		} //for
		Q[source]=-1;				//Remove source from Q[]

		//cout <<"inside dijkastra --> source : "<<source<<"\n";

		for(j=0;j<=n-1;j++)
//To find neighbours and update
		{
		if((MHG.M_hattan_matrix[source][j].length != -1)&&Q[j]!=-1 )
		{

		if((temp_port_allocated_matrix[source][j]==1)||((temp_allocated_port_info[source].n_i_port+1<PV)&&(temp_allocated_port_info[source].n_o_port+1<PV)&&(temp_allocated_port_info[j].n_i_port+1<PV)&&(temp_allocated_port_info[j].n_o_port+1<PV)))
			{
				min_dist = dist[source] + MHG.M_hattan_matrix[source][j].length * EL_BIT* R_BW +ER_BIT*R_BW;

				if(min_dist<dist[j])
				{
						dist[j]=min_dist;
						prev[j]=source;

				}//if
			}//if


		}//if

		}//for

	} //for
	}//if directpath
    //cout<<"prev contents for source:"<<source<<"\tdestination"<<desti<<endl;
    //cout<<"\nsrc:"<<src<<"dest:"<<desti<<
    //"BW:"<<R_BW<<endl;
    //int dd=desti;
    //cout<<dd<<"\t";
    //while(prev[dd]!=-1)
    //{  cout<<prev[dd]<<"\t";
      //dd=prev[dd];
    //}

    //getch();
}

///////////////end of lower Dijkstra

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// function upperDijkstra find the route of given src and desti , by reserving ports constraint
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Implementation_Graph::upperDijkstra(short source, short desti,int R_BW, short prev[size1])

{
	//cout<<"in upperdijkstra src :"<<source<<"\t destination:"<<desti<<"\tBW:"<<R_BW<<endl;
	short n=size1;
	float dist[size1];

     //stores min dis of all tiles frm source
	short Q[size1];

     //set of all nodes
	int src=source;
	short i,j, test;
	float min,min_dist;

     //represents min distance from a node
	//if block to check if direct link can be laid between src and destination, inner if to check if already link exist between src and destination if not check whether laying a link violated the port constraint

	if(MHG.M_hattan_matrix[source][desti].length != -1)
	{  
		if(utemp_port_allocated_matrix[source][desti] != 1)
		{
			if(utemp_allocated_port_info[source].n_i_port+1 < PV && utemp_allocated_port_info[source].n_o_port+1 < PV && 				utemp_allocated_port_info[desti].n_i_port+1 < PV && utemp_allocated_port_info[desti].n_o_port+1 < PV)
			{
				directpath=1;
			}//if
		}//port alloated
	
		else directpath=2;
 	}//MHG	

	if(!directpath)
	{
	struct port_info   temp_allocated_port_info[size1];

	

	short temp_port_allocated_matrix[size1][size1];

	for(i=0;i<size1;i++)
	{
		temp_allocated_port_info[i].n_i_port=utemp_allocated_port_info[i].n_i_port;//######

		temp_allocated_port_info[i].n_o_port=utemp_allocated_port_info[i].n_o_port;//####
	}


	for(i=0;i<size1;i++)
		for(j=0;j<size1;j++)
		temp_port_allocated_matrix[i][j] = utemp_port_allocated_matrix[i][j];



	for(i=0;i<n;i++)
	{
		prev[i]=-1;
		dist[i]=infinity;
		Q[i]=i;
	}

	dist[source]=ER_BIT * R_BW; //----> phase 2

	for(i=0;i<=n-1;i++)				//while Q[] is not empty
	{
		min= infinity;
		for(j=0;j<=n-1;j++)
		{	/*to find minimum value in dist[] and check corresponding 			file is not yet visited*/
			if((min> dist[j])&&(Q[j]!=-1))
			{
				min=dist[j];
				source=j;
			}  //if
		} //for
		Q[source]=-1;				//Remove source from Q[]

		//cout <<"inside dijkastra --> source : "<<source<<"\n";
		if(prev[source] != -1)
		{
			
			if(temp_port_allocated_matrix[source][prev[source]]!=1)
			{
				temp_port_allocated_matrix[source][prev[source]]=1;

				temp_port_allocated_matrix[prev[source]][source]=1;
			temp_allocated_port_info[source].n_i_port++;
			temp_allocated_port_info[source].n_o_port++;
			temp_allocated_port_info[prev[source]].n_i_port++;
			temp_allocated_port_info[prev[source]].n_o_port++;

			} //if

		}//if

		for(j=0;j<=n-1;j++)
//To find neighbours and update
		{
		if((MHG.M_hattan_matrix[source][j].length != -1)&&Q[j]!=-1 )
		{

		if((temp_port_allocated_matrix[source][j]==1)||((temp_allocated_port_info[source].n_i_port+1<PV)&&(temp_allocated_port_info[source].n_o_port+1<PV)&&(temp_allocated_port_info[j].n_i_port+1<PV)&&(temp_allocated_port_info[j].n_o_port+1<PV)))
			{
				min_dist = dist[source] + MHG.M_hattan_matrix[source][j].length * EL_BIT* R_BW +ER_BIT*R_BW;

				if(min_dist<dist[j])
				{
						dist[j]=min_dist;
						prev[j]=source;

				}//if
			}//if


		}//if

		}//for

	} //for
	}//if(!directpath)
    //cout<<"prev contents for source:"<<source<<"\tdestination"<<desti<<endl;
    //cout<<"\nsrc:"<<src<<"dest:"<<desti<<
    //"BW:"<<R_BW<<endl;
    //int dd=desti;
    //cout<<dd<<"\t";
    //while(prev[dd]!=-1)
    //{  cout<<prev[dd]<<"\t";
      //dd=prev[dd];
    //}

    //getch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Method finds shortest path from a specified source to destination by calling dijkstra function and updates Path_array, graph_const_matrix and routing table for corresponding source and destination with PRIORITY 0*/
//input  : source, destination, temp_m_hattan_matrix of type M_hattan as argument
//output : stores path from source to destination in new_path array
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Implementation_Graph::Dig_shortest_path(short source)
{
	//int count;

	short prev[size1];

//prev stores the predecessor of each tile which is at minimum distance from the tile

	short temp_path[size1];

//stores path from destination to source

	int i,x,j;

	//cout<<"in DSP caled from consturctor"<<endl;
	//getch();

	int Bclaimarray_size=ACG.return_B_Claim_size();
	for(int bi=0;bi<Bclaimarray_size ;bi++)
	{  //cout<<" src ------------- "<<source<<"bi------------------"<<bi<<endl;
	   directpath=0; 
	   if(source==ACG.B_Claim_array[bi][0])
	    {


	    //cout<<"src DSP"<<ACG.B_Claim_array[bi][0];//getch();
		int desti=ACG.B_Claim_array[bi][1];
		int R_BW=ACG.B_Claim_array[bi][2];
		Dijkstra(source,desti,R_BW,prev);	//calling dijkstra for particular source

		ACG.B_Claim_array[bi][3]=1;
		path_array[count][0]=source;
		path_array[count][1]=desti;
		path_array[count][2]=0;

	    if(directpath==1)
	     {    port_allocated_matrix[desti][source]=1;
	        	port_allocated_matrix[source][desti]=1;

		//shows thcat link exist between src n dest
			allocated_port_info[desti].n_i_port++;
			allocated_port_info[desti].n_o_port++;
			allocated_port_info[source].n_i_port++;
			allocated_port_info[source].n_o_port++;
			path_array[count][2]=1;
			path_array[count][3]=source;
			path_array[count][4]=desti;
			count++;
			cost += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[source][desti].length);			
			continue;
	
		}//IF
            else if (directpath==2) 
            {
		path_array[count][2]=1;	
		path_array[count][3]=source;
		path_array[count][4]=desti;
		count++;
		cost += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[source][desti].length);
		continue;
	   } //else directgpath==2	


	for(j=0;j<size1;j++)
	{
		temp_path[j]=-1;
	}

		cout<<"source ::" <<source<<":: count ::"<<count;
	//cout<<"\n";
	//cout<<"in DSP src to dest \t"<<source<<"\t"<<desti<<endl;
	for(i=0;desti!=-1;i++)
	{      //  cout<<desti<<"\t";

		if(prev[desti]!=-1)
		{

//checking if port info of particular tiles already exists,if they don't then mark them as used and increasing their no. of ports
			if(port_allocated_matrix[desti][prev[desti]]!=1)
			{
			port_allocated_matrix[desti][prev[desti]]=1;

//shows that link exist between prev[desti] & desti

			port_allocated_matrix[prev[desti]][desti]=1;

//shows that link exist between desti & prev[desti]

				allocated_port_info[desti].n_i_port++;
				allocated_port_info[desti].n_o_port++;
				allocated_port_info[prev[desti]].n_i_port++;
				allocated_port_info[prev[desti]].n_o_port++;
			}//IF
		}//IF
		//cout<<" "<<desti;

		temp_path[i]=desti;

//temp_path stores path from destination to souce
	cost += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[desti][prev[desti]].length);
	desti=prev[desti];

	}//FOR
	// to store the shortestpath of routed traffic characteristic
	for(i=i-1,x=3;i>=0;i--,x++)
	{
		path_array[count][x]=temp_path[i];
		path_array[count][2]=1;
		cout<<"within patharry loop count:"<<count<<endl;	
	}//if
	count++;
        
       }//if

    }//for



}

///////////////////////////////////////////////////////////////////////////////////////
//////////////// function upper_dig_shortest_path , find routes of all traffice characteristic of given source
//////////////////////////////////////////////////////////////////////////////////////////////////
void Implementation_Graph::upper_dig_shortest_path(short source)
{
	int count;

	short prev[size1];
	short temp_path[size1];


	int i,x,j;


	
	//cout<<"in upperDSP caled from upper for source:"<<source<<endl;
	//getch();

	int Bclaimarray_size=ACG.return_B_Claim_size();
	for(int bi=0;bi<Bclaimarray_size ;bi++)
	{
	    if(source==utemp_b_claim_array[bi][0])
	    {   directpath=0;
		int desti=utemp_b_claim_array[bi][1];
		int R_BW=utemp_b_claim_array[bi][2];
		upperDijkstra(source,desti,R_BW,prev);	//calling dijkstra for particular source

		temp.gpath_array[temp.count][0]=source;
		temp.gpath_array[temp.count][1]=desti;
		temp.gpath_array[temp.count][2]=0;		
		if(directpath==1)
	       {    utemp_port_allocated_matrix[desti][source]=1;
	        	utemp_port_allocated_matrix[source][desti]=1;

		//shows that link exist between src n dest
			utemp_allocated_port_info[desti].n_i_port++;
			utemp_allocated_port_info[desti].n_o_port++;
			utemp_allocated_port_info[source].n_i_port++;
			utemp_allocated_port_info[source].n_o_port++;
			
			 ubc += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[source][desti].length);	
			temp.gpath_array[temp.count][2]=1;
			temp.gpath_array[temp.count][3]=source;
			temp.gpath_array[temp.count][4]=desti;		
			temp.count++;continue;
	
		}//IF
             else if (directpath==2) 
             {
		
		temp.gpath_array[temp.count][2]=1;
		temp.gpath_array[temp.count][3]=source;
		temp.gpath_array[temp.count][4]=desti;		
		ubc += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[source][desti].length);
		temp.count++;continue;
	      } //else directgpath==2	


		
		for(j=0;j<size1;j++)
		{
			temp_path[j]=-1;
		}

	     for(i=0;desti!=-1;i++)
	     {      //  cout<<desti<<"\t";

		if(prev[desti]!=-1)
		{

		if(utemp_port_allocated_matrix[desti][prev[desti]]!=1)
			{
			utemp_port_allocated_matrix[desti][prev[desti]]=1;
		     utemp_port_allocated_matrix[prev[desti]][desti]=1;


				utemp_allocated_port_info[desti].n_i_port++;
				utemp_allocated_port_info[desti].n_o_port++;
				utemp_allocated_port_info[prev[desti]].n_i_port++;
				utemp_allocated_port_info[prev[desti]].n_o_port++;
			}//IF
		}//IF


		temp_path[i]=desti;

	ubc += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[desti][prev[desti]].length);
	desti=prev[desti];

	}//FOR */

	for(i=i-1,x=3;i>=0;i--,x++)
	{
		temp.gpath_array[temp.count][x]=temp_path[i];
		temp.gpath_array[temp.count][2]=1;
		cout<<"within patharry loop count:"<<count<<endl;	
	}//if
	temp.count++;
	//utemp_b_claim_array[bi][3]=1;
	}//if


    }//for

}

/////////////////////////////////////////////////////////////////////////////
//lower bound dig_shortest_path  : find the route of all traffic characrteries of given source , without reserving ports
///////////////////////////////////////////////////////////////////////////////////////////


void Implementation_Graph::lower_dig_shortest_path(short source)
{
	//int count;
	short prev[size1];
	short temp_path[size1];
	int i,x,j;
	//cout<<"in lowerDSP caled from upper for source:"<<source<<endl;
	//getch();
	int Bclaimarray_size=ACG.return_B_Claim_size();
	for(int bi=0;bi<Bclaimarray_size ;bi++)
	{   directpath=0;
	    if(source==utemp_b_claim_array[bi][0])
	    {
		int desti=utemp_b_claim_array[bi][1];
		int R_BW=utemp_b_claim_array[bi][2];
		lowerDijkstra(source,desti,R_BW,prev);	//calling dijkstra for particular source

		if(directpath==1 || directpath==2)
	        {    
		
			lbc += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[source][desti].length);
			continue;
	      	} //else directgpath==2				
		for(j=0;j<size1;j++)
		{
			temp_path[j]=-1;
		}

	     for(i=0;desti!=-1;i++)
	     {      //  cout<<desti<<"\t";

		temp_path[i]=desti;

		lbc += ER_BIT + (EL_BIT * MHG.M_hattan_matrix[desti][prev[desti]].length);
		desti=prev[desti];

		}//FOR */

	//utemp_b_claim_array[bi][3]=1;
	}//if


    }//for

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////function upperbound : routes all remaining traffic characteristics  , by reserving ports
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void Implementation_Graph:: upperbound()
{
	int i,j;

	for(i=0;i<size1;i++)
	for(j=0;j<size1;j++)

	utemp_port_allocated_matrix[i][j] = port_allocated_matrix[i][j];



	for(i=0;i<size1;i++)
	{
		utemp_allocated_port_info[i].n_i_port=allocated_port_info[i].n_i_port;

		utemp_allocated_port_info[i].n_o_port=allocated_port_info[i].n_o_port;
	}


	for(i=0;i<ACG.B_Claim_size;i++)
	for(j=0;j<4;j++)
	  utemp_b_claim_array[i][j]=ACG.B_Claim_array[i][j];

       //cout<<"bsum in upperbound"<<endl;
      for(i=0;i<nofsrc_traffic;i++)
      {	 for(j=0;j<3;j++)
	 {
	   utemp_bsum_claim_array[i][j]=ACG.Bsum_claim_array[i][j];
	  //cout<<utemp_bsum_claim_array[i][j]<<"\t";
	  //getch();
	 }
	 cout<<endl;
     }
	temp.count=0;
        //initialise temp gpath_array
	for(int i=0;i<(size1*size1) ; i++)
         {  for(j=0 ;j<size1;j++)
               temp.gpath_array[i][j]=-1;}
     
	//copy path array from paraent to temp
	for(int i=0;path_array[i][0]!=-1 ; i++)
         {  for(j=0 ;path_array[i][j]!=-1 ;j++)
               temp.gpath_array[i][j]=path_array[i][j];}
     
	temp.count=i;

	//here check for non routed tiles and then route them descendig order
       for(short si=0;si<nofsrc_traffic;si++)
	   if(utemp_bsum_claim_array[si][2]==0 && utemp_bsum_claim_array[si][1]!=0)
	      {
			short uper_src=utemp_bsum_claim_array[si][0];
			//cout<<"calling uper_dig_shortest_path for source:"<<uper_src<<endl;
			upper_dig_shortest_path(uper_src);
			utemp_bsum_claim_array[si][2]=1; //seting it to 1 or not does not make difference as arrray is traversed in order;
		    }//if

	//copy port allocated matrix info to temp struct global data

        for(int i=0;i<size1;i++)
		{temp.gallocated_port_info[i].n_i_port=allocated_port_info[i].n_i_port;
		temp.gallocated_port_info[i].n_o_port=allocated_port_info[i].n_o_port;}

	for(int i=0;i<size1;i++)
	{
		for(int j=0;j<size1;j++)
		temp.gport_allocated_matrix[i][j]=port_allocated_matrix[i][j];
	}


}//upperbound


/////////////////////////////////
//function lowerbound: find the route of all remaining node withour reserving ports even temporarily
/////////////////////////////////////////////////////////////////////////////////////

void Implementation_Graph:: lowerbound()
{
	int i,j;

	for(i=0;i<size1;i++)
	for(j=0;j<size1;j++)
	utemp_port_allocated_matrix[i][j] = port_allocated_matrix[i][j];



	for(i=0;i<size1;i++)
	{
		utemp_allocated_port_info[i].n_i_port=allocated_port_info[i].n_i_port;

		utemp_allocated_port_info[i].n_o_port=allocated_port_info[i].n_o_port;
	}


	for(i=0;i<ACG.B_Claim_size;i++)
	for(j=0;j<4;j++)
	  utemp_b_claim_array[i][j]=ACG.B_Claim_array[i][j];

       //cout<<"bsum in upperbound"<<endl;
      for(i=0;i<nofsrc_traffic;i++)
      {	 for(j=0;j<3;j++)
	 {
	   utemp_bsum_claim_array[i][j]=ACG.Bsum_claim_array[i][j];
	  //cout<<utemp_bsum_claim_array[i][j]<<"\t";
	  //getch();
	 }
	 cout<<endl;
     }
	//here check for non routed tiles and then route them descendig order
       for(short si=0;si<nofsrc_traffic;si++)
	   if(utemp_bsum_claim_array[si][2]==0 && utemp_bsum_claim_array[si][1]!=0)
	      {
			short uper_src=utemp_bsum_claim_array[si][0];
			//cout<<"calling lower_dig_shortest_path for source:"<<uper_src<<endl;
			lower_dig_shortest_path(uper_src);
			utemp_bsum_claim_array[si][2]=1; //seting it to 1 or not does not make difference as arrray is traversed in order;
	      }//if

}//lowerbound



////////////end of lower bound


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  here we have used prims algorithm on m_hattan_matrix[][] to create a minimum spanning tree & then BFS_queue 
//  is used on this minimum spanning tree to find levels and sibling order of various nodes of a tree.
//  input is temporary BW_claim_matrix and m_hattan_matrix as arguments.
//  function finds the root(node having maximum bandwidth requirement) from temp Bandwidth claim matrix.
//  defines BFS_queue in order to determine level number & sibling order.
//  output is Graph_const_matrix[][] and Tree_Rep_Matrix[size1][size1+3] in which Tree_Rep_Matrix[i][size1]--> contains parent of tile i, Tree_Rep_Matrix[i][size1+1]--> contains level no. of tile i,Tree_Rep_Matrix[i][size1+2]--> contains sibling order of tile i
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Implementation_Graph::Create_Minimum_Spanning_tree3()
{
	short Prims_visited_array[size1];// array storing visited nodes while traversing
	short k,i,current,j;
	short u=0;
	int max;  // max is used as index, to determining root.
	short root; // it holds the root of a minimum spanning tree.
	float min_distance;
	float Very_Large_Value;//larger than the maximum length between nodes 
	struct M_hattan Temp_M_Hattan_Matrix[size1][size1];//structure of type M_hattan
	short flag_nav =0;
	short x,y;
	short Visited_node_count=1;//  as root is already visited
	int BFS_queue[size1+1];//this array is used to determine the level_no and sibling order in Tree_Rep_matrix[][].
      		               //in BFS_queue[], size+1 is used because while loop is terminated when BFS_queue[]==-1
	

	for(i=0;i<size1;i++)
		{
	 		for(j=0;j<(size1+3);j++)
			{
				Tree_Rep_Matrix[i][j]=-1;
			}
		}	

	for(i=0;i<size1;i++)
		{
	 		for(j=0;j<(size1+1);j++)
			{
				parent_info[i][j]=-1;
			}
		}		


	//initialise escape path array
	for(int i=0;i<(size1*size1); i++)
		for(int j=0;j< (size1+3) ; j++)
   		   escape_path_array[i][j]=-1;


	//copying M_hattan_matrix into Temp_M_Hattan_Matrix
	
	for(x=0;x<size1;x++)
	{
 		for(y=0;y<size1;y++)
		{
			Temp_M_Hattan_Matrix[x][y].length=MHG.M_hattan_matrix[x][y].length;
		}
	} 
	
	
	max=BW_Claim_matrix[0][size1]; 
	root=0;

	//Finds the node with maximum bandwidth requirement and treat that node as root
	for(i=0;i<size1;i++)
	{
		if(max<BW_Claim_matrix[i][size1])
		{			
		 max=BW_Claim_matrix[i][size1];
		 root=i;
		}
	}
				
			
	Tree_Rep_Matrix[root][size1]=-1;// as parent of root is null so we represent this info by assigning parent 						   of root as -1
		
	for(x=0;x<size1;x++)				//initialising Prims_visited array by -1
	{
		Prims_visited_array[x]=-1;
	}
	
		
	Prims_visited_array[0]=root;			//the root is marked as visited
	for(i=0;i<size1;i++)				//as root is visited so there is no need to traverse for this tile
	{ 
		 Temp_M_Hattan_Matrix[i][root].length=-1;
	}
	Very_Large_Value=MHG.M_hattan_matrix[0][0].length;//initialising variable with maximum length
	
	// this loop determines very_large_value  which is equal to the value one greater than actual maximum length in M_Hattan_Matrix[][]
	for(j=0;j<size1;j++)
	{
		 for (k=0;k<size1;k++)   
		{
			if(MHG.M_hattan_matrix[j][k].length>Very_Large_Value)
				Very_Large_Value=MHG.M_hattan_matrix[j][k].length;
		}
	}
	Very_Large_Value=Very_Large_Value+1;
		
	short bfsq[size1];
        int front=0, rear=0;	
	
		short v=0;
		short current_source_node;
		short destination_node=-1;
		min_distance=Very_Large_Value;
		current=-1;
		flag_nav = 0;
		bfsq[front]= Prims_visited_array[0];
		while((Prims_visited_array[front]!=-1) && (front< size1) && (Visited_node_count <= size1))	//finds the tile which is at min dist. from set of visited tiles
		{
			current_source_node=Prims_visited_array[front];
			//flag_nav = 0;	
			for(j=0;j<size1;j++)
			{
					
				if((Temp_M_Hattan_Matrix[current_source_node][j].length!=-1)&& ((port_allocated_matrix[current_source_node][j]== 1)||((port_allocated_matrix[current_source_node][j]!=1)&&((allocated_port_info[current_source_node].n_i_port +1 ) <= PV)&&((allocated_port_info[current_source_node].n_o_port +1 ) <= PV )&&((allocated_port_info[j].n_i_port+1) <= PV)&&((allocated_port_info[j].n_o_port+1) <= PV))))
				{	
	  				
					
					Tree_Rep_Matrix[current_source_node][j]=Temp_M_Hattan_Matrix[current_source_node][j].length;
					//bfsq[++rear]=j;
						// Parent info is written in the Tree_Rep_Matrix
					Tree_Rep_Matrix[j][size1]=current_source_node;

					// Updating no of input and output ports
					if(port_allocated_matrix[current_source_node][j]!=1) {
						port_allocated_matrix[current_source_node][j]=1;		
						port_allocated_matrix[j][current_source_node]=1;
						allocated_port_info[current_source_node].n_i_port++;
						allocated_port_info[current_source_node].n_o_port++;
						allocated_port_info[j].n_i_port++;
						allocated_port_info[j].n_o_port++;	
					}

					Prims_visited_array[++v]=j;
					short d;
					for(d=0;d<size1;d++)
					{
						Temp_M_Hattan_Matrix[d][j].length=-1;
					}
					Visited_node_count++;
				}//if 
			
			}//for
             front++;
	 } //end of outer loop
	
	for(i=0;i<(size1+1);i++)  			// inserting -1 to BFS_queue
	{
		BFS_queue[i]=-1;
	}
	for(k=0;k<size1;k++)   				// searching Tree_Rep_Matrix[][] to determine the root
	{
		if(Tree_Rep_Matrix[k][size1]==-1)   	// condition applied on parent info of Tree_Rep_Matrix[][]
		{
			short root1=k;
			Tree_Rep_Matrix[k][size1+1]=0;  // level no of root
			Tree_Rep_Matrix[k][size1+2]=0;  // sibling order of root
			BFS_queue[0]=root1;
			break;
		}
	}
	i=0;j=0;
	short level=1;
	root=k;
	short par1,par2;
	short prev_node=BFS_queue[i];
	//calculating level no. and sibling oreder of nodes in tree rep matirx. Later a while loop is terminated when 		  bfs_queue[]=-1	
	int count=0;
	while(BFS_queue[i]!=-1)
	{
					
		for(k=0;k<size1;k++)
		{
			if(Tree_Rep_Matrix[BFS_queue[i]][k]>0)
			{
				par1=short(Tree_Rep_Matrix[prev_node][size1]);
				par2=short(Tree_Rep_Matrix[k][size1]);
				if((Tree_Rep_Matrix[BFS_queue[i]][size1]!=-1)&&(Tree_Rep_Matrix[par1][size1+1]!=Tree_Rep_Matrix[par2][size1+1]))                          
				{
				level++;
				}	
				Tree_Rep_Matrix[k][size1+1]=level;  // level no of tile k
				Tree_Rep_Matrix[k][size1+2]=j+1;    // sibling order of tile 


				 //here code to write the all parents and their leve, paretn_info [x][0] has level info nad 1 onwards parents list upwards		
				//
				if(parent_info[k][0] ==-1)
				{
				cout<<"COUNT:"<<count<<endl;				
				parent_info[k][0]=Tree_Rep_Matrix[k][size1+1]; 
				int pinfo=k,p=1;	
				while(Tree_Rep_Matrix[pinfo][size1]!= -1)
				{ 
					parent_info[k][p]=Tree_Rep_Matrix[pinfo][size1];
			                cout<<"pinfo:"<<pinfo<<"::"<< Tree_Rep_Matrix[pinfo][size1]<<endl;
					pinfo=Tree_Rep_Matrix[pinfo][size1];  
					p++;
				}
				}//ifif(parent_info[k][0] ==-1)

				BFS_queue[j+1]=k; 		    // neighbours of BFS_queue[] are put in the queue
				j++;
				prev_node=k;

			}//end of if

		}//end of for
	
		i++;

	}//end of while
	
	// Updating Graph_Const_Matrix
	/*for(int i=0;i<size1;i++){
		for(j=0;j<size1;j++){
				
			if(Tree_Rep_Matrix[i][j]!=-1){
			    //if(port_allocated_matrix[i][j]!=1){	
				Graph_Const_Matrix[i][j].length=Tree_Rep_Matrix[i][j];
				Graph_Const_Matrix[j][i].length=Tree_Rep_Matrix[i][j];
			     //}		
			}	
		}
	}*/

	// print the tree_rep_matrix	
	cout<<endl;	
	int  n1=0;
	for(int i=0;i<size1;i++){
		cout<<i<<"::"<<Tree_Rep_Matrix[i][size1]<<endl;		
		for(j=0;j<size1;j++){
				
			if(Tree_Rep_Matrix[i][j]!=-1)
			n1++;	
		}
	}

	for(int i=0;i<size1;i++)
        {   
            tile[i].nofparent=0;
	    tile[i].level=-1;
            tile[i].order=-1;
            for(int j=0;j<PV;j++)
	     {	for(int k=0;k<size1;k++)
              	{	tile[i].parent[j][k].ancestor=-1;
              		tile[i].parent[j][k].distance=-1;
	        }
 	     }
	}

	// gt all the details of tile to tile strucutre 
        //parent info has at [i][0]= level ,then [i][1] onwards ancestor list
	//loop to find out all ancestror of a all tile by tile
	for(i=0;i<size1;i++)
	{  
	    tile[i].level=parent_info[i][0];
           tile[i].order=Tree_Rep_Matrix[i][size1+2];
           for(j=0;j<size1;j++)
	   { int tcnt=0, count=1;
             if((port_allocated_matrix[i][j]==1) &&  (tile[i].level > parent_info[j][0]) )
              {
		  tile[i].nofparent++; // to check if parent is root  level node
	         
		 
                    tile[i].parent[(tile[i].nofparent)-1][tcnt].ancestor = j;
		    tile[i].parent[(tile[i].nofparent)-1][tcnt].distance = MHG.M_hattan_matrix[i][j].length;	
			tcnt++;  //count++;
		
		  
		
	       
			int prev=j;
			while( parent_info[j][count]!=-1)
		        { 
			tile[i].parent[(tile[i].nofparent)-1][tcnt].ancestor = parent_info[j][count];
			tile[i].parent[(tile[i].nofparent)-1][tcnt].distance = MHG.M_hattan_matrix[prev][parent_info[j][count]].length;	
			int temp=tcnt-1;
			prev=parent_info[j][count]!=-1;
       			
			while(temp >=0)			
			{

                          tile[i].parent[(tile[i].nofparent)-1][tcnt].distance +=   tile[i].parent[(tile[i].nofparent)-1]        [temp].distance	;
				cout<<"temo Hello";
				temp--;
			}//while			
			tcnt++;  count++;
			
			}//while
		  	
		}//if
           }//for
        }//for


	//PRINT THE TILE PARENT INFO IN DET[AIL

       cout<<"hello ************ parent "<<endl;
	for(int i=0;i<size1;i++)
        {   
            cout<<"tile : "<<i<<endl;
            cout<<"Nofparetns-->"<<tile[i].nofparent<<":::"<<endl;
	    cout<<"level--->"<<tile[i].level<<":::"<<endl;
            cout<<"order-->"<<tile[i].order<<":::"<<endl;
	     cout<<endl<<"Parent info for tile ---:"<<i<<endl;
            for(int j=0;j<PV;j++)
		{
		//cout<<endl<<"parent "<<j+1<<"-->"<<endl;               
		for(int k=0;k<size1;k++)
		 { cout<<":"<<tile[i].parent[j][k].ancestor; 
		   cout<<":"<<tile[i].parent[j][k].distance; }
		cout<<endl;
		}//fpr
  	}//for






	cout<<endl<<"parent info of all tiles"<<":::"<<endl;
        for(i=0;i<size1;i++)
		{       cout<<i<<":";
	 		for(j=0;j<(size1+1);j++)
			{
				if(parent_info[i][j]!=-1)
					cout<<parent_info[i][j]<<"::";
			}
                         cout<<endl<<endl;
		}	

	
  
        
	//traverse patharray for recordinf escape path for all intermediate nodes
        for(int i=0; i< noftraffic; i++)
	{       short desti=path_array[i][1];
		for(int j=3; path_array[i][j]!= desti; j++)
                {updownroute(path_array[i][j], desti);
		 write_escape_path(path_array[i][j], desti);}
         }
	cout<<"\nESCPAE PATH"<<endl;
	cout<<"escape count"<<escape_count;        
	 for(int i=0; i< escape_count; i++)
	{       cout<<"inside escaep"<<endl;
		for(int j=0; escape_path_array[i][j] != -1; j++)
                 { 
			cout<<escape_path_array[i][j]<<":";}
                cout<<"\n\n";
         }


       	}//function create minimum spanning tree3


// function for write_escape_path

void Implementation_Graph::write_escape_path(short src,short dest)
{
	//CHECK IF ALREADY  PATH EXIST 
        for(int i =0; i < escape_count ; i++)	
	{	if((escape_path_array[i][0]==src) &&  (escape_path_array[i][1]==dest))
		{	cout<<"already path exist "; return;}
	}
	if(escapeinfo[0]==1) // if direct link 
        { cout<<"write Path array"<<endl;
	   escape_path_array[escape_count][0]= src ;
	  escape_path_array[escape_count][1]= dest ;  
          escape_path_array[escape_count][2]= 0 ; 
          escape_path_array[escape_count][3]= src ; 
          escape_path_array[escape_count][4]= dest ; 
	  escape_count++;
	}
	else if (escapeinfo[0]==2) //src level is less than destination level
	{      int i,j;
		cout<<"case 2"<<endl;
		 escape_path_array[escape_count][0]= src ;
	  	escape_path_array[escape_count][1]= dest ;  
         	 escape_path_array[escape_count][2]= 0 ; 
		//cout<<"PATH"<<escape_path_array[escape_count][0]<<":"<<escape_path_array[escape_count][1]<<":"<<escape_path_array[escape_count][2]<<":" ; 
		for( i=escapeinfo[3], j=3 ; i>=0; i--,j++)
			 { escape_path_array[escape_count][j]= tile[dest].parent[escapeinfo[2]][i].ancestor ;	
				//cout<< escape_path_array[escape_count][j]<<":";
			}
		escape_path_array[escape_count][j]=dest;
		//cout<<escape_path_array[escape_count][j];
		escape_count++;
	}
	else if(escapeinfo[0]==3) // src level is greater than dest level
	{      int i,j;
		cout<<"case 3"<<endl;
		 escape_path_array[escape_count][0]= src ;
	  	escape_path_array[escape_count][1]= dest ;  
         	 escape_path_array[escape_count][2]= 0 ; 
		escape_path_array[escape_count][3]= src ;
		//cout<<"PATH"<<escape_path_array[escape_count][0]<<":"<<escape_path_array[escape_count][1]<<":"<<escape_path_array[escape_count][2]<<":"<<escape_path_array[escape_count][3]<<":" ; 
		for( i=0, j=4 ; i<=escapeinfo[3]; i++,j++)
			 {escape_path_array[escape_count][j]= tile[src].parent[escapeinfo[1]][i].ancestor ;	
		//	cout<< escape_path_array[escape_count][j]<<":";
			}
		escape_count++;
	}
	else if(escapeinfo[0]==4)
        {	
		int i,j;
		cout<<"case 4"<<endl;
		escape_path_array[escape_count][0]= src ;
	  	escape_path_array[escape_count][1]= dest ;  
         	escape_path_array[escape_count][2]= 0 ; 
		escape_path_array[escape_count][3]= src ;
		//cout<<"PATH"<<escape_path_array[escape_count][0]<<":"<<escape_path_array[escape_count][1]<<":"<<escape_path_array[escape_count][2]<<":"<<escape_path_array[escape_count][3]<<":"; 
		for( j=4,i=0 ; i <=  escapeinfo[4] ; i++,j++)
			{ escape_path_array[escape_count][j]=tile[src].parent[escapeinfo[1]][i].ancestor;
				//cout<< escape_path_array[escape_count][j]<<":";
			}
		for ( i=escapeinfo[5]-1 ; i>=0 ;i--,j++)
			{escape_path_array[escape_count][j]=tile[dest].parent[escapeinfo[2]][i].ancestor;	
			//cout<< escape_path_array[escape_count][j]<<":";
			}
		escape_path_array[escape_count][j]=dest;
                //cout<<escape_path_array[escape_count][j]; 
		escape_count++;

	}
		
        // write the escape path to array


}// void Implementation_Graph::updownroute(short src,short dest)

// end of function




//function for updown routing

void Implementation_Graph::updownroute(short src,short dest)
{
	int commonparent[8][6]; // 0-srcp,1-destp, 2-ancestor , 3-distance     
	int flag=0;
	cout<<endl<<"updoqn routing function"<<endl;
       // cout<<"src Level->"<<tile[src].level<<endl<< "order->"<< tile[src].order << "nofparents->"<<tile[src].nofparent<<endl;
        cout<<"SOURCE-> "<< src <<":::"<<"dest->"<<dest;
	for(int i=0;i<6;i++)
           escapeinfo[i]=-1;
	if(port_allocated_matrix[src][dest] ==1) //check if src - dest has direct link
           //return 1;
	{	escapeinfo[0]=1;cout<<"return 1"<<endl;
	} //if(port_allocated_matrix[src][dest] ==1) 
        else
        {   int np;
	    //
		if(tile[src].level < tile[dest].level) // check if src is in ancestor list of dest
		{
			for(int nop=0; nop<tile[dest].nofparent;nop++)
			{
				for( np=0 ; ((tile[dest].parent[nop][np].ancestor != src) && tile[dest].parent[nop][np].ancestor != -1) ; np++);
				if(tile[dest].parent[nop][np].ancestor==src) 
				{ cout<<"return 2"<<endl;
				  escapeinfo[0]=2; escapeinfo[2]=nop;escapeinfo[3]=np;
				  flag=1;
				}
                         }
		}//if(tile[src].level < tile[dest].level) 
		else if(tile[src].level > tile[dest].level) // check if dest is in ancestor list of source
		{
			for(int nop=0; nop<tile[src].nofparent;nop++)
			{
				for( np=0 ; ((tile[src].parent[nop][np].ancestor != dest)  && (tile[src].parent[nop][np].ancestor != -1)) ; np++);
				if(tile[src].parent[nop][np].ancestor==dest) 
				{ cout<<"return 3"<<endl;
				  escapeinfo[0]=3; escapeinfo[1]=nop;escapeinfo[3]=np;// esacpe[3] store the index of dest
				  flag=1; 
				}
                         }
		}//if(tile[src].level < tile[dest].level) 								
		if(flag==0) // check for common parent for src and destination
		{  cout<<"HHHHHHHHHHHHHHHHHHHHHHHH"<<endl;
		   int count=0;//for commonparent counter
		   for(int srcp=0; srcp<tile[src].nofparent; srcp++)
		   {	for(int destp=0; destp< tile[dest].nofparent; destp++)
			{	int j=0,k=0;
				while (tile[src].parent[srcp][j].ancestor != -1) 
				{ k=0;
				  while(tile[dest].parent[destp][k].ancestor != -1)
				  {	if(tile[src].parent[srcp][j].ancestor == tile[dest].parent[destp][k].ancestor)
					{ //if (tile[src].parent[srcp][j].ancestor ==root) continue;  else 
			            cout<<endl<<"COMMON Parent->"<<tile[src].parent[srcp][j].ancestor;					
				    commonparent[count][0]=srcp, commonparent[count][1]=destp , 
				    commonparent[count][2]=tile[src].parent[srcp][j].ancestor;	
				    commonparent[count][3]=tile[src].parent[srcp][j].distance;
				    commonparent[count][4]=j; commonparent[count][5]=k; // j is srcparent col value, k is dest column value
					count++;
				    cout<<"COUNT in updown routing->"<<count<<endl;					
				       }//if
					
					 k++;
				  }//while(tile[dest].parent[destp][k].ancestor != -1))
					j++;
				}//while (tile[src].parent[srcp][j].ancestor != -1) 
			}//for(int destp=0; destp< tile[dest[.nofparentl destp)
		   }//for(int srcp=0; srcp<tile[src].noparent; srcp)
		 cout<<endl<<"common parent info";
		   int temp=0, tempdistance=99999,index_distance;
		 while(temp< count)
		 {  
						
			cout<<endl<<commonparent[temp][0]<<":"<<commonparent[temp][1]<<":"<<commonparent[temp][2]<<":"<<commonparent[temp][3];
			if(tempdistance > commonparent[temp][3])
		   	{
			tempdistance=commonparent[temp][3];
			index_distance=temp;
     		   	}//if
			temp++;
		}  // while(temp< count)
		//copying common 		
		escapeinfo[0]=4; escapeinfo[1]=commonparent[index_distance][0]; escapeinfo[2]=commonparent[index_distance][1], escapeinfo[3]=commonparent[index_distance][2]; escapeinfo[4]=commonparent[index_distance][4]; escapeinfo[5]=commonparent[index_distance][5];
		

		}//else  common parent

		//loop to find out minimum common ancestor 
				cout<<"Escape Path informatin"<<endl;
		//for(int i=0;i<4;i++)
                cout<<"Type->"<<escapeinfo[0]<<endl<<"Src parent pointer->"<<escapeinfo[1]<<endl<<"Dest parent pointer->"<<escapeinfo[2]<<endl<<"Common parent->"<<escapeinfo[3]<<endl<< " src parent col index"<<escapeinfo[4]<<endl<<" dest parent col index"<<escapeinfo[5]<<endl;

	//NOTE ESCAPE INFO STORED THE 0-TYPE 1-SRRC PARENT INDES; 2-DEST PARENT INDES ;3 - COMMON ANCESTOR IF ANY; 4-SRC COL INDEX FOR COMMON PARENT; 5- DEST COL INDEX FOR COMMON PARENT;

	} //else
}// updown route


// end of updown routing
/////////////////////end of spanniong tree





// A class for priority queue.
 // TODO: use heapsort data structure for speeding up
 typedef class PQueue
 {
 private:

     pimplementation_graph head;

 public:
	     int length;     
	PQueue();
     ~PQueue();
     int Length()
     {
	 return length;
     }
     int empty();
     void insert(pimplementation_graph node);
     pimplementation_graph next();
     void traverse();
 } PQueue, *pPQueue;


PQueue::PQueue()
 {
     length = 0;
     head = NULL;
 }

 PQueue::~PQueue()
 {  pimplementation_graph node;
    while(head!=NULL)
    {  node=head;
      head=head->next;
      delete node;
    }
 }



 int PQueue::empty()
 {
     if (length==0)
	 return 1;
     else
	 return 0;
 }

 void PQueue::insert(pimplementation_graph node)
 {
     // here we should insert the node at the position which
     // is decided by the cost of the node

     //node->next=head;
     //head=node;
     //cout<<"inserted in quwuw"<<endl;
     if (length == 0)
     {
	 head = node;
	 node->next = NULL;
	 length++;
	 cout<<"insertd in queue"<<endl;
	 return;
     }
     pimplementation_graph parentNode = NULL;
     pimplementation_graph curNode = head;
     int i=0;
     for (i=0; i<length; i++)
     {
	 if (curNode->cost > node->cost)
	     //if (curNode->upperBound > node->upperBound)
	     break;
	 parentNode = curNode;
	 curNode = curNode->next;
     }
     if (parentNode == NULL)
     {
	 pimplementation_graph oldHead = head;
	 head = node;
	 node->next = oldHead;
	 length++;
	 return;
     }
     pimplementation_graph  pNode = parentNode->next;
     parentNode->next = node;
     node->next = pNode;
     length++;
 }

pimplementation_graph PQueue::next()
 {
     if (length==0)
	 return NULL;
     pimplementation_graph  oldHead = head;
     head = oldHead->next;
     length --;
     return oldHead;
 }

// here change this traverse to w.r.t src tiles routed info*********
void   PQueue::traverse()
{


	int i,j;
	pimplementation_graph  currentNode=head;
	cout<<"nodes in the queue are:: "<<length<<endl;
	while(currentNode!=NULL)
	{
	//cout<<"node-";
	//int n=currentNode->stage;
	//for(int i=0;i<n;i++)
	//cout<< currentNode->mappingSequency[i];

	cout<<"queue:stage-->"<<currentNode->stage<<"::";
	cout<<"queue :cost"<<currentNode->cost<<"::";
	cout<<"queue:upercost"<<currentNode->ubc<<"::";
	cout<<"queue:lowercost"<<currentNode->lbc<<endl;
	//cout<<"BSUM array"<<endl;	//cout<<"BSUM array"<<endl;
	//for(i=0;i<size1;i++)
	  //  cout<<currentNode->ACG.Bsum_claim_array[i][0]<<":"<<currentNode->ACG.Bsum_claim_array[i][1]<<":"<<currentNode->ACG.Bsum_claim_array[i][2]<<endl;
	//getch();
	//cout<<"B Claim Array"<<endl;
       //	for(i=0;i<size1;i++)
	 //   cout<<currentNode->ACG.B_Claim_array[i][0]<<":"<<currentNode->ACG.B_Claim_array[i][1]<<":"<<currentNode->ACG.B_Claim_array[i][2]<<":"<<currentNode->ACG.B_Claim_array[i][3]<<endl;
	currentNode=currentNode->next;
	//cout<<", ";
	//getch();
	}
}










void bbmapping()
 {
     //float minCost = MAX_VALUE;
     //float minUpperBound = MAX_VALUE;
     pPQueue Q = new PQueue();

	//here check the size of bsum_claim array then have a loop
	//FIRST LEVEL NODES R CREATED WITH TILE 0 TO TILEN NOT INTHE ORDER OF SORTED BSUM ARRAY
	cout<<"Creating the first level nodes"<<endl;
     for(short i=0;i<nofsrc_traffic;i++)
     {
		 //if(i==8)
		  //cout<<"debug Hello"<<endl;

		 pimplementation_graph pNode = new Implementation_Graph(i);
		 cout<<"i:Pnode->illegal:"<<i<<":"<<pNode->illegal<<endl;
		 //getch();


		 if(pNode==NULL)
		 {

		 cout<<"out of memory implementaionbbmapping"<<endl;
		 //getch();
		 }
		 else
		{

		   if(pNode->illegal == 1)
		   {  delete pNode;
		    cout<<"ifafter delete"<<endl;
		   // getch();
		   }
		 else{
			cout<<"node created"<<endl;
			if(gubcost > pNode->ubc)  
						{ gubcost=pNode->ubc;
						  gubcost_stage=pNode->stage;
						  gcost=pNode->cost;
						copy_global_data();
						  
						}
			//if(gcost > pNode->cost) gcost=pNode->cost;			
			Q->insert(pNode);nofnodes++;
			//Q->traverse();
			//getch();
			}//else
		 }//else
		 //Q->insert(pNode);
      }//for

        Q->traverse();
	pimplementation_graph bestmapping=NULL;
	int previndex=0;
      while(!Q->empty())
	{	
      		pimplementation_graph parent=Q->next();
		previndex=0;
		cout<<"Queue length------------>"<<Q->length<<endl;
		cout<<"global ubc::global cost"<<gubcost<<"::"<<gcost<<"::::"<<endl<< "parent cost ::ubc::lbc:illegal::***";
		cout<<parent->cost<<":"<<parent->ubc<<":"<<parent->lbc<<":"<<parent->illegal<<endl;
      		if((parent->lbc - gubcost) > 0.00005)
                {   cout<<"parent lbc > gubc"<<endl; 
			cout<<"delete parent"<<endl;
 			delete parent;
                        continue;
			//flag =1;
		}
		
		else if(Q->length < pqsize )
               	{    
			insertall:			
			for(int i=0;i< nofsrc_traffic;i++)
			{
				if(parent->ACG.Bsum_claim_array[i][2]==0)
				{
					cout<<"Q->length ::: value of i -> "<<i<<endl;                                     
					pimplementation_graph child=new Implementation_Graph(parent,i);
				     if(child == NULL)
				     {  cout<<"null pointer while creating child"<<endl; }
				     else
				     { //cout<<"bef if child lbc n ubc "<< child->lbc <<":"<<child->ubc<<endl;
					
					
					if((child->lbc -  child->ubc ) > 0.00005)
		   			{      cout<<"child lbc is greater than ubc"<<endl;
						cout<<"child delete "<<endl;  
						delete child;continue; }
					else if((child->lbc - parent->ubc ) > 0.00005)
					{cout<<"child lbc greter than parent ubc ::::::"<<endl;cout<<"child deleted"; 
						delete child; continue;}
					 else if((child->lbc - gubcost) > 0.00005)
					{      cout<<"child lbc is greater than global ubc"<<endl;
						cout<<"child delete "<<endl;  
						delete child; continue;}

		 			else{
						//cout<<"node created"<<endl;
						
						
						if((gubcost > child->ubc)) 
						{ gubcost=child->ubc;
						  copy_global_data();
						}
						
						
						cout<<"stage checing against nofsrc_traffic--"<<child->stage<<endl;
						if(child->stage == nofsrc_traffic)						
						{ 	
							//gcost=child->cost;
							//if(gcost < gubcost) gubcost=gcost;
							cout<<"satage reached"<<endl;
							//if(child->cost < gcost)
							//{ gcost=child->cost;
							cout<<"best mapping _______________"<<bestmapping  <<endl;
							if(bestmapping)
								{if(bestmapping->cost > child->cost)
									bestmapping=child;}
                                                        else bestmapping=child;
							
						}			
						else 
						{
							Q->insert(child);nofnodes++;
     							cout<<"child inserted : Q Length--------- "<<Q->length<<endl;
							//Q->traverse();							
							previndex=i;
							if(Q->length >= pqsize )
							{cout<<"goto slective insert"<<endl; goto selectiveinsert;}
					    	 }//else						     
		 			}//else		                                     
		   		}//if
			}//for of srctraffic
		}	//continue;

		}//if Q->len
		
		else 
                {
			selectiveinsert:
				//if(parent->ubc == gubcost) goto insertall;				
				if((parent->ubc - gubcost) > 0.00005 )
				{ cout<<"sel parent deleted"<<endl;
					delete parent; continue ; }
				
				int index=parent->bestuperbound();
				if(index <= previndex)
				{ cout<<"sel_index parent deleted ";
				delete parent ;continue;}

				pimplementation_graph child=new Implementation_Graph(parent,index);

				 if(child == NULL)
				 {  cout<<"null pointer while creating child"<<endl; }
				 else
				 { //cout<<"bef if child lbc n ubc "<< child->lbc <<":"<<child->ubc<<endl;
					
					
					if((child->lbc -  child->ubc) > 0.00005 )
		   			{  cout<<"selecetive child lbc is greater than ubc"<<endl;
						cout<<"selc child delete "<<endl;  
						delete child; continue;}
					else if((child->ubc - parent->ubc ) > 0.00005) 
					{cout<<"selectvi child ubc greter than parent ubc ::::::"<<endl; delete child;continue;}
 					else if((child->lbc - child->ubc ) > 0.00005){ cout<<"child lbc > ubc:: "<<endl; delete child; continue;}
					else if((child->lbc - gubcost) > 0.00005)
					{      cout<<"sel child lbc is greater than global ubc"<<endl;
						cout<<"sel child delete "<<endl;  
						delete child; }

		 			else{
						//cout<<"node created"<<endl;
						
						
						if((gubcost > child->ubc)) 
						{ gubcost=child->ubc;
						  gubcost_stage=child->stage;
						   copy_global_data();
						  //gcost=child->cost;
						}
						//if(minlbc > child->lbc) minlbc=child->lbc;
						//if(gcost > child->cost) gcost=child->cost;						
						
						cout<<"selec stage checing against nofsrc_traffic--"<<child->stage<<endl;
						if(child->stage == nofsrc_traffic)						
						{ 	
							//gcost=child->cost;
							//if(gcost < gubcost) gubcost=gcost;
							cout<<"satage reached"<<endl;
							//if(child->cost < gcost)
							//{ gcost=child->cost;
							cout<<"best mapping _______________"<<bestmapping <<endl;
							if(bestmapping)
								{if(bestmapping->cost > child->cost)
									bestmapping=child;}
                                                        else bestmapping=child;
							
						}			
							else 
						{
							Q->insert(child);nofnodes++;
     							cout<<"selective child child inserted : Q Length--------- "<<Q->length<<endl;
							//Q->traverse();
							//if(Q->length >= pqsize ) goto selectiveinsert;
					        }//else						     
		 			    }//else
					}//else of child created		

		}// else of  selective insert
       		delete parent;

  }//while Q->empty();


	cout<<"************************nofnodes ::---->"<<nofnodes<<endl;
  if(bestmapping)
  {	cout<<"BEST Mapping"<<endl;

	cout<<"COST -------> "<<bestmapping->cost<<endl<<"UBC------------> "<<bestmapping->ubc<<endl<<"LBC----------->"<<bestmapping->lbc<<endl;
       for(int bi=0;bi<bestmapping->count; bi++)
       {  
         for(int j=0;(bestmapping->path_array[bi][j] != -1);j++)       
		cout<<bestmapping->path_array[bi][j] <<"::";
         cout<<endl;
        }
      cout<<"********************************8spanning tree**************************:"; 
      bestmapping->Create_Minimum_Spanning_tree3();
	bestmapping->create_routing_files();
	bestmapping->create_topology_files();

	//write path array to file 
	int i,j;
	ofstream file;
	file.open("path_array.txt");
	//for(i=0;bestmapping->path_array[i][0]!=-1;i++)
	for(i=0;i<bestmapping->count;i++)	
	{
		file << bestmapping->path_array[i][0] <<"\t     ";
		file << bestmapping->path_array[i][1] <<"\t\t";
		file << (!bestmapping->path_array[i][2]) <<"\t\t";
		for(j=3;bestmapping->path_array[i][j]!=-1;j++){
			file << bestmapping->path_array[i][j] << "\t" ;
			}
		file << "\n";
		}
	for(i=0;i<escape_count;i++)
		{file << escape_path_array[i][0] <<"\t     ";
		file << escape_path_array[i][1] <<"\t\t";
		file << (!escape_path_array[i][2]) <<"\t\t";
		for(j=3;escape_path_array[i][j]!=-1;j++){
			file << escape_path_array[i][j] << "\t" ;
			}
		file << "\n";
		}
	file.close();
		  
	       
	  
		
	//creare bw_traffice file
	cout<<"BW Traffic file "<<endl;
	ofstream file1,file2;
	file1.open("topology/application.config");
	
	file2.open("topology/not_source.config");	
	for(int i=0;i<size1;i++)
	{
		int count = 0;
		for(int j=0;j<size1;j++)
		{
			if(BW_Claim_matrix[i][j] != -1)
			{
											
				char filename[10];
				char filename1[10];
				char path[50];
				int load=0;
				strcpy(path,"bw_traffic/");
				//source_tile = gene_array[id].Read_source();
				sprintf(filename, "%d",i);
				strcat(filename,"_");
				sprintf(filename1, "%d",count);
				strcat(filename,filename1);
				strcat(path,filename);
				//strcat(path,".txt");
				cout<<"Path"<<path<<endl;
				file.open(path);
				if(!file.is_open())
					cout<<"IF block";
				else
				{
					load = BW_Claim_matrix[i][j];//--->phase 2
				//destination_tile = gene_array[id].Read_desti();	
				file<<"PKT_SIZE 8\n";
				file<<"LOAD "<<load<<"\n";
				file<<"DESTINATION FIXED "<<j<<"\n";
				file<<"FLIT_INTERVAL 2";
				file.close();
				count++;
				}
				
				
			}//(BW_Claim_matrix[i][j] != -1)
			
		}//for(int j=0;j<size1;j++)
		if(count !=0)
		{
			file1<<i<<" BWCBR"<<count<<".so\n";
			
		}
		else if(count == 0) 
		{
			
			file2<<i<<" "<<"NOT A SOURCE TILE"<<"\n";

			
		}


	}//for(int i=0;i<size1;i++)			
	file1.close(); file2.close();	file.close();		

	}//if(best mapping)
	else if(gd.flag==1)
        {  cout<<"glabal dt available";
        }
	else 
        cout<<"No Best mapping"<<endl;


}//closinf bpmppimg

//topoloy : linklength and topology files 

void Implementation_Graph::create_topology_files()
{
	ofstream file;
	ofstream file1;
	file.open("topology/topology_IR.config");
	file1.open("topology/link_length");
	for(int i=0;i<size1;i++){
		file<<i<<" ";
		for(int j=0;j<size1;j++){
			
				//file1 << i << "\t" << j << "\t" << Graph_Const_Matrix[i][j].length << "\n";
				if(port_allocated_matrix[i][j]==1){
				file1 << i << "\t" << j << "\t" << MHG.M_hattan_matrix[i][j].length<< "\n";
				file<<j<<" ";	}	
				else
				{
				file1 << i << "\t" << j << "\t" <<"-1"<< "\n"		;
				//file<<j<<" ";	
				}
				
		}
		file<<"-1\n";
	}

	file.close();
	file1.close();
}


void Implementation_Graph::create_routing_files()
{
	int i,j;
	
	//loop continues for each source-destination pair present in path_array
	cout<<"value of count ie no of entries in path array->"<<count<<endl;	
	for(i=0;i<count;i++){
		int source=path_array[i][0];
		int dest=path_array[i][1];
		
		//Traversing the path in path_array and updating the routing files of encountered tiles
		for(j=3;path_array[i][j+1]!=-1;j++){
			ofstream file;
			char filename[10];
			char path[50];
			sprintf(filename, "%d", path_array[i][j]);
			strcat(filename,".txt");
			strcpy(path,"routing_table/");
			strcat(path,filename);
			cout<<"path array to rtalble file name:"<< path<<":I->"<<i<<"::j->"<<j<<endl;
			//Opening the file "tile_id.txt" and writing Source, Destination, Next_tile and Priority to it
			file.open(path,ios::app);
			file << source <<"\t";
			file << dest <<"\t";
			file << path_array[i][j+1]<<"\t";	//Next tile for current tile_id
			file <<	(!path_array[i][2])<<"\n";	//Priority is negation of visited status, "0" for shortest path and "1" for up_down 
			
			file.close();		
		}
	}
	for(i=0;escape_path_array[i][0]!=-1;i++){
		int source=escape_path_array[i][0];
		int dest=escape_path_array[i][1];
		
		//Traversing the path in path_array and updating the routing files of encountered tiles
		for(j=3;escape_path_array[i][j+1]!=-1;j++){
			ofstream file;
			char filename[10];
			char path[50];
			sprintf(filename, "%d", escape_path_array[i][j]);
			strcat(filename,".txt");
			strcpy(path,"routing_table/");
			strcat(path,filename);
			cout<<"escape path array to rtalble file name:"<< path<<":I->"<<i<<"::j->"<<j<<endl;
			//Opening the file "tile_id.txt" and writing Source, Destination, Next_tile and Priority to it
			file.open(path,ios::app);
			file << source <<"\t";
			file << dest <<"\t";
			file << escape_path_array[i][j+1]<<"\t";	//Next tile for current tile_id
			file <<	(!escape_path_array[i][2])<<"\n";	//Priority is negation of visited status, "0" for shortest path and "1" for up_down 
			
			file.close();		
		}	//escape for
	}	//EScape for


	// create routing table files for NIRGAM
	int count1=-1;
	char str1[50];
	for(int i=0;i<size1;i++){
		ifstream file1;
		char filename1[10];
		char path1[50];
		sprintf(filename1, "%d", i);
		strcat(filename1,".txt");
		strcpy(path1,"routing_table/");
		strcat(path1,filename1);
		file1.open(path1,ios::in);
		if(!file1.is_open()){
			
			char filename2[10];
			char path2[50];
			ofstream file2;
			sprintf(filename2, "%d", i);
			strcat(filename2,".txt");
			strcpy(path2,"opt_for_nirgam/rtable/");
			strcat(path2,filename2);
			file2.open(path2,ios::out);
			file2 <<"NUM_RTABLE_ENTRIES "<<"0"<<"\n";
			file2.close();
			file1.close();
			continue;
			//cout<<"Error opening router file for NIRGAM \n";
			//exit(1);
		}
		count1=-1;
		while(!file1.eof()){
			file1.getline(str1,50);
			count1++;
			}
		char filename3[10];
		char path3[50];
		ofstream file3;
		sprintf(filename3, "%d", i);
		strcat(filename3,".txt");
		strcpy(path3,"opt_for_nirgam/rtable/");
		strcat(path3,filename3);
		file3.open(path3,ios::out);
		file3 <<"NUM_RTABLE_ENTRIES "<<count1<<"\n";
		file3 <<"#source\tdestination\tnexttile\tpriority(0_for_shortest_path(virtual_channel_0,1,2)_&_1_for_deadlock_free(virtual_channel_3)_path)\n";
			
		if(!file1.is_open()){
			cout<<"Error opening router file for NIRGAM \n";
			exit(1);
		}
		
		file1.clear();//clears the eof flag
		file1.seekg(0,ios::beg);
		if(file1.eof()) cout <<"already end of file\n";
		while(!file1.eof()){
			char str2[50];
			file1.getline(str2,50);
			file3<<str2<<"\n";
		}
	file1.close();
	file3.close();
	}//end of for

/*******************NAV NIRGAM MODIFICATION (routing tables) end ******/




} // crearte routing files


//////////////////////////////////////
// main function
//////////////////////////////////////
int main()
{
	//clrscr();
	cout<<"SSSSSSSSSSSSSSSSSSSSSSSSS";
	if(BW_Claim_matrix=new int[size1][size1+1]){} //throws exception if specified memory not available
			else{
			cout<<"\nOut of memory in allocation of global BW_Claim_Matrix\n";
			exit (1);
			}
	

	create_BW_matrix();
 
	allocate_memory();
	initialise_global_data();
        cout<<"nofsrc traffic::"<<nofsrc_traffic <<endl<<"noftraffic::"<<noftraffic<<endl;
	bbmapping();

	return 0;
}         //main

