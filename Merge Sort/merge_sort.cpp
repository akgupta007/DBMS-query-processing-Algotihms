#include <iostream>
#include <fstream>
#include "constants.h"
#include "file_manager.h"
#include "errors.h"
#include <bits/stdc++.h> 
#include <string>
#include <cmath>
using namespace std;

void sort_file(int *pageData){
	int n;
	int i = 0;	
	for(; i< PAGE_CONTENT_SIZE/sizeof(int); i++){
		memcpy (&n, &pageData[i], sizeof(int));
		if(n == INT_MIN) break;
	}
	sort(pageData, pageData + i);
}

void create_runs(FileManager &fm, char *filename){
	FileHandler fh = fm.OpenFile(filename);
	PageHandler ph = fh.LastPage();
	string file = "run_"+to_string(0);
	FileHandler fh_new;
	try{
		fh_new = fm.CreateFile(file.c_str());
	}
	catch(InvalidFileException err){
		fm.DestroyFile(file.c_str());
		fh_new = fm.CreateFile(file.c_str());
	}
	PageHandler ph_new;
	int *pageData;
	int *pageData_new;
	int temp;
	int p = INT_MIN;
	int total_pages = ph.GetPageNum() + 1;
	int size_new_page = 0;
	for(int i=0; i<total_pages; i++){
		ph = fh.PageAt(i);
		fh.UnpinPage(i);
		pageData = (int* ) ph.GetData();
		int n = 0;
		memcpy (&temp, &pageData[n], sizeof(int));
		while( temp != INT_MIN ){
			n++;
			if (size_new_page == 0){
				ph_new = fh_new.NewPage();
				pageData_new = (int *) ph_new.GetData();
				fh_new.MarkDirty(ph_new.GetPageNum());		
			}
			memcpy (&pageData_new[size_new_page], &temp, sizeof(int));
			size_new_page++;
			memcpy (&pageData_new[size_new_page], &p, sizeof(int));
			memcpy (&temp, &pageData[n], sizeof(int));
			if (size_new_page == PAGE_CONTENT_SIZE/sizeof(int) - 1){
				sort(pageData_new, pageData_new + size_new_page);
				size_new_page = 0;
				fh_new.UnpinPage(ph_new.GetPageNum());
				fh_new.FlushPages();
			}
		}
	}
	fh.FlushPages();
	bool wh = fh_new.UnpinPage(ph_new.GetPageNum());
	if (wh == 1){
		sort(pageData_new, pageData_new + size_new_page);
		fh_new.FlushPage(ph_new.GetPageNum());
	}
	fm.CloseFile(fh_new);
	return;
}

