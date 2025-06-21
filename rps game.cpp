#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define fname "players.txt"

FILE *fp;

struct entity {
	int attack, defense, speed, hp;
	char className[10];
};

//function to randomize stat
int randomizeStat(int base) {
	int variation = base / 5; // ±20% variation
	int min = base - variation;
	int max = base + variation;
	if (min < 1) min = 1; // Avoid negative stats
	return rand() % (max - min + 1) + min;
}


//generates a random enemy with random stats
struct entity generateEnemy(struct entity *player) {
	struct entity enemy;
		enemy.attack  = randomizeStat(player->attack);
		enemy.defense = randomizeStat(player->defense);
		enemy.speed   = randomizeStat(player->speed);
		enemy.hp      = randomizeStat(player->hp);
	return enemy;
};

//function prototype
void mainmenu (struct entity *player);
void gamemenu(char pName[], struct entity *player);

//function to clear the console when enter is pressed
void clearCls () {
	while (getchar() != '\n'); getchar(); system("cls");
}

//validates the opened file
void validateFile (FILE *fp) {
	if (fp == NULL) { printf("Unable to access file"); return; }
}

//function to save player
int saveFile (char pName[], struct entity *player) {
	fp = fopen(fname, "a"); validateFile(fp);
	//prints the values in the txt
	int result = fprintf(fp ,"%s|%s|%d|%d|%d|%d\n", pName, player->className, player->attack, player->defense, player->speed, player->hp);
	fclose(fp); return result;
}

//function to load in the player
int loadFile (char pName[], struct entity *player) {
	fp = fopen(fname, "r"); validateFile(fp);
	char line[100], tempname[20]; //stores the whole line in file till format completes

	//fgets reads the whole line and stores it in char till \n is detected
	while(fgets(line, sizeof(line), fp)) {
		//sscanf scans the file into a string and then parses it into variables
		sscanf(line, "%[^|]", tempname);
		//strcmp returns 0 if match
		if (strcmp(pName, tempname) == 0) {
			sscanf(line, "%[^|]|%[^|]|%d|%d|%d|%d", pName, player->className, &player->attack, &player->defense, &player->speed, &player->hp);
			fclose(fp); return 1; // if found
		}
	}
	fclose(fp); return 0; //if not found
}

//function to make character
void makeCharacter(char pName[], struct entity *player) {
	int choice,atk, def, spd, hp;
	char className[10];

	system("cls");
	printf("\nWelcome, %s!. Please choose your class.\n", pName);

	//ensures the user chooses a class
	while (1) {
		printf("\n----------------------------- Classes -----------------------------\n"
				"%-20s %10s %10s %10s %10s\n"
				"-------------------------------------------------------------------\n", "Class Name", "Atk", "Def", "Spd", "HP");
		printf("%-20s %10d %10d %10d %10d\n", "1. Knight", 70, 90, 60, 280);
		printf("%-20s %10d %10d %10d %10d\n", "2. Lancer", 90, 45, 100, 265);
		printf("%-20s %10d %10d %10d %10d\n", "3. Viking", 60,110, 30, 300);

		printf("\n---> Enter choice (1 / 2 / 3): ");

		if (scanf("%d", &choice) != 1) { //checks if the input is a number
			printf("Invalid input! Please enter a number.\n");
			while (getchar() != '\n'); continue;
		}

		if (choice > 3 || choice < 0) { printf("Invalid choice, try again.\n"); }
		break;
	}

	switch (choice) {
		case 1:
			strcpy(className, "knight"); atk = 70; def = 90; spd = 60; hp = 280; break;
		case 2:
			strcpy(className, "lancer"); atk = 90; def = 45; spd = 100; hp = 265; break;
		case 3:
			strcpy(className, "viking"); atk = 60; def = 110; spd = 30; hp = 300; break;
		default:
			printf("Unknown error occurred\n"); return;
	}

	//saves the values to+ the struct
	//additional note: to access the content of a pointer struct, use ->
	strcpy(player->className, className);
	player->attack = atk;
	player->defense = def;
	player->speed = spd;
	player->hp = hp;

	//checks if theplayer is saved successfully
	if(saveFile(pName, player) > 0) {
		printf("\n%s chose %s!\nStats - Atk: %d, Def: %d, Spd: %d, HP: %d\n", pName, className, atk, def, spd, hp);
		printf("-> Character saved! press enter to continue");

		clearCls(); mainmenu(player);
	}
}

