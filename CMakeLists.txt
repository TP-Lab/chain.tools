cmake_minimum_required(VERSION 3.5)
project(chain.tools VERSION 1.0.0)

find_package(eosio.cdt)

add_contract(chain.tools chain.tools ${CMAKE_CURRENT_SOURCE_DIR}/src/chian.tools.cpp)
target_include_directories(chain.tools
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../eosio.system/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../eosio.token/include)

set_target_properties(chain.tools
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

#target_ricardian_directory( chain.tools ${CMAKE_CURRENT_SOURCE_DIR}/ricardian )