include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state(RESET)
set(CMAKE_CXX_STANDARD 17)

set(FILESYSTEM_HAVE_FS FALSE)

check_include_file_cxx("filesystem" FILESYSTEM_EXISTS)
check_include_file_cxx("experimental/filesystem" EXPERIMENTAL_FILESYSTEM_EXISTS)

if(FILESYSTEM_EXISTS)
    set(FILESYSTEM_HAVE_FS TRUE)
    set(FILESYSTEM_HEADER filesystem)
    set(FILESYSTEM_NAMESPACE std::filesystem)
elseif(EXPERIMENTAL_FILESYSTEM_EXISTS)
    if("experimental" IN_LIST Filesystem_FIND_COMPONENTS)
        set(FILESYSTEM_HAVE_FS   TRUE)
        set(FILESYSTEM_HEADER    experimental/filesystem)
        set(FILESYSTEM_NAMESPACE std::experimental::filesystem)
    endif()
endif()

if(NOT FILESYSTEM_HAVE_FS)
    if(Filesystem_FIND_REQUIRED)
        message(FATAL_ERROR "The used compiler does not have std::filesystem support")
    else()
        set(Filesystem_FOUND FALSE CACHE BOOL "" FORCE)
    endif()
    return()
endif()

string(CONFIGURE [[
    #include <@FILESYSTEM_HEADER@>

    int main() {
    auto cwd = @FILESYSTEM_NAMESPACE@::current_path();
    return cwd.string().size();
    }
    ]] code @ONLY)

check_cxx_source_compiles("${code}" FILESYSTEM_NO_LINK_NEEDED)
set(CMAKE_REQUIRED_LIBRARIES -lstdc++fs)
check_cxx_source_compiles("${code}" FILESYSTEM_STDCPPFS_NEEDED)
set(CMAKE_REQUIRED_LIBRARIES -lc++fs)
check_cxx_source_compiles("${code}" FILESYSTEM_CPPFS_NEEDED)
cmake_pop_check_state()

if(NOT FILESYSTEM_NO_LINK_NEEDED OR NOT FILESYSTEM_STDCPPFS_NEEDED OR NOT FILESYSTEM_CPPFS_NEEDED)
    if(Filesystem_FIND_REQUIRED)
        message(FATAL_ERROR "Cannot Compile Simple Program using std::filesystem")
    else()
        set(Filesystem_FOUND FALSE CACHE BOOL "" FORCE)
    endif()
    return()
endif()

add_library(std::filesystem INTERFACE IMPORTED GLOBAL)
target_compile_definitions(std::filesystem
    INTERFACE 
    STD_FS_IS_EXPERIMENTAL=$<NOT:$<BOOL:${FILESYSTEM_EXISTS}>>
)

if(FILESYSTEM_STDCPPFS_NEEDED)
    target_link_libraries(std::filesystem INTERFACE -lstdc++fs)
elseif(FILESYSTEM_CPPFS_NEEDED)
    target_link_libraries(std::filesystem INTERFACE -lc++fs)
endif()

set(Filesystem_FOUND TRUE CACHE BOOL "" FORCE)

