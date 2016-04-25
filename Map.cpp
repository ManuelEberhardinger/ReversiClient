#include "Map.h"
#include <sstream>
#include <climits>
#include <iterator>

/*Map als 2D vector speichern*/
Map::Map(const std::string &map){ //< Map als 2D vector speichern
	std::stringstream stream(map.substr(5, map.size()));
	std::vector<std::vector<char>> tempField;
	std::vector<std::string> words;
	std::string word;
	while (stream >> word)
		words.push_back(word);
	players = stoi(words[0]);
	stones = stoi(words[1]);
	bombs = stoi(words[2]);
	strength = stoi(words[3]);
	height = stoi(words[4]);
	length = stoi(words[5]);
	int k = 0;
	for (int i = 0; i < height; i++){
		std::vector<char> temp;
		for (int j = 0; j < length; j++){
			temp.push_back(words[i + 6 + j + k][0]);
		}
		k += length - 1;
		tempField.push_back(temp);
	}
	//In Eindimensionales Array kopieren
	field = new char[height*length];
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; x++){
			field[y * length + x] = tempField[y][x];
		}
	}
	int numberOfTransitions = words.size() - (6 + height*length);
	for (int i = 0; i < numberOfTransitions; i++){
		std::vector<unsigned int> trans;
		for (int j = 0; j < 7; i++, j++){
			std::string temp = words[6 + height*length + i];
			if (temp == "<->")
				continue;
			trans.push_back(stoi(temp));
		}
		i--;
		transitions.push_back(trans);
	}
	//Liste füllen mit Wahrheitswerten füllen 
	std::vector< std::vector<bool> > isTransitionTemp;
	bool found = false;
	for (int y = 0; y < height; y++){
		std::vector<bool> temp;
		for (int x = 0; x < length; x++){
			for (auto transition : transitions){
				if ((transition[0] == x && transition[1] == y) || (transition[3] == x && transition[4] == y)){
					temp.push_back(true);
					found = true;
					break; // Da eine Transition an der Stelle befunden, nächstes Feld also überprüfen
				}
			}
			if (!found)
				temp.push_back(false);
			found = false;
		}
		isTransitionTemp.push_back(temp);
	}
	//Kopiere in Array
	isTransitionList = new bool[height*length];
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; x++){
			isTransitionList[y * length + x] = isTransitionTemp[y][x];
		}
	}
	found = false;
	for (int y = 0; y < height; y++){
		std::vector<std::vector<std::vector<int>>> partResult;
		for (int x = 0; x < length; x++){
			std::vector<std::vector<int>> fieldResult; // Vector für Transitionen der Felder
			std::vector<int> actualTrans; //Vector für Transitione selbst
			if (isTransitionList[y * length + x]){
				for (auto transition : transitions){
					if (transition[0] == x && transition[1] == y && transition[3] == x && transition[4] == y){
						actualTrans.push_back(transition[2]), actualTrans.push_back(transition[3]), actualTrans.push_back(transition[4]); //Transitionswerte speichern
						if (transition[5] >= 4)
							actualTrans.push_back(transition[5] - 4);
						else
							actualTrans.push_back(transition[5] + 4);

						fieldResult.push_back(actualTrans);  // Eine komplette Transition speichern
						actualTrans = {};
						actualTrans.push_back(transition[5]), actualTrans.push_back(transition[0]), actualTrans.push_back(transition[1]); //Transitionswerte speichern
						if (transition[2] >= 4)
							actualTrans.push_back(transition[2] - 4);
						else
							actualTrans.push_back(transition[2] + 4);

						fieldResult.push_back(actualTrans); //Eine komplette Transition speichern
					}
					else if (transition[0] == x && transition[1] == y){
						actualTrans.push_back(transition[2]), actualTrans.push_back(transition[3]), actualTrans.push_back(transition[4]); //Transitionswerte speichern
						if (transition[5] >= 4)
							actualTrans.push_back(transition[5] - 4);
						else
							actualTrans.push_back(transition[5] + 4);

						fieldResult.push_back(actualTrans);  // Eine komplette Transition speichern
						actualTrans = {};

					}
					else if (transition[3] == x && transition[4] == y){
						actualTrans.push_back(transition[5]), actualTrans.push_back(transition[0]), actualTrans.push_back(transition[1]); //Transitionswerte speichern
						if (transition[2] >= 4)
							actualTrans.push_back(transition[2] - 4);
						else
							actualTrans.push_back(transition[2] + 4);

						fieldResult.push_back(actualTrans); //Eine komplette Transition speichern
						actualTrans = {};
					}
				}	
			}
			partResult.push_back(fieldResult); // Transition der Zeilen wird gespeichert
		}
		TransitionDirections.push_back(partResult);
	}
	//Coordinaten der Ecksteine Speichern
	findFieldValue();

	//Anzahl leerer Felder
	_emptyFields = 0;
	for (int i = 0; i < height*length; i++)
		if (field[i] == '0' || field[i] == 'b' || field[i] == 'c' || field[i] == 'i')
			_emptyFields++;
}

