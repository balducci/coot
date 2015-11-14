
#include <iomanip>
#include <algorithm>
#include "coot-utils/residue-and-atom-specs.hh"
#include "coot-utils/coot-coord-utils.hh"
#include "coot-utils/coot-map-utils.hh"
#include "trace.hh"
#include "ligand.hh"
// #include "coot-utils/tree.hh"
#include "analysis/stats.hh"

// First we will flood the map, then look for atom pair that are 2.8
// to 4.8A apart and spin a search the density

coot::trace::trace(const clipper::Xmap<float>& xmap_in) {

   xmap = xmap_in;
   flood_atom_mask_radius = 1.5;
   flood_atom_mask_radius = 1.1; 
   rmsd_cut_off = 1.5;

   mol = 0;
   atom_selection = 0;
   n_selected_atoms = 0;

   add_atom_names_in_map_output = false; 

}

std::ostream&
coot::operator<<(std::ostream &s, scored_node_t sn) {

   s << "[" << sn.atom_idx << " " << sn.spin_score << " "
     << sn.alpha << " " << sn.reversed_flag << "]";
   return s;
} 


void
coot::trace::action() {

   minimol::molecule flood_mol = get_flood_molecule();

   mmdb::Manager *mol = flood_mol.pcmmdbmanager();

   if (mol) { 
      std::vector<std::pair<unsigned int, unsigned int> > apwd =
	 atom_pairs_within_distance(mol, 3.81, 1.0);
      std::vector<std::pair<unsigned int, scored_node_t> > scores = spin_score_pairs(apwd);

      // I abandon this method.  Longer trees have bad connectivity
      // and wipe out decent smaller polypeptides.
      // 
      // trace_graph();
      // print_interesting_trees();

      // Now start from the best peptide and try to put coordinate there

      unsigned int n_top_spin_pairs_for_tracing_starts = 10;
      unsigned int n_top_spin_pairs = 1000; // use only the top 1000 for
                                            // potential connections

      if (scores.size() < n_top_spin_pairs)
	 n_top_spin_pairs = scores.size();
      else
	 scores.resize(n_top_spin_pairs);
      
      make_connection_map(scores);
      
      for (unsigned int i=0; i<n_top_spin_pairs_for_tracing_starts; i++) {
	 std::vector<scored_node_t> start_path;
	 std::string chain_id = frag_idx_to_chain_id(i);

	 // fix start_path to be like it is in test_model()?
	 // make_fragment(scores[i], start_path, chain_id);
      }
   }
}

std::string
coot::trace::frag_idx_to_chain_id(unsigned int idx) const {

   std::string s = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

   if (idx < s.length()) {
      char c = s[idx];
      std::string ss(1,c);
      return ss;
   } else {
      return "Z";
   }
} 
      


coot::minimol::molecule
coot::trace::get_flood_molecule() const {

   coot::ligand lig;
   lig.set_cluster_size_check_off();
   lig.set_chemically_sensible_check_off();
   lig.set_sphericity_test_off();
	       
   lig.set_map_atom_mask_radius(flood_atom_mask_radius);
   lig.set_water_to_protein_distance_limits(10.0, 1.5);
   
   lig.import_map_from(xmap);
   
   lig.flood2(rmsd_cut_off);
   coot::minimol::molecule water_mol = lig.water_mol();
   // debug
   std::string output_pdb = "flood-mol.pdb";
   water_mol.write_file(output_pdb, 30.0);
   lig.output_map("find-waters-masked-flooded.map");
   
   return water_mol;
   
}

std::vector<std::pair<unsigned int, unsigned int> >
coot::trace::atom_pairs_within_distance(mmdb::Manager *mol_in,
					double trans_dist,
					double trans_dist_variation) {

   // set class members mol, atom_selection and n_sel_atoms.
   mol = mol_in;  // the peaks in the map - some of which are CAs hopefully.
   

   std::vector<std::pair<unsigned int, unsigned int> > v;
   if (mol) {
      selhnd = mol->NewSelection(); // d (in destructor)
      mol->SelectAtoms(selhnd, 0, "*",
		       mmdb::ANY_RES, // starting resno, an int
		       "*", // any insertion code
		       mmdb::ANY_RES, // ending resno
		       "*", // ending insertion code
		       "*", // any residue name
		       "*", // atom name
		       "*", // elements
		       "");

      atom_selection = 0; // member data - cleared on descruction
      n_selected_atoms = 0;
      mol->GetSelIndex(selhnd, atom_selection, n_selected_atoms);

      std::cout << "selected " << n_selected_atoms << " for distance pair check"
		<< std::endl;

      int uddHnd = mol->RegisterUDInteger(mmdb::UDR_ATOM, "index");
      if (uddHnd<0)  {
	 std::cout << " atom bonding registration failed.\n";
      } else {
	 for (int i=0; i< n_selected_atoms; i++) {
	    mmdb::Atom *at = atom_selection[i];
	    at->PutUDData(uddHnd, i); // is this needed any more?
	 }
 
	 mmdb::Contact *pscontact = NULL;
	 int n_contacts;
	 long i_contact_group = 1;
	 mmdb::mat44 my_matt;
	 for (int i=0; i<4; i++) 
	    for (int j=0; j<4; j++) 
	       my_matt[i][j] = 0.0;      
	 for (int i=0; i<4; i++) my_matt[i][i] = 1.0;
	 //
	 mmdb::realtype local_dist_max = trans_dist + trans_dist_variation;
	 mmdb::realtype local_dist_min = trans_dist - trans_dist_variation;

	 std::cout << "debug:: SeekContacts with distance limits "
		   << local_dist_min << " " << local_dist_max << std::endl;
	 
	 mol->SeekContacts(atom_selection, n_selected_atoms,
			   atom_selection, n_selected_atoms,
			   local_dist_min, local_dist_max,
			   0,        // seqDist 0 -> in same res also
			   pscontact, n_contacts,
			   0, &my_matt, i_contact_group);

	 if (n_contacts > 0) {
	    if (pscontact) {
	       for (int i=0; i<n_contacts; i++) {
		  if (pscontact[i].id1 < pscontact[i].id2) { 
		     mmdb::Atom *at_1 = atom_selection[pscontact[i].id1];
		     mmdb::Atom *at_2 = atom_selection[pscontact[i].id2];
		     int idx_1, idx_2;
		     at_1->GetUDData(uddHnd, idx_1);
		     at_2->GetUDData(uddHnd, idx_2);
		     std::pair<unsigned int, unsigned int> p(idx_1, idx_2);

		     if (1) {  // debug
			// check that saa atoms are at the same place
			// as sel_atoms atoms.
			clipper::Coord_orth sel_atom_1_co = co(at_1);
			clipper::Coord_orth sel_atom_2_co = co(at_2);
		     } 
			
		     v.push_back(p);
		  }
	       }
	    }
	 }

	 std::cout << "found " << n_contacts << " distance pairs " << std::endl;
	 std::cout << "made  " << v.size() << " distance pairs " << std::endl;
      }
   }
   return v;

}
   

