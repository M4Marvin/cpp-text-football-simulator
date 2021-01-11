#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <sstream>
#include <algorithm>
#include <iterator>

#define ROW 10
#define COL 6

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class Game;
class Map;
class Team;
class Ball;
class Player;

class Map
{
public:
	Map();

	void printMap();
	bool isEmpty(int, int);
	void setPositions(int, int, string);
	string getMapContent(int, int);

private:
	string map[ROW][COL];
};

class Team
{
public:
	Team(int, int, int, int, string, Game *);

	void action();
	void initVectorTeam(string);

	// setters
	void setName(string);
	void setPasses(int);
	void setMistakes(int);
	void setGoals(int);
	void setSteals(int);

	void readFile(string);

	// getters
	int getPasses();
	int getMistakes();
	int getGoals();
	int getSteals();
	int chooseAction(); /* we randomly select one of the xx moves*/
	string getName();
	Game *getGameInstance(); /* Returns a pointer to a game object*/
	vector<Player *> getPlayersVector();

	bool isInSameTeam(Player *p1, Player *p2);
	bool isInRedTeam(Player *p);
	bool isInBlueTeam(Player *p);

	vector<string> split(const string &s, char delim);
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

	void printTeamInfo();

private:
	string name;			  /* Team name */
	int passes;				  /* passes */
	int mistakes;			  /* Mistakes */
	int goals;				  /* Goal */
	int steals;				  /* Thefts */
	Game *game;				  /* Object came */
	vector<Player *> players; /* Vector with the players */
};

class Ball
{
public:
	// constructor
	Ball(int, int, string, Player *, Player *, Game *);

	void assignBall(Player *);
	void assignBall();
	void changeBall(int, int);

	// setters
	void setX(int);
	void setY(int);
	void setID(string);
	void setCurrentPlayer(Player *);
	void setPreviousPlayer(Player *);

	// getters
	int getX() const;
	int getY() const;
	string getID();
	Player *getCurrentPlayer() const;  /* Returns the player who has the ball */
	Player *getPreviousPlayer() const; /* Returns the player who had the ball */

private:
	int x; /* Coordinate x of the ball */
	int y; /* Coordinate the ball*/
	string id;
	Player *currentPlayer;	/* He points to the player who has the ball */
	Player *previousPlayer; /* It points to the previous player who had the ball */
	Game *game;				/* Pointer to an object */
};

class Player
{
public:
	Player(int, int, int, int, int, string, Game *);

	// setters
	void setX(int);
	void setY(int);
	void setPlayerNum(int);
	void setTargetCol(int);
	void setMoveCol(int);
	void setName(string);
	void setID(string);

	// getters
	int getX();
	int getY();
	int getPlayerNum();
	int getTargetCol();
	int getMoveCol();
	string getName();
	string getID();
	Game *getGameInstance();
	Player *getPlayer(int, int);

	bool hasBall();

	void move();
	virtual void printPlayerInfo();
	virtual void transfer();
	virtual void specialMove() = 0;

private:
	int x;
	int y;
	int playerNum; /* jersey number */
	int targetCol; /* line target*/
	int moveCol;   /* Linear Motion*/
	string name;   /* name of player*/
	string id;
	Game *game;
};

class Defender : public Player
{
public:
	Defender(int, int, int, int, int, string, Game *, bool);

	void setYellowCard(bool);
	bool hasYellowCard();

	virtual void specialMove();

private:
	bool yellowCard;
	string id;
};

class Striker : public Player
{
public:
	Striker(int, int, int, int, int, string, Game *);

	virtual void transfer();
	virtual void specialMove();

private:
	//string id;
};

class Game
{
public:
	// Constructor
	Game();

	void init();
	void start();
	void runTurn();
	void placeOnMap();
	void printPlayers();
	bool scored();
	void sentra(string);

	// setters
	void setRounds(int);
	void setGoals(int);

	// getters
	int getRounds();
	int getGoals();
	Team *getTeam(string);
	Ball *getBall();
	Map *getMap();

private:
	int rounds;
	int goals;
	int ballRounds;
	Map *map;
	Team *redTeam;
	Team *blueTeam;
	Ball *ball;
	vector<Player *> redPlayers;
	vector<Player *> bluePlayers;
};
int main(void)
{
	// We create a game object

	Game *game = new Game();

	system("read -p 'Press Enter to continue...' var");
}

Ball::Ball(int x, int y, string id, Player *pPlayer, Player *cPlayer, Game *game)
{
	setX(x);
	setY(y);
	setID(id);
	this->game = game;
	setCurrentPlayer(cPlayer);
	setPreviousPlayer(pPlayer);
}

void Ball::assignBall(Player *p)
{
	Map *map = game->getMap();
	// If the ball is free and you are in an adjacent position from the player we assign it to them

	if (currentPlayer == NULL)
	{
		// Check if it is next to the player
		if (getX() == p->getX() && (abs(getY() - p->getY()) == 1))
		{
			map->setPositions(getX(), getY(), "---");
			// We change the coordinates of the ball

			// We change the coordinates of the ball
			setX(p->getX());
			setY(p->getY());

			// We find which player is already there
			string s = map->getMapContent(p->getX(), p->getY());

			// We want to change the name of the player in that position

			p->setID(s.substr(0, 1) + s.substr(1, 1) + "*");

			// We depict it on paper
			map->setPositions(p->getX(), p->getY(), p->getID());
			setCurrentPlayer(p);
			//setPreviousPlayer(NULL);
			cout << p->getPlayerNum() << " -> was shocked" << endl;
		}

		setPreviousPlayer(NULL);
	}
}

