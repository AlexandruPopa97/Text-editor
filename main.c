#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct node
{
	char c;
	struct node *next;		//Lista mea este una dublu inlantuita.
	struct node *prev;
};

typedef struct node nod;

struct lista_capete
{
	nod *capCurent;
	nod *capNext;
};

typedef struct lista_capete capete;

struct curs
{
	int pozitie_cursor;
	struct curs *next;
};
typedef struct curs cursor;

struct undoRedo
{
	capete *capCopie;
	cursor *capCursor;			//Stiva mea contine 2 liste, una cu capete de liste(copii ale listei care se 
	unsigned int lenght;		//salveaza dupa fiecare comanda executata) si o lista in care salvez pozitiile cursorului.
};

typedef struct undoRedo stiva;

void citire_text(FILE *f,char nume[],nod **CapDeLista)
{	
	nod *p,*q;
	char c;
	f = fopen(nume,"rb");
	if(f == NULL)
	{
		printf("Fisierul date.in nu a putut fi deschis!\n");	//Citirea caracterelor din text si initializarea listei
		exit(1);												//se face in aceasta functie care va returna prin parametru
	}															//capul listei.
	*CapDeLista = (nod*)malloc(sizeof(nod));					//Capul listei nu contine caracter pentru a putea insera si 
	p = (nod*)malloc(sizeof(nod));								//la inceputul listei.Deci caractere incep de la al 2-lea nod
	if((c = fgetc(f)) != EOF)
	{
		(*CapDeLista)->next = p;
		(*CapDeLista)->prev = NULL;
		p->prev = *CapDeLista;
		p->c = c;
		p->next = NULL;
	}
	while((c = fgetc(f)) != EOF)
	{
		q = (nod*)malloc(sizeof(nod));
		q->c = c;
		p->next = q;
		q->next = NULL;
		q->prev = p;
		p = q;
	}
	fclose(f);
}

void move(nod **cursor,int n)
{	
	int i;
	if(n > 0)
	{
		for(i=0;i<n;i++)
		{	
			if((*cursor)->next != NULL)			//Aceasta functie realizeaza comanda move N.
			(*cursor) = (*cursor)->next;		//Deci muta cursorul cu n pozitii si il returneaza prin parametru.
		}
	}
	else if(n < 0)
	{
		for(i=0;i<-n;i++)
		{
			if((*cursor)->prev != NULL)
			(*cursor) = (*cursor)->prev;
		}
	}
}

void insert_string(nod **cursor, char *string)
{
	int i,n;
	nod *p;
	n = strlen(string);
	for(i=0;i<n;i++)
	{
		p = (nod*)malloc(sizeof(nod));
		if((*cursor)->next != NULL)
		{
			p->next = (*cursor)->next;				//Aceasta functie realizeaza comanda insert String
			(*cursor)->next->prev = p;				//stringul este considerat un vector de caractere
			(*cursor)->next = p;					//se creeaza noduri si se introduce pe 2 cazuri:
			p->prev = *cursor;						//1)Atunci cand se insereaza la final.
			p->c = string[i];						//2)Atunci cand se insereaza in interiorul listei(inclusiv la inceput)
			(*cursor) = p;
		}											//si se vor reface legaturile.
		else										//De asemenea cursorul de muta la finalului stringului introdus.
		{
			p->next = NULL;
			p->c = string[i];
			(*cursor)->next = p;
			p->prev = (*cursor);
			(*cursor) = p;
		}
	}
}

void sterge_n(nod *cursor,int n)
{
	int i;
	nod *aux;
	for(i=0;i<n;i++)
	{
		if(cursor->next->next)
		{
			aux = cursor->next;
			cursor->next->next->prev = cursor;			//Aceasta functie sterge N noduri din lista de la pozitia curenta
			cursor->next = cursor->next->next;			//a cursorului, aceasta fiind deschimbata.
			free(aux);									//Stergerea se realizeaza tot pe 2 cazuri:
		}												//1)Cand se sterge nodul final.
		else if(cursor->next)							//2)Cand se sterge un nod din centrul liste si se refac legaturile.
		{											
			aux = cursor->next;
			cursor->next = NULL;
			free(aux);
			break;
		}
	}
}

void copy_n(nod *cursor,int n,char **buffer)
{
	int i;
	(*buffer) = (char*)malloc(n*sizeof(char));		//Functia copiaza N caractere sub forma unui sir.
	for(i=0;i<n;i++)
	{
		if(cursor->next != NULL)
		{
			(*buffer)[i] = cursor->next->c;
			cursor = cursor->next;
		}
	}
	(*buffer)[i]='\0';

}

void paste(nod **cursor,char buffer[])
{
	insert_string(cursor,buffer);			//Am vrut sa am si o functie cu numele paste.
}