//function to display stats in battle phase
void displayStats (struct entity *enemy, char pName[], struct entity *player) {
		printf("\n-----------------------------------------------------------------\n"
				"%-20s %10s %10s %10s %10s\n", "Entity", "Atk", "Def", "Spd", "HP"
				"-------------------------------------------------------------------\n");
		printf("%-20s %10d %10d %10d %10d\n", pName, player->attack, player->defense, player->speed, player->hp);
		printf("%-20s %10d %10d %10d %10d\n\n", "Enemy", enemy->attack, enemy->defense, enemy->speed, enemy->hp);
}

//function to display choice in battle phase
void evaluateChoice (int choice, char turn[]) {
	switch (choice) {
		case 1:
			strcpy(turn, "rock"); break;
		case 2:
			strcpy(turn, "paper"); break;
		case 3:
			strcpy(turn, "scissors"); break;
		default:
			printf("Invalid choice"); break;
	}
}

//function to calculate damage
void attackCalc(struct entity *attacker, struct entity *defender, const char *attackerName, const char *defenderName) {
    int damage = attacker->attack - (defender->defense / 2);
	    if (damage < 0) damage = 0;
		defender->hp -= damage;
    printf("-> %s attacks! %s takes %d damage.\n", attackerName, defenderName, damage);
    printf("%s has %d hp left.\n", defenderName, defender->hp);
}

//function to calculate dodge rate
int dodgeCalc (int speedDiff, struct entity *player, struct entity enemy) {
	int dodgeChance = 30 + (speedDiff * 100 / (enemy.speed + player->speed));
		if (dodgeChance < 5) dodgeChance = 5;
		if (dodgeChance > 50) dodgeChance = 50;
	return dodgeChance;
}

//battle phease function0
void battlePhase (char pName[], struct entity *player) {
	//generates random enemy stats
	struct entity enemy = generateEnemy(player);
	char p[10], e[10];

	while (player->hp > 0 && enemy.hp > 0) {
		srand(time(NULL));
		displayStats(&enemy, pName, player);
		printf("1 = Rock\n"
				"2 = Paper\n"
				"3 = Scissors\n\n");

		printf("Please choose your move: ( 1 / 2 / 3 ): ");
		int playerChoice,enemyChoice = rand() % 3 + 1;
		scanf("%d", &playerChoice);

		evaluateChoice(playerChoice, p); evaluateChoice(enemyChoice, e);

		system("cls");
		printf("-> %s chose %s.\n", pName, p);
		printf("-> Enemy chose %s.\n\n", e);

		// RPS choices: 1=Rock, 2=Paper, 3=Scissors
		if (playerChoice == enemyChoice) {
			printf("-> It's a draw! No one attacks.\n");
		} else if ((playerChoice == 1 && enemyChoice == 3) ||
		           (playerChoice == 2 && enemyChoice == 1) ||
		           (playerChoice == 3 && enemyChoice == 2)) {
			// Player wins RPS ? Player attacks
			int speedDiff = enemy.speed - player->speed;
	  		int dodgeChance = dodgeCalc(speedDiff, player, enemy);

			if (rand() % 100 < dodgeChance) {
			    printf("-> Enemy dodged the attack!\n");
			} else {
			    attackCalc(player, &enemy, pName, "Enemy");
			}
		} else {
			// Enemy wins RPS ? Enemy attacks
			int speedDiff = player->speed - enemy.speed;
   			int dodgeChance = dodgeCalc(speedDiff, player, enemy);

			if (rand() % 100 < dodgeChance) {
			    printf("-> Player dodged the attack!\n");
			} else {
			    attackCalc(&enemy, player, "Enemy", pName);
			}
		}
		// Optional pause between rounds
		printf("\n---> Press Enter to continue to next round...");
		clearCls();
	}

	// Win logic after loop
	if (player->hp <= 0 && enemy.hp <= 0) {
		printf("It's a draw! Both you and the enemy fell.\n");
	} else if (enemy.hp <= 0) {
		printf("You won the battle! Enemy defeated.\n");
	} else {
		printf("You were defeated by the enemy...\n");
	}

	// Wait before returning to game menu
	printf("\n---> Press Enter to return to menu...");
	clearCls(); gamemenu(pName, player);
}