// return sorted scores
// 
std::vector<std::pair<unsigned int, coot::scored_node_t> > 
coot::trace::spin_score_pairs(const std::vector<std::pair<unsigned int, unsigned int> > &apwd) {

   // apwd : atom (index) pairs within distance

   unsigned int n_top = 1000; // top-scoring spin-score pairs for tracing
   
   std::vector<std::pair<unsigned int, scored_node_t> > scores(apwd.size()*2);

#pragma omp parallel for shared(scores)   
   for (unsigned int i=0; i<apwd.size(); i++) {
      const unsigned &at_idx_1 = apwd[i].first;
      const unsigned &at_idx_2 = apwd[i].second;

      scores[2*i ]  = spin_score(at_idx_1, at_idx_2);
      scores[2*i+1] = spin_score(at_idx_2, at_idx_1);

      // debug
      // 
      output_spin_score(scores[2*i  ], at_idx_1, at_idx_2);
      output_spin_score(scores[2*i+1], at_idx_2, at_idx_1);
   }

   if (scores.size() > n_top) {
      std::sort(scores.begin(), scores.end(), scored_node_t::sort_pair_scores);
      scores.resize(n_top);
   } else {
      n_top = scores.size();
   }

   // debugging
   // 
   std::cout << "---- sorted scores ----- " << n_top <<  std::endl;
   for (unsigned int i=0; i<n_top; i++) {
      const std::string &at_name_1 = index_to_name(scores[i].first);
      const std::string &at_name_2 = index_to_name(scores[i].second.atom_idx);
      int res_no_1 = atom_selection[scores[i].first          ]->GetSeqNum();
      int res_no_2 = atom_selection[scores[i].second.atom_idx]->GetSeqNum();
      
      std::cout << "sorted spin scores " << " " << std::setw(4) << scores[i].first << " "
		<< " to " << std::setw(4) << scores[i].second.atom_idx << " "
		<< at_name_1 << " " << std::setw(3) << res_no_1 << " " 
		<< at_name_2 << " " << std::setw(3) << res_no_2 << " " 
		<< index_to_pos(scores[i].first).format() << " "
		<< index_to_pos(scores[i].second.atom_idx).format() << " "
		<< scores[i].second.spin_score << std::endl;
   }
   return scores;
   
}

void
coot::trace::output_spin_score(const std::pair<unsigned int, scored_node_t> &score,
			       unsigned int at_idx_1,
			       unsigned int at_idx_2) const {

   // debugging output
   //
   //
   bool ca_1_flag = false;
   bool ca_2_flag = false;
   bool consecutive_flag = false;
   // 
   // indexing looks OK.
   if (index_to_name(at_idx_1) == " CA ") ca_1_flag = true;
   if (index_to_name(at_idx_2) == " CA ") ca_2_flag = true;
   // 
   if (ca_1_flag && ca_2_flag) {
      int resno_delta =
	 atom_selection[at_idx_2]->GetSeqNum() -
	 atom_selection[at_idx_1]->GetSeqNum();
      if (resno_delta == 1)
	 consecutive_flag = true;
   } 
   // 
   if (true) { // debugging
      const std::string &at_name_1 = index_to_name(at_idx_1);
      const std::string &at_name_2 = index_to_name(at_idx_2);
      int res_no_1 = atom_selection[at_idx_1]->GetSeqNum();
      int res_no_2 = atom_selection[at_idx_2]->GetSeqNum();

      clipper::Coord_orth co_1 = index_to_pos(at_idx_1);
      clipper::Coord_orth co_2 = index_to_pos(at_idx_2);
      double dist = clipper::Coord_orth::length(co_1, co_2);

      std::cout << "spin-scores " << at_idx_1 << " ";
      if (add_atom_names_in_map_output)
	 std::cout << at_name_1 << " " << res_no_1 << " "
		   << at_idx_2 << " ";
      if (add_atom_names_in_map_output)
	 std::cout << at_name_2 << " " << res_no_2 << " ";
      std::cout // << " dist: " << dist << "    "
	 << " score: " << score.second.spin_score << "  " 
	 << co_1.x() << " " << co_1.y() << " " << co_1.z() << "    "
	 << co_2.x() << " " << co_2.y() << " " << co_2.z() << "    "
	 << ca_1_flag << " " << ca_2_flag << " " << consecutive_flag
	 << std::endl;
   }


}

