#include "bnb_openMP.cpp"

class Knapsack_Problem;

class Knapsack_Solution: public Solution{
	public:
		int weight,cost;
		vector<int> took; // object nos. chosen
		int pos; // number of objects taken into consideration
		int get_cost();
		bool is_feasible();
		void print_sol();
		int get_bound();
		int to_str(char *buffer); // MPI message is passed as char stream
};

class Knapsack_Problem: public Problem<Knapsack_Solution>{
	public:
		vector<int> costs;
		vector<int> weights;
		int n; // num of objects
		int W; // upper limit on total weight of objects selected
		bool get_goal();
		Knapsack_Solution empty_sol();
		Knapsack_Solution worst_sol();
		vector<Knapsack_Solution> expand(Knapsack_Solution s);
		int to_str(char *buffer);
		static Knapsack_Solution decode_sol(char *buffer, int &pos);
		static Knapsack_Problem decode_prob(char *buffer, int &pos);
};

/* ----------------- method defs: ------------------------------ */

int Knapsack_Solution::get_cost(){
	return cost;
}

Knapsack_Solution Knapsack_Problem::decode_sol(char *buffer, int &pos){
	int b, ntook;
	Knapsack_Solution ks;
	sscanf(buffer+pos,"%d%d%d%d%n",&(ks.weight),&(ks.cost),&(ks.pos),&ntook,&b);
	pos += b;
	for(int i=0;i<ntook;i++){
		int temp;
		sscanf(buffer+pos,"%d%n",&temp,&b);
		pos += b;
		ks.took.push_back(temp);
	}
	return ks;
}

int Knapsack_Solution::to_str(char *buffer){
	int p = 0;
	p += sprintf(buffer+p,"%d %d %d %d ",weight,cost,pos,(int)took.size());
	for(int i=0;i<took.size();i++)
		p += sprintf(buffer+p, "%d ",took[i]);
	return p;
}

struct item{
	int weight, cost;
	double ratio;
};

bool comp(item a, item b){
	return a.ratio > b.ratio;
}

int Knapsack_Solution::get_bound(){
	int bound = cost;
	vector<item> rem; // remaining items
	for(int i=pos;i< ((Knapsack_Problem *)prob_ptr) ->n;i++){
		item it;
		it.weight = ((Knapsack_Problem *)prob_ptr) -> weights[i];
		it.cost = ((Knapsack_Problem *)prob_ptr) -> costs[i];
		it.ratio = ( (double) it.cost) / ( (double) it.weight);
		rem.push_back(it);
	}
	int w = weight;
	sort(rem.begin(),rem.end(),comp); // decreasing order of ratio of cost to weight;
	for(int j=0;j<rem.size() && w <= ((Knapsack_Problem *)prob_ptr)->W;j++){
		//taking items till weight is exceeded
		bound += rem[j].cost;
		w += rem[j].weight;
	}
	return bound;
}

bool Knapsack_Solution::is_feasible(){
	return (pos == (((Knapsack_Problem *)prob_ptr)->n) );
}




int Knapsack_Problem::to_str(char* buffer){
    int p = 0;
    p += sprintf(buffer+p,"%d %d ",n,W);
    for(int i=0;i<n;i++)
        p += sprintf(buffer+p,"%d ",weights[i]);
    for(int i=0;i<n;i++)
        p += sprintf(buffer+p,"%d ",costs[i]);
    return p;
}

void Knapsack_Solution::print_sol(){
	
    
    cout<<"Max val of "<<cost<<" achieved by taking objects ";
	
    for(int i=0;i<took.size();i++)
		cout<<took[i]<<", ";
	cout<<endl;
}

bool Knapsack_Problem::get_goal(){
	return true; // it is a maximization problem
}

Knapsack_Problem Knapsack_Problem::decode_prob(char *buffer, int &pos){
    int b;
    Knapsack_Problem kp;
    sscanf(buffer+pos,"%d%d%n",&(kp.n),&(kp.W),&b);
    pos += b;
    for(int i=0;i<kp.n;i++){
        int temp;
        sscanf(buffer+pos,"%d%n",&temp,&b);
        pos += b;
        kp.weights.push_back(temp);
    }
    for(int i=0;i<kp.n;i++){
        int temp;
        sscanf(buffer+pos,"%d%n",&temp,&b);
        pos += b;
        kp.costs.push_back(temp);
    }
    return kp;
}



Knapsack_Solution Knapsack_Problem::worst_sol(){
	Knapsack_Solution s1;
	s1.prob_ptr = this;
	s1.cost = -1000000;
	s1.weight = 0;
	return s1;
}

Knapsack_Solution Knapsack_Problem::empty_sol(){
    Knapsack_Solution s1;
    s1.pos = 0;
    s1.prob_ptr = this;
    s1.weight = s1.cost = 0;
    return s1;
}





int main(){
	BnB_SOLVER<Knapsack_Problem, Knapsack_Solution> bnbs;
	Knapsack_Problem p;
	cin>>p.n;
	cin>>p.W;
	for(int i=0;i<p.n;i++){
		int temp;
		cin>>temp;
		p.weights.push_back(temp);
	}
	for(int i=0;i<p.n;i++){
		int temp;
		cin>>temp;
		p.costs.push_back(temp);
	}
	bnbs.solve(p);
	return 0;
}
