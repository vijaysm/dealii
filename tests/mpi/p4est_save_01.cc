//---------------------------------------------------------------------------
//    $Id: p4est_2d_refine_02.cc 23906 2011-07-01 14:59:13Z bangerth $
//    Version: $Name$
//
//    Copyright (C) 2009, 2010, 2011 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//---------------------------------------------------------------------------


// save and load a triangulation

#include "../tests.h"
#include "coarse_grid_common.h"
#include <deal.II/base/logstream.h>
#include <deal.II/base/tensor.h>
#include <deal.II/grid/tria.h>
#include <deal.II/distributed/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_tools.h>
#include <deal.II/base/utilities.h>
#include <deal.II/distributed/solution_transfer.h>
#include <deal.II/lac/petsc_parallel_vector.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/dofs/dof_tools.h>


#include <deal.II/fe/fe_q.h>

#include <fstream>



template<int dim>
void test()
{
  unsigned int myid = Utilities::System::get_this_mpi_process (MPI_COMM_WORLD);

  if (Utilities::System::get_this_mpi_process (MPI_COMM_WORLD) == 0)
    deallog << "hyper_cube" << std::endl;

  std::string filename =
      (std::string("p4est_save_01/ncpu_") + Utilities::int_to_string(Utilities::System::get_n_mpi_processes (MPI_COMM_WORLD)) + "/dat");
  {
    parallel::distributed::Triangulation<dim> tr(MPI_COMM_WORLD);

    GridGenerator::hyper_cube(tr);

    tr.refine_global(2);
    for (typename Triangulation<dim>::active_cell_iterator
	   cell = tr.begin_active();
	 cell != tr.end(); ++cell)
      if (!cell->is_ghost() && !cell->is_artificial())
	if (cell->center().norm() < 0.3)
	  {
	    cell->set_refine_flag();
	  }

    tr.execute_coarsening_and_refinement ();

    tr.save(filename.c_str());
  
    if (myid == 0)
      {
	deallog << "#cells = " << tr.n_global_active_cells() << std::endl;
	deallog << "cells(0) = " << tr.n_active_cells() << std::endl;
      }
    deallog << "Checksum: "
	    << tr.get_checksum ()
	    << std::endl;
  
  }
  MPI_Barrier(MPI_COMM_WORLD);

  {
    parallel::distributed::Triangulation<dim> tr(MPI_COMM_WORLD);

    GridGenerator::hyper_cube(tr);
    tr.load(filename.c_str());

    if (myid == 0)
      {
	deallog << "#cells = " << tr.n_global_active_cells() << std::endl;
	deallog << "cells(0) = " << tr.n_active_cells() << std::endl;
      }
    deallog << "Checksum: "
	    << tr.get_checksum ()
	    << std::endl;
  }

  if (Utilities::System::get_this_mpi_process (MPI_COMM_WORLD) == 0)
    deallog << "OK" << std::endl;
}


int main(int argc, char *argv[])
{
#ifdef DEAL_II_COMPILER_SUPPORTS_MPI
  PetscInitialize(&argc,&argv,0,0);

//  MPI_Init (&argc,&argv);
#else
  (void)argc;
  (void)argv;
#endif

  unsigned int myid = Utilities::System::get_this_mpi_process (MPI_COMM_WORLD);


  deallog.push(Utilities::int_to_string(myid));

  if (myid == 0)
    {
      std::ofstream logfile(output_file_for_mpi("p4est_save_01").c_str());
      deallog.attach(logfile);
      deallog.depth_console(0);
      deallog.threshold_double(1.e-10);

      deallog.push("2d");
      test<2>();
      deallog.pop();
    }
  else
    test<2>();


#ifdef DEAL_II_COMPILER_SUPPORTS_MPI
				   //MPI_Finalize();
  PetscFinalize();

#endif
}