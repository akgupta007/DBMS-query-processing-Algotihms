
#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <fstream>
#include <list>
#include <utility>      // std::pair
#include <cstring>
#include <climits>

using namespace std;

list<int> getValue(const char *filename){
	ifstream myfile(filename);
	if(!myfile){
		cout<<"could not open file " << filename <<endl;
		exit (0);
	}
	list<int> num;
	int n;
	while(!myfile.eof()){
		myfile >> n;
		num.push_back(n);
	}
	myfile.close();
	return num;
}

list<int> checkPage(list<int> lst, char* pageData){
	int n;
	int num = lst.front();
	list<int> position;
	int i = 0;
	for(; i< PAGE_CONTENT_SIZE/sizeof(int); i++){
		memcpy (&n, &pageData[i*sizeof(int)], sizeof(int));
		if (n == INT_MIN){
			break;
		}
		if (n > num){
			lst.pop_front();
			position.push_back(i);
			i--;
			if(lst.empty()){
				break;
			}
			num = lst.front();
		}
	}
	while( !lst.empty() ){
		position.push_back(i);
		lst.pop_front();
	}
	return position;
}

void InsertValue(int num, int position, int pageNo, FileHandler &fh){
	try{
		PageHandler ph = fh.PageAt(pageNo);
		bool pin = fh.MarkDirty(pageNo);
		char *pageData = ph.GetData();
		int n;
		int max_data = PAGE_CONTENT_SIZE/sizeof(int);
		for(int i = position; i< max_data; i++){
			memcpy (&n, &pageData[i*sizeof(int)], sizeof(int));
			if( n == INT_MIN ){
				if (i == max_data -1){
					fh.UnpinPage(pageNo);
					InsertValue(num, 0, pageNo + 1, fh);
					return;
				}
				else {
					memcpy (&pageData[i*sizeof(int)], &num, sizeof(int));
					memcpy (&pageData[(i+1)*sizeof(int)], &n, sizeof(int));
					break;
				}
			}
			memcpy (&pageData[i*sizeof(int)], &num, sizeof(int));
			num = n;
		}
		return;
	}
	catch(InvalidPageException args){
		//create New Page
		PageHandler ph = fh.NewPage();
		char *pageData = ph.GetData();
		int m = INT_MIN;
		memcpy (&pageData[0], &m, sizeof(int));
		InsertValue(num, 0, ph.GetPageNum(), fh);
		return;
	}
}


void PrintPage(PageHandler ph){
	char* data = ph.GetData ();
	int temp_num;
	// Output the all integers
	for(int i=0; i<1022 ; i++){
		memcpy (&temp_num, &data[i*4], sizeof(int));
		cout << i<< "th number: " << temp_num << endl;
		if(temp_num == INT_MIN) break;
	}
	return;
}

list<pair<int, int>> binary_search(list<int> num, FileHandler &fh, int first, int last){
	list<pair<int, int>> location;
	PageHandler ph;
	int mid = (first + last)/2;
	list<int> pr;
	ph = fh.PageAt(mid);
	fh.UnpinPage(mid);
	char *pageData = ph.GetData();
	pr = checkPage (num, pageData); // returns position which is in the page

// for (auto v : pr)
 //        std::cout << v << "  ";
    list<int> first_half;
	list<int> second_half;
	int n;
	list<pair<int, int>>::iterator end_location = location.begin();
	int flag = 0;
	for(list<int>::iterator it=pr.begin(), it_num = num.begin(); it_num != num.end(); ++it, ++it_num)
	{
		if(*it == 0){
			first_half.push_back(*it_num);
			location.push_back(make_pair(mid, *it));
		}
		else{
			memcpy (&n, &pageData[(*it)*sizeof(int)], sizeof(int));
			location.push_back(make_pair(mid, *it));
			if(n == INT_MIN){
				second_half.push_back(*it_num);
				if(flag == 0){
					flag = 1;
					end_location = location.end();
					--end_location;
				}
			}
		}
	}
	
	if (first == last){
		return location;
	}

	if( mid > first)
	{
		if(!first_half.empty())
		{
			list<pair<int, int>> first_location = binary_search(first_half, fh, first, mid - 1);
			//location.insert(location.begin(), first_location.begin(), first_location.end());
			list<pair<int, int>>::iterator start_it = location.begin();
			while( !first_location.empty() ){
				start_it = location.erase(start_it);
				location.insert(start_it, first_location.front());
				first_location.pop_front();
			}
		}
	}
	if(!second_half.empty()){
		list<pair<int, int>> last_location = binary_search(second_half, fh, mid + 1, last);
		while( !last_location.empty() ){
			if ( last_location.front().second != 0 ){
				end_location = location.erase(end_location);
				location.insert(end_location, last_location.front());
			}
			else{
				++end_location;
			}
			last_location.pop_front();
		}
	}
	return location;
}

void Insert(list<int> num, FileHandler &fh){

	PageHandler ph;
	PageHandler ph_last = fh.LastPage ();
	int total_pages = ph_last.GetPageNum ()+1;
	int last = ph_last.GetPageNum ();
	int first = 0;

	list<pair<int, int>> location;
    
    location = binary_search(num, fh, first, last);
    fh.FlushPages();
    for(list<pair<int, int>>::iterator it = location.end(); it!= location.begin();){
    	it--;
    	// cout<< "Inserting value "<< num.back() << " at Page Number " << (*it).first << " and position "<< (*it).second << "..." <<endl;
		InsertValue(num.back(), (*it).second, (*it).first, fh);
		//bool wh = fh.FlushPages();
		num.pop_back();
	}
	fh.FlushPages();

}

int main(int argc, char* argv[]) {
	//read input file
	const char *filename = argv[2];
	list<int> num = getValue(filename);
	num.sort();
	
	FileManager fm;
	FileHandler fh;
	
	// Reopen the same file, but for reading this time
	fh = fm.OpenFile (argv[1]);
	
	Insert(num, fh);
	
	PageHandler ph_last = fh.LastPage ();
	int total_pages = ph_last.GetPageNum ()+1;
	
	fh.FlushPages();
	fm.CloseFile (fh);

}