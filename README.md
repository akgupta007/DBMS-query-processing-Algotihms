# This is a course assignment for the graduate-level Database Management System course taught by [**Prof. Maya Ramanath**]

- Code
	+ Buffer Manager maintains buffer of given number of Pages with LRU replacement Policy
	+ File Manager is the wrapper over Buffer Manager to enable File reading. 
	+ Read documentation.txt for details on File Manager.
	+ Following algorithms have been implemented using File Manager. 
		+ Insertion
		+ External Merge Sort
		+ Binary Search

- Testcases
	+ Sample test files have been added to testcases directory. Read the README.txt for details on the testcases.

- Note
	+ Please don't try any funny business like by-passing the file_manager or storing the complete (large) file in your own memory. The file manager we provided must be used for every access to file.