void Ball::assignBall()
{
	// We are looking to find the closest player
	Team *redTeam = game->getTeam("red");
	vector<Player *> redPlayers = redTeam->getPlayersVector();
	Team *blueTeam = game->getTeam("blue");
	vector<Player *> bluePlayers = blueTeam->getPlayersVector();
	Map *map = game->getMap();

	int min = 10;
	int playerIndex = -1;

	cout << "assignball()!" << endl;
	// List of nearby players
	vector<Player *> nearestPlayers;

	for (int i = 0; i < redPlayers.size(); i++)
	{

		cout << redPlayers[i]->getPlayerNum() << endl;
		if (getX() == redPlayers[i]->getX())
		{
			cout << "mpika yo" << endl;
			nearestPlayers.push_back(redPlayers[i]);
		}
	}

	for (int i = 0; i < bluePlayers.size(); i++)
	{
		cout << bluePlayers[i]->getPlayerNum() << endl;
		cout << "player x = " << bluePlayers[i]->getX() << endl;
		if (getX() == bluePlayers[i]->getX())
		{
			cout << "mpika" << endl;
			nearestPlayers.push_back(bluePlayers[i]);
		}
	}

	cout << "SIZE = " << nearestPlayers.size() << endl;
	if (nearestPlayers.size() == 0)
		return;

	for (int i = 0; i < nearestPlayers.size(); i++)
	{
		cout << nearestPlayers[i]->getY() << endl;
		if (abs(getY() - nearestPlayers[i]->getY()) < min)
		{
			min = abs(getY() - nearestPlayers[i]->getY());
			playerIndex = i;
		}
	}
	cout << "index" << playerIndex << endl;

	string s = map->getMapContent(nearestPlayers[playerIndex]->getX(), nearestPlayers[playerIndex]->getY());

	cout << "s: " << s << endl;
	//We want to change the name of the player in that position
	nearestPlayers[playerIndex]->setID(s.substr(0, 1) + s.substr(1, 1) + "*");

	//We depict it on paper
	map->setPositions(getX(), getY(), "---");
	setX(nearestPlayers[playerIndex]->getX());
	setY(nearestPlayers[playerIndex]->getY());
	map->setPositions(nearestPlayers[playerIndex]->getX(), nearestPlayers[playerIndex]->getY(), nearestPlayers[playerIndex]->getID());

	//if (nearestPlayers.size() == 0) return;
	setCurrentPlayer(nearestPlayers[playerIndex]);
	cout << nearestPlayers[playerIndex]->getPlayerNum() << " -> was shocked!" << endl;
}

// setters functions
void Ball::setX(int x) { this->x = x; }
void Ball::setID(string id) { this->id = id; }
void Ball::setY(int y) { this->y = y; }
void Ball::setPreviousPlayer(Player *p) { previousPlayer = p; }

void Ball::changeBall(int targetX, int targetY)
{
	Map *map = game->getMap();
	Player *p = getCurrentPlayer();

	// We change the player's id so that he no longer holds the ball
	string s = map->getMapContent(p->getX(), p->getY());
	p->setID(s.substr(0, 1) + s.substr(1, 1) + "-");

	// we put it on the map
	map->setPositions(getX(), getY(), p->getID());

	// We send the ball there
	setX(targetX);
	setY(targetY);

	// We check if the position where the ball will go is free
	if (map->isEmpty(getX(), getY()))
	{
		// We put the ball in its new position on the paper
		map->setPositions(getX(), getY(), "***");

		// we set the ball free
		setPreviousPlayer(p);
		setCurrentPlayer(NULL);

		// increase errors
		Team *redTeam = game->getTeam("red");
		Team *blueTeam = game->getTeam("blue");

		if (!((getX() == 0 || getX() == 9) && (getY() == 2 || getY() == 3)))
		{
			// We increase the errors of PreviPlayer
			if (redTeam->isInRedTeam(getPreviousPlayer()))
				redTeam->setMistakes(redTeam->getMistakes() + 1);
			else if (blueTeam->isInBlueTeam(getPreviousPlayer()))
				blueTeam->setMistakes(blueTeam->getMistakes() + 1);
		}
	}
	else
	{
		// We find which player is already there
		string s = map->getMapContent(getX(), getY());

		// We want to change the name of the player in that position
		Player *newPlayer = p->getPlayer(getX(), getY());
		newPlayer->setID(s.substr(0, 1) + s.substr(1, 1) + "*");

		// We depict it on paper
		map->setPositions(newPlayer->getX(), newPlayer->getY(), newPlayer->getID());

		// We nominate the new player
		setPreviousPlayer(p);
		setCurrentPlayer(newPlayer);

		if (!((getX() == 0 || getX() == 9) && (getY() == 2 || getY() == 3)))
		{
			Team *redTeam = game->getTeam("red");
			Team *blueTeam = game->getTeam("blue");

			if (redTeam->isInSameTeam(getPreviousPlayer(), getCurrentPlayer()))
			{
				redTeam->setPasses(redTeam->getPasses() + 1);
			}
			else if (blueTeam->isInSameTeam(getPreviousPlayer(), getCurrentPlayer()))
			{
				blueTeam->setPasses(blueTeam->getPasses() + 1);
			}
			else
			{
				// We increase the errors of Previous Player
				if (redTeam->isInRedTeam(getPreviousPlayer()))
					redTeam->setMistakes(redTeam->getMistakes() + 1);
				else if (blueTeam->isInBlueTeam(getPreviousPlayer()))
					blueTeam->setMistakes(blueTeam->getMistakes() + 1);
			}
		}
	}
}

