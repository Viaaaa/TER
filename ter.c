#include <stdio.h>
#include <stdlib.h>
//#include <cplex.h>


int debug = 1;
int taille_agents = 8;
int taille_regles = 10;


typedef struct environnement environnement;
typedef struct agent agent;
typedef struct regle regle;


//Structure représentant un environnement
struct environnement{
	int nombre_agents;
	agent* agents;
	
	int nombre_regles;
	regle* regles;
};


//Agent
struct agent{
	char* nom;
};

//Regle
struct regle{
	int* negatif_gauche;
	agent** agents_gauche;
	int* negatif_droite;
	agent** agents_droite;
};

int eql(char* a, char* b){
	int i;
	for(i=0; i<taille_agents; i++){
		if(a[i] != b[i]) return 0;
	}
	return 1;
}

int min(int a, int b){
	if(a <= b) return a;
	return b;
}

int abs(int a){
	if(a >= 0) return a;
	return -a;
}


//Fonction lisant un fichier rr (environnement) et remplissant notre structure
environnement recuperer_environnement(char* nom_fichier){
	FILE* fichier = NULL;
    fichier = fopen(nom_fichier, "r");
    if(fichier == NULL){
    	printf("Impossible d'ouvrir le fichier d'environnement\n");
    	exit(EXIT_FAILURE);
    }
    int i ,j ,ou = 0;
    char c;
    environnement env;
    env.nombre_agents = 0;
    env.nombre_regles = 0;
    //Boucle qui compte le nombre d'agents et de règles
    do 	{
  		c = fgetc(fichier);
  		if(c == '#'){
  			do{
  				c = fgetc(fichier);
  			} while (c != '\n');
  		}
 		else if(ou < 6 && (c == '+' || c == '-')) ou = -1;	
 		else if(ou == -1 && c == ':'){
 			env.nombre_agents++;
 			ou = 0;
 		}	
 		else if(c == 'r' && ou < 6) ou++;
 		else if(c == 'u' && ou == 1) ou++;
 		else if(c == 'l' && ou == 2) ou++;
 		else if(c == 'e' && ou == 3) ou++;
 		else if(c == 's' && ou == 4) ou++;
 		else if(c == ':' && ou == 5) ou++;
 		else if(c == '>' && ou == 6) ou++;
 		else if(ou == 7){
 			if(c == '>') env.nombre_regles++;
 			ou = 6;		
 		}
 		else if(ou < 6 && c != ' ') ou = 0;
    } while (c != EOF);
    
    if(debug) printf("\nNombre d'agents: %d  Nombre de règles: %d\n", env.nombre_agents, env.nombre_regles);
    ou = 0;
    rewind(fichier);    
	
	
	//On stocke les agents
	int compteur_agents = 0, dans_agents = 0;
    env.agents = malloc(env.nombre_agents*sizeof(agent));
    for(i=0; i<env.nombre_agents; i++){
    	env.agents[i].nom = malloc(taille_agents*sizeof(char));
    }
	for(j=0; j<env.nombre_agents; j++){
		for(i=0; i<taille_agents; i++){
			env.agents[j].nom[i] = ' ';
		}
    }
    
    do 	{  	
  		c = fgetc(fichier);
 		if(c == '#'){
  			do{
  				c = fgetc(fichier);
  			} while (c != '\n');
  		}
 		else if(c == '+' || c == '-') ou = -1;	
 		else if(ou == -1 && c == ':'){	
 			do{
 				fseek(fichier, -2, SEEK_CUR);
 				c = fgetc(fichier);
 			} while (c != '\n');
 			do{
 				c = fgetc(fichier);
 			} while(c == ' ');
 			do{
 				env.agents[compteur_agents].nom[dans_agents] = c;
 				dans_agents++;
 				c = fgetc(fichier);
 			} while (c != '+' && c != '-');
 			dans_agents = 0;
 			compteur_agents++;
 			ou = 0;
 		}
 		else if(c == 'r') ou++;
 		else if(c == 'u' && ou == 1) ou++;
 		else if(c == 'l' && ou == 2) ou++;
 		else if(c == 'e' && ou == 3) ou++;
 		else if(c == 's' && ou == 4) ou++;
 		else if(c == ':' && ou == 5) ou++;
 		else if(ou < 6 && c != ' ') ou = 0;
    } while (ou != 6);
    
    if(debug){
    	j=0;
    	for(i=0; i<env.nombre_agents; i++){	
			while(env.agents[i].nom[j] != ' '){
				printf("%c", env.agents[i].nom[j]);
				j++;
			}
			j = 0;
			printf(" ");
		}
		printf("\n");
    }
    
    
    //On stocke les regles
    env.regles = malloc(env.nombre_regles*sizeof(regle));
    for(i=0; i<env.nombre_regles; i++){
    	env.regles[i].negatif_gauche = malloc(taille_regles*sizeof(int));
    	env.regles[i].agents_gauche = malloc(taille_regles*sizeof(agent*));
    	env.regles[i].negatif_droite = malloc(taille_regles*sizeof(int));
    	env.regles[i].agents_droite = malloc(taille_regles*sizeof(agent*));   	
    }
    for(i=0; i<env.nombre_regles; i++){
    	for(j=0; j<taille_regles; j++){
    		env.regles[i].negatif_gauche[j] = -1;
    		env.regles[i].negatif_droite[j] = -1;
    	}
    }
    char* temp = malloc(taille_agents*sizeof(char));
    for(i=0; i<taille_agents; i++){
    	temp[i] = ' ';
    }
    int dans_regles = 0, compteur_regles = 0;
    ou = 0;
    dans_agents = 0;
    
    do{
    	c = fgetc(fichier);
    	if(c == '#'){
  			do{
  				c = fgetc(fichier);
  			} while (c != '\n');
  		}
 		else if(c == '>' && (ou == 0 || ou == 1)) ou++;
    	if(ou == 2){
    		do{
 				fseek(fichier, -2, SEEK_CUR);
 				c = fgetc(fichier);
 			} while (c != '\n');
 			
 			//gauche
 			do{
 				c = fgetc(fichier);
 				if(c == ','){
 					dans_regles++;
 					dans_agents = 0;
 					for(i=0; i<taille_agents; i++){
    					temp[i] = ' ';
  					}
 				}
 				// c est une lettre
 				else if((c >= 65 && c <= 90) || (c >= 97 && c >= 122)){
 					do{
 						temp[dans_agents] = c;
 						dans_agents++;
 						c = fgetc(fichier);	
 					} while(c != '+' && c != '-');
 					if(c == '+') env.regles[compteur_regles].negatif_gauche[dans_regles] = 0;
 					else if(c == '-') env.regles[compteur_regles].negatif_gauche[dans_regles] = 1;
 					for(i = 0; i < env.nombre_agents; i++){
 						if(eql(env.agents[i].nom, temp)) env.regles[compteur_regles].agents_gauche[dans_regles] = &env.agents[i];
 					}
 				}			
 			} while (c != '>');
 			fseek(fichier, 1, SEEK_CUR);
 			dans_regles=0;
 			dans_agents=0;
 			for(i=0; i<taille_agents; i++){
    			temp[i] = ' ';
  			}
 			//droite
 			do{
 				c = fgetc(fichier);
 				if(c == ','){
 					dans_regles++;
 					dans_agents = 0;
 					for(i=0; i<taille_agents; i++){
    					temp[i] = ' ';
  					}
 				}
 				// c est une lettre
 				else if((c >= 65 && c <= 90) || (c >= 97 && c >= 122)){
 					do{
 						temp[dans_agents] = c;
 						dans_agents++;
 						c = fgetc(fichier);	
 					} while(c != '+' && c != '-');
 					if(c == '+') env.regles[compteur_regles].negatif_droite[dans_regles] = 0;
 					else if(c == '-') env.regles[compteur_regles].negatif_droite[dans_regles] = 1;
 					for(i = 0; i < env.nombre_agents; i++){
 						if(eql(env.agents[i].nom, temp)) env.regles[compteur_regles].agents_droite[dans_regles] = &env.agents[i];
 					}	
 				}
 			} while (c != '\n' && c!= '#');
 			dans_regles=0;
 			dans_agents=0;
 			for(i=0; i<taille_agents; i++){
    			temp[i] = ' ';
  			}
 			compteur_regles++;
 			ou = 0;	
    	}
    
    } while (c != EOF);	
    
    if(debug){
    	int k=0;
    	j=0;
    	for(i=0; i<env.nombre_regles; i++){
			//Gauche
			while(env.regles[i].negatif_gauche[j] != -1){
				while(env.regles[i].agents_gauche[j]->nom[k] != ' '){
					printf("%c", env.regles[i].agents_gauche[j]->nom[k]);
					k++;
				}
				if(env.regles[i].negatif_gauche[j]) printf("-");
				else printf("+");
				printf(" ");
				k=0;
				j++;
			}
			printf(">> ");
			j=0;
			k=0;
			//droite
			while(env.regles[i].negatif_droite[j] != -1){
				while(env.regles[i].agents_droite[j]->nom[k] != ' '){
					printf("%c", env.regles[i].agents_droite[j]->nom[k]);
					k++;
				}
				if(env.regles[i].negatif_droite[j]) printf("-");
				else printf("+");
				printf(" ");
				k=0;
				j++;
			}
			printf("\n");
			j=0;
			k=0;
		}
	printf("\n");
    }
    
    free(temp);
    fclose(fichier);
    return env;
}

