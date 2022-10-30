#include <time.h>
#include "input.h"
/***************************************************************************************************************************
Truth Table
****************************************************************************************************************************/
//clock_t start_t, end_t, total_t;
   	
//start_t = clock();


int NOTG[5] = {1,0,DC,Db,D};
int ANDG[5][5]={{0,0,0,0,0},
		{0,1,DC,D,Db},
		{0,DC,DC,DC,DC},
		{0,D,DC,D,0},
		{0,Db,DC,0,Db}};
int ORG[5][5]={{0,1,DC,D,Db},
               {1,1,1,1,1},
               {DC,1,DC,DC,DC},
               {D,1,DC,D,1},
               {Db,1,DC,1,Db}};
int ACT[3][3]={{0,D,DC},
               {Db,1,DC},
               {DC,DC,DC}};

int ReadPat(FILE *Pat, PATTERN *Patterns){
	char line[Mlin];// variable line with mlin max characters
	int i=0,n; // i=row ,n =column
	// read line by line
	while(!feof(Pat)){
		if(fgets(line,Mlin,Pat)!=NULL){//reads the pattern in the stream
			for(n=0;n<strlen(line);n++){//strlen=measures the string length
				if(line[n]=='1')			Patterns[i].Pat[n]=1;
				else if(line[n]=='0')			Patterns[i].Pat[n]=0;
				else if(line[n]=='x'||line[n]=='X')	Patterns[i].Pat[n]=DC;
			}
			i++;
			if(i==100) break;
		}
	}

	return i; //total number of patterns
}

//int ReadFau(FILE *ffau, FAULT *stuck) {return 0;}
int readfault(FILE *folt,FAULT *stuck){
char line[Mft];
char arr[2][Mft];int i=0;
while(fgets(line,Mft ,folt)!=NULL){////reads the faults in the stream
char *token = strtok(line,"/");// / dividing into 2parts
int j=0;
while(token!=NULL)
{
strcpy(arr[j],token);
token= strtok(NULL,"/");
j++;
}
stuck[i].g=atoi(arr[0]);
stuck[i].saf=atoi(arr[1]);
printf("%d\n",stuck[i].g);
printf("%d\n",stuck[i].saf);
i++;
}
return i;
}

void LogicSim(GATE *Node, PATTERN *Patterns, int Tgat, int Tpat, FILE *Res)
{
	int i,j,k; //i=total patterns,j=total gates

	for(i=0;i<Tpat;i++){
		k=0;
	printf("%d ", i);
		for(j=0;j<=Tgat;j++){//printf("%d",j);getchar();
			if(Node[j].Type !=0){
				if(Node[j].Type==INPT){
					Node[j].Val = Patterns[i].Pat[k];  //putting the input vectir in PI
					k++;
				}
				else{
					Gout(Node, j); //LS for gates
				}
				if(Node[j].Po) {
					printf("%d",Node[j].Val);     
					if(Node[j].Val<2)  fputc(Node[j].Val+'0' , Res);// write the char c into file res,converting int to char
					else if (Node[j].Val==2)  fputc('x', Res);
				}
			}
		}

//PrintGats(Node,Tgat); 
              printf("\n");
	 
		fputc('\n' , Res);

		
	}
}


void FaultSimulation(GATE *Node, PATTERN *Patterns, FAULT *fault, int Tgat, int Tpat, int Tfault, FILE *Res)
{
	int i,j,k,a, detect=0,fdp=0,p; //i=total patterns,j=total gates
	int x, saf;
	
	for(a=0;a<Tfault;a++){
		x=fault[a].g; saf=fault[a].saf;
 		Node[x].Fval = saf;
		fprintf(Res,"%d/%d\n",x,saf);
		for(i=0;i<Tpat;i++){
			k=0;
			printf("%d ", i);detect=0;
			for(j=0;j<=Tgat;j++){
				if(Node[j].Type !=0){
					if(Node[j].Type==INPT){
						Node[j].Val = Patterns[i].Pat[k];
						k++;
					}
					else{
						Gout(Node, j);
					}
					if(Node[j].Fval!=6){
						Node[j].Val=ACT[Node[j].Fval][Node[j].Val];
					}
					if(Node[j].Po) {
                                             
						if((Node[j].Val==3)||(Node[j].Val==4)){
							detect=1;								
							}
					
					}
				}
		
			}
			/*printf("\n");
			PrintGats(Node,Tgat);  
			fputc('\n' , Res);*/
			
			
				for(p=0;p<k;p++) fprintf(Res,"%d", Patterns[i].Pat[p]);
				if(detect)fprintf(Res,"  Detected");
				else fprintf(Res,"  Undetected");	
				fprintf(Res,"\n");

			
		}
		fprintf(Res,"\n");

		
		//printf("ERROR");getchar();
		Node[x].Fval=6;
	}
}


void Gout(GATE *Node, int a)
{
	LIST *lfin;
	int val; // local variable
	if(Node[a].Type==FROM){
		lfin=Node[a].Fin;
		while(lfin!=NULL){
			Node[a].Val=Node[lfin->Id].Val; 
			lfin=lfin->Next;
		}
	}
	else if(Node[a].Type==BUFF){
		lfin=Node[a].Fin;
		while(lfin!=NULL){
			Node[a].Val=Node[lfin->Id].Val; 
			lfin=lfin->Next;
		}
	}
	else if(Node[a].Type==NOT){
		lfin=Node[a].Fin;
		while(lfin!=NULL){
			Node[a].Val=NOTG[Node[lfin->Id].Val]; 
			lfin=lfin->Next;
		}
	}
	else if(Node[a].Type==AND){
		lfin=Node[a].Fin;
		val=Node[lfin->Id].Val;// non control for AND gate
		lfin=lfin->Next;
		while(lfin!=NULL){
			val=ANDG[val][Node[lfin->Id].Val]; 
			lfin=lfin->Next;
		}
		Node[a].Val=val;
	}
	else if(Node[a].Type==NAND){
		lfin=Node[a].Fin;
		val=Node[lfin->Id].Val;
		lfin=lfin->Next;// non control for AND gate
		while(lfin!=NULL){
			val=ANDG[val][Node[lfin->Id].Val]; 
			lfin=lfin->Next;
		}
		Node[a].Val=NOTG[val];
	}
	else if(Node[a].Type==OR){
		lfin=Node[a].Fin;
		val=Node[lfin->Id].Val;// non control for OR gate
		lfin=lfin->Next;//lfin=Node[a].Fin;
		while(lfin!=NULL){
			val=ORG[val][Node[lfin->Id].Val]; 
			lfin=lfin->Next;
		 }
		Node[a].Val=val;
	}
	else if(Node[a].Type==NOR){
		lfin=Node[a].Fin;
		val=Node[lfin->Id].Val;// non control for OR gate
		lfin=lfin->Next;
		while(lfin!=NULL){
			val=ORG[val][Node[lfin->Id].Val]; 
			lfin=lfin->Next;
		 }
		Node[a].Val=NOTG[val];

	}
	
	else if(Node[a].Type==XOR) {printf("ERROR: XOR GATE FOUND\n");exit(0);}
	else if(Node[a].Type==XNOR){printf("ERROR: XNOR GATE FOUND\n");exit(0);}
}

/***************************************************************************************************/



