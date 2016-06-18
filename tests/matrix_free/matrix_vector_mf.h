//------------------  matrix_vector_common.h  ------------------------
//    Version: $Name$
//
//------------------  matrix_vector_common.h  ------------------------


// this is a template for matrix-vector products with the Helmholtz equation
// (zero and first derivatives) on different kinds of meshes (Cartesian,
// general, with and without hanging nodes). It also tests the multithreading
// in case it was enabled

#include "../tests.h"

#include <deal.II/matrix_free/matrix_free.h>
#include <deal.II/matrix_free/fe_evaluation.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/la_parallel_vector.h>


template <int dim, int fe_degree, typename VectorType>
void
helmholtz_operator (const MatrixFree<dim,typename VectorType::value_type> &data,
                    VectorType                                            &dst,
                    const VectorType                                      &src,
                    const std::pair<unsigned int,unsigned int>            &cell_range)
{
  typedef typename VectorType::value_type Number;
  FEEvaluation<dim,fe_degree,fe_degree+1,1,Number> fe_eval (data);
  const unsigned int n_q_points = fe_eval.n_q_points;

  for (unsigned int cell=cell_range.first; cell<cell_range.second; ++cell)
    {
      fe_eval.reinit (cell);
      fe_eval.read_dof_values (src);
      fe_eval.evaluate (true, true, false);
      for (unsigned int q=0; q<n_q_points; ++q)
        {
          fe_eval.submit_value (Number(10)*fe_eval.get_value(q),q);
          fe_eval.submit_gradient (fe_eval.get_gradient(q),q);
        }
      fe_eval.integrate (true,true);
      fe_eval.distribute_local_to_global (dst);
    }
}



template <int dim, int fe_degree, typename Number, typename VectorType=Vector<Number> >
class MatrixFreeTest
{
public:
  typedef VectorizedArray<Number> vector_t;

  MatrixFreeTest(const MatrixFree<dim,Number> &data_in):
    data (data_in)
  {};

  void vmult (VectorType       &dst,
              const VectorType &src) const
  {
    dst = 0;
    const std_cxx11::function<void(const MatrixFree<dim,typename VectorType::value_type> &,
                                   VectorType &,
                                   const VectorType &,
                                   const std::pair<unsigned int,unsigned int> &)>
    wrap = helmholtz_operator<dim,fe_degree,VectorType>;
    data.cell_loop (wrap, dst, src);
  };

private:
  const MatrixFree<dim,Number> &data;
};
