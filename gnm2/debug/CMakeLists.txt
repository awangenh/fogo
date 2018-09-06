ADD_DEFINITIONS(
  -D_DEBUG_
)

INCLUDE_DIRECTORIES (
  ${GNM2_SOURCE_DIR}/include
#  ${GNM2_SOURCE_DIR}/include/gnm
  ${IMGMAN_LIB_INCLUDE}
#  ${REGIONPROPS_LIB_INCLUDE}
  ${LABELING_LIB_INCLUDE}  
)

ADD_EXECUTABLE (iGNM.exe 
  ${GNM2_SOURCE_DIR}/source/main.cpp
  ${GNM2_SOURCE_DIR}/source/gnm/algorithm.cpp
  ${GNM2_SOURCE_DIR}/source/gnm/controller.cpp
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_date.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_dbl.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_end.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_file.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_int.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_lit.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_rem.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_rex.c
  ${GNM2_SOURCE_DIR}/source/argtable2/arg_str.c
  ${GNM2_SOURCE_DIR}/source/argtable2/argtable2.c
  ${GNM2_SOURCE_DIR}/source/argtable2/getopt1.c
  ${GNM2_SOURCE_DIR}/source/argtable2/getopt.c
)

LINK_DIRECTORIES (
  ${IMGMAN_LIB_DIR}
#  ${REGIONPROPS_LIB_DIR}
  ${LABELING_LIB_DIR}
)

TARGET_LINK_LIBRARIES (iGNM.exe
  ${IMGMAN_LIB_DIR}/lib${IMGMAN_LIB}.so
#  ${REGIONPROPS_LIB_DIR}/lib${REGIONPROPS_LIB}.a
  ${LABELING_LIB_DIR}/lib${LABELING_LIB}.a
)