void copy_into(FileManager &fm, const char *filename, char *output_file){
	FileHandler fh = fm.OpenFile(filename);
	PageHandler ph = fh.LastPage();
	FileHandler fh_new;
	try{
		fh_new = fm.CreateFile(output_file);
	}
	catch(InvalidFileException err){
		fm.DestroyFile(output_file);
		fh_new = fm.CreateFile(output_file);
	}
	PageHandler ph_new;
	int *pageData;
	int *pageData_new;
	int total_pages = ph.GetPageNum() + 1;
	for(int i=0; i<total_pages; i++){
		ph_new = fh_new.NewPage();
		ph = fh.PageAt(i);
		fh.UnpinPage(i);
		pageData = (int* ) ph.GetData();
		pageData_new = (int *) ph_new.GetData();
		memcpy (&pageData_new[0], &pageData[0], PAGE_CONTENT_SIZE);
		fh_new.MarkDirty(ph_new.GetPageNum());
		fh_new.UnpinPage(ph_new.GetPageNum());
		fh_new.FlushPages();
	}
	fh.FlushPages();
	fh_new.FlushPages();
	fm.CloseFile(fh_new);
	return;
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

void sort_buffer(FileHandler &fh, FileHandler &fh_new, vector<int> indexes, int offset, int total_pages){
	vector<int> counter;
	PageHandler ph[indexes.size()];
	vector<char*>pageData;
	for(int i=0; i < indexes.size(); i++){
		ph[i] = fh.PageAt(indexes[i]);
		pageData.push_back(ph[i].GetData());
		counter.push_back(0);
	}
	int min;
	memcpy (&min, &pageData[0][counter[0]*sizeof(int)], sizeof(int));
	int temp;
	int pos = 0;
	int size_new_page = 0;
	PageHandler ph_new;
	char *pageData_new;
	int p = INT_MIN;
	int n =0;
	// memcpy (&pageData_new[0], &p, sizeof(int));
	int c = 0;
	while(true){
		//cout<<n;
		if(n == indexes.size()){
			n = 0;
			//cout<<endl;
			//cout<<c++;
			counter[pos]++;
			if(size_new_page == 0){
				ph_new = fh_new.NewPage();
				pageData_new = ph_new.GetData();
				memcpy (&pageData_new[0], &p, sizeof(int));
				bool wh = fh_new.MarkDirty(ph_new.GetPageNum());
			}
			memcpy (&pageData_new[size_new_page*sizeof(int)], &min, sizeof(int));
			size_new_page++;
			memcpy (&pageData_new[size_new_page*sizeof(int)], &p, sizeof(int));
			if(size_new_page == PAGE_CONTENT_SIZE/sizeof(int)-1){
				size_new_page = 0;
				fh_new.UnpinPage(ph_new.GetPageNum());
				fh_new.FlushPage(ph_new.GetPageNum());
				// ph_new = fh_new.NewPage();
				// pageData_new = ph_new.GetData();
				// memcpy (&pageData_new[0], &p, sizeof(int));
			}
			memcpy (&min, &pageData[0][counter[0]*sizeof(int)], sizeof(int));
			pos = 0;
		}
		memcpy (&temp, &pageData[n][counter[n]*sizeof(int)], sizeof(int));
		if(temp == INT_MIN){
			fh.UnpinPage(indexes[n]);
			fh.FlushPage(indexes[n]);
			indexes[n]++;
			if(indexes[n] % offset == 0 || indexes[n] == total_pages){
				indexes.erase(indexes.begin() + n);
				pageData.erase(pageData.begin() + n);
				counter.erase(counter.begin() + n);
				if(indexes.empty()) {
					bool wh = fh_new.UnpinPage(ph_new.GetPageNum());
					if(wh == 1){
						fh_new.FlushPage(ph_new.GetPageNum());
					}
					fh.FlushPages();
					return;
				}
			}
			else{
				ph[n] = fh.PageAt(indexes[n]);
				pageData[n] = ph[n].GetData();
				counter[n] = 0;	
			}
			if(n == 0){
				memcpy (&min, &pageData[0][counter[0]*sizeof(int)], sizeof(int));
				pos = 0;
			}
			continue;
		}
		if( min > temp){
			pos = n;
			min = temp;
		}
		n++;
	}
}

string merge_files(int r, int total_pages, FileManager &fm,const char *filename){
	FileHandler fh = fm.OpenFile(filename);
	int avail_buf_size = BUFFER_SIZE - 1;
	vector<int> indexes;
	int offset = pow(avail_buf_size,(r-1));
	string s = "run_" + to_string(r);
	const char *file = s.c_str();
	FileHandler fh_new;
	try{
		fh_new = fm.CreateFile(file);
	}
	catch(InvalidFileException err){
		fm.DestroyFile(file);
		fh_new = fm.CreateFile(file);
	}
	// cout<<offset<<endl;
	for(int i=0; i*offset<total_pages; i++){
		int position = i*offset;
		indexes.push_back(i*offset);
		if(i%avail_buf_size == (avail_buf_size - 1) || (i+1)*offset >= total_pages){
			sort_buffer(fh, fh_new, indexes, offset, total_pages);
			indexes.clear();
		}
	}
	float condition = total_pages/pow(avail_buf_size,r);
	total_pages = fh_new.LastPage().GetPageNum() + 1;
	fm.CloseFile(fh);
	fm.CloseFile(fh_new);
	// cout<<file<<endl;
	// cout<<condition <<endl;
	fm.DestroyFile(filename);
	if(condition <= 1) return string(file);
	return merge_files(r+1, total_pages, fm, file);
}
void create(FileManager &fm, int k){
	for(int r = 0; r<k ;r++){
		string s = "run_" + to_string(r);
		const char *file = s.c_str();
		FileHandler fh_new;
		try{
			fh_new = fm.CreateFile(file);
		}
		catch(InvalidFileException err){
			fm.DestroyFile(file);
			fh_new = fm.CreateFile(file);
		}
	}
}

int main(int argc, char* argv[]){
	FileManager fm;
	FileHandler fh;
	
	// Reopen the same file, but for reading this time
	
	create_runs(fm, argv[1]);
	string file = "run_"+to_string(0);
	fh = fm.OpenFile (file.c_str());
	PageHandler ph_last = fh.LastPage ();
	int total_pages = ph_last.GetPageNum ()+1;
	// cout<<total_pages<<endl;
	// for(int i=0; i<total_pages; i++){
	// 	cout << endl;
	// 	cout << "Page Number: " << i <<endl;
 //    	PrintPage(fh.PageAt(i));
 //    	fh.UnpinPage(i);
 //    }
    fh.FlushPages();

    string output = merge_files(1, total_pages, fm, file.c_str());
	// cout<<output<<endl;
	fh = fm.OpenFile(output.c_str());
	ph_last = fh.LastPage ();
	total_pages = ph_last.GetPageNum ()+1;
	// for(int i=0; i<total_pages; i++){
	// 	cout << endl;
	// 	cout << "Page Number: " << i <<endl;
 //    	PrintPage(fh.PageAt(i));
 //    	fh.UnpinPage(i);
 //    }
    copy_into(fm, output.c_str() , argv[2]);
    fm.DestroyFile(output.c_str());
    fh.FlushPages();
	fm.CloseFile (fh);
	return 0;
}