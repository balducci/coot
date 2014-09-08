#include <iostream>
#include <iomanip>


#ifndef  __STRING_H
#include <string>
#endif

#ifndef  __MMDB_Manager__
#include <mmdb2/mmdb_manager.h>
//#include"mmdb_tables.h"
#endif

#include "CXXChargeTable.h"

using namespace std;

class CXXChargeTable;

namespace CXXUtils{
	
	int assignUnitedAtomRadius(mmdb::PManager, int selHnd);
	int assignCharge(mmdb::PManager theManager, int selHnd, CXXChargeTable *theChargeTable);
	int selectionStringToSelHnd(mmdb::PManager allAtomsManager_in, std::string selectionString, int existingSelection=-1, mmdb::SELECTION_KEY selKeyRequest=mmdb::SKEY_NEW);
	int unCharge(mmdb::PManager theManager,int selHnd);
	void reformatAtomRadii();
	
	typedef struct AtomRadius_{
		string atomName;
		string residueName;
		float radius;
	} AtomRadius;
	
	static AtomRadius unitedAtomRadii[] = {
	{ " CA ", "GLY", 2.04},
	{ " CA ", "*  ", 1.99},
	{ " C  ", "*  ", 1.94},
	{ " CB ", "VAL", 1.99},
	{ " CB ", "THR", 1.99},
	{ " CB ", "ILE", 1.99},
	{ " CB ", "ALA", 2.09},
	{ " CB ", "*  ", 2.04},
	{ " CG ", "PRO", 2.04},
	{ " CG ", "MET", 2.04},
	{ " CG ", "LYS", 2.04},
	{ " CG ", "LEU", 1.99},
	{ " CG ", "GLU", 2.04},
	{ " CG ", "GLN", 2.04},
	{ " CG ", "ARG", 2.04},
	{ " CG ", "*  ", 1.94},
	{ " CG1", "VAL", 2.09},
	{ " CG1", "ILE", 2.04},
	{ " CG2", "*  ", 2.09},
	{ " CD ", "PRO", 2.04},
	{ " CD ", "LYS", 2.04},
	{ " CD ", "GLU", 1.94},
	{ " CD ", "GLN", 1.94},
	{ " CD ", "ARG", 2.04},
	{ " CD1", "LEU", 2.09},
	{ " CD1", "ILE", 2.09},
	{ " CD1", "*  ", 1.99},
	{ " CD2", "TRP", 1.94},
	{ " CD2", "LEU", 2.09},
	{ " CD2", "*  ", 1.99},
	{ " CE ", "MET", 2.09},
	{ " CE ", "LYS", 2.04},
	{ " CE1", "TYR", 1.99},
	{ " CE1", "PHE", 1.99},
	{ " CE1", "HIS", 1.99},
	{ " CE2", "TYR", 1.99},
	{ " CE2", "TRP", 1.94},
	{ " CE2", "PHE", 1.99},
	{ " CE3", "TRP", 1.99},
	{ " CZ ", "TYR", 1.94},
	{ " CZ ", "PHE", 1.99},
	{ " CZ ", "ARG", 1.94},
	{ " CZ2", "TRP", 1.99},
	{ " CZ3", "TRP", 1.99},
	{ " CH2", "TRP", 1.99},
	{ " N  ", "VAL", 1.89},
	{ " N  ", "TYR", 1.89},
	{ " N  ", "TRP", 1.89},
	{ " N  ", "THR", 1.89},
	{ " N  ", "SER", 1.89},
	{ " N  ", "PRO", 1.84},
	{ " N  ", "PHE", 1.89},
	{ " N  ", "MET", 1.89},
	{ " N  ", "LYS", 1.89},
	{ " N  ", "LEU", 1.89},
	{ " N  ", "ILE", 1.89},
	{ " N  ", "HIS", 1.89},
	{ " N  ", "GLY", 1.89},
	{ " N  ", "GLU", 1.89},
	{ " N  ", "GLN", 1.89},
	{ " N  ", "CYS", 1.89},
	{ " N  ", "ASP", 1.89},
	{ " N  ", "ASN", 1.89},
	{ " N  ", "ARG", 1.89},
	{ " N  ", "*  ", 1.89},
	{ " ND1", "HIS", 1.89},
	{ " ND2", "ASN", 1.94},
	{ " NE ", "ARG", 1.89},
	{ " NE1", "TRP", 1.89},
	{ " NE2", "HIS", 1.84},
	{ " NE2", "GLN", 1.94},
	{ " NH1", "ARG", 1.94},
	{ " NH2", "ARG", 1.94},
	{ " NZ ", "LYS", 1.99},
	{ " O  ", "*  ", 1.75},
	{ " OG ", "SER", 1.8},
	{ " OG1", "THR", 1.8},
	{ " OD1", "ASP", 1.75},
	{ " OD1", "ASN", 1.75},
	{ " OD2", "ASP", 1.75},
	{ " OE1", "GLU", 1.75},
	{ " OE1", "GLN", 1.75},
	{ " OE2", "GLU", 1.75},
	{ " OH ", "TYR", 1.8},
	{ " SG ", "CYS", 2.12},
	{ " SD ", "MET", 2.07},
	{ " O  ", "WAT", 1.85},
	{ " O  ", "DOD", 1.85},
	{ " O  ", "H2O", 1.85},
	{ " O  ", "HOH", 1.85},
	{ " O  ", "OH2", 1.85},
	{ " OW0", "WAT", 1.85},
	{ " O  ", "WAT", 1.85},
	{ " OH2", "WAT", 1.85},
	{ " OD2", "DOD", 1.85},
	{ " OH2", "H2O", 1.85},
	{ " OH2", "HOH", 1.85},
	{ " OH2", "OH2", 1.85},
	{ " OH2", "WAT", 1.85},
	{ "HG  ", "ASP", 1.6},
	{ "HD2 ", "ASP", 1.6},
	{ "HD1 ", "ASP", 1.6},
	{ "DG  ", "ASP", 1.6},
	{ "DD2 ", "ASP", 1.6},
	{ "DD1 ", "ASP", 1.6},
	{ " AD2", "ASN", 1.94},
	{ " AD1", "ASN", 1.75},
	{ "HG  ", "ASN", 1.6},
	{ "DG  ", "ASN", 1.6},
	{ "HND2", "ASN", 1.2},
	{ "HND1", "ASN", 1.2},
	{ "HD22", "ASN", 1.2},
	{ "HD21", "ASN", 1.2},
	{ "HAD2", "ASN", 1.2},
	{ " #HD2", "ASN", 1.2},
	{ " #DD2", "ASN", 1.2},
	{ "HZ  ", "ARG", 1.6},
	{ "HG2 ", "ARG", 1.6},
	{ "HG1 ", "ARG", 1.6},
	{ "HD2 ", "ARG", 1.6},
	{ "HD1 ", "ARG", 1.6},
	{ "DZ  ", "ARG", 1.6},
	{ " #HG", "ARG", 1.6},
	{ " #HD", "ARG", 1.6},
	{ " #DG", "ARG", 1.6},
	{ " #DD", "ARG", 1.6},
	{ "HNH2", "ARG", 1.2},
	{ "HNH1", "ARG", 1.2},
	{ "HNE ", "ARG", 1.2},
	{ "HN22", "ARG", 1.2},
	{ "HN21", "ARG", 1.2},
	{ "HN12", "ARG", 1.2},
	{ "HN11", "ARG", 1.2},
	{ "HH22", "ARG", 1.2},
	{ "HH21", "ARG", 1.2},
	{ "HH11", "ARG", 1.2},
	{ "HE  ", "ARG", 1.2},
	{ " #HH2", "ARG", 1.2},
	{ " #HH1", "ARG", 1.2},
	{ " #HE", "ARG", 1.2},
	{ " #DH2", "ARG", 1.2},
	{ " #DH1", "ARG", 1.2},
	{ " #DE", "ARG", 1.2},
	{ "HG  ", "CYS", 1.6},
	{ "DG  ", "CYS", 1.6},
	{ "HSG ", "CYS", 1.2},
	{ "HG  ", "CYS", 1.2},
	{ "HG2 ", "GLU", 1.6},
	{ "HG1 ", "GLU", 1.6},
	{ "HE2 ", "GLU", 1.6},
	{ "HE1 ", "GLU", 1.6},
	{ "HD  ", "GLU", 1.6},
	{ "DE2 ", "GLU", 1.6},
	{ "DE1 ", "GLU", 1.6},
	{ "DD  ", "GLU", 1.6},
	{ " #HG", "GLU", 1.6},
	{ " #DG", "GLU", 1.6},
	{ " AE2", "GLN", 1.94},
	{ " AE1", "GLN", 1.75},
	{ "HG2 ", "GLN", 1.6},
	{ "HG1 ", "GLN", 1.6},
	{ "HD  ", "GLN", 1.6},
	{ "DD  ", "GLN", 1.6},
	{ " #HG", "GLN", 1.6},
	{ " #DG", "GLN", 1.6},
	{ "HNE2", "GLN", 1.2},
	{ "HNE1", "GLN", 1.2},
	{ "HE22", "GLN", 1.2},
	{ "HE21", "GLN", 1.2},
	{ "HAE2", "GLN", 1.2},
	{ " #HE2", "GLN", 1.2},
	{ " #DE2", "GLN", 1.2},
	{ "HA2 ", "GLY", 1.6},
	{ "HA1 ", "GLY", 1.6},
	{ " #HA", "GLY", 1.6},
	{ " AE1", "HIS", 1.99},
	{ " AD2", "HIS", 1.99},
	{ " AD1", "HIS", 1.89},
	{ " AE2", "HIS", 1.84},
	{ "HE1 ", "HIS", 1.6},
	{ "HD2 ", "HIS", 1.6},
	{ "DE1 ", "HIS", 1.6},
	{ "DD2 ", "HIS", 1.6},
	{ "HND1", "HIS", 1.2},
	{ "HE2 ", "HIS", 1.2},
	{ "HD1 ", "HIS", 1.2},
	{ "HD1 ", "HIS", 1.2},
	{ "HAD1", "HIS", 1.2},
	{ "DE2 ", "HIS", 1.2},
	{ "DD1 ", "HIS", 1.2},
	{ "HG2 ", "MET", 1.6},
	{ "HG1 ", "MET", 1.6},
	{ "HE3 ", "MET", 1.6},
	{ "HE2 ", "MET", 1.6},
	{ "HE1 ", "MET", 1.6},
	{ " #HG", "MET", 1.6},
	{ " #HE", "MET", 1.6},
	{ " #DG", "MET", 1.6},
	{ " #DE", "MET", 1.6},
	{ "HG2 ", "LYS", 1.6},
	{ "HG1 ", "LYS", 1.6},
	{ "HE2 ", "LYS", 1.6},
	{ "HE1 ", "LYS", 1.6},
	{ "HD2 ", "LYS", 1.6},
	{ "HD1 ", "LYS", 1.6},
	{ " #HG", "LYS", 1.6},
	{ " #HE", "LYS", 1.6},
	{ " #HD", "LYS", 1.6},
	{ " #DG", "LYS", 1.6},
	{ " #DE", "LYS", 1.6},
	{ " #DD", "LYS", 1.6},
	{ "HZ3 ", "LYS", 1.2},
	{ "HZ2 ", "LYS", 1.2},
	{ "HZ1 ", "LYS", 1.2},
	{ "HNZ3", "LYS", 1.2},
	{ "HNZ2", "LYS", 1.2},
	{ "HNZ1", "LYS", 1.2},
	{ "HNZ ", "LYS", 1.2},
	{ " #HZ", "LYS", 1.2},
	{ " #DZ", "LYS", 1.2},
	{ "HG  ", "LEU", 1.6},
	{ "HD6 ", "LEU", 1.6},
	{ "HD5 ", "LEU", 1.6},
	{ "HD4 ", "LEU", 1.6},
	{ "HD3 ", "LEU", 1.6},
	{ "HD2 ", "LEU", 1.6},
	{ "HD1 ", "LEU", 1.6},
	{ "DG  ", "LEU", 1.6},
	{ " #HD2", "LEU", 1.6},
	{ " #HD1", "LEU", 1.6},
	{ " #DD2", "LEU", 1.6},
	{ " #DD1", "LEU", 1.6},
	{ "HG5 ", "ILE", 1.6},
	{ "HG4 ", "ILE", 1.6},
	{ "HG3 ", "ILE", 1.6},
	{ "HG2 ", "ILE", 1.6},
	{ "HG1 ", "ILE", 1.6},
	{ "HD3 ", "ILE", 1.6},
	{ "HD2 ", "ILE", 1.6},
	{ "HD1 ", "ILE", 1.6},
	{ " #HG2", "ILE", 1.6},
	{ " #HG1", "ILE", 1.6},
	{ " #HD1", "ILE", 1.6},
	{ " #DG2", "ILE", 1.6},
	{ " #DG1", "ILE", 1.6},
	{ " #DD1", "ILE", 1.6},
	{ "HG  ", "SER", 1.6},
	{ "DG  ", "SER", 1.6},
	{ "HOG ", "SER", 1.2},
	{ "HG  ", "SER", 1.2},
	{ "HG2 ", "PRO", 1.6},
	{ "HG1 ", "PRO", 1.6},
	{ "HD2 ", "PRO", 1.6},
	{ "HD1 ", "PRO", 1.6},
	{ " #HG", "PRO", 1.6},
	{ " #HD", "PRO", 1.6},
	{ " #DG", "PRO", 1.6},
	{ " #DD", "PRO", 1.6},
	{ "HZ  ", "PHE", 1.6},
	{ "HE2 ", "PHE", 1.6},
	{ "HE1 ", "PHE", 1.6},
	{ "HD2 ", "PHE", 1.6},
	{ "HD1 ", "PHE", 1.6},
	{ "DZ  ", "PHE", 1.6},
	{ "DE2 ", "PHE", 1.6},
	{ "DE1 ", "PHE", 1.6},
	{ "DD2 ", "PHE", 1.6},
	{ "DD1 ", "PHE", 1.6},
	{ "HH  ", "TYR", 1.6},
	{ "HE2 ", "TYR", 1.6},
	{ "HE1 ", "TYR", 1.6},
	{ "HD2 ", "TYR", 1.6},
	{ "HD1 ", "TYR", 1.6},
	{ "DH  ", "TYR", 1.6},
	{ "DE2 ", "TYR", 1.6},
	{ "DE1 ", "TYR", 1.6},
	{ "DD2 ", "TYR", 1.6},
	{ "DD1 ", "TYR", 1.6},
	{ "HOH ", "TYR", 1.2},
	{ "HH  ", "TYR", 1.2},
	{ "HZ2 ", "TRP", 1.6},
	{ "HZ1 ", "TRP", 1.6},
	{ "HH  ", "TRP", 1.6},
	{ "HE  ", "TRP", 1.6},
	{ " #HZ3", "TRP", 1.6},
	{ " #HZ2", "TRP", 1.6},
	{ " #HH2", "TRP", 1.6},
	{ " #HE3", "TRP", 1.6},
	{ " #HD1", "TRP", 1.6},
	{ " #DZ3", "TRP", 1.6},
	{ " #DZ2", "TRP", 1.6},
	{ " #DH2", "TRP", 1.6},
	{ " #DE3", "TRP", 1.6},
	{ " #DD1", "TRP", 1.6},
	{ "HNE1", "TRP", 1.2},
	{ "HNE ", "TRP", 1.2},
	{ "HE1 ", "TRP", 1.2},
	{ " #HE1", "TRP", 1.2},
	{ " #DE1", "TRP", 1.2},
	{ "HG3 ", "THR", 1.6},
	{ "HG2 ", "THR", 1.6},
	{ "HG1 ", "THR", 1.6},
	{ "HB  ", "THR", 1.6},
	{ " #HG2", "THR", 1.6},
	{ " #HG1", "THR", 1.6},
	{ " #DG2", "THR", 1.6},
	{ " #DG1", "THR", 1.6},
	{ "HOG1", "THR", 1.2},
	{ "HOG ", "THR", 1.2},
	{ "HG1 ", "THR", 1.2},
	{ "HG6 ", "VAL", 1.6},
	{ "HG5 ", "VAL", 1.6},
	{ "HG4 ", "VAL", 1.6},
	{ "HG3 ", "VAL", 1.6},
	{ "HG2 ", "VAL", 1.6},
	{ "HG1 ", "VAL", 1.6},
	{ "HB  ", "VAL", 1.6},
	{ " #HG2", "VAL", 1.6},
	{ " #HG1", "VAL", 1.6},
	{ " #DG2", "VAL", 1.6},
	{ " #DG1", "VAL", 1.6},
	{ " CA ", "CBX", 2.09},
	{ " ND1", "HRU", 1.84},
	{ " ND1", "HSD", 1.84},
	{ " ND1", "HSP", 1.89},
	{ " NE2", "HRU", 1.89},
	{ " NE2", "HSD", 1.89},
	{ " NE2", "HSP", 1.89},
	{ " OT2", "CTE", 1.75},
	{ " OT2", "CTR", 1.75},
	{ " CH3", "ACE", 2.09},
	{ " C3 ", "ACE", 2.09},
	{ "HO  ", "WAT", 1.2},
	{ "H2  ", "WAT", 1.2},
	{ "H1  ", "WAT", 1.2},
	{ " CB ", "UNK", 1.94},
	{ " N  ", "UNK", 1.89},
	{ " C6 ", "U  ", 1.99},
	{ " C5 ", "U  ", 1.99},
	{ " C4 ", "U  ", 1.94},
	{ " C2 ", "U  ", 1.94},
	{ " N3 ", "U  ", 1.89},
	{ " N1 ", "U  ", 1.84},
	{ " O4 ", "U  ", 1.75},
	{ " O2 ", "U  ", 1.75},
	{ "HN3 ", "U  ", 1.2},
	{ " C5M", "T  ", 2.09},
	{ " C5A", "T  ", 2.09},
	{ " C6 ", "T  ", 1.99},
	{ " C5 ", "T  ", 1.94},
	{ " C4 ", "T  ", 1.94},
	{ " C2 ", "T  ", 1.94},
	{ " N3 ", "T  ", 1.89},
	{ " N1 ", "T  ", 1.84},
	{ " O4 ", "T  ", 1.75},
	{ " O2 ", "T  ", 1.75},
	{ "HN3 ", "T  ", 1.2},
	{ "HO  ", "OH2", 1.2},
	{ "H2  ", "OH2", 1.2},
	{ "H1  ", "OH2", 1.2},
	{ "HT3 ", "NTR", 1.2},
	{ "HT2 ", "NTR", 1.2},
	{ "HT1 ", "NTR", 1.2},
	{ "HT3 ", "NTE", 1.2},
	{ "HT2 ", "NTE", 1.2},
	{ "HT1 ", "NTE", 1.2},
	{ " C8 ", "NT ", 2.09},
	{ " C14", "NT ", 2.09},
	{ " C2 ", "NT ", 2.04},
	{ " C17", "NT ", 2.04},
	{ " C16", "NT ", 2.04},
	{ " C7 ", "NT ", 1.99},
	{ " C5 ", "NT ", 1.99},
	{ " C13", "NT ", 1.99},
	{ " C11", "NT ", 1.99},
	{ " N9 ", "NT ", 1.94},
	{ " N2 ", "NT ", 1.94},
	{ " N10", "NT ", 1.94},
	{ " N1 ", "NT ", 1.94},
	{ " C9 ", "NT ", 1.94},
	{ " C6 ", "NT ", 1.94},
	{ " C4 ", "NT ", 1.94},
	{ " C3 ", "NT ", 1.94},
	{ " C18", "NT ", 1.94},
	{ " C15", "NT ", 1.94},
	{ " C12", "NT ", 1.94},
	{ " C10", "NT ", 1.94},
	{ " C1 ", "NT ", 1.94},
	{ " N8 ", "NT ", 1.89},
	{ " N6 ", "NT ", 1.89},
	{ " N4 ", "NT ", 1.89},
	{ " N3 ", "NT ", 1.89},
	{ " N7 ", "NT ", 1.84},
	{ " N5 ", "NT ", 1.84},
	{ " O3 ", "NT ", 1.75},
	{ " O2 ", "NT ", 1.75},
	{ " O1 ", "NT ", 1.75},
	{ "HN9 ", "NT ", 1.2},
	{ "HN8 ", "NT ", 1.2},
	{ "HN6 ", "NT ", 1.2},
	{ "HN4 ", "NT ", 1.2},
	{ "HN3 ", "NT ", 1.2},
	{ "HN2 ", "NT ", 1.2},
	{ "HN10", "NT ", 1.2},
	{ "HN1 ", "NT ", 1.2},
	{ " C8 ", "I  ", 1.99},
	{ " C2 ", "I  ", 1.99},
	{ " C6 ", "I  ", 1.94},
	{ " C5 ", "I  ", 1.94},
	{ " C4 ", "I  ", 1.94},
	{ " N1 ", "I  ", 1.89},
	{ " N9 ", "I  ", 1.84},
	{ " N7 ", "I  ", 1.84},
	{ " N3 ", "I  ", 1.84},
	{ " O6 ", "I  ", 1.75},
	{ "HN1 ", "I  ", 1.2},
	{ " CE1", "HSP", 1.99},
	{ " CD2", "HSP", 1.99},
	{ " AE1", "HSP", 1.99},
	{ " AD2", "HSP", 1.99},
	{ " N  ", "HSP", 1.89},
	{ " AE2", "HSP", 1.89},
	{ " AD1", "HSP", 1.89},
	{ "HNE2", "HSP", 1.2},
	{ "HND1", "HSP", 1.2},
	{ "HE2 ", "HSP", 1.2},
	{ "HD1 ", "HSP", 1.2},
	{ "HAE2", "HSP", 1.2},
	{ "HAD1", "HSP", 1.2},
	{ " CE1", "HSD", 1.99},
	{ " CD2", "HSD", 1.99},
	{ " AE1", "HSD", 1.99},
	{ " AD2", "HSD", 1.99},
	{ " N  ", "HSD", 1.89},
	{ " AE2", "HSD", 1.89},
	{ " AD1", "HSD", 1.84},
	{ "HNE2", "HSD", 1.2},
	{ "HE2 ", "HSD", 1.2},
	{ "HAE2", "HSD", 1.2},
	{ " RU ", "HRU", 2.27},
	{ " N5 ", "HRU", 1.99},
	{ " N4 ", "HRU", 1.99},
	{ " N3 ", "HRU", 1.99},
	{ " N2 ", "HRU", 1.99},
	{ " N1 ", "HRU", 1.99},
	{ " CE1", "HRU", 1.99},
	{ " CD2", "HRU", 1.99},
	{ " N  ", "HRU", 1.89},
	{ "HO  ", "HOH", 1.2},
	{ "H2  ", "HOH", 1.2},
	{ "H1  ", "HOH", 1.2},
	{ " FE ", "HEM", 2.27},
	{ " CMD", "HEM", 2.09},
	{ " CMC", "HEM", 2.09},
	{ " CMB", "HEM", 2.09},
	{ " CMA", "HEM", 2.09},
	{ " CBD", "HEM", 2.04},
	{ " CBC", "HEM", 2.04},
	{ " CBB", "HEM", 2.04},
	{ " CBA", "HEM", 2.04},
	{ " CAD", "HEM", 2.04},
	{ " CAA", "HEM", 2.04},
	{ " CHD", "HEM", 1.99},
	{ " CHC", "HEM", 1.99},
	{ " CHB", "HEM", 1.99},
	{ " CHA", "HEM", 1.99},
	{ " CAC", "HEM", 1.99},
	{ " CAB", "HEM", 1.99},
	{ " CGD", "HEM", 1.94},
	{ " CGA", "HEM", 1.94},
	{ " C4D", "HEM", 1.94},
	{ " C4C", "HEM", 1.94},
	{ " C4B", "HEM", 1.94},
	{ " C4A", "HEM", 1.94},
	{ " C3D", "HEM", 1.94},
	{ " C3C", "HEM", 1.94},
	{ " C3B", "HEM", 1.94},
	{ " C3A", "HEM", 1.94},
	{ " C2D", "HEM", 1.94},
	{ " C2C", "HEM", 1.94},
	{ " C2B", "HEM", 1.94},
	{ " C2A", "HEM", 1.94},
	{ " C1D", "HEM", 1.94},
	{ " C1C", "HEM", 1.94},
	{ " C1B", "HEM", 1.94},
	{ " C1A", "HEM", 1.94},
	{ " N_D", "HEM", 1.84},
	{ " N_C", "HEM", 1.84},
	{ " N_B", "HEM", 1.84},
	{ " N_A", "HEM", 1.84},
	{ " O2D", "HEM", 1.75},
	{ " O2A", "HEM", 1.75},
	{ " O1D", "HEM", 1.75},
	{ " O1A", "HEM", 1.75},
	{ "HO  ", "H2O", 1.2},
	{ "H2  ", "H2O", 1.2},
	{ "H1  ", "H2O", 1.2},
	{ " C8 ", "G  ", 1.99},
	{ " N2 ", "G  ", 1.94},
	{ " C6 ", "G  ", 1.94},
	{ " C5 ", "G  ", 1.94},
	{ " C4 ", "G  ", 1.94},
	{ " C2 ", "G  ", 1.94},
	{ " N1 ", "G  ", 1.89},
	{ " N9 ", "G  ", 1.84},
	{ " N7 ", "G  ", 1.84},
	{ " N3 ", "G  ", 1.84},
	{ " O6 ", "G  ", 1.75},
	{ "HN2 ", "G  ", 1.2},
	{ "HN1 ", "G  ", 1.2},
	{ "DO  ", "DOD", 1.2},
	{ "D2  ", "DOD", 1.2},
	{ "D1  ", "DOD", 1.2},
	{ " OXT", "CTE", 1.75},
	{ " N  ", "CBX", 1.89},
	{ "HN  ", "CBX", 1.2},
	{ "H   ", "CBX", 1.2},
	{ " C6 ", "C  ", 1.99},
	{ " C5 ", "C  ", 1.99},
	{ " N4 ", "C  ", 1.94},
	{ " C4 ", "C  ", 1.94},
	{ " C2 ", "C  ", 1.94},
	{ " N3 ", "C  ", 1.84},
	{ " N1 ", "C  ", 1.84},
	{ " O2 ", "C  ", 1.75},
	{ "HN4 ", "C  ", 1.2},
	{ "HN3 ", "C  ", 1.2},
	{ " BR ", "BR ", 2.1},
	{ " C8 ", "A  ", 1.99},
	{ " C2 ", "A  ", 1.99},
	{ " N6 ", "A  ", 1.94},
	{ " C6 ", "A  ", 1.94},
	{ " C5 ", "A  ", 1.94},
	{ " C4 ", "A  ", 1.94},
	{ " N9 ", "A  ", 1.84},
	{ " N7 ", "A  ", 1.84},
	{ " N3 ", "A  ", 1.84},
	{ " N1 ", "A  ", 1.84},
	{ "HN6 ", "A  ", 1.2},
	{ " C5M", "5MU", 2.09},
	{ " C5A", "5MU", 2.09},
	{ " C6 ", "5MU", 1.99},
	{ " C5 ", "5MU", 1.94},
	{ " C4 ", "5MU", 1.94},
	{ " C2 ", "5MU", 1.94},
	{ " N3 ", "5MU", 1.89},
	{ " N1 ", "5MU", 1.84},
	{ " O4 ", "5MU", 1.75},
	{ " O2 ", "5MU", 1.75},
	{ "HN3 ", "5MU", 1.2},
	{ " Al6", "*  ", 2.3},
	{ " Al3", "*  ", 2.3},
	{ " Zn ", "*  ", 2.27},
	{ "Ti  ", "*  ", 2.27},
	{ " Ru ", "*  ", 2.27},
	{ " Fe ", "*  ", 2.27},
	{ " Si3", "*  ", 2.21},
	{ " P_3", "*  ", 2.14},
	{ " P  ", "*  ", 2.14},
	{ " S_31", "*  ", 2.12},
	{ " Br ", "*  ", 2.1},
	{ " C_33", "*  ", 2.09},
	{ " S_3", "*  ", 2.07},
	{ " C_R2", "*  ", 2.04},
	{ " C_32", "*  ", 2.04},
	{ " C_22", "*  ", 2.04},
	{ " C5'", "*  ", 2.04},
	{ " C2'", "*  ", 2.04},
	{ " N_33", "*  ", 1.99},
	{ " C_R1", "*  ", 1.99},
	{ " C_31", "*  ", 1.99},
	{ " C_21", "*  ", 1.99},
	{ " C_11", "*  ", 1.99},
	{ " C4'", "*  ", 1.99},
	{ " C3'", "*  ", 1.99},
	{ " C1'", "*  ", 1.99},
	{ " Cl ", "*  ", 1.95},
	{ " N_R2", "*  ", 1.94},
	{ " N_32", "*  ", 1.94},
	{ " N_22", "*  ", 1.94},
	{ " C_R", "*  ", 1.94},
	{ " C_3", "*  ", 1.94},
	{ " C_2", "*  ", 1.94},
	{ " C_1", "*  ", 1.94},
	{ " N_R1", "*  ", 1.89},
	{ " N_31", "*  ", 1.89},
	{ " N_21", "*  ", 1.89},
	{ " O_32", "*  ", 1.85},
	{ " N_R", "*  ", 1.84},
	{ " N_3", "*  ", 1.84},
	{ " N_2", "*  ", 1.84},
	{ " N_1", "*  ", 1.84},
	{ " O_R1", "*  ", 1.8},
	{ " O_31", "*  ", 1.8},
	{ " O2'", "*  ", 1.8},
	{ " O_R", "*  ", 1.75},
	{ " O_4", "*  ", 1.75},
	{ " O_3", "*  ", 1.75},
	{ " O_2", "*  ", 1.75},
	{ " OXT", "*  ", 1.75},
	{ " O5'", "*  ", 1.75},
	{ " O4'", "*  ", 1.75},
	{ " O3'", "*  ", 1.75},
	{ " O2P", "*  ", 1.75},
	{ " O1P", "*  ", 1.75},
	{ " Mn ", "*  ", 1.73},
	{ " Ca ", "*  ", 1.73},
	{ " F_ ", "*  ", 1.64},
	{ "H_  ", "*  ", 1.6},
	{ "HB3 ", "*  ", 1.6},
	{ "HB2 ", "*  ", 1.6},
	{ "HB1 ", "*  ", 1.6},
	{ "HA  ", "*  ", 1.6},
	{ "DA  ", "*  ", 1.6},
	{ " #HB", "*  ", 1.6},
	{ " #DB", "*  ", 1.6},
	{ " Na ", "*  ", 1.57},
	{ "H___", "*  ", 1.2},
	{ "HO5'", "*  ", 1.2},
	{ "HO3'", "*  ", 1.2},
	{ "HO2'", "*  ", 1.2},
	{ "HN  ", "*  ", 1.2},
	{ "H   ", "*  ", 1.2},
	{ " #H ", "*  ", 1.2},
	{ " #D ", "*  ", 1.2},
	{ " C* ", "*  ", 2},
	{ " O* ", "*  ", 1.8},
	{ " N* ", "*  ", 1.99},
	{ " S* ", "*  ", 2.12},
	{ " P* ", "*  ", 2.14}		
	};
	static int nAtomRadii = sizeof(unitedAtomRadii) / sizeof(AtomRadius);
}