//Destruktor
Map::~Map(){
	//Speicher für Arrays befreien
	delete[] field;
	delete[] fieldValues;
	delete[] isTransitionList;
}

/*Ausgabe Operator überschreiben*/
std::ostream&  operator<<(std::ostream& out, const Map &map){ //< Ausgabe Operator überschreiben
	out << map.players << std::endl;
	out << map.stones << std::endl;
	out << map.bombs << " " << map.strength << std::endl;
	out << map.height << " " << map.length << std::endl;
	for (int y = 0; y < map.height; ++y) {
		for (int x = 0; x < map.length; ++x)
			out << map.field[y * map.length + x] << " ";
		out << std::endl;
	}

	int transCount = 0;
	for (const auto &vec : map.transitions){
		for (const auto &i : vec){
			out << i << " ";
			transCount++;
			if (transCount == 3)
				out << "<-> ";	
		}
		transCount = 0;
		out << std::endl;
	}
	return out;
}

int Map::GetEmptyFields() const
{
	return _emptyFields;
}

void Map::SetEmptyFieldsLower(int x, int y)
{
	char value = field[y * length + x];
	if (value == '0' || value == 'c' || value == 'b' || value == 'i')
		_emptyFields--;
}

int Map::GetBombs() const
{
	return bombs;
}

int Map::GetStones() const
{
	return stones;
}

int Map::GetStrength() const
{
	return strength;
}

int Map::GetPlayers() const
{
	return players;
}

int Map::GetHeight() const
{
	return height;
}

int Map::GetLength() const
{
	return length;
}

//Könnte Speicherleck verursachen sollte aber ungefährlich sein, da Smartpointer mit richtigen Destruktor
std::shared_ptr<char>  Map::GetField() const {
	std::shared_ptr<char> result(new char[height*length], [](char *p) { delete[] p; });
	char *temp = result.get();
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x){
			*temp = field[y * length + x];
			++temp;
		}
	}
	return result;
}

void Map::SetField(const std::shared_ptr<char> other) {
	char* temp = other.get();
	char* fieldPtr = field;
	int size = length * height;
	for (int i = 0; i < size; ++i, ++temp, ++fieldPtr){
		*fieldPtr = *temp;
	}
}

int Map::evaluate(const GamePiece &player) {
	char playerNr = (char) player.getPlayerNr() + 48;
    int myStoneValue = 0, enemyStoneValue = 0;
	int size = length * height;
	//Temporäre Pointer
	double *fieldValuesPtr = fieldValues;
	char *fieldPtr = field;
	for (int i = 0; i < size; ++i, ++fieldValuesPtr, ++fieldPtr){
		char value = *fieldPtr;
		if (value > 48 && value < 58){
			if (value == playerNr){
				myStoneValue += *fieldValuesPtr;
			}
			else
				enemyStoneValue += *fieldValuesPtr;
		}
	}
	std::vector<AIMove> MoveList;
	getMoveList(player, MoveList);
	return MoveList.size() * 2048 + player.getOverrideStonesNr() * 512 + (myStoneValue - enemyStoneValue);
}