void Ball::setCurrentPlayer(Player *p)
{
	if (p == NULL)
		currentPlayer = NULL;
	else
		currentPlayer = p;
}

// getters functions
int Ball::getX() const { return x; }
int Ball::getY() const { return y; }
string Ball::getID() { return id; }
Player *Ball::getCurrentPlayer() const { return currentPlayer; }
Player *Ball::getPreviousPlayer() const { return previousPlayer; }

Defender::Defender(int x, int y, int pNum, int tCol, int mCol, string name, Game *game, bool yCard)
	: Player(x, y, pNum, tCol, mCol, name, game)
{
	setYellowCard(yCard);
}

void Defender::specialMove()
{
	// Check if there is an opposing player with the ball in the next square
	Game *game = getGameInstance();
	Team *redTeam = game->getTeam("red");
	Team *blueTeam = game->getTeam("blue");
	Ball *ball = game->getBall();
	Map *map = game->getMap();

	bool blueFlag = false;
	bool redFlag = false;

	// Check if I already have the ball
	if (hasBall())
		return;

	// Check if there is an opponent close to the ball
	// check who this player belongs to
	if (redTeam->isInRedTeam(this))
	{
		// opens to red
		// so we are looking at the blue players
		// we are looking to find out if there is a blue player next to him with the ball
		vector<Player *> bluePlayers = blueTeam->getPlayersVector();
		for (int i = 0; i < bluePlayers.size(); i++)
		{
			// we check if there is a player on the left
			if (getY() - bluePlayers[i]->getY() == 1 && getX() == bluePlayers[i]->getX() && bluePlayers[i] == ball->getCurrentPlayer())
			{
				blueFlag = true;
			}
			else if (bluePlayers[i]->getY() - getY() == 1 && getX() == bluePlayers[i]->getX() && bluePlayers[i] == ball->getCurrentPlayer())
			{
				blueFlag = true;
			}
			else if (getX() - bluePlayers[i]->getX() == 1 && getX() == bluePlayers[i]->getX() && bluePlayers[i] == ball->getCurrentPlayer())
			{
				blueFlag = true;
			}
			else if (bluePlayers[i]->getX() - getX() == 1 && getX() == bluePlayers[i]->getX() && bluePlayers[i] == ball->getCurrentPlayer())
			{
				blueFlag = true;
			}

			// we finally found an opponent player with the ball
			if (blueFlag)
			{

				int p = 1 + rand() % 100;

				if (p <= 70)
				{
					// steal the ball
					cout << "Player " << getPlayerNum() << " stole the ball" << endl;
					ball->setPreviousPlayer(bluePlayers[i]);
					ball->setCurrentPlayer(this);

					// increase the thefts by one
					redTeam->setSteals(redTeam->getSteals() + 1);
				}
				else
				{
					int yellow = 1 + rand() % 100;

					if (yellow <= 20)
					{
						// eats yellow
						// we check if it is already yellow
						if (hasYellowCard())
						{
							//we show it from the simulation
							cout << "Player " << getPlayerNum() << " got a red card" << endl;

							//erase it from the paper
							map->setPositions(getX(), getY(), "---");

							vector<Player *> redPlayers = redTeam->getPlayersVector();

							// delete him from the list
							for (int i = 0; i < redPlayers.size(); i++)
							{
								if (getX() == redPlayers[i]->getX() && getY() == redPlayers[i]->getY())
								{
									redPlayers.erase(redPlayers.begin() + i);
								}
							}
						}
						else
						{
							cout << "Player " << getPlayerNum() << " got a yellow card" << endl;
							setYellowCard(true);
						}
					}
				}
			}
		}
	}
	else
	{
		// opens in blue
		// so we are looking for red players
		// we are looking to find if there is a red player next to him with the ball
		vector<Player *> redPlayers = redTeam->getPlayersVector();
		for (int i = 0; i < redPlayers.size(); i++)
		{
			// we check if there is a player on the left
			if (getY() - redPlayers[i]->getY() == 1 && getX() == redPlayers[i]->getX() && redPlayers[i] == ball->getCurrentPlayer())
			{
				redFlag = true;
			}
			else if (redPlayers[i]->getY() - getY() == 1 && getX() == redPlayers[i]->getX() && redPlayers[i] == ball->getCurrentPlayer())
			{
				redFlag = true;
			}
			else if (getX() - redPlayers[i]->getX() == 1 && getX() == redPlayers[i]->getX() && redPlayers[i] == ball->getCurrentPlayer())
			{
				redFlag = true;
			}
			else if (redPlayers[i]->getX() - getX() == 1 && getX() == redPlayers[i]->getX() && redPlayers[i] == ball->getCurrentPlayer())
			{
				redFlag = true;
			}

			// we finally found an opponent player with the ball
			if (redFlag)
			{
				int p = 1 + rand() % 100;

				if (p <= 70)
				{
					// steal the ball
					ball->changeBall(getX(), getY());

					// increase the thefts by one
					blueTeam->setSteals(blueTeam->getSteals() + 1);
					redFlag = true;
				}
				else
				{
					int yellow = 1 + rand() % 100;

					if (yellow <= 20)
					{
						// eats yellow
						// we check if it is already yellow
						if (hasYellowCard())
						{
							// we show it from the simulation
							cout << "O paixtis " << getPlayerNum() << " pire kokkini karta" << endl;

							// erase it from the paper
							map->setPositions(getX(), getY(), "---");

							vector<Player *> bluePlayers = blueTeam->getPlayersVector();

							// delete him from the list
							for (int i = 0; i < bluePlayers.size(); i++)
							{
								if (getX() == bluePlayers[i]->getX() && getY() == bluePlayers[i]->getY())
								{
									bluePlayers.erase(bluePlayers.begin() + i);
								}
							}
							redFlag = false;
						}
						else
						{
							cout << "Player " << getPlayerNum() << "got a yellow card." << endl;
							setYellowCard(true);
							redFlag = false;
						}
					}
				}
			}
		}
	}
}

