#include<stdio.h>
#include<string.h>

enum {MAX_PARTICIPANTS = 100};
enum {MAX_TEXTE = 30};
enum {MAX_CONCOURS = 20};
enum {MAX_ARGS = 4};

//Contenu d'un participant
//nom = Nom du participant
//prenom = Prenom du participant
typedef struct{
	char nom[MAX_TEXTE];
	char prenom[MAX_TEXTE];
} Participant;

//Contenu d'un concour
//titre = Titre du concour
//inscription = Tableau des identifiants des participants inscrits
typedef struct{
	char titre[MAX_TEXTE];
	int inscipritions[MAX_PARTICIPANTS];
} Concour;

//Ensemble des concours actifs
//concours = Ensemble des concours
//nb = Nombre de concours
typedef struct {
	Concour concours[MAX_CONCOURS];
	int nb;
} ConcoursActifs;

//Ensemble des participants inscrits
//participant = Ensemble des participants
//nb = Nombre de participants
typedef struct {
	Participant participants[MAX_PARTICIPANTS];
	int nb;
} ParticipantsActif;

//Contexte de la session du programme
//participants = Participants inscrits en mémoire
//concouts = Concours inscrits en mémoire
typedef struct {
	ParticipantsActif participants;
	ConcoursActifs concours;
	int exit;
} Contexte;

//Arguments de la commande utilisateur
//values = Liste de valeurs des arguments
//nb = Nombre d'arguments
typedef struct {
	char values[MAX_ARGS][MAX_TEXTE+1];
	int nb;
} Arguments;

///Commande CREER
///Permet de creer un concour
///titre = Titre du concour
void CmdCreer(char titre[], Contexte* context) {
	size_t length = strlen(titre);

	if (length <= 0 && length >= MAX_TEXTE) {
		printf("Nom trop long");
		return;
	}

	for (int i=0; i<=context->concours.nb; ++i) {
		if (!strcmp(context->concours.concours[i].titre, titre)) {
			printf("Nom incorrect");
			return;
		}
	}

	Concour nv_concour;
	strcpy(nv_concour.titre,titre);

	context->concours.concours[context->concours.nb] = nv_concour;
	++context->concours.nb;

	printf("Creation enregistree (%d)", context->concours.nb);
}

///Commande PARTICIPANTS
///Affiche la liste des participants
void CmdParticipants(Contexte context) {
	ParticipantsActif participants = context.participants;
	if (participants.nb != 0) {
		for (int i=0; i<participants.nb; ++i) {
			Participant p = participants.participants[participants.nb];
			printf("(%d) %s %s : X concours\n", i, p.nom, p.prenom);
		}
	}
	else {
		printf("Aucun participant inscrit");
	}
}

///Commande CONCOURS
///Affiche la liste des concours
void CmdConcours() {
	
}

///Recuperation des arguments
Arguments cmdProcessArgs(char input[]) {
	Arguments args = {};
	int i = 0;
	int ri = 0;

	//Lecture commande et arguments
	while (input[i] != '\0') {
		if (input[i] == ' ') {
			++args.nb;
			ri = 0;
			if (args.nb> MAX_ARGS) {
				printf("Trop d'arguments");
				return args;
			}
		}
		else {
			args.values[args.nb][ri] = input[i];
			++ri;
		}
		++i;
	}
	args.values[args.nb][ri-1] = '\0';
	++args.nb;
	return args;
}

///Detection des commandes
void cmdProcess(char input[], Contexte* context) {

	Arguments args;
	args = cmdProcessArgs(input);
	char commande[MAX_TEXTE+1];
	strcpy(commande, args.values[0]);
	
	//PARTICIPANTS
	if (!strcmp(commande, "PARTICIPANTS")) {
		CmdParticipants(*context);
	}
	//CREER
	else if (!strcmp(commande, "CREER")) {
		if (args.nb == 2) {
			CmdCreer(args.values[1], context);
		}
		else {
			printf("CREER [titre]");
		}
	}
	//EXIT
	else if (!strcmp(commande, "EXIT")) {
		context->exit = 1;
	}
	else {
		printf("Commande non reconnue");
	}
	printf("\n");
}

int main() {
	Contexte contexte;
	contexte.participants.nb = 0;
	contexte.concours.nb = 0;
	contexte.exit = 0;

	int result = 0;
	while (contexte.exit == 0) {
		char commande[MAX_TEXTE] = "";
		fgets(commande, MAX_TEXTE, stdin);
		cmdProcess(commande, &contexte);
	}
}
