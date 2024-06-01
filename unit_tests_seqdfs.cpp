/*
 * All test cases for testing SeqDFS technique reside here.
 */
#include <cstddef>
#include <memory>
#define BOOST_TEST_MODULE seqdfs_all_tests
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>
#include "seqdfs_vs_boost_dfs_main.hpp"

constexpr int NUM_VERTS_TEST_OK_PAPER_GRAPH = 2;
namespace boost_utest_data = boost::unit_test::data;

class test_research_paper_graph_gen_fixture {
 private:
  unigraph_with_vert_vec_type test_graph_with_vec;
  std::shared_ptr<unigraph_with_vert_vec_type> ptr_to_unigraph_with_vert_vec;

 public:
  test_research_paper_graph_gen_fixture() {
    const std::size_t graph_num_vertices = 12;
    test_graph_with_vec = {};
    vert_vec_type vertex_vector(graph_num_vertices);
    unigraph_type test_graph(graph_num_vertices);
    test_graph_with_vec.unigraph_ptr =
        std::make_shared<unigraph_type>(test_graph);
    test_graph_with_vec.vert_vec_ptr =
        std::make_shared<vert_vec_type>(vertex_vector);
    ptr_to_unigraph_with_vert_vec =
        std::make_shared<unigraph_with_vert_vec_type>(test_graph_with_vec);

    for (long vertex_num = 0;
         vertex_num < static_cast<long>(graph_num_vertices); ++vertex_num) {
      custom_vertex_props_struct vertex = {0, false, vertex_num,
                                           color_template::white()};
      ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(vertex_num) =
          add_vertex(vertex, *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    }

    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(0),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(1),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(0),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(2),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(1),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(3),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(1),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(4),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(2),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(3),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(2),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(5),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(2),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(6),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(3),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(8),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(5),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(2),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(7),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(2),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(7),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(8),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(8),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(2),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(8),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(9),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(9),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(1),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(10),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(8),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(10),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(11),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
    boost::add_edge(ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(11),
                    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->at(9),
                    *(ptr_to_unigraph_with_vert_vec->unigraph_ptr));
  }
  ~test_research_paper_graph_gen_fixture() {
    ptr_to_unigraph_with_vert_vec->unigraph_ptr->clear();
    ptr_to_unigraph_with_vert_vec->vert_vec_ptr->clear();
  }
  unigraph_with_vert_vec_type get_constructed_graph() {
    return *ptr_to_unigraph_with_vert_vec;
  }
};

BOOST_DATA_TEST_CASE_F(test_research_paper_graph_gen_fixture,
                       test_research_paper_graph,
                       boost_utest_data::xrange(NUM_VERTS_TEST_OK_PAPER_GRAPH),
                       vertex_idx) {
  vert_name_map_type vertices_idx_to_name_map;
  vert_visited_map_type vertices_idx_to_visit_data_map;
  unigraph_with_vert_vec_type test_graph_with_vert_vec = {};
  final_dfs_deque_result_type dfs_final_vert_seq_seqdfs = {};
  final_dfs_deque_result_type dfs_final_vert_seq_boost_dfs = {};
  vert_deque_type deque_of_in_deg_zero_verts = {};
  dfs_seq_with_map_type dfs_constructed_seq_with_map = {};
  color_map_type graph_color_map = {};
  long seq_idx = 0;

  test_graph_with_vert_vec = get_constructed_graph();
  rec_in_deque_dfs_visit_class record_in_deque_dfs_visitor(
      test_graph_with_vert_vec.vert_vec_ptr->size());

  vertices_idx_to_name_map =
      boost::get(&custom_vertex_props_struct::vert_name,
                 *(test_graph_with_vert_vec.unigraph_ptr));
  vertices_idx_to_visit_data_map =
      boost::get(&custom_vertex_props_struct::visited,
                 *(test_graph_with_vert_vec.unigraph_ptr));
  graph_color_map = boost::get(&custom_vertex_props_struct::vertex_color,
                               *(test_graph_with_vert_vec.unigraph_ptr));

  std::cout << "\n\nReordering graph in test_research_paper_graph test suite "
               "using SeqDFS technique...\n";
  dfs_constructed_seq_with_map = dfs_seq_cons(
      test_graph_with_vert_vec, deque_of_in_deg_zero_verts, seq_idx);

  for (const vert_descrip_type& vertex :
       *(test_graph_with_vert_vec.vert_vec_ptr)) {
    vertices_idx_to_visit_data_map[vertex] = false;
  }
  long recursion_lvl = -1;

  std::cout << "Test based on research paper graph now testing SeqDFS starting "
               "with vertex \""
            << vertices_idx_to_name_map[deque_of_in_deg_zero_verts[vertex_idx]]
            << "\" in the graph...\n";
  seq_first_dfs(dfs_constructed_seq_with_map.vert_to_dfs_seq_idx_map_ptr
                    ->find(deque_of_in_deg_zero_verts[vertex_idx])
                    ->second,
                dfs_constructed_seq_with_map,
                *(test_graph_with_vert_vec.unigraph_ptr),
                dfs_final_vert_seq_seqdfs, recursion_lvl);
  boost::depth_first_visit(*(test_graph_with_vert_vec.unigraph_ptr),
                           deque_of_in_deg_zero_verts[vertex_idx],
                           record_in_deque_dfs_visitor, graph_color_map);
  dfs_final_vert_seq_boost_dfs = record_in_deque_dfs_visitor.get_result_deque();

  BOOST_TEST(check_if_search_results_are_equal(dfs_final_vert_seq_boost_dfs,
                                               dfs_final_vert_seq_seqdfs,
                                               vertices_idx_to_name_map));
  std::cout << "\n\n";
}