int Map::evaluate(const GamePiece &player, int moveCount) {
	char playerNr = (char)player.getPlayerNr() + 48;
    int myStoneValue = 0, enemyStoneValue = 0;
	int size = length * height;
	//Temporäre Pointer
	double *fieldValuesPtr = fieldValues;
	char *fieldPtr = field;
	for (int i = 0; i < size; ++i, ++fieldValuesPtr, ++fieldPtr){
		char value = *fieldPtr;
		if (value > 48 && value < 58){
			if (value == playerNr){
				myStoneValue += *fieldValuesPtr;
			}
			else
				enemyStoneValue += *fieldValuesPtr;
		}
	}
	return (moveCount << 11) + (player.getOverrideStonesNr() << 9) + (myStoneValue - enemyStoneValue); //Shifts sollten theoretisch schneller sein
}

void Map::InversionEvent(){
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x)
		{
			char &element = field[y * length + x];
			if (element > '0' && element <= players + 48)
			{
				int intElement = element - 48;
				element = ((intElement % players) + 1) + 48;

			}
		}
	}
}

void Map::findFieldValue(){
	std::vector<std::vector<double> > fieldValuesTemp;
	for (int y = 0; y < height; ++y){
		std::vector<double> temp;
		for (int x = 0; x < length; ++x){
			int orgX = x, orgY = y;
			double fieldValue = 0;
			char value = field[y * length + x];
			bool isAtBorder[8] = { false, false, false, false, false, false, false, false };
			if (!(value == '-')){
				for (int dir = 0; dir <= 7; ++dir, x = orgX, y = orgY){
					switch (dir){
					case 0:
						y--;
						break;
					case 1:
						x++;
						y--;
						break;
					case 2:
						x++;
						break;
					case 3:
						x++;
						y++;
						break;
					case 4:
						y++;
						break;
					case 5:
						x--;
						y++;
						break;
					case 6:
						x--;
						break;
					case 7:
						x--;
						y--;
						break;
					}
					/*Schräg Auserhalb des Feldes*/
					if (((x == -1) || (x == length)) || ((y == -1) || (y == height))){
						isAtBorder[dir] = true;
						continue;
					}
					//Wenn innerhalb des Feldes
					if ((field[y * length + x] == '-')){
						isAtBorder[dir] = true;
						continue;
					}
				}


				//Wenn Transition an der Stelle in Bestimmter Richtung dann ist an der Stelle doch eine Diagonale möglich
				if (isTransitionList[y * length + x]){
					auto TransitionsAtCoordinate = TransitionDirections[y][x];
					for (const auto &trans : TransitionsAtCoordinate){
						isAtBorder[trans[0]] = false;
					}
				}
				//Hier wird geschaut wieviel Diagonalen es An der Stelle (x/y) nicht existieren
				int diagonals = 0;
				for (int i = 0; i < 4; ++i){
					if (!(isAtBorder[i]) && !(isAtBorder[i + 4])) //Wenn an beiden Enden der Diagonalen freies Feld
						++diagonals;
				}
				//Hier werden die Werte vergeben
				switch (diagonals){
					//Das ist eine Ecke
				case 0:
					fieldValue = 5;
					break;
				case 1:
					fieldValue = 3;
					break;
				case 2:
					fieldValue = 1;
					break;
				case 3:
					fieldValue = 1;
					break;
					//Hier ist ein Steine in der Mitte des Feldes
				case 4:
					fieldValue = 1;
					break;
				}
			}
			//Wenn Koordinate auf x,y ein Loch ist
			else
				fieldValue = 0;
			//Sondersteine: Nicht berücksichtigt da Wert des Feldes sich verändern würde nach besetzen
			temp.push_back(fieldValue);
		}
		fieldValuesTemp.push_back(temp);
	}
	//Werte in ein Array abspeichern
	fieldValues = new double[height * length];
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x){
			fieldValues[y * length + x] = fieldValuesTemp[y][x];
		}
	}
}