void
coot::trace::make_connection_map(const std::vector<std::pair<unsigned int, scored_node_t> > &scores) {
   
   double good_enough_score = 0.5; // overwritten

   if (scores.size() > 0)
      good_enough_score = scores.back().second.spin_score;
   std::cout << "DEBUG:: set good_enough_score to make connection map: "
	     << good_enough_score << std::endl;

   // add them to the connection map:
   // 
   std::vector<scored_node_t>::const_iterator it;

   for (unsigned int i=0; i<scores.size(); i++) {
      if (scores[i].second.spin_score  > good_enough_score) {
	 it = std::find(fwd_connection_map[scores[i].first].begin(),
			fwd_connection_map[scores[i].first].end(),
			scores[i].second);
	 if (it == fwd_connection_map[scores[i].first].end())
	    fwd_connection_map[scores[i].first].push_back(scores[i].second);
      }
   }

   // make the reverse connection map from that: "a node was (fwd)
   // connected by these nodes"
   //
   std::map<unsigned int, std::vector<scored_node_t> >::const_iterator itm;
   for (itm = fwd_connection_map.begin(); itm != fwd_connection_map.end(); itm++) {
      const std::vector<scored_node_t> &v = itm->second;
      for (it=v.begin(); it!=v.end(); it++) {
	 scored_node_t reverse_node(itm->first, it->spin_score, it->alpha, true);
	 bck_connection_map[it->atom_idx].push_back(reverse_node);
      }
   } 


   // debug::
   // 
   for (itm=fwd_connection_map.begin(); itm!=fwd_connection_map.end(); itm++) {
      int res_no = atom_selection[itm->first]->GetSeqNum();
      std::cout << "fwd-map " << itm->first <<  "  ["
		<< std::setw(2) << itm->second.size() << "] ";
      if (add_atom_names_in_map_output)
	 std::cout << atom_selection[itm->first]->name << " " << res_no << " ";
      std::cout << index_to_pos(itm->first).format() << "  ";
      for (unsigned int jj=0; jj<itm->second.size(); jj++) {
	 int res_no_n = atom_selection[itm->second[jj].atom_idx]->GetSeqNum();
	 std::cout << "  " << itm->second[jj].atom_idx << " ";
	 if (add_atom_names_in_map_output)
	    std::cout << atom_selection[itm->second[jj].atom_idx]->name << " "
		      << res_no_n << " ";
      }
      std::cout << std::endl;
   }

   for (itm=bck_connection_map.begin(); itm!=bck_connection_map.end(); itm++) {
      int res_no = atom_selection[itm->first]->GetSeqNum();
      std::cout << "bck-map " << itm->first <<  "  ["
		<< std::setw(2) << itm->second.size() << "] ";
      if (add_atom_names_in_map_output)
	 std::cout << atom_selection[itm->first]->name << " " << res_no << " ";
      std::cout << index_to_pos(itm->first).format() << "  ";
      for (unsigned int jj=0; jj<itm->second.size(); jj++) {
	 int res_no_n = atom_selection[itm->second[jj].atom_idx]->GetSeqNum();
	 std::cout << "  " << itm->second[jj].atom_idx << " ";
	 if (add_atom_names_in_map_output)
	    std::cout << atom_selection[itm->second[jj].atom_idx]->name << " "
		      << res_no_n << " ";
      }
      std::cout << std::endl;
   }
   
}

