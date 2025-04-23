#include <iostream>
#include <vector>
#include <string>
using namespace std;


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

int main(){
	Map m;
	//m.display();
	//Player P1;
	//Player P2;
	int playerTurn = 1;

	return 0;

}