void liberer_environnement(environnement env){
	int i;
    for(i=0; i<env.nombre_agents; i++){
    	free(env.agents[i].nom);
    }
    free(env.agents);
	for(i=0; i<env.nombre_regles; i++){
    	free(env.regles[i].negatif_gauche);
    	free(env.regles[i].agents_gauche);
    	free(env.regles[i].negatif_droite);
    	free(env.regles[i].agents_droite);
    }
    free(env.regles);
}
    
int taille(int* partie){
	int i=0;
	while(partie[i] != -1){
		i++;
	}
	return i;
}    
    
//Place à gauche de l'agent l dans la règle i
int place_gauche(environnement env, int l, int i){
	int j = 0;
	while(&env.agents[j] != env.regles[i].agents_gauche[l]){
		j++;
	} 
	if(env.regles[i].negatif_gauche[l]) j = 2*j + 1;
	else j = 2*j;
	return j;
}

//Place à droite de l'agent l dans la règle i
int place_droite(environnement env, int l, int i){
	int j = 0;
	while(&env.agents[j] != env.regles[i].agents_droite[l]){
		j++;
	} 
	if(env.regles[i].negatif_droite[l]) j = 2*j + 1;
	else j = 2*j;
	return j;
}
    
void creer_mod(environnement env1, environnement env2, char* nom_fichier){
	FILE* fichier = NULL;
    fichier = fopen(nom_fichier, "w");
    if(fichier == NULL){
    	printf("Impossible d'ouvrir le fichier pour l'écriture du modèle\n");
    	exit(EXIT_FAILURE);
    }
    
    int i,j,k,l,m;
    fprintf(fichier, "dvar boolean X[0..%d][0..%d];\n", env2.nombre_agents*2 - 1, env1.nombre_agents*2 - 1);
    fprintf(fichier, "dvar boolean Y[0..%d][0..%d];\n", env2.nombre_regles-1, env1.nombre_regles-1);
    for(j=0; j<env2.nombre_regles; j++){
    	for(i=0; i<env1.nombre_regles; i++){
    		k = (taille(env1.regles[i].negatif_gauche)*taille(env2.regles[j].negatif_gauche)) + (taille(env1.regles[i].negatif_droite)*taille(env2.regles[j].negatif_droite)) - 1;
    		fprintf(fichier, "dvar boolean S%d[0..%d];\n", i+(j*env1.nombre_regles), k);
    	}
    }
    fprintf(fichier, "dvar int S;\nmaximize S;\n\nsubject to{\n	//Lignes agents\n");
    
    for(j=0; j<env2.nombre_agents*2; j++){
    	fprintf(fichier,"	");
    	for(i=0; i<env1.nombre_agents*2; i++){
    		if(i != 0) fprintf(fichier, " + X[%d][%d]", j, i);
    		else fprintf(fichier, "X[%d][%d]", j, i);
    	}
    	fprintf(fichier," <= 1;\n");
    }
    
    fprintf(fichier,"\n	//Colonnes agents\n");
    for(i=0; i<env1.nombre_agents*2; i++){
    fprintf(fichier,"	");
    	for(j=0; j<env2.nombre_agents*2; j++){
    		if(j != 0) fprintf(fichier, " + X[%d][%d]", j, i);
    		else fprintf(fichier, "X[%d][%d]", j, i);
    	}
    	fprintf(fichier," <= 1;\n");
    }
    
    fprintf(fichier,"\n	//Egalite des croix agents\n");
    for(j=0; j<env2.nombre_agents*2; j=j+2){
    	for(i=0; i<env1.nombre_agents*2; i=i+2){
    		fprintf(fichier,"	X[%d][%d] == X[%d][%d];\n", j, i, j+1, i+1);
    		fprintf(fichier,"	X[%d][%d] == X[%d][%d];\n", j, i+1, j+1, i);
    	}	
    	fprintf(fichier,"\n");
    }
    
    fprintf(fichier,"	//Lignes regles\n");
    for(j=0; j<env2.nombre_regles; j++){
    	fprintf(fichier,"	");
    	for(i=0; i<env1.nombre_regles; i++){
    		if(i != 0) fprintf(fichier, " + Y[%d][%d]", j, i);
    		else fprintf(fichier, "Y[%d][%d]", j, i);
    	}   
    	fprintf(fichier," <= 1;\n");
   	}
   	
   	fprintf(fichier,"\n	//Colonnes regles\n");
    for(i=0; i<env1.nombre_regles; i++){
    	fprintf(fichier,"	");
    	for(j=0; j<env2.nombre_regles; j++){
		 	if(j != 0) fprintf(fichier, " + Y[%d][%d]", j, i);
				else fprintf(fichier, "Y[%d][%d]", j, i);
			}   
			fprintf(fichier," <= 1;\n");
   	}
   	
   	fprintf(fichier,"\n	//Fonction d'evaluation\n");
   	for(j=0; j<env2.nombre_regles; j++){
	   	for(i=0; i<env1.nombre_regles; i++){
	   		//Gauche
	   		for(l=0; l < taille(env2.regles[j].negatif_gauche); l++){
	   			for(k=0; k < taille(env1.regles[i].negatif_gauche); k++){
		   			fprintf(fichier,"	S%d[%d] <= Y[%d][%d];	S%d[%d] <= X[%d][%d];	S%d[%d] >= Y[%d][%d] + X[%d][%d] - 1;",
		   			 i+(j*env1.nombre_regles), k+(l*taille(env1.regles[i].negatif_gauche)), j, i,
		   			 i+(j*env1.nombre_regles), k+(l*taille(env1.regles[i].negatif_gauche)), place_gauche(env2, l, j), place_gauche(env1, k, i),		   			 
		   			 i+(j*env1.nombre_regles), k+(l*taille(env1.regles[i].negatif_gauche)), j, i, place_gauche(env2, l, j), place_gauche(env1, k, i));
		   			fprintf(fichier,"\n");
		   		}
	   		}
	   		m = taille(env2.regles[j].negatif_gauche)*taille(env1.regles[i].negatif_gauche);
	   		//Droite
	   		for(l=0; l < taille(env2.regles[j].negatif_droite); l++){
	   			for(k=0; k < taille(env1.regles[i].negatif_droite); k++){
		   			fprintf(fichier,"	S%d[%d] <= Y[%d][%d];	S%d[%d] <= X[%d][%d];	S%d[%d] >= Y[%d][%d] + X[%d][%d] - 1;",
		   			 i+(j*env1.nombre_regles), k+(l*taille(env1.regles[i].negatif_droite))+m, j, i,
		   			 i+(j*env1.nombre_regles), k+(l*taille(env1.regles[i].negatif_droite))+m, place_droite(env2, l, j), place_droite(env1, k, i),		   			 
		   			 i+(j*env1.nombre_regles), k+(l*taille(env1.regles[i].negatif_droite))+m, j, i, place_droite(env2, l, j), place_droite(env1, k, i));
		   			fprintf(fichier,"\n");
		   		}
	   		}
	   		fprintf(fichier,"\n");
	   	}
	}
   	
   	fprintf(fichier,"	S == ");
   	for(j=0; j<env2.nombre_regles; j++){
	   	for(i=0; i<env1.nombre_regles; i++){
	   		k = (taille(env1.regles[i].negatif_gauche)*taille(env2.regles[j].negatif_gauche)) + (taille(env1.regles[i].negatif_droite)*taille(env2.regles[j].negatif_droite));
	   		for(l=0; l < k; l++){
	   			if(i != 0 || j != 0 || l != 0) fprintf(fichier," + 2*S%d[%d]", i+(j*env1.nombre_regles), l);
	   			else fprintf(fichier,"2*S%d[%d]", i+(j*env1.nombre_regles), l);
	   		}
	   		k = min(taille(env1.regles[i].negatif_gauche), taille(env2.regles[j].negatif_gauche)) + abs(taille(env1.regles[i].negatif_gauche) - taille(env2.regles[j].negatif_gauche))
	   		 + min(taille(env1.regles[i].negatif_droite), taille(env2.regles[j].negatif_droite)) + abs(taille(env1.regles[i].negatif_droite) - taille(env2.regles[j].negatif_droite));
	   		fprintf(fichier," - %d * Y[%d][%d]\n	  ", k, j, i);
   		}
   		
   	}
   	fprintf(fichier,";\n}\n");
   	
	fclose(fichier);
		
	if(debug){
		fichier = fopen(nom_fichier, "r");
		if(fichier == NULL){
			printf("Impossible d'ouvrir le fichier pour la lecture du modèle\n");
			exit(EXIT_FAILURE);
		}
		char c;
		c=fgetc(fichier);
		do{
			printf("%c", c);
			c = fgetc(fichier);			
		} while(c != EOF);
		fclose(fichier);
	}
}




int main(int argc, char* argv[]){
	if(argc != 4){
		printf("Utilisation: ./ter fichier1 fichier2 sortie.mod\n");
		exit(1);
	}
	
	
	environnement env1 = recuperer_environnement(argv[1]);
	environnement env2 = recuperer_environnement(argv[2]);	
	creer_mod(env1, env2, argv[3]);
	liberer_environnement(env1);
	liberer_environnement(env2);
	
	return 0;
}
