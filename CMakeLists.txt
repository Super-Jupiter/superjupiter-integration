cmake_minimum_required(VERSION 3.16)
project(SuperJupiterIntegration VERSION 0.1.0 LANGUAGES CXX)

set(_superjupiter_integration_top_level OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(_superjupiter_integration_top_level ON)
endif()
option(SUPERJUPITER_INTEGRATION_BUILD_TESTS "Build Integration tests" ${_superjupiter_integration_top_level})
option(SUPERJUPITER_INTEGRATION_BUILD_EXAMPLES "Build Integration host example" ${_superjupiter_integration_top_level})
option(SUPERJUPITER_INTEGRATION_INSTALL "Generate Integration install rules" ${_superjupiter_integration_top_level})

add_library(SuperJupiterIntegration INTERFACE)
add_library(SuperJupiter::Integration ALIAS SuperJupiterIntegration)
set_target_properties(SuperJupiterIntegration PROPERTIES EXPORT_NAME Integration)
target_include_directories(SuperJupiterIntegration INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)
target_compile_features(SuperJupiterIntegration INTERFACE cxx_std_11)

if(SUPERJUPITER_INTEGRATION_BUILD_TESTS)
  enable_testing()
  add_executable(superjupiter-integration-tests tests/test.cpp)
  target_link_libraries(superjupiter-integration-tests PRIVATE SuperJupiter::Integration)
  add_test(NAME superjupiter-integration-tests COMMAND superjupiter-integration-tests)
  add_executable(superjupiter-integration-namespace-test tests/namespace_isolation.cpp)
  target_link_libraries(superjupiter-integration-namespace-test PRIVATE SuperJupiter::Integration)
  add_test(NAME superjupiter-integration-namespace-test COMMAND superjupiter-integration-namespace-test)
endif()
if(SUPERJUPITER_INTEGRATION_BUILD_EXAMPLES)
  add_executable(superjupiter-integration-replay tools/replay.cpp)
  target_link_libraries(superjupiter-integration-replay PRIVATE SuperJupiter::Integration)
endif()
if(SUPERJUPITER_INTEGRATION_INSTALL)
  include(CMakePackageConfigHelpers)
  set(_superjupiter_integration_package_dir lib/cmake/SuperJupiterIntegration)
  install(TARGETS SuperJupiterIntegration EXPORT SuperJupiterIntegrationTargets)
  install(FILES src/SuperJupiterIntegration.h DESTINATION include)
  install(EXPORT SuperJupiterIntegrationTargets NAMESPACE SuperJupiter::
    DESTINATION ${_superjupiter_integration_package_dir})
  configure_package_config_file(cmake/SuperJupiterIntegrationConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterIntegrationConfig.cmake
    INSTALL_DESTINATION ${_superjupiter_integration_package_dir})
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterIntegrationConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion ARCH_INDEPENDENT)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterIntegrationConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterIntegrationConfigVersion.cmake
    DESTINATION ${_superjupiter_integration_package_dir})
  install(FILES LICENSE DESTINATION share/licenses/SuperJupiterIntegration)
endif()
