#ifndef AI_H
#define AI_H
#include "Map.h"
#include "AIMove.h"
#include <vector>
#include "GamePiece.h"
#include "Timer.h"
#include <climits>

class AI
{
private:
	int _myPlayer;
	int _allPlayersSize, _playersWithoutMove, _counter, _numberOfMoves;
	int _lastAlpha, _lastBeta;
	Timer _tOneTurn;
	int NextPlayer(int player, int playersSize);
	int Min(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves, int alpha, int beta);
	int MinOrdered(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves, int alpha, int beta);
public:
	AI(int myPlayer, int allPlayersSize);
	int paranoidAlgorithm(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves = 0);
	void SetAllPlayersSizeLower();
	void SetCounter();
	int GetCounter() const;
	int Max(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves = 0, int alpha = INT_MIN, int beta = INT_MAX);
	void IterativeDepthAlgorithm(Map &map, std::vector<GamePiece> allPlayers, AIMove &bestMove, bool &enoughTime, long time);
	void AspirationWindowAlgorithm(Map &map, std::vector<GamePiece> allPlayers, AIMove &bestMove, bool &enoughTime, long time);
	int  MaxOrdered(Map &map, std::vector<GamePiece> allPlayers, int playerNr, int depth, int orgDepth, AIMove &bestMove, bool &enoughTime, int noMoves = 0, int alpha = INT_MIN, int beta = INT_MAX);
};

#endif