#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct Unit {
	int Owner; //for either player
	int id;
	int row;
	int col;
	int hp;
	int att;
	int def;
	int move;
	int cost;
	char img;
};

class Infantry : public Unit{
	public:
		Infantry(int owner, int id){
			this->Owner = owner;
			this->id = id;
			row = 0;
			col=0;
			hp= 10;
			att = 1;
			def = 1;
			move = 1;
			cost = 2;
			img = 'I';
		}
};

class Armor : public Unit {
	public:
		Armor (int owner, int id){
			this->Owner = owner;
			this->id = id;
			row = 0;
			col =0;
			hp = 15;
			att = 2;
			def = 2;
			move = 2;
			cost = 4;
			img = 'A';
		}
};

class Scout : public Unit {
	public:
		Scout(int owner, int id){
			this->Owner = owner;
			this->id = id;
			row = 0;
			col = 0;
			hp = 15;
			att = 1;
			def = 1;
			move = 3;
			cost = 2;
			img = 'S';
		}
};

class Artillery : public Unit{
	public:
		Artillery(int owner, int id){
			this->Owner = owner;
			this->id = id;
			row = 0;
			col = 0;
			hp = 5;
			att = 3;
			def = 1;
			move = 1;
			cost = 5;
			img = 'R';
		}
};

class Map {

	vector<vector<string>> grid;
	public:
		int rows;
		int cols;
		//R = Row; C = collumn
		Map(int R=15, int C=15){
			rows = R;
			cols = C;
			grid.resize(rows);
			for (int r = 0; r < rows; r++){
				grid[r].resize(cols);
				for( int c = 0; c<cols; c++){
					grid[r][c] = ".";
				}
			}
		}
		void display(){
			cout << "   ";
			for(int c=0; c<cols; c++){
				cout << c << " ";
				if(c<10){cout<<" ";}
			}
			cout<<"\n";

			for(int r= 0;r<rows;r++){
				cout<< r << " ";
				if( r<10){ cout<<" ";}
				for (int c=0; c<cols;c++){
					cout<<grid[r][c]<< "  ";}
				cout<<"\n";
			}
		}
		bool isInBounds(int r, int c){
			return (r>=0 && r<rows && c >=0 && c<cols);
		}
		bool isEmpty(int r, int c){
			if (!isInBounds(r,c))
				return false;
			return (grid[r][c] == ".");
		}
		bool place(int r, int c, string &tok){
			if(isEmpty(r,c)){
				grid[r][c] = tok;
				return true;
			}
			return false;
		}
		void clear(int r, int c) {
			if(isInBounds(r,c)){
				grid[r][c] = ".";
			}
		}

};
struct CountEntry{
	char img;
	int count;
};

class Player {
	public:
		int id;
		int money;
		vector<Unit*> units;
		int objRow;
		int objCol;
		vector<CountEntry> placedCount;

		Player(int PId = 1, int startMoney= 20){
			id = PId;
			money = startMoney;
			objRow = 0;
			objCol = 0;
		}

		int incCount(char img){
			for (int i = 0; i< (int)placedCount.size(); i++){
				if(placedCount[i].img == img){
					placedCount[i].count++;
					return placedCount[i].count;
				}
			}
			CountEntry e;
			e.img = img;
			e.count =1;
			placedCount.push_back(e);
			return 1;
		}

		Unit* findUnit(string &tok){
			for (int i = 0; i<(int)units.size();i++){
				Unit* u = units[i];
				string check = "";
				check+= u->img;
				check+= to_string(u->id);
				if(check ==tok){
					return u;
				}
			}
			return nullptr;
		}
		bool removeUnit(const string &tok, Map &map){
			for(int i= 0; i<(int)units.size();i++){
				Unit* u = units[i];
				string check = "";
				check+= u->img;
				check+= to_string(u->id);
				if(check ==tok){
					map.clear(u->row, u->col);
					money+=u->cost;
					delete u;
					units.erase(units.begin() +i);
					return true;
				}
			}
			return false;
		}
};


