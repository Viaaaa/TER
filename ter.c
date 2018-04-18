#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
	char nom[5];
};

//Regle
struct regle{
	int negatif_gauche[10];
	agent* agents_gauche[10];
	int negatif_droite[10];
	agent* agents_droite[10];
};

int eql(char* a, char* b){
	int i;
	for(i=0; i<5; i++){
		if(a[i] != b[i]) return 0;
	}
	return 1;
}


//Fonction lisant un fichier rr (environnement) et remplissant notre structure
environnement recuperer_environnement(char* nom_fichier){
	FILE* fichier = NULL;
    fichier = fopen(nom_fichier, "r");
    if(fichier == NULL){
    	printf("Impossible d'ouvrir le fichier\n");
    	exit(EXIT_FAILURE);
    }
    int i ,j ,ou = 0;
    char c;
    struct environnement env;
    env.nombre_agents = 0;
    env.nombre_regles = 0;
    //Boucle qui compte le nombre d'agents et de règles
    do 	{
  		c = fgetc(fichier);
 		if(ou < 6 && (c == '+' || c == '-')) ou = -1;	
 		else if(ou == -1 && c == ':'){
 			env.nombre_agents++;
 			ou = 0;
 		}	
 		else if(c == 'r') ou++;
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
 		else if(ou < 6) ou = 0;
    } while (c != EOF);
    ou = 0;
    rewind(fichier);    

	//On stocke les agents
	int compteur_agents = 0, dans_agents = 0;
    env.agents = malloc(env.nombre_agents*sizeof(agent));
	for(j=0; j<env.nombre_agents; j++){
		for(i=0; i<5; i++){
			env.agents[j].nom[i] = ' ';
		}
    }
    
    do 	{
  		c = fgetc(fichier);
 		if(c == '+' || c == '-') ou = -1;	
 		else if(ou == -1 && c == ':'){
 			fseek(fichier, -2, SEEK_CUR);
 			do{
 				fseek(fichier, -2, SEEK_CUR);
 				c = fgetc(fichier);
 			} while (c != ' ' && c != '\n');
 			c = fgetc(fichier);
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
 		else if(ou < 6) ou = 0;
    } while (ou != 6);
    
    //On stocke les regles
    env.regles = malloc(env.nombre_regles*sizeof(regle));
    char temp[5];
    for(i=0; i<5; i++){
    	temp[i] = ' ';
    }
    int dans_regles, compteur_regles = 0;
    ou = 0;
    dans_agents = 0;
    do{
    	c = fgetc(fichier);
    	if(c == '>' & (ou == 0 || ou == 1)) ou++;
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
 					for(i=0; i<5; i++){
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
 			for(i=0; i<5; i++){
    			temp[i] = ' ';
  			}
 			//droite
 			do{
 				c = fgetc(fichier);
 				if(c == ','){
 					dans_regles++;
 					dans_agents = 0;
 					for(i=0; i<5; i++){
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
 			} while (c != '\n');
 			dans_regles=0;
 			dans_agents=0;
 			for(i=0; i<5; i++){
    			temp[i] = ' ';
  			}
 			compteur_regles++;
 			ou = 0;	
    	}
    
    } while (c != EOF);
    fclose(fichier);
    return env;
}
    
void creer_mod(environnement env1, environnement env2){

}

int main(int argc, char* argv[]){
	if(argc != 3){
		printf("Utilisation: ./ter fichier1 fichier2\n");
		exit(1);
	}
	

	
	environnement env1 = recuperer_environnement(argv[1]);
	environnement env2 = recuperer_environnement(argv[2]);
	creer_mod(env1, env2);
	
	free(env1.agents);
	free(env1.regles);
	free(env2.agents);
	free(env2.regles);
	
	return 0;
}
