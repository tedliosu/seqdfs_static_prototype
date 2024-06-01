
/*
 * File where SeqDFS functions and Boost DFS visitor are defined,
 * in addition to helper functions definition.
 */

#include <iostream>
#include "seqdfs_vs_boost_dfs_main.hpp"

typedef boost::property_map<
    unigraph_type,
    decltype(custom_vertex_props_struct::seq_pop_out_idx)
        custom_vertex_props_struct::*>::type vert_pop_out_idx_map_type;
typedef boost::graph_traits<unigraph_type>::out_edge_iterator out_edge_itr_type;

void print_dfs_search_result(
    const final_dfs_deque_result_type& dfs_final_vert_seq,
    const vert_name_map_type& vertices_idx_to_name_map) {
  for (const vert_descript_to_recur_depth_type& vertex_depth_pair :
       dfs_final_vert_seq) {
    std::cout << "Vertex \""
              << vertices_idx_to_name_map[vertex_depth_pair.first]
              << "\" at depth " << vertex_depth_pair.second << ".\n";
  }
}

bool check_if_search_results_are_equal(
    const final_dfs_deque_result_type& final_deque_expected,
    const final_dfs_deque_result_type& final_deque_actual,
    const vert_name_map_type& vertices_idx_to_name_map) {
  bool are_deeply_equal = true;
  if (final_deque_actual.size() != final_deque_expected.size()) {
    std::cerr << "\tERROR: expected final list of vertex-depth pairs  to be of "
                 "length "
              << final_deque_expected.size()
              << ",\n\t\tbut got actual list length of "
              << final_deque_actual.size() << ".\n";
    are_deeply_equal = false;
  } else {
    auto expected_deque_itr = final_deque_expected.begin();
    long curr_deque_idx = 0;

    for (auto actual_deque_itr = final_deque_actual.begin();
         actual_deque_itr != final_deque_actual.end() &&
         expected_deque_itr != final_deque_expected.end() && are_deeply_equal;
         ++actual_deque_itr) {
      long vertex_name_expected =
          vertices_idx_to_name_map[expected_deque_itr->first];
      long vertex_name_actual =
          vertices_idx_to_name_map[actual_deque_itr->first];
      if (vertex_name_expected != vertex_name_actual ||
          expected_deque_itr->second != actual_deque_itr->second) {
        std::cerr << "\tERROR: expected at position " << curr_deque_idx
                  << " in result list vertex \"" << vertex_name_expected
                  << "\"\n\t\tat search depth of " << expected_deque_itr->second
                  << "; but got vertex \"" << vertex_name_actual
                  << "\" instead\n\t\tat search depth of "
                  << actual_deque_itr->second << ".\n\n";
        are_deeply_equal = false;
      }
      ++expected_deque_itr;
      ++curr_deque_idx;
    }
  }

  if (are_deeply_equal) {
    std::cout << "\n\n\tCongrats; actual list of vertex-depth pairs is same as "
                 "expected!\n\n";
  }

  return are_deeply_equal;
}

