//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include <limits.h>

using namespace std;

int main(int argc, char* argv[]) {
	FileManager fm;

	// open the file for reading
	char* filename = argv[1];
	int number = stoi(string(argv[2]));

	FileHandler fh = fm.OpenFile (filename);
	// cout << "File opened" << endl;

	// Get the very first page and its data
	PageHandler first = fh.FirstPage ();
	PageHandler last = fh.LastPage ();
	PageHandler mid_page;
	
	// start the algo
	int start = first.GetPageNum();
	int end = last.GetPageNum();
	// cout << start << " " << end<< endl;

	int mid = ( start + end )/2;

	int index = -1;

	while(start<=end){
		
		// fetch the mid page data
		mid = ( start + end )/2;
		mid_page = fh.PageAt(mid);
		char* data = mid_page.GetData();

		int s=0;
		// check first page number
		memcpy (&s, &data[0], sizeof(int));
		if(number < s){
			// search left
			end = mid-1;
			continue;
		}
		// check if nothing is present
		if(s == INT_MIN)
			break;
		
		// find size of data
		int i=0;
		while(s != INT_MIN){
			memcpy (&s, &data[4*i], sizeof(int));
			i++;
		}
		int size = i-1;

		// check last page number
		memcpy (&s, &data[4*(size-1)], sizeof(int));
		if(number > s){
			// search right
			start = mid+1;
			continue;
		}
		
		// we reach here only when we can find the number in page (only)
		// search the number
		for(i=0; i<size; i++){
			memcpy (&s, &data[4*i], sizeof(int));

			if(s == number){
				index = i;
				break;
			}
		}
		break;
	}
	// print the result
	if(index == -1)
		cout << "-1,-1\n";

	else{
		cout << mid+1 <<"," << index+1 << endl;
	}

	// Close the file
	fm.CloseFile (fh);
}