coot::minimol::fragment
coot::trace::make_fragment(std::pair<unsigned int, coot::scored_node_t> scored_node,
			   int res_no_base,
			   std::string chain_id) {

   std::cout << "make_fragment() called with  node: " << scored_node.first
	     << " scored_node.idx: " << scored_node.second.atom_idx << std::endl;
   
   clipper::Coord_orth pos_1 = index_to_pos(scored_node.first);
   clipper::Coord_orth pos_2 = index_to_pos(scored_node.second.atom_idx);

   if (scored_node.second.reversed_flag)
      std::swap(pos_1, pos_2);

   // as in spin_score():
   // 
   clipper::Coord_orth arb(0,0,1);
   clipper::Coord_orth diff_p(pos_2 - pos_1);
   clipper::Coord_orth diff_p_unit(diff_p.unit());

   clipper::Coord_orth perp(clipper::Coord_orth::cross(arb, diff_p));
   clipper::Coord_orth perp_unit(perp.unit());

   clipper::Coord_orth double_perp(clipper::Coord_orth::cross(diff_p, perp));
   clipper::Coord_orth double_perp_unit(double_perp.unit());

   double diff_p_len = sqrt(diff_p.lengthsq());

   double alpha = scored_node.second.alpha;

   // << " using alpha " << alpha << std::endl;

   double along_CA_CA_pt_O = 1.73; // the C is lower down than the O.
   double along_CA_CA_pt_C = 1.46;
   double along_CA_CA_pt_N = 2.44;

   double ideal_peptide_length = 3.81;

   // we don't want the peptide to be scrunched up on one side of a
   // "long" peptide... let the atom positions expand along a long peptide. 
   
   double f_ca_ca_o = along_CA_CA_pt_O * diff_p_len/ideal_peptide_length;
   double f_ca_ca_c = along_CA_CA_pt_C * diff_p_len/ideal_peptide_length;
   double f_ca_ca_n = along_CA_CA_pt_N * diff_p_len/ideal_peptide_length;

   clipper::Coord_orth rel_line_pt_O(diff_p_unit * f_ca_ca_o + perp_unit * 1.66);
   clipper::Coord_orth rel_line_pt_C(diff_p_unit * f_ca_ca_c + perp_unit * 0.48);
   clipper::Coord_orth rel_line_pt_N(diff_p_unit * f_ca_ca_n - perp_unit * 0.47);

   clipper::Coord_orth p_N = util::rotate_round_vector(diff_p_unit,
						       pos_1 + rel_line_pt_N,
						       pos_1, alpha);

   clipper::Coord_orth p_O = util::rotate_round_vector(diff_p_unit,
						       pos_1 + rel_line_pt_O,
						       pos_1, alpha);
   
   clipper::Coord_orth p_C = util::rotate_round_vector(diff_p_unit,
						       pos_1 + rel_line_pt_C,
						       pos_1, alpha);

   minimol::residue r1(res_no_base,   "ALA");
   minimol::residue r2(res_no_base+1, "ALA");
   minimol::atom at_O   (" O  ", "O", p_O,   "", 10);
   minimol::atom at_C   (" C  ", "C", p_C,   "", 10);
   minimol::atom at_N   (" N  ", "N", p_N,   "", 10);
   minimol::atom at_CA_1(" CA ", "C", pos_1, "", 10);
   minimol::atom at_CA_2(" CA ", "C", pos_2, "", 10);

   r1.addatom(at_CA_1);
   r1.addatom(at_C);
   r1.addatom(at_O);
   
   r2.addatom(at_N);
   r2.addatom(at_CA_2);

   minimol::fragment f(chain_id, false); // use non-expanding constructor
   f.addresidue(r1, false);
   f.addresidue(r2, false);

   if (false) { // debug
      // is f in a good state now?
      minimol::molecule m_debug(f);
      std::string  node_string_1 = util::int_to_string(scored_node.first);
      std::string  node_string_2 = util::int_to_string(scored_node.second.atom_idx);
      std::string fn_ = "just-a-pair-" + node_string_1 + "-" + node_string_2 + ".pdb";
      m_debug.write_file(fn_, 10);
   }
   
   return f;
}

bool
coot::trace::nice_fit(const minimol::fragment &f) const {

   return true;
}

bool
coot::trace::nice_fit(const minimol::residue &r1, const minimol::residue &r2) const {

   double s = get_fit_score(r1, r2);
   return true;

}

double
coot::trace::get_fit_score(const minimol::residue &r1, const minimol::residue &r2) const {

   return 0;
}

coot::minimol::fragment
coot::trace::merge_fragments(const coot::minimol::fragment &f1,
			     const coot::minimol::fragment &f2) const {

   bool debug = false;
   minimol::fragment merged;

   if (debug) { 
      std::cout << "---- in merge_fragments() f1: " << f1 << std::endl;
      std::cout << "---- in merge_fragments() f2: " << f2 << std::endl;

      for (int ires=f1.min_res_no(); ires<=f1.max_residue_number(); ires++)
	 for (unsigned int iat=0; iat<f1[ires].atoms.size(); iat++)
	    std::cout << "   mol1: res: " << ires << " atom " << f1[ires].atoms[iat]
		      << std::endl;
      for (int ires=f2.min_res_no(); ires<=f2.max_residue_number(); ires++)
	 for (unsigned int iat=0; iat<f2[ires].atoms.size(); iat++)
	    std::cout << "   mol2: res: " << ires << " atom " << f2[ires].atoms[iat]
		      << std::endl;
   }

   merged = f1;
   for (int ires=f2.min_res_no(); ires<=f2.max_residue_number(); ires++) {

      // Does this residue exist in merged already?
      // If so, the we want to replace or add to the atoms that are already there
      // If not, add this residue

      bool have_residue = true;
      try {
	 const minimol::residue &r = merged[ires];
      }
      catch (const std::runtime_error &rte) {
	 have_residue = false;
	 std::cout << "caught no residue for " << ires << std::endl;
      } 

      if (! have_residue) {
	 minimol::residue r = f2[ires];
	 r.seqnum = ires;
	 std::cout << "merge_residues() calling addresidue() for ires " << ires
		   << " and name " << r.name << std::endl;
	 merged.addresidue(r, false);
      } else {

	 if (f2[ires].atoms.size()) { 
	    // atoms to be added or replace existing atoms

	    if (debug)
	       std::cout << "atoms to be added or replace existing atoms for ires "
			 << ires << " and internal seqnum " << f2[ires].seqnum
			 << std::endl;

	    // OK, residue merges[ires] does exist but it's empty

	    merged[ires].seqnum = ires;
	    merged[ires].name = f2[ires].name;

	    for (unsigned int jat=0; jat<f2[ires].atoms.size(); jat++) {

	       bool found = false;
	       // current atoms
	       for (unsigned int iat=0; iat<merged[ires].atoms.size(); iat++) {
	       
		  if (merged[ires][iat].name == f2[ires][jat].name) {
		     merged[ires][iat] = f2[ires][jat];
		     if (debug)
			std::cout << "replaced atom in ires  " << ires << ": "
				  << merged[ires][iat] << std::endl;
		     found = true;
		     break;
		  }
	       }
	       if (! found) {
		  if (debug)
		     std::cout << "add atom " << f2[ires][jat] << std::endl;
		  merged[ires].addatom(f2[ires][jat]);
	       }
	    }
	 }
      }
   }
   return merged;
} 


