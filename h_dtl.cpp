#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

const double LOG_2 = log10(2);
int al = 10;
double sl = 0.5;

enum Type{
	T_ROOT = 0,
	T_AGE,
	T_PRIOR,
	T_ALLE,
	T_ANTI,
	T_SCORE,
	T_LEAF
};


//Calculate entropy for any pos and neg becasreful about log 0 will cause nan
double calc_entropy(double pos,double neg){
   double p = (double)pos/(double)(pos+neg);
   double n = (double)neg/(double)(pos+neg);
   if (p == 0) {
       p = 1;
   }
   if (n == 0) {
       n = 1;
   }
   
   return -(p*log10(p)/LOG_2+n*log10(n)/LOG_2);
}

//divide the age by agelevel of 10  //ie. 0-10, 10-20 and etc
//divide the score by scoreLevel of 0.5

class Heart{
	private:
		int m_ageLevel;
		int m_prior;
		int m_dead;
		int m_alle;
		int m_anti;
		int m_scoreLevel;
	public:
		Heart( int age = 10, int prior = 0,int alle = 0, int anti = 0, double score = 0.5, int dead = 0){
			m_ageLevel = age/al;
			m_prior = prior;
			m_alle = alle;
			m_anti = anti;
			m_scoreLevel = (score)/sl;
			m_dead = dead;
			//cout<<m_scoreLevel<<endl;
		}

		Heart(const Heart &p ){
			m_ageLevel = p.getAgeLevel();
			m_prior = p.getPrior();
			m_alle = p.getAlle();
			m_anti = p.getAnti();
			m_scoreLevel = p.getScoreLevel();
			m_dead = p.getDead();
			
		}	
		
		int getAgeLevel() const{
		   return m_ageLevel;
	   	}
		
		int getDead() const{
		   return m_dead;
	   	}
		
		int getPrior() const{
		   return m_prior;
	   	}

		int getAlle() const{
			return m_alle;
		}

		int getAnti() const{
			return m_anti;
		}
		
		int getScoreLevel() const{
			return m_scoreLevel;
		}

		int getType(Type t){
			if(t == T_AGE){
				return m_ageLevel;
			}else if(t == T_PRIOR){
				return m_prior;
			}else if(t == T_ALLE){
				return m_alle;
			}else if(t == T_ANTI){
				return m_anti;
			}else if(t == T_SCORE){
				return m_scoreLevel;
			}
		}
};



class HTree{
	private:
		int m_dead, m_pos, m_neg,m_id;
		vector<Heart> m_list;
		vector<HTree*> m_leaf;
		vector<Type> m_attr;
		Type m_type;

	public:
		HTree(){
			m_dead = 0;
			m_pos = 0;
			m_neg = 0;
			m_type = T_ROOT;
			m_attr.push_back(T_AGE);
			m_attr.push_back(T_PRIOR);
			m_attr.push_back(T_ALLE);
			m_attr.push_back(T_ANTI);
			m_attr.push_back(T_SCORE);
		}

		HTree(vector<Type> & attr){
			m_dead = 0;
			m_pos = 0;
			m_neg = 0;
			m_attr = attr;
		}
		
		int getDead(){
			return m_dead;
		}

		int getId(){
			return m_id;
		}
		
		Type getType(){
			return m_type;
		}

		void add_h(Heart &p){
			m_list.push_back(p);
			if(p.getDead()== 1){
				m_pos += 1;
			}else{
				m_neg += 1;
			}
		}
	
