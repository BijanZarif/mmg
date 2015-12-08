###############################################################################
#####
#####         C Tests
#####
###############################################################################

ADD_EXECUTABLE(libmmg3d_example0_a
  ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example0/example0_a/main.c)

ADD_EXECUTABLE(libmmg3d_example0_b
  ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example0/example0_b/main.c)

ADD_EXECUTABLE(libmmg3d_example1
  ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example1/main.c)

ADD_EXECUTABLE(libmmg3d_example2
  ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example2/main.c)

ADD_EXECUTABLE(libmmg3d_example3
  ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example3/mmg3d.c)

ADD_EXECUTABLE(libmmg3d_example4
  ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example4/main.c)

 IF ( WIN32 AND ((NOT MINGW) AND USE_SCOTCH) )
    my_add_link_flags(libmmg3d_example0_a "/SAFESEH:NO")
    my_add_link_flags(libmmg3d_example0_b "/SAFESEH:NO")
    my_add_link_flags(libmmg3d_example1 "/SAFESEH:NO")
    my_add_link_flags(libmmg3d_example2 "/SAFESEH:NO")
    my_add_link_flags(libmmg3d_example3 "/SAFESEH:NO")
    my_add_link_flags(libmmg3d_example4 "/SAFESEH:NO")
 ENDIF ( )

IF ( LIBMMG3D_STATIC )

  TARGET_LINK_LIBRARIES(libmmg3d_example0_a ${PROJECT_NAME}3d_a)
  TARGET_LINK_LIBRARIES(libmmg3d_example0_b ${PROJECT_NAME}3d_a)
  TARGET_LINK_LIBRARIES(libmmg3d_example1   ${PROJECT_NAME}3d_a)
  TARGET_LINK_LIBRARIES(libmmg3d_example2   ${PROJECT_NAME}3d_a)
  TARGET_LINK_LIBRARIES(libmmg3d_example3   ${PROJECT_NAME}3d_a)
  TARGET_LINK_LIBRARIES(libmmg3d_example4   ${PROJECT_NAME}3d_a)

ELSEIF ( LIBMMG3D_SHARED )

  TARGET_LINK_LIBRARIES(libmmg3d_example0_a ${PROJECT_NAME}3d_so)
  TARGET_LINK_LIBRARIES(libmmg3d_example0_b ${PROJECT_NAME}3d_so)
  TARGET_LINK_LIBRARIES(libmmg3d_example1   ${PROJECT_NAME}3d_so)
  TARGET_LINK_LIBRARIES(libmmg3d_example2   ${PROJECT_NAME}3d_so)
  TARGET_LINK_LIBRARIES(libmmg3d_example3   ${PROJECT_NAME}3d_so)
  TARGET_LINK_LIBRARIES(libmmg3d_example4   ${PROJECT_NAME}3d_so)

ELSE ()
  MESSAGE(WARNING "You must activate the compilation of the static or"
    " shared ${PROJECT_NAME} library to compile this tests." )
ENDIF ()

INSTALL(TARGETS libmmg3d_example0_a RUNTIME DESTINATION bin )
INSTALL(TARGETS libmmg3d_example0_b RUNTIME DESTINATION bin )
INSTALL(TARGETS libmmg3d_example1   RUNTIME DESTINATION bin )
INSTALL(TARGETS libmmg3d_example2   RUNTIME DESTINATION bin )
INSTALL(TARGETS libmmg3d_example3   RUNTIME DESTINATION bin )
INSTALL(TARGETS libmmg3d_example4   RUNTIME DESTINATION bin )

###############################################################################
#####
#####         Fortran Tests
#####
###############################################################################
IF (CMAKE_Fortran_COMPILER)
  ENABLE_LANGUAGE (Fortran)

  ADD_EXECUTABLE(libmmg3d_fortran_a
    ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example0_fortran/example0_a/main.F90)

  ADD_EXECUTABLE(libmmg3d_fortran_b
    ${CMAKE_SOURCE_DIR}/libexamples/mmg3d/example0_fortran/example0_b/main.F90)

  IF ( WIN32 AND ((NOT MINGW) AND USE_SCOTCH) )
    my_add_link_flags(libmmg3d_fortran_a "/SAFESEH:NO")
    my_add_link_flags(libmmg3d_fortran_b "/SAFESEH:NO")
  ENDIF ( )

  IF ( LIBMMG3D_STATIC )

    TARGET_LINK_LIBRARIES(libmmg3d_fortran_a  ${PROJECT_NAME}3d_a)
    TARGET_LINK_LIBRARIES(libmmg3d_fortran_b  ${PROJECT_NAME}3d_a)

  ELSEIF ( LIBMMG3D_SHARED )

    TARGET_LINK_LIBRARIES(libmmg3d_fortran_a  ${PROJECT_NAME}3d_so)
    TARGET_LINK_LIBRARIES(libmmg3d_fortran_b  ${PROJECT_NAME}3d_so)

  ELSE ()
    MESSAGE(WARNING "You must activate the compilation of the static or"
      " shared ${PROJECT_NAME} library to compile this tests." )
  ENDIF ()


  INSTALL(TARGETS libmmg3d_fortran_b  RUNTIME DESTINATION bin )
  INSTALL(TARGETS libmmg3d_fortran_a  RUNTIME DESTINATION bin )

ENDIF (CMAKE_Fortran_COMPILER)
