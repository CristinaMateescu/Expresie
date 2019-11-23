//Mateescu Cristina-Ramona, 311CB, tema3
	typedef struct informatie
	{char* string;
	 int type;   // 0 for operator , 1 for variable
	}Info,*Tinfo;

	typedef struct nod
	{ Tinfo info;
	  struct nod *st,*dr;
	}TNod, *TArb, **Arb;


	typedef struct variabila
	{ char *name;
	  int  value;
	} Var,*TVar;


	typedef struct celulag
  	{
	void *info;
	struct celulag *urm;

  	} Celula, *TLista, **ALista;

	