void seq_first_dfs_on_non_tree_edges(
    const long vertex_idx,
    const vert_deque_type& vert_seq_in_dfs_seq,
    const dfs_seq_with_map_type& dfs_seq_with_map,
    unigraph_type& unigraph,
    final_dfs_deque_result_type& dfs_search_result_deque,
    long& recursion_lvl) {
  vert_visited_map_type vertices_idx_to_visit_data_map =
      boost::get(&custom_vertex_props_struct::visited, unigraph);
  vert_pop_out_idx_map_type vert_pop_out_idx_map =
      boost::get(&custom_vertex_props_struct::seq_pop_out_idx, unigraph);
  vert_descrip_type curr_vert = vert_seq_in_dfs_seq[vertex_idx];
  vertices_idx_to_visit_data_map[curr_vert] = true;
  ++recursion_lvl;
  dfs_search_result_deque.push_back(std::make_pair(curr_vert, recursion_lvl));
  long next_vert_idx =
      dfs_seq_with_map.vert_to_dfs_seq_idx_map_ptr->find(curr_vert)->second + 1;
  if (next_vert_idx < static_cast<long>(dfs_seq_with_map.dfs_seq_ptr->size())) {
    vert_descrip_type next_vert =
        dfs_seq_with_map.dfs_seq_ptr->at(next_vert_idx).front();
    while (vert_pop_out_idx_map[next_vert] <= vert_pop_out_idx_map[curr_vert]) {
      if (!vertices_idx_to_visit_data_map[next_vert]) {
        seq_first_dfs(next_vert_idx, dfs_seq_with_map, unigraph,
                      dfs_search_result_deque, recursion_lvl);
      }
      next_vert_idx = vert_pop_out_idx_map[next_vert];
      if (next_vert_idx >=
          static_cast<long>(dfs_seq_with_map.dfs_seq_ptr->size())) {
        break;
      } else {
        next_vert = dfs_seq_with_map.dfs_seq_ptr->at(next_vert_idx).front();
      }
    }
  }
  long curr_vert_idx =
      dfs_seq_with_map.vert_to_dfs_seq_idx_map_ptr->find(curr_vert)->second;
  for (long non_tree_edges_vertex_idx = 1;
       non_tree_edges_vertex_idx <
       static_cast<long>(
           dfs_seq_with_map.dfs_seq_ptr->at(curr_vert_idx).size());
       ++non_tree_edges_vertex_idx) {
    if (!vertices_idx_to_visit_data_map[dfs_seq_with_map.dfs_seq_ptr->at(
            curr_vert_idx)[non_tree_edges_vertex_idx]]) {
      seq_first_dfs_on_non_tree_edges(
          non_tree_edges_vertex_idx,
          dfs_seq_with_map.dfs_seq_ptr->at(curr_vert_idx), dfs_seq_with_map,
          unigraph, dfs_search_result_deque, recursion_lvl);
    }
  }

  --recursion_lvl;
}

void seq_first_dfs(const long vertex_idx,
                   const dfs_seq_with_map_type& dfs_seq_with_map,
                   unigraph_type& unigraph,
                   final_dfs_deque_result_type& dfs_search_result_deque,
                   long& recursion_lvl) {
  vert_visited_map_type vertices_idx_to_visit_data_map =
      boost::get(&custom_vertex_props_struct::visited, unigraph);
  vert_pop_out_idx_map_type vert_pop_out_idx_map =
      boost::get(&custom_vertex_props_struct::seq_pop_out_idx, unigraph);
  vert_descrip_type curr_vert =
      dfs_seq_with_map.dfs_seq_ptr->at(vertex_idx).front();
  vertices_idx_to_visit_data_map[curr_vert] = true;
  ++recursion_lvl;
  dfs_search_result_deque.push_back(std::make_pair(curr_vert, recursion_lvl));
  long next_vert_idx = vertex_idx + 1;
  if (next_vert_idx < static_cast<long>(dfs_seq_with_map.dfs_seq_ptr->size())) {
    vert_descrip_type next_vert =
        dfs_seq_with_map.dfs_seq_ptr->at(next_vert_idx).front();
    while (vert_pop_out_idx_map[next_vert] <= vert_pop_out_idx_map[curr_vert]) {
      if (!vertices_idx_to_visit_data_map[next_vert]) {
        seq_first_dfs(next_vert_idx, dfs_seq_with_map, unigraph,
                      dfs_search_result_deque, recursion_lvl);
      }
      next_vert_idx = vert_pop_out_idx_map[next_vert];
      if (next_vert_idx >=
          static_cast<long>(dfs_seq_with_map.dfs_seq_ptr->size())) {
        break;
      } else {
        next_vert = dfs_seq_with_map.dfs_seq_ptr->at(next_vert_idx).front();
      }
    }
  }

  for (long non_tree_edges_vertex_idx = 1;
       non_tree_edges_vertex_idx <
       static_cast<long>(dfs_seq_with_map.dfs_seq_ptr->at(vertex_idx).size());
       ++non_tree_edges_vertex_idx) {
    if (!vertices_idx_to_visit_data_map[dfs_seq_with_map.dfs_seq_ptr->at(
            vertex_idx)[non_tree_edges_vertex_idx]]) {
      seq_first_dfs_on_non_tree_edges(
          non_tree_edges_vertex_idx,
          dfs_seq_with_map.dfs_seq_ptr->at(vertex_idx), dfs_seq_with_map,
          unigraph, dfs_search_result_deque, recursion_lvl);
    }
  }

  --recursion_lvl;
}

