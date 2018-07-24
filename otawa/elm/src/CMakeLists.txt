
# compilation options
include_directories("../include")
include_directories(".")
if(LIBXML2_FOUND)
	include_directories(${LIBXML2_INCLUDE_DIR})
endif()
if(LIBXSLT_FOUND)
	include_directories(${LIBXSLT_INCLUDE_DIR})
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g3 -Wall")


# source list
set(LIBELM_LA_SOURCES
	"concepts.h"
	"doc.h"
	"alloc_BlockAllocator.cpp"
	"alloc_BlockAllocatorWithGC.cpp"
	"alloc_DefaultAllocator.cpp"
	"alloc_SimpleGC.cpp"
	"alloc_GroupedGC.cpp"
	"alloc_StackAllocator.cpp"
	"avl_GenTree.cpp"
	"avl_Tree.cpp"
	"block_DynBlock.cpp"
	"checksum_Fletcher.cpp"
	"checksum_MD5.cpp"
	"data_ArrayList.cpp"
	"data_BiDiList.cpp"
	"data_List.cpp"
	"data_ListQueue.cpp"
	"data_SortedList.cpp"
	"data_Table.cpp"
	"data_Vector.cpp"
	"debug.cpp"
	"genstruct.h"
	"genstruct_AssocList.cpp"
	"genstruct_DLList.cpp"
	"genstruct_FragTable.cpp"
	"genstruct_HashTable.cpp"
	"genstruct_DAGNode.cpp"
	"genstruct_SortedBinMap.cpp"
	"genstruct_SortedBinTree.cpp"
	"genstruct_Tree.cpp"
	"genstruct_VectorQueue.cpp"
	"genstruct_Table.cpp"
	"genstruct_Tree.cpp"
	"imm_list.cpp"
	"inhstruct_BinTree.cpp"
	"inhstruct_SortedBinTree.cpp"
	"inhstruct_DLList.cpp"
	"inhstruct_SLList.cpp"
	"inhstruct_Tree.cpp"
	"ini.cpp"
	"int.cpp"
	"io_BlockInStream.cpp"
	"io_BlockOutStream.cpp"
	"io_BufferedInStream.cpp"
	"io_BufferedOutStream.cpp"
	"io_InFileStream.cpp"
	"io_Input.cpp"
	"io_InStream.cpp"
	"io_IOException.cpp"
	"io_Monitor.cpp"
	"io_OutFileStream.cpp"
	"io_Output.cpp"
	"io_OutStream.cpp"
	"io_RandomAccessStream.cpp"
	"io_StreamPipe.cpp"
	"io_TeeOutStream.cpp"
	"io_UnixInStream.cpp"
	"io_UnixOutStream.cpp"
	"io_WinInStream.cpp"
	"io_WinOutStream.cpp"
	"Iterator.cpp"
	"json.cpp"
	"json_Parser.cpp"
	"log_Log.cpp"
	"option_Option.cpp"
	"option_BoolOption.cpp"
	"option_ActionOption.cpp"
	"option_EnumOption.cpp"
	"option_IntOption.cpp"
	"option_ListOption.cpp"
	"option_Manager.cpp"
	"option_StandardOption.cpp"
	"option_StringList.cpp"
	"option_StringOption.cpp"
	"option_SwitchOption.cpp"
	"option_ValueOption.cpp"
	"serial2_serial.cpp"
	"serial2_TextSerializer.cpp"
	"stree_Tree.cpp"
	"string_AutoString.cpp"
	"string_Char.cpp"
	"string_String.cpp"
	"string_StringBuffer.cpp"
	"string_utf8.cpp"
	"string_utf16.cpp"
	"system_File.cpp"
	"system_FileItem.cpp"
	"system_Directory.cpp"
	"system_Path.cpp"
	"system_Plugin.cpp"
	"system_Plugger.cpp"
	"system_Process.cpp"
	"system_ProcessBuilder.cpp"
	"system_StopWatch.cpp"
	"system_System.cpp"
	"system_SystemException.cpp"
	"system_SystemIO.cpp"
	"type_info.cpp"
	"util_BitVector.cpp"
	"util_Cleaner.cpp"
	"util_Comparator.cpp"
	"util_delegate.cpp"
	"util_Equiv.cpp"
	"util_ErrorHandler.cpp"
	"util_Formatter.cpp"
	"util_HashKey.cpp"
	"util_Initializer.cpp"
	"util_MessageException.cpp"
	"util_Option.cpp"
	"util_Pair.cpp"
	"util_Ref.cpp"
	"util_strong_type.cpp"
	"util_test.cpp"
	"util_VarArg.cpp"
	"util_Version.cpp"
	"util_WAHVector.cpp"
	"utility.cpp"
	)


