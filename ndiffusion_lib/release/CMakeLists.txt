INCLUDE_DIRECTORIES (
  ${NDIFFUSION_SOURCE_DIR}/include
  ${IMGMAN_LIB_INCLUDE}  
)

ADD_LIBRARY (ndiffusion SHARED
  ${NDIFFUSION_SOURCE_DIR}/source/ndiffusion/ndiffusion.cpp
)