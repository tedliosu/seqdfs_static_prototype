/*
 * Main include file for SeqDFS vs Boost DFS program.
 */

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_node_map.hpp>
#include <deque>
#include <memory>
#include <utility>

typedef struct custom_vertex_props {
  long seq_pop_out_idx;
  bool visited;
  long vert_name;
  boost::default_color_type vertex_color;
} custom_vertex_props_struct;
typedef boost::adjacency_list<boost::setS,
                              boost::setS,
                              boost::bidirectionalS,
                              custom_vertex_props_struct>
    unigraph_type;
typedef unigraph_type::vertex_descriptor vert_descrip_type;
typedef unigraph_type::edge_descriptor edge_descrip_type;
typedef std::vector<vert_descrip_type> vert_vec_type;
typedef std::deque<vert_descrip_type> vert_deque_type;
typedef std::vector<vert_deque_type> dfs_seq_type;
typedef struct unigraph_with_vert_vec {
  std::shared_ptr<unigraph_type> unigraph_ptr;
  std::shared_ptr<vert_vec_type> vert_vec_ptr;
} unigraph_with_vert_vec_type;
typedef boost::property_map<unigraph_type,
                            decltype(custom_vertex_props_struct::vertex_color)
                                custom_vertex_props_struct::*>::type
    color_map_type;
typedef boost::property_map<unigraph_type,
                            decltype(custom_vertex_props_struct::vert_name)
                                custom_vertex_props_struct::*>::type
    vert_name_map_type;
typedef boost::property_map<unigraph_type,
                            decltype(custom_vertex_props_struct::visited)
                                custom_vertex_props_struct::*>::type
    vert_visited_map_type;
typedef boost::color_traits<color_map_type> color_template;
typedef boost::unordered_flat_map<vert_descrip_type, long>
    vert_to_dfs_seq_idx_map_type;
typedef struct dfs_seq_with_map {
  std::shared_ptr<dfs_seq_type> dfs_seq_ptr;
  std::shared_ptr<vert_to_dfs_seq_idx_map_type> vert_to_dfs_seq_idx_map_ptr;
} dfs_seq_with_map_type;
typedef std::pair<vert_descrip_type, long> vert_descript_to_recur_depth_type;
typedef std::deque<vert_descript_to_recur_depth_type>
    final_dfs_deque_result_type;

class rec_in_deque_dfs_visit_class : public boost::default_dfs_visitor {
 private:
  std::shared_ptr<final_dfs_deque_result_type> ptr_final_dfs_deque;
  final_dfs_deque_result_type dfs_results;
  boost::unordered_node_map<vert_descrip_type, long> vertex_to_recur_depth_map;

 public:
  rec_in_deque_dfs_visit_class(const std::size_t num_vertices) {
    dfs_results = {};
    ptr_final_dfs_deque =
        std::make_shared<final_dfs_deque_result_type>(dfs_results);
    vertex_to_recur_depth_map = {};
    vertex_to_recur_depth_map.reserve(num_vertices);
  }
  void start_vertex(vert_descrip_type vertex, const unigraph_type& unigraph) {
    vertex_to_recur_depth_map.insert(std::make_pair(vertex, 0L));
  }
  void discover_vertex(vert_descrip_type vertex,
                       const unigraph_type& unigraph) {
    ptr_final_dfs_deque->push_back(
        std::make_pair(vertex, vertex_to_recur_depth_map[vertex]));
  }
  void tree_edge(edge_descrip_type tree_edge, const unigraph_type& unigraph) {
    vert_descrip_type source_vert = boost::source(tree_edge, unigraph);
    vert_descrip_type target_vert = boost::target(tree_edge, unigraph);
    long source_vert_depth = vertex_to_recur_depth_map[source_vert];
    vertex_to_recur_depth_map.insert(
        std::make_pair(target_vert, source_vert_depth + 1L));
  }
  void reset_to_init() {
    ptr_final_dfs_deque->clear();
    vertex_to_recur_depth_map.clear();
  }
  const final_dfs_deque_result_type& get_result_deque() {
    return *ptr_final_dfs_deque;
  }
};

void print_dfs_search_result(
    const final_dfs_deque_result_type& dfs_final_vert_seq,
    const vert_name_map_type& vertices_idx_to_name_map);

bool check_if_search_results_are_equal(
    const final_dfs_deque_result_type& final_deque_expected,
    const final_dfs_deque_result_type& final_deque_actual,
    const vert_name_map_type& vertices_idx_to_name_map);

void seq_first_dfs(const long vertex_idx,
                   const dfs_seq_with_map_type& dfs_seq_with_map,
                   unigraph_type& unigraph,
                   final_dfs_deque_result_type& dfs_search_result_deque,
                   long& recursion_lvl);

dfs_seq_with_map_type dfs_seq_cons(
    const unigraph_with_vert_vec_type& graph_with_vert_vec,
    vert_deque_type& deque_of_in_deg_zero_verts,
    long& seq_idx);
