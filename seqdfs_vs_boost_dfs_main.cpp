
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_node_set.hpp>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <utility>
#include "seqdfs_vs_boost_dfs_main.hpp"

constexpr int RANDOM_GRAPH_SEED = 94893;
constexpr int INTEGRAL_E_ADDIT_TO_V_RATIO = 3;
constexpr int INTEGRAL_TRIES_TO_V_RATION_MAX = 25;
constexpr int NUM_ITERS = 4;

// Don't touch below constants!
constexpr double NUM_MILLISEC_IN_SEC = 1000.0;
constexpr int PRINT_PRECISION = 6;

typedef boost::graph_traits<unigraph_type>::edge_iterator edge_itr_type;
typedef std::pair<vert_descrip_type, long> vert_descript_to_recur_depth_type;
typedef std::deque<vert_descript_to_recur_depth_type>
    final_dfs_deque_result_type;
typedef std::pair<long, long> edge_using_long_type;
typedef std::vector<edge_using_long_type> long_edges_vec_type;

unigraph_with_vert_vec_type generate_random_graph(std::size_t num_vertices) {
  std::size_t num_addit_edges = INTEGRAL_E_ADDIT_TO_V_RATIO * num_vertices + 1;
  long num_max_tries_adding_edges =
      INTEGRAL_TRIES_TO_V_RATION_MAX * num_vertices;
  unigraph_with_vert_vec_type graph_with_vert_vec = {};
  unigraph_type unigraph(num_vertices);
  vert_vec_type vert_vec(num_vertices);
  std::vector<long> vert_names_n_idxs_vec(num_vertices);
  long_edges_vec_type edges_to_be_inserted(num_vertices - 1 + num_addit_edges);
  std::mt19937_64 mers_twist_gen(RANDOM_GRAPH_SEED);
  std::uniform_int_distribution<long> vertex_idx_gen(
      0, static_cast<long>(num_vertices) - 1);
  std::shared_ptr<unigraph_type> unigraph_ptr =
      std::make_shared<unigraph_type>(unigraph);
  auto edges_using_long_cmp = [](const edge_using_long_type& first_edge,
                                 const edge_using_long_type& second_edge) {
    if (first_edge.first != second_edge.first) {
      return first_edge.first < second_edge.first;
    } else {
      return first_edge.second < second_edge.second;
    }
  };
  std::set<edge_using_long_type, decltype(edges_using_long_cmp)> all_edges_set(
      edges_using_long_cmp);
  graph_with_vert_vec.unigraph_ptr = std::make_shared<unigraph_type>(unigraph);
  graph_with_vert_vec.vert_vec_ptr = std::make_shared<vert_vec_type>(vert_vec);

  for (long vertex_num = 0; vertex_num < static_cast<long>(num_vertices);
       ++vertex_num) {
    custom_vertex_props_struct vertex = {0, false, vertex_num,
                                         color_template::white()};
    vert_names_n_idxs_vec[vertex_num] = vertex_num;
    graph_with_vert_vec.vert_vec_ptr->at(vertex_num) =
        add_vertex(vertex, *(graph_with_vert_vec.unigraph_ptr));
  }

  std::shuffle(vert_names_n_idxs_vec.begin(), vert_names_n_idxs_vec.end(),
               mers_twist_gen);

  for (long vertex_num = 1; vertex_num < static_cast<long>(num_vertices);
       ++vertex_num) {
    edge_using_long_type one_edge_to_be_inserted =
        std::make_pair(vert_names_n_idxs_vec[vertex_num - 1],
                       vert_names_n_idxs_vec[vertex_num]);
    all_edges_set.insert(one_edge_to_be_inserted);
    edges_to_be_inserted[vertex_num - 1] = one_edge_to_be_inserted;
  }

  long curr_num_addit_edges = 0;
  long current_num_tries = 0;
  while (curr_num_addit_edges < static_cast<long>(num_addit_edges) &&
         current_num_tries < num_max_tries_adding_edges) {
    long vertex_src_idx = vertex_idx_gen(mers_twist_gen);
    long vert_targ_idx = vertex_idx_gen(mers_twist_gen);
    edge_using_long_type edge_candidate =
        std::make_pair(vertex_src_idx, vert_targ_idx);

    if (edge_candidate.first != edge_candidate.second &&
        all_edges_set.find(edge_candidate) == all_edges_set.end()) {
      edges_to_be_inserted[num_vertices - 1 + curr_num_addit_edges] =
          edge_candidate;
      all_edges_set.insert(edge_candidate);
      ++curr_num_addit_edges;
    }
    ++current_num_tries;
  }

  std::sort(edges_to_be_inserted.begin(), edges_to_be_inserted.end(),
            edges_using_long_cmp);

  for (const edge_using_long_type& edge : edges_to_be_inserted) {
    boost::add_edge(graph_with_vert_vec.vert_vec_ptr->at(edge.first),
                    graph_with_vert_vec.vert_vec_ptr->at(edge.second),
                    *(graph_with_vert_vec.unigraph_ptr));
  }

  return graph_with_vert_vec;
}

