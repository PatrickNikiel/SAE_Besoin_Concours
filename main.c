#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma warning(disable:4996)
enum { MAX_PARTICIPANTS = 100 };
enum { MAX_TEXTE = 30 };
enum { MAX_CONCOURS = 20 };
enum { MAX_ARGS = 4 };
enum { MAX_CMD = MAX_TEXTE*MAX_ARGS};

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
	int positions[MAX_PARTICIPANTS];
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

void plural(int value) {
	if (abs(value) > 1) {
		printf("s");
	}
}

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
	for (int i = 0; i < MAX_PARTICIPANTS; ++i) {
		nv_concour.scores[i] = 0;
		nv_concour.positions[i] = 1;
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
			if (i < context->participants.nb-1) {
				printf("\n");
			}
		}
	}
	else {
		printf("Aucun participant inscrit");
	}
}

void sortIdsByScore(int* ids, int* scores, int size) {
	for (int i=0; i<size-1; ++i) {
		for (int j=0; j<size-1; ++j) {
			if (scores[ids[j]] < scores[ids[j+1]]) {
				int tmp = ids[j];
				ids[j] = ids[j+1];
				ids[j+1] = tmp;
			}
		}
	}
}

//Retourne le nombre de participants
int storeActiveParticipants(int* in, Concour* concour, int nb_participants) {
	int count = 0;
	for (int id = 0; id < nb_participants; id++) {
		if (concour->scores[id] > 0) {
			in[count] = id;
			count++;
		}
	}
	return count;
}

int isParticipantEx(int* scores, int* participants, int id, int participants_nb, int* before_score, int* score) {
	*score = scores[participants[id]];
	int before_id = id>0?id-1:id;
	int after_id = id<participants_nb-1?id+1:id;
	*before_score = scores[participants[before_id]];
	int after_score = scores[participants[after_id]];
	return (*before_score==*score && before_id < id)||(after_score==*score && after_id > id);
}

//Commande CLASSEMENT
void CmdClassement(char* concour_id, Contexte* context) {
	int c_id = atoi(concour_id)-1;
	if (c_id >= 0 && c_id < context->concours.nb) {
		Concour* c = &context->concours.concours[c_id];
		printf("%s\n", c->titre);

		ParticipantsActif* p = &context->participants;
		int list[MAX_PARTICIPANTS];
		int p_nb = storeActiveParticipants(list, c, p->nb);

		if (p_nb == 0) {
			printf("Aucun participant");
			return;
		}
	
		sortIdsByScore(list, c->scores, p_nb);
		
		int pos = 1;
		for (int i=0; i<p_nb; i++) {
			int score = 0;
			int before_score = 0;
			int is_ex = isParticipantEx(c->scores, list, i, p_nb, &before_score, &score);
			if (before_score != score) {
				pos=i+1;
			}
			printf("%d%s", pos, is_ex?"ex":"");
			printf(" -> (%d) %s %s : %d point", list[i]+1, p->participants[list[i]].prenom,  p->participants[list[i]].nom, score);
			plural(score);
			if (i < p_nb-1) {
				printf("\n");
			}
		}
	}
	else {
		printf("Identifiant incorrect");
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
				count += (c.scores[p] > 0) ? 1 : 0;
			}
			printf("(%d) %s : %d participant", i + 1, c.titre, count);
			plural(count);
			if (i < context->concours.nb-1) {
				printf("\n");
			}
		}
	}
	else {
		printf("Aucun concours cree");
	}
}

//SCORES
void CmdScores(char* id, Contexte* context) {
	int p_id = atoi(id)-1;
	int nb_scores = 0;
	Participant p = context->participants.participants[p_id];
	if (p_id >= 0 && p_id < context->participants.nb) {
		printf("%s %s\n", p.prenom, p.nom);
		for (int c_id = 0; c_id < context->concours.nb; ++c_id) {
			Concour c = context->concours.concours[c_id];
			if (c.scores[p_id] > 0) {
				if (nb_scores > 0) {
					printf("\n");
				}
				printf("(%d) %s : %d point", c_id+1, c.titre, c.scores[p_id]);
				plural(c.scores[p_id]);
				++nb_scores;
			}
		}
		if (nb_scores == 0) {
			printf("Aucun gain");
		}
	}
	else {
		printf("Identifiant incorrect");
	}
}

void CmdGain(char* personne_id, char* concour_id, char* points, Contexte* context) {
	int p_id = atoi(personne_id) - 1;
	int c_id = atoi(concour_id) - 1;
	int i_points = atoi(points);

	if (i_points <= 0) {
		printf("Points incorrects");
		return;
	}
	if (c_id < 0 || c_id >= context->concours.nb) {
		printf("Concours incorrect");
		return;
	}
	if (p_id < 0 || p_id >= context->participants.nb) {
		printf("Participant incorrect");
		return;
	}
	context->concours.concours[c_id].scores[p_id] += i_points;
	printf("Gain enregistre");
}