void backspace(nod **cursor)
{
	nod *aux;
	if((*cursor)->c && (*cursor)->next != NULL)
	{	
		aux = (*cursor);
		(*cursor)->prev->next = (*cursor)->next;		//Functia realizeaza comanda backspace,modificand si corsorul
		(*cursor)->next->prev = (*cursor)->prev;		//se face pe 2 cazuri (ultimul nod/nod interior listei).
		(*cursor) = (*cursor)->prev;
		free(aux);
	}
	else if((*cursor)->c)
	{
		aux = (*cursor);
		(*cursor)->prev->next = NULL;
		(*cursor) = (*cursor)->prev;
		free(aux);
	}
}

stiva *creeaza_stiva()
{
	stiva *stack;
	stack = (stiva*)malloc(sizeof(stiva));					//Functia creeaza si initializeaza o stiva.
	stack->lenght = 0;										//Am folosit 2 stive pentru undo/redo care merg de multe ori.
	stack->capCopie = (capete*)malloc(sizeof(capete));		
	stack->capCopie->capCurent = NULL;
	stack->capCopie->capNext = NULL;
	stack->capCursor = NULL;
	return stack;
}

int copiaza_lista(nod *cap,nod **capcopy)
{
	nod *p,*q,*w;
	if(cap != NULL)
	{
	(*capcopy) = (nod*)malloc(sizeof(nod));				//In stivele mele de undo/redo am introdus copii ale listei
	p = (nod*)malloc(sizeof(nod));						//cu modificari aferente deci aveam nevoie de o functie care
	(*capcopy)->next = p;								//sa imi faca copiile respective.
	(*capcopy)->prev = NULL;							//Se creeaza un cap nou care va fi returnat si se aloca atatea 
	w = cap;											//noduri cate are si lista initiala, preluand si valorile acesteia.
	w = w->next;
	p->prev = (*capcopy);
	p->c = w->c;
	p->next = NULL;
	w = w->next;
	while(w != NULL)
	{
		q = (nod*)malloc(sizeof(nod));
		q->c = w->c;
		q->prev = p;
		q->next = NULL;
		p->next = q;
		p = q;
		w = w->next;
	}
	}

}

void push_stiva(stiva *stack,nod *cap,int pozitie_cursorr)
{
	nod *capDel,*p;
	cursor *crs;
	int i;
	capDel = (nod*)malloc(sizeof(nod));
	crs = (cursor*)malloc(sizeof(cursor));				//Functie ce realizeaza push in stive.
	copiaza_lista(cap,&capDel);							//Push-ul este dublu adica se adauga si in lista de capete de liste
	if(stack->capCursor == NULL)						//cat si in lista de cursoare.
	{
		crs->pozitie_cursor = pozitie_cursorr;
		crs->next = NULL;
		stack->capCursor = crs;
	}
	else 
		{	
			crs->next = stack->capCursor;
			crs->pozitie_cursor = pozitie_cursorr;
			stack->capCursor = crs;
		}
	if (stack->capCopie->capCurent == NULL)
	{
		stack->capCopie->capNext = NULL;
		stack->capCopie->capCurent = capDel;
	}
	else
	{
		stack->capCopie->capNext = stack->capCopie->capCurent;
		stack->capCopie->capCurent = capDel;
	}
	stack->lenght++;
}

int pop_stiva(stiva *stack,nod **cap,int *pozitie)
{
	if(stack->lenght > 0)
	{																		//Functia de pop din stiva.
		copiaza_lista(stack->capCopie->capCurent,&(*cap));					//Returneaza atat pozitia cursorului cat si capul
		(*pozitie) = stack->capCursor->pozitie_cursor;						//listei care va fi scoasa din stiva.
		if(stack->capCopie->capNext != NULL && stack->capCursor->next != NULL)
		{
			stack->capCopie->capCurent = stack->capCopie->capNext;
			stack->capCursor = stack->capCursor->next;
		}
	stack->lenght--;
	}
	else return 0;
}

void find_replace(nod *cap,char *cuvant1,char *cuvant2)
{
	int i,nr = strlen(cuvant1),k;
	nod *p,*q;
	p = cap;
	while(p->next != NULL)
	{
		if(p->next->c == cuvant1[0])					//Functia de find and replace.
		{												//Cauta cuvantul in lista de caractere si apoi apeleaza 2 functii:
			k = 1;										//functia de stergere pentru a sterge cuvantul cautat
			q = p->next->next;							//si functia de inserare pentru a-l inlocui cu cunvatul care trebuie
			for(i=1;q != NULL,q->next != NULL,i<nr;i++)	//introdus.
			{
				if(cuvant1[i] == q->c)
					k++;
				else break;
				q = q->next;
			}
			if (k == nr)
			{
				sterge_n(p,nr);
				insert_string(&(p),cuvant2);
			}

		}
		p = p->next;
	}
}

