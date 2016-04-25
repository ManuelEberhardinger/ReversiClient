#include "AI.h"
#include <climits>
#include <iostream>
#include <algorithm>
#include <cmath>

AI::AI(int myPlayer, int allPlayersSize)
{
	_myPlayer = myPlayer;
	_allPlayersSize = allPlayersSize;
	_playersWithoutMove = 0;
	_counter = 0;
	_numberOfMoves = 0;
	_lastAlpha = INT_MIN;
	_lastBeta = INT_MAX;
}

int AI::NextPlayer(int player, int playersSize)
{
	if (player < playersSize)
		player++;
	else
		player = 1;

	return player;
}

void AI::SetCounter()
{
	_counter = 0;
}

int AI::GetCounter() const
{
	return _counter;
}

bool SortForMin(const AIMove &lhs, const AIMove &rhs){
	return lhs.score < rhs.score;
}

bool SortForMax(const AIMove &lhs, const AIMove &rhs){
	return lhs.score > rhs.score;
}

int AI::paranoidAlgorithm(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves){
	_counter++;
	if (depth == 0){
		return map.evaluate(allPlayers[_myPlayer - 1]);
	}
	if (enoughTime == false)
		return 0;
	std::vector<AIMove> MoveList;
	map.getReducedMoveList(allPlayers[playerNr - 1], MoveList);
	//Wenn keine Züge mehr möglich
	if (MoveList.size() == 0){
		++noMoves;
		if (noMoves == _allPlayersSize)
			return map.CheckForWin(_myPlayer);
		return paranoidAlgorithm(map, allPlayers, NextPlayer(playerNr, allPlayers.size()), depth, orgDepth, bestMove, enoughTime, noMoves);
	}
	//Map UND Spieler zwischenspeichern, weil sie sich ändert in set Methode
	auto tempMap = map.GetField();
	GamePiece playerCopy = allPlayers[playerNr - 1];
	int bestScore, returnedScore;
	if (playerNr == _myPlayer)//MAX
		bestScore = INT_MIN; //Minus Unendlich
	else//MIN
		bestScore = INT_MAX; //Plus Unendlich
	for (auto move : MoveList){
		if (enoughTime == false)
			return 0;
		map.Set(move.x, move.y, move.bonus, allPlayers[playerNr - 1]); //Achtung hier ändern sich Spielerwerte und Map
		returnedScore = paranoidAlgorithm(map, allPlayers, NextPlayer(playerNr, allPlayers.size()), depth - 1, orgDepth, bestMove, enoughTime, 0);
		//Ausgangswerte in Spielfeld UND Spielerwerte werden wieder hergestellt
		map.SetField(tempMap);
		allPlayers[playerNr - 1] = playerCopy;
		//MAX
		if (playerNr == _myPlayer){
			if (bestScore < returnedScore)
			{
				bestScore = returnedScore;
				if (depth == orgDepth)
					bestMove = move;
			}
		}
		//MIN
		else{
			if (bestScore > returnedScore)
			{
				bestScore = returnedScore;
				if (depth == orgDepth)
					bestMove = move;
			}
		}
	}
	return bestScore;
}

