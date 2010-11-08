/*! \file stre.h
    \ingroup OPT10
    \brief STRE class declaration
*/

#ifndef _opt_interfrag_h_
#define _opt_interfrag_h_

#include "linear_algebra.h"

namespace opt {
/*
INTERFRAG is a set of coordinates between two fragments (A and B).
Up to 3 reference atoms on each fragment (dA[3] and dB[3]) are defined 
as linear combinations of the atoms in A and B, using A_weight and B_weight.

The six coordinates in the set are in a canonical order:
D[0], RAB     = distance between dA[0] and dB[0]
D[1], theta_A = angle,   dA[1]-dA[0]-dB[0]
D[2], theta_B = angle,   dA[0]-dB[0]-dB[1]
D[3], tau     = dihedral angle, dA[1]-dA[0]-dB[0]-dB[1]
D[4], phi_A   = dihedral angle, dA[2]-dA[1]-dA[0]-dB[0]
D[5], phi_B   = dihedral angle, dA[0]-dB[0]-dB[1]-dB[2]

inter_geom[0] = dA[2]; // atoms ordered by connectivity of coordinates
inter_geom[1] = dA[1]; // for A, this is reverse of order in which weights
inter_geom[2] = dA[0]; // are provided to the constructor
inter_geom[3] = dB[0];
inter_geom[4] = dB[1];
inter_geom[5] = dB[2];
*/

class INTERFRAG {

  FRAG *A; // pointer to fragment A
  FRAG *B; // pointer to fragment B

  int A_index;  // index of fragment A in MOLECULE fragments, vector<FRAG *>
  int B_index;  // index of fragment B in MOLECULE fragments, vector<FRAG *>

  int ndA; // number of reference points used on A (<= 3)
  int ndB; // number of reference points used on B (<= 3)

  // weights on the atoms of fragment A/B, defining the reference points
  double **weightA; // dimension [ndA][A->natom]
  double **weightB; // dimension [ndB][B->natom]

  FRAG *inter_frag; // pseudo-fragment that contains the 6 reference points as its atoms

  bool D_on[6]; // indicates which coordinates [0-5] are present;
                // if ndA or ndB <3, then not all 6 coordinates are defined
  public:

  // memory provided by calling function
  INTERFRAG(FRAG *A_in, FRAG *B_in, int A_index_in, int B_index_in,
      double **weightA_in, double **weightB_in, int ndA_in=3, int ndB_in=3);

  ~INTERFRAG() { delete inter_frag; }

  // update location of reference points using fragment member geometries
  void update_reference_points(void) {
    update_reference_points(A->geom, B->geom);
  }

  // update location of reference points using given geometries
  void update_reference_points(GeomType new_geom_A, GeomType new_geom_B);

  int g_nintco(void) const;

  // return vector index of fragments in molecule vector
  int g_A_index(void) const { return A_index; }
  int g_B_index(void) const { return B_index; }

  int g_ndA(void) const { return ndA; }
  int g_ndB(void) const { return ndB; }

  // compute and return coordinate values - using fragment member geometries
  double *intco_values(void) {
    intco_values(A->geom, B->geom);
  }

  // compute and return coordinate values - using given fragment geometries
  double *intco_values(GeomType new_geom_A, GeomType new_geom_B);

  // returns B matrix from member geometries
  double **compute_B(void) {
    compute_B(A->geom, B->geom);
  }
  // returns B matrix (internals by 3*natomA + 3*natomB)
  double **compute_B(GeomType new_geom_A, GeomType new_geom_B);

  // returns derivative B matrix from member geometries
  double **compute_derivative_B(int intco_index) {
    compute_derivative_B(intco_index, A->geom, B->geom);
  }
  // returns derivative B matrix for one internal, returns 3*natomA x 3*natomA
  double **compute_derivative_B(int intco_index, GeomType new_geom_A, GeomType new_geom_B);

  // print reference point definitions and values of coordinates
  void print_intcos(FILE *fp) const;

  // print coordinate definitions
  void print_intco_dat(FILE *fp, int atom_offset_A=0, int atom_offset_B=0) const;

  // get number of atoms in the two fragments
  int g_natom(void) const { return (A->g_natom() + B->g_natom()); }
  int g_natom_A(void) const { return (A->g_natom()); }
  int g_natom_B(void) const { return (B->g_natom()); }

  int g_intco_natom_A(int intco_index, int atom) {
    if (intco_index < 0 || intco_index > 5)
      throw("INTERFRAG::g_intco_atom_A() intco_index not between 0 and 5");
    return (A->intcos[intco_index]->g_natom());
  }

  bool coordinate_on(int i) const {
    if (i<0 || i>5) throw("INTERFRAG::coordinate_on argument must be 0-5");
    return D_on[i];
  }

  double ** H_guess(void); // guess Hessian

  bool orient_fragment(double *q_target);

}; // class INTERFRAG

} // opt

#endif