void free_mem(nod *cap,stiva *undo,stiva *redo)
{
	nod *p,*aux;
	int degeaba;
	p = cap;
	while(p)
	{
		aux = p;
		p = p->next;								//Functia asta dealoca toate nodurile pentru care am alocat memorie.
		free(aux);									//Inclusiv copiilor pe care le-am salvat in cele 2 stive undo/redo.
	}
	while(pop_stiva(undo,&cap,&degeaba) != 0)
	{
		p = cap;
		while(p)
		{
			aux = p;
			p = p->next;
			free(aux);
		}
	}
	while(pop_stiva(redo,&cap,&degeaba) != 0)
	{
		p = cap;
		while(p)
		{
			aux = p;
			p = p->next;
			free(aux);
		}
	}
}

int main(int argc,char ** argv)
{
	FILE *f;
	nod *CapDeLista,*cursor,*p;						//Aici declar variabilele pe care le voi folosi.
	char *buffer,*operatie,*deInserat;				//Declar si 2 stive una de undo si una de redo.
	char *cuvant,*cuvant_nou;						//Dupa deschiderea fisierului care contine operatiile ce trebuie facute
	cuvant = (char*)malloc(16*sizeof(char));		//incep sa citesc siruri de caractere, apoi acestea trec printr-o serie
	cuvant_nou = (char*)malloc(16*sizeof(char));	//de coparatii pentru a identifica operatia.
	int i,nr,pozitie_cursor = 0,poz2;				//In functie de operatie mai citesc parametrii(ex:numarul N sau stringul)
	stiva *undo,*redo;								//apoi salvez cate o copie inainte de executarea functiei in stiva
													//de undo,modific valoarea numerica a cursorului si apelez functia.
	citire_text(f,argv[1],&CapDeLista);
	cursor = CapDeLista;
	undo = creeaza_stiva();
	redo = creeaza_stiva();

	f = fopen(argv[2],"rt");
	if(f == NULL)
	{
		printf("Fisierul de operatii nu a putut fi deschis!\n");
		exit(1);
	}
	fscanf(f,"%d",&nr);
	while((fscanf(f,"%s",operatie)) != EOF)
	{
		if(strcmp(operatie,"del") == 0)
		{
			push_stiva(undo,CapDeLista,pozitie_cursor);
			fscanf(f,"%d",&nr);
			sterge_n(cursor,nr);
		}
		else if(strcmp(operatie,"move") == 0)
		{
		   push_stiva(undo,CapDeLista,pozitie_cursor);
			fscanf(f,"%d",&nr);
			pozitie_cursor = pozitie_cursor + nr;
			move(&cursor,nr);
		}
		else if(strcmp(operatie,"copy") == 0)
		{
			fscanf(f,"%d",&nr);
			copy_n(cursor,nr,&buffer);
		}
		else if(strcmp(operatie,"paste") == 0)
		{
			push_stiva(undo,CapDeLista,pozitie_cursor);
			pozitie_cursor = pozitie_cursor + strlen(buffer);
			paste(&cursor,buffer);
		}
		else if(strcmp(operatie,"insert") == 0)
		{
			push_stiva(undo,CapDeLista,pozitie_cursor);
			fscanf(f,"%s",deInserat);
			pozitie_cursor = pozitie_cursor + strlen(deInserat);
			insert_string(&cursor,deInserat);
		}
		else if(strcmp(operatie,"backspace") == 0)
		{
			push_stiva(undo,CapDeLista,pozitie_cursor);
			pozitie_cursor = pozitie_cursor - 1;
			backspace(&cursor);
		}
		else if(strcmp(operatie,"undo") == 0)
		{
			push_stiva(redo,CapDeLista,pozitie_cursor);
			pop_stiva(undo,&CapDeLista,&poz2);					//Cand fac undo salvez lista principala in stiva de redo.
			pozitie_cursor = poz2;								//De asemenea fac pop si la pozitia cursorului.
			cursor = CapDeLista;
			move(&cursor,poz2);
		}
		else if(strcmp(operatie,"redo") == 0)
		{
			push_stiva(undo,CapDeLista,pozitie_cursor);		//La fel si in cazul operatie de redo, salvez copie in stiva de
			pop_stiva(redo,&CapDeLista,&poz2);				//undo si ma asigur ca pozitia cursorului estea cea buna.
			pozitie_cursor = poz2;
			cursor = CapDeLista;
			move(&cursor,poz2);
		}
		else if(strcmp(operatie,"replace") == 0)
		{
			fscanf(f,"%s",cuvant);
			fscanf(f,"%s",cuvant_nou);
			push_stiva(redo,CapDeLista,pozitie_cursor);
			find_replace(CapDeLista,cuvant,cuvant_nou);
		}
	}
	fclose(f);
	f = fopen(argv[3],"wt");
	if(f == NULL)
	{
		printf("Fisierul rezultat.out nu a putut fi creat!\n");		
		exit(1);								
	}
	p = CapDeLista->next;						//La final deschid fisierul de output si scriu lista obtinuta.
	while(p != NULL)
		{
		 	fprintf(f,"%c",p->c);
			p = p->next;
		}
	fclose(f);
	free(cuvant);
	free(cuvant_nou);
	free_mem(CapDeLista,undo,redo);
	return 0;
}
