#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
void pageRank(Graph g, double *solution, double damping, double convergence)
{
  // initialize vertex weights to uniform probability. Double
  // precision scores are used to avoid underflow for large graphs
  int numNodes = num_nodes(g);
  double equal_prob = 1.0 / numNodes;
  double *old_rank;
  double *new_rank;
  old_rank = (double*)malloc(sizeof(double) * numNodes);
  if (!old_rank) {
    return;
  }
  new_rank = (double*)malloc(sizeof(double) * numNodes);
  if (!new_rank) {
    free(old_rank);
    return;
  }

  for (int i = 0; i < numNodes; i++) {
    old_rank[i] = equal_prob;
  }

  // initialize variables required for page_rank algorithm
  bool converged = false;
  double global_diff;
  double no_outgoing_prob;
  double no_outgoing_val;

  double *local_diff = (double *)malloc(sizeof(double) * numNodes);
  if (!local_diff) {
   free(old_rank);
   free(new_rank);
   return;
  }

  while (!converged) {

    no_outgoing_prob = 0.0;

    #pragma omp parallel for reduction(+:no_outgoing_prob) schedule(dynamic, 4)
    for (int i = 0; i < numNodes; i++) {
      if (outgoing_size(g, i) == 0) {
        no_outgoing_prob += old_rank[i];
      }

      new_rank[i] = 0.0;
      // const Vertex* in_arr = incoming_begin(g, i);
      // int in_size = incoming_size(g, i);
      // for (int j = 0; j < in_size; j++) {
      //   new_rank[i] += old_rank[in_arr[j]] / outgoing_size(g, in_arr[j]);
      // }
      const Vertex* i_start = incoming_begin(g, i);
      const Vertex* i_end = incoming_end(g, i);
      for (const Vertex* j = i_start; j != i_end; j++) {
        new_rank[i] += old_rank[*j] / outgoing_size(g, *j);
      }

      // new_rank[vi] = (damping * new_rank[vi]) + (1.0-damping) / numNodes;
      new_rank[i] = (damping * new_rank[i]) + (1.0 - damping) / numNodes;
    }

    // compute how much per-node scores have changed
    // global_diff = sum over all nodes vi { abs(new_rank[vi] - old_rank[vi]) };
    global_diff = 0.0;
    no_outgoing_val = damping * no_outgoing_prob * equal_prob;

    #pragma omp parallel for
    for (int i = 0; i < numNodes; i++) {
      new_rank[i] += no_outgoing_val;
      local_diff[i] = abs(new_rank[i] - old_rank[i]);
    }

    #pragma omp parallel for reduction(+:global_diff)
    for (int i = 0; i < numNodes; i++) {
      // #pragma omp atomic
      global_diff += local_diff[i];
    }

    std::swap(new_rank, old_rank);

    converged = (global_diff < convergence);
  }

  for (int i; i < numNodes; i++) {
    solution[i] = old_rank[i];
  }
  free(old_rank);
  free(new_rank);
  free(local_diff);
}