void Defender::setYellowCard(bool yCard) { this->yellowCard = yCard; }
bool Defender::hasYellowCard() { return yellowCard; }

Game::Game()
{
	// random number generator
	srand((unsigned)time(0));

	// initialization function
	init();
}

void Game::init()
{
	// init map
	map = new Map();

	// init readTeam
	redTeam = new Team(0, 0, 0, 0, "red", this);
	redPlayers = redTeam->getPlayersVector();

	// init blueTeam
	blueTeam = new Team(0, 0, 0, 0, "blue", this);
	bluePlayers = blueTeam->getPlayersVector();

	// init ball
	ball = new Ball(1, 1, "***", NULL, NULL, this);

	// init statistics
	setRounds(15);
	setGoals(7);

	// placeOnMap
	placeOnMap();

	// printMap
	map->printMap();

	cout << "name\tPasses\tmistakes\tgoal\tThefts" << endl;
	blueTeam->printTeamInfo();
	cout << endl;

	cout << "Name\tJeresey\tTarget\tTraffic\tID" << endl;
	for (int i = 0; i < bluePlayers.size(); i++)
	{
		bluePlayers[i]->printPlayerInfo();
	}

	// the simulation begins
	start();
}

void Game::start()
{
	int roundsPassed = 0;
	int choice = 1;
	int x = ball->getX();
	int y = ball->getY();
	// game loop
	while (roundsPassed < getRounds() && choice == 1)
	{

		do
		{
			// 1. runTurn();
			runTurn();

			// 2. We check if a team reached 7 goals
			if (blueTeam->getGoals() == 7)
			{
				cout << "Blue Team Wins!" << endl;
				exit(1);
			}
			else if (redTeam->getGoals() == 7)
			{
				cout << "Red Team Wins!" << endl;
				exit(1);
			}

			// we increase the rounds
			roundsPassed++;

			map->printMap();

			cout << "Name Passes Mistakes  Goals  Thefts" << endl;
			blueTeam->printTeamInfo();
			redTeam->printTeamInfo();

			if (x == ball->getX() && y == ball->getY())
			{
				if (ball->getCurrentPlayer() == NULL)
				{
					ballRounds++;
				}
			}
			else
			{
				x = ball->getX();
				y = ball->getY();
				ballRounds = 0;
			}

			cout << ballRounds << endl;
			cout << "Do you want to continue (Press 1): ";
			cin >> choice;
		} while (choice == 1);
	}
}

void Game::runTurn()
{
	// Choose at random which team will play first
	int randomTeam = 1 + (rand() % 2);

	// 1 - RedTeam
	// 2 - BlueTeam
	if (randomTeam == 1)
	{
		cout << "\n\nRedTeam action()" << endl;
		redTeam->action();
		cout << "\n\nBlueTeam action()" << endl;
		blueTeam->action();
	}
	else
	{
		cout << "\n\nBlueTeam action()" << endl;
		blueTeam->action();
		cout << "\n\nRedTeam action()" << endl;
		redTeam->action();
	}

	// We check if a team scored a goal, otherwise we call assignVall ()
	// we have to check if the ball is next to one of the goals
	if ((ball->getX() == 0 || ball->getX() == 9) && (ball->getY() == 2 || ball->getY() == 3))
	{
		if (!scored())
			ball->assignBall();
	}
}

bool Game::scored()
{
	// We are checking if retTeam scored
	if ((ball->getX() == 0) && (ball->getY() == 2 || ball->getY() == 3))
	{
		int goalPropability = 1 + (rand() % 100);

		if (goalPropability <= 10)
		{
			redTeam->setGoals(redTeam->getGoals() + 1);
			cout << "Scorare h redTeam" << endl;
			sentra("blue");
			return true;
		}
	}
	else if ((ball->getX() == 9) && (ball->getY() == 2 || ball->getY() == 3))
	{
		int goalPropability = 1 + (rand() % 100);

		if (goalPropability <= 10)
		{
			blueTeam->setGoals(blueTeam->getGoals() + 1);
			cout << "Scorare h blueTeam" << endl;

			// we randomly give the ball to a player of the blue team
			sentra("red");

			return true;
		}
	}

	return false;
}

