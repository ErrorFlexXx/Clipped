cmake_minimum_required(VERSION 3.9)

project(Clipped)

# The following components of Clipped library are mandatory:
add_subdirectory(Utils)

# All other sublibraries can be used with an add_subdirectory and
# target_link_libraries call from a higher level cmake lists file.
