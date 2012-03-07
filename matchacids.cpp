#include <stdio.h>  // Not using iostream for Linux compatibility.
#include <string.h> // So that we can use strcmp() on Linux.
#include <stdlib.h> // Use exit() on Linux.
#include <vector>   // Arrays of dynamic length
#include <sstream>  // Convert between strings and numbers
#include <stdarg.h> // Functions that take variable number of arguments

/* DATA STRUCTURES */
// Stores the information necessary to identify an amino acid.
class AminoAcid {
public:
	std::string resname; // Amino acid name
	unsigned int resid;  // Amino acid number
	char chain;          // Chain name
	
	std::string to_metapocket() {
		// Convert resid from int to string
		std::stringstream resid_out;
		resid_out << resid;
		std::string to_str = resname + "^" + resid_out.str() + "^" + chain;
		
		return to_str;
	}
	
	bool equalTo(AminoAcid other) {
		return (resid == other.resid && chain == other.chain);
	}
	
	std::string to_cdd() {
		// TODO
		std::string to_str = "unimplemented";
		return to_str;
	}
private:
};

// Stores two AminoAcid objects.
class AminoAcidRange {
public:
	AminoAcid a;
	AminoAcid b;
	
	std::string to_metapocket() {
		// Get the string representations of the two acids.
		std::string to_str = a.to_metapocket() + "-" + b.to_metapocket();
		return to_str;
	}
	
	std::string to_cdd() {
		std::string to_str = a.to_cdd() + "-" + b.to_cdd();
		return to_str;
	}
private:
};

struct ProteinFile {
	FILE* file;
	unsigned int format;
};

/* GLOBAL VARIABLES */
// Stores the list of acids.
std::vector<AminoAcidRange> acids1;

// Stores the paths of the 2 files that we take as input.
std::string input1_path, input2_path;

// Stores whether the user passed in -v as an argument.
bool isDebug = false;

/* ENUMERATIONS */
enum arg_indicies {
	i_command, // Ignored in parsing because this is the name of the program
	i_input1,
	i_input2,
	i_verbosity,
	arg_indicies_length // So that we can find the length of the enum without hardcoding it.
};

enum formats {
	form_vmd,
	form_metapocket,
	form_cdd,
	s_length
};

/* FUNCTION PROTOTYPES */
// See function bodies for detailed descriptions.
void print_help();
void debug(const char* format, ...);

void load_acids1();
unsigned int search_acids1(AminoAcid target);
void compare_acids2();

void print_acids();
void write_acids();
void output_acids(FILE* dest);

/* MAIN */
int main(int argc, const char* argv[]) {
	// ARGUMENTS
	// Did we get enough arguments to run the program?
	if (argc < arg_indicies_length - 1) {
		printf("error: too few arguments\n");
		print_help();
		return 1;
	}
	
	// The user asked for help by passing in -h or --help.
	if (argv[1][1] == 'h' || strcmp("--help", argv[1]) == 0) {
		print_help();
		return 0;
	}
	
	// Grab the paths of the 2 input files.
	input1_path = argv[i_input1];
	input2_path = argv[i_input2];
	
	// The user wants verbose mode.
	if (argc > arg_indicies_length - 1 && argv[i_verbosity][1] == 'v') {
		isDebug = true;
	}
	
	debug("Parsed arguments\n");
	
	// ACTUAL PROCESSING
	
	load_acids1();
	
	compare_acids2();
	
	return 0;
}

void print_help() {
	// TODO: write a help message
	printf("Unimplemented\n");
}

// Drop-in replacement for printf, but only prints when isDebug is true.
void debug(const char* format, ...) {
	if (isDebug) {
		va_list argp;
		va_start(argp, format);
		vprintf(format, argp);
		va_end(argp);
	}
}

ProteinFile open_proteinfile(std::string path) {
	ProteinFile pf;
	
	debug("Opening file %s\n", path.c_str());
	pf.file = fopen(path.c_str(), "r");
	// fopen() returns null if it can't open the file for whatever reason.
	if (pf.file == NULL) {
		printf("error: can't open input file: %s\n", path.c_str());
		exit(1);
	}
	
	// Finds the first instance of '.' after index 0.
	unsigned int format_ext_start = path.find_first_of('.', 0) + 1;
	
	// Grab the 3 characters after the first '.'
	std::string format_ext = path.substr(format_ext_start, 3);
	debug("identified format string as %s at index %d\n", format_ext.c_str(), format_ext_start);
	if (format_ext == "vmd") {
		pf.format = form_vmd;
	}
	else if (format_ext == "mtp") {
		pf.format = form_metapocket;
	}
	else if (format_ext == "cdd") {
		pf.format = form_cdd;
	}
	else {
		printf("error: invalid acid format extension for %s\n", path.c_str());
		exit(3);
	}
	
	debug("format ID for this file is %d\n", pf.format);
	
	return pf;
}

