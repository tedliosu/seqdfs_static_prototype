/*
 * Main include file for SeqDFS vs Boost DFS program.
 */

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <deque>
#include <memory>
#include <type_traits>
#include <utility>

// Temporary workaround for vecS currently breaking unit tests
#ifdef SEQDFS_FAST_GRAPH_MODE
using out_edge_container_type = boost::vecS;
using vertex_container_type = boost::vecS;
#else
using out_edge_container_type = boost::setS;
using vertex_container_type = boost::setS;
#endif

typedef struct custom_vertex_props {
  long seq_pop_out_idx;
  bool visited;
  long vert_name;
  boost::default_color_type vertex_color;
} custom_vertex_props_struct;
typedef boost::adjacency_list<out_edge_container_type,
                              vertex_container_type,
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
typedef boost::property_map<
    unigraph_type,
    decltype(custom_vertex_props_struct::seq_pop_out_idx)
        custom_vertex_props_struct::*>::type vert_pop_out_idx_map_type;
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

template <class RecordCandidate, class DetectionSlot = void>
struct record_vert_returns_void : std::false_type {};

template <class RecordCandidate>
struct record_vert_returns_void<
    RecordCandidate,
    std::void_t<decltype(std::declval<RecordCandidate&>().record_vert(
        std::declval<vert_descrip_type>(),
        std::declval<long>()))>>
    : std::is_same<decltype(std::declval<RecordCandidate&>().record_vert(
                       std::declval<vert_descrip_type>(),
                       std::declval<long>())),
                   void> {};

template <class RecordCandidate, class DetectionSlot = void>
struct num_recorded_verts_returns_size : std::false_type {};

template <class RecordCandidate>
struct num_recorded_verts_returns_size<
    RecordCandidate,
    std::void_t<
        decltype(std::declval<const RecordCandidate&>().num_recorded_verts())>>
    : std::is_convertible<
          decltype(std::declval<const RecordCandidate&>().num_recorded_verts()),
          std::size_t> {};

template <class RecorderCandidate>
struct is_dfs_recorder
    : std::bool_constant<
          record_vert_returns_void<RecorderCandidate>::value &&
          num_recorded_verts_returns_size<RecorderCandidate>::value> {};

class deque_with_depth_recorder {
 private:
  final_dfs_deque_result_type& output_deque;

 public:
  explicit deque_with_depth_recorder(final_dfs_deque_result_type& output_ref)
      : output_deque(output_ref) {}

  void record_vert(vert_descrip_type vertex, long recur_lvl) {
    output_deque.push_back(std::make_pair(vertex, recur_lvl));
  }

  std::size_t num_recorded_verts() const { return output_deque.size(); }
};

class count_only_recorder {
 private:
  std::size_t count = 0;

 public:
  void record_vert(vert_descrip_type, long) { ++count; }

  std::size_t num_recorded_verts() const { return count; }

  void reset() { count = 0; }
};

class rec_in_counter_dfs_visit_class : public boost::default_dfs_visitor {
 private:
  std::shared_ptr<std::size_t> ptr_to_counter;
  std::size_t counter = 0;

 public:
  rec_in_counter_dfs_visit_class() {
    ptr_to_counter = std::make_shared<std::size_t>(counter);
  }
  void start_vertex(vert_descrip_type, const unigraph_type&) {
    *ptr_to_counter = 0;
  }
  void discover_vertex(vert_descrip_type, const unigraph_type&) {
    ++(*ptr_to_counter);
  }
  void reset_to_init(vert_vec_type&) { *ptr_to_counter = 0; }
  std::size_t get_num_verts_recorded() { return *ptr_to_counter; }
};

class rec_in_deque_dfs_visit_class : public boost::default_dfs_visitor {
 private:
  std::shared_ptr<final_dfs_deque_result_type> ptr_final_dfs_deque;
  final_dfs_deque_result_type dfs_results;
  boost::unordered_flat_map<vert_descrip_type, long> vertex_to_recur_depth_map;

 public:
  explicit rec_in_deque_dfs_visit_class(const std::size_t num_vertices) {
    dfs_results = {};
    ptr_final_dfs_deque =
        std::make_shared<final_dfs_deque_result_type>(dfs_results);
    vertex_to_recur_depth_map = {};
    vertex_to_recur_depth_map.reserve(num_vertices);
  }
  void start_vertex(vert_descrip_type vertex, const unigraph_type& unigraph) {
    vertex_to_recur_depth_map[vertex] = 0L;
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
    vertex_to_recur_depth_map[target_vert] = source_vert_depth + 1L;
  }
  void reset_to_init(vert_vec_type& vert_vec_ref) {
    ptr_final_dfs_deque->clear();
    vertex_to_recur_depth_map.clear();
    for (const vert_descrip_type& vertex : vert_vec_ref) {
      vertex_to_recur_depth_map[vertex] = -1L;
    }
  }
  const final_dfs_deque_result_type& get_result_deque() {
    return *ptr_final_dfs_deque;
  }
};

