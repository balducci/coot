#ifndef READ_MOLECULE_HH
#define READ_MOLECULE_HH

#ifdef USE_PYTHON
#include "Python.h"
#endif

#include <string>

/*! \brief read coordinates from filename with option to not recentre.

   set recentre_on_read_pdb_flag to 0 if you don't want the view to
   recentre on the new coordinates. */
int handle_read_draw_molecule_with_recentre(const std::string &filename,
					    int recentre_on_read_pdb_flag);

/*! \brief read coordinates from filename and recentre the new
  molecule at the screen rotation centre. */
int handle_read_draw_molecule_and_move_molecule_here(const std::string &filename);

/*! \brief read coordinates from filename */
int read_pdb(const std::string &filename);

/* pass back the newly created molecule number */
/*! \brief a synonym for read-pdb.  Read the coordinates from
  filename (can be pdb, cif or shelx format)  */
int handle_read_draw_molecule(const std::string &filename);


//! set (or unset) GEMMI as the molecule parser. Currently by passing an int.
void set_use_gemmi_as_model_molecule_parser(int state);


/* \} */

/*  ----------------------------------------------------------------------- */
/*                  SHELX stuff                                             */
/*  ----------------------------------------------------------------------- */
/* section SHELXL Functions */
/*! \name SHELXL Functions */
/* \{ */
/*! \brief read a SHELXL .ins file */
int read_shelx_ins_file(const std::string &filename, short int recentre_flag);
/*! \brief write a SHELXL .ins file for molecule number imol */
int write_shelx_ins_file(int imol, const char *filename);
/* for shelx fcf file that needs to be filtered: */
int handle_shelx_fcf_file_internal(const char *filename);
#ifdef __cplusplus/* protection from use in callbacks.c, else compilation probs */
#ifdef USE_GUILE
/*! \brief @return the chain id for the given residue.

@return false if can't do it/fail. */
SCM chain_id_for_shelxl_residue_number(int imol, int resno);
#endif /* USE_GUILE */
/* return 1 for yes, 0 for invalid imol or no. */
int is_shelx_molecule(int imol);

#ifdef USE_PYTHON
/*! \brief @return the chain id for the given residue.  Return Py_False if
  can't do it/fail. */
PyObject *chain_id_for_shelxl_residue_number_py(int imol, int resno);
#endif /* USE_PYTHON */

void add_shelx_string_to_molecule(int imol, const char *string);
#endif /* c++ */


#endif // READ_MOLECULE_HH
