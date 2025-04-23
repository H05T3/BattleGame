#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ---------------------
// 1) Unit Module
// ---------------------
struct Unit {
    int Owner;    // which player
    int id;       // per‐type counter
    int row, col; // map coords
    int hp;       // hit points
    int att;      // attack
    int def;      // defense
    int mv;       // movement
    int cost;     // purchase cost
    char img;     // display char
};

class Infantry : public Unit {
public:
    Infantry(int owner_, int id_) {
        Owner = owner_;
        id    = id_;
        row = col = 0;
        hp   = 10; att = 1; def = 1; mv = 1; cost = 1; img = 'I';
    }
};

class Armor : public Unit {
public:
    Armor(int owner_, int id_) {
        Owner = owner_;
        id    = id_;
        row = col = 0;
        hp   = 15; att = 2; def = 2; mv = 2; cost = 4; img = 'A';
    }
};

class Scout : public Unit {
public:
    Scout(int owner_, int id_) {
        Owner = owner_;
        id    = id_;
        row = col = 0;
        hp   = 10; att = 1; def = 1; mv = 3; cost = 2; img = 'S';
    }
};

class Artillery : public Unit {
public:
    Artillery(int owner_, int id_) {
        Owner = owner_;
        id    = id_;
        row = col = 0;
        hp   =  5; att = 3; def = 1; mv = 1; cost = 5; img = 'R';
    }
};

// ---------------------
// 2) Map Module
// ---------------------
class Map {
public:
    int rows, cols;
    vector<vector<string>> grid;

    Map(int R = 15, int C = 15) {
        rows = R; cols = C;
        grid.assign(rows, vector<string>(cols, "."));
    }

    void display() {
        cout << "   ";
        for (int c = 0; c < cols; c++) {
            cout << c << (c<10? "  ":" ");
        }
        cout << "\n";
        for (int r = 0; r < rows; r++) {
            cout << (r<10? " ":"") << r << " ";
            for (int c = 0; c < cols; c++) {
                cout << grid[r][c] << "  ";
            }
            cout << "\n";
        }
    }

    bool inBounds(int r,int c) {
        return r>=0 && r<rows && c>=0 && c<cols;
    }

    bool isEmpty(int r,int c) {
        return inBounds(r,c) && grid[r][c]==".";
    }

    bool place(int r,int c,const string &tok) {
        if (isEmpty(r,c)) {
            grid[r][c] = tok;
            return true;
        }
        return false;
    }

    void clear(int r,int c) {
        if (inBounds(r,c)) grid[r][c] = ".";
    }
};

// ---------------------
// 3) Player Module
// ---------------------
struct CountEntry {
    char img;
    int count;
};

class Player {
public:
    int id;
    int money;
    vector<Unit*> units;
    int objRow, objCol;
    vector<CountEntry> placedCount;

    Player(int pid = 1, int startMoney = 20) {
        id     = pid;
        money  = startMoney;
        objRow = objCol = 0;
    }

    int incCount(char img) {
        for (int i = 0; i < (int)placedCount.size(); i++) {
            if (placedCount[i].img == img) {
                placedCount[i].count++;
                return placedCount[i].count;
            }
        }
        // first one of this type
        placedCount.push_back({img,1});
        return 1;
    }

    Unit* findUnit(const string &tok) {
        for (int i = 0; i < (int)units.size(); i++) {
            Unit* u = units[i];
            string check = "";
            check += u->img;
            check += to_string(u->id);
            if (check == tok) {
                return u;
            }
        }
        return nullptr;
    }

    bool removeUnit(const string &tok, Map &map) {
        for (int i = 0; i < (int)units.size(); i++) {
            Unit* u = units[i];
            string check = "";
            check += u->img;
            check += to_string(u->id);
            if (check == tok) {
                map.clear(u->row, u->col);
                money += u->cost;
                delete u;
                units.erase(units.begin() + i);
                return true;
            }
        }
        return false;
    }
};

// ---------------------
// 4) Game Module
// ---------------------
class Game {
    Map    map;
    Player p1, p2;
    int    turn;