void
coot::trace::trace_graph() {

   // fill interesting_trees

   std::map<unsigned int, std::vector<scored_node_t> >::const_iterator it;

   std::cout << "in ---- trace_graph() --- tr is of size "
	     << fwd_connection_map.size() << std::endl;

   for(it=fwd_connection_map.begin(); it!=fwd_connection_map.end(); it++) {

      std::vector<scored_node_t> path;
      unsigned int iv=it->first;

      // check here if iv is a leaf

      if (it->second.size() == 1) { 
	 // std::cout << "---- trace start with node " << iv << std::endl;
	 scored_node_t leaf(iv, 0, 0);
	 next_vertex(path, 0, leaf);
      }
   }
   sort_filter_interesting_trees();
}

void
coot::trace::sort_filter_interesting_trees() {

   std::sort(interesting_trees.begin(),
	     interesting_trees.end(),
	     sort_trees_by_length);

   if (false)  // debuggging. 
      for (unsigned int itree=0; itree<interesting_trees.size(); itree++)
	 std::cout << "   itree " << itree << " " << interesting_trees[itree].size()
		   << std::endl;
}



std::pair<unsigned int, coot::scored_node_t> 
coot::trace::spin_score(unsigned int idx_1, unsigned int idx_2) const {

   double score = 0;

   mmdb::Atom *at_1 = atom_selection[idx_1]; 
   mmdb::Atom *at_2 = atom_selection[idx_2];

   const clipper::Coord_orth pos_1 = index_to_pos(idx_1);

   const clipper::Coord_orth pos_2 = index_to_pos(idx_2);

   // draw a line between pos_1 and pos_2
   
   // find a point A that is 1.56 down the stick and 1.57 away from the line
   // find a point B that is 1.9  down the stick and 1.91 away from the line
   // find a point C that is 1.9  down the stick and 1.91 away from the line
   //      in the opposite direction to B.

   // spin this points A, B and C around the pos_1 - pos_2 line and the score is
   // rho(A) - rho(B) - rho(C)

   clipper::Coord_orth arb(0,0,1);
   clipper::Coord_orth diff_p(pos_2 - pos_1);
   clipper::Coord_orth diff_p_unit(diff_p.unit());

   clipper::Coord_orth perp(clipper::Coord_orth::cross(arb, diff_p));
   clipper::Coord_orth perp_unit(perp.unit());

   clipper::Coord_orth double_perp(clipper::Coord_orth::cross(diff_p, perp));
   clipper::Coord_orth double_perp_unit(double_perp.unit());

   double along_CA_CA_pt_O = 1.53; // the C is lower down than the O.
   double along_CA_CA_pt_for_perp = 2.33;
                                         
                                         
   double along_CA_CA_pt_N = 2.5;
   double ideal_peptide_length = 3.81;

   // we don't want the peptide to be scrunched up on one side of a
   // "long" peptide... let the atom positions expand along a long peptide. 
   double diff_p_len = sqrt(diff_p.lengthsq());
   double f_ca_ca_o = along_CA_CA_pt_O * diff_p_len/ideal_peptide_length;
   // double f_ca_ca_c = along_CA_CA_pt_C * diff_p_len/ideal_peptide_length;
   double f_ca_ca_n = along_CA_CA_pt_N * diff_p_len/ideal_peptide_length;
   double f_ca_ca_pt_for_perp = along_CA_CA_pt_for_perp * diff_p_len/ideal_peptide_length;

   // clipper::Coord_orth rel_line_pt_C(diff_p_unit * f_ca_ca_c + perp_unit * 0.7);
   // clipper::Coord_orth rel_line_pt_N(diff_p_unit * f_ca_ca_n - perp_unit * 0.5);

   // there is good density 1.9A away from the mid-line in the direction of the CO
   // (at the O). 
   // there is little density 3.7A away from the mid-line in the direction of the CO
   clipper::Coord_orth rel_line_pt_O(      diff_p_unit * f_ca_ca_o + perp_unit * 1.89);
   clipper::Coord_orth rel_line_pt_O_low(  diff_p_unit * f_ca_ca_o + perp_unit * 3.9);
   clipper::Coord_orth rel_line_pt_CO_anti(diff_p_unit * f_ca_ca_o - perp_unit * 0.5);
   clipper::Coord_orth rel_line_pt_N(      diff_p_unit * f_ca_ca_n - perp_unit * 0.3);
   clipper::Coord_orth rel_line_pt_perp1(diff_p_unit * f_ca_ca_pt_for_perp  + double_perp_unit * 1.85);
   clipper::Coord_orth rel_line_pt_perp2(diff_p_unit * f_ca_ca_pt_for_perp  - double_perp_unit * 1.72);
   

   float rho_at_1 = util::density_at_point(xmap, pos_1);
   float rho_at_2 = util::density_at_point(xmap, pos_2);

   // the mid-point between CAs should have density too.
   clipper::Coord_orth pt_mid(pos_1 * 0.50 + pos_2 * 0.5);
   float rho_mid = util::density_at_point(xmap, pt_mid);

   int n_steps = 36;
   float best_score = -999;

   float rho_CO_best = -999; // for testing scoring
   float rho_CO_low_best = -999;
   float rho_2_best  = -999;
   float rho_3_best  = -999;
   double alpha_best = -1; // negative indicates this was not set - which is a bad thing.

   // these can be optimized with machine learning?
   float scale_CO       =  0.5;
   float scale_CO_low   = -0.6;
   float scale_CO_anti  = -0.1;
   float scale_perp     = -0.7;
   float scale_mid      =  1.6;
   float scale_non_line =  1.0;
   float scale_N        = -0.0;

   // unsigned int idx_test_1 = 1228;
   // unsigned int idx_test_2 = 1238;
   // unsigned int idx_test_1 = 101;
   // unsigned int idx_test_2 = 109;
   
   unsigned int idx_test_1 = 131;
   unsigned int idx_test_2 = 139;
   
   for (int i=0; i< int(n_steps); i++) { 
      double alpha = 2 * M_PI * double(i)/double(n_steps);

      // direction position orig-shift angle
      // 
      clipper::Coord_orth p_CO = util::rotate_round_vector(diff_p_unit,
							  pos_1 + rel_line_pt_O,
							  pos_1, alpha);
      
      clipper::Coord_orth p_CO_low = util::rotate_round_vector(diff_p_unit,
							  pos_1 + rel_line_pt_O_low,
							  pos_1, alpha);
      
      clipper::Coord_orth p_CO_anti = util::rotate_round_vector(diff_p_unit,
							  pos_1 + rel_line_pt_CO_anti,
							  pos_1, alpha);
      
      clipper::Coord_orth p_N = util::rotate_round_vector(diff_p_unit,
							  pos_1 + rel_line_pt_N,
							  pos_1, alpha);
      
      clipper::Coord_orth p_2 = util::rotate_round_vector(diff_p_unit,
							  pos_1 + rel_line_pt_perp1,
							  pos_1, alpha);
      clipper::Coord_orth p_3 = util::rotate_round_vector(diff_p_unit,
							  pos_1 + rel_line_pt_perp2,
							  pos_1, alpha);
      
      float rho_CO      = util::density_at_point(xmap, p_CO);
      float rho_CO_low  = util::density_at_point(xmap, p_CO_low);
      float rho_CO_anti = util::density_at_point(xmap, p_CO_anti);
      float rho_N       = util::density_at_point(xmap, p_N);
      float rho_2       = util::density_at_point(xmap, p_2);
      float rho_3       = util::density_at_point(xmap, p_3);

      float this_score =
	 scale_CO      * rho_CO      +
	 scale_CO_low  * rho_CO_low  + 
	 scale_CO_anti * rho_CO_anti +
	 scale_N       * rho_N       +
	 scale_perp    * rho_2       +
	 scale_perp    * rho_3;

      if (idx_1 == idx_test_1 && idx_2 == idx_test_2) {
	 std::cout << "debug_pos:: CO     " << p_CO.x() << " " << p_CO.y() << " " << p_CO.z()
		   << " " << rho_CO << std::endl;
	 std::cout << "debug_pos:: CO_low " << p_CO_low.x() << " " << p_CO_low.y()
		   << " " << p_CO_low.z() << " " << rho_CO_low << std::endl;
	 std::cout << "debug_pos:: perp-1 " << p_2.x() << " " << p_2.y() << " " << p_2.z()
		   << " " << rho_2 << std::endl;
	 std::cout << "debug_pos:: perp-2 " << p_3.x() << " " << p_3.y() << " " << p_3.z()
		   << " " << rho_3 << std::endl;
	 std::cout << "debug_pos:: N " << p_N.x() << " " << p_N.y() << " " << p_N.z()
		   << " " << rho_N << std::endl;
      } 
      
      if (this_score > best_score) { 
	 best_score = this_score;
	 rho_CO_best = rho_CO;
	 rho_CO_low_best = rho_CO_low;
	 rho_2_best = rho_2;
	 rho_3_best = rho_3;
	 alpha_best = alpha;
      }
   }

   float non_line_equal_density_penalty_1 = rho_at_1 + rho_at_2 - 2 * rho_mid;
   float non_line_equal_density_penalty = 
      - non_line_equal_density_penalty_1 * non_line_equal_density_penalty_1;

   best_score += scale_mid * rho_mid;
   best_score += scale_non_line * non_line_equal_density_penalty;

   if (idx_1 == idx_test_1 && idx_2 == idx_test_2) {
      std::cout << "debug score-parts: " << rho_at_1 << " " << rho_at_2 << " mid: "
		<< rho_mid << " CO " << rho_CO_best
		<< " perp-1 " << rho_2_best << " perp-2 " << rho_3_best
		<< " = " << best_score << std::endl;
   }

   scored_node_t best_node(idx_2, best_score, alpha_best);
   return std::pair<unsigned int, scored_node_t> (idx_1, best_node);
}