template <class Recorder>
typename std::enable_if_t<is_dfs_recorder<Recorder>::value, void>
seq_first_dfs_on_non_tree_edges(const long vertex_idx,
                                const vert_deque_type& vert_seq_in_dfs_seq,
                                const dfs_seq_with_map_type& dfs_seq_with_map,
                                unigraph_type& unigraph,
                                Recorder& recorder_inst,
                                long& recursion_lvl) {
  vert_visited_map_type vertices_idx_to_visit_data_map =
      boost::get(&custom_vertex_props_struct::visited, unigraph);
  vert_pop_out_idx_map_type vert_pop_out_idx_map =
      boost::get(&custom_vertex_props_struct::seq_pop_out_idx, unigraph);
  vert_descrip_type curr_vert = vert_seq_in_dfs_seq[vertex_idx];
  vertices_idx_to_visit_data_map[curr_vert] = true;
  ++recursion_lvl;
  recorder_inst.record_vert(curr_vert, recursion_lvl);
  long next_vert_idx =
      dfs_seq_with_map.vert_to_dfs_seq_idx_map_ptr->find(curr_vert)->second + 1;
  if (next_vert_idx < static_cast<long>(dfs_seq_with_map.dfs_seq_ptr->size())) {
    vert_descrip_type next_vert =
        dfs_seq_with_map.dfs_seq_ptr->at(next_vert_idx).front();
    while (vert_pop_out_idx_map[next_vert] <= vert_pop_out_idx_map[curr_vert]) {
      if (!vertices_idx_to_visit_data_map[next_vert]) {
        seq_first_dfs(next_vert_idx, dfs_seq_with_map, unigraph, recorder_inst,
                      recursion_lvl);
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
          unigraph, recorder_inst, recursion_lvl);
    }
  }

  --recursion_lvl;
}

template <class Recorder>
typename std::enable_if_t<is_dfs_recorder<Recorder>::value, void> seq_first_dfs(
    const long vertex_idx,
    const dfs_seq_with_map_type& dfs_seq_with_map,
    unigraph_type& unigraph,
    Recorder& recorder_inst,
    long& recursion_lvl) {
  vert_visited_map_type vertices_idx_to_visit_data_map =
      boost::get(&custom_vertex_props_struct::visited, unigraph);
  vert_pop_out_idx_map_type vert_pop_out_idx_map =
      boost::get(&custom_vertex_props_struct::seq_pop_out_idx, unigraph);
  vert_descrip_type curr_vert =
      dfs_seq_with_map.dfs_seq_ptr->at(vertex_idx).front();
  vertices_idx_to_visit_data_map[curr_vert] = true;
  ++recursion_lvl;
  recorder_inst.record_vert(curr_vert, recursion_lvl);
  long next_vert_idx = vertex_idx + 1;
  if (next_vert_idx < static_cast<long>(dfs_seq_with_map.dfs_seq_ptr->size())) {
    vert_descrip_type next_vert =
        dfs_seq_with_map.dfs_seq_ptr->at(next_vert_idx).front();
    while (vert_pop_out_idx_map[next_vert] <= vert_pop_out_idx_map[curr_vert]) {
      if (!vertices_idx_to_visit_data_map[next_vert]) {
        seq_first_dfs(next_vert_idx, dfs_seq_with_map, unigraph, recorder_inst,
                      recursion_lvl);
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
          unigraph, recorder_inst, recursion_lvl);
    }
  }

  --recursion_lvl;
}

void print_dfs_search_result(
    const final_dfs_deque_result_type& dfs_final_vert_seq,
    const vert_name_map_type& vertices_idx_to_name_map);

bool check_if_search_results_are_equal(
    const final_dfs_deque_result_type& final_deque_expected,
    const final_dfs_deque_result_type& final_deque_actual,
    const vert_name_map_type& vertices_idx_to_name_map);

dfs_seq_with_map_type dfs_seq_cons(
    const unigraph_with_vert_vec_type& graph_with_vert_vec,
    vert_deque_type& deque_of_in_deg_zero_verts,
    long& seq_idx);
