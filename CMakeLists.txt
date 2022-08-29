
cmake_minimum_required(VERSION 3.5)
project(neurolab VERSION 0.0.1 DESCRIPTION "Library to simulate stochastic differential equations for neural computation applications.")

FIND_PACKAGE( Boost REQUIRED )
FIND_PACKAGE( Gnuplot )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} h)

set(GNUPLOT_EXECUTABLE "\"/usr/bin/gnuplot \"")
include(GNUInstallDirs)

add_library(neurolab SHARED
    src/conditionalestimator.cxx
    src/dependanceestimator.cxx
    src/differentiable.cxx
    src/display.cxx
    src/estimator.cxx
    src/eventmultiplexer.cxx
    src/eventplayer.cxx
    src/function.cxx
    src/ifneuron.cxx
    src/intervalestimator.cxx
    src/matrix.cxx
    src/mlneuron.cxx
    src/neuron.cxx
    src/noises.cxx
    src/parametric.cxx
    src/physical.cxx
    src/processes.cxx
    src/processestimator.cxx
    src/scalarestimator.cxx
    src/seriesestimator.cxx
    src/spikeestimator.cxx
    src/stochastic.cxx
    src/synapse.cxx
    src/thetaneuron.cxx
    src/timedependent.cxx
    src/wiener.cxx
)

set_target_properties(neurolab PROPERTIES VERSION ${PROJECT_VERSION})
configure_file(neurolab.pc.in neurolab.pc @ONLY)
target_include_directories(neurolab PRIVATE h)
target_include_directories(neurolab PRIVATE src)
target_compile_definitions(neurolab PRIVATE GNUPLOT_EXECUTABLE=${GNUPLOT_EXECUTABLE})
target_compile_definitions(neurolab PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)
install(TARGETS neurolab
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/neurolab)
install(FILES ${CMAKE_BINARY_DIR}/neurolab.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
install(DIRECTORY neurolab DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
