#######################################################
# @fn find_files_in_dir
# @brief Searching extensions [in_ext] contain \n
#  in directory [in_dir], result put into \n
#  variable [out_files].
#######################################################
function(find_files_in_dir in_dir in_ext out_files)
    # Scan all files in directory.
    if(${CMAKE_SYSTEM_NAME}  MATCHES "Linux")
        execute_process(COMMAND find ${in_dir} -type f
                RESULT_VARIABLE ret
                OUTPUT_VARIABLE dir_dirs)
        string(REPLACE "\n" ";" src_dirs_list ${dir_dirs})
        # set(src_dirs_list ${src_dirs_list} ${in_dir})
        # message(STATUS "<find_files_in_dir()-in_dir=${in_dir}")
        # message(STATUS "<find_files_in_dir()-in_ext=${in_ext}")
        # message(STATUS "<find_files_in_dir()-dir_dirs=${dir_dirs}")
        # message(STATUS "<find_files_in_dir()-src_dirs_list=${src_dirs_list}")

        # Filter files by rule.
        set(src_file )
        foreach(ext ${in_ext})
            # message(STATUS "<find_files_in_dir()>" "^.*\\.${ext}$")
            foreach(f ${src_dirs_list})
                if(${f} MATCHES "^.*\\.${ext}$")
                    set(src_file ${src_file} ${f})
                    # message("<find_files_in_dir()-#1> ${f}")
                else()
                    # message("<find_files_in_dir()-#2> ${f}")
                endif()
                # message("% ${f}")
            endforeach(f)
        endforeach(ext)
    elseif(${CMAKE_SYSTEM_NAME}  MATCHES "Windows")
        set(src_file )
        foreach(ext ${in_ext})
            file(GLOB_RECURSE src_dirs_list "${in_dir}/*.${ext}")
            set(src_file ${src_file} ${src_dirs_list})
        endforeach(ext)
        # message(STATUS  "<find_files_in_dir()-in_dir> ${in_dir}")
    endif(${CMAKE_SYSTEM_NAME}  MATCHES "Linux")
    # message(STATUS "<find_files_in_dir()-src_file=${src_file}")
    # message(STATUS "<find_files_in_dir()-out_files=${out_files}")
    set(${out_files} ${src_file} PARENT_SCOPE)
    # message(STATUS "<find_files_in_dir()-out_files=${out_files}")
endfunction(find_files_in_dir)

#######################################################
# @fn find_src_in_dir
# @brief Searching c/c++ source in director [in_dir], 
#  result output to [out_files]
#######################################################
function(find_src_in_dir in_dir out_files)
    find_files_in_dir(${in_dir} "cpp;c;cxx;" out_file_)
    # message(STATUS "<find_src_in_dir()-out_file_=${out_file_}")
    set(${out_files} ${out_file_} PARENT_SCOPE)
endfunction(find_src_in_dir)

#######################################################
# @fn find_header_in_dir
# @brief Searching c/c++ header in director [in_dir], 
#  result output to [out_files]
#######################################################
function(find_header_in_dir in_dir out_files)
    find_files_in_dir(${in_dir} "h;hpp" out_files_)
    # message(STATUS "<find_header_in_dir()-out_file_=${out_files_}")
    set(${out_files} ${out_files_} PARENT_SCOPE)
endfunction(find_header_in_dir)

#######################################################
# @fn find_sub_dir
# @brief Searching sub-director in [in_dir]
#######################################################
function(find_sub_dir in_dir out_dirs)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        # Scan all files in directory.
        execute_process(COMMAND find ${in_dir} -type d
                RESULT_VARIABLE ret
                OUTPUT_VARIABLE dir_dirs)
        string(REPLACE "\n" ";" sub_dirs_list ${dir_dirs})
    elseif(${CMAKE_SYSTEM_NAME}  MATCHES "Windows")
        file(GLOB children RELATIVE ${in_dir} ${in_dir}/*)
        set(dirlist "")
        foreach(child ${dir_dirs})
            if(IS_DIRECTORY ${in_dir}/${child})
                LIST(APPEND dirlist ${in_dir}/${child})
            endif()
        endforeach()
        # message(STATUS "<find_sub_dir()-dirlist=${dirlist}")
        # string(REPLACE "\n" ";" src_dirs_list ${dir_dirs})
    endif(${CMAKE_SYSTEM_NAME}  MATCHES "Linux")
    # message(STATUS "<find_sub_dir()-src_dirs_list=${src_dirs_list}")
    set(${out_dirs} ${sub_dirs_list} PARENT_SCOPE)
endfunction(find_sub_dir)