//Achtung erniedrigt Überschreibsteine in player
void Map::getMoveList(const GamePiece &player, std::vector<AIMove> &moves) {
    int playerNr = player.getPlayerNr();
    int overrideStones = player.getOverrideStonesNr();
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x){
			char actual = field[y * length + x];
			_firstX = x, _firstY = y;
			AIMove temp;
			//Trivial wenn Loch
			if (actual == '-')
				continue;
			//Trivial wenn Expansionsstein und Überschreibstein
			else if (actual == 'x'){
                if (overrideStones >= 1){
					temp.x = x, temp.y = y, temp.bonus = 0;
					moves.push_back(temp);
				}
				continue;
			}
			//Wenn Spieler und Überschreibstein vorhanden
			else if (actual > 48 && actual < 58){
                if (overrideStones >= 1){
					for (int dir = 0; dir < 8; dir++){
                        if (moveDirectionsShort(x, y, playerNr, dir)){
							temp.x = x, temp.y = y, temp.bonus = 0;
							moves.push_back(temp);
							break;
						}
					}
				}
				continue;
			}
			//Wenn leer oder Inversion
			else if (actual == '0' || actual == 'i'){
				for (int dir = 0; dir < 8; dir++){
                    if (moveDirectionsShort(x, y, playerNr, dir)){
						temp.x = x, temp.y = y, temp.bonus = 0;
						moves.push_back(temp);
						break;
					}
				}
				continue;
			}
			//Bonus Stein gesetzt
			else if (actual == 'b'){
				for (int dir = 0; dir < 8; dir++){
                    if (moveDirectionsShort(x, y, playerNr, dir)){
						temp.x = x, temp.y = y, temp.bonus = 20;
						moves.push_back(temp);
						temp.bonus = 21;
						moves.push_back(temp);
						break;
					}
				}
				continue;
			}
			//Choice Stein gesetzt
			else if (actual == 'c'){
				for (int dir = 0; dir < 8; dir++){
                    if (moveDirectionsShort(x, y, playerNr, dir)){
						temp.x = x, temp.y = y, temp.bonus = 0;
						for (int i = 1; i <= players; i++){
								temp.bonus = i;
								moves.push_back(temp);
						}
						break;
					}
				}
			}
		}
	}
}

void Map::getReducedMoveList(const GamePiece &player, std::vector<AIMove> &moves) {
    int playerNr = player.getPlayerNr();
    int overrideStones = player.getOverrideStonesNr();
    std::vector<AIMove> overrideMoves;
    for (int y = 0; y < height; ++y){
        for (int x = 0; x < length; ++x){
            char actual = field[y * length + x];
            _firstX = x, _firstY = y;
            AIMove temp;
            //Trivial wenn Loch
            if (actual == '-')
                continue;
            //Trivial wenn Expansionsstein und Überschreibstein
            else if (actual == 'x'){
                if (overrideStones >= 1){
                    temp.x = x, temp.y = y, temp.bonus = 0;
                    overrideMoves.push_back(temp);
                }
                continue;
            }
            //Wenn Spieler und Überschreibstein vorhanden
            else if (actual > 48 && actual < 58){
                if (overrideStones >= 1){
                    for (int dir = 0; dir < 8; dir++){
                        if (moveDirectionsShort(x, y, playerNr, dir)){
                            temp.x = x, temp.y = y, temp.bonus = 0;
                            overrideMoves.push_back(temp);
                            break;
                        }
                    }
                }
                continue;
            }
            //Wenn leer oder Inversion
            else if (actual == '0' || actual == 'i'){
                for (int dir = 0; dir < 8; dir++){
                    if (moveDirectionsShort(x, y, playerNr, dir)){
                        temp.x = x, temp.y = y, temp.bonus = 0;
                        moves.push_back(temp);
                        break;
                    }
                }
                continue;
            }
            //Bonus Stein gesetzt
            else if (actual == 'b'){
                for (int dir = 0; dir < 8; dir++){
                    if (moveDirectionsShort(x, y, playerNr, dir)){
                        temp.x = x, temp.y = y, temp.bonus = 20;
                        moves.push_back(temp);
                        temp.bonus = 21;
                        moves.push_back(temp);
                        break;
                    }
                }
                continue;
            }
            //Choice Stein gesetzt
            else if (actual == 'c'){
                for (int dir = 0; dir < 8; dir++){
                    if (moveDirectionsShort(x, y, playerNr, dir)){
                        temp.x = x, temp.y = y, temp.bonus = 0;
                        for (int i = 1; i <= players; i++){
                            temp.bonus = i;
                            moves.push_back(temp);
                        }
                        break;
                    }
                }
            }
        }
    }
    if (moves.empty())
    {
        moves = std::move(overrideMoves);
    }
    /*if (moves.size() < 6)
    {
        moves.reserve(moves.size() + overrideMoves.size());
        std::move(std::begin(overrideMoves), std::end(overrideMoves), std::back_inserter(moves));
        overrideMoves.clear();
    }*/
}

