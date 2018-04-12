/*
 * @BEGIN LICENSE
 *
 * Psi4: an open-source quantum chemistry software package
 *
 * Copyright (c) 2007-2017 The Psi4 Developers.
 *
 * The copyrights for code used from other parties are included in
 * the corresponding files.
 *
 * This file is part of Psi4.
 *
 * Psi4 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * Psi4 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with Psi4; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @END LICENSE
 */

#ifndef THREE_INDEX_CHOLESKY
#define THREE_INDEX_CHOLESKY

#include "psi4/psi4-dec.h"
#include "psi4/libmints/typedefs.h"

namespace psi {

class Vector;
class TwoBodyAOInt;
class BasisSet;

class Cholesky {

protected:
    /// Maximum Chebyshev error allowed in the decomposition
    double delta_;
    /// Maximum memory to use, in doubles
    size_t memory_;
    /// Full L (Q x n), if choleskify() called()
    SharedMatrix L_;
    /// Number of columns required, if choleskify() called
    size_t Q_;

public:
    /*!
     * Constructor, does not build decomposition.
     * \param delta maximum Chebyshev error allowed in the decomposition
     * \param memory maximum memory allowed, in doubles
     **/
    Cholesky(double delta, size_t memory);
    /// Destructor, resets L_
    virtual ~Cholesky();

    /// Perform the cholesky decomposition (requires 2QN memory)
    virtual void choleskify();

    /// Shared pointer to decomposition (Q x N), if choleskify() called
    SharedMatrix L() const { return L_; }
    /// Number of columns required to reach accuracy delta, if choleskify() called
    size_t Q() const { return Q_; }
    /// Dimension of the original square tensor, provided by the subclass
    virtual size_t N() = 0;
    /// Maximum Chebyshev error allowed in the decomposition
    double delta() const { return delta_; }

    /// Diagonal of the original square tensor, provided by the subclass
    virtual void compute_diagonal(double* target) = 0;
    /// Row row of the original square tensor, provided by the subclass
    virtual void compute_row(int row, double* target) = 0;

};

class CholeskyMatrix : public Cholesky {

protected:
    SharedMatrix A_;
public:
    CholeskyMatrix(SharedMatrix A, double delta, size_t memory);
    virtual ~CholeskyMatrix();

    virtual size_t N();
    virtual void compute_diagonal(double* target);
    virtual void compute_row(int row, double* target);
};

class PSI_API CholeskyERI : public Cholesky {

protected:
    double schwarz_;
    std::shared_ptr<BasisSet> basisset_;
    std::shared_ptr<TwoBodyAOInt> integral_;
public:
    CholeskyERI(std::shared_ptr<TwoBodyAOInt> integral, double schwarz, double delta, size_t memory);
    virtual ~CholeskyERI();

    virtual size_t N();
    virtual void compute_diagonal(double* target);
    virtual void compute_row(int row, double* target);
};

class CholeskyMP2 : public Cholesky {

protected:
    bool symmetric_;
    SharedMatrix Qia_;
    std::shared_ptr<Vector> eps_aocc_;
    std::shared_ptr<Vector> eps_avir_;
public:
    CholeskyMP2(SharedMatrix Qia, std::shared_ptr<Vector> eps_aocc,
        std::shared_ptr<Vector> eps_avir, bool symmetric,
        double delta, size_t memory);
    virtual ~CholeskyMP2();

    virtual size_t N();
    virtual void compute_diagonal(double* target);
    virtual void compute_row(int row, double* target);
};

class CholeskyDelta : public Cholesky {

protected:
    std::shared_ptr<Vector> eps_aocc_;
    std::shared_ptr<Vector> eps_avir_;
public:
    CholeskyDelta(std::shared_ptr<Vector> eps_aocc,
        std::shared_ptr<Vector> eps_avir,
        double delta, size_t memory);
    virtual ~CholeskyDelta();

    virtual size_t N();
    virtual void compute_diagonal(double* target);
    virtual void compute_row(int row, double* target);
};

class CholeskyLocal : public Cholesky {

protected:
    SharedMatrix C_;
public:
    CholeskyLocal(SharedMatrix C,
        double delta, size_t memory);
    virtual ~CholeskyLocal();

    virtual size_t N();
    virtual void compute_diagonal(double* target);
    virtual void compute_row(int row, double* target);
};

} // Namespace psi
#endif
