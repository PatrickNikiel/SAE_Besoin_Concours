#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma warning(disable:4996)
enum { MAX_PARTICIPANTS = 100 };
enum { MAX_TEXTE = 30 };
enum { MAX_CONCOURS = 20 };
enum { MAX_ARGS = 4 };

//Contenu d'un participant
//nom = Nom du participant
//prenom = Prenom du participant
typedef struct {
	char nom[MAX_TEXTE];
	char prenom[MAX_TEXTE];
} Participant;

//Ensemble des participants inscrits
//participant = Ensemble des participants
//nb = Nombre de participants
typedef struct {
	Participant participants[MAX_PARTICIPANTS];
	int nb;
} ParticipantsActif;

//Contenu d'un concour
//titre = Titre du concour
//inscription = Tableau des identifiants des participants inscrits
typedef struct {
	char titre[MAX_TEXTE];
	int scores[MAX_PARTICIPANTS];
} Concour;

//Ensemble des concours actifs
//concours = Ensemble des concours
//nb = Nombre de concours
typedef struct {
	Concour concours[MAX_CONCOURS];
	int nb;
} ConcoursActifs;

//Contexte de la session du programme
//participants = Participants inscrits en mémoire
//concouts = Concours inscrits en mémoire
//EXIT : si exit = 1 fin de la boucle
typedef struct {
	ParticipantsActif participants;
	ConcoursActifs concours;
	int exit;
} Contexte;

//Arguments de la commande utilisateur
//values = Liste de valeurs des arguments
//nb = Nombre d'arguments
typedef struct {
	char values[MAX_ARGS][MAX_TEXTE];
	int nb;
} Arguments;

///Commande CREER
///Permet de creer un concour
///titre = Titre du concour
void CmdCreer(char titre[], Contexte* context) {
	size_t length = strlen(titre);

	if (length <= 0 || length >= MAX_TEXTE) {
		printf("Nom trop long");
		return;
	}

	for (int i = 0; i <= context->concours.nb; ++i) {
		if (!strcmp(context->concours.concours[i].titre, titre)) {
			printf("Nom incorrect");
			return;
		}
	}

	Concour nv_concour;
	for (int i = 0; i < MAX_PARTICIPANTS; i++) {
		nv_concour.scores[i] = 0;
	}
	strcpy(nv_concour.titre, titre);

	context->concours.concours[context->concours.nb] = nv_concour;
	++context->concours.nb;

	printf("Creation enregistree (%d)", context->concours.nb);
}

///Commande PARTICIPANTS
///Affiche la liste des participants
void CmdParticipants(Contexte* context) {
	if (context->participants.nb != 0) {
		for (int i = 0; i < context->participants.nb; ++i) {
			Participant p = context->participants.participants[i];
			int count = 0;
			for (int c = 0; c < context->concours.nb; c++) {
				count += (context->concours.concours[c].scores[i] == 0) ? 0 : 1;
			}
			printf("(%d) %s %s : %d concours", i + 1, p.prenom, p.nom, count);
			if (i < context->concours.nb) {
				printf("\n");
			}
		}
	}
	else {
		printf("Aucun participant inscrit");
	}
}


///Commande CONCOURS
///Affiche la liste des concours
void CmdConcours(Contexte* context) {
	if (context->concours.nb != 0) {
		for (int i = 0; i < context->concours.nb; ++i) {
			Concour c = context->concours.concours[i];
			int count = 0;
			for (int p = 0; p < context->participants.nb; p++) {
				count += (context->concours.concours->scores[p] == 0) ? 0 : 1;
			}
			printf("(%d) %s : %d participants", i + 1, c.titre, count);
			if (i < context->concours.nb) {
				printf("\n");
			}
		}
	}
	else {
		printf("Aucun concours cree");
	}
}
void CmdScores(char* id, Contexte* context) {
	int i_id = atoi(id)-1;
	int nb_scores = 0;
	Participant p = context->participants.participants[i_id];
	if (i_id >= 0 && i_id <= context->participants.nb) {
		printf("%s %s\n", p.prenom, p.nom);
		for (int i = 0; i < context->concours.nb; ++i) {
			Concour c = context->concours.concours[i];
			if (c.scores[i] > 0) {
				printf("(%d) %s : %d points\n",i+1 , c.titre,c.scores[i]);
				++nb_scores;
			}
		}
		if (nb_scores == 0) {
			printf("Aucun gai\n");
		}
	}
	else {
		printf("Identifiant incorrect\n");
	}
}


void CmdGain(char* personne_id, char* concour_id, char* points, Contexte* context) {
	int p_id = atoi(personne_id) - 1;
	int c_id = atoi(concour_id) - 1;
	int i_points = atoi(points);
	if (c_id < context->concours.nb && c_id >= 0 && p_id >= 0 && p_id < context->participants.nb) {
		context->concours.concours[c_id].scores[p_id] += i_points;
		printf("Gain enregistre");
	}
	else {
		return;
	}
}

///Recuperation des arguments
Arguments cmdProcessArgs(char input[]) {
	Arguments args;
	args.nb = 0;
	int i = 0;
	int ri = 0;

	//Lecture commande et arguments
	while (input[i] != '\0') {
		if (input[i] == ' ') {
			args.values[args.nb][ri] = '\0';
			++args.nb;
			ri = 0;
			if (args.nb > MAX_ARGS) {
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
	args.values[args.nb][ri - 1] = '\0';
	++args.nb;
	return args;
}
void CmdInscrire(char* prenom, char* nom, Contexte* context) {
	Participant nv_parti;
	strcpy(nv_parti.prenom, prenom);
	strcpy(nv_parti.nom, nom);
	context->participants.participants[context->participants.nb] = nv_parti;
	++context->participants.nb;
	printf("Inscription enregistree (%d)", context->participants.nb);
}


///Detection des commandes
void cmdProcess(char input[], Contexte* context) {

	Arguments args;
	args = cmdProcessArgs(input);
	char commande[MAX_TEXTE + 1];
	strcpy(commande, args.values[0]);

	//PARTICIPANTS
	if (!strcmp(commande, "PARTICIPANTS")) {
		CmdParticipants(context);
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
	//SCORES
	else if (!strcmp(commande, "SCORES")) {
		if (args.nb == 2) {
			CmdScores(args.values[1], context);
		}
		else {
			printf("SCORES [ID PARTICIPANTS]");
		}
	}
	//INSCRIRE
	else if (!strcmp(commande, "INSCRIRE")) {
		if (args.nb == 3) {
			CmdInscrire(args.values[1], args.values[2], context);
		}
		else {
			printf("INSCRIRE [PRENOM] [NOM]");
		}
	}
	//CONCOURS
	else if (!strcmp(commande, "CONCOURS")) {
		CmdConcours(context);
	}
	//GAIN
	else if (!strcmp(commande, "GAIN")) {
		if (args.nb == 4) {
			CmdGain(args.values[1], args.values[2], args.values[3], context);
		}
		else {
			printf("GAIN [ID_PARTICIPANT] [ID_CONCOUR] [SOMME]");
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
		fgets(commande, MAX_TEXTE * MAX_ARGS, stdin);
		cmdProcess(commande, &contexte);
	}
}