		void build(){
			//set dead
			if(m_pos > m_neg){
				m_dead = 1;
			}else{
				m_dead = 0;
			}
			cout<<m_pos<<" "<<m_neg<<" "<<m_dead<<endl;
			
			double max_ig = -1.0;
			vector<Type> :: iterator it ;
			for(it = m_attr.begin();it!= m_attr.end() ; it++){
				double ig = calc_ig(*it);
				if(ig > max_ig){
					m_type = (*it);
				}
			}
			
			//If condition for only one attribute left
			if(m_attr.size()<=1){
				m_type = T_LEAF;
				return ;
			}else if(m_pos == 0 || m_neg == 0){
				m_type = T_LEAF;
				return ;
			}

			//Get the remaining attribute
			vector<Type> c_attr;
			for(it = m_attr.begin();it!= m_attr.end() ; it++){
				if((*it) != m_type){
					c_attr.push_back(*it);
				}
			}
			
			//Find the id of children list
			vector<Heart> :: iterator n_it;
			for (n_it = m_list.begin();n_it!= m_list.end() ; n_it++){
				vector<HTree *> :: iterator l_it;
				bool is_find = false;
				for(l_it =m_leaf.begin();l_it != m_leaf.end();l_it++){
					if((*l_it)->getId() == (*n_it).getType(m_type)){
						(*l_it)->add_h(*n_it);
						is_find = true;
					}
				}
				if (!is_find) {
						HTree *t = new HTree(c_attr);
						t->add_h(*n_it);
						m_leaf.push_back(t);
				}
			}
			
			//Now the child will iterate themselves
			vector<HTree *> :: iterator l_it;
			for(l_it =m_leaf.begin();l_it != m_leaf.end();l_it++){
				(*l_it)->build();
			}
		}
		
		double calc_ig(Type t){
			vector<int> tested;			
			vector<Heart> :: iterator n_it;
			double ig = 1.0;
			for (n_it = m_list.begin();n_it!= m_list.end() ; n_it++){
				int data = (*n_it).getType(t);
				if (find(tested.begin(), tested.end(), data)==tested.end()) {
				   	tested.push_back(data);
					vector<Heart> :: iterator it;
					int pos = 0;
					int neg = 0;
					for (it = m_list.begin();it!= m_list.end() ; it++){	
						if(data  == (*it).getType(t)){
							if((*it).getDead() == 1){
								pos += 1;
							}else{
								neg += 1;
							}
						}
					}
					ig = ig - (double)(pos+neg)/(double)(m_pos+m_neg) *calc_entropy((double)pos,(double)neg);
				}
			}
			return ig;
		}

		//Testing
		double train_accuracy(){
			vector<Heart> :: iterator it ;
			int correct = 0;

			for (it = m_list.begin();it!= m_list.end() ; it++) {
				if(test(*it)){
					correct += 1;
				}
       		}
			return (double)correct/(double)(m_pos+m_neg);
		}

		
		bool test(Heart &p){
			HTree * root = this;
			while(root->getType()!= T_LEAF){
				Type t = root->getType();
				vector<HTree *> :: iterator it ;
				for (it = m_leaf.begin();it!= m_leaf.end();it++){
					if((*it)->getId() == p.getType(t)){
						root = (*it);
						break;
					}
				}
			}
			return (root->getDead() == p.getDead());
		}

};




int main(){

	ifstream trainH("HTrainSet.txt");
	ifstream testH("HTestSet.txt");
	HTree *h_dtl = new HTree();
	
	/
	if (trainH.is_open()) {
		int age, prior,anti, alle,dead;
		double score;
		while (trainH.good()) {
		   	trainH >> age >> prior>>anti>>alle>>score>>dead;
			Heart p(age,prior,anti,alle,score,dead);
			h_dtl->add_h(p);
		}
	   	trainH.close();
	}

	h_dtl->build();
	cout<<"Accuracy for training data"<<h_dtl->train_accuracy() <<endl;   
	
	//calculate based on entropy
	int h_count = 0;
	int h_correct = 0;

	if(testH.is_open()){
		int age, prior, dead, anti, alle;
		double score;
		while (testH.good()) {
		   	testH >> age >> prior>>anti>>alle>>score>>dead;
			Heart p(age,prior,anti,alle,score,dead);
			if(h_dtl->test(p)){
				h_correct += 1;
			}
			h_count += 1;
		}
	   	testH.close();
	}

	cout<<"Accuracy for testing data"<<(double)h_correct/(double)h_count<<endl;
  
    return 0;        
}