int main() {
  vert_name_map_type vertices_idx_to_name_map;
  vert_visited_map_type vertices_idx_to_visit_data_map;
  unigraph_with_vert_vec_type test_graph_with_vert_vec = {};
  final_dfs_deque_result_type dfs_final_vert_seq_seqdfs = {};
  final_dfs_deque_result_type dfs_final_vert_seq_boost_dfs = {};
  vert_deque_type deque_of_in_deg_zero_verts = {};
  color_map_type graph_color_map = {};
  long seq_idx = 0;
  std::size_t num_vertices = 0;

  while (num_vertices <= 0) {
    std::cout
        << "Enter the number of graph verticies (NON-ZERO POSITIVE VALUE): ";
    std::cin >> num_vertices;
  }

  rec_in_deque_dfs_visit_class record_in_deque_dfs_visitor(num_vertices);

  std::cout << "\n\n\n\nGenerating graph, please be patient...\n";
  test_graph_with_vert_vec = generate_random_graph(num_vertices);
  std::cout << "Graph generation done!\n";

  vertices_idx_to_name_map =
      boost::get(&custom_vertex_props_struct::vert_name,
                 *(test_graph_with_vert_vec.unigraph_ptr));
  vertices_idx_to_visit_data_map =
      boost::get(&custom_vertex_props_struct::visited,
                 *(test_graph_with_vert_vec.unigraph_ptr));
  graph_color_map = boost::get(&custom_vertex_props_struct::vertex_color,
                               *(test_graph_with_vert_vec.unigraph_ptr));

  std::cout
      << "\n\nGenerating reordered copy of graph using SeqDFS technique...\n";
  auto start_moment = std::chrono::steady_clock::now();
  dfs_seq_with_map_type dfs_constructed_seq_with_map = dfs_seq_cons(
      test_graph_with_vert_vec, deque_of_in_deg_zero_verts, seq_idx);
  auto end_moment = std::chrono::steady_clock::now();
  auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                          end_moment - start_moment)
                          .count() /
                      NUM_MILLISEC_IN_SEC;
  std::cout << std::fixed << std::setprecision(PRINT_PRECISION)
            << "Generating reordered copy took " << time_elapsed
            << " seconds.\n";

  std::cout << "\n\n\nPicking UP TO " << NUM_ITERS
            << " \"equally spaced\" vertices to do DFS traversal benchmark.\n";
  std::shared_ptr<vert_vec_type> ptr_to_all_verts =
      test_graph_with_vert_vec.vert_vec_ptr;
  const long fin_num_vertices = ptr_to_all_verts->size();
  const long iter_stride_in_vec =
      fin_num_vertices < NUM_ITERS ? 1 : fin_num_vertices / NUM_ITERS;

  auto total_exec_times_seqdfs = std::chrono::nanoseconds(0);
  auto total_exec_times_boost_dfs = std::chrono::nanoseconds(0);

  for (long vert_idx = 0; vert_idx < fin_num_vertices;
       vert_idx += iter_stride_in_vec) {
    // Reset values
    for (const vert_descrip_type& vertex :
         *(test_graph_with_vert_vec.vert_vec_ptr)) {
      vertices_idx_to_visit_data_map[vertex] = false;
    }
    long recursion_lvl = -1;
    dfs_final_vert_seq_seqdfs.clear();

    std::cout << "\n\n\tStarting SeqDFS-style DFS on reordered graph copy at "
                 "vertex \""
              << vertices_idx_to_name_map[ptr_to_all_verts->at(vert_idx)]
              << "\"...\n";
    start_moment = std::chrono::steady_clock::now();
    seq_first_dfs(dfs_constructed_seq_with_map.vert_to_dfs_seq_idx_map_ptr
                      ->find(ptr_to_all_verts->at(vert_idx))
                      ->second,
                  dfs_constructed_seq_with_map,
                  *(test_graph_with_vert_vec.unigraph_ptr),
                  dfs_final_vert_seq_seqdfs, recursion_lvl);
    end_moment = std::chrono::steady_clock::now();
    total_exec_times_seqdfs += (end_moment - start_moment);
    time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                       end_moment - start_moment)
                       .count() /
                   NUM_MILLISEC_IN_SEC;
    std::cout << std::fixed << std::setprecision(PRINT_PRECISION)
              << "\tSeqDFS-style DFS as mentioned previously took "
              << time_elapsed << " seconds.\n";

    std::cout << "\tSeqDFS-style DFS as mentioned previously traversed "
              << dfs_final_vert_seq_seqdfs.size()
              << " vertices out\n\t\tof a total of " << ptr_to_all_verts->size()
              << "\n";

    dfs_final_vert_seq_boost_dfs.clear();
    record_in_deque_dfs_visitor.reset_to_init();
    for (const vert_descrip_type& vertex : *ptr_to_all_verts) {
      graph_color_map[vertex] = color_template::white();
    }

    std::cout << "\n\n\tStarting regular boost DFS search starting at vertex \""
              << vertices_idx_to_name_map[ptr_to_all_verts->at(vert_idx)]
              << "\"...\n";

    start_moment = std::chrono::steady_clock::now();
    boost::depth_first_visit(*(test_graph_with_vert_vec.unigraph_ptr),
                             ptr_to_all_verts->at(vert_idx),
                             record_in_deque_dfs_visitor, graph_color_map);
    end_moment = std::chrono::steady_clock::now();
    total_exec_times_boost_dfs += (end_moment - start_moment);
    time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                       end_moment - start_moment)
                       .count() /
                   NUM_MILLISEC_IN_SEC;
    std::cout << std::fixed << std::setprecision(PRINT_PRECISION)
              << "\tBoost built-in DFS as mentioned previously took "
              << time_elapsed << " seconds.\n";

    dfs_final_vert_seq_boost_dfs =
        record_in_deque_dfs_visitor.get_result_deque();

    std::cout << "\tBoost DFS as mentioned previously traversed "
              << dfs_final_vert_seq_boost_dfs.size()
              << " vertices out\n\t\tof a total of " << ptr_to_all_verts->size()
              << "\n";

    // Check only works for first vertex unfortunately due to graph reordering
    if (vert_idx == 0) {
      check_if_search_results_are_equal(dfs_final_vert_seq_boost_dfs,
                                        dfs_final_vert_seq_seqdfs,
                                        vertices_idx_to_name_map);
    }
  }
  time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                     total_exec_times_seqdfs)
                     .count() /
                 (NUM_MILLISEC_IN_SEC * NUM_ITERS);
  std::cout << std::fixed << std::setprecision(PRINT_PRECISION)
            << "\n\n\nSeqDFS-style DFS as mentioned previously took "
            << time_elapsed << " seconds on average per iteration\n";
  time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                     total_exec_times_boost_dfs)
                     .count() /
                 (NUM_MILLISEC_IN_SEC * NUM_ITERS);
  std::cout << std::fixed << std::setprecision(PRINT_PRECISION)
            << "\n\nBoost built-in DFS as mentioned previously took "
            << time_elapsed << " seconds on average per iteration\n";
  std::cout << "\n\n\n";

  return EXIT_SUCCESS;
}