void
coot::trace::print_interesting_trees() const {

   for (unsigned int itree=0; itree<interesting_trees.size(); itree++) {
      std::cout << "interesting tree " << itree << ": ";
      for (unsigned int j=0; j<interesting_trees[itree].size(); j++) { 
	 const unsigned int &idx = interesting_trees[itree][j].atom_idx;
	 int res_no = atom_selection[idx]->GetSeqNum();
	 std::cout << "  " << idx;
	 if (add_atom_names_in_map_output)
	    std::cout << " (" << atom_selection[idx]->name << " " << res_no << ")";
      }
      std::cout << std::endl;
   }

   minimol::molecule m;
   int offset = 0;

   if (false) { 
      for (unsigned int itree=0; itree<interesting_trees.size(); itree++) {
	 minimol::fragment f(util::int_to_string(itree));
	 for (unsigned int j=0; j<interesting_trees[itree].size(); j++) {
	    minimol::residue r(j + offset, "ALA");
	    minimol::atom at(" CA ", "C", index_to_pos(interesting_trees[itree][j].atom_idx), "", 10);
	    r.addatom(at);
	    f.addresidue(r, false);
	 }
	 // offset += interesting_trees[itree].size();
	 m.fragments.push_back(f);
      }
      std::cout << "writing interesting.pdb " << std::endl;
      m.write_file("interesting.pdb", 20);
   }
} 