AIMove Map::GetFirstPossibleMove(const GamePiece &player) {
	int playerNr = player.getPlayerNr();
	int overrideStones = player.getOverrideStonesNr();
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x){
			char actual = field[y * length + x];
			_firstX = x, _firstY = y;
			AIMove temp;
			//Trivial wenn Loch
			if (actual == '-')
				continue;
			//Trivial wenn Expansionsstein und Überschreibstein
			else if (actual == 'x'){
				if (overrideStones >= 1){
					temp.x = x, temp.y = y, temp.bonus = 0;
					return temp;
				}
				continue;
			}
			//Wenn Spieler und Überschreibstein vorhanden
			else if (actual > 48 && actual < 58){
				if (overrideStones >= 1){
					for (int dir = 0; dir < 8; dir++){
						if (moveDirectionsShort(x, y, playerNr, dir)){
							temp.x = x, temp.y = y, temp.bonus = 0;
							return temp;
							break;
						}
					}
				}
				continue;
			}
			//Wenn leer oder Inversion
			else if (actual == '0' || actual == 'i'){
				for (int dir = 0; dir < 8; dir++){
					if (moveDirectionsShort(x, y, playerNr, dir)){
						temp.x = x, temp.y = y, temp.bonus = 0;
						return temp;
						break;
					}
				}
				continue;
			}
			//Bonus Stein gesetzt
			else if (actual == 'b'){
				for (int dir = 0; dir < 8; dir++){
					if (moveDirectionsShort(x, y, playerNr, dir)){
						temp.x = x, temp.y = y, temp.bonus = 21;
						return temp;
						break;
					}
				}
				continue;
			}
			//Choice Stein gesetzt
			else if (actual == 'c'){
				for (int dir = 0; dir < 8; dir++){
					if (moveDirectionsShort(x, y, playerNr, dir)){
						temp.x = x, temp.y = y, temp.bonus = 0;
						for (int i = 1; i <= players; i++){
							temp.bonus = i;
							return temp;
						}
						break;
					}
				}
			}
		}
	}
}

bool Map::moveDirectionsShort(int x, int y, int playerNr, int dir, bool flippedStone, bool transitioned) const{
	char actual;
	int prevX, prevY;
		do{
			prevX = x, prevY = y;
			if (!transitioned){
				switch (dir){
				case 0:
					y--;
					break;
				case 1:
					y--, x++;
					break;
				case 2:
					x++;
					break;
				case 3:
					y++, x++;
					break;
				case 4:
					y++;
					break;
				case 5:
					y++, x--;
					break;
				case 6:
					x--;
					break;
				case 7:
					y--, x--;
					break;
				}
			}
			transitioned = false;
			//Wenn außerhalb überprüfen ob Transition
			if (y < 0 || x < 0 || y >= height || x >= length){
				if (isTransitionList[prevY * length + prevX] == true){
					auto allTransitions = TransitionDirections[prevY][prevX];
					for (auto transition : allTransitions){
						if (dir == transition[0]){
							return moveDirectionsShort(transition[1], transition[2], playerNr, transition[3], flippedStone, true);
						}
					}
				}
					return false;
			}
			//Zwischenspeichern, da innerhalb des Feldes
			actual = field[y * length + x];
			//Wenn Loch überprüfe ob Transition
			if(actual == '-'){
				if (isTransitionList[prevY * length + prevX] == true){
					auto allTransitions = TransitionDirections[prevY][prevX];
					for (auto transition : allTransitions){
						if (dir == transition[0]){
							return moveDirectionsShort(transition[1], transition[2], playerNr, transition[3], flippedStone, true);
						}
					}
				}
				return false;
			}
			//Sonderfeld außer Expansionsstein ODER leeres Feld
			if ((actual > 58 && actual != 'x') || (actual == '0'))
				return false;
			if (_firstX == x && _firstY == y)
				return false;
			//Wenn Gegenspieler oder Expansionsstein dann flippedStone = true
			if ((actual > 48 && actual < 58 && actual != (playerNr + 48)) || (actual == 'x'))
				flippedStone = true;
			//Wenn Steine umgedreht 
			if ((actual == playerNr + 48) && flippedStone)
				return true;
			if (actual == playerNr + 48)
				return false;

		} while (true);
		return false;
}