void CmdInscrire(char* prenom, char* nom, Contexte* context) {
	for (int i = 0; i<context->participants.nb; ++i) {
		if (!strcmp(context->participants.participants[i].nom, nom) && !strcmp(context->participants.participants[i].prenom, prenom)) {
			printf("Nom incorrect");
			return;
		}
	}

	Participant nv_parti;
	strcpy(nv_parti.prenom, prenom);
	strcpy(nv_parti.nom, nom);
	int nv_id = context->participants.nb;                       
	for (int c_id = 0; c_id < context->concours.nb; ++c_id) {
		Concour* c = &context->concours.concours[c_id];
		int rang = 1;
		for (int p_id = 0; p_id < nv_id; ++p_id) {
			if (c->scores[p_id] > 0) {
				++rang;
			}
		}
		c->positions[nv_id] = rang;
	}

	//printf("-%s - %s\n", prenom, nom);
	context->participants.participants[context->participants.nb] = nv_parti;
	++context->participants.nb;
	printf("Inscription enregistree (%d)", context->participants.nb);
}

void CmdComparaison(char* concour_1, char* concour_2, Contexte* contexte) {
	int i_1 = atoi(concour_1)-1;
	int i_2 = atoi(concour_2)-1;
	
	if (i_1 < 0 || i_1 >= contexte->concours.nb) {
		printf("Identifiant incorrect");
		return;
	}
	if (i_2 < 0 || i_2 >= contexte->concours.nb) {
		printf("Identifiant incorrect");
		return;
	}

	Concour* c_1 = &contexte->concours.concours[i_1];
	Concour* c_2 = &contexte->concours.concours[i_2];

	int count = 0;
	for (int p_i = 0; p_i < contexte->participants.nb; ++p_i) {
		if (c_1->scores[p_i] > 0 || c_2->scores[p_i] > 0) {
			count++;
		}
	}

	printf("%s versus %s\n", c_2->titre, c_1->titre);
	if (count == 0) {
		printf("Aucun participant");
		return;
	}

	for (int p_i = 0; p_i < contexte->participants.nb; ++p_i) {
		if (c_1->scores[p_i] == 0 && c_2->scores[p_i] > 0) {
			printf("Concours incompatibles");
			return;
		}
	}

	count = 0;
	for (int id = 0; id < contexte->participants.nb; id++) {
		int score_1 = c_1->scores[id];
		int score_2 = c_2->scores[id];
		if (score_1 > 0) {
			if (count > 0) {
				printf("\n");
			}
			printf("(%d) %s %s : ", id+1, contexte->participants.participants[id].prenom, contexte->participants.participants[id].nom);
			printf("%d point", score_2);
			plural(score_2);
			printf("/%d point", score_1);
			plural(score_1);
			printf(" : %.2f%%", ((float)score_2 / score_1)*100);
			count++;
		}
	}
}

void cmdBilan(char* concour_id, Contexte* context) {
	int c_id = atoi(concour_id)-1;
	if (c_id >= 0 && c_id < context->concours.nb) {
		Concour* c = &context->concours.concours[c_id];
		printf("%s\n", c->titre);

		ParticipantsActif* p = &context->participants;
		int list[MAX_PARTICIPANTS];
		int p_nb = storeActiveParticipants(list, c, p->nb);

		if (p_nb == 0) {
			printf("Aucun participant");
			return;
		}

		sortIdsByScore(list, c->scores, p_nb);
		
		int pos = 1;
		for (int i=0; i<p_nb; i++) {
			int score = 0;
			int before_score = 0;
			int is_ex = isParticipantEx(c->scores, list, i, p_nb, &before_score, &score);
			if (before_score != score) {
				pos=i+1;
			}
			printf("%d%s", pos, is_ex?"ex":"");
			printf(" -> (%d) %s %s : %d point", list[i]+1, p->participants[list[i]].prenom,  p->participants[list[i]].nom, score);
			plural(score);
			int diff = c->positions[list[i]]-pos;
			printf(" : ");
			if (diff >= 0) {
				printf("+");
			}
			printf("%d place", diff);
			c->positions[list[i]] = pos;
			plural(diff);
			if (i < p_nb-1) {
				printf("\n");
			}
		}
	}
	else {
		printf("Identifiant incorrect");
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

///Detection des commandes
void cmdProcess(char input[], Contexte* context) {

	Arguments args;
	args = cmdProcessArgs(input);
	char commande[MAX_TEXTE];
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
			printf("CREER [TITRE]");
		}
	}
	//CLASSEMENT
	else if (!strcmp(commande, "CLASSEMENT")) {
		if (args.nb == 2) {
			CmdClassement(args.values[1], context);
		}
		else {
			printf("CLASSEMENT [ID CONCOUR]");
		}
	}
	//SCORES
	else if (!strcmp(commande, "SCORES")) {
		if (args.nb == 2) {
			CmdScores(args.values[1], context);
		}
		else {
			printf("SCORES [ID PARTICIPANT]");
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
	//COMPARAISON
	else if (!strcmp(commande, "COMPARAISON")) {
		if (args.nb == 3) {
			CmdComparaison(args.values[1], args.values[2], context);
		}
		else {
			printf("COMPARAISON [CONCOUR_1] [CONCOUR_2]");
		}
	}
	//BILAN
	else if (!strcmp(commande, "BILAN")) {
		if (args.nb == 2) {
			cmdBilan(args.values[1], context);
		}
		else {
			printf("BILAN [ID_CONCOUR]"); 
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
		char commande[MAX_CMD] = "";
		fgets(commande, MAX_CMD, stdin);
		cmdProcess(commande, &contexte);
	}
}
