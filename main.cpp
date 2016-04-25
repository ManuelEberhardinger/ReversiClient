#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include "Map.h"
#include "Socket.h"
#include "AI.h"
#include "AIMove.h"
#include "Timer.h"

using namespace std;

string GetStringForSend(const u_short x, const u_short y, const char bonus);
void FromShort(short number, unsigned char &byte1, unsigned char &byte2);
short ToShort(short byte1, short byte2);
long ToLong(unsigned char bytes[]);
void debugShowMoves(const Map &map, const vector<AIMove> &moves);
AIMove getMove(const string &message);
void elapsedTime(bool &enoughTime, long time);

int main(int argc, char** argv){
	std::string ip = "127.0.0.1", port = "7777";
	if (argc > 1){
			for (int i = 1; i < argc; ++i) {
				if (std::string(argv[i]) == "-p") {
					if (i + 1 < argc) {
						port = argv[++i];
					}
					else
						std::cout << "Fehler kein Port angegeben" << std::endl;
				}
				if (std::string(argv[i]) == "-i") {
					if (i + 1 < argc) {
						ip = argv[++i];
					}
					else
						std::cout << "Fehler kein Ip angegeben" << std::endl;
				}
			}
	}
	Socket socket(ip, port);
	std::string group = { 0x01, 0x00, 0x00, 0x00, 0x01, 0x0B };
	socket.sendT(group);
	std::cout << "Waiting for other players..." << std::endl;
	std::string command = socket.receiveT();

	Map map(command);
	//cout << "Game started" << endl <<  map << endl;
	command = socket.receiveT();
	int playerNr = (int) command[5];
	cout << "Group ID: " << playerNr << endl;

	vector<GamePiece> allPlayers;
	for (int i = 0; i < map.GetPlayers(); i++)
	{
		GamePiece player(i+1, map.GetBombs(), map.GetStrength(), map.GetStones());
		allPlayers.push_back(player);
	}
	AI ai(playerNr, map.GetPlayers());
	
	bool endPhase1 = false, endPhase2 = false;
	short nr;
	short depthSearch;
	unsigned char time[4];
	long timeForMove;
	int player, disqualifiedPlayer;
	AIMove move;
	bool enoughTime = true;
	thread timeThread;

	while (!endPhase1 || !endPhase2)
	{
		command = socket.receiveT();
		nr = (short)command[0];
		enoughTime = true;

		switch (nr)
		{
		case 4:
			depthSearch = (short)command[9];
			for (int i = 0; i < 4; i++)
				time[i] = command[5 + i];
			timeForMove = ToLong(time);
			ai.SetCounter();
			if (!endPhase1){
				if (timeForMove > 0)
				{
					timeThread = thread(elapsedTime, ref(enoughTime), timeForMove);
					ai.IterativeDepthAlgorithm(map, allPlayers, move, enoughTime, timeForMove);
					timeThread.join();
				}
				else
					ai.MaxOrdered(map, allPlayers, playerNr, depthSearch, depthSearch, move, enoughTime);
			}
			else{
				if (timeForMove > 0){
					timeThread = thread(elapsedTime, ref(enoughTime), timeForMove);
					map.FindSimpleBombMove(move, playerNr, enoughTime);
					timeThread.join();
				}
				else
					map.FindSimpleBombMove(move, playerNr, enoughTime);
			}
			socket.sendT(GetStringForSend(move.x, move.y, move.bonus));
			break;
		case 6:
			player = command[10];
			move = getMove(command);
			if (!endPhase1){
				map.Set(move.x, move.y, move.bonus, allPlayers[player - 1]);
				map.SetEmptyFieldsLower(move.x, move.y);
			}
			else {
				map.Bomb(move.x, move.y);
				cout << map << endl;
			}
			break;
		case 7:
			disqualifiedPlayer = (int) command[5];
			cout << "Disqualifiziert Spieler: " << disqualifiedPlayer << endl;
			allPlayers[disqualifiedPlayer-1].setIsDisqualified(true);
			std::cout << map << std::endl;
			if (disqualifiedPlayer == playerNr)
			{
				cout << "Spielende" << endl;
				endPhase1 = true;
			}
			ai.SetAllPlayersSizeLower();
			break;
		case 8:
			cout << "Erste Phase Zuende" << endl;
			endPhase1 = true;
			break;
		case 9:
			cout << "Spielende" << endl; //Nötig falls alle Spieler disqualifiziert werden, damit schleife abbricht
			endPhase2 = true;
			endPhase1 = true;
			break;
		}
	}
	cout << map << endl;
	system("pause");
	return 0;
}

void elapsedTime(bool &enoughTime, long time)
{
	Timer t;
	t.start();
	while (enoughTime)
	{
		if (t.getTime() + 200 > time) //200ms zur Sicherheit dazu gerechnet
		{
			enoughTime = false;
			cout << "Zeit aus" << endl;
		}
	}
	t.stop();
	cout << "Zeit: " << t.getTime() << endl;
}

string GetStringForSend(const u_short x, const u_short y, const char bonus){
	std::string base = { 0x05, 0x00, 0x00, 0x00, 0x05 };
	unsigned char byteX1, byteX2;
	FromShort(x, byteX1, byteX2);
	unsigned char byteY1, byteY2;
	FromShort(y, byteY1, byteY2);
	std::string temp = { (char)byteX2, (char)byteX1, (char)byteY2, (char)byteY1, bonus };
	return base.append(temp);
}

void FromShort(short number, unsigned char &byte1, unsigned char &byte2)
{
	byte2 = (unsigned char)(number >> 8);
	byte1 = (unsigned char)(number & 255);
}

short ToShort(short byte1, short byte2)
{
	return (byte2 << 8) + byte1;
}

void debugShowMoves(const Map &map, const vector<AIMove> &moves){
	auto f = map.GetField();
	char *field = f.get();
	bool found = false;
	int length = map.GetLength();
	int height = map.GetHeight();
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x){
			cout << field[y * length + x];
			for (auto move : moves){
				if (move.x == x && move.y == y){
					cout << "'";
					found = true;
					break;
				}
			}
			if (!found){
				cout << " ";
			}
			found = false;
		}
		cout << endl;
	}
	cout << "Anzahl möglicher Positionen: " <<  moves.size() << endl;
}

AIMove getMove(const string &message){
	AIMove result;
	unsigned char first, second;
	first = message[5], second = message[6];
	u_short x = ToShort(second, first);
	first = message[7], second = message[8];
	u_short y = ToShort(second, first);
	result.x = x, result.y = y;
	result.bonus = message[9];
	return result;
}

long ToLong(unsigned char bytes[])
{
	return (long)bytes[3] | ((long)bytes[2] << 8) | ((long)bytes[1] << 16) | ((long)bytes[0] << 24);
}
