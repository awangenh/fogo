INCLUDE_DIRECTORIES (
  ${EDISON_LIB_SOURCE_DIR}/Source/edge 
  ${EDISON_LIB_SOURCE_DIR}/Source/prompt 
  ${EDISON_LIB_SOURCE_DIR}/Source/segm)

ADD_LIBRARY (edison 
  ${EDISON_LIB_SOURCE_DIR}/Source/edge/BgEdgeList.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/edge/BgEdgeDetect.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/edge/BgEdge.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/edge/BgGlobalFc.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/edge/BgImage.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/segm/ms.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/segm/RAList.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/segm/msSysPrompt.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/segm/msImageProcessor.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/segm/rlist.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/prompt/globalFnc.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/prompt/parser.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/prompt/main.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/prompt/edison.cpp
  ${EDISON_LIB_SOURCE_DIR}/Source/prompt/libppm.cpp)