void
coot::trace::test_model(mmdb::Manager *mol) {

   add_atom_names_in_map_output = true; // makes sense only when use use template pdb
                                        // for atom seeding
   
   std::vector<std::pair<unsigned int, unsigned int> > apwd =
      atom_pairs_within_distance(mol, 3.81, 1);

   std::vector<std::pair<unsigned int, scored_node_t> > scores = spin_score_pairs(apwd);

   unsigned int n_top = 1000; // top-scoring spin-score pairs for tracing
   
   std::sort(scores.begin(), scores.end(), scored_node_t::sort_pair_scores);
   if (scores.size() > n_top) {
      scores.resize(n_top);
   } else {
      n_top = scores.size();
   }
   
   make_connection_map(scores);
   

   int n_top_fragments = 2;
   // 

#pragma omp parallel for // currently we write out the fragments, not store them
   
   for (int i=0; i<n_top_fragments; i++) {

      // int tid = omp_get_thread_num();
      // std::cout << "OMP::: " << tid << std::endl;

      std::vector<scored_node_t> start_path;
      scored_node_t dummy_first;
      dummy_first.atom_idx = scores[i].first;
      start_path.push_back(dummy_first);
      
      std::string chain_id = frag_idx_to_chain_id(i);
      std::cout << "----------- test_model() starting point number  " << i
		<< " chain_id " << chain_id << std::endl;

      int res_no_base = scores[i].first;

      res_no_base = i;

      // actually, we *have* the first spin pair but it gets thrown
      // away and "rediscovered" when build_2_choose_1() does just
      // that (and hopefully picks the idx scores[i].second.atom_ids).
      // 

      dir_t dir = FORWARDS;
      follow_fragment(scores[i].first, start_path, res_no_base, chain_id, dir);
   }
}

void
coot::trace::follow_fragment(unsigned int atom_idx, const std::vector<scored_node_t> &path_in,
			     int res_no_base,
			     const std::string &chain_id,
			     dir_t dir) {


   std::vector<scored_node_t> start_path = path_in;
   minimol::fragment running_fragment;
   unsigned int atom_idx_in = atom_idx;
   
   bool status = true;
   
   while (status) {
      // this function should return the atom_idx of the node that
      // is a best neighbour of atom_idx
      //

      int res_no = res_no_base + start_path.size();
      if (dir == BACKWARDS)
	 res_no = res_no_base - start_path.size();
      
      std::pair<bool, scored_node_t> sn =
	 build_2_choose_1(atom_idx, start_path, res_no, chain_id, dir);
      status = sn.first;
      std::cout << "From node " << atom_idx << " got next: status: "
		<< sn.first << " and index: " << sn.second.atom_idx << std::endl;
      if (sn.first) {
	 std::pair<unsigned int, scored_node_t> p(atom_idx, sn.second);

	 // f needs to get the "right" residue numbers.
	 minimol::fragment f = make_fragment(p, res_no, chain_id);
	 
	 running_fragment = merge_fragments(running_fragment, f);

	 // setup for next round
	 atom_idx = sn.second.atom_idx;
	 start_path.push_back(sn.second);
	 
      } else {
	 // what's in running fragment?

	 minimol::molecule m_debug(running_fragment);
	 std::string  node_string = util::int_to_string(res_no_base);
	 std::string fn_ = "running-frag-node-" + node_string + ".pdb";
	 std::cout << ":::::: fragment output: " << fn_ << std::endl;
	 m_debug.write_file(fn_, 10);
	 
      }
   }
}