bool Map::CheckMove(int x, int y, int playerNr, int flippedStones[8]){
	_firstX = x, _firstY = y;
	bool moveAllowed = false;
	for (int i = 0; i < 8; i++) //flippedStone initialisieren
		flippedStones[i] = 0;
	for (int dir = 0; dir <= 7; ++dir){
		if (MoveDirection(x, y, playerNr, dir, flippedStones[dir], false))
			moveAllowed = true;
	}
	if (moveAllowed)
		return true;
	return false;
}

//Geht in die Richtung "dir" und Schreibt die Anzahl der Steine die in der Richtung überschrieben werden in flippedStones
bool Map::MoveDirection(int x, int y, int playerNr, int dir, int &flippedStones, bool transitioned){
	int c, k;
	//Konstanten, die zu x und y addiert werden, werden festgelegt
	switch (dir){
	case 0:
		c = -1, k = 0;
		break;
	case 1:
		c = -1, k = 1;
		break;
	case 2:
		c = 0, k = 1;
		break;
	case 3:
		c = 1, k = 1;
		break;
	case 4:
		c = 1, k = 0;
		break;
	case 5:
		c = 1, k = -1;
		break;
	case 6:
		c = 0, k = -1;
		break;
	case 7:
		c = -1, k = -1;
		break;
	}
	int prevX, prevY;
	do{
		prevX = x, prevY = y; // x und y speichern bevor man in eine Richtung geht
		if (!transitioned){
			y += c;
			x += k;
		}
		transitioned = false;
		//Wenn außerhalb überprüfen ob Transition
		if (y < 0 || x < 0 || y >= height || x >= length){
			if (isTransitionList[prevY * length + prevX] == true){
				auto allTransitions = TransitionDirections[prevY][prevX];
				for (auto transition : allTransitions){
					if (dir == transition[0]){
						return MoveDirection(transition[1], transition[2], playerNr, transition[3], flippedStones, true);
					}
				}
			}
			flippedStones = 0;
			return false;
		}
		char actual = field[y * length + x];
		if (actual == '-'){
			if (isTransitionList[prevY * length + prevX]){
				auto allTransitions = TransitionDirections[prevY][prevX];
				for (auto transition : allTransitions){
					if (dir == transition[0]){
						return MoveDirection(transition[1], transition[2], playerNr, transition[3], flippedStones, true);
					}
				}
			}
			flippedStones = 0;
			return false;
		}
		//Sonderfeld außer Expansionsstein oder leeres Feld
		if ((actual > 58 && actual != 'x') || actual == '0'){
			flippedStones = 0;
			return false;
		}

		//Wenn Steine umgedreht 
		if (_firstX == x && _firstY == y){
			flippedStones = 0;
			return false;
		}

		//Wenn Gegenspieler oder Expansionsstein dann flippedStone = true HIER WIRD ÜBERSCHRIEBEN
		if ((actual > 48 && actual < 58 && actual != playerNr + 48) || (actual == 'x')){
			flippedStones++;
			continue;
		}
		if ((actual == playerNr + 48) && flippedStones > 0){
			return true;
		}
		if (actual == playerNr + 48){
			flippedStones = 0;
			return false;
		}

	} while (true);
	return false;
}

void Map::FlipStones(int x, int y, int bonus, GamePiece &player, int flippedStones[8]){
	char actual = field[y * length + x];
	int playerNr = player.getPlayerNr();
	for (int dir = 0; dir < 8; dir++){
		MoveDirectionAndFlip(x, y, playerNr, dir, flippedStones[dir], false);
	}
	field[y * length + x] = playerNr + 48;
	if (actual == 'x' || (actual > 48 && actual < 58)){
		player.setOverrideStonesLower();
	}

	if (actual == 'i'){
		InversionEvent();
	}
	if (actual == 'b'){
		if (bonus == 20)
			player.setBombsNrHigher();
		else
			player.setOverrideStonesNrHigher();
	}
	if (actual == 'c'){
		for (int y = 0; y < height; ++y){
			for (int x = 0; x < length; ++x){
				char &element = field[y * length + x];
				if (element == bonus + 48)
					element = playerNr + 48;
				else if (element == playerNr + 48)
					element = bonus + 48;
			}
		}
	}
}