void Game::sentra(string team)
{
	if (team.compare("red") == 0)
	{
		// scorareRed
		int rndPlayer = rand() % redPlayers.size();
		int x = redPlayers[rndPlayer]->getX();
		int y = redPlayers[rndPlayer]->getY();

		// we erase the ball
		map->setPositions(ball->getX(), ball->getY(), "---");

		ball->setX(x);
		ball->setY(y);

		// We find which player is already there
		string s = map->getMapContent(x, y);

		// We want to change the name of the player in that position
		Player *newPlayer = redPlayers[rndPlayer]->getPlayer(x, y);
		newPlayer->setID(s.substr(0, 1) + s.substr(1, 1) + "*");

		// We depict it on paper
		map->setPositions(newPlayer->getX(), newPlayer->getY(), newPlayer->getID());

		// We nominate the new player
		ball->setPreviousPlayer(NULL);
		ball->setCurrentPlayer(newPlayer);
	}
	else
	{
		// scorareBlue
		// scorareRed
		int rndPlayer = rand() % bluePlayers.size();
		int x = bluePlayers[rndPlayer]->getX();
		int y = bluePlayers[rndPlayer]->getY();

		// I erase the ball
		map->setPositions(ball->getX(), ball->getY(), "---");

		ball->setX(x);
		ball->setY(y);

		// We find which player is already there
		string s = map->getMapContent(x, y);

		// We want to change the name of the player in that position
		Player *newPlayer = bluePlayers[rndPlayer]->getPlayer(x, y);
		newPlayer->setID(s.substr(0, 1) + s.substr(1, 1) + "*");

		// We depict it on paper
		map->setPositions(newPlayer->getX(), newPlayer->getY(), newPlayer->getID());

		// We nominate the new player
		ball->setPreviousPlayer(NULL);
		ball->setCurrentPlayer(newPlayer);
	}
}

void Game::placeOnMap()
{
	// place redTeam on map
	for (int i = 0; i < redPlayers.size(); i++)
	{
		map->setPositions(redPlayers[i]->getX(), redPlayers[i]->getY(), redPlayers[i]->getID());
	}

	// place blueTeam on map
	for (int i = 0; i < bluePlayers.size(); i++)
	{
		map->setPositions(bluePlayers[i]->getX(), bluePlayers[i]->getY(), bluePlayers[i]->getID());
	}

	// place ball on map
	map->setPositions(ball->getX(), ball->getY(), ball->getID());
}

// setters-getters
void Game::setRounds(int rounds) { this->rounds = rounds; }
void Game::setGoals(int goals) { this->goals = goals; }
int Game::getRounds() { return rounds; }
int Game::getGoals() { return goals; }
Map *Game::getMap() { return map; }
Ball *Game::getBall() { return ball; }

Team *Game::getTeam(string name)
{
	if (name.compare(redTeam->getName()) == 0)
	{
		// returned to redTeam
		return redTeam;
	}
	else if (name.compare(blueTeam->getName()) == 0)
	{
		// returned blueTeam
		return blueTeam;
	}
	return NULL;
}

void Game::printPlayers()
{
	for (int i = 0; i < redPlayers.size(); i++)
	{
		cout << redPlayers[i]->getName() << endl;
	}

	for (int i = 0; i < bluePlayers.size(); i++)
	{
		cout << bluePlayers[i]->getName() << endl;
	}
}

Map::Map()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			map[i][j] = "---";
		}
	}
}

