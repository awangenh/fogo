INCLUDE_DIRECTORIES (
  ${LABELING_LIB_SOURCE_DIR}/Include)

ADD_LIBRARY (labeling
  ${LABELING_LIB_SOURCE_DIR}/Source/Graph/GraphLabeling.cpp
  ${LABELING_LIB_SOURCE_DIR}/Source/Labeling/Region.cpp
  ${LABELING_LIB_SOURCE_DIR}/Source/Labeling/Labeling.cpp
  ${LABELING_LIB_SOURCE_DIR}/Source/SegFormat/SegFormatLabeling.cpp)