void Map::Set(int x, int y, int bonus, GamePiece &player){
	int flippedStones[8];
	CheckMove(x, y, player.getPlayerNr(), flippedStones);
	FlipStones(x, y, bonus, player, flippedStones);
}

//Überschreibt gemäß der Anzahl StonesToFlip und der Richtung
void Map::MoveDirectionAndFlip(int x, int y, int playerNr, int dir, int StonesToFlip, bool transitioned){
	int c, k;
	//Konstanten, die zu x und y addiert werden, werden festgelegt
	switch (dir){
	case 0:
		c = -1, k = 0;
		break;
	case 1:
		c = -1, k = 1;
		break;
	case 2:
		c = 0, k = 1;
		break;
	case 3:
		c = 1, k = 1;
		break;
	case 4:
		c = 1, k = 0;
		break;
	case 5:
		c = 1, k = -1;
		break;
	case 6:
		c = 0, k = -1;
		break;
	case 7:
		c = -1, k = -1;
		break;
	}
	int prevX, prevY;
	for (; StonesToFlip > 0; StonesToFlip--){
		prevX = x, prevY = y; // x und y speichern bevor man in eine Richtung geht
		if (!transitioned){
			y += c;
			x += k;
		}
		transitioned = false;
		//Wenn außerhalb überprüfen ob Transition
		if (y < 0 || x < 0 || y >= height || x >= length){
			if (isTransitionList[prevY * length + prevX] == true){
				auto allTransitions = TransitionDirections[prevY][prevX];
				for (auto transition : allTransitions){
					if (dir == transition[0]){
						return MoveDirectionAndFlip(transition[1], transition[2], playerNr, transition[3], StonesToFlip, true);
					}
				}
			}
			return;
		}
		char actual = field[y * length + x];
		if (actual == '-'){
			if (isTransitionList[prevY * length + prevX]){
				auto allTransitions = TransitionDirections[prevY][prevX];
				for (auto transition : allTransitions){
					if (dir == transition[0]){
						return MoveDirectionAndFlip(transition[1], transition[2], playerNr, transition[3], StonesToFlip, true);
					}
				}
			}
			return;
		}
		//Ich weis dass ich überschreiben darf solange ich innerhalb des Feldes bin
		field[y * length + x] = playerNr + 48;
	}
}