    // helper to prompt valid coords
    void promptCoord(const string &msg,int &r,int &c,int pid,bool half) {
        while (true) {
            cout << msg << " row: "; cin >> r;
            cout << "col: ";      cin >> c;
            if (!map.inBounds(r,c)) {
                cout<<"→ out of bounds\n";
                continue;
            }
            if (half) {
                if (pid==1 && r>6) { cout<<"→ rows 0–6 only\n"; continue; }
                if (pid==2 && r<8) { cout<<"→ rows 8–14 only\n"; continue; }
            }
            break;
        }
    }

    void placeObjectives() {
        int r,c;
        // Player 1
        map.display();
        cout<<"Player 1: place objective (O1)\n";
        promptCoord("Objective",r,c,1,true);
        p1.objRow = r; p1.objCol = c;
        map.place(r,c,"O1");
        // Player 2
        map.display();
        cout<<"Player 2: place objective (O2)\n";
        promptCoord("Objective",r,c,2,true);
        p2.objRow = r; p2.objCol = c;
        map.place(r,c,"O2");
    }

    void purchasePhase(Player &P) {
        while (true) {
            map.display();
            cout<<"Player "<<P.id<<" Money: "<<P.money<<"\n";
            cout<<"(b)uy  (r)emove  (e)nd\n> ";
            char cmd; cin>>cmd;
            if (cmd=='e') break;

            if (cmd=='b') {
                cout<<"0=Infantry(1$) 1=Armor(4$) 2=Scout(2$) 3=Artillery(5$)\n> ";
                int choice; cin>>choice;
                Unit* u = nullptr;
                if      (choice==0) u = new Infantry( P.id, P.incCount('I') );
                else if (choice==1) u = new Armor(    P.id, P.incCount('A') );
                else if (choice==2) u = new Scout(    P.id, P.incCount('S') );
                else if (choice==3) u = new Artillery(P.id, P.incCount('R') );
                else {
                    cout<<"Invalid choice\n";
                    continue;
                }
                if (u->cost > P.money) {
                    cout<<"Not enough money\n";
                    delete u;
                    continue;
                }
                int rr,cc;
                promptCoord("Place unit",rr,cc,P.id,true);
                if (!map.isEmpty(rr,cc)) {
                    cout<<"Occupied\n";
                    delete u;
                    continue;
                }
                P.money -= u->cost;
                u->row = rr; u->col = cc;
                P.units.push_back(u);
                string tok = ""; tok += u->img; tok += to_string(u->id);
                map.place(rr,cc,tok);
            }
            else if (cmd=='r') {
                cout<<"Token to remove: ";
                string tok; cin>>tok;
                if (!P.removeUnit(tok,map))
                    cout<<"Removal failed\n";
            }
            else {
                cout<<"Unknown command\n";
            }
        }
    }

public:
    Game() : map(15,15), p1(1,20), p2(2,20), turn(1) {}

    void play() {
        placeObjectives();
        purchasePhase(p1);
        purchasePhase(p2);

        while (true) {
            map.display();
            Player *cur = (turn==1 ? &p1 : &p2);
            Player *opp = (turn==1 ? &p2 : &p1);
            cout<<"Player "<<cur->id<<"'s turn (3 actions)\n";

            for (int a=0; a<3; a++) {
                cout<<"(b)uy or (e)nd turn\n> ";
                char cmd; cin>>cmd;
                if (cmd=='e') break;
                if (cmd=='b') purchasePhase(*cur);
                else { cout<<"Invalid action\n"; a--; }
            }

            // check objective capture
            for (int i=0; i<(int)cur->units.size(); i++) {
                Unit* u = cur->units[i];
                if (u->row==opp->objRow && u->col==opp->objCol) {
                    map.display();
                    cout<<"Player "<<cur->id<<" captures objective and wins!\n";
                    return;
                }
            }
            turn = (turn==1?2:1);
        }
    }
};

int main(){
    Game game;
    game.play();
    return 0;
}