//game menu function after login
void gamemenu(char pName[], struct entity *player) {
	loadFile(pName, player);
	int choice;

	printf("--------------- ------------------------ ---------------\n"
			"-------------------- Prototype Game --------------------\n"
			"--------------- ------------------------ ---------------\n\n"
			"-> Welcome back, %s\n\n", pName);
	printf("1. To Battle\n"
			"2. Upgrade\n"
			"3. Stats\n"
			"4. Main Menu\n");

	printf("\n---> Enter action: ");
	//checks if choice is a number
	if (scanf("%d", &choice) != 1) {
		printf("Invalid input.\n");
		return;
	}

	while (getchar() != '\n');

	switch (choice) {
		case 1:
			system("cls"); battlePhase(pName, player);
			break;
		case 2:
			system("cls");
			printf("Sorry, this feature isn't available yet.\n\n");
			printf("\n---> Press enter to go back");
			clearCls(); gamemenu(pName, player);
			break;
		case 3:
			system("cls");
			printf("---------- %s's current stats ----------\n\n"
					"-> Class: %s\n"
					"-> Attack: %d\n"
					"-> Defense: %d\n"
					"-> Speed:%d\n"
					"-> Health Points: %d\n\n"
					"----------- ------------------ -----------\n", pName, player->className, player->attack, player->defense, player->speed, player->hp);
			printf("\n---> Press enter to go back");
			clearCls(); gamemenu(pName, player);
			break;
		case 4:
			system("cls"); mainmenu(player);
			break;
		default:
			printf("Feature not available yet.\n");
			break;
	}
}

//main menu function
void mainmenu (struct entity *player) {
	int choice;
	char pName[20];

	printf("--------------- ------------------------ ---------------\n"
			"---------- Welcome to Jebril's Prototype Game ----------\n"
			"--------------- ------------------------ ---------------\n\n"
			"1. Create new character\n"
			"2. Load character\n"
			"3. About\n");

	printf("\n---> Enter action: ");
	//checks if choice is a number
	if (scanf("%d", &choice) != 1) {
		printf("Invalid input.\n");
		return;
	}
	while (getchar() != '\n');

	switch (choice) {
		case 1:
			printf("\nPlease enter name: ");
			scanf(" %[^\n]", pName);
			makeCharacter(pName, player);
			break;
		case 2:
			while (1) {
				printf("\nPlease enter name: ");
				scanf(" %[^\n]", pName);

				if(loadFile(pName, player) == 0) {
					printf("Character does not exist\n");
					continue;
				}
				system("cls"); gamemenu(pName, player);
				break;
			}
			break;
		case 3:
			system("cls");
			printf("-------------------- ------------------------ --------------------\n\n"
					"This prototype game is made by Jebril Carlos to practice C\n"
					"\nCreate a character and select a class, \nFight monsters with randomized stats that match yours\n"
					"Pick between rock, paper, and scissors. Will your luck prevail?\n"
					"Level up to fight even tougher battles.\n\n"
					"Ps. The upgrade feature and class mechanics is kinda useless as of now.\n\n"
					"-------------------- ------------------------ --------------------\n\n");
			printf("\n---> Press enter to go back");
			clearCls();	mainmenu(player);
			break;
		default:
			printf("Feature not available yet.\n");
			break;
	}
	return;
}

int main() {
	srand(time(NULL));
	struct entity player;
	//pass the player address once
	mainmenu(&player);
}