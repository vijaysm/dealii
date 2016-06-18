// ---------------------------------------------------------------------
//
// Copyright (C) 2005 - 2015 by the deal.II authors
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



// Test GridGenerator::create_triangulation_with_removed_cells

#include "../tests.h"
#include <deal.II/base/logstream.h>
#include <deal.II/base/tensor.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>


#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/grid/grid_generator.h>

#include <deal.II/grid/tria_boundary_lib.h>

#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_tools.h>

#include <deal.II/grid/manifold_lib.h>

#include <fstream>
#include <iomanip>


template<int dim, int spacedim>
void test(std::ostream &out)
{
  Triangulation<dim,spacedim>   triangulation;

  static SphericalManifold<dim,spacedim> surface_description;

  GridGenerator::hyper_sphere(triangulation);

  triangulation.set_all_manifold_ids(0);
  triangulation.set_manifold (0, surface_description);
  triangulation.refine_global(3);

  GridOut go;
  go.write_gnuplot(triangulation, out);
}


int main()
{
  std::ofstream logfile("output");
  deallog.attach(logfile);
  deallog.threshold_double(1.e-10);

  test<1,2>(logfile);
  test<2,3>(logfile);
}