int  AI::Max(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves, int alpha, int beta){
	_counter++;
	if (depth == 0){
		return map.evaluate(allPlayers[_myPlayer - 1], _numberOfMoves);
	}
	if (enoughTime == false)
		return 0;
	std::vector<AIMove> MoveList;
	map.getReducedMoveList(allPlayers[playerNr - 1], MoveList);
	_numberOfMoves = MoveList.size();
	if (MoveList.size() == 0){
		++noMoves;
		if (noMoves == _allPlayersSize)
			return map.CheckForWin(_myPlayer);
		int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
		if (nextPlayerNr == _myPlayer)
			return Max(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
		else
			return Min(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
	}
	//Map UND Spieler zwischenspeichern, weil sie sich ändert in set Methode
	auto tempMap = map.GetField();
	GamePiece playerCopy = allPlayers[playerNr - 1];
	int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
	int value = INT_MIN;
	for (const auto &move : MoveList){
		if (enoughTime == false)
			return 0;
		map.Set(move.x, move.y, move.bonus, allPlayers[playerNr - 1]);
		if (nextPlayerNr == _myPlayer)
			value = Max(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		else
			value = Min(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		map.SetField(tempMap);
		allPlayers[playerNr - 1] = playerCopy;
		//Hier wird geschnitten
		if (value >= beta){
			return value;
		}
		if (value > alpha){
			alpha = value;
			if (depth == orgDepth)
				bestMove = move;
		}
	}
	return alpha;
}

void AI::IterativeDepthAlgorithm(Map &map, std::vector<GamePiece> allPlayers, AIMove &bestMove, bool &enoughTime, long time)
{
	int depth = 1; //Anfangstiefe 1
	AIMove backupMove = map.GetFirstPossibleMove(allPlayers[_myPlayer - 1]);
	bestMove = backupMove;
	AIMove possibleMove;
	while (enoughTime)
	{
		MaxOrdered(map, allPlayers, _myPlayer, depth, depth, possibleMove, enoughTime);
		depth++;
		if (enoughTime)
			bestMove = possibleMove;
	}
	if (bestMove.x == backupMove.x && bestMove.y == backupMove.y && depth == 1)
		std::cout << "BackupMove benutzt" << std::endl;
	std::cout << "Tiefe: " << depth << std::endl;
}

void AI::AspirationWindowAlgorithm(Map &map, std::vector<GamePiece> allPlayers, AIMove &bestMove, bool &enoughTime, long time)
{
	int depth = 1; //Anfangstiefe 1
	//int timeForNextMove = 0;
	//Timer t;
	int alpha = INT_MIN, beta = INT_MAX;
	//t.start();
	AIMove possibleMove;
	AIMove backupMove = map.GetFirstPossibleMove(allPlayers[_myPlayer - 1]);
	bestMove = backupMove;
	while (enoughTime)
	{
		//if (map.GetEmptyFields() < depth && allPlayers[_myPlayer].getOverrideStonesNr() <= 0) //Das Tiefe nicht zu weit berechnet wird
		//	enoughTime = false;
		possibleMove.x = -1;
		MaxOrdered(map, allPlayers, _myPlayer, depth, depth, possibleMove, enoughTime, 0, alpha, beta);
		//Wenn Zug falsch wird Algorithmus nochmal mit INT_MIN und INT_MAX auf gleicher Tiefe aufgerufen
		//Wenn Zug richtig ist, wird tiefe erhöht, Aspiration Window vergrößert
		if (possibleMove.x == -1){
			alpha = INT_MIN;
			beta = INT_MAX;
			std::cout << "Aspiration Window zurueckgesetzt!" << std::endl;
			continue;
		}
		else{
			std::cout << "lastAlpha " << _lastAlpha << std::endl << "lastBeta " << _lastBeta << std::endl;
			alpha = _lastAlpha - 5000;
			beta = _lastBeta + 5000;
			std::cout << "Alpha " << alpha << std::endl << "Beta " << beta << std::endl;
			++depth;
			//timeForNextMove = std::pow(_numberOfMoves, 0.75 * depth) * 1; //Konstanten müssen noch gefunden werden
			//if (timeForNextMove > time - t.getTime() - 100) //100ms zur Sicherheit noch abgezogen
			//	enoughTime = false;
		}
		if (enoughTime)
			bestMove = possibleMove;
	}
	if (bestMove.x == backupMove.x && bestMove.y == backupMove.y && depth == 1)
		std::cout << "BackupMove benutzt" << std::endl;
	std::cout << "Tiefe: " << depth << std::endl;
}

int AI::Min(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves, int alpha, int beta){
	_counter++;
	if (depth == 0){
		return map.evaluate(allPlayers[_myPlayer - 1]);
	}
	if (enoughTime == false)
		return 0;
	std::vector<AIMove> MoveList;
	map.getReducedMoveList(allPlayers[playerNr - 1], MoveList);
	_numberOfMoves = MoveList.size();
	if (MoveList.size() == 0){
		++noMoves;
		if (noMoves == _allPlayersSize)
			return map.CheckForWin(_myPlayer);
		int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
		if (nextPlayerNr == _myPlayer)
			return Max(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
		else
			return Min(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
	}

	//Map UND Spieler zwischenspeichern, weil sie sich ändert in set Methode
	auto tempMap = map.GetField();
	GamePiece playerCopy = allPlayers[playerNr - 1];
	int value = INT_MAX;
	int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
	for (auto &move : MoveList){
		if (enoughTime == false)
			return 0;
		map.Set(move.x, move.y, move.bonus, allPlayers[playerNr - 1]);
		if (nextPlayerNr == _myPlayer)
			value = Max(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		else
			value = Min(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		map.SetField(tempMap);
		allPlayers[playerNr - 1] = playerCopy;
		//Hier wird geschnitten
		if (value <= alpha){
			return value;
		}
		//Hier ist das aktuelle Minimum
		if (beta > value)
			beta = value;
	}
	return beta;
}

void AI::SetAllPlayersSizeLower()
{
	_allPlayersSize--;
}

int AI::MaxOrdered(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves, int alpha, int beta){
	_counter++;
	if (depth == 0){
		return map.evaluate(allPlayers[_myPlayer - 1], _numberOfMoves);
	}
	if (enoughTime == false)
		return 0;
	std::vector<AIMove> MoveList;
    map.getReducedMoveList(allPlayers[playerNr - 1], MoveList);
	if (MoveList.size() == 0) {
		++noMoves;
		if (noMoves == _allPlayersSize)
			return map.CheckForWin(_myPlayer);
		int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
		if (nextPlayerNr == _myPlayer)
			return MaxOrdered(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
		else
			return MinOrdered(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
	}
	//Map UND Spieler zwischenspeichern, weil sie sich ändert in set Methode
	auto tempMap = map.GetField();
	GamePiece playerCopy = allPlayers[playerNr - 1];
	int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
	int value = INT_MIN;
	if (depth > 1 && depth < (orgDepth - 1)) {
		for (auto &move : MoveList){
			map.Set(move.x, move.y, move.bonus, allPlayers[playerNr - 1]);
			move.score = map.CountStones(_myPlayer);
			map.SetField(tempMap);
			allPlayers[playerNr - 1] = playerCopy;
		}
		std::sort(MoveList.begin(), MoveList.end(), SortForMax);
	}
	//Nur in Max Knoten wird numberOfMoves gesetzt
	_numberOfMoves = MoveList.size();
	for (const auto &move : MoveList){
		if (enoughTime == false)
			return 0;
		map.Set(move.x, move.y, move.bonus, allPlayers[playerNr - 1]);
		if (nextPlayerNr == _myPlayer)
			value = MaxOrdered(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		else
			value = MinOrdered(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		map.SetField(tempMap);
		allPlayers[playerNr - 1] = playerCopy;
		//Hier wird geschnitten
		if (value >= beta){
			return value;
		}
		if (value > alpha){
			alpha = value;
			_lastAlpha = alpha; //Für Aspiration Window
			if (depth == orgDepth)
				bestMove = move;
		}
	}
	return alpha;
}

int AI::MinOrdered(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves, int alpha, int beta){
	_counter++;
	if (depth == 0){
		return map.evaluate(allPlayers[_myPlayer - 1], _numberOfMoves);
	}
	if (enoughTime == false)
		return 0;
	std::vector<AIMove> MoveList;
    map.getReducedMoveList(allPlayers[playerNr - 1], MoveList);
	if (MoveList.size() == 0) {
		++noMoves;
		if (noMoves == _allPlayersSize)
			return map.CheckForWin(_myPlayer);
		int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
		if (nextPlayerNr == _myPlayer)
			return MaxOrdered(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
		else
			return MinOrdered(map, allPlayers, nextPlayerNr, depth, orgDepth, bestMove, enoughTime, noMoves, alpha, beta);
	}
	//Map UND Spieler zwischenspeichern, weil sie sich ändert in set Methode
	auto tempMap = map.GetField();
	GamePiece playerCopy = allPlayers[playerNr - 1];
	int value = INT_MAX;
	int nextPlayerNr = NextPlayer(playerNr, allPlayers.size());
	if (depth > 1 && depth < (orgDepth - 1)) {
		for (auto &move : MoveList){
			map.Set(move.x, move.y, move.bonus, allPlayers[playerNr - 1]);
			move.score = map.CountStones(_myPlayer);
			map.SetField(tempMap);
			allPlayers[playerNr - 1] = playerCopy;
		}
		std::sort(MoveList.begin(), MoveList.end(), SortForMin);
	}
	for (const auto &move : MoveList){
		if (enoughTime == false)
			return 0;
		map.Set(move.x, move.y, move.bonus, allPlayers[playerNr - 1]);
		if (nextPlayerNr == _myPlayer)
			value = MaxOrdered(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		else
			value = MinOrdered(map, allPlayers, nextPlayerNr, depth - 1, orgDepth, bestMove, enoughTime, 0, alpha, beta);
		map.SetField(tempMap);
		allPlayers[playerNr - 1] = playerCopy;
		//Hier wird geschnitten
		if (value <= alpha){
			return value;
		}
		//Hier ist das aktuelle Minimum
		if (beta > value){
			beta = value;
			_lastBeta = beta; //Für Aspiration Window
		}
	}
	return beta;
}