int Map::CheckForWin(int playerNr) const {
	int stones[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	char *fieldPtr = field;
	int size = length * height;
	char value;
	for (int i = 0; i < size; ++i, ++fieldPtr){
		value = *fieldPtr;
		if (value > 48 && value < 58){
			++stones[value - 49];
		}
	}
	for (int i = 0; i < 8; ++i){
		if (i + 1 != playerNr){
			if (stones[playerNr - 1] < stones[i])
				return INT_MIN + 1;
		}
	}
	return INT_MAX - 1;
}

int Map::CountStones(int playerNr) const {
	char *fieldPtr = field;
	int size = length * height;
	int myStones = 0, enemyStones = 0;
	char value;
	char playerChar = playerNr + 48;
	for (int i = 0; i < size; ++i, ++fieldPtr){
		value = *fieldPtr;
		if (value > 48 && value < 58){
			if (value == playerChar)
				++myStones;
			else
				++enemyStones;
		}
	}
	return myStones - enemyStones;
}

namespace std {
	template <> struct hash<std::pair<int, int>> {
		inline size_t operator()(const std::pair<int, int> &v) const {
			std::hash<int> int_hasher;
			return int_hasher(v.first) ^ int_hasher(v.second);
		}
	};
}

int Map::Bomb(int x, int y) {
	std::unordered_map<std::pair<int, int>, int> coordinates;
	coordinates.insert({{ x, y }, strength});
	Bombing(x, y, strength, coordinates);
	for (const auto &coordinate : coordinates) {
		field[coordinate.first.first + coordinate.first.second * length] = '-';
	}
	return 0;
}

//Schreibt in coordinates alle Felder die von der Bombe erwischt wurden
void Map::Bombing(int x, int y, int strengthLeft, std::unordered_map<std::pair<int, int>, int> &coordinates) {
	if (strengthLeft == 0)
		return;
	int prevX = x, prevY = y;
	for (int dir = 0; dir <= 7; ++dir, x = prevX, y = prevY) {
		int c, k;
		//Konstanten, die zu x und y addiert werden, werden festgelegt
		switch (dir){
		case 0:
			c = -1, k = 0;
			break;
		case 1:
			c = -1, k = 1;
			break;
		case 2:
			c = 0, k = 1;
			break;
		case 3:
			c = 1, k = 1;
			break;
		case 4:
			c = 1, k = 0;
			break;
		case 5:
			c = 1, k = -1;
			break;
		case 6:
			c = 0, k = -1;
			break;
		case 7:
			c = -1, k = -1;
			break;
		}
		y += c;
		x += k;

		if (y < 0 || x < 0 || y >= height || x >= length) {
			if (isTransitionList[prevY * length + prevX] == true){
				auto &allTransitions = TransitionDirections[prevY][prevX];
				for (auto transition : allTransitions){
					//Wir haben eine mögliche Transition gefunden in Richtung "dir"
					if (dir == transition[0]){
						if (field[transition[1] + transition[2] * length] != '-') {
						auto it = coordinates.find({transition[1], transition[2]});
						//Transition noch nicht besucht
						if (it == coordinates.end()){
							coordinates.insert({{transition[1], transition[2]}, strengthLeft});
							Bombing(transition[1], transition[2], strengthLeft - 1, coordinates);
						}
						//Transition besucht
						else{
							//Transition besucht aber mit geringerer Stärke
							if (coordinates[{transition[1], transition[2]}]  < strengthLeft){
								coordinates[{transition[1], transition[2]}] = strengthLeft;
								Bombing(transition[1], transition[2], strengthLeft - 1, coordinates);
							}
						}
					}
					}
				}
			}
			continue;
		}
		char actual = field[y *length + x];
		if(actual == '-') {
			if (isTransitionList[prevY * length + prevX] == true){
				auto &allTransitions = TransitionDirections[prevY][prevX];
				for (auto transition : allTransitions){
					if (dir == transition[0]){
						if (field[transition[1] + transition[2] * length] != '-') {
						auto it = coordinates.find({ transition[1], transition[2] });
						//Transition noch nicht besucht
						if (it == coordinates.end()){
							coordinates.insert({ { transition[1], transition[2] }, strengthLeft });
							Bombing(transition[1], transition[2], strengthLeft - 1, coordinates);
						}
						//Transition besucht
						else{
							//Transition besucht aber mit geringerer Stärke
							if (coordinates[{transition[1], transition[2]}] < strengthLeft){
								coordinates[{transition[1], transition[2]}] = strengthLeft;
								Bombing(transition[1], transition[2], strengthLeft - 1, coordinates);
							}
						}
					}
					}
				}
			}
			continue;
		}
		else {
			auto it = coordinates.find({ x, y });
			if (it == coordinates.end()){
				coordinates.insert({ { x, y }, strengthLeft });
				Bombing(x, y, strengthLeft - 1, coordinates);
			}
			else if (coordinates[{x, y}] < strengthLeft){
				coordinates[{x, y}] = strengthLeft;
				Bombing(x, y, strengthLeft - 1, coordinates);
			}
		}
	}
}

void Map::FindFirstBombMove(AIMove &move) {
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x){
			if (field[x + y * length] != '-'){
				move.x = x, move.y = y, move.bonus = 0;
				return;
			}
		}
	}
}

void Map::FindSimpleBombMove(AIMove &move, int myPlayerNr, bool &enoughTime) {
	move.bonus = 0;
	char playerChar = myPlayerNr + 48;
	int bestscore = INT_MIN, lastscore = 0;
	std::unordered_map<std::pair<int,int>, int> coordinates;
	for (int y = 0; y < height; ++y){
		for (int x = 0; x < length; ++x) {
			if (enoughTime == false)
				return;
			if (field[x + y * length] == '-')
				continue;
			else{
				coordinates.insert({ { x, y }, strength });
				Bombing(x, y, strength, coordinates);
				for (const auto &coordinate : coordinates) {
					if (field[x + y * length] == playerChar)
						--lastscore;
					else
						++lastscore;
				}
				if (lastscore > bestscore){
					bestscore = lastscore;
					move.x = x, move.y = y;
				}
			}
			//Zurücksetzen
			lastscore = 0;
			coordinates.clear();
		}
	}
	enoughTime = false;
}