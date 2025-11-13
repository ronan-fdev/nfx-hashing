#==============================================================================
# nfx-hashing - CMake Sources
#==============================================================================

#----------------------------------------------
# Conditional headers and sources
#----------------------------------------------

set(PUBLIC_HEADERS)

list(APPEND PUBLIC_HEADERS
	${NFX_HASHING_INCLUDE_DIR}/nfx/hashing/Algorithms.h
	${NFX_HASHING_INCLUDE_DIR}/nfx/hashing/Concepts.h
	${NFX_HASHING_INCLUDE_DIR}/nfx/hashing/Constants.h
	${NFX_HASHING_INCLUDE_DIR}/nfx/hashing/Hash.h
	${NFX_HASHING_INCLUDE_DIR}/nfx/hashing/Hasher.h

	${NFX_HASHING_INCLUDE_DIR}/nfx/detail/hashing/Algorithms.inl
	${NFX_HASHING_INCLUDE_DIR}/nfx/detail/hashing/Hash.inl
	${NFX_HASHING_INCLUDE_DIR}/nfx/detail/hashing/Hasher.inl

	${NFX_HASHING_INCLUDE_DIR}/nfx/Hashing.h
)

#----------------------------------------------
# Library definition
#----------------------------------------------

# --- Create header-only interface library ---
add_library(${PROJECT_NAME} INTERFACE)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#----------------------------------------------
# Target properties
#----------------------------------------------

# --- Configure interface library ---
target_include_directories(${PROJECT_NAME}
	INTERFACE
		$<BUILD_INTERFACE:${NFX_HASHING_INCLUDE_DIR}>
		$<INSTALL_INTERFACE:include>
)

# Set interface compile features for C++20
target_compile_features(${PROJECT_NAME}
	INTERFACE
		cxx_std_20
)
