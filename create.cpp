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