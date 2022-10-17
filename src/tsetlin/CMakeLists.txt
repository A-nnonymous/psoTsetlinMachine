cmake_minimum_required(VERSION 3.22.a)
project(psoTsetlinMachine VERSION 0.1.0)

aux_source_directory(. TM_LIB_SRCS)
add_library(tmLib ${TM_LIB_SRCS})