void cons_dfs_seq_from_vert(const vert_descrip_type& vertex,
                            const dfs_seq_with_map_type& dfs_seq_with_map,
                            const unigraph_type& unigraph,
                            const vert_visited_map_type& visited_data_map,
                            const vert_pop_out_idx_map_type& pop_out_idx_map,
                            long& seq_idx,
                            const long total_num_vertices) {
  out_edge_itr_type edge_itr_out_edges, edge_itr_out_edges_end;
  visited_data_map[vertex] = true;
  vert_deque_type vertex_seq = {};
  dfs_seq_with_map.dfs_seq_ptr->push_back(vertex_seq);
  long vert_seq_dest_idx = dfs_seq_with_map.dfs_seq_ptr->size() - 1;
  dfs_seq_with_map.dfs_seq_ptr->back().push_back(vertex);
  dfs_seq_with_map.vert_to_dfs_seq_idx_map_ptr->insert(
      std::make_pair(vertex, vert_seq_dest_idx));
  ++seq_idx;
  for (boost::tie(edge_itr_out_edges, edge_itr_out_edges_end) =
           boost::out_edges(vertex, unigraph);
       edge_itr_out_edges != edge_itr_out_edges_end; ++edge_itr_out_edges) {
    vert_descrip_type target_vert =
        boost::target(*edge_itr_out_edges, unigraph);
    if (!visited_data_map[target_vert]) {
      cons_dfs_seq_from_vert(target_vert, dfs_seq_with_map, unigraph,
                             visited_data_map, pop_out_idx_map, seq_idx,
                             total_num_vertices);
    } else {
      dfs_seq_with_map.dfs_seq_ptr->at(vert_seq_dest_idx)
          .push_back(target_vert);
    }
  }
  pop_out_idx_map[vertex] = seq_idx;
}

dfs_seq_with_map_type dfs_seq_cons(
    const unigraph_with_vert_vec_type& graph_with_vert_vec,
    vert_deque_type& deque_of_in_deg_zero_verts,
    long& seq_idx) {
  dfs_seq_type sequence = {};
  vert_to_dfs_seq_idx_map_type vert_to_dfs_seq_idx_map = {};
  dfs_seq_with_map_type dfs_seq_with_map = {};
  const long total_num_vertices = graph_with_vert_vec.vert_vec_ptr->size();
  dfs_seq_with_map.dfs_seq_ptr = std::make_shared<dfs_seq_type>(sequence);
  dfs_seq_with_map.vert_to_dfs_seq_idx_map_ptr =
      std::make_shared<vert_to_dfs_seq_idx_map_type>(vert_to_dfs_seq_idx_map);
  dfs_seq_with_map.dfs_seq_ptr->reserve(total_num_vertices);
  dfs_seq_with_map.vert_to_dfs_seq_idx_map_ptr->reserve(total_num_vertices);
  vert_visited_map_type vertices_idx_to_visit_data_map =
      boost::get(&custom_vertex_props_struct::visited,
                 *(graph_with_vert_vec.unigraph_ptr));
  vert_pop_out_idx_map_type vert_pop_out_idx_map =
      boost::get(&custom_vertex_props_struct::seq_pop_out_idx,
                 *(graph_with_vert_vec.unigraph_ptr));
  for (const vert_descrip_type& vertex : *(graph_with_vert_vec.vert_vec_ptr)) {
    if (boost::in_degree(vertex, *(graph_with_vert_vec.unigraph_ptr)) <= 0) {
      deque_of_in_deg_zero_verts.push_back(vertex);
      cons_dfs_seq_from_vert(vertex, dfs_seq_with_map,
                             *(graph_with_vert_vec.unigraph_ptr),
                             vertices_idx_to_visit_data_map,
                             vert_pop_out_idx_map, seq_idx, total_num_vertices);
    }
  }

  for (const vert_descrip_type& vertex : *(graph_with_vert_vec.vert_vec_ptr)) {
    if (!vertices_idx_to_visit_data_map[vertex]) {
      cons_dfs_seq_from_vert(vertex, dfs_seq_with_map,
                             *(graph_with_vert_vec.unigraph_ptr),
                             vertices_idx_to_visit_data_map,
                             vert_pop_out_idx_map, seq_idx, total_num_vertices);
    }
  }

  return dfs_seq_with_map;
}
