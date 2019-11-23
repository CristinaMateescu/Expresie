//Mateescu Cristina-Ramona, 311CB, tema3	
	#include <stdio.h>
	#include "info.h"
	#include <string.h>
	#include <stdlib.h>
	#include <math.h>

	TVar AlocVar (char *name, int value)

	{ TVar var=(TVar)malloc(sizeof(Var));
	  	if ( !var ) return NULL;
	  var->name=(char *)malloc((strlen(name)+1)*sizeof(char));
	  	if ( !var->name ) 
			{free(var);return NULL;}
	  strcpy(var->name,name);
	  var->value=value;
	  return var;	
	}

	
	TLista AlocCel ( void* info )

	{ TLista aux=(TLista)malloc(sizeof(Celula));
	  	if( !aux ) return NULL;
	  aux->urm=NULL;
	  aux->info=info;
	  return aux;
	}

	
	Tinfo AlocInfo (char * string,int type )
	
	{ Tinfo info=(Tinfo)malloc(sizeof(Info));
	  if(!info) return NULL;
	
	  info->string=(char*)malloc((strlen(string)+1)*sizeof(char));
	  if ( !(info->string) ) 
		{free(info);
		 return NULL;}
	  strcpy(info->string,string);
	  info->type=type;
	  return info;


	}
	
	TArb AlocArb ( Tinfo info)
	
	{ TArb arb=(TArb)malloc(sizeof(TNod));
	  if(!arb)return NULL;
	  
	  arb->info=info;
	  arb->st=NULL;
	  arb->dr=NULL;
	
	 return arb;
	}
	


	void citire_arb ( FILE * in, TArb *arbore)

	{ 
	  //1.Citire expresie
	  char c,string[11];
	  c=' ';
	  char plus[2]="+";
	  char minus[2]="-";
	  char inmultire[2]="*";
	  char radical[5]="sqrt";
	  char impartire[2]="/";
	  char putere[2]="^";

	  
	  fscanf ( in, "%s%c", string,&c);
	  int type;
	  //type=0 - operator
	  //type=1 - variabila

	  //2.Verific daca e operator
	  if (strcmp(string,plus)==0 ||
	      strcmp(string,minus)==0 ||
	      strcmp(string,inmultire)==0 ||
	      strcmp(string,radical)==0 ||
	      strcmp(string,impartire)==0 ||
	      strcmp(string,putere)==0)
			type=0;
	  else type=1;


	  //Se aloca memorie pentru campul info
	  Tinfo info=AlocInfo(string,type);

	  if (info==NULL) return;

	  TArb nod=AlocArb(info);
	
	  if (!nod)
		{ free(info->string);
	 	  free(info);
		  return;
		}
	
	  //Se adauga in arbore
	  (*arbore)=nod;	
	
	  //Daca e operator - se apeleaza functia pentru st si dr
	  //cu exceptia cazului cand operatorul este 'sqrt'
	  if ( type == 0 )
		{if ( strcmp( ((Tinfo)(nod->info))->string,radical ) == 0 )
		  citire_arb ( in, &(nod->st) );
		else
		{ citire_arb ( in, &(nod->st) );
		  citire_arb ( in, &(nod->dr) );
		}}
	
	}

	
	int eval ( FILE *out, TArb arbore, int *err ,TLista variables)
	{ char plus[2]="+";
	  char minus[2]="-";
	  char inmultire[2]="*";
	  char radical[5]="sqrt";
	  char impartire[2]="/";
	  char putere[2]="^";
	  if ( (*err) )
		{ //1. Daca este o variabila 
		  if( ((Tinfo)(arbore->info))->type == 1 )
		  	{ //1.1 Daca este numar
			  if (  ( ((Tinfo)(arbore->info))->string[0] >= '0' &&
			       ((Tinfo)(arbore->info))->string[0] <= '9' ) ||
				(  strlen( ((Tinfo)(arbore->info))->string )>1 && 
				 ((Tinfo)(arbore->info))->string[0]=='-' ))
				{ int s=0,i=0;
				  if (((Tinfo)(arbore->info))->string[0]=='-')i++;
				  //1.1.1 Transformare string in int
				  while (i<strlen(((Tinfo)(arbore->info))->string)) 
					{ s=s*10+(((Tinfo)(arbore->info))->string[i]-'0');
					  i++;
					}
				  if (((Tinfo)(arbore->info))->string[0]=='-')return s*(-1);	
				  return s;
				 
				}
			  else  { //1.2 Daca este variabila
				  //Se cauta in lista de variabile
				  TLista p=variables;
				  while ( p!=NULL &&
					  strcmp( ((TVar)(p->info))->name,
					  ((Tinfo)(arbore->info))->string ) !=0  ) p=p->urm;

				  //1.2.2 Daca nu exista variabila
				  if ( !p ) 
					{fprintf( out , "Variable %s undeclared\n", 
					 ((Tinfo)(arbore->info))->string );
					 (*err)=0;
					 return 0;
					}
				  //1.2.3 Daca exista, returnez valoarea ei
				  else return ((TVar)(p->info))->value;
				}
			}

		  //2. Daca este operator
		  else if( ((Tinfo)(arbore->info))->type == 0 )
			{ //PASI
			  //1. st
			  int st=eval(out,arbore->st,err,variables);
			  
			  if( (*err)==0 ) return 0;
			  
			  //1.1. Daca operatorul este radical
			  if ( strcmp(((Tinfo)(arbore->info))->string,radical)==0 )
				{//1.1.1. verific ca valoarea returnata sa nu fie negativa
				 if ( st < 0 )
					{ (*err) = 0;
					  fprintf( out, "Invalid expression\n");
					  return 0;
					}
				int xradical=sqrt(st);
				return xradical;

				}

			  //2. dr
			  int dr=eval(out,arbore->dr,err,variables);
			  
				
			  if( (*err)==0 ) return 0;
			
			  //2.1 Daca operatorul este impartire
			  if ( strcmp(((Tinfo)(arbore->info))->string,impartire)==0 )
				{//2.1.1 Verific sa nu fie impartire la 0
				if (dr == 0 )
					{ (*err) = 0;
					  fprintf( out, "Invalid expression\n");
					  return 0;
					}

				return st/dr;
				}
					
			  //3. returnez stanga "operator" dreapta
			  if ( strcmp(((Tinfo)(arbore->info))->string,plus)==0 ) 
					{return st+dr;}
			  if ( strcmp(((Tinfo)(arbore->info))->string,minus)==0 ) 
					 {return st-dr;}
			  if ( strcmp(((Tinfo)(arbore->info))->string,inmultire)==0 ) 
					{return st*dr;}
			  if ( strcmp(((Tinfo)(arbore->info))->string,putere)==0 ) 
					{int xputere=pow(st,dr);
					return xputere;}


			}
		}

	  else 
		return 0;	

	
	}

	void elib (TLista * variables)
	{ TLista p=(*variables);
	  while (p!=NULL)
		{ TLista aux=p;
		  p=p->urm;
		  free( ((TVar)(aux->info))->name );
		  free(((TVar)(aux->info)));
		  free(aux);
		}	

	}


	void elib_arb ( Arb arbore )
	{ 
	  char radical[5]="sqrt";
	  TArb nod=(*arbore);
	  if( ((Tinfo)(nod->info))->type == 0 )
		{if ( strcmp(((Tinfo)(nod->info))->string,radical)==0 )
		      elib_arb(&((*arbore)->st));
		 else 
		      {elib_arb(&((*arbore)->st));
	 	       elib_arb(&((*arbore)->dr));
			}
		}
	  
	  free( ((Tinfo)((*arbore)->info))->string );
	  free ( (*arbore)->info );
	  free ( (*arbore) );	
	  (*arbore)=NULL;

	}


	int main (int argc,char *argv[])
	
	{
	 FILE *in=fopen( argv[1],"rt" );
	 FILE *out=fopen( argv[2],"wt" );

	 int var_nr;
	 TLista variables=NULL,ant;
	 
	 //1.Se citeste numarul de variabile
	 fscanf( in, "%d", &var_nr );
	
	 //2.Se construieste lista de variabile
	 int i;
	 for ( i=1; i<=var_nr; i++ )

	 	{ char name[11]; 
		  int val;

		  fscanf(in, "%s = %d",name,&val);
		  TVar var=AlocVar(name,val);
		  if (var==NULL)
			{printf("Nu se poate construi lista de variabile");
			 elib(&variables);
			 return 0;}

		  TLista aux=AlocCel(var);
		  if (!aux)
			{free(var->name);
			 free(var);
			 printf("Nu se poate construi lista de variabile");
			 elib(&variables);
			 return 0;
			}

		  if ( variables == NULL ) {variables = aux;
					    ant=variables;}
		  else {ant->urm=aux;
			ant=ant->urm;}
	
		}
	
	//3.Se citeste numarul de expresii
	int expr_nr;
	fscanf( in, "%d", &expr_nr );
	
	//4.Se va citi pe rand fiecare expresie si se va construi arborele
	while ( expr_nr > 0 )
		{//Pornesc cu un arbore NULL 
		 TArb arbore=NULL;

		 //Citire si constructie arbore
		 citire_arb( in,&arbore );
		 

		//Evaluare expresie
		 int err=1;
		 //err are valoarea 1 daca nu exista nicio eroare
		 //err are valoarea 0 daca una dintre variabile nu exista
		 //		  sau daca o expresie este invalida
 		 int rez=eval (out,arbore,&err,variables);
		
		 if ( err != 0 )fprintf(out,"%d\n",rez);

		elib_arb(&arbore);
		expr_nr--;
		}	
	elib(&variables);
	variables=NULL;
	fclose(in);
  	fclose(out);
	return 0;
	}
