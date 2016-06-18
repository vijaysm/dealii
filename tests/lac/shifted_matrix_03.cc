// ---------------------------------------------------------------------
//
// Copyright (C) 2014 - 2015 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the deal.II distribution.
//
// ---------------------------------------------------------------------

// check ShiftedMatrix::checkResidual

#include "../tests.h"
#include <deal.II/base/logstream.h>
#include <deal.II/lac/shifted_matrix.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/vector.h>

#include <fstream>
#include <iomanip>
#include <cmath>

template<typename number>
void
checkResidual(FullMatrix<number> &A, double sigma, Vector<number> &V,
              Vector<number> &R)
{
  deallog << "residual" << std::endl;

  ShiftedMatrix < FullMatrix<number> > S(A, sigma);
  Vector<number> O(A.m());
  double residual;

  residual = S.residual(O, V, R);

  // Check the dimensions of the result matrix
  Assert(A.m() == O.size(), ExcInternalError());
  deallog << "Dimensions of result vector verified" << std::endl;

  deallog << "Residual vector" << std::endl;
  for (unsigned int i = 0; i < O.size(); ++i)
    deallog << O(i) << '\t';
  deallog << std::endl;

  deallog << "Residual value" << std::endl;
  deallog << residual << std::endl;
}

int
main()
{
  std::ofstream logfile("output");
  deallog << std::fixed;
  deallog << std::setprecision(4);
  deallog.attach(logfile);
  deallog.threshold_double(1.e-10);

  const double Adata[] =
  { 2, 3, 4, 5 };

  FullMatrix<double> A(2, 2);

  A.fill(Adata);

  Vector<double> V(2);
  V(0) = 1;
  V(1) = 2;

  Vector<double> R(2);
  R(0) = 1;
  R(1) = 1;

  checkResidual<double>(A, 2, V, R);
}