void Map::printMap()
{
	cout<<endl;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			cout << map[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

bool Map::isEmpty(int x, int y)
{

	if (map[x][y].compare(0, 3, "---") == 0)
	{
		return true;
	}

	return false;
}

void Map::setPositions(int x, int y, string id)
{
	map[x][y] = id;
}

string Map::getMapContent(int x, int y)
{
	return map[x][y];
}

Player::Player(int x, int y, int pNum, int tCol, int mCol, string name, Game *game)
{
	setX(x);
	setY(y);
	setPlayerNum(pNum);
	setTargetCol(tCol);
	setMoveCol(mCol);
	setName(name);
	this->game = game;
}

void Player::move()
{
	Map *map = game->getMap();
	Ball *ball = game->getBall();
	bool flag = false;

	// 1 - left movement, 2 - movement Right
	int randomMove = 1 + (rand() % 2);

	if (randomMove == 1)
	{
		// Check if the position on our left is empty or if we will go out of bounds
		if (getY() == 0)
			return;
		if (map->isEmpty(getX(), getY() - 1) && getY() != 0)
		{
			if (hasBall())
			{
				flag = true;
			}
			// We erase the old position of the player on the paper
			map->setPositions(getX(), getY(), "---");
			setY(getY() - 1);

			// We put the new position of the player on the paper
			map->setPositions(getX(), getY(), getID());

			if (flag)
			{
				ball->setX(getX());
				ball->setY(getY());
			}
		}
	}
	else if (randomMove == 2)
	{
		// Check if the position on our right is empty or if we will go out of bounds
		if (getY() == 5)
			return;
		if (map->isEmpty(getX(), getY() + 1) && getY() != 5)
		{
			if (hasBall())
			{
				flag = true;
			}
			// We erase the old position of the player on the paper
			map->setPositions(getX(), getY(), "---");
			setY(getY() + 1);

			// We put the new position of the player on the paper
			map->setPositions(getX(), getY(), getID());

			if (flag)
			{
				ball->setX(getX());
				ball->setY(getY());
			}
		}
	}
}

void Player::transfer()
{
	Ball *ball = game->getBall();

	if (hasBall())
	{
		// We randomly select a column of the Target bar
		int randomCol = rand() % 6;
		ball->changeBall(getTargetCol(), randomCol);
		cout << "Edwse pasa stis : " << ball->getX() << "," << ball->getY() << endl;
	}
}

void Player::printPlayerInfo()
{
	cout << getName() << "\t" << getPlayerNum() << "\t" << getTargetCol()
		 << "\t" << getMoveCol() << "\t" << getID() << endl;
}

Player *Player::getPlayer(int x, int y)
{
	Team *redTeam = game->getTeam("red");
	Team *blueTeam = game->getTeam("blue");
	vector<Player *> redPlayers = redTeam->getPlayersVector();
	vector<Player *> bluePlayers = blueTeam->getPlayersVector();

	for (int i = 0; i < redPlayers.size(); i++)
	{
		if (redPlayers[i]->getX() == x && redPlayers[i]->getY() == y)
		{
			return redPlayers[i];
		}
		else if (bluePlayers[i]->getX() == x && bluePlayers[i]->getY() == y)
		{
			return bluePlayers[i];
		}
	}

	return NULL;
}

bool Player::hasBall()
{
	// We check if the player has the ball
	Ball *ball = game->getBall();

	if (getX() == ball->getX() && getY() == ball->getY())
		return true;

	return false;
}

// setters
void Player::setX(int x) { this->x = x; }
void Player::setY(int y) { this->y = y; }
void Player::setPlayerNum(int pNum) { this->playerNum = pNum; }
void Player::setTargetCol(int tCol) { this->targetCol = tCol; }
void Player::setMoveCol(int mCol) { this->moveCol = mCol; }
void Player::setName(string name) { this->name = name; }
void Player::setID(string id) { this->id = id; }

// getters
int Player::getX() { return x; }
int Player::getY() { return y; }
int Player::getPlayerNum() { return playerNum; }
int Player::getTargetCol() { return targetCol; }
int Player::getMoveCol() { return moveCol; }
string Player::getName() { return name; }
string Player::getID() { return id; }
Game *Player::getGameInstance() { return game; }

Striker::Striker(int x, int y, int pNum, int tCol, int mCol, string name, Game *game)
	: Player(x, y, pNum, tCol, mCol, name, game)
{
}

void Striker::transfer()
{
	Game *game = getGameInstance();
	Ball *ball = game->getBall();
	Map *map = game->getMap();

	// we check if he has the ball
	if (!hasBall())
		return;

	// with 70% chance of sending the ball to 2 central columns (2-3)
	int p = 1 + rand() % 100;

	if (p <= 70)
	{

		int q = 1 + rand() % 100;
		if (q <= 50)
		{
			// Any steel
			ball->changeBall(getTargetCol(), 2);
		}
		else
		{
			// these styles
			ball->changeBall(getTargetCol(), 3);
		}
	}
	else
	{
		// we randomly select one of the columns 0,1,4,5
		int q = 1 + rand() % 100;

		if (q <= 25)
		{
			// we choose0
			ball->changeBall(getTargetCol(), 0);
		}
		else if (q <= 50)
		{
			// we choose 1
			ball->changeBall(getTargetCol(), 1);
		}
		else if (q <= 75)
		{
			// we choose4
			ball->changeBall(getTargetCol(), 4);
		}
		else
		{
			// we choose5
			ball->changeBall(getTargetCol(), 5);
		}
	}
}

void Striker::specialMove()
{
	Game *game = getGameInstance();
	Ball *ball = game->getBall();
	Team *redTeam = game->getTeam("red");
	Team *blueTeam = game->getTeam("blue");
	vector<Player *> redPlayers = redTeam->getPlayersVector();
	vector<Player *> bluePlayers = blueTeam->getPlayersVector();
	vector<Player *> nearest1, nearest2, nearest3;
	Map *map = game->getMap();

	bool redFlag = false, blueFlag = false;

	// Check if I already have the ball
	if (hasBall())
		return;

	// we check if our teammate has the ball

	//to check
	if (redTeam->isInRedTeam(this))
	{
		// open in red
		for (int i = 0; i < redPlayers.size(); i++)
		{
			if (redPlayers[i]->getX() == ball->getX() && redPlayers[i]->getY() == ball->getY())
			{
				// move 2 Position Away From Our Nearest Opponent
				// we find the closest opponent (see team)
				for (int j = 0; j < bluePlayers.size(); j++)
				{
					// we are looking for an opponent on the same line
					if (bluePlayers[j]->getX() == getX())
					{
						if (abs(bluePlayers[j]->getY() - getY()) == 1)
						{
							nearest1.push_back(bluePlayers[j]);
							redFlag = true;
						}
						else if (abs(bluePlayers[j]->getY() - getY()) == 2)
						{
							nearest2.push_back(bluePlayers[j]);
							redFlag = true;
						}
						else if (abs(bluePlayers[j]->getY() - getY()) == 3)
						{
							nearest3.push_back(bluePlayers[j]);
							redFlag = true;
						}
					}
				}
			}

			if (redFlag)
			{

				if (nearest1.size() == 1)
				{
					if ((getY() > nearest1.at(0)->getY()) && (getY() + 2 <= 5))
					{
						// movoe the player 2 positions right
						// we erase old position
						map->setPositions(getX(), getY(), "---");
						setY(getY() + 2);
						map->setPositions(getX(), getY(), getID());
					}
					else if ((getY() < nearest1.at(0)->getY()) && (getY() - 2 >= 0))
					{
						//mooo Player2PositionsOnTheLeft
						map->setPositions(getX(), getY(), "---");
						setY(getY() - 2);
						map->setPositions(getX(), getY(), getID());
					}
				}
				else if (nearest2.size() == 1)
				{
					if ((getY() > nearest2.at(0)->getY()) && (getY() + 2 <= 5))
					{
						// move the player 2 theseAreTheTenth
						map->setPositions(getX(), getY(), "---");
						setY(getY() + 2);
						map->setPositions(getX(), getY(), getID());
					}
					else if ((getY() < nearest2.at(0)->getY()) && (getY() - 2 >= 0))
					{
						// move player 2 seats left
						map->setPositions(getX(), getY(), "---");
						setY(getY() - 2);
						map->setPositions(getX(), getY(), getID());
					}
				}
				else if (nearest3.size() == 1)
				{
					if ((getY() > nearest3.at(0)->getY()) && (getY() + 2 <= 5))
					{
						// move the player 2 theseAreTheTenth
						map->setPositions(getX(), getY(), "---");
						setY(getY() + 2);
						map->setPositions(getX(), getY(), getID());
					}
					else if ((getY() < nearest3.at(0)->getY()) && (getY() - 2 >= 0))
					{
						// move player 2 seats left
						map->setPositions(getX(), getY(), "---");
						setY(getY() - 2);
						map->setPositions(getX(), getY(), getID());
					}
				}
			}
		}
	}
	else if (blueTeam->isInBlueTeam(this))
	{

		// open in red
		for (int i = 0; i < bluePlayers.size(); i++)
		{
			if (bluePlayers[i]->getX() == ball->getX() && bluePlayers[i]->getY() == ball->getY())
			{
				// move 2 positions away from our nearest opponent
				// we find the closest opponent (see team)
				for (int j = 0; j < redPlayers.size(); j++)
				{
					// we are looking for an opponent on the same line
					if (redPlayers[j]->getX() == getX())
					{
						if (abs(redPlayers[j]->getY() - getY()) == 1)
						{
							nearest1.push_back(redPlayers[j]);
							blueFlag = true;
						}
						else if (abs(redPlayers[j]->getY() - getY()) == 2)
						{
							nearest2.push_back(redPlayers[j]);
							blueFlag = true;
						}
						else if (abs(redPlayers[j]->getY() - getY()) == 3)
						{
							nearest3.push_back(redPlayers[j]);
							blueFlag = true;
						}
					}
				}
			}
		}

		if (blueFlag)
		{
			// we have found opponents who are on the same line as us
			// and have a distance of 1.2, x

			// 1.if there is 1 with apostasi 1,2, 3

			// Distance 1
			if (nearest1.size() == 1)
			{
				if ((getY() > nearest1.at(0)->getY()) && (getY() + 2 <= 5))
				{
					// move the player 2 theseAreTheTenth
					map->setPositions(getX(), getY(), "---");
					setY(getY() + 2);
					map->setPositions(getX(), getY(), getID());
				}
				else if ((getY() < nearest1.at(0)->getY()) && (getY() - 2 >= 0))
				{
					// move player 2 seats left
					map->setPositions(getX(), getY(), "---");
					setY(getY() - 2);
					map->setPositions(getX(), getY(), getID());
				}
			}
			else if (nearest2.size() == 1)
			{
				if ((getY() > nearest2.at(0)->getY()) && (getY() + 2 <= 5))
				{
					// mooe the player 2 positions right
					map->setPositions(getX(), getY(), "---");
					setY(getY() + 2);
					map->setPositions(getX(), getY(), getID());
				}

				else if ((getY() < nearest2.at(0)->getY()) && (getY() - 2 >= 0))
				{
					// move player 2
					map->setPositions(getX(), getY(), "---");
					setY(getY() - 2);
					map->setPositions(getX(), getY(), getID());
				}
			}
			else if (nearest3.size() == 1)
			{
				if ((getY() > nearest3.at(0)->getY()) && (getY() + 2 <= 5))
				{
					//move the player 2
					map->setPositions(getX(), getY(), "---");
					setY(getY() + 2);
					map->setPositions(getX(), getY(), getID());
				}
				else if ((getY() < nearest3.at(0)->getY()) && (getY() - 2 >= 0))
				{
					// move player 2
					map->setPositions(getX(), getY(), "---");
					setY(getY() - 2);
					map->setPositions(getX(), getY(), getID());
				}
			}
		}
	}
}

Team::Team(int passes, int mistakes, int goals, int steals, string name, Game *game)
{
	setPasses(passes);
	setMistakes(mistakes);
	setGoals(goals);
	setName(name);
	setSteals(steals);
	this->game = game;

	//initVectorTeam(name);
	readFile(name);
}

void Team::action()
{
	Ball *ball = game->getBall();
	Map *map = game->getMap();

	for (int i = 0; i < players.size(); i++)
	{
		// We perform the actions of each player
		int move = chooseAction();

		switch (move)
		{

		case 1:
			cout << players[i]->getPlayerNum() << " -> "
				 << "move()" << endl;

			// Movement - move()
			players[i]->move();

			// We call insignificant to see if the player can get the ball
			ball->assignBall(players[i]);

			break;
		case 2:
			cout << players[i]->getPlayerNum() << " -> "
				 << "transfer()" << endl;

			//Transfer ()
			players[i]->transfer();

			// We call insignificant
			ball->assignBall(players[i]);

			break;

		case 3:
			cout << players[i]->getPlayerNum() << " -> "
				 << "specialMove()" << endl;

			// Traffic Special - SpecialMove ()
			players[i]->specialMove();

			// We call insignificant
			ball->assignBall(players[i]);

			break;
		}
	}
}

void Team::initVectorTeam(string name)
{
	if (name.compare("red") == 0)
	{
		string i = "R";
		Defender *def1 = new Defender(6, 0, 3, 1, 6, "pipis1", game, false);
		def1->setID(i + "3-");
		Defender *def2 = new Defender(7, 3, 4, 2, 7, "talak2", game, false);
		def2->setID(i + "4-");

		Defender *def3 = new Defender(8, 2, 5, 6, 8, "talak12", game, false);
		def3->setID(i + "5-");

		Striker *str1 = new Striker(1, 2, 1, 0, 1, "pipen3", game);
		str1->setID(i + "1-");
		Striker *str2 = new Striker(2, 3, 2, 0, 2, "talaken4", game);
		str2->setID(i + "2-");

		players.push_back(def1);
		players.push_back(def2);
		players.push_back(def3);
		players.push_back(str1);
		players.push_back(str2);
	}
	else if (name.compare("blue") == 0)
	{
		string i = "B";
		Defender *def1 = new Defender(1, 3, 1, 3, 1, "pipis6", game, false);
		def1->setID(i + "1-");
		Defender *def2 = new Defender(2, 2, 2, 7, 2, "talak6", game, false);
		def2->setID(i + "2-");

		Defender *def3 = new Defender(3, 0, 3, 8, 3, "talak31", game, false);
		def3->setID(i + "3-");

		Striker *str1 = new Striker(7, 2, 4, 9, 7, "pipen7", game);
		str1->setID(i + "4-");
		Striker *str2 = new Striker(8, 3, 5, 9, 8, "taen8", game);
		str2->setID(i + "5-");

		players.push_back(def1);
		players.push_back(def2);
		players.push_back(def3);
		players.push_back(str1);
		players.push_back(str2);
	}
}

int Team::chooseAction()
{
	int randomMove = 1 + rand() % 100;

	if (randomMove <= 35)
	{
		// move()
		return 1;
	}
	else if (randomMove <= 70)
	{
		// transfer()
		return 2;
	}
	else
	{
		// specialMove()
		return 3;
	}
}

bool Team::isInSameTeam(Player *p1, Player *p2)
{
	int counter = 0;

	if (p1 == NULL || p2 == NULL)
		return false;

	for (int i = 0; i < players.size(); i++)
	{
		if (players[i]->getName().compare(p1->getName()) == 0)
			counter++;
		if (players[i]->getName().compare(p2->getName()) == 0)
			counter++;
	}

	// We check if we found both 2 in the same group
	if (counter == 2)
		return true;
	else
		return false;
}

bool Team::isInRedTeam(Player *p)
{
	if (p == NULL)
		return false;
	for (int i = 0; i < players.size(); i++)
		if (players[i]->getName() == p->getName())
			return true;
	return false;
}

bool Team::isInBlueTeam(Player *p)
{
	if (p == NULL)
		return false;
	for (int i = 0; i < players.size(); i++)
		if (players[i]->getName() == p->getName())
			return true;
	return false;
}

Game *Team::getGameInstance()
{
	return game;
}

void Team::printTeamInfo()
{
	cout << getName() << "\t" << getPasses() << "\t" << getMistakes() << "\t"
		 << getGoals() << "\t" << getSteals() << endl;
}

vector<Player *> Team::getPlayersVector() { return players; }

void Team::setName(string name) { this->name = name; }
void Team::setPasses(int passes) { this->passes = passes; }
void Team::setMistakes(int mistakes) { this->mistakes = mistakes; }
void Team::setGoals(int goals) { this->goals = goals; }
void Team::setSteals(int steals) { this->steals = steals; }

string Team::getName() { return name; }
int Team::getPasses() { return passes; }
int Team::getMistakes() { return mistakes; }
int Team::getGoals() { return goals; }
int Team::getSteals() { return steals; }

void Team::readFile(string teamName)
{
	string line;
	std::ifstream myfile("file.txt");

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			// we read the line
			// store in Vector
			vector<string> v = split(line, ',');
			if (teamName.compare("red") == 0)
			{
				if (v.at(0).compare("red") == 0)
				{
					if (v.at(1).compare("defender") == 0)
					{
						bool b = (v.at(8).compare("false") == 0) ? false : true;
						Defender *def = new Defender(
							atoi(v.at(2).c_str()),
							atoi(v.at(3).c_str()),
							atoi(v.at(4).c_str()),
							atoi(v.at(5).c_str()),
							atoi(v.at(6).c_str()),
							v.at(7),
							game,
							b);
						def->setID("R" + v.at(4) + "-");
						players.push_back(def);
					}
					else if (v.at(1).compare("striker") == 0)
					{
						Striker *str = new Striker(
							atoi(v.at(2).c_str()),
							atoi(v.at(3).c_str()),
							atoi(v.at(4).c_str()),
							atoi(v.at(5).c_str()),
							atoi(v.at(6).c_str()),
							v.at(7),
							game);
						str->setID("R" + v.at(4) + "-");
						players.push_back(str);
					}
				}
			}
			else if (teamName.compare("blue") == 0)
			{
				if (v.at(0).compare("blue") == 0)
				{
					if (v.at(1).compare("defender") == 0)
					{
						bool b = (v.at(8).compare("false") == 0) ? false : true;
						Defender *def = new Defender(
							atoi(v.at(2).c_str()),
							atoi(v.at(3).c_str()),
							atoi(v.at(4).c_str()),
							atoi(v.at(5).c_str()),
							atoi(v.at(6).c_str()),
							v.at(7),
							game,
							b);
						def->setID("B" + v.at(4) + "-");
						players.push_back(def);
					}
					else if (v.at(1).compare("striker") == 0)
					{
						Striker *str = new Striker(
							atoi(v.at(2).c_str()),
							atoi(v.at(3).c_str()),
							atoi(v.at(4).c_str()),
							atoi(v.at(5).c_str()),
							atoi(v.at(6).c_str()),
							v.at(7),
							game);
						str->setID("B" + v.at(4) + "-");
						players.push_back(str);
					}
				}
			}
		}
		myfile.close();
	}
	else
		cout << "Unable to open file";
}

vector<string> Team::split(const string &s, char delim)
{
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

vector<string> &Team::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}