std::pair<bool, coot::scored_node_t>
coot::trace::build_2_choose_1(unsigned int atom_idx,
			      const std::vector<scored_node_t> &start_path,
			      int res_no_base,
			      const std::string &chain_id,
			      dir_t dir) {

   bool status = false;
   
   // this functions checks that the neighbors don't have to same atom_idx as
   // those in path 
   // 
   std::vector<scored_node_t> neighbs_1 = get_neighbours_of_vertex_excluding_path(atom_idx, start_path, dir);

   std::cout << "INFO:: node-lev-1 " << atom_idx << " has "
	     << neighbs_1.size() << " neighbours:" << std::endl;
   for (unsigned int j=0; j<neighbs_1.size(); j++)
      std::cout << "   " << neighbs_1[j].atom_idx << std::endl;
   
   std::vector<indexed_frag_t> frag_store;

   for (unsigned int j=0; j<neighbs_1.size(); j++) { 
      std::pair<unsigned int, scored_node_t> p_1(atom_idx, neighbs_1[j]);
      minimol::fragment f_1 = make_fragment(p_1, res_no_base, chain_id);

      if (nice_fit(f_1)) {

	 std::vector<scored_node_t> path = start_path;
	 path.push_back(neighbs_1[j]);

	 std::vector<scored_node_t> neighbs_2 =
	    get_neighbours_of_vertex_excluding_path(neighbs_1[j].atom_idx, path, dir);

	 std::cout << "INFO:: node-lev-2 " << neighbs_1[j].atom_idx << " has "
		   << neighbs_2.size() << " neighbours:" << std::endl;
	 for (unsigned int jj=0; jj<neighbs_2.size(); jj++)
	    std::cout << "   " << neighbs_2[jj].atom_idx << std::endl;

	 for (unsigned int jj=0; jj<neighbs_2.size(); jj++) {

	    double ang = path_candidate_angle(path, neighbs_2[jj].atom_idx);

	    if (false) 
	       std::cout << "   angle from " << path[1].atom_idx << " " << path[2].atom_idx
			 << " " << neighbs_2[jj].atom_idx << "  " << ang
			 << " (should be more than " << 0.8 * M_PI * 0.5 << ")" << std::endl;
		     
	    if (ang > 0.8 * M_PI * 0.5) {

	       std::pair<unsigned int, scored_node_t> p_2(neighbs_1[j].atom_idx, neighbs_2[jj]);
	       minimol::fragment f_2 = make_fragment(p_2, res_no_base, chain_id);

	       // Now store f1, f2 indexed on neighbs_1[j].atom_idx, neighbs_2[jj].atom_idx
	       // so that we can pick choose the best fit and continue the fragment starting
	       // from neighbs_1[j]

	       indexed_frag_t idx_frag(neighbs_1[j], neighbs_2[jj], f_1, f_2);

	       frag_store.push_back(idx_frag);

	    }
	 }
      }
   }

   // now choose the best frag in frag_store

   std::cout << "I have to choose the best of " << frag_store.size() << " frags " << std::endl;

   double best_score = -9999;
   scored_node_t best_scored_node;
   
   if (frag_store.size() > 0) {
      for (unsigned int i=0; i<frag_store.size(); i++) { 
	 if (frag_store[i].get_score() > best_score) {
	    best_score       = frag_store[i].get_score();
	    best_scored_node = frag_store[i].node_1;
	 }
      }
   }

   if (best_score > 3) { // hack value
      status = true;
   } 

   return std::pair<bool, scored_node_t> (status, best_scored_node);

} 


void
coot::trace::print_tree(const std::vector<unsigned int> &path) const {

   std::cout << "path: ";
   for (unsigned int i=0; i<path.size(); i++) {
      int res_no = atom_selection[path[i]]->GetSeqNum();
      std::cout << "  " << path[i] << " (" << index_to_name(path[i]) << " "
		<< res_no << ")";
   }
   std::cout << std::endl;

   if (false) 
      for (unsigned int i=0; i<path.size(); i++) {
	 const clipper::Coord_orth &pt = index_to_pos(path[i]);
	 std::cout << "long path " << i
		   << " " << pt.x()
		   << " " << pt.y()
		   << " " << pt.z()
		   << std::endl;
   }
}



double
coot::trace::path_candidate_angle(const std::vector<scored_node_t> &path,
				  unsigned int candidate_vertex) const {

   unsigned int l = path.size();
   const clipper::Coord_orth &pt_1 = index_to_pos(candidate_vertex);
   const clipper::Coord_orth &pt_2 = index_to_pos(path[l-1].atom_idx);
   const clipper::Coord_orth &pt_3 = index_to_pos(path[l-2].atom_idx);
   double angle = clipper::Coord_orth::angle(pt_1, pt_2, pt_3);

   return angle;
} 

void
coot::trace::add_tree_maybe(const std::vector<scored_node_t> &path) {


   // add this path if there is not already a path that is longer that
   // contains at least n atoms of the same path points.

   bool add_this = true;
   unsigned int n_match_crit = 2;
   unsigned int n_match_for_replace = 2; // at least this number of matches to replace an existing tree

   for (unsigned int itree=0; itree<interesting_trees.size(); itree++) {
      unsigned int n_match = 0;

      // do I already have something that's longer than path and
      // similar to path already in interesting_trees? (if so, set
      // add_this to false).

      // the rejection should take account of the length of the
      // overlapping paths.
      //
      // if they are in the same direction and the length of the paths
      // is much longer than the overlaps, then they should be merged.
      // How can that happen though? The path explorer (next_vertex)
      // should find the merged tree.
      
      if (path.size() < interesting_trees[itree].size()) {
	 for (unsigned int i=0; i<interesting_trees[itree].size(); i++) {
	    for (unsigned int j=0; j<path.size(); j++) {
	       if (path[j] == interesting_trees[itree][i]) {
		  n_match ++;
	       }
	    }

	    if (n_match >= n_match_crit) {
	       add_this = false;
	       break;
	    }
	 }
      }

      
   }

   if (0) { 
      std::cout << "add status " << add_this << " for tree of length " << path.size()
		<< " because " << interesting_trees.size() << " trees of lengths ";
      for (unsigned int ii=0; ii<interesting_trees.size(); ii++) { 
	 std::cout << "  " << interesting_trees[ii].size();
      }
      std::cout << std::endl;
   }

   if (add_this) {
      interesting_trees.erase(std::remove_if(interesting_trees.begin(),
					     interesting_trees.end(),
					     trace_path_eraser(path, n_match_for_replace)),
			      interesting_trees.end());
      interesting_trees.push_back(path);
   } 
}

