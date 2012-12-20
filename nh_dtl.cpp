#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

const double LOG_2 = log10(2);

enum Type{
	T_ROOT = 0,
	T_AGE,
	T_PRIOR,
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
class NoHeart{
	private:
		int m_ageLevel;
		int m_prior;
		int m_dead;

	public:
		NoHeart( int age = 10, int prior = 0, int dead = 0 ){
			m_ageLevel = age/10;
			m_prior = prior;
			m_dead = dead;
		}

		NoHeart(const NoHeart &p ){
			m_ageLevel = p.getAgeLevel();
			m_prior = p.getPrior();
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

		int getType(Type t){
			if(t == T_PRIOR){
				return m_prior;
			}else{
				return m_ageLevel;
			}
		}
};



class NHTree{
	private:
		int m_dead, m_pos, m_neg;
		vector<NoHeart> m_list;
		vector<NHTree*> m_leaf;
		vector<Type> m_attr;
		Type m_type;
		int m_id;

	public:
		NHTree(){
			m_dead = 0;
			m_pos = 0;
			m_neg = 0;
			m_type = T_ROOT;
			m_attr.push_back(T_AGE);
			m_attr.push_back(T_PRIOR);
		}

		NHTree(vector<Type> & attr){
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

		void add_nh(NoHeart &p){
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
			double max_ig = -1;
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
			}

			//Get the remaining attribute
			vector<Type> c_attr;
			for(it = m_attr.begin();it!= m_attr.end() ; it++){
				if((*it) != m_type){
					c_attr.push_back(*it);
				}
			}
			
			//Find the id of children list
			vector<NoHeart> :: iterator n_it;
			for (n_it = m_list.begin();n_it!= m_list.end() ; n_it++){
				vector<NHTree *> :: iterator l_it;
				bool is_find = false;
				for(l_it =m_leaf.begin();l_it != m_leaf.end();l_it++){
					if((*l_it)->getId() == (*n_it).getType(m_type)){
						(*l_it)->add_nh(*n_it);
						is_find = true;
					}
				}
				if (!is_find) {
						NHTree *t = new NHTree(c_attr);
						t->add_nh(*n_it);
						m_leaf.push_back(t);
				}
			}
			
			//Now the child will iterate themselves
			vector<NHTree *> :: iterator l_it;
			for(l_it =m_leaf.begin();l_it != m_leaf.end();l_it++){
				(*l_it)->build();
			}
		}
		
		double calc_ig(Type t){
			vector<int> tested;			
			vector<NoHeart> :: iterator n_it;
			double ig = 0.0;
			for (n_it = m_list.begin();n_it!= m_list.end() ; n_it++){
				int data = (*n_it).getType(t);
				if (find(tested.begin(), tested.end(), data)==tested.end()) {
				   	tested.push_back(data);
					vector<NoHeart> :: iterator it;
					int pos = 0;
					int neg = 0;
					for (it = m_list.begin();it!= m_list.end() ; it++){	
						if(data  = (*it).getType(t)){
							if((*it).getDead() == 1){
								pos += 1;
							}else{
								neg += 1;
							}
						}
					}
					ig -= calc_entropy((double)pos,(double)neg);
				}
			}
			return ig;
		}

		//Testing
		double train_accuracy(){
			vector<NoHeart> :: iterator it ;
			int correct = 0;

			for (it = m_list.begin();it!= m_list.end() ; it++) {
				if(test(*it)){
					correct += 1;
				}
       		}
			return (double)correct/(double)(m_pos+m_neg);
		}

		
		bool test(NoHeart &p){
			NHTree * root = this;
			while(root->getType()!= T_LEAF){
				Type t = root->getType();
				vector<NHTree *> :: iterator it ;
				for (it = m_leaf.begin();it!= m_leaf.end();it++){
					if((*it)->getId() == p.getType(t)){
						root = (*it);
						break;
					}
				}
			}
			return (root->getDead() == p.getDead());
		}

		bool is_empty(){
			return m_leaf.empty();
		}
};




int main(){

	ifstream trainNH("NHTrainSet.txt");
	ifstream testNH("NHTestSet.txt");
	NHTree *nh_dtl = new NHTree();
	if (trainNH.is_open()) {
		int age, prior, dead;
		while (trainNH.good()) {
		   	trainNH >> age >> prior >> dead;
			NoHeart p(age,prior,dead);
			nh_dtl->add_nh(p);
		}
	   	trainNH.close();
	}
	nh_dtl->build();
	cout<<"Accuracy for training data"<< nh_dtl->train_accuracy() <<endl;   
	
	//calculate based on entropy
	int nh_count = 0;
	int nh_correct = 0;

	if(testNH.is_open()){
		int age, prior, dead;
		while (testNH.good()) {
		   	testNH >> age >> prior >> dead;
			NoHeart p(age,prior,dead);
			if(nh_dtl->test(p)){
				nh_correct += 1;
			}
			nh_count += 1;
		}
	   	testNH.close();
	}
	cout<<"Accuracy for testing data"<<(double)nh_correct/(double)nh_count <<endl;
  
    return 0;        
}

