sampleobjects_insertion = buffer_manager.o file_manager.o insertion.o
sampleobjects_merge_sort = buffer_manager.o file_manager.o merge_sort.o
sampleobjects_binary_search = buffer_manager.o file_manager.o binary_search.o
sampleobjects_check = buffer_manager.o file_manager.o check.o

check : $(sampleobjects_check)
	     g++ -std=c++11 -o check $(sampleobjects_check)

check.o : check.cpp
	g++ -std=c++11 -c check.cpp

merge_sort : $(sampleobjects_merge_sort)
	     g++ -std=c++11 -o merge_sort $(sampleobjects_merge_sort)

insertion : $(sampleobjects_insertion)
	     g++ -std=c++11 -o insertion $(sampleobjects_insertion)

binary_search : $(sampleobjects_binary_search)
	     g++ -std=c++11 -o binary_search $(sampleobjects_binary_search)

binary_search.o : binary_search.cpp
	g++ -std=c++11 -c binary_search.cpp

merge_sort.o : merge_sort.cpp
	g++ -std=c++11 -c merge_sort.cpp

insertion.o : insertion.cpp
	g++ -std=c++11 -c insertion.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f insertion
	rm -f merge_sort
	rm -f binary_search