# optional crash handler
if(GDB_PATH AND UNIX AND NOT WIN32 AND NOT WIN64)
	# be careful: the order of these sources matters for static object initialization
	list(APPEND LIBELM_LA_SOURCES "debug_GDBCrashHandler.cpp")
endif()
list(APPEND LIBELM_LA_SOURCES "debug_CrashHandler.cpp")


# optional socket
if(CMAKE_THREAD_LIBS_INIT OR WIN32 OR WIN64 OR CMAKE_USE_PTHREADS_INIT)
	list(APPEND LIBELM_LA_SOURCES "system_Thread.cpp" 	"sys_JobScheduler.cpp")
endif()
if(HAS_SOCKET)
	list(APPEND LIBELM_LA_SOURCES  "net_ClientSocket.cpp" "net_ServerSocket.cpp")
endif()


# optional xom module
if(LIBXML2_FOUND)
	list(APPEND LIBELM_LA_SOURCES
		"serial2_XOMUnserializer.cpp"
		"serial2_XOMSerializer.cpp"
		"xom_Attribute.cpp"
		"xom_Builder.cpp"
		"xom_Comment.cpp"
		"xom_Document.cpp"
		"xom_Element.cpp"
		"xom_Elements.cpp"
		"xom_Exception.cpp"
		"xom_Node.cpp"
		"xom_NodeFactory.cpp"
		"xom_ParentNode.cpp"
		"xom_Serializer.cpp"
		"xom_String.cpp"
		"xom_Text.cpp"
		"xom_XIncluder.cpp"
		"xom_macros.h")
endif()


# optional xom XSLT support
if(LIBXSLT_FOUND)
	list(APPEND LIBELM_LA_SOURCES "xom_XSLTransform.cpp")
endif()


# system IO selection
if(MINGW_LINUX OR WIN32)
	list(REMOVE_ITEM LIBELM_LA_SOURCES io_UnixInStream.cpp)
	list(REMOVE_ITEM LIBELM_LA_SOURCES io_UnixOutStream.cpp)
elseif(UNIX OR APPLE)
	list(REMOVE_ITEM LIBELM_LA_SOURCES io_WinInStream.cpp)
	list(REMOVE_ITEM LIBELM_LA_SOURCES io_WinOutStream.cpp)
endif()


# library definition
set(CMAKE_MACOSX_RPATH true)
add_library(elm SHARED ${LIBELM_LA_SOURCES})
if(LIBXML2_FOUND)
	#if(UNIX)
		target_link_libraries(elm ${LIBXML2_LIBRARIES})
	#elseif(WIN32)
	#	target_link_libraries(elm ${LIBXML2})
	#endif(UNIX)
endif()
if(LIBXSLT_FOUND)
	#if(UNIX)
		target_link_libraries(elm ${LIBXSLT_LIBRARIES})
	#elseif(WIN32)
	#	target_link_libraries(elm ${LIBXSLT})
	#endif(UNIX)
endif()
if(CMAKE_THREAD_LIBS_INIT)
	message(STATUS "link with ${CMAKE_THREAD_LIBS_INIT}")
	target_link_libraries(elm ${CMAKE_THREAD_LIBS_INIT})
endif()
if(UNIX)
	target_link_libraries(elm "-ldl")
endif()

# installation
if(INSTALL_LIB)
    install(TARGETS elm RUNTIME DESTINATION bin LIBRARY DESTINATION lib)
endif()