class Game{
	Map map;
	//m.display();
	Player P1;
	Player P2;
	int turn;


	void promptCoords(string &msg, int &r, int &c, int pid, bool half){
		while(true) {
			cout<<msg<<" row: "; cin>>r;
			cout<<"col: "; cin>>c;
			if(!map.isInBounds(r,c)){
				cout<<"Out of Bounds\n";
				continue;}
			if(half){
				if(pid == 1 && (r<0||r>6)){
					cout<<"Rows 0-6\n";
					continue;
				}
				if(pid == 2 && (r<8||r>14)){
					cout<<"Rows 8-14\n";
					continue;
				}
				break;
			}
		}
		void placeObjectives(){
			int r;
			int c;
			map.display();
			cout<<"P1 Place O1\n";
			promptCoord("Objective",r,c,1,true);
			p1.objRow = r;
			p1.objCol= c;
			map.place(r,c,"O1");

			map.display();
			cout<<"P2 place O2\n";
			promptCoord("Objective",r,c,2,true);
			p2.objRow = r;
			p2.objCol = c;
			map.place(r,c,"O2");
		}
		void purchasePhase(Player &P){
			char cmd;
			while(true){
				map.display();
				cout<<"P"<<P.id<<" $"<<P.money<<"b - buy r - remove e - end\n> ";
				cin>>cmd;
				if(cmd=='e') break;
				if(cmd=='b'){
					cout<<"0:Infantry, 1:Armor, 2:Scout, 3:Artillery\n";
					int idx;
					cin>>idx;
					Unit* u=nullptr;
					if(idx==0) u = new Infantry(P.id,P.incCount('I'));
					else if(idx ==1) u = new Armor(P.id,P.incCount('A'));
					else if(idx ==2) u = new Scout(P.id,P.incCount('S'));
					else if(idx ==3) u = new Artillery(P.id,P.incCount('R'));
					else{
						cout<<"Invalid Input\n";
						continue;}
					if(u->cost>P.money){
						cout<<"Not enough money\n";
						delete u;
						continue;}
					int rr,cc;
					promptCoord("Place",rr,cc,P.id,true);
					if(!map.isEmpty(rr,cc)){
						cout<<"Occupied\n";
						delete u;
						continue;
					}
					P.money-=u->cost;
				        u->row ==rr; 
					u->col = cc;
					P.units.push_back(u);
					string tok="";
					tok+=u->img;
					tok+=to_string(u->id);
					map.place(rr,cc,tok);
				}
				else if(cmd=='r'){
					cout<<"Remove token: ";
					string tok;
					cin>>tok;
					if(!P.removeUnit(tok,map)){
						cout<<"Fail\n";
					}
				}
				else cout<<"Unknown command\n";
			}
		}
	}
	public:
		Game()
			:map(15,15),p1(1,20),p2(2,20),turn(1)
		{}

		void play(){
			placeObjectives();
			purchasePhase(p1);
			purchasePhase(p2);
			while(true){
				map.display();
				Player *current;
				Player *other;
				if(turn ==1){
					current = &p1;
					other = &p2;
				} else{
					current = &p2;
					other = &p1;
				}

				cout << "Player " << current ->id<< "'s turn" << endl;
				int actions = 3;

				for (int i =0; i<actions; i++){
					cout<< "Choose action: b - buy or e - end turn" << endl;
					cout<< "> ";
					char action;
					cin >> action;
					if(action == 'e'){
						break;}
					else if(action =='b'){
						purchasePhase(*current);
					} else{
						cout<< "Invalid action" << endl;
						i--;
					}
				}

				for (Unit* u: current->units){
					if(u->row == other->objRow && u-> col == other->objCol){
						map.display();
						cout<<"Player " << current->id<< " captures objective and wins!" << endl;
						return;
					}
				}

				if (turn == 1){
					turn = 2;
				} else{
					turn = 1;
				}
			}
		}
};

int main(){
	Game game;
	game.play();
	return 0;
}

