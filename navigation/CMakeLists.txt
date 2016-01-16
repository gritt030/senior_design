add_library(navigate  virtualenvironment.cpp
                      frontiermap.cpp
                      ./../occupancygrid/grid/grid.cpp
                      ./../pngwriter/png_writer.cpp
                      )

add_library(navigation_map  navigationmap/navigationmap.cpp
                            navigationmap/grid/opengrid.cpp
                            navigationmap/grid/frontiergrid.cpp
                            )
