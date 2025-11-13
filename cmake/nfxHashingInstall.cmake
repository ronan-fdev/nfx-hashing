#==============================================================================
# nfx-hashing - Library installation
#==============================================================================

#----------------------------------------------
# Installation condition check
#----------------------------------------------

if(NOT NFX_HASHING_INSTALL_PROJECT)
	message(STATUS "Installation disabled, skipping...")
	return()
endif()

#----------------------------------------------
# Installation paths configuration
#----------------------------------------------

include(GNUInstallDirs)

message(STATUS "System installation paths:")
message(STATUS "  Headers      : ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "  Library      : ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
message(STATUS "  Binaries     : ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
message(STATUS "  CMake configs: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/nfx-hashing")
message(STATUS "  Documentation: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}")

#----------------------------------------------
# Install headers
#----------------------------------------------

install(
	DIRECTORY "${NFX_HASHING_INCLUDE_DIR}/"
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	COMPONENT Development
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

#----------------------------------------------
# Install library targets
#----------------------------------------------

set(INSTALL_TARGETS)

# Header-only interface library
list(APPEND INSTALL_TARGETS ${PROJECT_NAME})

if(INSTALL_TARGETS)
	install(
		TARGETS ${INSTALL_TARGETS}
		EXPORT nfx-hashing-targets
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
			COMPONENT Development
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			COMPONENT Runtime
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			COMPONENT Runtime
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)
endif()

#----------------------------------------------
# Install CMake config files
#----------------------------------------------

install(
	EXPORT nfx-hashing-targets
	FILE nfx-hashing-targets.cmake
	NAMESPACE nfx-hashing::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nfx-hashing
	COMPONENT Development
)

# Install separate target files for each configuration (multi-config generators)
if(CMAKE_CONFIGURATION_TYPES)
	foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
		install(
			EXPORT nfx-hashing-targets
			FILE nfx-hashing-targets-${CONFIG}.cmake
			NAMESPACE nfx-hashing::
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nfx-hashing
			CONFIGURATIONS ${CONFIG}
			COMPONENT Development
		)
	endforeach()
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/nfx-hashing-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/nfx-hashing-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/nfx-hashing-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nfx-hashing
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/nfx-hashing-config.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/nfx-hashing-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nfx-hashing
	COMPONENT Development
)

#----------------------------------------------
# Install license files
#----------------------------------------------

install(
	FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt"
	DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
)

install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/licenses/"
	DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses"
	FILES_MATCHING PATTERN "LICENSE.txt-*"
)

#----------------------------------------------
# Install documentation
#----------------------------------------------

if(NFX_HASHING_BUILD_DOCUMENTATION)
	install(
		DIRECTORY "${CMAKE_BINARY_DIR}/doc/html"
		DESTINATION ${CMAKE_INSTALL_DOCDIR}
		OPTIONAL
		COMPONENT Documentation
	)
	
	if(WIN32)
		# Install Windows .cmd batch file
		install(
			FILES "${CMAKE_BINARY_DIR}/doc/index.html.cmd"
			DESTINATION ${CMAKE_INSTALL_DOCDIR}
			OPTIONAL
			COMPONENT Documentation
		)
	else()
		# Install Unix symlink
		install(
			FILES "${CMAKE_BINARY_DIR}/doc/index.html"
			DESTINATION ${CMAKE_INSTALL_DOCDIR}
			OPTIONAL
			COMPONENT Documentation
		)
	endif()
endif()

message(STATUS "Installation configured for targets: ${INSTALL_TARGETS}")