// Grab acids from the first file and throw them into the acids1 vector.
void load_acids1() {
	// Open the file using the method
	ProteinFile file_acids1 = open_proteinfile(input1_path);
	
	if (file_acids1.format != form_vmd) {
		printf("error: file1 must be in VMD format.\n");
		exit(5);
	}
	
	// Loop while we hit an end-of-file (EOF) character.
	// i isn't declared with the for loop because we want to access it later.
	unsigned int i;
	for (i = 0; feof(file_acids1.file) == 0; i++) {
		// Store the next line of the file.
		debug("Loading next line\n");
		
		// Grab the acid represented by the string.
		debug("Parsing acid %d\n", i);
		
		// Some temporary variables to hold the data.	
		// TODO: Is it necessary to have a 4th character that's set to null?
		char resname1[4], resname2[4];
		resname1[3] = resname2[3] = '\0';
		unsigned int resid1, resid2;
		char chain1, chain2;
		
		// Use fscanf to extract formatted data from the file.
		int fscanresult = fscanf(file_acids1.file, "%3s%u_chain%c-%3s%u_chain%c", 
			resname1, &resid1, &chain1, resname2, &resid2, &chain2);
		
		// Save our data into an AminoAcidRange object.
		AminoAcidRange temp_range;
		temp_range.a.resname = resname1;
		temp_range.a.resid = resid1;
		temp_range.a.chain = chain1;
		
		temp_range.b.resname = resname2;
		temp_range.b.resid = resid2;
		temp_range.b.chain = chain2;
		
		debug("Data: %s, %d, %c to %s, %d, %c\n", resname1, resid1, chain1, resname2, resid2, chain2);
		debug("Parsed acid range: %s\n", temp_range.to_metapocket().c_str());
		
		// Add this amino acid to the list
		acids1.push_back(temp_range);
		debug("\n");
	}
	
	// Delete the last item.
	// Weird off-by-one error where the last entry is repeated.
	acids1.pop_back();
	
	// Always clean up your toys when you're done playing with them.
	debug("Closing file\n");
	fclose(file_acids1.file);
	
	// TODO: acid count isn't correct because fscanf() freaks out when we 
	// have trailing hyphens.
	debug("Parsed %d acids\n", i);
}

// Returns the index of the first acids1 element that matches AminoAcid.
// Returns NULL if it wasn't found.
unsigned int search_acids1(AminoAcid target) {
	unsigned int depends;
	// TODO: write code for searching for acids
	return NULL;
}

void compare_acids2() {
	// TODO: load the second file and find the common acids
	printf("Unimplemented compare_acids2();\n");
	
	ProteinFile file_acids2 = open_proteinfile(input2_path);
	
	// Tracks whether we're in a block, per the block strategy outlined below.
	bool mtp_inblock = false;
	// Stores each block.
	std::string buffer;
	
	// Loop while we hit an end-of-file (EOF) character.
	// i isn't declared with the for loop because we want to access it later.
	unsigned int i;
	for (i = 0; feof(file_acids2.file) == 0; i++) {
		// Pull a character from the file.
		char c = fgetc(file_acids2.file);
		
		// Stores amino acid data so that we can search for it in acids1 later.
		AminoAcid temp_acid;
		
		switch (file_acids2.format) {
			// TODO: Make sure to process each binding site separately. As in, try to
			// match EVERY acid in a site to the list. We might need another vector to
			// prevent cross-contamination of amino acids between binding sites.
			case form_metapocket:
				// Strategy: parse everything into space-delimited blocks. Whenever we have
				// a block, parse it using sscanf().
				if (!mtp_inblock && c != ' ') {					
					// The _scanf() functions can't write to C++ string objects. 
					// So we need this temp variable.
					char resname[4];
					resname[3] = '\0';
					
					// Pull data from the buffer
					debug("Parsing buffer %s\n", buffer.c_str());
					sscanf(buffer.c_str(), "%3s_%c^%d^", resname, &(temp_acid.chain), &(temp_acid.resid));
					temp_acid.resname = resname;
					debug("Parsed acid %s\n\n", temp_acid.to_metapocket().c_str());
					
					// We want to reuse the buffer variable for each block.
					buffer.clear();
				}
				if (c == ' ' && mtp_inblock) {
					mtp_inblock = false;
					debug("leaving block\n");
				}
				if (c != ' ' && !mtp_inblock) {
					mtp_inblock = true;
					debug("entering block\n");
				}
				if (mtp_inblock) {
					buffer.push_back(c);
				}
				break;
				
			case form_cdd:
				
				break;
				
			default:
				printf("error: file2 must be in CDD or MTP format.\n");
				exit(6);
				break;
		}
		
		// TODO: Search for temp_acid in acids1 vector.
	}
	
	// Always clean up your toys when you're done playing with them.
	debug("Closing file\n");
	fclose(file_acids2.file);
}

// TODO do we still need the next 3 functions? Perhaps to modify them?
/*
// Print the acids vector by saving its contents to stdout.
void print_acids() {
	debug("Printing to stdout\n");
	printf("Parsed proteins:\n");
	output_acids(stdout);
}

// Save the acids to the output file.
void write_acids() {
	FILE* writefile = fopen(path_out.c_str(), "w");
	// fopen() returns null if it can't open the file for whatever reason.
	if (writefile == NULL) {
		printf("error: can't open file: %s\n", path_out.c_str());
		return;
	}
	
	debug("Saving to file %s\n", path_out.c_str());
	output_acids(writefile);
	fclose(writefile);
}

// Write the acids to a specified output file. Assumes the FILE* is not null.
// If we want to print them to the terminal, we pass stdout as the file.
void output_acids(FILE* dest) {
	// Traverse the acids vector.
	for (int i = 0; i < acids.size(); i++) {
		// Print the requested representation (a string) to a file called dest.
		// TODO this code is no longer usable. 
		switch (output_mode) {
			case metapocket:
				fprintf(dest, "%s\n", acids[i].to_metapocket().c_str());
				break;
			case cdd:
				fprintf(dest, "%s\n", acids[i].to_cdd().c_str());
				break;
			default:
				fprintf(dest, "invalid output_mode!\n");
				break;
		}
	}
}*/
